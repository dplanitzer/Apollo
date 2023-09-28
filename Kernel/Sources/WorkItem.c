//
//  WorkItem.c
//  Apollo
//
//  Created by Dietmar Planitzer on 9/27/23.
//  Copyright © 2023 Dietmar Planitzer. All rights reserved.
//

#include "DispatchQueuePriv.h"

////////////////////////////////////////////////////////////////////////////////
// MARK: -
// MARK: Work Items


void WorkItem_Init(WorkItemRef _Nonnull pItem, enum ItemType type, DispatchQueueClosure closure, Bool isOwnedByQueue)
{
    SListNode_Init(&pItem->queue_entry);
    pItem->closure = closure;
    pItem->is_owned_by_queue = isOwnedByQueue;
    pItem->is_being_dispatched = false;
    pItem->cancelled = false;
    pItem->type = type;
}

// Creates a work item which will invoke the given closure. Note that work items
// are one-shot: they execute their closure and then the work item is destroyed.
ErrorCode WorkItem_Create_Internal(DispatchQueueClosure closure, Bool isOwnedByQueue, WorkItemRef _Nullable * _Nonnull pOutItem)
{
    decl_try_err();
    WorkItemRef pItem;
    
    try(kalloc(sizeof(WorkItem), (Byte**) &pItem));
    WorkItem_Init(pItem, kItemType_Immediate, closure, isOwnedByQueue);
    *pOutItem = pItem;
    return EOK;

catch:
    *pOutItem = NULL;
    return err;
}

// Creates a work item which will invoke the given closure. Note that work items
// are one-shot: they execute their closure and then the work item is destroyed.
// This is the creation method for parties that are external to the dispatch
// queue implementation.
ErrorCode WorkItem_Create(DispatchQueueClosure closure, WorkItemRef _Nullable * _Nonnull pOutItem)
{
    return WorkItem_Create_Internal(closure, false, pOutItem);
}

void WorkItem_Deinit(WorkItemRef _Nonnull pItem)
{
    SListNode_Deinit(&pItem->queue_entry);
    pItem->closure.func = NULL;
    pItem->closure.context = NULL;
    pItem->closure.isUser = false;
    pItem->completion = NULL;
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

// Sets the cancelled state of the given work item. The work item is marked as
// cancelled if the parameter is true and the cancelled state if cleared if the
// parameter is false. Note that it is the responsibility of the work item
// closure to check the cancelled state and to act appropriately on it.
// Clearing the cancelled state of a work item should normally not be necessary.
// The functionality exists to enable work item caching and reuse.
void WorkItem_SetCancelled(WorkItemRef _Nonnull pItem, Bool flag)
{
    pItem->cancelled = flag;
}

// Returns true if the given work item is in cancelled state.
Bool WorkItem_IsCancelled(WorkItemRef _Nonnull pItem)
{
    return pItem->cancelled;
}


////////////////////////////////////////////////////////////////////////////////
// MARK: -
// MARK: Timers


void _Nullable Timer_Init(TimerRef _Nonnull pTimer, TimeInterval deadline, TimeInterval interval, DispatchQueueClosure closure, Bool isOwnedByQueue)
{
    enum ItemType type = TimeInterval_Greater(interval, kTimeInterval_Zero) ? kItemType_RepeatingTimer : kItemType_OneShotTimer;

    WorkItem_Init((WorkItem*)pTimer, type, closure, isOwnedByQueue);
    pTimer->deadline = deadline;
    pTimer->interval = interval;
}

// Creates a new timer. The timer will fire on or after 'deadline'. If 'interval'
// is greater than 0 then the timer will repeat until cancelled.
ErrorCode Timer_Create_Internal(TimeInterval deadline, TimeInterval interval, DispatchQueueClosure closure, Bool isOwnedByQueue, TimerRef _Nullable * _Nonnull pOutTimer)
{
    decl_try_err();
    TimerRef pTimer;
    
    try(kalloc(sizeof(Timer), (Byte**) &pTimer));
    Timer_Init(pTimer, deadline, interval, closure, isOwnedByQueue);
    *pOutTimer = pTimer;
    return EOK;

catch:
    *pOutTimer = NULL;
    return err;
}

// Creates a new timer. The timer will fire on or after 'deadline'. If 'interval'
// is greater than 0 then the timer will repeat until cancelled.
// This is the creation method for parties that are external to the dispatch
// queue implementation.
ErrorCode Timer_Create(TimeInterval deadline, TimeInterval interval, DispatchQueueClosure closure, TimerRef _Nullable * _Nonnull pOutTimer)
{
    return Timer_Create_Internal(deadline, interval, closure, false, pOutTimer);
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
// MARK: Completion Signalers


void CompletionSignaler_Init(CompletionSignaler* _Nonnull pItem)
{
    SListNode_Init(&pItem->queue_entry);
    pItem->isInterrupted = false;
}

// Creates a completion signaler.
ErrorCode CompletionSignaler_Create(CompletionSignaler* _Nullable * _Nonnull pOutComp)
{
    decl_try_err();
    CompletionSignaler* pItem;
    
    try(kalloc(sizeof(CompletionSignaler), (Byte**) &pItem));
    CompletionSignaler_Init(pItem);
    Semaphore_Init(&pItem->semaphore, 0);
    *pOutComp = pItem;
    return EOK;

catch:
    *pOutComp = NULL;
    return err;
}

void CompletionSignaler_Deinit(CompletionSignaler* _Nonnull pItem)
{
    SListNode_Deinit(&pItem->queue_entry);
    pItem->isInterrupted = false;
}

// Deallocates the given completion signaler.
void CompletionSignaler_Destroy(CompletionSignaler* _Nullable pItem)
{
    if (pItem) {
        CompletionSignaler_Deinit(pItem);
        Semaphore_Deinit(&pItem->semaphore);
        kfree((Byte*) pItem);
    }
}
