#include <inc/mmu.h>
#include <inc/x86.h>
#include <inc/assert.h>

#include <kern/pmap.h>
#include <kern/trap.h>
#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/env.h>
#include <kern/syscall.h>
#include <kern/sched.h>
#include <kern/kclock.h>
#include <kern/picirq.h>
#include <kern/cpu.h>
#include <kern/spinlock.h>

#include <inc/string.h>

//static struct Taskstate ts;

/* For debugging, so print_trapframe can distinguish between printing
 * a saved trapframe and printing the current trapframe and print some
 * additional information in the latter case.
 */
static struct Trapframe *last_tf;

/* Interrupt descriptor table.  (Must be built at run time because
 * shifted function addresses can't be represented in relocation records.)
 */
struct Gatedesc idt[256] = { { 0 } };
struct Pseudodesc idt_pd = {
	sizeof(idt) - 1, (uint32_t) idt
};


static const char *trapname(int trapno)
{
	static const char * const excnames[] = {
		"Divide error",
		"Debug",
		"Non-Maskable Interrupt",
		"Breakpoint",
		"Overflow",
		"BOUND Range Exceeded",
		"Invalid Opcode",
		"Device Not Available",
		"Double Fault",
		"Coprocessor Segment Overrun",
		"Invalid TSS",
		"Segment Not Present",
		"Stack Fault",
		"General Protection",
		"Page Fault",
		"(unknown trap)",
		"x87 FPU Floating-Point Error",
		"Alignment Check",
		"Machine-Check",
		"SIMD Floating-Point Exception"
	};

    static const char * const irqnames[] = {
            "Hardware Interrupt Timer",
            "Hardware Interrupt KBD",
            "","",
            "Hardware Interrupt Serial",
            "", "",
            "Hardware Interrupt Spurious",
            "", "", "", "", "", "",
            "Hardware Interrupt IDE",
            "", "", "", "",
            "Hardware Interrupt ERROR",
    };

	if (trapno < ARRAY_SIZE(excnames))
		return excnames[trapno];
	if (trapno == T_SYSCALL)
		return "System call";
	if (trapno >= IRQ_OFFSET && trapno < IRQ_OFFSET + 16)
		return irqnames[trapno - IRQ_OFFSET];
	return "(unknown trap)";
}

#define DECLARE_TRAPENTRY(func_name, entry_num, privLevel) \
    extern void func_name();                \
    SETGATE(idt[entry_num], 1, GD_KT, &func_name, privLevel)

#define DECLARE_INTENTRY(funcName, intNumber, privLevel) \
    extern void funcName();                \
    SETGATE(idt[intNumber], 0, GD_KT, &funcName, privLevel)

static bool is_irq(uint32_t trapno) {
    return trapno >= IRQ_OFFSET && trapno < IRQ_OFFSET + 16;
}

void
trap_init(void)
{
	extern struct Segdesc gdt[];

	// LAB 3: Your code here.
	/*
	 * This is a test for IDT setup.
	 * Only Divide-by-0, indexed by 0, is set to the idt.
	 * Args to macro after idt[]:
	 *  - set this to be a trap.
	 *  - code segment selector in gdt is 1, according to boot/boot.S.
	 *  - offset in code segment is function address of the handler.
	 *  - set handler privilege level to be kernel level.
	 */
    DECLARE_INTENTRY(t_divide, T_DIVIDE, 0)
    DECLARE_INTENTRY(t_debug, T_DEBUG, 3)
    DECLARE_INTENTRY(t_nmi, T_NMI, 0)
    DECLARE_INTENTRY(t_brkpt, T_BRKPT, 3)
    DECLARE_INTENTRY(t_oflow, T_OFLOW, 0)
    DECLARE_INTENTRY(t_bound, T_BOUND, 0)
    DECLARE_INTENTRY(t_illop, T_ILLOP, 0)
    DECLARE_INTENTRY(t_device, T_DEVICE, 0)
    DECLARE_INTENTRY(t_dblflt, T_DBLFLT, 0)
    DECLARE_INTENTRY(t_tss, T_TSS, 0)
    DECLARE_INTENTRY(t_segnp, T_SEGNP, 0)
    DECLARE_INTENTRY(t_stack, T_STACK, 0)
    DECLARE_INTENTRY(t_gpflt, T_GPFLT, 0)
    DECLARE_INTENTRY(t_pgflt, T_PGFLT, 3)
    DECLARE_INTENTRY(t_fperr, T_FPERR, 0)
    DECLARE_INTENTRY(t_align, T_ALIGN, 0)
    DECLARE_INTENTRY(t_mchk, T_MCHK, 0)
    DECLARE_INTENTRY(t_simderr, T_SIMDERR, 0)

    DECLARE_INTENTRY(t_syscall, T_SYSCALL, 3)

    DECLARE_INTENTRY(irq_timer, IRQ_TIMER + IRQ_OFFSET, 0)
    DECLARE_INTENTRY(irq_kbd, IRQ_KBD + IRQ_OFFSET, 0)
    DECLARE_INTENTRY(irq_serial, IRQ_SERIAL + IRQ_OFFSET, 0)
    DECLARE_INTENTRY(irq_spurious, IRQ_SPURIOUS + IRQ_OFFSET, 0)
    DECLARE_INTENTRY(irq_ide, IRQ_IDE + IRQ_OFFSET, 0)
    DECLARE_INTENTRY(irq_error, IRQ_ERROR + IRQ_OFFSET, 0)

    // Per-CPU setup
	trap_init_percpu();
}

