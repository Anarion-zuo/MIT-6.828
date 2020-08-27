// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>

#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/kdebug.h>
#include <kern/trap.h>

#define CMDBUF_SIZE	80	// enough for one VGA text line


struct Command {
	const char *name;
	const char *desc;
	// return -1 to force monitor to exit
	int (*func)(int argc, char** argv, struct Trapframe* tf);
};

static struct Command commands[] = {
	{ "help", "Display this list of commands", mon_help },
    { "kerninfo", "Display information about the kernel", mon_kerninfo },
    { "backtrace", "Trace back call stack", mon_backtrace },
    { "quit", "Exit kernel debug shell", mon_quitdebug },
    { "printtrap", "Print current TrapFrame", mon_printtrap },
    { "tracetrap", "Print trace of current Breakpoint", mon_trapcurtrace },
};

/***** Implementations of basic kernel monitor commands *****/

int
mon_help(int argc, char **argv, struct Trapframe *tf)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(commands); i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].desc);
	return 0;
}

int
mon_kerninfo(int argc, char **argv, struct Trapframe *tf)
{
	extern char _start[], entry[], etext[], edata[], end[];

	cprintf("Special kernel symbols:\n");
	cprintf("  _start                  %08x (phys)\n", _start);
	cprintf("  entry  %08x (virt)  %08x (phys)\n", entry, entry - KERNBASE);
	cprintf("  etext  %08x (virt)  %08x (phys)\n", etext, etext - KERNBASE);
	cprintf("  edata  %08x (virt)  %08x (phys)\n", edata, edata - KERNBASE);
	cprintf("  end    %08x (virt)  %08x (phys)\n", end, end - KERNBASE);
	cprintf("Kernel executable memory footprint: %dKB\n",
		ROUNDUP(end - entry, 1024) / 1024);
	return 0;
}

static int
print_curtrace(uint32_t ebp, uint32_t eip) {
    cprintf("ebp %x, eip %x\n", ebp, eip);
    struct Eipdebuginfo info;
    int ret = debuginfo_eip(eip, &info);
    cprintf("    %s: %d: %.*s+%d\n",
            info.eip_file, info.eip_line, info.eip_fn_namelen, info.eip_fn_name, eip - info.eip_fn_addr);
    return ret;
}

static int
print_backtrace(uint32_t ebp) {
    int *ebp_base_ptr = (int *)ebp;           // extract stack base
    uint32_t eip = ebp_base_ptr[1];   // extract just above this stack
    while (1) {
        /*
        cprintf("ebp %x, eip %x\n", ebp, eip);

//        int *args = ebp_base_ptr + 2;

//        for (int i = 0; i < 5; ++i) {
//            cprintf("%x ", args[i]);
//        }
//        cprintf("\n");

        struct Eipdebuginfo info;
        int ret = debuginfo_eip(eip, &info);
        if (ret) {
            break;
        }
        cprintf("    %s: %d: %.*s+%d\n",
                info.eip_file, info.eip_line, info.eip_fn_namelen, info.eip_fn_name, eip - info.eip_fn_addr);
    */
        if (print_curtrace(ebp, eip)) {
            break;
        }
        ebp = *ebp_base_ptr;
        ebp_base_ptr = (int *) ebp;
        eip = ebp_base_ptr[1];
    }
    return 0;
}

int mon_traptrace(int argc, char **argv, struct Trapframe *tf) {
    uint32_t ebp = tf->tf_regs.reg_ebp;
    return print_backtrace(ebp);
}

int mon_trapcurtrace(int arg, char **argv, struct Trapframe *tf) {
    uint32_t ebp = tf->tf_regs.reg_ebp;
    int *ebp_base_ptr = (int *)ebp;           // extract stack base
    uint32_t eip = ebp_base_ptr[1];   // extract just above this stack
    print_curtrace(ebp, eip);
    return 0;
}

int
mon_backtrace(int argc, char **argv, struct Trapframe *tf)
{
    typedef int (*this_func_type)(int, char **, struct Trapframe *);
	// Your code here.
	return print_backtrace(read_ebp());
}

int
mon_quitdebug(int argc, char **argv, struct Trapframe *tf) {
    cprintf("quitting...\n");
    return -1;
}

int
mon_printtrap(int argc, char **argv, struct Trapframe *tf) {
    if (tf == NULL) {
        cprintf("Null TrapFrame... Nothing to print...\n");
        return 0;
    }
    print_trapframe(tf);
    return 0;
}

/***** Kernel monitor command interpreter *****/

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

static int
runcmd(char *buf, struct Trapframe *tf)
{
	int argc;
	char *argv[MAXARGS];
	int i;

	// Parse the command buffer into whitespace-separated arguments
	argc = 0;
	argv[argc] = 0;
	while (1) {
		// gobble whitespace
		while (*buf && strchr(WHITESPACE, *buf))
			*buf++ = 0;
		if (*buf == 0)
			break;

		// save and scan past next arg
		if (argc == MAXARGS-1) {
			cprintf("Too many arguments (max %d)\n", MAXARGS);
			return 0;
		}
		argv[argc++] = buf;
		while (*buf && !strchr(WHITESPACE, *buf))
			buf++;
	}
	argv[argc] = 0;

	// Lookup and invoke the command
	if (argc == 0)
		return 0;
	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		if (strcmp(argv[0], commands[i].name) == 0)
			return commands[i].func(argc, argv, tf);
	}
	cprintf("Unknown command '%s'\n", argv[0]);
	return 0;
}

void
monitor_run(struct Trapframe *tf) {
    char *buf;
    while (1) {
        buf = readline("K> ");
        if (buf != NULL)
            if (runcmd(buf, tf) < 0)
                break;
    }
}

void monitor_welcome() {
    cprintf("Welcome to the JOS kernel monitor!\n");
    cprintf("Type 'help' for a list of commands.\n");
}

void
monitor(struct Trapframe *tf)
{
	monitor_welcome();
	if (tf != NULL)
		print_trapframe(tf);

	monitor_run(tf);
}
