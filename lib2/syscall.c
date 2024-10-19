//extern void sys_exit(int status);
//extern int sys_write(int fd, const void* buff, size_t count);

/**
 * x86_64 syscall interface
 * rax, rdx = (rax, rdi, rsi, rdx, r10, r8, r9)
 * 
 * c abi
 * rax = (rdi, rsi, rdx, rcx, r8, r9)
 * 
 */
#define size_t unsigned long long


static size_t syscall2(size_t id, size_t reg1, size_t reg2) {
	size_t rax asm("rax") = id;
	size_t rdi asm("rdi") = reg1;
	size_t rsi asm("rsi") = reg2;

	asm volatile(
		"syscall"
		: "+r" (rax)
		: "r" (rax), "r" (rdi), "r" (rsi)
	);

    return rax;
}

// static size_t syscall3(size_t id/*rdi*/, size_t reg1/*rsi*/, size_t reg2/*rdx*/, size_t reg3/*rcx*/) {
// 	asm("movq %rdi, %rax");	// rax = rdi
// 	asm("movq %rsi, %rdi"); // rdi = rsi
// 	asm("movq %rdx, %rsi");	// rsi = rdx
// 	asm("movq %rcx, %rdx"); // rdx = rcx
// 	asm("syscall");
// }

static size_t syscall3(size_t syscall_number, size_t arg1, size_t arg2, size_t arg3) {
	// move to the correct registers
	register size_t rax asm("rax") = syscall_number;
	register size_t rdi asm("rdi") = arg1;
	register size_t rsi asm("rsi") = arg2;
	register size_t rdx asm("rdx") = arg3;

	// make a syscall, outputting to rax, taking in implicit arguments rax, rdi, rsi and rdx
	asm volatile(
		"syscall"
		: "+r" (rax)
		: "r" (rax), "r" (rdi), "r" (rsi), "r" (rdx)
		: "memory"
	);

	return rax;
}


static size_t syscall1(size_t id/*rdi */, size_t reg1/*rsi */) {
	register size_t rax asm("rax") = id;
	register size_t rdi asm("rdi") = reg1;

	asm volatile("syscall"
		: "+r" (rax)
		: "r" (rax), "r" (rdi)
	); 

	return rax;
}
// x86-64        rdi   rsi   rdx   r10   r8    r9    -
static size_t syscall6(int id, size_t reg1, size_t reg2, size_t reg3, size_t reg4, size_t reg5, size_t reg6) {
	register size_t rax asm("rax") = id;
	register size_t rdi asm("rdi") = reg1;
	register size_t rsi asm("rsi") = reg2;
	register size_t rdx asm("rdx") = reg3;
	register size_t r10 asm("r10") = reg4;
	register size_t r8  asm("r8")  = reg5;
	register size_t r9  asm("r9")  = reg6; 

	// make a syscall, outputting to rax, taking in implicit arguments rax, rdi, rsi and rdx
	asm volatile(
		"syscall"
		: "+r" (rax)
		: "r" (rax), "r" (rdi), "r" (rsi), "r" (rdx), "r" (r10), "r" (r8), "r" (r9)
		: "memory"
	);

	return rax;
}


size_t sys_write(int fd, const void* string, size_t len) {
	size_t v = syscall3((size_t)1, (size_t)fd, (size_t)string, (size_t)len);
    return v;
}

void sys_exit(int status) {
	syscall1(60, status);
	
    return;
}

size_t sys_read(int fd, char* buffer, size_t count) {
    return syscall3(0,fd, (size_t) buffer, count);
}

size_t sys_brk(size_t brk) {
    size_t brk_val = syscall1(12,(size_t) brk);
    return (size_t) brk_val;
}


int sys_open(const char* pathname, int flags, int mode) {
	return (int) syscall3(2, pathname, flags, mode);
}

void *sys_mmap(void *addr, unsigned long long length, int prot, int flags, int fd, long offset) {
	void* ret = (void*) syscall6(9, (size_t) addr, (size_t)length, (size_t)prot, (size_t)flags, (size_t)fd, (size_t) offset);
	return ret; 
}