// Initialize and load the per-CPU TSS and IDT
void
trap_init_percpu(void)
{
	// The example code here sets up the Task State Segment (TSS) and
	// the TSS descriptor for CPU 0. But it is incorrect if we are
	// running on other CPUs because each CPU has its own kernel stack.
	// Fix the code so that it works for all CPUs.
	//
	// Hints:
	//   - The macro "thiscpu" always refers to the current CPU's
	//     struct CpuInfo;
	//   - The ID of the current CPU is given by cpunum() or
	//     thiscpu->cpu_id;
	//   - Use "thiscpu->cpu_ts" as the TSS for the current CPU,
	//     rather than the global "ts" variable;
	//   - Use gdt[(GD_TSS0 >> 3) + i] for CPU i's TSS descriptor;
	//   - You mapped the per-CPU kernel stacks in mem_init_mp()
	//   - Initialize cpu_ts.ts_iomb to prevent unauthorized environments
	//     from doing IO (0 is not the correct value!)
	//
	// ltr sets a 'busy' flag in the TSS selector, so if you
	// accidentally load the same TSS on more than one CPU, you'll
	// get a triple fault.  If you set up an individual CPU's TSS
	// wrong, you may not get a fault until you try to return from
	// user space on that CPU.
	//
	// LAB 4: Your code here:

	// Setup a TSS so that we get the right stack
	// when we trap to the kernel.
	struct Taskstate *ts = &thiscpu->cpu_ts;
	ts->ts_esp0 = KSTACKTOP - thiscpu->cpu_id * (KSTKSIZE + KSTKGAP);
	ts->ts_ss0 = GD_KD;
	ts->ts_iomb = sizeof(struct Taskstate);

	// Initialize the TSS slot of the gdt.
	gdt[(GD_TSS0 >> 3) + thiscpu->cpu_id] = SEG16(STS_T32A, (uint32_t) (ts),
					sizeof(struct Taskstate) - 1, 0);
	gdt[(GD_TSS0 >> 3) + thiscpu->cpu_id].sd_s = 0;

	// Load the TSS selector (like other segment selectors, the
	// bottom three bits are special; we leave them 0)
	ltr(GD_TSS0 + (thiscpu->cpu_id << 3));

	// Load the IDT
	lidt(&idt_pd);
}

void
print_trapframe(struct Trapframe *tf)
{
	cprintf("TRAP frame at %p from CPU %d\n", tf, cpunum());
	print_regs(&tf->tf_regs);
	cprintf("  es   0x----%04x\n", tf->tf_es);
	cprintf("  ds   0x----%04x\n", tf->tf_ds);
	cprintf("  trap 0x%08x %s\n", tf->tf_trapno, trapname(tf->tf_trapno));
	// If this trap was a page fault that just happened
	// (so %cr2 is meaningful), print the faulting linear address.
	if (tf == last_tf && tf->tf_trapno == T_PGFLT)
		cprintf("  cr2  0x%08x\n", rcr2());
	cprintf("  err  0x%08x", tf->tf_err);
	// For page faults, print decoded fault error code:
	// U/K=fault occurred in user/kernel mode
	// W/R=a write/read caused the fault
	// PR=a protection violation caused the fault (NP=page not present).
	if (tf->tf_trapno == T_PGFLT)
		cprintf(" [%s, %s, %s]\n",
			tf->tf_err & 4 ? "user" : "kernel",
			tf->tf_err & 2 ? "write" : "read",
			tf->tf_err & 1 ? "protection" : "not-present");
	else
		cprintf("\n");
	cprintf("  eip  0x%08x\n", tf->tf_eip);
	cprintf("  cs   0x----%04x\n", tf->tf_cs);
	cprintf("  flag 0x%08x\n", tf->tf_eflags);
	if ((tf->tf_cs & 3) != 0) {
		cprintf("  esp  0x%08x\n", tf->tf_esp);
		cprintf("  ss   0x----%04x\n", tf->tf_ss);
	}
}

