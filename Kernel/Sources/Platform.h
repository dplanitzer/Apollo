//
//  Platform.h
//  Kernel
//
//  Created by Dietmar Planitzer on 2/4/21.
//  Copyright © 2021 Dietmar Planitzer. All rights reserved.
//

#ifndef Platform_h
#define Platform_h

#include <klib/Types.h>

//
// CPU
//

// Size of a standard page in bytes
#define CPU_PAGE_SIZE   4096


#if __LP64__
#define STACK_ALIGNMENT  16
#elif __ILP32__
#define STACK_ALIGNMENT  4
#else
#error "don't know how to align stack pointers"
#endif


// CPU types
#define CPU_MODEL_68000     0
#define CPU_MODEL_68010     1
#define CPU_MODEL_68020     2
#define CPU_MODEL_68030     3
#define CPU_MODEL_68040     4
#define CPU_MODEL_68060     6

// FPU types
#define FPU_MODEL_NONE      0
#define FPU_MODEL_68881     1
#define FPU_MODEL_68882     2
#define FPU_MODEL_68040     3
#define FPU_MODEL_68060     4


// CPU register state
typedef struct _CpuContext {
    // 68000 or better
    UInt32      d[8];
    UInt32      a[8];
    UInt32      usp;
    UInt32      pc;
    UInt16      sr;
    UInt16      padding;
    // 68881, 68882, 68040 or better
    UInt32      fsave[54];  // fsave / frestore data (see M68000PRM p. 6-12)
    Float96     fp[8];
    UInt32      fpcr;
    UInt32      fpsr;
    UInt32      fpiar;
} CpuContext;


// Exception stack frame
typedef struct _ExceptionStackFrame {
    UInt16  sr;
    UInt32  pc;
    struct {
        UInt16  format: 4;
        UInt16  vector: 12;
    }       fv;
    union {
        struct _Format2 {
            UInt32  addr;
        }   f2;
        struct _Format3 {
            UInt32  ea;
        }   f3;
        struct _Format4 {
            UInt32  ea;
            UInt32  pcFaultedInstr;
        }   f4;
        struct _Format7 {
            UInt32  ea;
            UInt16  ssw;
            UInt16  wb3s;
            UInt16  wb2s;
            UInt16  wb1s;
            UInt32  fa;
            UInt32  wb3a;
            UInt32  wb3d;
            UInt32  wb2a;
            UInt32  wb2d;
            UInt32  wb1a;
            UInt32  wb1d;
            UInt32  pd1;
            UInt32  pd2;
            UInt32  pd3;
        }   f7;
        struct _Format9 {
            UInt32  ia;
            UInt16  ir[4];
        }   f9;
        struct _FormatA {
            UInt16  ir0;
            UInt16  ssw;
            UInt16  ipsc;
            UInt16  ipsb;
            UInt32  dataCycleFaultAddress;
            UInt16  ir1;
            UInt16  ir2;
            UInt32  dataOutputBuffer;
            UInt16  ir3;
            UInt16  ir4;
        }   fa;
        struct _FormatB {
            UInt16  ir0;
            UInt16  ssw;
            UInt16  ipsc;
            UInt16  ipsb;
            UInt32  dataCycleFaultAddress;
            UInt16  ir1;
            UInt16  ir2;
            UInt32  dataOutputBuffer;
            UInt16  ir3;
            UInt16  ir4;
            UInt16  ir5;
            UInt16  ir6;
            UInt32  stageBAddress;
            UInt16  ir7;
            UInt16  ir8;
            UInt32  dataInputBuffer;
            UInt16  ir9;
            UInt16  ir10;
            UInt16  ir11;
            UInt16  version;
            UInt16  ir[18];
        }   fb;
    }       u;
} ExceptionStackFrame;


typedef void (* _Nonnull Cpu_UserClosure)(Byte* _Nullable pContext);


