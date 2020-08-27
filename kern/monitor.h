#ifndef JOS_KERN_MONITOR_H
#define JOS_KERN_MONITOR_H
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

struct Trapframe;

// Activate the kernel monitor,
// optionally providing a trap frame indicating the current state
// (NULL if none).
void monitor(struct Trapframe *tf);
void monitor_run(struct Trapframe *tf);
void monitor_welcome();

// Functions implementing monitor commands.
int mon_help(int argc, char **argv, struct Trapframe *tf);
int mon_kerninfo(int argc, char **argv, struct Trapframe *tf);
int mon_backtrace(int argc, char **argv, struct Trapframe *tf);
int mon_quitdebug(int argc, char **argv, struct Trapframe *tf);
int mon_printtrap(int argc, char **argv, struct Trapframe *tf);
int mon_traptrace(int argc, char **argv, struct Trapframe *tf);
int mon_trapcurtrace(int arg, char **argv, struct Trapframe *tf);

#endif	// !JOS_KERN_MONITOR_H