void
print_regs(struct PushRegs *regs)
{
	cprintf("  edi  0x%08x\n", regs->reg_edi);
	cprintf("  esi  0x%08x\n", regs->reg_esi);
	cprintf("  ebp  0x%08x\n", regs->reg_ebp);
	cprintf("  oesp 0x%08x\n", regs->reg_oesp);
	cprintf("  ebx  0x%08x\n", regs->reg_ebx);
	cprintf("  edx  0x%08x\n", regs->reg_edx);
	cprintf("  ecx  0x%08x\n", regs->reg_ecx);
	cprintf("  eax  0x%08x\n", regs->reg_eax);
}

static void handle_divzero() {
//    cprintf("exception: divide by zero encoutered.\n");
}

extern int print_backtrace(uint32_t ebp);

static void debug_breakpoint_handler(struct Trapframe *tf) {
    print_backtrace(tf->tf_regs.reg_ebp);
//    print_trapframe(tf);
//    cprintf("Breakpoint at:\n");
//    monitor_welcome();
//    monitor_run(tf);
}

static void handle_syscall(struct Trapframe *tf) {
    // this function extracts registers from Trapframe and passes them onto real syscall dispatcher
    struct PushRegs *pushRegs = &tf->tf_regs;
    pushRegs->reg_eax = syscall(pushRegs->reg_eax, pushRegs->reg_edx, pushRegs->reg_ecx, pushRegs->reg_ebx, pushRegs->reg_edi, pushRegs->reg_esi);
}

static void irq_dispatch(struct Trapframe *tf, uint32_t irqno) {
    switch (irqno) {
        case IRQ_TIMER:
            break;
        case IRQ_SPURIOUS:
            // Handle spurious interrupts
            // The hardware sometimes raises these because of noise on the
            // IRQ line or other reasons. We don't care.
//            cprintf("Spurious interrupt on irq 7\n");
            print_trapframe(tf);
            return;
        default:
            print_trapframe(tf);
            if (tf->tf_cs == GD_KT)
                panic("unhandled trap in kernel");
            else {
                env_destroy(curenv);
                return;
            }
    }
}

//void _trapexit_kernel(struct KTrapframe *);

static void
trap_exit(struct Trapframe *tf) {
    cprintf("[kernel] exiting trap handler ");
    if ((tf->tf_cs & 3) == 3) {
        cprintf("to user\n");
        // exit to user
        if (curenv && curenv->env_status == ENV_RUNNING)
            env_run(curenv);
        else
            sched_yield();
    }
    cprintf("to kernel\n");
    struct KTrapframe ktf;
    ktf.ktf_regs = tf->tf_regs;
    ktf.ktf_eip = tf->tf_eip;
    ktf.ktf_eflags = tf->tf_eflags;
    ktf.ktf_esp = tf->tf_esp;
    asm volatile(
        "\tmovl %0,%%esp\n"
        "\tpopal\n"
        "\tpopf\n"
        "\tmovl (%%esp), %%esp\n"
        "\tret\n"
        : : "g" (&ktf) : "memory");
//    _trapexit_kernel(&ktf);
}

static void
trap_dispatch(struct Trapframe *tf)
{
	// Handle processor exceptions.
	// LAB 3: Your code here.
    uint32_t trapno = tf->tf_trapno;
    switch (trapno) {
        case T_DIVIDE:
            //return;
//            cprintf("[kernel] user process divide by 0 error caught\n");
            env_destroy(curenv);
            return;
        case T_DEBUG:
            debug_breakpoint_handler(tf);
            return ;
        case T_BRKPT:
            debug_breakpoint_handler(tf);
            return;
        case T_PGFLT:
            page_fault_handler(tf);
            return;
        case T_SYSCALL:
            handle_syscall(tf);
            return;
        default:
//            cprintf("trap caught! number %u\n", tf->tf_trapno);
            break;
    }

    /*
    if (is_irq(trapno)) {
        irq_dispatch(tf, trapno - IRQ_OFFSET);
    }
     */
    // Handle clock interrupts. Don't forget to acknowledge the
    // interrupt using lapic_eoi() before calling the scheduler!
    // LAB 4: Your code here.
    if (trapno == IRQ_OFFSET + IRQ_TIMER) {
        if ((tf->tf_cs & 3) == 3) {
//            cprintf("[kernel] CPU %d interrupt user envid %d by timer\n", thiscpu->cpu_id, curenv ? curenv->env_id : -1);
//                curenv->env_status = ENV_RUNNABLE;
        } else {
//            cprintf("[kernel] CPU %d interrupt by timer when waiting on new env\n", thiscpu->cpu_id);
        }
        lapic_eoi();
        sched_yield();
    }

	// Handle keyboard and serial interrupts.
	// LAB 5: Your code here.

	// Unexpected trap: The user process or the kernel has a bug.
	print_trapframe(tf);
	if (tf->tf_cs == GD_KT)
		panic("unhandled trap in kernel");
	else {
		env_destroy(curenv);
		return;
	}
}

