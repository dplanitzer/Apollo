//
//  DispatchQueue.c
//  Apollo
//
//  Created by Dietmar Planitzer on 4/21/21.
//  Copyright © 2021 Dietmar Planitzer. All rights reserved.
//

#include "DispatchQueue.h"
#include "ConditionVariable.h"
#include "Heap.h"
#include "List.h"
#include "Lock.h"
#include "MonotonicClock.h"
#include "Semaphore.h"
#include "VirtualProcessorPool.h"
#include "VirtualProcessorScheduler.h"


enum ItemType {
    kItemType_Immediate = 0,    // Execute the item as soon as possible
    kItemType_OneShotTimer,     // Execute the item once on or after its deadline
    kItemType_RepeatingTimer,   // Execute the item on or after its deadline and then reschedule it for the next deadline
};


typedef struct _WorkItem {
    ListNode                    queue_entry;
    DispatchQueue_Closure       closure;
    Byte* _Nullable _Weak       context;
    Semaphore * _Nullable _Weak completion_sema;
    Bool                        is_owned_by_queue;      // item was created and is owned by the dispatch queue and thus is eligble to be moved to the work item cache
    AtomicBool                  is_being_dispatched;    // shared between all dispatch queues (set to true while the work item is in the process of being dispatched by a queue; false if no queue is using it)
    AtomicBool                  cancelled;              // shared between dispatch queue and queue user
    Int8                        type;
} WorkItem;


typedef struct _Timer {
    WorkItem        item;
    TimeInterval    deadline;           // Time when the timer closure should be executed
    TimeInterval    interval;
} Timer;


// A concurrency lane is a virtual processor and all associated resources. The
// resources are specific to this virtual processor and shall only be used in
// connection with this virtual processor. There's one concurrency lane per
// dispatch queue concurrency level.
typedef struct _ConcurrencyLane {
    VirtualProcessor* _Nullable  vp;     // The virtual processor assigned to this concurrency lane
} ConcurrencyLane;


#define MAX_ITEM_CACHE_COUNT    8
#define MAX_TIMER_CACHE_COUNT   8
typedef struct _DispatchQueue {
    List                item_queue;         // Queue of work items that should be executed as soon as possible
    List                timer_queue;        // Queue of items that should be executed on or after their deadline
    List                item_cache_queue;   // Cache of reusable work items
    List                timer_cache_queue;  // Cache of reusable timers
    Lock                lock;
    ConditionVariable   cond_var;
    Int8                maxConcurrency;         // maximum number of concurrency lanes we are allowed to allocate and use
    Int8                availableConcurrency;   // number of concurrency lanes we have acquired and are available for use
    Int8                qos;
    Int8                priority;
    Int8                item_cache_count;
    Int8                timer_cache_count;
    Int8                reserved[2];
    ConcurrencyLane     concurrency_lanes[1];   // up to 'maxConcurrency' concurrency lanes
} DispatchQueue;


////////////////////////////////////////////////////////////////////////////////
// MARK: -
// MARK: Work Items


static void WorkItem_Init(WorkItemRef _Nonnull pItem, enum ItemType type, DispatchQueue_Closure _Nonnull pClosure, Byte* _Nullable pContext, Bool isOwnedByQueue)
{
    ListNode_Init(&pItem->queue_entry);
    pItem->closure = pClosure;
    pItem->context = pContext;
    pItem->is_owned_by_queue = isOwnedByQueue;
    pItem->is_being_dispatched = false;
    pItem->cancelled = false;
    pItem->type = type;
}

// Creates a work item which will invoke the given closure. Note that work items
// are one-shot: they execute their closure and then the work item is destroyed.
static WorkItemRef _Nullable WorkItem_Create_Internal(DispatchQueue_Closure _Nonnull pClosure, Byte* _Nullable pContext, Bool isOwnedByQueue)
{
    WorkItem* pItem = (WorkItem*) kalloc(sizeof(WorkItem), HEAP_ALLOC_OPTION_CPU);
    
    if (pItem) {
        WorkItem_Init(pItem, kItemType_Immediate, pClosure, pContext, isOwnedByQueue);
    }
    return pItem;
}

