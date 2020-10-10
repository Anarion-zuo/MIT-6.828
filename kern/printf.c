// Simple implementation of cprintf console output for the kernel,
// based on printfmt() and the kernel console's cputchar().

#include <inc/types.h>
#include <inc/stdio.h>
#include <inc/stdarg.h>
#include <kern/spinlock.h>

static struct spinlock printLock = {
    .locked = 0
};

void lock_print() {
    spin_lock(&printLock);
}

void unlock_print() {
    spin_unlock(&printLock);
}

static void
putch(int ch, int *cnt)
{
	cputchar(ch);
	*cnt++;
}

int
vcprintf(const char *fmt, va_list ap)
{
	int cnt = 0;
	vprintfmt((void*)putch, &cnt, fmt, ap);
	return cnt;
}

int
cprintf(const char *fmt, ...)
{
	va_list ap;
	int cnt;

    lock_print();

	va_start(ap, fmt);
	cnt = vcprintf(fmt, ap);
	va_end(ap);

	unlock_print();

	return cnt;
}

