/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters/processid.hpp
 *
 * Purpose:     String inserter for process identity.
 *
 * Created:     16th October 2006
 * Updated:     14th February 2010
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


/** \file pantheios/inserters/processid.hpp
 *
 * [C++ only] String inserter for process identity.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_PROCESSID
#define PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_PROCESSID

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_PROCESSID_MAJOR      2
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_PROCESSID_MINOR      2
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_PROCESSID_REVISION   3
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_PROCESSID_EDIT       19
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
 * Typedefs
 */

/** Global instance that may be used to insert a process identity into a log
 *   statement.
 *
 * \ingroup group__application_layer_interface__inserters
 * \code
  pantheios::log_NOTICE("process id=", pantheios::processId);
 * \endcode
 *
 * This will produce output such as the following:
\htmlonly
<pre>
   <b>process id=2436</b>
</pre>
\endhtmlonly
 */
extern struct processId_t const* processId;

/* /////////////////////////////////////////////////////////////////////////
 * Worker functions
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

PANTHEIOS_CALL(pan_char_t const*) pantheios_getCurrentProcessIdString(void);
PANTHEIOS_CALL(size_t)      pantheios_getCurrentProcessIdStringLength(void);

PANTHEIOS_CALL_DEPRECATED(pan_char_t const*, pantheios_processId, pantheios_getCurrentProcessIdString) pantheios_processId(void);
PANTHEIOS_CALL_DEPRECATED(size_t, pantheios_processIdLength, pantheios_getCurrentProcessIdStringLength) pantheios_processIdLength(void);

#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * String Access Shims
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

# if !defined(PANTHEIOS_NO_NAMESPACE)
namespace shims
{
# endif /* !PANTHEIOS_NO_NAMESPACE */

# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_data_w(struct processId_t const*)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_data_a(struct processId_t const*)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return pantheios_getCurrentProcessIdString();
}
inline pan_char_t const* c_str_data(struct processId_t const*)
{
    return pantheios_getCurrentProcessIdString();
}

# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline size_t c_str_len_w(struct processId_t const*)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline size_t c_str_len_a(struct processId_t const*)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return pantheios_getCurrentProcessIdStringLength();
}
inline size_t c_str_len(struct processId_t const*)
{
    return pantheios_getCurrentProcessIdStringLength();
}

# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_ptr_w(struct processId_t const*)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_ptr_a(struct processId_t const*)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return pantheios_getCurrentProcessIdString();
}
inline pan_char_t const* c_str_ptr(struct processId_t const*)
{
    return pantheios_getCurrentProcessIdString();
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

#ifdef PANTHEIOS_USE_WIDE_STRINGS
    using ::pantheios::shims::c_str_data_w;
    using ::pantheios::shims::c_str_len_w;
    using ::pantheios::shims::c_str_ptr_w;
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    using ::pantheios::shims::c_str_data_a;
    using ::pantheios::shims::c_str_len_a;
    using ::pantheios::shims::c_str_ptr_a;
#endif /* PANTHEIOS_USE_WIDE_STRINGS */
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

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_PROCESSID */

/* ///////////////////////////// end of file //////////////////////////// */
