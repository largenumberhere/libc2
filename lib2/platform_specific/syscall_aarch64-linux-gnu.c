//extern void sys_exit(int status);
//extern int sys_write(int fd, const void* buff, size_t count);


#define size_t unsigned long long
#define NULL (void*)0

static size_t syscall6(int id, size_t reg1, size_t reg2, size_t reg3, size_t reg4, size_t reg5, size_t reg6) {
	/**
	 * x86_64 syscall interface
	 * rax, rdx = (rax, rdi, rsi, rdx, r10, x8, r9)
	 * 
	 * c abi
	 * rax = (rdi, rsi, rdx, rcx, x8, r9)
	 * 
	 */
	
//	register size_t rax asm("rax") = id; register size_t rdi asm("rdi") = reg1; register size_t rsi asm("rsi") = reg2; register size_t rdx asm("rdx") = reg3; register size_t r10 asm("r10") = reg4; register size_t x8 asm("x8") = reg5; register size_t r9 
//	asm("r9") = reg6; make a syscall, outputting to rax, taking in implicit arguments rax, rdi, rsi and rdx asm volatile( "syscall" "+r" (rax) "r" (rax), "r" (rdi), "r" (rsi), "r" (rdx), "r" (r10), "r" (x8), "r" (r9) "memory" );

	// arm 64
	volatile register size_t x8 asm("x8") = (size_t) id;

	volatile register size_t x0 asm("x0") = (size_t) reg1;
	volatile register size_t x1 asm("x1") = (size_t) reg2;
	volatile register size_t x2 asm("x2") = (size_t) reg3;
	volatile register size_t x3 asm("x3") = (size_t) reg4;
	volatile register size_t x4 asm("x4") = (size_t) reg5;
	volatile register size_t x5 asm("x5") = (size_t) reg6;


	asm volatile("svc 0"
		: "+r" (x0)
		: "r" (x8), "r" (x0), "r" (x1), "r" (x2), "r" (x3), "r" (x4), "r" (x5)
		: "memory"
	);


	return x0;
}

static size_t syscall2(size_t id, size_t reg1, size_t reg2) {
	return syscall6(id, reg1, reg2, NULL, NULL, NULL, NULL);
}

static size_t syscall1(size_t id, size_t reg1) {
	return syscall6(id, reg1, NULL, NULL, NULL, NULL, NULL);
}

static size_t syscall3(size_t id, size_t reg1, size_t reg2, size_t reg3) {
	return syscall6(id, reg1, reg2, reg3, NULL, NULL, NULL);
}

static size_t syscall4(size_t id, size_t reg1, size_t reg2, size_t reg3, size_t reg4) {
	return syscall6(id, reg1, reg2, reg3, reg4, (size_t)NULL, (size_t)NULL);
}



size_t sys_write(int fd, const void* string, size_t len) {
	size_t v = syscall3((size_t)64, (size_t)fd, (size_t)string, (size_t)len);
    return v;
}

void sys_exit(int status) {
	syscall1(93, status);
	
    return;
}

size_t sys_read(int fd, char* buffer, size_t count) {
    return syscall3(63, (size_t) fd, (size_t) buffer, (size_t) count);
}

size_t sys_brk(size_t brk) {
    size_t brk_val = syscall1(214,(size_t) brk);
    return (size_t) brk_val;
}


//https://sites.uclouvain.be/SystInfo/usr/include/linux/fcntl.h.html
#define AT_FDCWD                -100    /* Special value used to indicate
                                           openat should use the current
                                           working directory. */

int sys_open(const char* pathname, int flags, int mode) {
	return (int) syscall4(56, AT_FDCWD, pathname, flags, mode);
}



void *sys_mmap(void *addr, unsigned long long length, int prot, int flags, int fd, long offset) {
	void* ret = (void*) syscall6(222, (size_t) addr, (size_t)length, (size_t)prot, (size_t)flags, (size_t)fd, (size_t) offset);
	return ret; 
}

int sys_munmap(void* addr, size_t length) {
	// int ret = (int) syscall2(11, (unsigned long)addr, (size_t)length);
	// return ret;
	// volatile register size_t x0 asm("x0") = (size_t) 11;
  	// volatile register size_t x1 asm("x1") = (unsigned long) addr;
  	// volatile register size_t x2 asm("x2") = (size_t) length;

	// asm volatile("svc 0"
	// :"+r" (x0)
	// :"r" (x0), "r" (x1), "r" (x2)
	// : "memory"
	// );


	return (int) syscall2(215, (size_t)addr, (size_t)length);
}

int sys_close(int fd) {
	return syscall1(57, fd);
}
