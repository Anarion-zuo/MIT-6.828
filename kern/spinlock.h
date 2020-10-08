#ifndef JOS_INC_SPINLOCK_H
#define JOS_INC_SPINLOCK_H

#include <inc/types.h>
#include <kern/cpu.h>

// Comment this to disable spinlock debugging
//#define DEBUG_SPINLOCK

// Mutual exclusion lock.
struct spinlock {
	unsigned locked;       // Is the lock held?

#ifdef DEBUG_SPINLOCK
	// For debugging:
	char *name;            // Name of lock.
	struct CpuInfo *cpu;   // The CPU holding the lock.
	uintptr_t pcs[10];     // The call stack (an array of program counters)
	                       // that locked the lock.
#endif
};

void __spin_initlock(struct spinlock *lk, char *name);
void spin_lock(struct spinlock *lk);
void spin_unlock(struct spinlock *lk);

#define spin_initlock(lock)   __spin_initlock(lock, #lock)

extern struct spinlock kernel_lock;

static inline void
lock_kernel(void)
{
//    cprintf("[kernel] CPU %d fetching kernel lock\n", thiscpu->cpu_id);
	spin_lock(&kernel_lock);
//    cprintf("[kernel] CPU %d kernel lock fetched\n", thiscpu->cpu_id);
}

static inline void
unlock_kernel(void)
{
//    cprintf("[kernel] CPU %d release kernel lock\n", thiscpu->cpu_id);
    spin_unlock(&kernel_lock);

	// Normally we wouldn't need to do this, but QEMU only runs
	// one CPU at a time and has a long time-slice.  Without the
	// pause, this CPU is likely to reacquire the lock before
	// another CPU has even been given a chance to acquire it.
	asm volatile("pause");
}

#endif
