/*
 * Nova-Context
 * This file is part of the Nova-Context library source code.
 * Copyright (c) 2023 - The Nova Project
 */

#ifndef NOVA_CONTEXT_TSAN_H
#define NOVA_CONTEXT_TSAN_H

#if defined(__SANITIZER_THREAD__) || __has_feature(thread_sanitizer)
 #define NOVA_CONTEXT_TSAN

 #ifdef __cplusplus
extern "C" {
 #endif

void *__tsan_get_current_fiber(void);
void *__tsan_create_fiber(unsigned flags);
void __tsan_destroy_fiber(void *fiber);
void __tsan_switch_to_fiber(void *fiber, unsigned flags);

 #ifdef __cplusplus
}
 #endif

#endif

#endif //NOVA_CONTEXT_TSAN_H
