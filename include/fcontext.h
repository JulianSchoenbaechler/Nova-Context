/*
 * Nova-Context
 * This file is part of the Nova-Context library source code.
 * Copyright (c) 2023 - The Nova Project
 */

#ifndef NOVA_CONTEXT_FCONTEXT_H
#define NOVA_CONTEXT_FCONTEXT_H

#include <stdint.h>
#include <stddef.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *fcontext_t;

typedef struct fcontext_transfer
{
    fcontext_t fctx;
    void *data;
} fcontext_transfer_t;

NOVA_CONTEXT_API
fcontext_t NOVA_CONTEXT_CALLDECL
make_fcontext(void *sp, size_t size, void (*fn)(fcontext_transfer_t));

NOVA_CONTEXT_API
fcontext_transfer_t NOVA_CONTEXT_CALLDECL
jump_fcontext(fcontext_t const to, void *vp);

NOVA_CONTEXT_API
fcontext_transfer_t NOVA_CONTEXT_CALLDECL
ontop_fcontext(fcontext_t const to, void *vp, fcontext_transfer_t (*fn)(fcontext_transfer_t));

#ifdef __cplusplus
}
#endif

#endif //NOVA_CONTEXT_FCONTEXT_H
