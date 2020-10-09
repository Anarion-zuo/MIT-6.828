#ifndef JOS_INC_SYSCALL_H
#define JOS_INC_SYSCALL_H

/* system call numbers */
enum {
	SYS_cputs = 0,
	SYS_cgetc,
	SYS_getenvid,
	SYS_env_destroy,
	SYS_page_alloc,
	SYS_page_map,
	SYS_page_unmap,
	SYS_exofork,
	SYS_env_set_status,
	SYS_env_set_pgfault_upcall,
	SYS_yield,
	SYS_ipc_try_send,
	SYS_ipc_recv,
	NSYSCALLS
};


static const char *syscall_name(int syscallno) {
    switch (syscallno) {
        case SYS_cputs:
            return "cputs";
        case SYS_getenvid:
            return "getenvid";
        case SYS_env_destroy:
            return "env_destroy";
        case SYS_yield:
            return "yield";
        case SYS_exofork:
            return "exofork";
        case SYS_env_set_status:
            return "env_set_status";
        case SYS_page_alloc:
            return "page_alloc";
        case SYS_page_map:
            return "page_map";
        case SYS_page_unmap:
            return "page_unmap";
        case SYS_env_set_pgfault_upcall:
            return "env_set_pgfault_upcall";
        case SYS_ipc_try_send:
            return "ipc_try_send";
        case SYS_ipc_recv:
            return "ipc_recv";
        default:
            return "invalid_syscall";
    }
}

#endif /* !JOS_INC_SYSCALL_H */