// Creates a work item which will invoke the given closure. Note that work items
// are one-shot: they execute their closure and then the work item is destroyed.
// This is the creation method for parties that are external to the dispatch
// queue implementation.
WorkItemRef _Nullable WorkItem_Create(DispatchQueue_Closure _Nonnull pClosure, Byte* _Nullable pContext)
{
    return WorkItem_Create_Internal(pClosure, pContext, false);
}

static void WorkItem_Deinit(WorkItemRef _Nonnull pItem)
{
    ListNode_Deinit(&pItem->queue_entry);
    pItem->closure = NULL;
    pItem->context = NULL;
    pItem->completion_sema = NULL;
    // Leave is_owned_by_queue alone
    pItem->is_being_dispatched = false;
    pItem->cancelled = false;
}

// Deallocates the given work item.
void WorkItem_Destroy(WorkItemRef _Nullable pItem)
{
    if (pItem) {
        WorkItem_Deinit(pItem);
        kfree((Byte*) pItem);
    }
}

// Cancels the given work item. The work item is marked as cancelled but it is
// the responsibility of the work item closure to check the cancelled state and
// to act appropriately on it.
void WorkItem_Cancel(WorkItemRef _Nonnull pItem)
{
    pItem->cancelled = true;
}

// Returns true if the given work item is in cancelled state.
Bool WorkItem_IsCancelled(WorkItemRef _Nonnull pItem)
{
    return pItem->cancelled;
}


////////////////////////////////////////////////////////////////////////////////
// MARK: -
// MARK: Timers


static void _Nullable Timer_Init(TimerRef _Nonnull pTimer, TimeInterval deadline, TimeInterval interval, DispatchQueue_Closure _Nonnull pClosure, Byte* _Nullable pContext, Bool isOwnedByQueue)
{
    enum ItemType type = TimeInterval_Greater(interval, kTimeInterval_Zero) ? kItemType_RepeatingTimer : kItemType_OneShotTimer;

    WorkItem_Init((WorkItem*)pTimer, type, pClosure, pContext, isOwnedByQueue);
    pTimer->deadline = deadline;
    pTimer->interval = interval;
}

// Creates a new timer. The timer will fire on or after 'deadline'. If 'interval'
// is greater than 0 then the timer will repeat until cancelled.
static TimerRef _Nullable Timer_Create_Internal(TimeInterval deadline, TimeInterval interval, DispatchQueue_Closure _Nonnull pClosure, Byte* _Nullable pContext, Bool isOwnedByQueue)
{
    Timer* pTimer = (Timer*) kalloc(sizeof(Timer), HEAP_ALLOC_OPTION_CPU);
    
    if (pTimer) {
        Timer_Init(pTimer, deadline, interval, pClosure, pContext, isOwnedByQueue);
    }
    return pTimer;
}

// Creates a new timer. The timer will fire on or after 'deadline'. If 'interval'
// is greater than 0 then the timer will repeat until cancelled.
// This is the creation method for parties that are external to the dispatch
// queue implementation.
TimerRef _Nullable Timer_Create(TimeInterval deadline, TimeInterval interval, DispatchQueue_Closure _Nonnull pClosure, Byte* _Nullable pContext)
{
    return Timer_Create_Internal(deadline, interval, pClosure, pContext, false);
}

static inline void Timer_Deinit(TimerRef pTimer)
{
    WorkItem_Deinit((WorkItemRef) pTimer);
}

void Timer_Destroy(TimerRef _Nullable pTimer)
{
    if (pTimer) {
        Timer_Deinit(pTimer);
        kfree((Byte*) pTimer);
    }
}


////////////////////////////////////////////////////////////////////////////////
// MARK: -
// MARK: Dispatch Queue