void
trap(struct Trapframe *tf)
{
    // The environment may have set DF and some versions
	// of GCC rely on DF being clear
	asm volatile("cld" ::: "cc");

	// Halt the CPU if some other CPU has called panic()
	extern char *panicstr;
	if (panicstr)
		asm volatile("hlt");

	// Re-acqurie the big kernel lock if we were halted in
	// sched_yield()
	if (xchg(&thiscpu->cpu_status, CPU_STARTED) == CPU_HALTED)
		lock_kernel();
//	cprintf("[kernel] CPU %d trapped as interrupt %d %s from [%s] eip 0x%lx\n", thiscpu->cpu_id, tf->tf_trapno, trapname(tf->tf_trapno), (tf->tf_cs & 3) == 3 ? "user" : "kernel", tf->tf_eip);
	// Check that interrupts are disabled.  If this assertion
	// fails, DO NOT be tempted to fix it by inserting a "cli" in
	// the interrupt path.
	/*
	if (!(read_eflags() & FL_IF)) {
	    assert(thiscpu->cpu_status != CPU_HALTED);
	}
	 */
    assert(!(read_eflags() & FL_IF));
    /*
    uint32_t eflags = read_eflags();
    if (!(eflags & FL_IF)) {
        if ((tf->tf_cs & 3) == 3) {
            // user mode enables interrupt
        } else {
            // kernel mode ignores interrupt
            if (is_irq(tf->tf_trapno)) {
                cprintf("[kernel] ignores hardware interrupt %s\n", trapname(tf->tf_trapno));
                trap_exit(tf);
                return;
            }
            panic("Interrupt not disabled in kernel mode");
        }
	}
     */

	if ((tf->tf_cs & 3) == 3) {
		// Trapped from user mode.
		// Acquire the big kernel lock before doing any
		// serious kernel work.
		// LAB 4: Your code here.

		lock_kernel();

		assert(curenv);

		// Garbage collect if current enviroment is a zombie
		if (curenv->env_status == ENV_DYING) {
//		    cprintf("[kernel] CPU %d collecting dying envid %08x\n", thiscpu->cpu_id, curenv->env_id);
			env_free(curenv);
			curenv = NULL;
			sched_yield();
		}

		// Copy trap frame (which is currently on the stack)
		// into 'curenv->env_tf', so that running the environment
		// will restart at the trap point.
		curenv->env_tf = *tf;
		// The trapframe on the stack should be ignored from here on.
		tf = &curenv->env_tf;
	}

	// Record that tf is the last real trapframe so
	// print_trapframe can print some additional information.
	last_tf = tf;

	// Dispatch based on what type of trap occurred
	trap_dispatch(tf);

	// If we made it to this point, then no other environment was
	// scheduled, so we should return to the current environment
	// if doing so makes sense.
//	trap_exit(tf);
    if (curenv && curenv->env_status == ENV_RUNNING)
        env_run(curenv);
    else
        sched_yield();
}

static int va_in_exceptionstack(void *va) {
    return (uint32_t)va <= UXSTACKTOP && (uint32_t)va > UXSTACKTOP - PGSIZE;
}

static void page_fault_exit(uint32_t fault_va, struct Trapframe *tf) {
    // Destroy the environment that caused the fault.
    cprintf("[%08x] user fault va %08x ip %08x\n",
            curenv->env_id, fault_va, tf->tf_eip);
    print_trapframe(tf);
    env_destroy(curenv);
}

extern void _pgfault_upcall(void);

