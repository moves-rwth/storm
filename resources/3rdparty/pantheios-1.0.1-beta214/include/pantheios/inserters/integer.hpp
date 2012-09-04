/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters/integer.hpp
 *
 * Purpose:     String inserters for fundamental types
 *
 * Created:     21st June 2005
 * Updated:     18th October 2012
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2012, Matthew Wilson and Synesis Software
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


/** \file pantheios/inserters/integer.hpp
 *
 * [C++ only] Definition of the pantheios::integer string inserter for
 *   for integral built-in types.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_INTEGER
#define PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_INTEGER

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_INTEGER_MAJOR    2
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_INTEGER_MINOR    5
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_INTEGER_REVISION 5
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_INTEGER_EDIT     37
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

/** Class for inserting integral types into Pantheios diagnostic logging
 *   statements.
 *
 * \ingroup group__application_layer_interface__inserters
 *
 * This class converts an integer into a string, thereby enabling it to be
 * inserted into a logging statement. Consider the following statement
 *
 * \code
  int         i   = 123;
  char        s[] = "abc";
  std::string str("def");

  pantheios::log(pantheios::notice, "s=", s, ", i=", pantheios::integer(i), ", str=", str);
 * \endcode
 *
 * This will produce the output:
 *
 * &nbsp;&nbsp;&nbsp;&nbsp;<b>s=abc, i=123, str=def</b>
 *
 * The constructor takes a second, defaulted (to 0), parameter, which allows
 * a width (up to a maximum of 255) and a format to be specified. The low
 * 8 bits of this parameter are interpreted as an unsigned integer specifying
 * the width. The remaining bits are treated as bit flags that control the
 *
 * \note Currently, Pantheios does not support the insertion of integral types
 * in diagnostic logging statements, due to the various ambiguities inherent
 * in the C++ language. (See chapters 14, 15, 19, 24 of
 * <a href = "http://imperfectcplusplus.com" target="_blank">Imperfect C++</a>
 * for discussions of these issues.) It is possible that a future version of
 * the library will be able to incorporate them directly, so long as that does
 * not sacrifice Pantheios's central claim of not paying for what you don't use.
 */
class integer
{
/// \name Member Types
/// @{
public:
    typedef integer     class_type;
/// @}

/// \name Implementation
/// @{
private:
    size_t init_(::stlsoft::sint8_t i);
    size_t init_(::stlsoft::uint8_t i);
    size_t init_(::stlsoft::sint16_t i);
    size_t init_(::stlsoft::uint16_t i);
    size_t init_(::stlsoft::sint32_t i);
    size_t init_(::stlsoft::uint32_t i);
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    size_t init_(::stlsoft::sint64_t i);
    size_t init_(::stlsoft::uint64_t i);
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
#ifdef STLSOFT_CF_INT_DISTINCT_INT_TYPE
    size_t init_(int i);
    size_t init_(unsigned int i);
#endif /* STLSOFT_CF_INT_DISTINCT_INT_TYPE */
#ifdef STLSOFT_CF_LONG_DISTINCT_INT_TYPE
    size_t init_(long i);
    size_t init_(unsigned long i);
#endif /* STLSOFT_CF_LONG_DISTINCT_INT_TYPE */
/// @}

/// \name Construction
/// @{
public:
    /// [DEPRECATED] Construct from an 8-bit signed integer
    ///
    /// \deprecated This function is now deprecated, and will be removed from a future
    ///   version of Pantheios; instead use either the 1-parameter or 3-parameter overload.
    explicit integer(::stlsoft::sint8_t i, int widthAndFormat);
    ///  [DEPRECATED] Construct from an 8-bit unsigned integer
    ///
    /// \deprecated This function is now deprecated, and will be removed from a future
    ///   version of Pantheios; instead use either the 1-parameter or 3-parameter overload.
    explicit integer(::stlsoft::uint8_t i, int widthAndFormat);
    ///  [DEPRECATED] Construct from a 16-bit signed integer
    ///
    /// \deprecated This function is now deprecated, and will be removed from a future
    ///   version of Pantheios; instead use either the 1-parameter or 3-parameter overload.
    explicit integer(::stlsoft::sint16_t i, int widthAndFormat);
    ///  [DEPRECATED] Construct from a 16-bit unsigned integer
    ///
    /// \deprecated This function is now deprecated, and will be removed from a future
    ///   version of Pantheios; instead use either the 1-parameter or 3-parameter overload.
    explicit integer(::stlsoft::uint16_t i, int widthAndFormat);
    ///  [DEPRECATED] Construct from a 32-bit signed integer
    ///
    /// \deprecated This function is now deprecated, and will be removed from a future
    ///   version of Pantheios; instead use either the 1-parameter or 3-parameter overload.
    explicit integer(::stlsoft::sint32_t i, int widthAndFormat);
    ///  [DEPRECATED] Construct from a 32-bit unsigned integer
    ///
    /// \deprecated This function is now deprecated, and will be removed from a future
    ///   version of Pantheios; instead use either the 1-parameter or 3-parameter overload.
    explicit integer(::stlsoft::uint32_t i, int widthAndFormat);
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    ///  [DEPRECATED] Construct from a 64-bit signed integer
    ///
    /// \deprecated This function is now deprecated, and will be removed from a future
    ///   version of Pantheios; instead use either the 1-parameter or 3-parameter overload.
    explicit integer(::stlsoft::sint64_t i, int widthAndFormat);
    ///  [DEPRECATED] Construct from a 64-bit unsigned integer
    ///
    /// \deprecated This function is now deprecated, and will be removed from a future
    ///   version of Pantheios; instead use either the 1-parameter or 3-parameter overload.
    explicit integer(::stlsoft::uint64_t i, int widthAndFormat);
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
#ifdef STLSOFT_CF_INT_DISTINCT_INT_TYPE
    ///  [DEPRECATED] Construct from an int
    ///
    /// \deprecated This function is now deprecated, and will be removed from a future
    ///   version of Pantheios; instead use either the 1-parameter or 3-parameter overload.
    explicit integer(int i, int widthAndFormat);
    ///  [DEPRECATED] Construct from an unsigned int
    ///
    /// \deprecated This function is now deprecated, and will be removed from a future
    ///   version of Pantheios; instead use either the 1-parameter or 3-parameter overload.
    explicit integer(unsigned int i, int widthAndFormat);
#endif /* STLSOFT_CF_INT_DISTINCT_INT_TYPE */
#ifdef STLSOFT_CF_LONG_DISTINCT_INT_TYPE
    ///  [DEPRECATED] Construct from a long
    ///
    /// \deprecated This function is now deprecated, and will be removed from a future
    ///   version of Pantheios; instead use either the 1-parameter or 3-parameter overload.
    explicit integer(long i, int widthAndFormat);
    ///  [DEPRECATED] Construct from an unsigned long
    ///
    /// \deprecated This function is now deprecated, and will be removed from a future
    ///   version of Pantheios; instead use either the 1-parameter or 3-parameter overload.
    explicit integer(unsigned long i, int widthAndFormat);
#endif /* STLSOFT_CF_LONG_DISTINCT_INT_TYPE */

