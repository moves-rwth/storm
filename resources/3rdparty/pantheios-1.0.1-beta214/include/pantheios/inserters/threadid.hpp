/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters/threadid.hpp
 *
 * Purpose:     String inserter for thread identity.
 *
 * Created:     16th October 2006
 * Updated:     22nd March 2010
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


/** \file pantheios/inserters/threadid.hpp
 *
 * [C++ only] String inserter for thread identity.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_THREADID
#define PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_THREADID

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_THREADID_MAJOR       2
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_THREADID_MINOR       2
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_THREADID_REVISION    1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_THREADID_EDIT        18
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

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
struct threadId_t;
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/** Global instance that may be used to insert a thread identity into a log
 *   statement.
 *
 * \ingroup group__application_layer_interface__inserters
 * \code
  pantheios::log_NOTICE("thread id=", pantheios::threadId);
 * \endcode
 *
 * This will produce output such as the following:
\htmlonly
<pre>
   <b>thread id=1310</b>
</pre>
\endhtmlonly
 */
extern struct threadId_t const* threadId;

/* /////////////////////////////////////////////////////////////////////////
 * Inserter classes and String Access Shims
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

# if !defined(PANTHEIOS_NO_NAMESPACE)
namespace inserters
{
# endif /* !PANTHEIOS_NO_NAMESPACE */

class thread_id_t
{
public:
    typedef thread_id_t             class_type;
# if !defined(PANTHEIOS_NO_NAMESPACE)
    typedef ::pantheios::threadId_t id_type;
# else /* ? !PANTHEIOS_NO_NAMESPACE */
    typedef threadId_t              id_type;
# endif /* !PANTHEIOS_NO_NAMESPACE */

public:
    thread_id_t();

public:
    operator size_t () const;
    operator pan_char_t const* () const;

private:
    void construct_();
    void construct_() const;

private:
    pan_char_t  m_value[21];
    size_t      m_len;
};

# if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace inserters */
#  define PANTHEIOS_INSERTERS_DECLARE_thread_id_t   ::pantheios::inserters::thread_id_t
# else /* ? !PANTHEIOS_NO_NAMESPACE */
#  define PANTHEIOS_INSERTERS_DECLARE_thread_id_t   thread_id_t
# endif /* !PANTHEIOS_NO_NAMESPACE */

# if !defined(PANTHEIOS_NO_NAMESPACE)
namespace shims
{
# endif /* !PANTHEIOS_NO_NAMESPACE */

# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline PANTHEIOS_INSERTERS_DECLARE_thread_id_t c_str_data_w(PANTHEIOS_INSERTERS_DECLARE_thread_id_t::id_type const*)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline PANTHEIOS_INSERTERS_DECLARE_thread_id_t c_str_data_a(PANTHEIOS_INSERTERS_DECLARE_thread_id_t::id_type const*)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return PANTHEIOS_INSERTERS_DECLARE_thread_id_t();
}
inline PANTHEIOS_INSERTERS_DECLARE_thread_id_t c_str_data(PANTHEIOS_INSERTERS_DECLARE_thread_id_t::id_type const*)
{
    return PANTHEIOS_INSERTERS_DECLARE_thread_id_t();
}

# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline PANTHEIOS_INSERTERS_DECLARE_thread_id_t c_str_len_w(PANTHEIOS_INSERTERS_DECLARE_thread_id_t::id_type const*)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline PANTHEIOS_INSERTERS_DECLARE_thread_id_t c_str_len_a(PANTHEIOS_INSERTERS_DECLARE_thread_id_t::id_type const*)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return PANTHEIOS_INSERTERS_DECLARE_thread_id_t();
}
inline PANTHEIOS_INSERTERS_DECLARE_thread_id_t c_str_len(PANTHEIOS_INSERTERS_DECLARE_thread_id_t::id_type const*)
{
    return PANTHEIOS_INSERTERS_DECLARE_thread_id_t();
}

# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline PANTHEIOS_INSERTERS_DECLARE_thread_id_t c_str_ptr_w(PANTHEIOS_INSERTERS_DECLARE_thread_id_t::id_type const*)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline PANTHEIOS_INSERTERS_DECLARE_thread_id_t c_str_ptr_a(PANTHEIOS_INSERTERS_DECLARE_thread_id_t::id_type const*)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return PANTHEIOS_INSERTERS_DECLARE_thread_id_t();
}
inline PANTHEIOS_INSERTERS_DECLARE_thread_id_t c_str_ptr(PANTHEIOS_INSERTERS_DECLARE_thread_id_t::id_type const*)
{
    return PANTHEIOS_INSERTERS_DECLARE_thread_id_t();
}

# if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace shims */

#  if defined(STLSOFT_COMPILER_IS_GCC)
    /* GCC does not seem to correctly handle the phases of
     * processing of C++ templates, so we need to 'use' the
     * shims into the same namespace as the inserter class
     * in order that ADL can suffice instead.
     */
    using ::pantheios::shims::c_str_data_a;
    using ::pantheios::shims::c_str_data;

    using ::pantheios::shims::c_str_len_a;
    using ::pantheios::shims::c_str_len;

    using ::pantheios::shims::c_str_ptr_a;
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

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_THREADID */

/* ///////////////////////////// end of file //////////////////////////// */
