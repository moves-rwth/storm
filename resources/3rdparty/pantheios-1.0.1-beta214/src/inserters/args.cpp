/* /////////////////////////////////////////////////////////////////////////
 * File:        inserters/args.cpp
 *
 * Purpose:     Implementation of the inserter classes.
 *
 * Created:     16th October 2006
 * Updated:     10th August 2009
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2006-2009, Matthew Wilson and Synesis Software
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
#include <pantheios/internal/nox.h>

#include <pantheios/inserters/args.hpp>
#include <pantheios/quality/contract.h>
#include <pantheios/internal/safestr.h>

/* STLSoft Header Files */
#include <stlsoft/shims/access/string/std/c_string.h>
#include <platformstl/platformstl.h>

/* /////////////////////////////////////////////////////////////////////////
 * Warning suppression
 */

#if defined(STLSOFT_COMPILER_IS_BORLAND)
# pragma warn -8008
# pragma warn -8066
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * String encoding compatibility
 */

#ifdef PANTHEIOS_USE_WIDE_STRINGS
# define pan_strrchr_                   ::wcsrchr
# define pan_strpbrk_                   ::wcspbrk
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
# define pan_strrchr_                   ::strrchr
# define pan_strpbrk_                   ::strpbrk
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * args
 */

inline void args::construct_() const
{
    const_cast<class_type*>(this)->construct_();
}

args::args(int argc, pan_char_t const* const* argv, int flags /* = quoteArgsWithSpaces */, pan_char_t const* separator /* = ", " */)
    : m_flags(flags)
    , m_argc(argc)
    , m_argv(argv)
    , m_separator(separator)
    , m_result()
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(argc >= 0, "argument count must not be negative");
}

#ifdef STLSOFT_COMPILER_IS_BORLAND
args::args(int argc, pan_char_t** argv, int flags /* = quoteArgsWithSpaces */, pan_char_t const* separator /* = ", " */)
    : m_flags(flags)
    , m_argc(argc)
    , m_argv(argv)
    , m_separator(separator)
    , m_result()
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(argc >= 0, "argument count must not be negative");
}
#endif /* STLSOFT_COMPILER_IS_BORLAND */

args::~args() stlsoft_throw_0() // This is defined so that the destructors for the member variables are executed in the same link unit
{}

pan_char_t const* args::data() const
{
    if(m_result.empty())
    {
        construct_();
    }

    return m_result.data();
}

size_t args::size() const
{
    if(m_result.empty())
    {
        construct_();
    }

    return m_result.size();
}

void args::construct_()
{
    m_result.reserve(20 * static_cast<size_t>(m_argc));

    { for(int i = 0; i < m_argc; ++i)
    {
        pan_char_t const* arg = ::stlsoft::c_str_ptr(m_argv[i]); // ensure arg is never NULL

        if(0 == i)
        {
            if(arg0FileOnly == (m_flags & arg0FileOnly))
            {
                pan_char_t const* slash   =   pan_strrchr_(arg, PANTHEIOS_LITERAL_CHAR('/'));
#ifdef PLATFORMSTL_OS_IS_WINDOWS
                pan_char_t const* bslash  =   pan_strrchr_(arg, PANTHEIOS_LITERAL_CHAR('\\'));

                if(NULL == slash)
                {
                    slash = bslash;
                }
                else if(NULL != bslash)
                {
                    if(slash < bslash)
                    {
                        slash = bslash;
                    }
                }
#endif /* PLATFORMSTL_OS_IS_WINDOWS */

                if(NULL != slash)
                {
                    arg = slash + 1;
                }
            }
        }
        else
        {
            m_result += m_separator;
        }

        if( alwaysQuoteArgs == (m_flags & alwaysQuoteArgs) ||
            (   quoteArgsWithSpaces == (m_flags & quoteArgsWithSpaces) &&
                NULL != pan_strpbrk_(arg, PANTHEIOS_LITERAL_STRING(" \t"))))
        {
            m_result += PANTHEIOS_LITERAL_CHAR('"');
            m_result += arg;
            m_result += PANTHEIOS_LITERAL_CHAR('"');
        }
        else
        {
            m_result += arg;
        }
    }}
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