// Called at kernel startup time in order to create all kernel queues. Aborts
// if creation of a queue fails.
void DispatchQueue_CreateKernelQueues(const SystemDescription* _Nonnull pSysDesc)
{
    SystemGlobals* pSysGlobals = SystemGlobals_Get();
    
    pSysGlobals->dispatch_queue_realtime = DispatchQueue_Create(4, DISPATCH_QOS_REALTIME, 0);
    assert(pSysGlobals->dispatch_queue_realtime != NULL);
    
    pSysGlobals->dispatch_queue_main = DispatchQueue_Create(1, DISPATCH_QOS_INTERACTIVE, 0);
    assert(pSysGlobals->dispatch_queue_main != NULL);
    
    pSysGlobals->dispatch_queue_utility = DispatchQueue_Create(4, DISPATCH_QOS_UTILITY, 0);
    assert(pSysGlobals->dispatch_queue_utility != NULL);
    
    pSysGlobals->dispatch_queue_background = DispatchQueue_Create(4, DISPATCH_QOS_BACKGROUND, 0);
    assert(pSysGlobals->dispatch_queue_background != NULL);
    
    pSysGlobals->dispatch_queue_idle = DispatchQueue_Create(2, DISPATCH_QOS_IDLE, 0);
    assert(pSysGlobals->dispatch_queue_idle != NULL);
}

DispatchQueueRef _Nullable DispatchQueue_Create(Int maxConcurrency, Int qos, Int priority)
{
    assert(maxConcurrency >= 1);

    DispatchQueue* pQueue = (DispatchQueue*) kalloc(sizeof(DispatchQueue) + sizeof(ConcurrencyLane) * (maxConcurrency - 1), HEAP_ALLOC_OPTION_CPU);
        
    if (pQueue) {
        List_Init(&pQueue->item_queue);
        List_Init(&pQueue->timer_queue);
        List_Init(&pQueue->item_cache_queue);
        List_Init(&pQueue->timer_cache_queue);
        Lock_Init(&pQueue->lock);
        ConditionVariable_Init(&pQueue->cond_var);
        pQueue->maxConcurrency = (Int8)max(min(maxConcurrency, INT8_MAX), 1);
        pQueue->availableConcurrency = 0;
        pQueue->qos = qos;
        pQueue->priority = priority;
        pQueue->item_cache_count = 0;
        pQueue->timer_cache_count = 0;

        for (Int i = 0; i < maxConcurrency; i++) {
            pQueue->concurrency_lanes[i].vp = NULL;
        }
    }
    
    return pQueue;
}

static void DispatchQueue_DestroyConcurrencyLane_Locked(DispatchQueueRef _Nonnull pQueue, Int idx)
{
    assert(idx >= 0 && idx < pQueue->maxConcurrency);

    // XXX do nothing with the VP right now. We'll fix the whole VP relinquishing
    // XXX story soon
    pQueue->concurrency_lanes[idx].vp = NULL;
    pQueue->availableConcurrency--;
}

void DispatchQueue_Destroy(DispatchQueueRef _Nullable pQueue)
{
    if (pQueue) {
        // XXX fix the queue shutdown:
        // - DispatchQueue_Destroy should mark the queue as being destroyed/shut down
        // - this means that the queue should no longer accept new dispatch requests
        // - we should synchronously schedule the freeing of the resources on the dispatch queue
        // - or so
        // - then kfree the dispatch queue data structure here
        List_Deinit(&pQueue->item_queue);
        List_Deinit(&pQueue->timer_queue);
        List_Deinit(&pQueue->item_cache_queue);
        List_Deinit(&pQueue->timer_cache_queue);
        Lock_Deinit(&pQueue->lock);
        ConditionVariable_Deinit(&pQueue->cond_var);

        for (Int i = 0; i < pQueue->maxConcurrency; i++) {
            DispatchQueue_DestroyConcurrencyLane_Locked(pQueue, i);
        }

        kfree((Byte*) pQueue);
    }
}