extern void cpu_enable_irqs(void);
extern Int cpu_disable_irqs(void);
extern void cpu_restore_irqs(Int state);

extern const Character* _Nonnull cpu_get_model_name(Int8 cpu_model);

extern Int cpu_verify_ram_4b(Byte* pSrc);

extern Int cpu_guarded_read(Byte* _Nonnull src, Byte* _Nonnull buffer, Int buffer_size);
extern Int cpu_guarded_write(Byte* _Nonnull dst, const Byte* _Nonnull buffer, Int buffer_size);

extern void cpu_sleep(Int cpu_type);

extern void cpu_call_as_user(Cpu_UserClosure _Nonnull pClosure, Byte* _Nullable pContext);
extern void cpu_abort_call_as_user(void);

extern _Noreturn cpu_non_recoverable_error(void);
extern _Noreturn mem_non_recoverable_error(void);


//
// FPU
//
extern const Character* _Nonnull fpu_get_model_name(Int8 fpu_model);


//
// Memory
//

// Supported max number of memory descriptors
#define MEMORY_DESCRIPTORS_CAPACITY  8

// The type of memory
// Memory accessible to the CPU only
#define MEM_TYPE_MEMORY         0
// Memory accessible to the CPU and I/O (GPU, Audio, etc)
#define MEM_TYPE_UNIFIED_MEMORY 1

// A memory descriptor describes a contiguous range of RAM
typedef struct _MemoryDescriptor {
    Byte* _Nonnull  lower;
    Byte* _Nonnull  upper;
    Int8            type;       // MEM_TYPE_XXX
    UInt8           reserved[3];
} MemoryDescriptor;

typedef struct _MemoryLayout {
    Int                 descriptor_count;
    MemoryDescriptor    descriptor[MEMORY_DESCRIPTORS_CAPACITY];
} MemoryLayout;


extern Bool mem_check_region(MemoryLayout* pMemLayout, Byte* lower, Byte* upper, Int8 type);


#define CIAA_BASE           0xbfe001
#define CIAB_BASE           0xbfd000
#define RTC_BASE            0xdc0000
#define RAMSEY_CHIP_BASE    0xde0043
#define CUSTOM_BASE         0xdff000
#define DIAGNOSTIC_ROM_BASE 0xf00000
#define DIAGNOSTIC_ROM_SIZE 0x80000
#define EXT_ROM_BASE        0xf80000
#define EXT_ROM_SIZE        0x40000
#define BOOT_ROM_BASE       0xfc0000
#define BOOT_ROM_SIZE       0x40000


//
// CIA
//

// Reading / Writing CIA A/B registers
#define CIAA_BASE_DECL(cp) \
    volatile Byte* cp = (volatile Byte*) CIAA_BASE

#define CIAB_BASE_DECL(cp) \
    volatile Byte* cp = (volatile Byte*) CIAB_BASE

#define CIA_REG_8(cp, r) \
    ((volatile UInt8*)(cp + r))


#define CIA_PRA     0x000
#define CIA_PRB     0x100
#define CIA_DDRA    0x200
#define CIA_DDRB    0x300
#define CIA_TALO    0x400
#define CIA_TAHI    0x500
#define CIA_TBLO    0x600
#define CIA_TBHI    0x700
#define CIA_TODLO   0x800
#define CIA_TODMID  0x900
#define CIA_TODHI   0xa00
#define CIA_SDR     0xc00
#define CIA_ICR     0xd00
#define CIA_CRA     0xe00
#define CIA_CRB     0xf00

#define CIAA_PRAF_OVL   0x01
#define CIAA_PRAF_LED   0x02
#define CIAA_PRAF_CHNG  0x04
#define CIAA_PRAF_WPRO  0x08
#define CIAA_PRAF_TK0   0x10
#define CIAA_PRAF_RDY   0x20
#define CIAA_PRAF_FIR0  0x40
#define CIAA_PRAF_FIR1  0x80

