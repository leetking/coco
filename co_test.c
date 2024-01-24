#include "co.h"

#include <stdio.h>

void* fib_fn(void *n)
{
    int a = 0, b = 1;
    for (int i = 0; i < (long)n; ++i) {
        co_yield((void*)a);
        printf("fib_co: state %d\n", (int)co_state(co_self()));
        int c = a+b;
        a = b;
        b = c;
    }

    return a;
}

int main()
{
    long n = 10;
    coroutine_t *fib = co_create(fib_fn);
    printf("main_co: fib_co state %d\n", (int)co_state(fib));
    void *f;
    int i = 0;
    for (;;) {
        f = co_resume(fib, (void*)n);
        printf("main_co: fib(%d) = %ld, fib_co state %d\n", i++, (long)f, (int)co_state(fib));
        if (co_is_done(fib))
            break;
    }
    printf("fib done\n");
    co_destroy(fib);

    return 0;
}
