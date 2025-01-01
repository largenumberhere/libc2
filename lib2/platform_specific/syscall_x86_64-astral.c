// todo: fix the numbers mess

// stolen from mlibc

#define SYSCALL_PRINT 0
#define SYSCALL_MMAP 1
#define SYSCALL_OPENAT 2
#define SYSCALL_READ 3
#define SYSCALL_SEEK 4
#define SYSCALL_CLOSE 5
#define SYSCALL_ARCHCTL 6
#define SYSCALL_WRITE 7
#define SYSCALL_GETPID 8
#define SYSCALL_FSTAT 9
#define SYSCALL_FSTATAT 10
#define SYSCALL_FORK 11
#define SYSCALL_EXECVE 12
#define SYSCALL_EXIT 13
#define SYSCALL_WAITPID 14
#define SYSCALL_MUNMAP 15
#define SYSCALL_GETDENTS 16
#define SYSCALL_DUP 17
#define SYSCALL_DUP2 18
#define SYSCALL_DUP3 19
#define SYSCALL_FCNTL 20
#define SYSCALL_CHDIR 21
#define SYSCALL_PIPE2 22
#define SYSCALL_ISATTY 23
#define SYSCALL_FACCESSAT 24
#define SYSCALL_UNLINKAT 25
#define SYSCALL_IOCTL 26
#define SYSCALL_MKDIRAT 27
#define SYSCALL_CLOCKGET 28
#define SYSCALL_LINKAT 29
#define SYSCALL_READLINKAT 30
#define SYSCALL_FCHMOD 31
#define SYSCALL_FCHMODAT 32
#define SYSCALL_UMASK 33
#define SYSCALL_POLL 34
#define SYSCALL_NANOSLEEP 35
#define SYSCALL_FTRUNCATE 36
#define SYSCALL_MOUNT 37
#define SYSCALL_FCHOWNAT 38
#define SYSCALL_UTIMENSAT 39
#define SYSCALL_RENAMEAT 40
#define SYSCALL_SOCKET 41
#define SYSCALL_BIND 42
#define SYSCALL_SENDMSG 43
#define SYSCALL_SETSOCKOPT 44
#define SYSCALL_RECVMSG 45
#define SYSCALL_LISTEN 46
#define SYSCALL_CONNECT 47
#define SYSCALL_ACCEPT 48
#define SYSCALL_NEWTHREAD 49
#define SYSCALL_THREADEXIT 50
#define SYSCALL_FUTEX 51
#define SYSCALL_GETTID 52
#define SYSCALL_GETPPID 53
#define SYSCALL_GETPGID 54
#define SYSCALL_GETSID 55
#define SYSCALL_SETSID 56
#define SYSCALL_SETPGID 57
#define SYSCALL_SIGACTION 58
#define SYSCALL_SIGALTSTACK 59
#define SYSCALL_SIGPROCMASK 60
#define SYSCALL_KILL 61
#define SYSCALL_SIGRETURN 62
#define SYSCALL_UNAME 63
#define SYSCALL_HOSTNAME 64
#define SYSCALL_SYNC 65
#define SYSCALL_FSYNC 66
#define SYSCALL_FCHDIR 67
#define SYSCALL_SETITIMER 68
#define SYSCALL_GETITIMER 69
#define SYSCALL_SOCKETPAIR 70
#define SYSCALL_GETSOCKNAME 71
#define SYSCALL_GETPEERNAME 72
#define SYSCALL_CHROOT 73
#define SYSCALL_PAUSE 74
#define SYSCALL_PPOLL 75
#define SYSCALL_PREAD 76
#define SYSCALL_PWRITE 77
#define SYSCALL_MKNODAT 78
#define SYSCALL_GETRESUID 79
#define SYSCALL_GETRESGID 80
#define SYSCALL_SETRESUID 81
#define SYSCALL_SETRESGID 82
#define SYSCALL_MPROTECT 83
#define SYSCALL_SETUID 84
#define SYSCALL_SETEUID 85
#define SYSCALL_SETGID 86
#define SYSCALL_SETEGID 87
#define SYSCALL_SIGSUSPEND 88
#define SYSCALL_SIGTIMEDWAIT 89
#define SYSCALL_SIGPENDING 90
#define SYSCALL_KILLTHREAD 91

#define size_t unsigned long long
#define NULL (void*)0
#define EMPTY 0

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
	return syscall6(id, reg1, reg2, EMPTY, EMPTY, EMPTY, EMPTY);
}

static size_t syscall1(size_t id, size_t reg1) {
	return syscall6(id, reg1, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}

static size_t syscall3(size_t id, size_t reg1, size_t reg2, size_t reg3) {
	return syscall6(id, reg1, reg2, reg3, EMPTY, EMPTY, EMPTY);
}

static size_t syscall4(size_t id, size_t reg1, size_t reg2, size_t reg3, size_t reg4) {
	return syscall6(id, reg1, reg2, reg3, reg4, EMPTY, EMPTY);
}



size_t sys_write(int fd, const void* string, size_t len) {
	size_t v = syscall3(SYSCALL_WRITE, (size_t)fd, (size_t)string, (size_t)len);
    return v;
}

_Noreturn void sys_exit(int status) {
	syscall1(60, status);
	
	while (1) {}
}

size_t sys_read(int fd, char* buffer, size_t count) {
    return syscall3(SYSCALL_READ , (size_t) fd, (size_t) buffer, (size_t) count);
}

size_t sys_brk(size_t brk) {
	return 0;
    // size_t brk_val = syscall1(SYSCALL_BRK,(size_t) brk);
    // return (size_t) brk_val;
}


//https://sites.uclouvain.be/SystInfo/usr/include/linux/fcntl.h.html
#define AT_FDCWD                -100    /* Special value used to indicate
                                           openat should use the current
                                           working directory. */

int sys_open(const char* pathname, int flags, int mode) {
	return (int) syscall4(SYSCALL_OPENAT, AT_FDCWD, (size_t) pathname, flags, mode);
}



void *sys_mmap(void *addr, unsigned long long length, int prot, int flags, int fd, long offset) {
	void* ret = (void*) syscall6(SYSCALL_MMAP, (size_t) addr, (size_t)length, (size_t)prot, (size_t)flags, (size_t)fd, (size_t) offset);
	return ret; 
}

int sys_munmap(void* addr, size_t length) {
	return (int) syscall2(SYSCALL_MUNMAP, (size_t)addr, (size_t)length);
}

int sys_close(int fd) {
	return syscall1(SYSCALL_CLOSE, fd);
}