    ///  Construct from an 8-bit signed integer
    explicit integer(::stlsoft::sint8_t i);
    ///  Construct from an 8-bit unsigned integer
    explicit integer(::stlsoft::uint8_t i);
    ///  Construct from a 16-bit signed integer
    explicit integer(::stlsoft::sint16_t i);
    ///  Construct from a 16-bit unsigned integer
    explicit integer(::stlsoft::uint16_t i);
    ///  Construct from a 32-bit signed integer
    explicit integer(::stlsoft::sint32_t i);
    ///  Construct from a 32-bit unsigned integer
    explicit integer(::stlsoft::uint32_t i);
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    ///  Construct from a 64-bit signed integer
    explicit integer(::stlsoft::sint64_t i);
    ///  Construct from a 64-bit unsigned integer
    explicit integer(::stlsoft::uint64_t i);
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
#ifdef STLSOFT_CF_INT_DISTINCT_INT_TYPE
    ///  Construct from an int
    explicit integer(int i);
    ///  Construct from an unsigned int
    explicit integer(unsigned int i);
#endif /* STLSOFT_CF_INT_DISTINCT_INT_TYPE */
#ifdef STLSOFT_CF_LONG_DISTINCT_INT_TYPE
    ///  Construct from a long
    explicit integer(long i);
    ///  Construct from an unsigned long
    explicit integer(unsigned long i);
#endif /* STLSOFT_CF_LONG_DISTINCT_INT_TYPE */

