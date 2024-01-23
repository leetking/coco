#include "co_ctx.h"

#include <stdio.h>

enum { STACK_SIZE = 128*1024, };
char stack1[STACK_SIZE];
char stack2[STACK_SIZE];

ctx_t master;
ctx_t ctx1;
ctx_t ctx2;

void f1(transfer_t t)
{
    master = t.ctx;
    printf("f1 from main data: %ld\n", (long)t.data);
    void* arg = t.data + 1;
    co_jump_ctx(ctx2, arg);
}

void f2(transfer_t t)
{
    printf("f2 from f1 data: %ld\n", (long)t.data);
    void* arg = t.data + 1;
    co_jump_ctx(master, arg);
}

int main()
{
    ctx1 = co_make_ctx(stack1 + STACK_SIZE, STACK_SIZE, f1);
    ctx2 = co_make_ctx(stack2 + STACK_SIZE, STACK_SIZE, f2);

    transfer_t t = co_jump_ctx(ctx1, (void*)41);
    printf("main jump from f2, arg %ld\n", (long)t.data);

    /**
     * TODO: 支持普通协程优雅结束，而不是直接coredump
     * 每个协程隐含结束后调用exit
     */

    return 0;
}
