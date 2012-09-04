/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters/args.hpp
 *
 * Purpose:     String inserter for argc+argv pairs.
 *
 * Created:     19th October 2006
 * Updated:     5th May 2010
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


/** \file pantheios/inserters/args.hpp
 *
 * [C++ only] Definition of the pantheios::args inserter for
 *   <code>argc+argv</code> pairs.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_ARGS
#define PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_ARGS

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_ARGS_MAJOR       1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_ARGS_MINOR       6
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_ARGS_REVISION    4
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_ARGS_EDIT        28
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

#include <string>

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

/** Class for inserting <code>argc+argv</code> arguments into Pantheios
 *    diagnostic logging statements.
 *
 * \ingroup group__application_layer_interface__inserters
 *
 * This class formats an <code>argc+argv</code> pair into a string to
 * be inserted into a logging statement.
 *
 * By default, arguments that contain spaces (TAB or space character) will
 * be enclosed in double quotes, and those that do not will not. This
 * behaviour can be moderated by use of the
 * \link pantheios::args::neverQuoteArgs args::neverQuoteArgs\endlink
 * and
 * \link pantheios::args::alwaysQuoteArgs args::alwaysQuoteArgs\endlink
 * member constants.
 *
 * Consider the following statement:
 * \code
  int main(int argc, char* argv[])
  {
    pantheios::log_DEBUG("main(", args(argc, argv), ")");

    . . .
 * \endcode
 *
 * Suppose the program is <code>/test/myprog</code> and the command-line
 * arguments are <code>abc</code>, <code>-123</code> and
 * <code>Billy Jean</code>. This will produce the output:
\htmlonly
<pre>
   <b>/test/myprog, abc, -123, "Billy Jean"</b>
</pre>
\endhtmlonly
 *
 * We may specify different flags from the default
 * (\link pantheios::args::quoteArgsWithSpaces args::quoteArgsWithSpaces\endlink)
 * in the third constructor argument, as in:
 * \code
  int main(int argc, char* argv[])
  {
    pantheios::log_DEBUG("main(", pantheios::args(argc, argv, pantheios::args::neverQuoteArgs), ")");

    . . .
 * \endcode
 *
 * Using the same arguments, this will produce the output:
\htmlonly
<pre>
   <b>/test/myprog, abc, -123, Billy Jean</b>
</pre>
\endhtmlonly
 *
 */
class args
{
/// \name Member Types
/// @{
public:
    typedef args    class_type;
/// @}

/// \name Member Constants
/// @{
public:
    ///  The format flags
    enum format_flags
    {
            neverQuoteArgs      =   0x0000  /*!< arguments are never quoted */
        ,   quoteArgsWithSpaces =   0x0001  /*!< arguments are quoted if they contain spaces */
        ,   alwaysQuoteArgs     =   0x0002  /*!< arguments are always quoted */
        ,   arg0FileOnly        =   0x0004  /*!< causes only the file part of arg0 to be displayed */
    };
/// @}

/// \name Construction
/// @{
public:
    ///  Constructs an \link pantheios::args args\endlink inserter
    ///   that will display the command-line arguments.
    ///
    /// \param argc The number of arguments in the argument array.
    /// \param argv The array of arguments.
    /// \param flags \link pantheios::args::format_flags Flags\endlink that
    ///   control the formatting of the shim.
    /// \param separator String used to separate multiple arguments; defaults to ", "
    args(int argc, pan_char_t const* const* argv, int flags = quoteArgsWithSpaces, pan_char_t const* separator = PANTHEIOS_LITERAL_STRING(", "));

#ifdef STLSOFT_COMPILER_IS_BORLAND
    args(int argc, pan_char_t** argv, int flags = quoteArgsWithSpaces, pan_char_t const* separator = PANTHEIOS_LITERAL_STRING(", "));
#endif /* STLSOFT_COMPILER_IS_BORLAND */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
    ~args() stlsoft_throw_0();
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
/// @}

/// \name Accessors
/// @{
public:
    ///  A possibly non-nul-terminated non-null pointer to the c-style string representation of the integer
    pan_char_t const*   data() const;
    ///  A nul-terminated non-null pointer to the c-style string representation of the integer
    pan_char_t const*   c_str() const;
    ///  The size of the c-style string representation of the integer
    size_t              size() const;
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
    typedef std::basic_string<pan_char_t> string_type_;

    const int                   m_flags;
    const int                   m_argc;
    pan_char_t const* const*    m_argv;
    const string_type_          m_separator;
    string_type_                m_result;
/// @}

/// \name Not to be implemented
/// @{
private:
#if !defined(STLSOFT_COMPILER_IS_GCC)
    args(class_type const&);
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

/** \overload c_str_data_a(args const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_data_w(args const& a)
{
    return a.data();
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_data_a(args const& a)
{
    return a.data();
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** \overload c_str_data(args const&) */
inline pan_char_t const* c_str_data(args const& a)
{
    return a.data();
}

/** \overload c_str_len_a(args const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline size_t c_str_len_w(args const& a)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline size_t c_str_len_a(args const& a)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return a.size();
}
/** \overload c_str_len(args const&) */
inline size_t c_str_len(args const& a)
{
    return a.size();
}

/** \overload c_str_ptr_a(args const&) */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_ptr_w(args const& a)
{
    return a.c_str();
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_ptr_a(args const& a)
{
    return a.c_str();
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** \overload c_str_ptr(args const&) */
inline pan_char_t const* c_str_ptr(args const& a)
{
    return a.c_str();
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

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_ARGS */

/* ///////////////////////////// end of file //////////////////////////// */
