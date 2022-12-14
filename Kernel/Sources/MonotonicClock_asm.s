;
;  MonotonicClock_asm.i
;  Apollo
;
;  Created by Dietmar Planitzer on 2/11/21.
;  Copyright © 2021 Dietmar Planitzer. All rights reserved.
;

    include "chipset.i"
    include "lowmem.i"

    xdef _MonotonicClock_GetCurrentQuantums


;-------------------------------------------------------------------------------
; Quantums MonotonicClock_GetCurrentQuantums(void)
; Returns the current number of quantums
_MonotonicClock_GetCurrentQuantums:
    move.l  MONOTONIC_CLOCK_BASE + mtc_current_quantum, d0
    rts
