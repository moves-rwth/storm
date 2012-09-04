/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters/boolean.hpp
 *
 * Purpose:     String inserter for booleans.
 *
 * Created:     3rd August 2008
 * Updated:     18th June 2012
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2008-2012, Matthew Wilson and Synesis Software
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
 * - Neither the names of Matthew Wilson and Synesis Software nor the names
 *   of any contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
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


/** \file pantheios/inserters/boolean.hpp
 *
 * [C++ only] String inserter for booleans.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_BOOLEAN
#define PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_BOOLEAN

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_BOOLEAN_MAJOR    1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_BOOLEAN_MINOR    3
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_BOOLEAN_REVISION 3
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_BOOLEAN_EDIT     17
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

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#else /* ? !PANTHEIOS_NO_NAMESPACE */
# if defined(__RPCNDR_H__)
#  error pantheios::boolean may not be used with Windows when namespace is suspended. If you are including pantheios/inserters.hpp, try including only those specific inserter files that you need
# endif /* rpcndr.h */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

/** Class for inserting Boolean types into Pantheios diagnostic logging
 *   statements.
 *
 * \ingroup group__application_layer_interface__inserters
 *
 * Consider the following statement:
 *
 * \code
  char        s[] = "abc";
  std::string str("def");
  bool        b   = false;

  pantheios::log(pantheios::notice, "s=", s, ", b=", pantheios::boolean(b), ", str=", str);
 * \endcode
 *
 * This will produce the output:
 *
 * &nbsp;&nbsp;&nbsp;&nbsp;<b>s=abc, b=false, str=def</b>
 *
 * \note The class provides the static method set_value_strings() for
 *   assigning user-defined strings to represent true and false values.
 */
class boolean
{
/// \name Member Types
/// @{
public:
    typedef boolean     class_type;
/// @}

/// \name Construction
/// @{
public:
    /// Construct from a boolean value
    ///
    /// \param value The boolean whose value will be represented as a string
    explicit boolean(bool value)
        : m_value(value)
    {}
private:
    class_type& operator =(class_type const&);
/// @}

/// \name Control
/// @{
public:
    static void set_value_strings(pan_char_t const* falseName, pan_char_t const* trueName) /* throw(std::bad_alloc) */;
/// @}

/// \name Accessors
/// @{
public:
    ///  A possibly non-nul-terminated non-null pointer to the c-style string representation of the integer
    pan_char_t const* data() const
    {
        return get_slice_(m_value)->ptr;
    }
    ///  A nul-terminated non-null pointer to the c-style string representation of the integer
    pan_char_t const* c_str() const
    {
        return get_slice_(m_value)->ptr;
    }
    ///  The length of the c-style string representation of the integer
    size_t      length() const
    {
        return get_slice_(m_value)->len;
    }
/// @}

/// \name Implementation
/// @{
private:
    virtual
    pan_slice_t const* get_slice_(bool value) const
    {
        return get_slice_(0, value);
    }
protected:
    static pan_slice_t const* get_slice_(
        int     index
    ,   bool    value
    );
/// @}

/// \name Member Variables
/// @{
protected:
    const bool m_value;
/// @}
};

/* /////////////////////////////////////////////////////////////////////////
 * Implementation
 */

/* /////////////////////////////////////////////////////////////////////////
 * String Access Shims
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

# if !defined(PANTHEIOS_NO_NAMESPACE)
namespace shims
{
# endif /* !PANTHEIOS_NO_NAMESPACE */

# ifdef PANTHEIOS_USE_WIDE_STRINGS
/** \overload c_str_data_w(boolean const&) */
inline wchar_t const* c_str_data_w(boolean const& b)
{
    return b.data();
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
/** \overload c_str_data_a(boolean const&) */
inline char const* c_str_data_a(boolean const& b)
{
    return b.data();
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** \overload c_str_data(boolean const&) */
inline pan_char_t const* c_str_data(boolean const& b)
{
    return b.data();
}

# ifdef PANTHEIOS_USE_WIDE_STRINGS
/** \overload c_str_len_w(boolean const&) */
inline size_t c_str_len_w(boolean const& b)
{
    return b.length();
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
/** \overload c_str_len_a(boolean const&) */
inline size_t c_str_len_a(boolean const& b)
{
    return b.length();
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** \overload c_str_len(boolean const&) */
inline size_t c_str_len(boolean const& b)
{
    return b.length();
}

# ifdef PANTHEIOS_USE_WIDE_STRINGS
/** \overload c_str_ptr_w(boolean const&) */
inline wchar_t const* c_str_ptr_w(boolean const& b)
{
    return b.c_str();
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
/** \overload c_str_ptr_a(boolean const&) */
inline char const* c_str_ptr_a(boolean const& b)
{
    return b.c_str();
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** \overload c_str_ptr(boolean const&) */
inline pan_char_t const* c_str_ptr(boolean const& b)
{
    return b.c_str();
}

# if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace shims */

#  if defined(STLSOFT_COMPILER_IS_GCC)
    /* GCC does not seem to correctly handle the phases of
     * processing of C++ templates, so we need to 'use' the
     * shims into the same namespace as the inserter class
     * in order that ADL can suffice instead.
     */
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

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_BOOLEAN */

/* ///////////////////////////// end of file //////////////////////////// */
