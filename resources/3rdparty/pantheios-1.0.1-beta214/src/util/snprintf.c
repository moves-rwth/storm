/* /////////////////////////////////////////////////////////////////////////
 * File:        src/util/snprintf.c (was src/util/strutil.cpp)
 *
 * Purpose:     Implementation file for snprintf() utility functions.
 *
 * Created:     21st June 2005
 * Updated:     16th April 2009
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2009, Matthew Wilson and Synesis Software
 * Copyright (c) 1999-2005, Synesis Software and Matthew Wilson
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


/* Pantheios Header Files
 *
 * NOTE: We do _not_ include pantheios/pantheios.hpp here, since we are
 *  not using any of the Application Layer.
 */

#include <pantheios/pantheios.h>
#include <pantheios/internal/lean.h>

#include <pantheios/util/string/snprintf.h>

#include <pantheios/internal/safestr.h>

/* STLSoft header files */

#include <stlsoft/stlsoft.h>

/* Standard C header files */

#include <stdarg.h>
#include <stdio.h>

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

#define PANTHEIOS_COVER_MARK_ENTRY()


/* /////////////////////////////////////////////////////////////////////////
 * Compiler / feature discrimination
 *
 * pantheios_util_vsnprintf_a_(d, n, f, a)
 * pantheios_util_vsnprintf_w_(d, n, f, a)
 *
 * pantheios_util_vsnprintf_(d, n, f, a)
 */

#if defined(STLSOFT_COMPILER_IS_DMC) || \
    (   (   defined(WIN32) || \
            defined(WIN64)) && \
        (   defined(STLSOFT_COMPILER_IS_INTEL) || \
            (   defined(STLSOFT_COMPILER_IS_COMO) && \
                defined(_MSC_VER)))) || \
    defined(STLSOFT_COMPILER_IS_MSVC)
# ifdef PANTHEIOS_USING_SAFE_STR_FUNCTIONS
#  define pantheios_util_vsnprintf_a_(d, n, f, a)      _vsnprintf_s((d), (n), _TRUNCATE, (f), (a))
#  define pantheios_util_vsnprintf_w_(d, n, f, a)      _vsnwprintf_s((d), (n), _TRUNCATE, (f), (a))
# else /* ? PANTHEIOS_USING_SAFE_STR_FUNCTIONS */
#  define pantheios_util_vsnprintf_a_(d, n, f, a)      _vsnprintf((d), (n), (f), (a))
#  define pantheios_util_vsnprintf_w_(d, n, f, a)      _vsnwprintf((d), (n), (f), (a))
# endif /* PANTHEIOS_USING_SAFE_STR_FUNCTIONS */
#else /* ? compiler */
#  define pantheios_util_vsnprintf_a_(d, n, f, a)      vsnprintf((d), (n), (f), (a))
#  define pantheios_util_vsnprintf_w_(d, n, f, a)      vsnwprintf((d), (n), (f), (a))
#endif /* compiler */

#ifdef PANTHEIOS_USE_WIDE_STRINGS
# define pantheios_util_vsnprintf_(d, n, f, a)         pantheios_util_vsnprintf_w_(d, n, f, a)
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
# define pantheios_util_vsnprintf_(d, n, f, a)         pantheios_util_vsnprintf_a_(d, n, f, a)
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* /////////////////////////////////////////////////////////////////////////
 * Private API
 */

PANTHEIOS_CALL(int) pantheios_util_vsnprintf_a(
    char*       dest
,   size_t      cchDest
,   char const* fmt
,   va_list     args
)
{
    PANTHEIOS_COVER_MARK_ENTRY();

    return pantheios_util_vsnprintf_a_(dest, cchDest, fmt, args);
}

PANTHEIOS_CALL(int) pantheios_util_snprintf_a(
    char*       dest
,   size_t      cchDest
,   const char* fmt
,   ...
)
{
    va_list args;
    int     ret;

    PANTHEIOS_COVER_MARK_ENTRY();

    va_start(args, fmt);

    ret = pantheios_util_vsnprintf_a(dest, cchDest, fmt, args);

    va_end(args);

    PANTHEIOS_COVER_MARK_ENTRY();

    return ret;
}

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

PANTHEIOS_CALL(int) pantheios_util_vsnprintf(
    pan_char_t*         dest
,   size_t              cchDest
,   pan_char_t const*   fmt
,   va_list             args
)
{
    PANTHEIOS_COVER_MARK_ENTRY();

    return pantheios_util_vsnprintf_(dest, cchDest, fmt, args);
}

PANTHEIOS_CALL(int) pantheios_util_snprintf(
    pan_char_t*         dest
,   size_t              cchDest
,   const pan_char_t*   fmt
,   ...
)
{
    va_list args;
    int     ret;

    PANTHEIOS_COVER_MARK_ENTRY();

    va_start(args, fmt);

    ret = pantheios_util_vsnprintf(dest, cchDest, fmt, args);

    va_end(args);

    PANTHEIOS_COVER_MARK_ENTRY();

    return ret;
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
