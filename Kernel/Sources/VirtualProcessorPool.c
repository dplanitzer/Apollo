//
//  VirtualProcessorPool.c
//  Apollo
//
//  Created by Dietmar Planitzer on 4/14/21.
//  Copyright © 2021 Dietmar Planitzer. All rights reserved.
//

#include "VirtualProcessorPool.h"
#include "Heap.h"
#include "Lock.h"
#include "SystemGlobals.h"
#include "VirtualProcessorScheduler.h"


void VirtualProcessorAttributes_Init(VirtualProcessorAttributes* _Nonnull pAttribs)
{
    pAttribs->kernelStackSize = VP_DEFAULT_KERNEL_STACK_SIZE;
    pAttribs->userStackSize = VP_DEFAULT_USER_STACK_SIZE;
    pAttribs->priority = VP_PRIORITY_NORMAL;
}


////////////////////////////////////////////////////////////////////////////////
// MARK: -
// MARK: Pool
////////////////////////////////////////////////////////////////////////////////


#define REUSE_CACHE_CAPACITY    16
typedef struct _VirtualProcessorPool {
    Lock    lock;
    List    inuse_queue;        // VPs in use
    List    reuse_queue;        // VPs available for reuse
    Int     inuse_count;        // count of VPs that are in use
    Int     reuse_count;        // count of how many VPs are in the reuse queue
    Int     reuse_capacity;     // reuse cache will not store more than this. If a VP exits while the cache is at max capacity -> VP will exit for good and get finalized
} VirtualProcessorPool;


// Returns the shared virtual processor pool.
VirtualProcessorPoolRef _Nonnull VirtualProcessorPool_GetShared(void)
{
    return SystemGlobals_Get()->virtual_processor_pool;
}

VirtualProcessorPoolRef _Nullable VirtualProcessorPool_Create(void)
{
    VirtualProcessorPool* pool = (VirtualProcessorPool*)kalloc(sizeof(VirtualProcessorPool), 0);
    FailNULL(pool);
    
    List_Init(&pool->inuse_queue);
    List_Init(&pool->reuse_queue);
    Lock_Init(&pool->lock);
    pool->inuse_count = 0;
    pool->reuse_count = 0;
    pool->reuse_capacity = REUSE_CACHE_CAPACITY;
    return pool;
    
failed:
    VirtualProcessorPool_Destroy(pool);
    return NULL;
}

void VirtualProcessorPool_Destroy(VirtualProcessorPoolRef _Nullable pool)
{
    if (pool) {
        List_Deinit(&pool->inuse_queue);
        List_Deinit(&pool->reuse_queue);
        Lock_Deinit(&pool->lock);
        kfree((Byte*)pool);
    }
}

VirtualProcessor* _Nonnull VirtualProcessorPool_AcquireVirtualProcessor(VirtualProcessorPoolRef _Nonnull pool, const VirtualProcessorAttributes* _Nonnull pAttribs, VirtualProcessor_Closure _Nonnull pClosure, Byte* _Nullable pContext, Bool isUser)
{
    VirtualProcessor* pVP = NULL;

    Lock_Lock(&pool->lock);

    // Try to reuse a cached VP
    VirtualProcessorOwner* pCurVP = (VirtualProcessorOwner*)pool->reuse_queue.first;
    while (pCurVP) {
        if (VirtualProcessor_IsSuspended(pCurVP->self)) {
            pVP = pCurVP->self;
            break;
        }
        pCurVP = (VirtualProcessorOwner*)pCurVP->queue_entry.next;
    }
        
    if (pVP) {
        List_Remove(&pool->reuse_queue, &pVP->owner.queue_entry);
        pool->reuse_count--;
        
        List_InsertBeforeFirst(&pool->inuse_queue, &pVP->owner.queue_entry);
        pool->inuse_count++;
    }
    
    Lock_Unlock(&pool->lock);
    
    
    // Create a new VP if we were not able to reuse a cached one
    if (pVP == NULL) {
        pVP = VirtualProcessor_Create();
        FailNULL(pVP);

        Lock_Lock(&pool->lock);
        List_InsertBeforeFirst(&pool->inuse_queue, &pVP->owner.queue_entry);
        pool->inuse_count++;
        Lock_Unlock(&pool->lock);
    }
    
    
    // Configure the VP
    VirtualProcessor_SetPriority(pVP, pAttribs->priority);
    VirtualProcessor_SetMaxKernelStackSize(pVP, pAttribs->kernelStackSize);
    VirtualProcessor_SetMaxUserStackSize(pVP, pAttribs->userStackSize);
    VirtualProcessor_SetClosure(pVP, pClosure, pContext, isUser);

    return pVP;

failed:
    return NULL;
}

// Relinquishes the given VP back to the reuse pool if possible. If the reuse
// pool is full then the given VP is suspended and scheduled for finalization
// instead. Note that the VP is suspended in any case.
void VirtualProcessorPool_RelinquishVirtualProcessor(VirtualProcessorPoolRef _Nonnull pool, VirtualProcessor* _Nonnull pVP)
{
    Bool didReuse = false;
    
    Lock_Lock(&pool->lock);
    
    List_Remove(&pool->inuse_queue, &pVP->owner.queue_entry);
    pool->inuse_count--;

    if (pool->reuse_count < pool->reuse_capacity) {
        List_InsertBeforeFirst(&pool->reuse_queue, &pVP->owner.queue_entry);
        pool->reuse_count++;
        didReuse = true;
    }
    Lock_Unlock(&pool->lock);
    
    if (didReuse) {
        VirtualProcessor_Suspend(pVP);
    } else {
        VirtualProcessor_ScheduleFinalization(pVP);
    }
    /* NOT REACHED */
}