static Int DispatchQueue_IndexOfConcurrencyLaneForVirtualProcessor_Locked(DispatchQueueRef _Nonnull pQueue, VirtualProcessor* pVP)
{
    for (Int i = 0; i < pQueue->maxConcurrency; i++) {
        if (pQueue->concurrency_lanes[i].vp == pVP) {
            return i;
        }
    }

    return -1;
}

static void DispatchQueue_SpawnVirtualProcessorIfNeeded_Locked(DispatchQueueRef _Nonnull pQueue, VirtualProcessor_Closure _Nonnull pWorkerFunc)
{
    // XXX be smarter here and defer spinning u a new VP until we really need one
    if (pQueue->availableConcurrency < pQueue->maxConcurrency) {
        VirtualProcessorAttributes attribs;
        
        VirtualProcessorAttributes_Init(&attribs);
        attribs.priority = pQueue->qos * DISPATCH_PRIORITY_COUNT + (pQueue->priority + DISPATCH_PRIORITY_COUNT / 2) + VP_PRIORITIES_RESERVED_LOW;
        VirtualProcessor* pVP = VirtualProcessorPool_AcquireVirtualProcessor(
                                                                            VirtualProcessorPool_GetShared(),
                                                                            &attribs,
                                                                            pWorkerFunc,
                                                                            (Byte*)pQueue,
                                                                            false);
        
        ConcurrencyLane* pConLane = NULL;
        for (Int i = 0; i < pQueue->maxConcurrency; i++) {
            if (pQueue->concurrency_lanes[i].vp == NULL) {
                pConLane = &pQueue->concurrency_lanes[i];
                break;
            }
        }
        assert(pConLane != NULL);
        
        pConLane->vp = pVP;
        pQueue->availableConcurrency++;

        VirtualProcessor_Resume(pVP, false);
    }
}

// Creates a work item for the given closure and closure context. Tries to reuse
// an existing work item from the work item cache whenever possible. Expects that
// the caller holds the dispatch queue lock.
static WorkItemRef _Nullable DispatchQueue_AcquireWorkItem_Locked(DispatchQueueRef _Nonnull pQueue, DispatchQueue_Closure _Nonnull pClosure, Byte* _Nullable pContext)
{
    WorkItemRef pItem = (WorkItemRef) List_RemoveFirst(&pQueue->item_cache_queue);

    if (pItem != NULL) {
        WorkItem_Init(pItem, kItemType_Immediate, pClosure, pContext, true);
        pQueue->item_cache_count--;
    } else {
        pItem = WorkItem_Create_Internal(pClosure, pContext, true);
    }
    
    return pItem;
}

// Reqlinquishes the given work item back to the item cache if possible. The
// item is freed if the cache is at capacity. The item must be owned by the
// dispatch queue.
static void DispatchQueue_RelinquishWorkItem_Locked(DispatchQueue* _Nonnull pQueue, WorkItemRef _Nonnull pItem)
{
    assert(pItem->is_owned_by_queue);

    if (pQueue->item_cache_count < MAX_ITEM_CACHE_COUNT) {
        WorkItem_Deinit(pItem);
        List_InsertBeforeFirst(&pQueue->item_cache_queue, &pItem->queue_entry);
        pQueue->item_cache_count++;
    } else {
        WorkItem_Destroy(pItem);
    }
}

// Creates a timer for the given closure and closure context. Tries to reuse
// an existing timer from the timer cache whenever possible. Expects that the
// caller holds the dispatch queue lock.
static TimerRef _Nullable DispatchQueue_AcquireTimer_Locked(DispatchQueueRef _Nonnull pQueue, TimeInterval deadline, TimeInterval interval, DispatchQueue_Closure _Nonnull pClosure, Byte* _Nullable pContext)
{
    TimerRef pTimer = (TimerRef) List_RemoveFirst(&pQueue->timer_cache_queue);

    if (pTimer != NULL) {
        Timer_Init(pTimer, deadline, interval, pClosure, pContext, true);
        pQueue->timer_cache_count--;
    } else {
        pTimer = Timer_Create_Internal(deadline, interval, pClosure, pContext, true);
    }
    
    return pTimer;
}