#define CIAB_PRAF_BUSY  0x01
#define CIAB_PRAF_POUT  0x02
#define CIAB_PRAF_SEL   0x04
#define CIAB_PRAF_DSR   0x08
#define CIAB_PRAF_CTS   0x10
#define CIAB_PRAF_CD    0x20
#define CIAB_PRAF_RTS   0x40
#define CIAB_PRAF_DTR   0x80

#define CIAB_PRBF_STEP  0x01
#define CIAB_PRBF_DIR   0x02
#define CIAB_PRBF_SIDE  0x04
#define CIAB_PRBF_SEL0  0x08
#define CIAB_PRBF_SEL1  0x10
#define CIAB_PRBF_SEL2  0x20
#define CIAB_PRBF_SEL3  0x40
#define CIAB_PRBF_MTR   0x80


//
// Chipset
//

// Reading / Writing chipset registers
#define CHIPSET_BASE_DECL(cp) \
    volatile Byte* cp = (volatile Byte*) CUSTOM_BASE

#define CHIPSET_REG_16(cp, r) \
    ((volatile UInt16*)(cp + r))

#define CHIPSET_REG_32(cp, r) \
    ((volatile UInt32*)(cp + r))


// 8361 (Regular) or 8370 (Fat) (Agnus-NTSC) = 10, 512KB
// 8367 (Pal) or 8371 (Fat-Pal) (Agnus-PAL) = 00, 512KB
// 8372 (Fat-hr) (agnushr),thru rev4 = 20 PAL, 30 NTSC, 1MB
// 8372 (Fat-hr) (agnushr),rev 5 = 22 PAL, 31 NTSC, 1MB
// 8374 (Alice) thru rev 2 = 22 PAL, 32 NTSC, 2MB
// 8374 (Alice) rev 3 thru rev 4 = 23 PAL, 33 NTSC, 2MB
#define CHIPSET_8361_NTSC       0x10
#define CHIPSET_8367_PAL        0x00
#define CHIPSET_8370_NTSC       0x10
#define CHIPSET_8371_PAL        0x00
#define CHIPSET_8372_rev4_PAL   0x20
#define CHIPSET_8372_rev4_NTSC  0x30
#define CHIPSET_8372_rev5_PAL   0x22
#define CHIPSET_8372_rev5_NTSC  0x31
#define CHIPSET_8374_rev2_PAL   0x22
#define CHIPSET_8374_rev2_NTSC  0x32
#define CHIPSET_8374_rev3_PAL   0x23
#define CHIPSET_8374_rev3_NTSC  0x33


// RAMSEY chip versions (32bit Amigas only. Like A3000 / A4000)
#define CHIPSET_RAMSEY_rev04    0x0d
#define CHIPSET_RAMSEY_rev07    0x0f


// Chipset registers
#define VPOSR       0x004
#define VHPOSR      0x006
#define DSKDATR     0x008
#define JOY0DAT     0x00a
#define JOY1DAT     0x00c
#define CLXDAT      0x00e
#define ADKCONR     0x010
#define POT0DAT     0x012
#define POT1DAT     0x014
#define POTINP      0x016
#define POTGOR      0x016
#define SERDATR     0x018
#define DSKBYTR     0x01a

#define INTENAR     0x01c
#define INTREQR     0x01e

#define DSKPT       0x020
#define DSKPTH      0x020
#define DSKPTL      0x022
#define DSKLEN      0x024
#define DSKDAT      0x026

#define REFPTR      0x028
#define VPOSW       0x02a
#define VHPOSW      0x02c
#define COPCON      0x02e
#define SERDAT      0x030
#define SERPER      0x032
#define POTGO       0x034
#define JOYTEST     0x036
#define STREQU      0x038
#define STRVBL      0x03a
#define STRHOR      0x03c
#define STRLONG     0x03e

