/* /////////////////////////////////////////////////////////////////////////
 * File:        src/util/threadid.c
 *
 * Purpose:     Threading utility functions
 *
 * Created:     4th January 2008
 * Updated:     10th August 2009
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2008-2009, Matthew Wilson and Synesis Software
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name(s) of Matthew Wilson and Synesis Software nor the
 *   names of any contributors may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ////////////////////////////////////////////////////////////////////// */


#include <pantheios/pantheios.h>
#include <pantheios/internal/lean.h>
#include <pantheios/quality/contract.h>
#include <pantheios/internal/threading.h>
#include <pantheios/util/system/threadid.h>

#include <platformstl/platformstl.h>

#if defined(PLATFORMSTL_OS_IS_UNIX)
# ifdef PANTHEIOS_MT
#  include <pthread.h>
# endif /* PANTHEIOS_MT */
#elif defined(PLATFORMSTL_OS_IS_WINDOWS)
# include <windows.h>
#endif /* PLATFORMSTL_OS_IS_???? */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

typedef stlsoft_ns_qual(ss_sint64_t)    sint64_t_;

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

PANTHEIOS_CALL(sint64_t_) pantheios_getCurrentThreadId(void)
{
#if defined(PLATFORMSTL_OS_IS_UNIX)
# ifndef PANTHEIOS_MT
    return 1;
# else /* ? PANTHEIOS_MT */
#  if 0
    return stlsoft_reinterpret_cast(sint64_t_, STLSOFT_NS_GLOBAL(pthread_self)());
#  else /* ? 0 */

    /* Mac OS-X (Leopard) whinges about this cast, so we do a union cast
     */
    union
    {
        sint64_t_   i64;
        pthread_t   thid;
    } u;

    STLSOFT_STATIC_ASSERT(8 == sizeof(u.i64));
    STLSOFT_STATIC_ASSERT(sizeof(u.thid) <= sizeof(u.i64));
    STLSOFT_STATIC_ASSERT(sizeof(STLSOFT_NS_GLOBAL(pthread_self())) <= sizeof(u.i64));

    u.i64   =   0;
    u.thid  =   STLSOFT_NS_GLOBAL(pthread_self)();

    return u.i64;
#  endif /* 0 */
# endif /* !PANTHEIOS_MT */
#elif defined(PLATFORMSTL_OS_IS_WINDOWS)
    return stlsoft_static_cast(sint64_t_, STLSOFT_NS_GLOBAL(GetCurrentThreadId)());
#else /* ? OS */
# error Not discriminated for platforms other than UNIX and Windows
#endif /* OS */
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