// Reqlinquishes the given timer back to the timer cache if possible. The timer
// is freed if the cache is at capacity. The timer must be owned by the dispatch
// queue.
static void DispatchQueue_RelinquishTimer_Locked(DispatchQueue* _Nonnull pQueue, TimerRef _Nonnull pTimer)
{
    assert(pTimer->item.is_owned_by_queue);

    if (pQueue->timer_cache_count < MAX_TIMER_CACHE_COUNT) {
        Timer_Deinit(pTimer);
        List_InsertBeforeFirst(&pQueue->timer_cache_queue, &pTimer->item.queue_entry);
        pQueue->timer_cache_count++;
    } else {
        Timer_Destroy(pTimer);
    }
}

// Asynchronously executes the given work item. The work item is executed as
// soon as possible. Expects to be called with the dispatch queue held.
static void DispatchQueue_DispatchWorkItemAsync_Locked(DispatchQueueRef _Nonnull pQueue, WorkItemRef _Nonnull pItem)
{
    List_InsertAfterLast(&pQueue->item_queue, &pItem->queue_entry);
    DispatchQueue_SpawnVirtualProcessorIfNeeded_Locked(pQueue, (VirtualProcessor_Closure)DispatchQueue_Run);
    ConditionVariable_Signal(&pQueue->cond_var, &pQueue->lock);
}

// Synchronously executes the given work item. The work item is executed as
// soon as possible and the caller remains blocked until the work item has finished
// execution. Expects that the caller holds the dispatch queue lock.
static void DispatchQueue_DispatchWorkItemSync_Locked(DispatchQueueRef _Nonnull pQueue, WorkItemRef _Nonnull pItem)
{
    Semaphore* pCompletionSema = Semaphore_Create(0);

    // The work item maintains a weak reference to the cached completion semaphore
    pItem->completion_sema = pCompletionSema;

    DispatchQueue_DispatchWorkItemAsync_Locked(pQueue, pItem);
    Semaphore_Acquire(pCompletionSema, kTimeInterval_Infinity);
    Semaphore_Destroy(pCompletionSema);
}

// Synchronously executes the given work item. The work item is executed as
// soon as possible and the caller remains blocked until the work item has finished
// execution.
void DispatchQueue_DispatchWorkItemSync(DispatchQueueRef _Nonnull pQueue, WorkItemRef _Nonnull pItem)
{
    if (AtomicBool_Set(&pItem->is_being_dispatched, true)) {
        // Some other queue is already dispatching this work item
        abort();
    }

    Lock_Lock(&pQueue->lock);
    DispatchQueue_DispatchWorkItemSync_Locked(pQueue, pItem);
}

// Synchronously executes the given closure. The closure is executed as soon as
// possible and the caller remains blocked until the closure has finished execution.
void DispatchQueue_DispatchSync(DispatchQueueRef _Nonnull pQueue, DispatchQueue_Closure _Nonnull pClosure, Byte* _Nullable pContext)
{
    Lock_Lock(&pQueue->lock);

    WorkItem* pItem = DispatchQueue_AcquireWorkItem_Locked(pQueue, pClosure, pContext);
    assert(pItem != NULL);
    DispatchQueue_DispatchWorkItemSync_Locked(pQueue, pItem);
}


// Asynchronously executes the given work item. The work item is executed as
// soon as possible.
void DispatchQueue_DispatchWorkItemAsync(DispatchQueueRef _Nonnull pQueue, WorkItemRef _Nonnull pItem)
{
    if (AtomicBool_Set(&pItem->is_being_dispatched, true)) {
        // Some other queue is already dispatching this work item
        abort();
    }

    Lock_Lock(&pQueue->lock);
    DispatchQueue_DispatchWorkItemAsync_Locked(pQueue, pItem);
}

