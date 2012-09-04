/* /////////////////////////////////////////////////////////////////////////
 * File:        src/inserters/b64.cpp
 *
 * Purpose:     Implementation of the b64 inserter class.
 *
 * Created:     31st July 2006
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
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ////////////////////////////////////////////////////////////////////// */


#define PANTHEIOS_NO_INCLUDE_STLSOFT_STRING_ACCESS

/* Pantheios Header Files */
#include <pantheios/pantheios.h>

#include <pantheios/inserters/b64.hpp>
#include <pantheios/quality/contract.h>

#ifdef PANTHEIOS_USE_WIDE_STRINGS
# include <pantheios/util/memory/auto_buffer_selector.hpp>
# include <stlsoft/conversion/char_conversions.hpp>
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* b64 Header Files */
#include <b64/b64.h>

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{

    using ::pantheios::core::pantheios_inserterAllocate;
    using ::pantheios::core::pantheios_inserterDeallocate;
# ifdef PANTHEIOS_USE_WIDE_STRINGS
    using ::pantheios::util::auto_buffer_selector;
# endif /* PANTHEIOS_USE_WIDE_STRINGS */

#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * b64
 */

b64::b64(   void const* pv
        ,   size_t      cb)
    : m_value(NULL)
    , m_len(0)
    , m_pv(pv)
    , m_cb(cb)
    , m_flags(0)
    , m_lineLen(-1)
    , m_rc(NULL)
{}

b64::b64(   void const* pv
        ,   size_t      cb
        ,   unsigned    flags)
    : m_value(NULL)
    , m_len(0)
    , m_pv(pv)
    , m_cb(cb)
    , m_flags(flags)
    , m_lineLen(-1)
    , m_rc(NULL)
{}

b64::b64(   void const*     pv
        ,   size_t          cb
        ,   unsigned        flags
        ,   int             lineLen
        ,   b64::B64_RC*    rc)
    : m_value(NULL)
    , m_len(0)
    , m_pv(pv)
    , m_cb(cb)
    , m_flags(flags)
    , m_lineLen(lineLen)
    , m_rc(rc)
{}

// TODO: create a Pantheios fast arena

b64::~b64() stlsoft_throw_0()
{
    pantheios_inserterDeallocate(const_cast<pan_char_t*>(m_value));
}


inline void b64::construct_() const
{
    const_cast<class_type*>(this)->construct_();
}

pan_char_t const* b64::data() const
{
    if(NULL == m_value)
    {
        construct_();
    }

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(NULL != m_value);

    return m_value;
#else /* ? STLSOFT_CF_EXCEPTION_SUPPORT */
    return (NULL != m_value) ? m_value : "";
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

pan_char_t const* b64::c_str() const
{
    return data();
}

size_t b64::length() const
{
    if(NULL == m_value)
    {
        construct_();
    }

    return m_len;
}

void b64::construct_()
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL == m_value, "cannot construct if value is non-null");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(0 == m_len, "cannot construct if length is non-0");

    size_t      cch     =   b64_api::b64_encode2(m_pv, m_cb, NULL, 0, m_flags, m_lineLen, m_rc);
    pan_char_t* value   =   static_cast<pan_char_t*>(pantheios_inserterAllocate(sizeof(pan_char_t) * (1 + cch)));

    if(NULL != value)
    {
        // When invoked with a buffer, b64_encode2() may return less (but never
        // more) than when invoked without.

#ifdef PANTHEIOS_USE_WIDE_STRINGS

# if defined(PANTHEIOS_NO_NAMESPACE)
        auto_buffer_selector<
# else /* ? PANTHEIOS_NO_NAMESPACE */
        ::pantheios::util::auto_buffer_selector<
# endif /* PANTHEIOS_NO_NAMESPACE */
            char
        ,   256
        >::type     buff(1 + cch);

        if(buff.empty())
        {
            pantheios_inserterDeallocate(const_cast<pan_char_t*>(m_value));

            value   =   NULL;
            cch     =   0;
        }
        else
        {
            cch = b64_api::b64_encode2(m_pv, m_cb, &buff[0], cch, m_flags, m_lineLen, m_rc);
            buff[cch] = '\0';

# if defined(STLSOFT_COMPILER_IS_MSVC) && \
     _MSC_VER >= 1400
#  pragma warning(push)
#  pragma warning(disable : 4996)
# endif
            ::mbstowcs(value, buff.data(), cch);
# if defined(STLSOFT_COMPILER_IS_MSVC) && \
     _MSC_VER >= 1400
#  pragma warning(pop)
# endif
            value[cch] = '\0';
        }

#else /* ? PANTHEIOS_USE_WIDE_STRINGS */

        cch = b64_api::b64_encode2(m_pv, m_cb, value, cch, m_flags, m_lineLen, m_rc);
        value[cch] = '\0';

#endif /* PANTHEIOS_USE_WIDE_STRINGS */

        m_value =   value;
        m_len   =   cch;
    }
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
