#ifndef CO_H_
#define CO_H_

typedef enum co_state_t {
    CO_READY = 0,
    CO_RUNNING = 1,
    CO_SUSPENDED = 2,
    CO_DONE = 3,
} co_state_t;

typedef struct coroutine_t coroutine_t;

coroutine_t *co_create(void *(*fn)(void *));
void* co_resume(coroutine_t *co, void *data);
void* co_yield(void *data);
coroutine_t *co_self();
co_state_t co_state(coroutine_t *co);
#define co_is_running(co) (CO_RUNNING == co_state(co))
#define co_is_done(co)    (CO_DONE == co_state(co))
int co_destroy(coroutine_t *co);

#endif // CO_H_