// Asynchronously executes the given closure. The closure is executed as soon as
// possible.
void DispatchQueue_DispatchAsync(DispatchQueueRef _Nonnull pQueue, DispatchQueue_Closure _Nonnull pClosure, Byte* _Nullable pContext)
{
    Lock_Lock(&pQueue->lock);

    WorkItem* pItem = DispatchQueue_AcquireWorkItem_Locked(pQueue, pClosure, pContext);
    assert(pItem != NULL);
    DispatchQueue_DispatchWorkItemAsync_Locked(pQueue, pItem);
}

// Adds the given timer to the timer queue. Expects that the queue is already
// locked. Does not wake up the queue.
static void DispatchQueue_AddTimer_Locked(DispatchQueueRef _Nonnull pQueue, TimerRef _Nonnull pTimer)
{
    TimerRef pPrevTimer = NULL;
    TimerRef pCurTimer = (TimerRef)pQueue->timer_queue.first;
    
    while (pCurTimer) {
        if (TimeInterval_Greater(pCurTimer->deadline, pTimer->deadline)) {
            break;
        }
        
        pPrevTimer = pCurTimer;
        pCurTimer = (TimerRef)pCurTimer->item.queue_entry.next;
    }
    
    List_InsertAfter(&pQueue->timer_queue, &pTimer->item.queue_entry, &pPrevTimer->item.queue_entry);
}

// Asynchronously executes the given timer when it comes due. Expects that the
// caller holds the dispatch queue lock.
void DispatchQueue_DispatchTimer_Locked(DispatchQueueRef _Nonnull pQueue, TimerRef _Nonnull pTimer)
{
    DispatchQueue_AddTimer_Locked(pQueue, pTimer);
    DispatchQueue_SpawnVirtualProcessorIfNeeded_Locked(pQueue, (VirtualProcessor_Closure)DispatchQueue_Run);
    ConditionVariable_Signal(&pQueue->cond_var, &pQueue->lock);
}

// Asynchronously executes the given timer when it comes due.
void DispatchQueue_DispatchTimer(DispatchQueueRef _Nonnull pQueue, TimerRef _Nonnull pTimer)
{
    if (AtomicBool_Set(&pTimer->item.is_being_dispatched, true)) {
        // Some other queue is already dispatching this timer
        abort();
    }

    Lock_Lock(&pQueue->lock);
    DispatchQueue_DispatchTimer_Locked(pQueue, pTimer);
}

// Asynchronously executes the given closure on or after 'deadline'. The dispatch
// queue will try to execute the closure as close to 'deadline' as possible.
void DispatchQueue_DispatchAsyncAfter(DispatchQueueRef _Nonnull pQueue, TimeInterval deadline, DispatchQueue_Closure _Nonnull pClosure, Byte* _Nullable pContext)
{
    Lock_Lock(&pQueue->lock);

    Timer* pTimer = DispatchQueue_AcquireTimer_Locked(pQueue, deadline, kTimeInterval_Zero, pClosure, pContext);
    assert(pTimer != NULL);
    DispatchQueue_DispatchTimer_Locked(pQueue, pTimer);
}



