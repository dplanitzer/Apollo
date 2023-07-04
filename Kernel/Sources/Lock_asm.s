;
;  Lock_asm.s
;  Apollo
;
;  Created by Dietmar Planitzer on 2/10/21.
;  Copyright © 2021 Dietmar Planitzer. All rights reserved.
;

    include "lowmem.i"

    xref _Lock_OnOwnershipViolation
    xref _Lock_OnWait
    xref _Lock_WakeUp
    xref _VirtualProcessor_GetCurrentVpid

    xdef _Lock_Lock
    xdef _Lock_Unlock


    clrso
lock_value              so.l    1    ; bit #7 == 1 -> lock is in acquired state; bset#7 == 0 -> lock is available for aquisition
lock_wait_queue_first   so.l    1
lock_wait_queue_last    so.l    1
lock_owner_vpid         so.l    1
lock_SIZEOF             so


;-------------------------------------------------------------------------------
; void Lock_Lock(Lock* _Nonnull pLock)
; Acquires the lock. Blocks the caller if the lock is not available.
_Lock_Lock:
    inline
    cargs la_saved_d7.l, la_lock_ptr.l

    move.l  d7, -(sp)
    ; try a fast acquire. This means that we take advantage of the fact that
    ; the bset instruction is atomic with respect to IRQs. This will give us
    ; the lock if it isn't currently held by someone else
    move.l  la_lock_ptr(sp), a0
    bset    #7, lock_value(a0)
    beq.s   .la_acquired_lock

    ; The lock is held by someone else - wait and then retry.
    ; First recheck the lock after we have disable preemption. we have to do this
    ; because the VP who held the lock may have dropped it by now but we are not
    ; yet on the wait queue and thus the other VP isn't able to wake us up and
    ; cause us to retry.
    DISABLE_PREEMPTION d7

.la_retry:
    move.l  la_lock_ptr(sp), a0
    bset    #7, lock_value(a0)
    beq.s   .la_acquired_slow

    move.l  #SCHEDULER_BASE, -(sp)
    move.l  a0, -(sp)
    jsr     _Lock_OnWait
    addq.l  #8, sp

    bra.s   .la_retry

.la_acquired_slow:
    RESTORE_PREEMPTION d7

.la_acquired_lock:
    ; validate that the lock has no bogus owner. Eg memory corruption could have
    ; caused the owner and other fields to get populated with random bytes 
    tst.l   lock_owner_vpid(a0)
    beq.s   .la_done
    move.l  a0, -(sp)
    jsr     _Lock_OnOwnershipViolation
    ; NOT REACHED

.la_done:
    jsr     _VirtualProcessor_GetCurrentVpid
    move.l  la_lock_ptr(sp), a0
    move.l  d0, lock_owner_vpid(a0)

    move.l  (sp)+, d7
    rts
    einline


;-------------------------------------------------------------------------------
; void Lock_Unlock(Lock* _Nonnull pLock)
; Unlocks the lock.
_Lock_Unlock:
    cargs lr_saved_d7.l, lr_lock_ptr.l

    move.l  d7, -(sp)

    ; make sure that we actually own the lock before we attempt to unlock it
    jsr     _VirtualProcessor_GetCurrentVpid
    move.l  lr_lock_ptr(sp), a0
    move.l  lock_owner_vpid(a0), d1
    cmp.l   d0, d1
    beq.s   .lr_do_unlock
    move.l  a0, -(sp)
    jsr     _Lock_OnOwnershipViolation
    ; NOT REACHED

.lr_do_unlock:
    clr.l   lock_owner_vpid(a0)

    DISABLE_PREEMPTION d7

    ; release the lock
    bclr    #7, lock_value(a0)

    ; move all the waiters back to the ready queue
    move.l  #SCHEDULER_BASE, -(sp)
    move.l  a0, -(sp)
    jsr     _Lock_WakeUp
    addq.l  #8, sp

    RESTORE_PREEMPTION d7

    move.l  (sp)+, d7
    rts