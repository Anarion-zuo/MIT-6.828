#include <inc/assert.h>
#include <inc/x86.h>
#include <kern/spinlock.h>
#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/monitor.h>

void sched_halt(void);

// Choose a user environment to run and run it.
void
sched_yield(void)
{
	struct Env *idle;

	// Implement simple round-robin scheduling.
	//
	// Search through 'envs' for an ENV_RUNNABLE environment in
	// circular fashion starting just after the env this CPU was
	// last running.  Switch to the first such environment found.
	//
	// If no envs are runnable, but the environment previously
	// running on this CPU is still ENV_RUNNING, it's okay to
	// choose that environment. Make sure curenv is not null before
	// dereferencing it.
	//
	// Never choose an environment that's currently running on
	// another CPU (env_status == ENV_RUNNING). If there are
	// no runnable environments, simply drop through to the code
	// below to halt the cpu.

	// LAB 4: Your code here.

//	cprintf("[kernel] CPU %d yielding current envid %08x\n", thiscpu->cpu_id, curenv ? curenv->env_id : -1);
/*
    idle = curenv;
    int idle_envid = (idle == NULL) ? -1 : ENVX(idle->env_id);
    int i;

    // from now on, kernel state is untouched, or must lock
    unlock_kernel();

    // before curenv
    for (i = idle_envid + 1; i < NENV; i++) {
        if (envs[i].env_status == ENV_RUNNABLE) {
            lock_kernel();
            if (envs[i].env_status == ENV_RUNNABLE) {
                env_run(&envs[i]);
            } else {
                unlock_kernel();
            }
        }
    }

    // after curenv
    for (i = 0; i < idle_envid; i++) {
        if (envs[i].env_status == ENV_RUNNABLE) {
            lock_kernel();
            if (envs[i].env_status == ENV_RUNNABLE) {
                env_run(&envs[i]);
            } else {
                unlock_kernel();
            }
        }
    }

    if (idle != NULL && idle->env_status == ENV_RUNNING) {
        lock_kernel();
        if (idle != NULL && idle->env_status == ENV_RUNNING) {
            env_run(idle);
        }
    }
*/
    uint32_t curenvIndex = (curenv?ENVX(curenv->env_id):0), offset; // curenv might be NULL!
    for (offset = 0; offset < NENV; ++offset) {
        uint32_t realIndex = (curenvIndex + offset) % NENV;
        if(envs[realIndex].env_status == ENV_RUNNABLE){
            env_run(&envs[realIndex]); // switch to the first runnable environment.env_run will never return.
        }
    }

    if (curenv && curenv->env_status == ENV_RUNNING) {
        // no envs are runnable,but the environment previously running on this CPU is still running.
        // It's okay to choose this environment.
        env_run(curenv);
    }
	// sched_halt never returns
	sched_halt();
}

// Halt this CPU when there is nothing to do. Wait until the
// timer interrupt wakes it up. This function never returns.
//
void
sched_halt(void)
{
	int i;

	// For debugging and testing purposes, if there are no runnable
	// environments in the system, then drop into the kernel monitor.
	for (i = 0; i < NENV; i++) {
		if ((envs[i].env_status == ENV_RUNNABLE ||
		     envs[i].env_status == ENV_RUNNING ||
		     envs[i].env_status == ENV_DYING))
			break;
	}
	if (i == NENV) {
		cprintf("No runnable environments in the system!\n");
		while (1)
			monitor(NULL);
	}

	// Mark that no environment is running on this CPU
	curenv = NULL;
	lcr3(PADDR(kern_pgdir));

	// Mark that this CPU is in the HALT state, so that when
	// timer interupts come in, we know we should re-acquire the
	// big kernel lock
	xchg(&thiscpu->cpu_status, CPU_HALTED);

//	cprintf("[kernel] CPU %d waiting on new envid\n", thiscpu->cpu_id);
	// Release the big kernel lock as if we were "leaving" the kernel
	unlock_kernel();

	// Reset stack pointer, enable interrupts and then halt.
	asm volatile (
		"movl $0, %%ebp\n"
		"movl %0, %%esp\n"
		"pushl $0\n"
		"pushl $0\n"
		// Uncomment the following line after completing exercise 13
		"sti\n"
		"1:\n"
		"hlt\n"
		"jmp 1b\n"
	: : "a" (thiscpu->cpu_ts.ts_esp0));
}