#define BLTCON0     0x040
#define BLTCON1     0x042
#define BLTAFWM     0x044
#define BLTALWM     0x046
#define BLTCPT      0x048
#define BLTCPTH     0x048
#define BLTCPL      0x04a
#define BLTBPT      0x04c
#define BLTBPTH     0x04c
#define BLTBPL      0x04e
#define BLTAPT      0x050
#define BLTAPTH     0x050
#define BLTAPL      0x052
#define BLTDPT      0x054
#define BLTDPTH     0x054
#define BLTDPL      0x056
#define BLTSIZE     0x058
#define BLTCON0L    0x05a
#define BLTSIZV     0x05c
#define BLTSIZH     0x05e
#define BLTCMOD     0x060
#define BLTBMOD     0x062
#define BLTAMOD     0x064
#define BLTDMOD     0x066
#define BLTCDAT     0x070
#define BLTBDAT     0x072
#define BLTADAT     0x074

#define SPRHDAT     0x078
#define BPLHDAT     0x07a
#define DENISEID    0x07c
#define DSKSYNC     0x07e

#define COP1LC      0x080
#define COP1LCH     0x080
#define COP1LCL     0x082
#define COP2LC      0x084
#define COP2LCH     0x084
#define COP2LCL     0x086
#define COPJMP1     0x088
#define COPJMP2     0x08A
#define COPINS      0x08c

#define DIWSTART    0x08e
#define DIWSTOP     0x090
#define DDFSTART    0x092
#define DDFSTOP     0x094
#define DMACON      0x096

#define CLXCON      0x098
#define INTENA      0x09a
#define INTREQ      0x09c

#define ADKCON      0x09e
#define AUD0LC      0x0a0
#define AUD0LCH     0x0a0
#define AUD0LCL     0x0a2
#define AUD0LEN     0x0a4
#define AUD0PER     0x0a6
#define AUD0VOL     0x0a8
#define AUD0DAT     0x0aa
#define AUD1LC      0x0b0
#define AUD1LCH     0x0b0
#define AUD1LCL     0x0b2
#define AUD1LEN     0x0b4
#define AUD1PER     0x0b6
#define AUD1VOL     0x0b8
#define AUD1DAT     0x0ba
#define AUD2LC      0x0c0
#define AUD2LCH     0x0c0
#define AUD2LCL     0x0c2
#define AUD2LEN     0x0c4
#define AUD2PER     0x0c6
#define AUD2VOL     0x0c8
#define AUD2DAT     0x0ca
#define AUD3LC      0x0d0
#define AUD3LCH     0x0d0
#define AUD3LCL     0x0d2
#define AUD3LEN     0x0d4
#define AUD3PER     0x0d6
#define AUD3VOL     0x0d8
#define AUD3DAT     0x0da

#define BPL_BASE    0x0e0
#define BPL1PT      0x0e0
#define BPL1PTH     0x0e0
#define BPL1PTL     0x0e2
#define BPL2PT      0x0e4
#define BPL2PTH     0x0e4
#define BPL2PTL     0x0e6
#define BPL3PT      0x0e8
#define BPL3PTH     0x0e8
#define BPL3PTL     0x0ea
#define BPL4PT      0x0ec
#define BPL4PTH     0x0ec
#define BPL4PTL     0x0ee
#define BPL5PT      0x0f0
#define BPL5PTH     0x0f0
#define BPL5PTL     0x0f2
#define BPL6PT      0x0f4
#define BPL6PTH     0x0f4
#define BPL6PTL     0x0f6
#define BPL7PT      0x0f8
#define BPL7PTH     0x0f8
#define BPL7PTL     0x0fa
#define BPL8PT      0x0fc
#define BPL8PTH     0x0fc
#define BPL8PTL     0x0fe

#define BPLCON0     0x100
#define BPLCON1     0x102
#define BPLCON2     0x104
#define BPLCON3     0x106
#define BPL1MOD     0x108
#define BPL2MOD     0x10a
#define BPLCON4     0x10c
#define CLXCON2     0x10e