void
page_fault_handler(struct Trapframe *tf)
{
	uint32_t fault_va;

	// Read processor's CR2 register to find the faulting address
	fault_va = rcr2();

	// Handle kernel-mode page faults.
    // LAB 3: Your code here.

    if ((tf->tf_cs & 3) != 3) {
        // tf comes from kernel mode
        cprintf("kernel fault va %08x ip %08x\n",
                fault_va, tf->tf_eip);
	    panic("Page fault in kernel mode!\n");
	}

	// We've already handled kernel-mode exceptions, so if we get here,
	// the page fault happened in user mode.

	// Call the environment's page fault upcall, if one exists.  Set up a
	// page fault stack frame on the user exception stack (below
	// UXSTACKTOP), then branch to curenv->env_pgfault_upcall.
	//
	// The page fault upcall might cause another page fault, in which case
	// we branch to the page fault upcall recursively, pushing another
	// page fault stack frame on top of the user exception stack.
	//
	// It is convenient for our code which returns from a page fault
	// (lib/pfentry.S) to have one word of scratch space at the top of the
	// trap-time stack; it allows us to more easily restore the eip/esp. In
	// the non-recursive case, we don't have to worry about this because
	// the top of the regular user stack is free.  In the recursive case,
	// this means we have to leave an extra word between the current top of
	// the exception stack and the new stack frame because the exception
	// stack _is_ the trap-time stack.
	//
	// If there's no page fault upcall, the environment didn't allocate a
	// page for its exception stack or can't write to it, or the exception
	// stack overflows, then destroy the environment that caused the fault.
	// Note that the grade script assumes you will first check for the page
	// fault upcall and print the "user fault va" message below if there is
	// none.  The remaining three checks can be combined into a single test.
	//
	// Hints:
	//   user_mem_assert() and env_run() are useful here.
	//   To change what the user environment runs, modify 'curenv->env_tf'
	//   (the 'tf' variable points at 'curenv->env_tf').

	// LAB 4: Your code here.
	int ret;

    // check user permissions
    /*
    ret = user_mem_check(curenv, (void *)fault_va, PGSIZE, PTE_U | PTE_W);
    if (ret < 0) {
        page_fault_exit(fault_va, tf);
        return;
    }
     */

    if (curenv->env_pgfault_upcall == NULL) {
        // no upcall
        page_fault_exit(fault_va, tf);
        return;
    }
    if (tf->tf_esp > USTACKTOP && tf->tf_esp <= UXSTACKTOP - PGSIZE) {
        // exception stack out of space
        page_fault_exit(fault_va, tf);
        return;
    }
//    cprintf("user fault 0x%lx eip 0x%lx esp 0x%lx\n", fault_va, tf->tf_eip, tf->tf_esp);
//    print_trapframe(tf);
    // setup env

    // setup stack pointer
//    envtf->tf_esp -= 4;
//    *(uint32_t *)envtf->tf_esp = envtf->tf_eip;
    struct UTrapframe *utf = NULL;
    if (tf->tf_esp < UXSTACKTOP && tf->tf_esp >= UXSTACKTOP - PGSIZE) {
        // must leave empty word for recursive faults
        utf = (struct UTrapframe *)(tf->tf_esp - sizeof(struct UTrapframe) - 4);
    } else {
        // not recursive faults
        utf = (struct UTrapframe *)(UXSTACKTOP - sizeof(struct UTrapframe));
    }
//    cprintf(" [%s, %s, %s]\n",
//            utf->utf_err & 4 ? "user" : "kernel",
//            utf->utf_err & 2 ? "write" : "read",
//            utf->utf_err & 1 ? "protection" : "not-present");
//    cprintf("user_mem_assert utf 0x%lx envid [%08x] fault_va 0x%lx %s %s %s\n", utf, curenv->env_id, fault_va, tf->tf_err & 2 ? "write" : "read", tf->tf_err & 4 ? "user" : "kernel", tf->tf_err & 1 ? "protection" : "not-present");
    user_mem_assert(curenv, utf, sizeof(struct UTrapframe), PTE_W);
    // pass struct UTrapframe as arguments
    // fault info
    utf->utf_fault_va = fault_va;
    utf->utf_err = tf->tf_err;
    // return states
    utf->utf_regs = tf->tf_regs;
    utf->utf_regs = tf->tf_regs;
    utf->utf_eip = tf->tf_eip;
    utf->utf_eflags = tf->tf_eflags;
    utf->utf_esp = tf->tf_esp;
    // new env run
    tf->tf_eip = (uintptr_t)curenv->env_pgfault_upcall;
    tf->tf_esp = (uintptr_t)utf;
//    cprintf("eip 0x%lx esp 0x%lx\n", tf->tf_esp, tf->tf_eip);
//    print_trapframe(envtf);
    // run env
    env_run(curenv);
}

