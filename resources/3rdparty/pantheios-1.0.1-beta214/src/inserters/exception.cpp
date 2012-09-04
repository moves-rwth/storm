/* /////////////////////////////////////////////////////////////////////////
 * File:        inserters/exception.cpp
 *
 * Purpose:     String inserter for std::exception-derived types.
 *
 * Created:     22nd March 2010
 * Updated:     22nd March 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2010, Matthew Wilson and Synesis Software
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

#include <pantheios/inserters/exception.hpp>

/* STLSoft Header Files */

#include <stlsoft/conversion/char_conversions.hpp>

/* Standard C Header Files */
#if defined(STLSOFT_COMPILER_IS_BORLAND)
# include <memory.h>
#endif /* compiler */
#include <string.h>

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
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace inserters
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Inserter classes
 */

// exception_inserter

inline void exception_inserter::construct_() const
{
    const_cast<class_type*>(this)->construct_();
}

void exception_inserter::construct_()
{
    stlsoft::m2w t(m_x.what());

#ifndef STLSOFT_CF_EXCEPTION_SUPPORT
    if(0u != t.size())
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        ;

#ifndef STLSOFT_CF_EXCEPTION_SUPPORT
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

/* explicit */ exception_inserter::exception_inserter(std::exception const& x)
    : m_value(NULL)
    , m_len(0)
    , m_x(x)
{}

pan_char_t const* exception_inserter::c_str() const
{
    return data();
}

pan_char_t const* exception_inserter::data() const
{
    if('\0' == m_value[0])
    {
        construct_();
    }

    return m_value;
}

size_t exception_inserter::length() const
{
    if('\0' == m_value[0])
    {
        construct_();
    }

    return m_len;
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace inserters */

} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
