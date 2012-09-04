/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/util/string/snprintf.h (was pantheios/util/string/string.h)
 *
 * Purpose:     snprintf() utility functions.
 *
 * Created:     21st June 2005
 * Updated:     15th March 2011
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2011, Matthew Wilson and Synesis Software
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


/** \file pantheios/util/string/snprintf.h
 *
 * [C, C++] snprintf() utility functions.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_UTIL_STRING_H_SNPRINTF
#define PANTHEIOS_INCL_PANTHEIOS_UTIL_STRING_H_SNPRINTF

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_UTIL_STRING_H_SNPRINTF_MAJOR       2
# define PANTHEIOS_VER_PANTHEIOS_UTIL_STRING_H_SNPRINTF_MINOR       2
# define PANTHEIOS_VER_PANTHEIOS_UTIL_STRING_H_SNPRINTF_REVISION    4
# define PANTHEIOS_VER_PANTHEIOS_UTIL_STRING_H_SNPRINTF_EDIT        20
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

#ifndef PANTHEIOS_INCL_H_STDARG
# define PANTHEIOS_INCL_H_STDARG
# include <stdarg.h>
#endif /* !PANTHEIOS_INCL_H_STDARG */

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/** An <code>snprintf()</code>-equivalent that insulates Pantheios
 *   implementation code from the peculiarities of different compilers
 *   and/or libraries.
 *
 * \param dest The buffer into which the result will be written
 * \param cchDest The number of character spaces that are available to be
 *   written
 * \param fmt The format string
 *
 * \return The usual return values defined for <code>snprinf()</code>
 */
PANTHEIOS_CALL(int) pantheios_util_snprintf(
    pan_char_t*         dest
,   size_t              cchDest
,   const pan_char_t*   fmt
,   ...
)
/* TODO: Change this to proper discriminated feature */
#if defined(STLSOFT_COMPILER_IS_GCC)
__attribute__((format(printf,3,4)))
#endif /* compiler */
;

/** A <code>vsnprintf()</code>-equivalent that insulates Pantheios
 *   implementation code from the peculiarities of different compilers
 *   and/or libraries.
 *
 * \param dest The buffer into which the result will be written
 * \param cchDest The number of character spaces that are available to be
 *   written
 * \param fmt The format string
 * \param args The argument list
 *
 * \return The usual return values defined for <code>vsnprinf()</code>
 */
PANTHEIOS_CALL(int) pantheios_util_vsnprintf(pan_char_t* dest, size_t cchDest, pan_char_t const* fmt, va_list args);

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
PANTHEIOS_CALL(int) pantheios_util_snprintf_a(
    char*       dest
,   size_t      cchDest
,   const char* fmt
,   ...
)
/* TODO: Change this to proper discriminated feature */
#if defined(STLSOFT_COMPILER_IS_GCC)
__attribute__((format(printf,3,4)))
#endif /* compiler */
;
PANTHEIOS_CALL(int) pantheios_util_vsnprintf_a(char* dest, size_t cchDest, char const* fmt, va_list args);
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Inclusion
 */

#ifdef STLSOFT_PPF_pragma_once_SUPPORT
# pragma once
#endif /* STLSOFT_PPF_pragma_once_SUPPORT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_UTIL_STRING_H_SNPRINTF */

/* ///////////////////////////// end of file //////////////////////////// */