#define BPL1DAT     0X110
#define BPL2DAT     0X112
#define BPL3DAT     0X114
#define BPL4DAT     0X116
#define BPL5DAT     0X118
#define BPL6DAT     0X11a
#define BPL7DAT     0X11c
#define BPL8DAT     0X11e

#define SPRITE_BASE 0x120
#define SPR0PT      0x120
#define SPR0PTH     0x120
#define SPR0PTL     0x122
#define SPR1PT      0x124
#define SPR1PTH     0x124
#define SPR1PTL     0x126
#define SPR2PT      0x128
#define SPR2PTH     0x128
#define SPR2PTL     0x12a
#define SPR3PT      0x12c
#define SPR3PTH     0x12c
#define SPR3PTL     0x12e
#define SPR4PT      0x130
#define SPR4PTH     0x130
#define SPR4PTL     0x132
#define SPR5PT      0x134
#define SPR5PTH     0x134
#define SPR5PTL     0x136
#define SPR6PT      0x138
#define SPR6PTH     0x138
#define SPR6PTL     0x13a
#define SPR7PT      0x13c
#define SPR7PTH     0x13c
#define SPR7PTL     0x13e

#define SPR0POS     0x140
#define SPR0CTL     0x142
#define SPR0DATA    0x144
#define SPR0DATB    0x146
#define SPR1POS     0x148
#define SPR1CTL     0x14a
#define SPR1DATA    0x14c
#define SPR1DATB    0x14e
#define SPR2POS     0x150
#define SPR2CTL     0x152
#define SPR2DATA    0x154
#define SPR2DATB    0x156
#define SPR3POS     0x158
#define SPR3CTL     0x15a
#define SPR3DATA    0x15c
#define SPR3DATB    0x15e
#define SPR4POS     0x160
#define SPR4CTL     0x162
#define SPR4DATA    0x164
#define SPR4DATB    0x166
#define SPR5POS     0x168
#define SPR5CTL     0x16a
#define SPR5DATA    0x16c
#define SPR5DATB    0x16e
#define SPR6POS     0x170
#define SPR6CTL     0x172
#define SPR6DATA    0x174
#define SPR6DATB    0x176
#define SPR7POS     0x178
#define SPR7CTL     0x17a
#define SPR7DATA    0x17c
#define SPR7DATB    0x17e

#define COLOR_COUNT 32
#define COLOR_BASE  0x180
#define COLOR00     COLOR_BASE+0x00
#define COLOR01     COLOR_BASE+0x02
#define COLOR02     COLOR_BASE+0x04
#define COLOR03     COLOR_BASE+0x06
#define COLOR04     COLOR_BASE+0x08
#define COLOR05     COLOR_BASE+0x0A
#define COLOR06     COLOR_BASE+0x0C
#define COLOR07     COLOR_BASE+0x0E
#define COLOR08     COLOR_BASE+0x10
#define COLOR09     COLOR_BASE+0x12
#define COLOR10     COLOR_BASE+0x14
#define COLOR11     COLOR_BASE+0x16
#define COLOR12     COLOR_BASE+0x18
#define COLOR13     COLOR_BASE+0x1A
#define COLOR14     COLOR_BASE+0x1C
#define COLOR15     COLOR_BASE+0x1E
#define COLOR16     COLOR_BASE+0x20
#define COLOR17     COLOR_BASE+0x22
#define COLOR18     COLOR_BASE+0x24
#define COLOR19     COLOR_BASE+0x26
#define COLOR20     COLOR_BASE+0x28
#define COLOR21     COLOR_BASE+0x2A
#define COLOR22     COLOR_BASE+0x2C
#define COLOR23     COLOR_BASE+0x2E
#define COLOR24     COLOR_BASE+0x30
#define COLOR25     COLOR_BASE+0x32
#define COLOR26     COLOR_BASE+0x34
#define COLOR27     COLOR_BASE+0x36
#define COLOR28     COLOR_BASE+0x38
#define COLOR29     COLOR_BASE+0x3A
#define COLOR30     COLOR_BASE+0x3C
#define COLOR31     COLOR_BASE+0x3E

