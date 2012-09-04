/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters/real.hpp
 *
 * Purpose:     String inserters for fundamental types
 *
 * Created:     21st June 2005
 * Updated:     14th February 2010
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


/** \file pantheios/inserters/real.hpp
 *
 * [C++ only] Definition of the pantheios::real string inserter for
 *   for floating-point built-in types.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_REAL
#define PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_REAL

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_REAL_MAJOR       2
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_REAL_MINOR       3
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_REAL_REVISION    4
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_REAL_EDIT        26
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_FMT
# include <pantheios/inserters/fmt.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_FMT */

#ifndef STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_H_FWD
# include <stlsoft/shims/access/string/fwd.h>
#endif /* !STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_H_FWD */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{

#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Inserter classes
 */

/** Class for inserting floating-point types into Pantheios diagnostic
 *   logging statements.
 *
 * \ingroup group__application_layer_interface__inserters
 *
 * This class converts a floating-point variable into a string, thereby enabling it to be
 * inserted into a logging statement. Consider the following statement
 *
 * \code
  double      d   = 123.456;
  char        s[] = "abc";
  std::string str("def");

  pantheios::log(pantheios::notice, "s=", s, ", d=", pantheios::real(d), ", str=", str);
 * \endcode
 *
 * This will produce the output:
 *
 * &nbsp;&nbsp;&nbsp;&nbsp;<b>s=abc, d=123.456, str=def</b>
 *
 * \note Currently, Pantheios does not support the implicit insertion of
 * floating-point types in diagnostic logging statements, due to the various
 * ambiguities inherent in the C++ language. (See chapters 14, 15, 19, 24 of
 * <a href = "http://imperfectcplusplus.com" target="_blank">Imperfect C++</a>
 * for discussions of these issues.) It is possible that a future version of
 * the library will be able to incorporate them directly, so long as that does
 * not sacrifice Pantheios's central claim of not paying for what you don't use.
 */
class real
{
public:
    /// This type
    typedef real    class_type;

public:
    /// Construct from a float value
    ///
    /// \param value The value to be inserted
    /// \param widthAndFormat Not currently used
    explicit real(float value, int widthAndFormat = 0);
    /// Construct from a double value
    ///
    /// \param value The value to be inserted
    /// \param widthAndFormat Not currently used
    explicit real(double value, int widthAndFormat = 0);
    /// Construct from a long double value
    ///
    /// \param value The value to be inserted
    /// \param widthAndFormat Not currently used
    explicit real(long double value, int widthAndFormat = 0);

private:
    explicit real(int); // Prevents implicit conversion from an integer

public:
    /// A possibly non-nul-terminated non-null pointer to the c-style string representation of the floating-point number
    pan_char_t const*   data() const;
    /// A nul-terminated non-null pointer to the c-style string representation of the floating-point number
    pan_char_t const*   c_str() const;
    /// The length of the c-style string representation of the floating-point number
    size_t              length() const;

private:
    void construct_() const;
    void construct_();

private:
    enum RealSize
    {
            typeIsFloat         =   -1
        ,   typeIsDouble        =   -2
        ,   typeIsLongDouble    =   -3
    };

    union u
    {
        float       fValue;
        double      dValue;
        long double ldValue;
    };

    u           m_value;            // The value, copied for lazy conversion
    size_t      m_len;              // Length, and marker for type: 1 == float; 2 == double; 3 == long double
    pan_char_t  m_sz[256];          // Marker for converted, if m_sz[0] == '\0'
    const int   m_widthAndFormat;

private:
#if !defined(STLSOFT_COMPILER_IS_GCC)
    real(class_type const&);
#endif /* compiler */
    class_type& operator =(class_type const&);
};

/* /////////////////////////////////////////////////////////////////////////
 * String Access Shims
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

# if !defined(PANTHEIOS_NO_NAMESPACE)
namespace shims
{
# endif /* !PANTHEIOS_NO_NAMESPACE */

/** \overload c_str_data_a(real const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_data_w(real const& r)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_data_a(real const& r)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return r.data();
}
/** \overload c_str_data(real const&) */
inline pan_char_t const* c_str_data(real const& r)
{
    return r.data();
}

/** \overload c_str_len_a(real const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline size_t c_str_len_w(real const& r)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline size_t c_str_len_a(real const& r)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return r.length();
}
/** \overload c_str_len(real const&) */
inline size_t c_str_len(real const& r)
{
    return r.length();
}

/** \overload c_str_ptr_a(real const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_ptr_w(real const& r)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_ptr_a(real const& r)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return r.c_str();
}
/** \overload c_str_ptr(real const&) */
inline pan_char_t const* c_str_ptr(real const& r)
{
    return r.c_str();
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

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_REAL */

/* ///////////////////////////// end of file //////////////////////////// */
