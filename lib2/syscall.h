#ifndef SYSCALL_H
#define SYSCALL_H

unsigned long long sys_write(int fd, const void* string, unsigned long long len);
void sys_exit(int status);
unsigned long long sys_read(int fd, char* buffer, unsigned long long count);
unsigned long long sys_brk(unsigned long long brk);
int sys_open(const char* pathname, int flags, int mode);
void *sys_mmap(void *addr, unsigned long long length, int prot, int flags, int fd, long offset);
int sys_munmap(void* addr, unsigned long long length);
int sys_close(int fd);

int sys_pipe(int *fd);

#endif