#define HTOTAL      0x1c0
#define HSSTOP      0x1c2
#define HBSTRT      0x1c4
#define HBSTOP      0x1c6
#define VTOTAL      0x1c8
#define VSSTOP      0x1ca
#define VBSTRT      0x1cc
#define VBSTOP      0x1ce
#define SPRHSTRT    0x1d0
#define SPRHSTOP    0x1d2
#define BPLHSTRT    0x1d4
#define BPLHSTOP    0x1d6
#define HHPOSW      0x1d8
#define HHPOSR      0x1da
#define BEAMCON0    0x1dc
#define HSSTRT      0x1de
#define VSSTRT      0x1e0
#define HCENTER     0x1e2
#define DIWHIGH     0x1e4
#define BPLHMOD     0x1e6
#define SPRHPT      0x1e8
#define SPRHPTH     0x1e8
#define SPRHPTL     0x1ea
#define BPLHPT      0x1ec
#define BPLHPTH     0x1ec
#define BPLHPTL     0x1ee
#define FMODE       0x1fc

#define BPLCON0F_ECSENA 0x0001
#define BPLCON0F_ERSY   0x0002
#define BPLCON0F_LACE   0x0004
#define BPLCON0F_LPEN   0x0008
#define BPLCON0F_BPU3   0x0010
#define BPLCON0F_BYPASS 0x0020
#define BPLCON0F_SHRES  0x0040
#define BPLCON0F_UHRES  0x0080
#define BPLCON0F_GAUD   0x0100
#define BPLCON0F_COLOR  0x0200
#define BPLCON0F_DPF    0x0400
#define BPLCON0F_HAM    0x0800
#define BPLCON0F_BPUx   0x7000      // mask (12..14)
#define BPLCON0F_HIRES  0x8000

#define BPLCON2F_PF1P       0x0007      // mask (0..2)
#define BPLCON2F_PF2P       0x0038      // mask (3..5)
#define BPLCON2F_PF2PRI     0x0040
#define BPLCON2F_SOGEN      0x0080
#define BPLCON2F_RDRAM      0x0100
#define BPLCON2F_KILLEHB    0x0200
#define BPLCON2F_ZDCTEN     0x0400
#define BPLCON2F_ZDBPEN     0x0800
#define BPLCON2F_ZDBPSEL    0x7000      // mask (12..14)

#define COPCONF_DANG        0x0002

#define DMACONF_AUDIOEN     0x000f       // mask (0..3)
#define DMACONF_AUD0EN      0x0001
#define DMACONF_AUD1EN      0x0002
#define DMACONF_AUD2EN      0x0004
#define DMACONF_AUD3EN      0x0008
#define DMACONF_DSKEN       0x0010
#define DMACONF_SPREN       0x0020
#define DMACONF_BLTEN       0x0040
#define DMACONF_COPEN       0x0080
#define DMACONF_BPLEN       0x0100
#define DMACONF_DMAEN       0x0200
#define DMACONF_BLTPRI      0x0400
#define DMACONF_BZERO       0x2000
#define DMACONF_BBUSY       0x4000
#define DMACONF_SETCLR      0x8000

#define POTGORF_START   0x0001
#define POTGORF_DATLX   0x0100
#define POTGORF_OUTLX   0x0200
#define POTGORF_DATLY   0x0400
#define POTGORF_OUTLY   0x0800
#define POTGORF_DATRX   0x1000
#define POTGORF_OUTRX   0x2000
#define POTGORF_DATRY   0x4000
#define POTGORF_OUTRY   0x8000


// Copper instructions
typedef UInt32  CopperInstruction;

#define COP_MOVE(reg, val)  (((reg) << 16) | (val))
#define COP_END()           0xfffffffe


