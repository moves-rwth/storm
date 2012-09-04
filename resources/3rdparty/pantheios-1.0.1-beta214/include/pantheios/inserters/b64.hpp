/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters/b64.hpp
 *
 * Purpose:     String inserter for binary regions in Base-64.
 *
 * Created:     31st July 2006
 * Updated:     23rd July 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2006-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/inserters/b64.hpp
 *
 * [C++ only; requires the
 *   <a href = "http://www.synesis.com.au/software/b64.html">b64</a>
 *   library] Definition of the pantheios::b64 inserter for binary regions.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_B64
#define PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_B64

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_B64_MAJOR    1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_B64_MINOR    4
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_B64_REVISION 3
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_B64_EDIT     23
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


/* b64 Header Files */
#ifdef PANTHEIOS_NO_NAMESPACE

 /* If the pantheios namespace is suppressed, the pantheios::b64 type will
  * be placed into the global namespace. This will clash with the b64
  * namespace.
  *
  * To obviate this problem, we define the b64 namespace to b64_api via the
  * b64 custom namespace preprocessor symbol B64_CUSTOM_NAMESPACE.
  *
  * To be able to do this requires v1.3.1 (or later) of b64, so that is
  * asserted also.
  */

# ifdef B64_INCL_B64_H_B64
#  error Cannot include b64/b64.h before pantheios/inserters/b64.hpp
# endif /* B64_INCL_B64_H_B64 */

# define B64_CUSTOM_NAMESPACE      b64_api

#endif /* PANTHEIOS_NO_NAMESPACE */

#include <b64/b64.h>    // If your compiler can't see this, you may be
                        // missing the b64 library. Download from
                        // http://www.synesis.com.au/software/b64.html



#ifdef PANTHEIOS_NO_NAMESPACE

# if !defined(B64_VER) || \
     B64_VER < 0x010301ff
#  error Version 1.3.1 (or later) of b64 is required
# endif /* B64_VER */

#else /* ? PANTHEIOS_NO_NAMESPACE */

 /* If we're not customising the b64 namespace, then we use a namespace
  * alias to get b64_api, which we need in order to disambiguate
  * pantheios::b64 with the pantheios namespace.
  */

namespace b64_api = ::b64;

#endif /* PANTHEIOS_NO_NAMESPACE */


/* STLSoft Header Files */
#ifndef STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_H_FWD
# include <stlsoft/shims/access/string/fwd.h>
#endif /* !STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_H_FWD */

/* Standard C Header Files */
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

  pantheios::log(pantheios::notice, "s=", s, ", b64=", pantheios::b64(ar, sizeof(ar)), ", str=", str);
 * \endcode
 *
 * This will produce the output:
\htmlonly
<pre>
   <b>s=abc, b64=0011223344556677, str=def</b>
</pre>
\endhtmlonly
 *
 * The bytes can be grouped and these groups separated. Consider the
 * following statement:
 * \code
  int         ar[2] = { 0x00112233, 0x44556677 };
  char        s[]   = "abc";
  std::string str("def");

  pantheios::log(pantheios::notice, "s=", s, ", b64=", pantheios::b64(ar, sizeof(ar), 2, "-"), ", str=", str);
 * \endcode
 *
 * This will produce the output:
 * \htmlonly
<pre>
   <b>s=abc, b64=2233-0011-6677-4455, str=def</b>
</pre>
\endhtmlonly
 *
 * The output can be split into lines. Consider the following statement:
 * \code
  int         ar[2] = { 0x00112233, 0x44556677 };
  char        s[]   = "abc";
  std::string str("def");

  pantheios::log(pantheios::notice, "s=", s, ", b64=", pantheios::b64(ar, sizeof(ar), 2, "-", 3, "\n\t"), ", str=", str);
 * \endcode
 *
 * This will produce the output:
\htmlonly
<pre>
  <b>s=abc, b64=2233-0011-6677
          4455, str=def</b>
</pre>
\endhtmlonly
 */
class b64
{
/// \name Member Types
/// @{
public:
    typedef b64             class_type;
    typedef b64_api::B64_RC B64_RC;
/// @}

/// \name Construction
/// @{
public:
    b64(    void const* pv
        ,   size_t      cb);

    b64(    void const* pv
        ,   size_t      cb
        ,   unsigned    flags);

    b64(    void const* pv
        ,   size_t      cb
        ,   unsigned    flags
        ,   int         lineLen
        ,   B64_RC*     rc = NULL);

    ~b64() stlsoft_throw_0();
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
    void const*         m_pv;
    size_t              m_cb;
    unsigned            m_flags;
    int                 m_lineLen;
    B64_RC*             m_rc;
/// @}

/// \name Not to be implemented
/// @{
private:
#if !defined(STLSOFT_COMPILER_IS_GCC)
    b64(class_type const&);
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

/** \overload c_str_data_a(b64 const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_data_w(b64 const& i)
{
    return i.data();
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_data_a(b64 const& i)
{
    return i.data();
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** \overload c_str_data(b64 const&) */
inline pan_char_t const* c_str_data(b64 const& i)
{
    return i.data();
}

/** \overload c_str_len_a(b64 const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline size_t c_str_len_w(b64 const& i)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline size_t c_str_len_a(b64 const& i)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return i.length();
}
/** \overload c_str_len(b64 const&) */
inline size_t c_str_len(b64 const& i)
{
    return i.length();
}

/** \overload c_str_ptr_a(b64 const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_ptr_w(b64 const& i)
{
    return i.c_str();
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_ptr_a(b64 const& i)
{
    return i.c_str();
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** \overload c_str_ptr(b64 const&) */
inline pan_char_t const* c_str_ptr(b64 const& i)
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

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_B64 */

/* ///////////////////////////// end of file //////////////////////////// */
