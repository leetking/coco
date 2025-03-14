#ifndef CO_H_
#define CO_H_

#include <stddef.h>
#include <inttypes.h>
#include <stdbool.h>

enum {
    CO_READY = 0,
    CO_RUNNING = 1,
    CO_SUSPENDED = 2,
    CO_DONE = 3,
};

typedef struct coroutine_t coroutine_t;

typedef struct co_create_opts_t {
    size_t stack_size;
} co_create_opts_t;
coroutine_t *co_create(void *(*fn)(void *), co_create_opts_t *opts);
void co_destroy(coroutine_t *co);
void *co_resume(coroutine_t *co, void *data);
void *co_yield(void *data);

coroutine_t *co_self();
bool co_is_main();
uint64_t co_id();

int co_state(coroutine_t *co);
#define co_is_ready(co)     (CO_READY == co_state(co))
#define co_is_running(co)   (CO_RUNNING == co_state(co))
#define co_is_suspended(co) (CO_SUSPENDED == co_state(co))
#define co_is_done(co)      (CO_DONE == co_state(co))
// TODO: add getter for co

#endif // CO_H_
