/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters/slice.hpp
 *
 * Purpose:     String inserter for slices of strings.
 *
 * Created:     13th February 2010
 * Updated:     17th March 2012
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2010-2012, Matthew Wilson and Synesis Software
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


/** \file pantheios/inserters/slice.hpp
 *
 * [c++ only] definition of the pantheios::slice string inserter for
 *   for string slices.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_PAD
#define PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_PAD

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_PAD_MAJOR    1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_PAD_MINOR    0
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_PAD_REVISION 3
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_PAD_EDIT     6
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

#include <stlsoft/memory/auto_buffer.hpp>
#include <stlsoft/shims/access/string.hpp>

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

#ifdef PANTHEIOS_CF_NAMESPACE_SUPPORT
//namespace inserters
//{
#endif /* PANTHEIOS_CF_NAMESPACE_SUPPORT */

/** Class for inserting string slices into Pantheios diagnostic logging
 *   statements.
 */
class slice_inserter
{
/// \name Member Types
/// @{
public:
    typedef slice_inserter                          class_type;
private:
    typedef stlsoft::auto_buffer<pan_char_t, 256>   buffer_type_;
/// @}

/// \name Construction
/// @{
public:
    slice_inserter(
        pan_char_t const*   str
    ,   size_t              len
    ,   pan_char_t const*   strName
    ,   pan_char_t const*   lenName
    ,   pan_char_t const*   equals
    ,   pan_char_t const*   separator
    );
#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
    slice_inserter(class_type const& rhs);
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
/// @}

/// \name Accessors
/// @{
public:
    ///  A possibly non-nul-terminated non-null pointer to the c-style string representation of the slice
    pan_char_t const*   data() const;
    ///  A nul-terminated non-null pointer to the c-style string representation of the slice
    pan_char_t const*   c_str() const;
    ///  The length of the c-style string representation of the slice
    size_t              length() const;
/// @}

/// \name Implementation
/// @{
private:
    void construct_() const;
    void construct_();
/// @}

/// \name Member Variables
/// @{
private:
    pan_char_t const*   m_str;
    size_t              m_len;
    pan_char_t const*   m_strName;
    pan_char_t const*   m_lenName;
    pan_char_t const*   m_equals;
    pan_char_t const*   m_separator;
    buffer_type_        m_buffer;
/// @}

/// \name Not to be implemented
/// @{
private:
    class_type& operator =(class_type const&);
/// @}
};

#ifdef PANTHEIOS_CF_NAMESPACE_SUPPORT
//namespace inserters
//{
#endif /* PANTHEIOS_CF_NAMESPACE_SUPPORT */

/* /////////////////////////////////////////////////////////////////////////
 * Inserter functions
 */

/** Defines slice of a string
 *
 * \param str Pointer to the first character in the string to be sliced
 * \param len Number of characters to be sliced
 *
 */
inline pan_slice_t slice(
    pan_char_t const*   str
,   size_t              len
)
{
    return pan_slice_t(str, len);
}

/** Defines a slice of string
 *
 * \param str Pointer to the first character in the string to be sliced
 * \param len Number of characters to be sliced
 * \param strName Name of the string parameter
 */
inline slice_inserter slice(
    pan_char_t const*   str
,   size_t              len
,   pan_char_t const*   strName
)
{
    return slice_inserter(str, len, strName, NULL, NULL, NULL);
}

/** Defines a slice of string
 *
 * \param str Pointer to the first character in the string to be sliced
 * \param len Number of characters to be sliced
 * \param strName Name of the string parameter
 * \param lenName Name of the length parameter
 */
inline slice_inserter slice(
    pan_char_t const*   str
,   size_t              len
,   pan_char_t const*   strName
,   pan_char_t const*   lenName
)
{
    return slice_inserter(str, len, strName, lenName, NULL, NULL);
}

/** Defines a slice of string
 *
 * \param str Pointer to the first character in the string to be sliced
 * \param len Number of characters to be sliced
 * \param strName Name of the string parameter
 * \param lenName Name of the length parameter
 * \param equals The string that separates the name and the value
 */
