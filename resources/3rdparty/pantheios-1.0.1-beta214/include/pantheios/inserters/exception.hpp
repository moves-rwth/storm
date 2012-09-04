/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters/exception.hpp
 *
 * Purpose:     String inserter for std::exception-derived types.
 *
 * Created:     22nd March 2010
 * Updated:     26th November 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/inserters/exception.hpp
 *
 * [C++ only] String inserter for std::exception-derived types.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_EXCEPTION
#define PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_EXCEPTION

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_EXCEPTION_MAJOR      1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_EXCEPTION_MINOR      0
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_EXCEPTION_REVISION   1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_EXCEPTION_EDIT       2
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

#ifndef STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_H_FWD
# include <stlsoft/shims/access/string/fwd.h>
#endif /* !STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_H_FWD */

#include <exception>

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

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

# if !defined(PANTHEIOS_NO_NAMESPACE)
namespace inserters
{
# endif /* !PANTHEIOS_NO_NAMESPACE */

class exception_inserter
{
public:
    typedef exception_inserter  class_type;

public:
    explicit exception_inserter(std::exception const& x);
private:
#if !defined(STLSOFT_COMPILER_IS_GCC)
//    exception_inserter(class_type const&);
#endif /* compiler */
    class_type& operator =(class_type const&);

public:
    pan_char_t const*   c_str() const;
    pan_char_t const*   data() const;
    size_t              length() const;

private:
    void construct_() const;
    void construct_();

private:
    pan_char_t*             m_value;
    size_t                  m_len;
    std::exception const&   m_x;
};

# if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace inserters */
# endif /* !PANTHEIOS_NO_NAMESPACE */

#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

/** Converts a reference to an exception to an instance of a type that
 *   allows the exception to be inserted into Pantheios diagnostic logging
 *   statements.
 */
#ifdef PANTHEIOS_USE_WIDE_STRINGS

inline
#  if !defined(PANTHEIOS_NO_NAMESPACE)
inserters::
#  endif /* !PANTHEIOS_NO_NAMESPACE */
exception_inserter exception(
    std::exception const& x
)
{
    return 
#  if !defined(PANTHEIOS_NO_NAMESPACE)
      inserters::
#  endif /* !PANTHEIOS_NO_NAMESPACE */
        exception_inserter(x);
}

#else /* ? PANTHEIOS_USE_WIDE_STRINGS */

inline std::exception const& exception(
    std::exception const& x
)
{
    return x;
}

#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* /////////////////////////////////////////////////////////////////////////
 * String Access Shims
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

# ifdef PANTHEIOS_USE_WIDE_STRINGS

#  if !defined(PANTHEIOS_NO_NAMESPACE)
namespace shims
{
    using inserters::exception_inserter;
#  endif /* !PANTHEIOS_NO_NAMESPACE */

/** \overload c_str_data_a(exception_inserter const&) */
inline wchar_t const* c_str_data_w(exception_inserter const& i)
{
    return i.data();
}
/** \overload c_str_data(exception_inserter const&) */
inline pan_char_t const* c_str_data(exception_inserter const& i)
{
    return i.data();
}

/** \overload c_str_len_a(exception_inserter const&) */
inline size_t c_str_len_w(exception_inserter const& i)
{
    return i.length();
}
/** \overload c_str_len(exception_inserter const&) */
inline size_t c_str_len(exception_inserter const& i)
{
    return i.length();
}

/** \overload c_str_ptr_a(exception_inserter const&) */
inline wchar_t const* c_str_ptr_w(exception_inserter const& i)
{
    return i.c_str();
}
/** \overload c_str_ptr(exception_inserter const&) */
inline pan_char_t const* c_str_ptr(exception_inserter const& i)
{
    return i.c_str();
}

#  if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace shims */

#   if defined(STLSOFT_COMPILER_IS_GCC)
    /* GCC does not seem to correctly handle the phases of
     * processing of C++ templates, so we need to 'use' the
     * shims into the same namespace as the inserter class
     * in order that ADL can suffice instead.
     */
    using ::pantheios::shims::c_str_data_w;
    using ::pantheios::shims::c_str_len_w;
    using ::pantheios::shims::c_str_ptr_w;
    using ::pantheios::shims::c_str_data;
    using ::pantheios::shims::c_str_len;
    using ::pantheios::shims::c_str_ptr;
#   endif /* compiler */

#  endif /* !PANTHEIOS_NO_NAMESPACE */

# endif /* PANTHEIOS_USE_WIDE_STRINGS */

#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */

# ifdef PANTHEIOS_USE_WIDE_STRINGS

namespace stlsoft
{
    // 'Export' the string access shims into the STLSoft namespace
    //
    // c_str_ptr(_w) is not necessary for version 1.0 of Pantheios, but it's
    // defined and exported in order to allow for the case where someone
    // may find a legitimate use for the conversion classes additional to
    // the type-tunneling of the Pantheios API.

    using ::pantheios::shims::c_str_data_w;
    using ::pantheios::shims::c_str_len_w;
    using ::pantheios::shims::c_str_ptr_w;
    using ::pantheios::shims::c_str_data;
    using ::pantheios::shims::c_str_len;
    using ::pantheios::shims::c_str_ptr;
}

# endif /* PANTHEIOS_USE_WIDE_STRINGS */

#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Inclusion
 */

#ifdef STLSOFT_PPF_pragma_once_SUPPORT
# pragma once
#endif /* STLSOFT_PPF_pragma_once_SUPPORT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_EXCEPTION */

/* ///////////////////////////// end of file //////////////////////////// */