    ///  Construct from an 8-bit signed integer
    ///
    /// \param i The integer to convert to string form
    /// \param minWidth The minimum width of the converted form. Must be in
    ///   the range [-128,127]
    /// \param format The format used in the conversion
    explicit integer(::stlsoft::sint8_t i, int minWidth, int format);
    ///  Construct from an 8-bit unsigned integer
    ///
    /// \param i The integer to convert to string form
    /// \param minWidth The minimum width of the converted form. Must be in
    ///   the range [-128,127]
    /// \param format The format used in the conversion
    explicit integer(::stlsoft::uint8_t i, int minWidth, int format);
    ///  Construct from a 16-bit signed integer
    ///
    /// \param i The integer to convert to string form
    /// \param minWidth The minimum width of the converted form. Must be in
    ///   the range [-128,127]
    /// \param format The format used in the conversion
    explicit integer(::stlsoft::sint16_t i, int minWidth, int format);
    ///  Construct from a 16-bit unsigned integer
    ///
    /// \param i The integer to convert to string form
    /// \param minWidth The minimum width of the converted form. Must be in
    ///   the range [-128,127]
    /// \param format The format used in the conversion
    explicit integer(::stlsoft::uint16_t i, int minWidth, int format);
    ///  Construct from a 32-bit signed integer
    ///
    /// \param i The integer to convert to string form
    /// \param minWidth The minimum width of the converted form. Must be in
    ///   the range [-128,127]
    /// \param format The format used in the conversion
    explicit integer(::stlsoft::sint32_t i, int minWidth, int format);
    ///  Construct from a 32-bit unsigned integer
    ///
    /// \param i The integer to convert to string form
    /// \param minWidth The minimum width of the converted form. Must be in
    ///   the range [-128,127]
    /// \param format The format used in the conversion
    explicit integer(::stlsoft::uint32_t i, int minWidth, int format);
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    ///  Construct from a 64-bit signed integer
    ///
    /// \param i The integer to convert to string form
    /// \param minWidth The minimum width of the converted form. Must be in
    ///   the range [-128,127]
    /// \param format The format used in the conversion
    explicit integer(::stlsoft::sint64_t i, int minWidth, int format);
    ///  Construct from a 64-bit unsigned integer
    ///
    /// \param i The integer to convert to string form
    /// \param minWidth The minimum width of the converted form. Must be in
    ///   the range [-128,127]
    /// \param format The format used in the conversion
    explicit integer(::stlsoft::uint64_t i, int minWidth, int format);
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
#ifdef STLSOFT_CF_INT_DISTINCT_INT_TYPE
    ///  Construct from an int
    ///
    /// \param i The integer to convert to string form
    /// \param minWidth The minimum width of the converted form. Must be in
    ///   the range [-128,127]
    /// \param format The format used in the conversion
    explicit integer(int i, int minWidth, int format);
    ///  Construct from an unsigned int
    ///
    /// \param i The integer to convert to string form
    /// \param minWidth The minimum width of the converted form. Must be in
    ///   the range [-128,127]
    /// \param format The format used in the conversion
    explicit integer(unsigned int i, int minWidth, int format);
#endif /* STLSOFT_CF_INT_DISTINCT_INT_TYPE */
#ifdef STLSOFT_CF_LONG_DISTINCT_INT_TYPE
    ///  Construct from a long
    ///
    /// \param i The integer to convert to string form
    /// \param minWidth The minimum width of the converted form. Must be in
    ///   the range [-128,127]
    /// \param format The format used in the conversion
    explicit integer(long i, int minWidth, int format);
    ///  Construct from an unsigned long
    ///
    /// \param i The integer to convert to string form
    /// \param minWidth The minimum width of the converted form. Must be in
    ///   the range [-128,127]
    /// \param format The format used in the conversion
    explicit integer(unsigned long i, int minWidth, int format);
#endif /* STLSOFT_CF_LONG_DISTINCT_INT_TYPE */
/// @}

/// \name Accessors
/// @{
public:
    ///  A possibly non-nul-terminated non-null pointer to the c-style string representation of the integer
    pan_char_t const*   data() const;
    ///  A nul-terminated non-null pointer to the c-style string representation of the integer
    pan_char_t const*   c_str() const;
    ///  The length of the c-style string representation of the integer
    size_t              length() const;
/// @}

/// \name Implementation
/// @{
private:
    void construct_() const;
    void construct_();

    static int validate_width_(int minWidth);
/// @}

/// \name Member Variables
/// @{
private:
    union u
    {
        ::stlsoft::sint32_t      s32;   //  typeIsS32
        ::stlsoft::uint32_t      u32;   //  typeIsU32
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
        ::stlsoft::sint64_t      s64;   //  typeIsS64
        ::stlsoft::uint64_t      u64;   //  typeIsU64
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
    };

    u           m_value;    // The value, copied for lazy conversion
    size_t      m_len;      // Length, and marker for type: 1 == float; 2 == double; 3 == long double
    const int   m_minWidth; // The minimum width
    const int   m_format;   // The format
    pan_char_t  m_sz[129];  // Marker for converted, if m_sz[0] == '\0'
/// @}

/// \name Not to be implemented
/// @{
private:
#if !defined(STLSOFT_COMPILER_IS_GCC)
    integer(class_type const&);
#endif /* compiler */
    class_type& operator =(class_type const&);
/// @}
};

/* /////////////////////////////////////////////////////////////////////////
 * String Access Shims
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

# if !defined(PANTHEIOS_NO_NAMESPACE)
namespace shims
{
# endif /* !PANTHEIOS_NO_NAMESPACE */

/** \overload c_str_data_a(integer const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_data_w(integer const& i)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_data_a(integer const& i)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return i.data();
}
/** \overload c_str_data(integer const&) */
inline pan_char_t const* c_str_data(integer const& i)
{
    return i.data();
}

/** \overload c_str_len_a(integer const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline size_t c_str_len_w(integer const& i)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline size_t c_str_len_a(integer const& i)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return i.length();
}
/** \overload c_str_len(integer const&) */
inline size_t c_str_len(integer const& i)
{
    return i.length();
}

/** \overload c_str_ptr_a(integer const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_ptr_w(integer const& i)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_ptr_a(integer const& i)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return i.c_str();
}
/** \overload c_str_ptr(integer const&) */
inline pan_char_t const* c_str_ptr(integer const& i)
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

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_INTEGER */

/* ///////////////////////////// end of file //////////////////////////// */
