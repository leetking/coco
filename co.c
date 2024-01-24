#include "co.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "co_ctx.h"

enum { STACK_SIZE = 128*1024, };

struct coroutine_t {
    ctx_t ctx;
    coroutine_t *caller;
    void *(*fn)(void *);
    void* data;
    co_state_t state;
    char stack[1];
};

coroutine_t *curr_co = NULL;
coroutine_t main_co;

static void co_return(void *data)
{
    coroutine_t *caller = curr_co->caller;
    curr_co->state = CO_DONE;
    caller->state = CO_RUNNING;
    caller->data = data;
    co_jump_ctx(caller->ctx, caller);
}

static int coroutine_fn(transfer_t t)
{
    // t.ctx: caller, t.data: callee
    coroutine_t *co = t.data;
    curr_co->ctx = t.ctx;
    curr_co = co;
    void *ret = co->fn(co->data);
    co_return(ret);
    return 0;
}

coroutine_t* co_create(void *(*fn)(void *))
{
    coroutine_t* co = malloc(sizeof(coroutine_t) + STACK_SIZE);
    void *sp = co->stack + STACK_SIZE;
    co->ctx = co_make_ctx(sp, STACK_SIZE, coroutine_fn);
    co->fn = fn;
    co->data = NULL;
    co->state = CO_READY;
    co->caller = NULL;

    return co;
}

co_state_t co_state(coroutine_t *co)
{
    return co->state;
}

void* co_resume(coroutine_t* co, void *data)
{
    if (!curr_co) {
        memset(&main_co, 0, sizeof(main_co));
        main_co.state = CO_RUNNING;
        curr_co = &main_co;
    }
    // CO_READY, CO_SUSPENDED -> CO_RUNNING
    curr_co->state = CO_SUSPENDED;
    co->state = CO_RUNNING;
    co->caller = curr_co;
    co->data = data;
    transfer_t t = co_jump_ctx(co->ctx, co);
    // t.ctx: callee, t.data: caller
    coroutine_t *caller = t.data;
    curr_co->ctx = t.ctx;
    curr_co = caller;
    return curr_co->data;
}

void *co_yield(void *data)
{
    if (!curr_co || !curr_co->caller) {
        printf("FATAL co_yield must be invoked in coroutine\n");
        exit(1);
    }

    coroutine_t *caller = curr_co->caller;
    curr_co->state = CO_SUSPENDED;
    caller->state = CO_RUNNING;
    caller->data = data;
    transfer_t t = co_jump_ctx(caller->ctx, caller);
    // t.ctx: caller, t.data: callee
    coroutine_t *co = t.data;
    curr_co->ctx = t.ctx;
    curr_co = co;
    return curr_co->data;
}

int co_destroy(coroutine_t *co)
{
    free(co);
    return 0;
}

coroutine_t *co_self()
{
    return curr_co;
}
