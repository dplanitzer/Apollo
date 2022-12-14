;
;  VirtualProcessorScheduler_asm.i
;  Apollo
;
;  Created by Dietmar Planitzer on 2/23/21.
;  Copyright © 2021 Dietmar Planitzer. All rights reserved.
;

    include "lowmem.i"

    xdef _VirtualProcessorScheduler_DisablePreemption
    xdef _VirtualProcessorScheduler_RestorePreemption
    xdef _VirtualProcessorScheduler_DisableCooperation
    xdef _VirtualProcessorScheduler_RestoreCooperation
    xdef _VirtualProcessorScheduler_IsCooperationEnabled
    xdef _VirtualProcessorScheduler_SwitchContext
    xdef __rtecall_VirtualProcessorScheduler_SwitchContext
    xdef __rtecall_VirtualProcessorScheduler_RestoreContext


;-------------------------------------------------------------------------------
; Int VirtualProcessorScheduler_DisablePreemption(void)
; Disables preemption and returns the previous preemption state.
_VirtualProcessorScheduler_DisablePreemption:
    DISABLE_PREEMPTION d0
    rts


;-------------------------------------------------------------------------------
; void VirtualProcessorScheduler_RestorePreemption(Int sps)
; Restores the preemption state to 'sps'. Note that this function call wipes out
; the condition codes and tracing enabled state.
_VirtualProcessorScheduler_RestorePreemption:
    cargs rp_state.l
    move.l  rp_state(sp), d0
    RESTORE_PREEMPTION d0
    rts


;-------------------------------------------------------------------------------
; Int VirtualProcessorScheduler_DisableCooperation(void)
; Disable voluntary context switches. These are context switches which are triggered
; by a call to wakeup()
_VirtualProcessorScheduler_DisableCooperation:
    DISABLE_COOPERATION d0
    rts


;-------------------------------------------------------------------------------
; void VirtualProcessorScheduler_RestoreCooperation(Int sps)
; Restores the given cooperation state. Voluntary context switches are reenabled if
; they were enabled before and disabled otherwise
_VirtualProcessorScheduler_RestoreCooperation:
    cargs   rc_state.l
    move.l  rc_state(sp), d0
    RESTORE_COOPERATION d0
    rts


;-------------------------------------------------------------------------------
; Int VirtualProcessorScheduler_IsCooperationEnabled(void)
; Returns true if voluntary context switches are currently enabled.
_VirtualProcessorScheduler_IsCooperationEnabled:
    btst    #SCHED_FLAG_VOLUNTARY_CSW_ENABLED, SCHEDULER_BASE + vps_flags
    sne     d0
    ext.w   d0
    ext.l   d0
    rts


;-------------------------------------------------------------------------------
; void VirtualProcessorScheduler_SwitchContext(void)
; Invokes the context switcher. Expects that preemption is disabled and that the
; scheduler set up a CSW request. Enables preemption as it switches to another VP.
; Once this function returns to the caller preemption is disabled again.
_VirtualProcessorScheduler_SwitchContext:
    inline
        ; build a RTE format #0 frame on the stack. The RTE return address points to
        ; our rts instruction.
        cmp.b   #CPU_MODEL_68000, SYS_DESC_BASE + sd_cpu_model
        beq.s   .1
        move.w  #0, -(sp)               ; format #0
.1:
        lea     .csw_return(pc), a0     ; PC
        move.l  a0, -(sp)
        move.w  sr, -(sp)               ; SR
        jmp     __rtecall_VirtualProcessorScheduler_SwitchContext

.csw_return:
        rts
    einline


;-------------------------------------------------------------------------------
; void __rtecall_VirtualProcessorScheduler_SwitchContext(void)
; Saves the CPU state of the currently running VP and restores the CPU state of
; the scheduled VP. Expects that it is called with a RTE frame format #0 stack
; frame on top of the stack. You want to call this function with a jmp instruction.
;
; Expected stack frame at entry:
; SP + 6: format #0 (68010+ only)
; SP + 2: PC
; SP + 0: SR
__rtecall_VirtualProcessorScheduler_SwitchContext:
    ; save the integer state
    move.l  a0, (SCHEDULER_BASE + vps_csw_scratch)
    move.l  (SCHEDULER_BASE + vps_running), a0
    lea     vp_save_area(a0), a0

    movem.l d0 - d7 / a0 - a7, (a0)
    move.l  (SCHEDULER_BASE + vps_csw_scratch), cpu_a0(a0)

    move.l  usp, a1
    move.l  a1, cpu_usp(a0)

    move.w  0(sp), cpu_sr(a0)
    move.l  2(sp), cpu_pc(a0)

    ; check whether we should save the FPU state
    btst    #CSWB_HW_HAS_FPU, SCHEDULER_BASE + vps_csw_hw
    beq.s   __rtecall_VirtualProcessorScheduler_RestoreContext

    ; save the FPU state. Note that the 68060 fmovem.l instruction does not
    ; support moving > 1 register at a time
    fsave       cpu_fsave(a0)
    fmovem      fp0 - fp7, cpu_fp0(a0)
    fmovem.l    fpcr, cpu_fpcr(a0)
    fmovem.l    fpsr, cpu_fpsr(a0)
    fmovem.l    fpiar, cpu_fpiar(a0)

__rtecall_VirtualProcessorScheduler_RestoreContext:
    ; consume the CSW switch signal
    bclr    #CSWB_SIGNAL_SWITCH, SCHEDULER_BASE + vps_csw_signals
    ; it's safe to trash all registers here 'cause we'll override them anyway
    ; make the scheduled VP the running VP and clear out vps_scheduled
    move.l  (SCHEDULER_BASE + vps_scheduled), a0
    move.l  a0, (SCHEDULER_BASE + vps_running)
    moveq.l #0, d0
    move.l  d0, (SCHEDULER_BASE + vps_scheduled)

    ; update the state to Running
    move.b  #kVirtualProcessorState_Running, vp_state(a0)
    lea     vp_save_area(a0), a0

    ; check whether we should restore the FPU state
    btst    #CSWB_HW_HAS_FPU, SCHEDULER_BASE + vps_csw_hw
    beq.s   .1

    ; restore the FPU state. Note that the 68060 fmovem.l instruction does not
    ; support moving > 1 register at a time
    fmovem      cpu_fp0(a0), fp0 - fp7
    fmovem.l    cpu_fpcr(a0), fpcr
    fmovem.l    cpu_fpsr(a0), fpsr
    fmovem.l    cpu_fpiar(a0), fpiar
    frestore    cpu_fsave(a0)

.1:
    ; switch to the new ssp and usp
    move.l  cpu_a7(a0), sp
    move.l  cpu_usp(a0), a1
    move.l  a1, usp
    
    ; build a stack frame for the RTE below which will cause the CPU to start
    ; execution of the code that we want to run next:
    ; SP +  2: PC
    ; SP +  0: SR
    move.l  cpu_pc(a0), 2(sp)
    move.w  cpu_sr(a0), 0(sp)
    
    ; restore the integer state
    movem.l (a0), d0 - d7 / a0 - a6

    rte
