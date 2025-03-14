#ifndef CO_IO_H_
#define CO_IO_H_

#include <stdbool.h>

#include <unistd.h>
#include <sys/socket.h>

#include "co.h"

// io
// fd 需要设置为 nonblocking
ssize_t co_read(int fd, void *buf, size_t nbytes);
ssize_t co_write(int fd, void const *buf, size_t nbytes);
int co_accept(int fd, struct sockaddr *addr, socklen_t *addr_len);
void co_sleep(int us);

// schedule
void cosch_add(void* (*fn)(void*), void* data);
void cosch_take(coroutine_t *co);
void cosch_run(bool (*abort_fn)(void*), void* data);

#endif // CO_IO_H_
