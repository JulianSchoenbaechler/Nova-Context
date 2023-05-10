/*
 * Nova-Context
 * This file is part of the Nova-Context library source code.
 * Copyright (c) 2023 - The Nova Project
 */

#ifndef NOVA_CONTEXT_CONFIG_H
#define NOVA_CONTEXT_CONFIG_H

#ifndef NOVA_CONTEXT_STATIC
 #ifdef nova_context_EXPORTS
  #if defined(_MSC_VER) || defined(__MINGW32__)
   #define NOVA_CONTEXT_API __declspec(dllexport)
  #else
   #define NOVA_CONTEXT_API __attribute__((__visibility__("default")))
  #endif
 #elif defined(_MSC_VER)
  #define NOVA_CONTEXT_API __declspec(dllimport)
 #endif
#else
 #define NOVA_CONTEXT_API
#endif

#if (defined(i386) || defined(__i386__) || defined(__i386) || \
     defined(__i486__) || defined(__i586__) || defined(__i686__) || \
     defined(__X86__) || defined(_X86_) || defined(__THW_INTEL__) || \
     defined(__I86__) || defined(__INTEL__) || defined(__IA32__) || \
     defined(_M_IX86) || defined(_I86_)) && defined(BOOST_WINDOWS)
 #define NOVA_CONTEXT_CALLDECL __cdecl
#else
 #define NOVA_CONTEXT_CALLDECL
#endif

#endif //NOVA_CONTEXT_CONFIG_H
