/* /////////////////////////////////////////////////////////////////////////
 * File:        src/util/strdup.c (was src/util/strutil.cpp)
 *
 * Purpose:     Implementation file for string duplication utility functions.
 *
 * Created:     21st June 2005
 * Updated:     23rd March 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2010, Matthew Wilson and Synesis Software
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


/* Pantheios Header Files */
#include <pantheios/pantheios.h>

#if defined(STLSOFT_COMPILER_IS_MWERKS)
# include <string.h>
#endif /* compiler */

#include <pantheios/util/string/strdup.h>
#include <pantheios/quality/contract.h>
#include <pantheios/internal/safestr.h>

/* STLSoft header files */
#include <stlsoft/stlsoft.h>

/* Standard C header files */
#include <string.h>
#include <wchar.h>

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

PANTHEIOS_CALL(char*) pantheios_util_strdup_nothrow_m(char const* s) /* stlsoft_throw_0() */
{
    if(NULL == s)
    {
        return NULL;
    }
    else
    {
        const size_t    len =   strlen(s);
        char*           s2  =   stlsoft_static_cast(char*, malloc(sizeof(char) * (1 + len)));

        if(NULL != s2)
        {
            memcpy(s2, s, sizeof(char) * (1 + len));
        }

        return s2;
    }
}

PANTHEIOS_CALL(void) pantheios_util_strfree_m(char* s) /* stlsoft_throw_0() */
{
    free(s);
}

PANTHEIOS_CALL(wchar_t*) pantheios_util_strdup_nothrow_w(wchar_t const* s) /* stlsoft_throw_0() */
{
    if(NULL == s)
    {
        return NULL;
    }
    else
    {
        const size_t    len =   wcslen(s);
        wchar_t*        s2  =   stlsoft_static_cast(wchar_t*, malloc(sizeof(wchar_t) * (1 + len)));

        if(NULL != s2)
        {
            memcpy(s2, s, sizeof(wchar_t) * (1 + len));
        }

        return s2;
    }
}

PANTHEIOS_CALL(void) pantheios_util_strfree_w(wchar_t* s) /* stlsoft_throw_0() */
{
    free(s);
}

PANTHEIOS_CALL(pan_char_t*) pantheios_util_strdup_nothrow(pan_char_t const* s) /* stlsoft_throw_0() */
{
#ifdef PANTHEIOS_USE_WIDE_STRINGS
    return pantheios_util_strdup_nothrow_w(s);
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    return pantheios_util_strdup_nothrow_m(s);
#endif /* PANTHEIOS_USE_WIDE_STRINGS */
}

PANTHEIOS_CALL(void) pantheios_util_strfree(pan_char_t* s) /* stlsoft_throw_0() */
{
#ifdef PANTHEIOS_USE_WIDE_STRINGS
    pantheios_util_strfree_w(s);
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    pantheios_util_strfree_m(s);
#endif /* PANTHEIOS_USE_WIDE_STRINGS */
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
