/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>



###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text
/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
    // this forms
    TRAPHANDLER_NOEC(t_divide, T_DIVIDE)
    TRAPHANDLER_NOEC(t_debug, T_DEBUG)
    TRAPHANDLER_NOEC(t_nmi, T_NMI)
    TRAPHANDLER_NOEC(t_brkpt, T_BRKPT)
    TRAPHANDLER_NOEC(t_oflow, T_OFLOW)
    TRAPHANDLER_NOEC(t_bound, T_BOUND)
    TRAPHANDLER_NOEC(t_illop, T_ILLOP)
    TRAPHANDLER_NOEC(t_device, T_DEVICE)
    TRAPHANDLER(t_dblflt, T_DBLFLT)
    TRAPHANDLER(t_tss, T_TSS)
    TRAPHANDLER(t_segnp, T_SEGNP)
    TRAPHANDLER(t_stack, T_STACK)
    TRAPHANDLER(t_gpflt, T_GPFLT)
    TRAPHANDLER(t_fperr, T_FPERR)
    TRAPHANDLER(t_align, T_ALIGN)
    TRAPHANDLER(t_mchk, T_MCHK)
    TRAPHANDLER(t_simderr, T_SIMDERR)


/*
 * Lab 3: Your code here for _alltraps
 */
 .global _alltraps
_alltraps:
    # entering function
    # pushl %ebp
    # mov %esp, %ebp

    # push trap frame
/*
    # stack grows downwards, therefore pushing should be in reverse declaration order
    xor %ebx, %ebx
    # uint16_t tf_padding4
    pushw %bx
    # uint16_t tf_ss
    pushw %ss
    # uintptr_t tf_esp
    pushl %esp
    # uint32_t tf_eflags
    pushl %ebx
    # uint16_t tf_padding3
    pushw %bx
    # uint16_t tf_cs
    pushw %cs
    # uintptr_t tf_eip
    pushl %ebx
    # uint32_t tf_err
    pushl 0x8(%ebp)
    # uint32_t tf_trapno
    pushl 0x4(%ebp)
    # uint16_t tf_padding2
    pushw %bx
    # uint16_t tf_ds
    pushw $(GD_KD)
    # uint16_t tf_padding1
    pushw %bx
    # uint16_t tf_es
    pushw $(GD_KD)
*/
    # struct PushRegs tf_regs
    # struct PushRegs is in reverse order declared

    pushl %ds
    pushl %es

    pushal
    # load GD_KD into %ds, %es
    movw $(GD_KD), %ax
    movw %ax, %ds
    movw %ax, %es
    # pass a pointer to the trap frame for function trap
    pushl %esp
    call trap
    # leaving function
    # pop %ebp
