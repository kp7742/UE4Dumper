#ifndef PROCESS_H
#define PROCESS_H

#include <sys/syscall.h>
#include <unistd.h>
#include <sys/uio.h>

pid_t target_pid = -1;

#if defined(__arm__)
int process_vm_readv_syscall = 376;
int process_vm_writev_syscall = 377;
#elif defined(__aarch64__)
int process_vm_readv_syscall = 270;
int process_vm_writev_syscall = 271;
#elif defined(__i386__)
int process_vm_readv_syscall = 347;
int process_vm_writev_syscall = 348;
#else
int process_vm_readv_syscall = 310;
int process_vm_writev_syscall = 311;
#endif

//Syscall Implementation of process_vm_readv & process_vm_writev
ssize_t process_v(pid_t __pid, const struct iovec* __local_iov, unsigned long __local_iov_count,
	const struct iovec* __remote_iov, unsigned long __remote_iov_count, unsigned long __flags, bool iswrite) {
	return syscall((iswrite ? process_vm_writev_syscall : process_vm_readv_syscall), __pid, __local_iov, __local_iov_count, __remote_iov, __remote_iov_count, __flags);
}

//Process VM Reader/Writer
bool pvm(void* address, void* buffer, size_t size, bool iswrite) {
	struct iovec local[1];
	struct iovec remote[1];

	local[0].iov_base = buffer;
	local[0].iov_len = size;
	remote[0].iov_base = address;
	remote[0].iov_len = size;

	if (target_pid < 0) {
		return false;
	}

	ssize_t bytes = process_v(target_pid, local, 1, remote, 1, 0, iswrite);
	//printf("process_vm_readv reads %zd bytes from PID: %d\n", bytes, target_pid);
	return bytes == size;
}

//Process Memory Reader
bool vm_readv(void* address, void* buffer, size_t size) {
	return pvm(address, buffer, size, false);
}

bool vm_writev(void* address, void* buffer, size_t size) {
	return pvm(address, buffer, size, true);
}

#endif
