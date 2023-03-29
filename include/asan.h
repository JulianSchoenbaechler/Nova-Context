/*
 * Nova-Context
 * This file is part of the Nova-Context library source code.
 * Copyright (c) 2023 - The Nova Project
 */

#ifndef NOVA_CONTEXT_ASAN_H
#define NOVA_CONTEXT_ASAN_H

#if defined(__SANITIZE_ADDRESS__) || __has_feature(address_sanitizer)
 #define NOVA_CONTEXT_ASAN

 #ifdef __cplusplus
extern "C" {
 #endif

void __sanitizer_start_switch_fiber(void **fake_stack_save, const void *bottom, size_t size);
void __sanitizer_finish_switch_fiber(void *fake_stack_save, const void **bottom_old, size_t *size_old);

 #ifdef __cplusplus
}
 #endif

#endif

#endif //NOVA_CONTEXT_ASAN_H
