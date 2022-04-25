#ifndef PROCESS_H
#define PROCESS_H

#include <unistd.h>
#include <sys/uio.h>
#include <sys/syscall.h>

pid_t target_pid = -1;

/*
 * https://man7.org/linux/man-pages/man2/process_vm_readv.2.html
 * Syscall Implementation of process_vm_readv & process_vm_writev
 */
bool pvm(void *address, void *buffer, size_t size, bool iswrite) {
    struct iovec local[1];
    struct iovec remote[1];

    local[0].iov_base = buffer;
    local[0].iov_len = size;
    remote[0].iov_base = address;
    remote[0].iov_len = size;

    if (target_pid < 0) {
        return false;
    }

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

    ssize_t bytes = syscall((iswrite ? process_vm_writev_syscall : process_vm_readv_syscall),
                            target_pid, local, 1, remote, 1, 0);
    //printf("process_vm_readv reads %zd bytes from PID: %d\n", bytes, target_pid);
    return bytes == size;
}

//Process Virtual Memory Reader
bool vm_readv(void *address, void *buffer, size_t size) {
    return pvm(address, buffer, size, false);
}

//Process Virtual Memory Writer
bool vm_writev(void *address, void *buffer, size_t size) {
    return pvm(address, buffer, size, true);
}

#endif
