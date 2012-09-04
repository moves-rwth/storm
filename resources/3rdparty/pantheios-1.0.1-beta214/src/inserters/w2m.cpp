/* /////////////////////////////////////////////////////////////////////////
 * File:        src/inserters/w2m.cpp
 *
 * Purpose:     Implementation of the pantheios::w2m inserter class.
 *
 * Created:     7th August 2008
 * Updated:     22nd November 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2008-2010, Matthew Wilson and Synesis Software
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


#define PANTHEIOS_NO_INCLUDE_STLSOFT_STRING_ACCESS

/* Pantheios Header Files */
#include <pantheios/pantheios.h>
#ifdef PANTHEIOS_USE_WIDE_STRINGS
# error This file can only be used in multibyte string builds
#endif /* PANTHEIOS_USE_WIDE_STRINGS */
#include <pantheios/internal/nox.h>

#include <pantheios/inserters/w2m.hpp>
#include <pantheios/internal/safestr.h>

/* Standard C Header Files */
#include <string.h>
#include <wchar.h>

    #include <stdio.h>

/* /////////////////////////////////////////////////////////////////////////
 * Warning suppression
 */

#if defined(STLSOFT_COMPILER_IS_BORLAND)
# pragma warn -8008
# pragma warn -8066
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{

    using ::pantheios::core::pantheios_inserterAllocate;
    using ::pantheios::core::pantheios_inserterDeallocate;

#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Inserter classes
 */

w2m::w2m(wchar_t const* s)
    : m_source(s)
    , m_sourceLen(sentinelLength_())
    , m_result(NULL)
    , m_length(0)
{}

w2m::w2m(wchar_t const* s, size_t len)
    : m_source(s)
    , m_sourceLen(len)
    , m_result(NULL)
    , m_length(0)
{}

int w2m::init_(wchar_t const* s, size_t n)
{
    m_source    =   s;
    m_sourceLen =   n;
    m_result    =   NULL;
    m_length    =   0;

    return 0;
}

w2m::~w2m() stlsoft_throw_0()
{
    pantheios_inserterDeallocate(m_result);
}

inline void w2m::construct_() const
{
    const_cast<w2m*>(this)->construct_();
}

void w2m::construct_()
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_APPL_LAYER(NULL == m_result, "cannot already be initialised");

    if(sentinelLength_() == m_sourceLen)
    {
        m_sourceLen = (NULL != m_source) ? ::wcslen(m_source) : 0;
    }

    if(0 != m_sourceLen)
    {
        // TODO: Verify that no multibyte encoding can be longer than 4 x widestring length
        size_t cb = sizeof(char) * (m_sourceLen * 4);

        m_result = static_cast<char*>(pantheios_inserterAllocate(cb));

        if(NULL != m_result)
        {
#ifdef PANTHEIOS_USING_SAFE_STR_FUNCTIONS
            size_t len;

            if(0 != ::wcstombs_s(&len, m_result, cb, m_source, m_sourceLen))
            {
                pantheios_inserterDeallocate(m_result);

                m_result = NULL;
            }
            else
            {
                m_result[len] = '\0';

                m_length = len - 1;
            }
#else /* PANTHEIOS_USING_SAFE_STR_FUNCTIONS */
            size_t len = ::wcstombs(m_result, m_source, cb);

            PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_INTERNAL((size_t(-1) == len || len < cb), "insufficient buffer");

            if(size_t(-1) == len)
            {
                pantheios_inserterDeallocate(m_result);

                m_result = NULL;
            }
            else
            {
                m_result[len] = '\0';

                m_length = len;
            }
#endif /* PANTHEIOS_USING_SAFE_STR_FUNCTIONS */
        }
    }
}

char const* w2m::data() const
{
    if(NULL == m_result)
    {
        construct_();
    }

    return m_result;
}

char const* w2m::c_str() const
{
    return data();
}

size_t w2m::length() const
{
    if(NULL == m_result)
    {
        construct_();
    }

    return m_length;
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
