/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters/blob.hpp
 *
 * Purpose:     String inserter for binary regions.
 *
 * Created:     21st June 2005
 * Updated:     22nd March 2010
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


/** \file pantheios/inserters/blob.hpp
 *
 * [C++ only] Definition of the pantheios::blob inserter for binary
 *  regions.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_BLOB
#define PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_BLOB

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_BLOB_MAJOR       2
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_BLOB_MINOR       4
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_BLOB_REVISION    4
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_BLOB_EDIT        27
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

#ifndef PANTHEIOS_INCL_H_STDIO
# define PANTHEIOS_INCL_H_STDIO
# include <stdio.h>
#endif /* !PANTHEIOS_INCL_H_STDIO */

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

/** Class for inserting binary regions types into Pantheios diagnostic
 *   logging statements.
 *
 * \ingroup group__application_layer_interface__inserters
 *
 * This class formats a binary region into a string, thereby enabling it to
 * be inserted into a logging statement. Consider the following statement:
 * \code
  int         ar[2] = { 0x00112233, 0x44556677 };
  char        s[]   = "abc";
  std::string str("def");

  pantheios::log(pantheios::notice, "s=", s, ", blob=", pantheios::blob(ar, sizeof(ar)), ", str=", str);
 * \endcode
 *
 * This will produce the output:
\htmlonly
<pre>
   <b>s=abc, blob=0011223344556677, str=def</b>
</pre>
\endhtmlonly
 *
 * The bytes can be grouped and these groups separated. Consider the
 * following statement:
 * \code
  int         ar[2] = { 0x00112233, 0x44556677 };
  char        s[]   = "abc";
  std::string str("def");

  pantheios::log(pantheios::notice, "s=", s, ", blob=", pantheios::blob(ar, sizeof(ar), 2, "-"), ", str=", str);
 * \endcode
 *
 * This will produce the output:
 * \htmlonly
<pre>
   <b>s=abc, blob=2233-0011-6677-4455, str=def</b>
</pre>
\endhtmlonly
 *
 * The output can be split into lines. Consider the following statement:
 * \code
  int         ar[2] = { 0x00112233, 0x44556677 };
  char        s[]   = "abc";
  std::string str("def");

  pantheios::log(pantheios::notice, "s=", s, ", blob=", pantheios::blob(ar, sizeof(ar), 2, "-", 3, "\n\t"), ", str=", str);
 * \endcode
 *
 * This will produce the output:
\htmlonly
<pre>
  <b>s=abc, blob=2233-0011-6677
          4455, str=def</b>
</pre>
\endhtmlonly
 */
class blob
{
/// \name Member Types
/// @{
public:
    typedef blob    class_type;
/// @}

/// \name Construction
/// @{
public:
    ///  Constructs a \link pantheios::blob blob\endlink inserter
    ///   that will display as an unbroken sequence of hexadecimal digits.
    ///
    /// \param pv Pointer to the first byte in the sequence to be displayed.
    /// \param cb Number of bytes in the sequence.
    blob(   void const* pv
        ,   size_t      cb);

    ///  Constructs a \link pantheios::blob blob\endlink inserter
    ///   that will break the sequence of hexadecimal digits into groups
    ///   of the specified number of bytes, separating groups with the
    ///   specified separator.
    ///
    /// \param pv Pointer to the first byte in the sequence to be displayed.
    /// \param cb Number of bytes in the sequence.
    /// \param bytesPerGroup Number of bytes per group. Must be a power of 2.
    /// \param groupSeparator The string used to separate the byte groups.
    blob(   void const*         pv
        ,   size_t              cb
        ,   unsigned            bytesPerGroup
        ,   pan_char_t const*   groupSeparator);

    ///  Constructs a \link pantheios::blob blob\endlink inserter
    ///   that will break the sequence of hexadecimal digits into groups
    ///   of the specified number of bytes, separating groups with the
    ///   specified separator, and break into lines of the specified
    ///   number of groups, using the given line separator.
    ///
    /// \param pv Pointer to the first byte in the sequence to be displayed.
    /// \param cb Number of bytes in the sequence.
    /// \param bytesPerGroup Number of bytes per group. Must be a power of 2.
    /// \param groupSeparator The string used to separate the byte groups.
    /// \param groupsPerLine Number of groups per line.
    /// \param lineSeparator The string used to separate the lines.
    blob(   void const*         pv
        ,   size_t              cb
        ,   unsigned            bytesPerGroup
        ,   pan_char_t const*   groupSeparator
        ,   int                 groupsPerLine
        ,   pan_char_t const*   lineSeparator  =   PANTHEIOS_LITERAL_STRING("\n")
    );

    ///  Releases any storage associated with the instance.
    ~blob() stlsoft_throw_0();
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
/// @}

/// \name Member Variables
/// @{
private:
    pan_char_t const*   m_value;
    size_t              m_len;
    void const* const   m_pv;
    size_t              m_cb;
    unsigned            m_byteGrouping;
    pan_char_t const*   m_groupSeparator;
    int                 m_groupsPerLine;
    pan_char_t const*   m_lineSeparator;
/// @}

/// \name Not to be implemented
/// @{
private:
#if !defined(STLSOFT_COMPILER_IS_GCC)
    blob(class_type const&);
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

/** \overload c_str_data_a(blob const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_data_w(blob const& b)
{
    return b.data();
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_data_a(blob const& b)
{
    return b.data();
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** \overload c_str_data(blob const&) */
inline pan_char_t const* c_str_data(blob const& b)
{
    return b.data();
}

/** \overload c_str_len_a(blob const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline size_t c_str_len_w(blob const& b)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline size_t c_str_len_a(blob const& b)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return b.length();
}
/** \overload c_str_len(blob const&) */
inline size_t c_str_len(blob const& b)
{
    return b.length();
}

/** \overload c_str_ptr_a(blob const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_ptr_w(blob const& b)
{
    return b.c_str();
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_ptr_a(blob const& b)
{
    return b.c_str();
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** \overload c_str_ptr(blob const&) */
inline pan_char_t const* c_str_ptr(blob const& b)
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

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_BLOB */

/* ///////////////////////////// end of file //////////////////////////// */