void DispatchQueue_Run(DispatchQueueRef _Nonnull pQueue)
{
    WorkItem* pItem = NULL;
    Timer* pTimerToRearm = NULL;

    while (true) {
        pItem = NULL;
        
        Lock_Lock(&pQueue->lock);

        // Rearm a repeating timer if we executed a repeating timer in the previous
        // iteration and it's not been cancelled in the meantime.
        if (pTimerToRearm) {
            if (!pTimerToRearm->item.cancelled) {
                // Repeating timer: rearm it with the next fire date that's in
                // the future (the next fire date we haven't already missed).
                const TimeInterval curTime = MonotonicClock_GetCurrentTime();
                
                do  {
                    pTimerToRearm->deadline = TimeInterval_Add(pTimerToRearm->deadline, pTimerToRearm->interval);
                } while (TimeInterval_Less(pTimerToRearm->deadline, curTime));
                DispatchQueue_AddTimer_Locked(pQueue, pTimerToRearm);
            }

            pTimerToRearm = NULL;
        }
        
        
        // Wait for work items to arrive or for timers to fire
        while (true) {
            if (pQueue->item_queue.first) {
                break;
            }
            
            // Compute a deadline for the wait. We do not wait if the deadline
            // is equal to the current time or it's in the past
            TimeInterval deadline;

            if (pQueue->timer_queue.first) {
                deadline = ((Timer*)pQueue->timer_queue.first)->deadline;
            } else {
                deadline = TimeInterval_Add(MonotonicClock_GetCurrentTime(), TimeInterval_MakeSeconds(2));
            }
            
            
            // Wait for work
            const Int err = ConditionVariable_Wait(&pQueue->cond_var, &pQueue->lock, deadline);
            if (err == ETIMEOUT) {
                break;
            }
        }
        
        
        // Exit this VP if we've waited for work for some time but haven't received
        // any new work
        if (List_IsEmpty(&pQueue->timer_queue) && List_IsEmpty(&pQueue->item_queue)) {
            DispatchQueue_DestroyConcurrencyLane_Locked(pQueue, DispatchQueue_IndexOfConcurrencyLaneForVirtualProcessor_Locked(pQueue, VirtualProcessor_GetCurrent()));
            Lock_Unlock(&pQueue->lock);
            break;
        }

        
        // Grab the first timer that's due. We give preference to timers because
        // they are tied to a specific deadline time while immediate work items
        // do not guarantee that they will execute at a specific time. So it's
        // acceptable to push them back on the timeline.
        Timer* pFirstTimer = (Timer*)pQueue->timer_queue.first;
        if (pFirstTimer && TimeInterval_LessEquals(pFirstTimer->deadline, MonotonicClock_GetCurrentTime())) {
            pItem = (WorkItem*) List_RemoveFirst(&pQueue->timer_queue);
        }

        
        // Grab the first work item if no timer is due
        if (pItem == NULL) {
            pItem = (WorkItem*) List_RemoveFirst(&pQueue->item_queue);
        }

        Lock_Unlock(&pQueue->lock);


        // Execute the work item
        pItem->closure(pItem->context);


        // Signal the work item's completion semaphore if needed
        if (pItem->completion_sema != NULL) {
            Semaphore_Release(pItem->completion_sema);
            pItem->completion_sema = NULL;
        }


        // Move the work item back to the item cache if possible or destroy it
        switch (pItem->type) {
            case kItemType_Immediate:
                if (pItem->is_owned_by_queue) {
                    Lock_Lock(&pQueue->lock);
                    DispatchQueue_RelinquishWorkItem_Locked(pQueue, pItem);
                    Lock_Unlock(&pQueue->lock);
                }
                break;
                
            case kItemType_OneShotTimer:
                if (pItem->is_owned_by_queue) {
                    Lock_Lock(&pQueue->lock);
                    DispatchQueue_RelinquishTimer_Locked(pQueue, (TimerRef) pItem);
                    Lock_Unlock(&pQueue->lock);
                }
                break;
                
            case kItemType_RepeatingTimer: {
                Timer* pTimer = (TimerRef)pItem;
                
                if (pTimer->item.cancelled) {
                    if (pItem->is_owned_by_queue) {
                        Lock_Lock(&pQueue->lock);
                        DispatchQueue_RelinquishTimer_Locked(pQueue, pTimer);
                        Lock_Unlock(&pQueue->lock);
                    }
                } else {
                    pTimerToRearm = pTimer;
                }
                break;
            }
                
            default:
                abort();
                break;
        }
    }
}
