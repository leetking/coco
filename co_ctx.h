#ifndef CO_CTX_H_
#define CO_CTX_H_

#include <stdlib.h>

typedef void *ctx_t;
typedef struct {
    ctx_t ctx;
    void *data;
} transfer_t;
ctx_t co_make_ctx(void *sp, size_t size, int (*fn)(transfer_t));
transfer_t co_jump_ctx(ctx_t const to, void *data);

#endif /* CO_CTX_H_ */
