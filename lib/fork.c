// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

static pte_t getpte(void *addr) {
    uintptr_t pdeIndex = PDX(addr);
    pde_t pde = uvpd[pdeIndex];
    if ((pde & PTE_P) == 0) {
        return 0;
    }
    return uvpt[PGNUM((uintptr_t)addr)];
}

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to aF
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
	/*
    cprintf("[%08x] real pgfault handler [%s, %s, %s] at addr 0x%lx eip 0x%lx\n",
            thisenv->env_id,
            err & 4 ? "user" : "kernel",
            err & 2 ? "write" : "read",
            err & 1 ? "protection" : "not-present",
            addr,
            utf->utf_eip);
            */
	if ((err & FEC_WR) == 0) {
        // not caused by write
        panic("User page fault at address 0x%lx eip 0x%lx not caused by writing!\n", addr, utf->utf_eip);
	}
	pte_t pte = getpte(addr);
	if (pte == 0) {
	    panic("Unmapped page present!\n");
	}
	// If the code gets here, the page is mapped.
	int perm = PTE_PERM(pte);
	if ((perm & PTE_COW) == 0) {
	    // not caused by cow page
	    panic("User page fault not caused in copy-on-write page!\n");
	}
	// If the code gets here, the page is cow.
	// setup new page permissions
	perm = PTE_P | PTE_U | PTE_W;

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
	int ret;
	// panic("pgfault not implemented");

	// allocate new page at PFTEMP
//    ret = sys_page_alloc(thisenv->env_id, PFTEMP, perm);
    ret = sys_page_alloc(0, PFTEMP, perm);
    if (ret < 0) {
        panic("Allocate new page for user page fault failed: %e!\n", ret);
    }
    // compute page begin
    addr = ROUNDDOWN(addr, 4096);
    // make copy
    memmove(PFTEMP, (void *)addr, PGSIZE);
    // map the page to target
//    ret = sys_page_map(thisenv->env_id, PFTEMP, thisenv->env_id, (void *)addr, perm);
    ret = sys_page_map(0, PFTEMP, 0, (void *)addr, perm);
    if (ret < 0) {
        panic("Mapping new page for user page fault failed: %e!\n", ret);
    }
    // unmap PFTEMP
//    ret = sys_page_unmap(thisenv->env_id, PFTEMP);
    ret = sys_page_unmap(0, PFTEMP);
    if (ret < 0) {
        panic("Unmap PFTEMP failed: %e!\n", ret);
    }
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
	// panic("duppage not implemented");
    int ret;

	void *pva = (void *) (pn << PTXSHIFT);
	if ((uint32_t)pva >= UTOP) {
	    // try meddling with address above UTOP
	    panic("duppage address too large\n");
	    return -E_INVAL;
	}
	pte_t pte = getpte(pva);
	if (pte == 0) {
	    return 0;
	}
    int perm = PTE_PERM(pte);
	if ((perm & PTE_U) == 0 || (perm & PTE_P) == 0) {
	    return -E_INVAL;
	}
	if ((perm & PTE_W) || (perm & PTE_COW)) {
	    // writable or cow page
	    ret = sys_page_map(thisenv->env_id, pva, envid, pva, PTE_COW | PTE_U | PTE_P);
	    if (ret < 0) {
	        return ret;
	    }
	    // must map this process table as well
	    ret = sys_page_map(thisenv->env_id, pva, thisenv->env_id, pva, PTE_COW | PTE_U | PTE_P);
        if (ret < 0) {
            return ret;
        }
	} else {
	    // read only page
        ret = sys_page_map(thisenv->env_id, pva, envid, pva, PTE_U | PTE_P);
        if (ret < 0) {
            return ret;
        }
        // does not map itself
    }
	return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	// panic("fork not implemented");
    int ret;
	// set page fault handler
	set_pgfault_handler(pgfault);
	// create empty child env
	envid_t newid = sys_exofork();
	if (newid == 0) {
	    // child process
	    envid_t childid = sys_getenvid();
	    thisenv = &envs[ENVX(childid)];
//	    cprintf("child envid [%08x]\n", childid);
	    // this return is for the child
	    return 0;
	}
//    cprintf("parent envid [%08x] child envid [%08x]\n", thisenv->env_id, newid);
    // parent process
    if (newid < 0) {
        panic("exofork failed %e\n", newid);
    }
	// allocate exception stack
    ret = sys_page_alloc(newid, (void *)(UXSTACKTOP - PGSIZE), PTE_U | PTE_W | PTE_P);
    if (ret < 0) {
        panic("allocate exception stack failed: %e\n", ret);
        return ret;
    }
    // set upcall
    ret = sys_env_set_pgfault_upcall(newid, thisenv->env_pgfault_upcall);
    if (ret < 0) {
        return ret;
    }
	// map address below UTOP
	for (int pn = UTEXT / PGSIZE; pn < USTACKTOP / PGSIZE; ++pn) {
	    ret = duppage(newid, pn);
	    if (ret < 0) {
	        panic("duppage: %e\n", ret);
	        return ret;
	    }
	}

	// mark child runnable
    ret = sys_env_set_status(newid, ENV_RUNNABLE);
	if (ret < 0) {
	    return ret;
	}

	// this return is for the parent
	return newid;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
