#include "co.h"

#include <stdio.h>
#include <inttypes.h>

void *fib_fn(void *n)
{
    printf("co_isnot_main: %d\n", !co_is_main());
    intptr_t a = 0, b = 1;
    for (int i = 0; i < (intptr_t)n; ++i) {
        co_yield((void *)a);
        printf("fib_co: state %d\n", (int)co_state(co_self()));
        intptr_t c = a+b;
        a = b;
        b = c;
    }

    return (void *)a;
}

int main()
{
    long n = 10;
    coroutine_t *fib = co_create(fib_fn, NULL);
    printf("co_is_main: %d\n", co_is_main());
    printf("main_co: %p\n", co_self());
    printf("main_co id: %zu\n", co_id());
    printf("main_co: fib_co state %d\n", (int)co_state(fib));
    int i = 0;
    while (!co_is_done(fib)) {
        void *f = co_resume(fib, (void *)n);
        printf("main_co: fib(%d) = %ld, fib_co state %d\n", i++, (long)f, (int)co_state(fib));
    }
    printf("fib done\n");
    co_destroy(fib);

    return 0;
}