inline slice_inserter slice(
    pan_char_t const*   str
,   size_t              len
,   pan_char_t const*   strName
,   pan_char_t const*   lenName
,   pan_char_t const*   equals
)
{
    return slice_inserter(str, len, strName, lenName, equals, NULL);
}

/** Defines a slice of string
 *
 * \param str Pointer to the first character in the string to be sliced
 * \param len Number of characters to be sliced
 * \param strName Name of the string parameter
 * \param lenName Name of the length parameter
 * \param equals The string that separates the name and the value
 * \param separator The string to be used between the string and length parameters
 */
inline slice_inserter slice(
    pan_char_t const*   str
,   size_t              len
,   pan_char_t const*   strName
,   pan_char_t const*   lenName
,   pan_char_t const*   equals
,   pan_char_t const*   separator
)
{
    return slice_inserter(str, len, strName, lenName, equals, separator);
}

/* /////////////////////////////////////////////////////////////////////////
 * String Access Shims
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

# if !defined(PANTHEIOS_NO_NAMESPACE)
namespace shims
{
# endif /* !PANTHEIOS_NO_NAMESPACE */

/** \overload c_str_data_a(slice_inserter const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_data_w(slice_inserter const& i)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_data_a(slice_inserter const& i)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return i.data();
}
/** \overload c_str_data(slice_inserter const&) */
inline pan_char_t const* c_str_data(slice_inserter const& i)
{
    return i.data();
}

/** \overload c_str_len_a(slice_inserter const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline size_t c_str_len_w(slice_inserter const& i)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline size_t c_str_len_a(slice_inserter const& i)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return i.length();
}
/** \overload c_str_len(slice_inserter const&) */
inline size_t c_str_len(slice_inserter const& i)
{
    return i.length();
}

/** \overload c_str_ptr_a(slice_inserter const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_ptr_w(slice_inserter const& i)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_ptr_a(slice_inserter const& i)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return i.c_str();
}
/** \overload c_str_ptr(slice_inserter const&) */
inline pan_char_t const* c_str_ptr(slice_inserter const& i)
{
    return i.c_str();
}

# if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace shims */

#  if defined(STLSOFT_COMPILER_IS_GCC)
    /* GCC does not seem to correctly handle the phases of
     * processing of C++ templates, so we need to 'use' the
     * shims into the same namespace as the inserter class
     * in order that ADL can suffice instead.
     */
#   ifdef PANTHEIOS_USE_WIDE_STRINGS
    using ::pantheios::shims::c_str_data_w;
    using ::pantheios::shims::c_str_len_w;
    using ::pantheios::shims::c_str_ptr_w;
#   else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    using ::pantheios::shims::c_str_data_a;
    using ::pantheios::shims::c_str_len_a;
    using ::pantheios::shims::c_str_ptr_a;
#   endif /* PANTHEIOS_USE_WIDE_STRINGS */
    using ::pantheios::shims::c_str_data;
    using ::pantheios::shims::c_str_len;
    using ::pantheios::shims::c_str_ptr;
#  endif /* compiler */

# endif /* !PANTHEIOS_NO_NAMESPACE */

#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */

namespace stlsoft
{
    // 'Export' the string access shims into the STLSoft namespace
    //
    // c_str_ptr(_a) is not necessary for version 1.0 of Pantheios, but it's
    // defined and exported in order to allow for the case where someone
    // may find a legitimate use for the conversion classes additional to
    // the type-tunneling of the Pantheios API.

# ifdef PANTHEIOS_USE_WIDE_STRINGS
    using ::pantheios::shims::c_str_data_w;
    using ::pantheios::shims::c_str_len_w;
    using ::pantheios::shims::c_str_ptr_w;
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    using ::pantheios::shims::c_str_data_a;
    using ::pantheios::shims::c_str_len_a;
    using ::pantheios::shims::c_str_ptr_a;
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
    using ::pantheios::shims::c_str_data;
    using ::pantheios::shims::c_str_len;
    using ::pantheios::shims::c_str_ptr;
}

#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Inclusion
 */

#ifdef STLSOFT_PPF_pragma_once_SUPPORT
# pragma once
#endif /* STLSOFT_PPF_pragma_once_SUPPORT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_PAD */

/* ///////////////////////////// end of file //////////////////////////// */
