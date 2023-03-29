
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//        http://www.boost.org/LICENSE_1_0.txt)

// Modified to match the Nova-Context project structure
#include "include/nova/context/fcontext.h"

// This C++ tail of ontop_fcontext() allocates fcontext_transfer_t{ from, vp } on the stack. If fn() throws a
// C++ exception, then the C++ runtime must remove this tail's stack frame.
extern "C" fcontext_transfer_t
ontop_fcontext_tail(int ignore, void *vp, fcontext_transfer_t (*fn)(fcontext_transfer_t), fcontext_t const from)
{
    return fn(fcontext_transfer_t{ from, vp });
}
