#include "co_ctx.h"

#include <stdio.h>

enum { STACK_SIZE = 128*1024, };
char stack1[STACK_SIZE];
char stack2[STACK_SIZE];

ctx_t master;
ctx_t ctx1;
ctx_t ctx2;

int f1(transfer_t t)
{
    master = t.ctx;
    printf("f1 from main data: %ld\n", (long)t.data);
    t = co_jump_ctx(ctx2, t.data+1);

    master = t.ctx;
    printf("f1 from main data: %ld\n", (long)t.data);
    co_jump_ctx(ctx2, t.data+1);

    return 0;
}

int f2(transfer_t t)
{
    ctx1 = t.ctx;
    printf("f2 from f1 data: %ld\n", (long)t.data);
    t = co_jump_ctx(master, t.data+1);

    ctx1 = t.ctx;
    printf("f2 from f1 data: %ld\n", (long)t.data);
    co_jump_ctx(master, t.data+1);

    return 0;
}

int main()
{
    ctx1 = co_make_ctx(stack1 + STACK_SIZE, STACK_SIZE, f1);
    ctx2 = co_make_ctx(stack2 + STACK_SIZE, STACK_SIZE, f2);

    transfer_t t = co_jump_ctx(ctx1, (void*)41);
    ctx2 = t.ctx;
    printf("main jump from f2, arg %ld\n", (long)t.data);

    t = co_jump_ctx(ctx1, t.data+1);
    ctx2 = t.ctx;
    printf("main jump from f2, arg %ld\n", (long)t.data);

    return 0;
}
