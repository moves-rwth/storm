/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/util/string/strdup.h (formerly part of pantheios/pantheios.h)
 *
 * Purpose:     String duplication utilty functions.
 *
 * Created:     19th April 2008
 * Updated:     22nd March 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2008-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/util/string/strdup.h
 *
 * [C, C++] String duplication functions.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_UTIL_STRING_H_STRDUP
#define PANTHEIOS_INCL_PANTHEIOS_UTIL_STRING_H_STRDUP

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_UTIL_STRING_H_STRDUP_MAJOR     1
# define PANTHEIOS_VER_PANTHEIOS_UTIL_STRING_H_STRDUP_MINOR     2
# define PANTHEIOS_VER_PANTHEIOS_UTIL_STRING_H_STRDUP_REVISION  2
# define PANTHEIOS_VER_PANTHEIOS_UTIL_STRING_H_STRDUP_EDIT      6
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

#ifndef STLSOFT_INCL_STLSOFT_H_STLSOFT
# include <stlsoft/stlsoft.h>
#endif /* !STLSOFT_INCL_STLSOFT_H_STLSOFT */

/* Standard C & C++ header files */
#ifdef STLSOFT_CF_THROW_BAD_ALLOC
# include <new>
# ifdef STLSOFT_COMPILER_IS_BORLAND
#  include <memory.h>
# endif /* compiler */
#endif /* STLSOFT_CF_THROW_BAD_ALLOC */

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

/** Non-exception-throwing C-style string duplication function
 *
 * \param s The string to duplicate
 *
 * \return A C-style string that is an equivalent copy of the source string,
 *   or a <code>NULL</code> pointer if memory could not be allocated.
 *
 * \note The returned value must be freed using pantheios_util_free()
 */
PANTHEIOS_CALL(pan_char_t*) pantheios_util_strdup_nothrow(pan_char_t const* s) /* stlsoft_throw_0() */;

/** Frees a duplicated string allocated by pantheios_util_strdup_nothrow()
 *   or pantheios_util_strdup_throw()
 *
 * \param s The string to deallocate
 */
PANTHEIOS_CALL(void) pantheios_util_strfree(pan_char_t* s) /* stlsoft_throw_0() */;

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
PANTHEIOS_CALL(char*) pantheios_util_strdup_nothrow_m(char const* s) /* stlsoft_throw_0() */;
PANTHEIOS_CALL(void) pantheios_util_strfree_m(char* s) /* stlsoft_throw_0() */;

PANTHEIOS_CALL(wchar_t*) pantheios_util_strdup_nothrow_w(wchar_t const* s) /* stlsoft_throw_0() */;
PANTHEIOS_CALL(void) pantheios_util_strfree_w(wchar_t* s) /* stlsoft_throw_0() */;
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * API (C++)
 */

#if defined(__cplusplus) && \
    defined(STLSOFT_CF_THROW_BAD_ALLOC)
/** [C++ only] Exception-throwing C-style string duplication function
 *
 * \param s The string to duplicate
 *
 * \return A C-style string that is an equivalent copy of the source string.
 *   If memory cannot be allocated, <code>std::bad_alloc</code> will be
 *   thrown.
 *
 * \note The returned value must be freed using pantheios_util_free()
 */
inline pan_char_t* pantheios_util_strdup_throw(pan_char_t const* s)
{
    pan_char_t* s2 = pantheios_util_strdup_nothrow(s);

    if( NULL == s2 &&
        NULL != s)
    {
        throw std::bad_alloc();
    }

    return s2;
}
#endif /* C++ && exceptions */


#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace util
{

/** Equivalent to \ref pantheios::pantheios_util_strdup_nothrow "pantheios_util_strdup_nothrow()".
 *
 * \ingroup group__utility
 */
inline pan_char_t* strdup_nothrow(pan_char_t const* s) stlsoft_throw_0()
{
    return pantheios_util_strdup_nothrow(s);
}

# if defined(STLSOFT_CF_THROW_BAD_ALLOC)
/** Equivalent to \ref pantheios::pantheios_util_strdup_throw "pantheios_util_strdup_throw()".
 *
 * \ingroup group__utility
 */
inline pan_char_t* strdup_throw(pan_char_t const* s)
{
    return pantheios_util_strdup_throw(s);
}
# endif /* exceptions */

/** Equivalent to \ref pantheios::pantheios_util_strfree "pantheios_util_strfree()".
 *
 * \ingroup group__utility
 */
inline void strfree(pan_char_t* s) stlsoft_throw_0()
{
    pantheios_util_strfree(s);
}

} /* namespace util */
#endif /* !PANTHEIOS_NO_NAMESPACE */

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

#endif /* !PANTHEIOS_INCL_PANTHEIOS_UTIL_STRING_H_STRDUP */

/* ///////////////////////////// end of file //////////////////////////// */
