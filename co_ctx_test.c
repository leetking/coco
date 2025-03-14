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
    printf("f1 <- main: recv %ld\n", (long)t.data);

    printf("f1 -> f2: pass %ld\n", (long)t.data+1);
    t = co_jump_ctx(ctx2, t.data+1);
    master = t.ctx;
    printf("f1 <- main: recv %ld\n", (long)t.data);

    printf("f1 -> f2: pass %ld\n", (long)t.data+1);
    t = co_jump_ctx(ctx2, t.data+1);
    printf("f1 <- main: recv %ld\n", (long)t.data);

    // NOTE: f1 返回后，会执行到 exit 导致进程结束
    printf("f1 return %ld\n", (long)t.data+1);
    return 0;
}

int f2(transfer_t t)
{
    ctx1 = t.ctx;
    printf("f2 <- f1: recv %ld\n", (long)t.data);

    printf("f2 -> main: pass %ld\n", (long)t.data+1);
    t = co_jump_ctx(master, t.data+1);
    ctx1 = t.ctx;
    printf("f2 <- f1: recv %ld\n", (long)t.data);

    printf("f2 -> main: pass %ld\n", (long)t.data+1);
    t = co_jump_ctx(master, t.data+1);
    printf("f2 <- ?: recv %ld\n", (long)t.data);

    printf("f2 return %ld\n", (long)t.data+1);
    return 0;
}

int main()
{
    ctx1 = co_make_ctx(stack1 + STACK_SIZE, STACK_SIZE, f1);
    ctx2 = co_make_ctx(stack2 + STACK_SIZE, STACK_SIZE, f2);

    void *data = (void *)41;
    printf("main -> f1: pass %ld\n", (long)data);
    transfer_t t = co_jump_ctx(ctx1, data);
    ctx2 = t.ctx;
    printf("main <- f2: recv %ld\n", (long)t.data);

    printf("main -> f1: pass %ld\n", (long)t.data+1);
    t = co_jump_ctx(ctx1, t.data+1);
    ctx2 = t.ctx;
    printf("main <- f2, recv %ld\n", (long)t.data);

    printf("main -> f1: pass %ld\n", (long)t.data+1);
    t = co_jump_ctx(ctx1, t.data+1);
    //ctx2 = t.ctx;
    //printf("main <- f2, recv %ld\n", (long)t.data);

    return 0;
}
