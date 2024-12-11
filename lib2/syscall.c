//extern void sys_exit(int status);
//extern int sys_write(int fd, const void* buff, size_t count);


#define size_t unsigned long long
#define NULL (void*)0

static size_t syscall6(int id, size_t reg1, size_t reg2, size_t reg3, size_t reg4, size_t reg5, size_t reg6) {
	register size_t rax asm("rax") = id;
	register size_t rdi asm("rdi") = reg1;
	register size_t rsi asm("rsi") = reg2;
	register size_t rdx asm("rdx") = reg3;
	register size_t r10 asm("r10") = reg4;
	register size_t r8  asm("r8")  = reg5;
	register size_t r9  asm("r9")  = reg6; 

	asm volatile(
		"syscall"
		: "+r" (rax)
		: "r" (rax), "r" (rdi), "r" (rsi), "r" (rdx), "r" (r10), "r" (r8), "r" (r9)
		: "memory"
	);

	return rax;
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
	size_t v = syscall3(1, (size_t)fd, (size_t)string, (size_t)len);
    return v;
}

void sys_exit(int status) {
	syscall1(60, status);
	
    return;
}

size_t sys_read(int fd, char* buffer, size_t count) {
    return syscall3(0, (size_t) fd, (size_t) buffer, (size_t) count);
}

size_t sys_brk(size_t brk) {
    size_t brk_val = syscall1(12,(size_t) brk);
    return (size_t) brk_val;
}


//https://sites.uclouvain.be/SystInfo/usr/include/linux/fcntl.h.html
#define AT_FDCWD                -100    /* Special value used to indicate
                                           openat should use the current
                                           working directory. */

int sys_open(const char* pathname, int flags, int mode) {
	return (int) syscall3(2, pathname, flags, mode);
}



void *sys_mmap(void *addr, unsigned long long length, int prot, int flags, int fd, long offset) {
	void* ret = (void*) syscall6(9, (size_t) addr, (size_t)length, (size_t)prot, (size_t)flags, (size_t)fd, (size_t) offset);
	return ret; 
}

int sys_munmap(void* addr, size_t length) {
	return (int) syscall2(11, (size_t)addr, (size_t)length);
}

int sys_close(int fd) {
	return syscall1(3, fd);
}