// The supported interrupts
// http://amigadev.elowar.com/read/ADCD_2.1/Hardware_Manual_guide/node0142.html
// http://amigadev.elowar.com/read/ADCD_2.1/Hardware_Manual_guide/node0036.html
#define INTERRUPT_ID_CIA_B_FLAG                     23
#define INTERRUPT_ID_CIA_B_SP                       22
#define INTERRUPT_ID_CIA_B_ALARM                    21
#define INTERRUPT_ID_CIA_B_TIMER_B                  20
#define INTERRUPT_ID_CIA_B_TIMER_A                  19

#define INTERRUPT_ID_CIA_A_FLAG                     18
#define INTERRUPT_ID_CIA_A_SP                       17
#define INTERRUPT_ID_CIA_A_ALARM                    16
#define INTERRUPT_ID_CIA_A_TIMER_B                  15
#define INTERRUPT_ID_CIA_A_TIMER_A                  14

#define INTERRUPT_ID_EXTERN                         13
#define INTERRUPT_ID_DISK_SYNC                      12
#define INTERRUPT_ID_SERIAL_RECEIVE_BUFFER_FULL     11
#define INTERRUPT_ID_AUDIO3                         10
#define INTERRUPT_ID_AUDIO2                         9
#define INTERRUPT_ID_AUDIO1                         8
#define INTERRUPT_ID_AUDIO0                         7
#define INTERRUPT_ID_BLITTER                        6
#define INTERRUPT_ID_VERTICAL_BLANK                 5
#define INTERRUPT_ID_COPPER                         4
#define INTERRUPT_ID_PORTS                          3
#define INTERRUPT_ID_SOFT                           2
#define INTERRUPT_ID_DISK_BLOCK                     1
#define INTERRUPT_ID_SERIAL_TRANSMIT_BUFFER_EMPTY   0

#define INTERRUPT_ID_COUNT                          24


// Stops all hardware timers and DMAs and stops all interrupts of the platform's
// motherboard chipset.
extern void chipset_reset(void);
extern UInt8 chipset_get_version(void);
extern UInt8 chipset_get_ramsey_version(void);
extern Bool chipset_is_ntsc(void);
extern Byte* chipset_get_upper_dma_limit(Int chipset_version);

extern void chipset_enable_interrupt(Int interruptId);
extern void chipset_disable_interrupt(Int interruptId);


#define INTERRUPT_ID_QUANTUM_TIMER  INTERRUPT_ID_CIA_A_TIMER_B

extern void chipset_start_quantum_timer(void);
extern void chipset_stop_quantum_timer(void);
extern Int32 chipset_get_quantum_timer_duration_ns(void);
extern Int32 chipset_get_quantum_timer_elapsed_ns(void);

extern UInt32 chipset_get_hsync_counter(void);


//
// Zorro Bus
//

// Supported max number of expansion boards
#define EXPANSION_BOARDS_CAPACITY    16

// Expanion board types
#define EXPANSION_TYPE_RAM  0
#define EXPANSION_TYPE_IO   1

// Expansion bus types
#define EXPANSION_BUS_ZORRO_2   0
#define EXPANSION_BUS_ZORRO_3   1

// An expansion board
typedef struct _ExpansionBoard {
    Byte* _Nonnull  start;          // base address
    UInt            physical_size;  // size of memory space reserved for this board
    UInt            logical_size;   // size of memory space actually occupied by the board
    Int8            type;
    Int8            bus;
    Int8            slot;
    Int8            reserved;
    UInt16          manufacturer;
    UInt16          product;
    UInt32          serial_number;
    // Update lowmem.i if you add a new property here
} ExpansionBoard;

typedef struct _ExpansionBus {
    Int                 board_count;
    ExpansionBoard      board[EXPANSION_BOARDS_CAPACITY];
} ExpansionBus;

extern void zorro_auto_config(ExpansionBus* pExpansionBus);

#endif /* Platform_h */
