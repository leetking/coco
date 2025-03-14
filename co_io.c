#include "co_io.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <sys/epoll.h>

#include "co.h"

enum { MAX_EVENTS = 102400, };

static int ep_fd = -1;

typedef struct coev_data_t {
    int fd;
    coroutine_t *co;
} coev_data_t;

static coev_data_t *coev_data_new(int fd)
{
    coev_data_t *data = malloc(sizeof(*data));
    *data = (coev_data_t){
        .fd = fd,
        .co = co_self(),
    };
    return data;
}

static void coev_data_free(coev_data_t *data)
{
    free(data);
}

static void coio_init()
{
    if (ep_fd >= 0)
        return;
    ep_fd = epoll_create1(0);
    if (ep_fd == -1) {
        perror("epoll_create1");
        exit(1);
    }
}

ssize_t co_read(int fd, void *buf, size_t nbytes)
{
    coio_init();
    // TODO: try read 直到里面没有数据，且没有读取到数据
    struct epoll_event rdev = {
        .events = EPOLLIN | EPOLLET,
        .data.ptr = coev_data_new(fd),
    };
    if (-1 == epoll_ctl(ep_fd, EPOLL_CTL_ADD, fd, &rdev))
        return -1;
    co_yield(NULL);
    return read(fd, buf, nbytes);
}

ssize_t co_write(int fd, void const* buf, size_t nbytes)
{
    coio_init();
    struct epoll_event wrev = {
        .events = EPOLLOUT | EPOLLET,
        .data.ptr = coev_data_new(fd),
    };
    if (-1 == epoll_ctl(ep_fd, EPOLL_CTL_ADD, fd, &wrev))
        return -1;
    co_yield(NULL);
    return write(fd, buf, nbytes);
}

int co_accept(int fd, struct sockaddr *addr, socklen_t *addr_len)
{
    coio_init();
    struct epoll_event rdev = {
        .events = EPOLLIN | EPOLLET,
        .data.ptr = coev_data_new(fd),
    };
    if (epoll_ctl(ep_fd, EPOLL_CTL_ADD, fd, &rdev) == -1) {
        return -1;
    }
    co_yield(NULL);
    return accept(fd, addr, addr_len);
}

void co_sleep(int us)
{
    // TODO: 采用时间轮实现定时
}



// schedule
void cosch_add(void* (*fn)(void*), void* data)
{
    coroutine_t *co = co_create(fn, NULL);
    co_resume(co, data);
}

void cosch_take(coroutine_t *co)
{
    if (CO_READY == co_state(co)) {
        co_resume(co, NULL);
    }
}

void cosch_run(bool (*abort_fn)(void*), void* data)
{
    if (!co_is_main()) {
        printf("FATAL must run in main coroutine\n");
        return;
    }

    struct epoll_event events[MAX_EVENTS];
    while (!abort_fn || !abort_fn(data)) {
        // timeout: us
        int nready = epoll_wait(ep_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nready; ++i) {
            coev_data_t *data = events[i].data.ptr;
            epoll_ctl(ep_fd, EPOLL_CTL_DEL, data->fd, NULL);
            co_resume(data->co, NULL);
            coev_data_free(data);
        }
        if (nready == -1) {
            perror("epoll_wait");
        }
    }
}
