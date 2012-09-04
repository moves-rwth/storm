/* /////////////////////////////////////////////////////////////////////////
 * File:        inserters/real.cpp
 *
 * Purpose:     Implementation of the inserter classes.
 *
 * Created:     21st June 2005
 * Updated:     10th August 2009
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2009, Matthew Wilson and Synesis Software
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


#define PANTHEIOS_NO_INCLUDE_STLSOFT_STRING_ACCESS

/* Pantheios Header Files */
#include <pantheios/pantheios.h>
#include <pantheios/internal/nox.h>

#if defined(STLSOFT_COMPILER_IS_MWERKS)
 /* For some to-be-determined reason, CodeWarrior can't see a whole lot of
  * things in STLSoft header files from <string.h>, even though those
  * files #include it.
  */
# include <string.h>
#endif /* compiler */

#include <pantheios/inserters/real.hpp>
#include <pantheios/util/string/snprintf.h>

#include <pantheios/quality/contract.h>

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
 * real
 */

/* explicit */ real::real(float value, int widthAndFormat /* = 0 */)
    : m_len(static_cast<size_t>(typeIsFloat))
    , m_widthAndFormat(widthAndFormat)
{
    m_value.fValue  =   value;
    m_sz[0]         =   '\0';
}

/* explicit */ real::real(double value, int widthAndFormat /* = 0 */)
    : m_len(static_cast<size_t>(typeIsDouble))
    , m_widthAndFormat(widthAndFormat)
{
    m_value.dValue  =   value;
    m_sz[0]         =   '\0';
}

/* explicit */ real::real(long double value, int widthAndFormat /* = 0 */)
    : m_len(static_cast<size_t>(typeIsLongDouble))
    , m_widthAndFormat(widthAndFormat)
{
    m_value.ldValue =   value;
    m_sz[0]         =   '\0';
}


pan_char_t const* real::data() const
{
    if(0 == m_sz[0])
    {
        construct_();
    }

    return m_sz;
}

pan_char_t const* real::c_str() const
{
    return data();
}

size_t real::length() const
{
    if(0 == m_sz[0])
    {
        construct_();
    }

    return m_len;
}

void real::construct_() const
{
    const_cast<class_type*>(this)->construct_();
}

void real::construct_()
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(0 == m_sz[0], "cannot construct if value is non-empty");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(static_cast<int>(m_len) < 0, "cannot construct if length is not a type marker");

    switch(static_cast<RealSize>(m_len))
    {
        case    typeIsFloat:
            m_len = static_cast<size_t>(pantheios_util_snprintf(&m_sz[0]
                                    ,   STLSOFT_NUM_ELEMENTS(m_sz)
                                    ,   PANTHEIOS_LITERAL_STRING("%g")
                                    ,   m_value.fValue));
            break;
        default:
            PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_APPL_LAYER("unexpected type marker");
            // Fall through
        case    typeIsDouble:
            m_len = static_cast<size_t>(pantheios_util_snprintf(&m_sz[0]
                                    ,   STLSOFT_NUM_ELEMENTS(m_sz)
                                    ,   PANTHEIOS_LITERAL_STRING("%g")
                                    ,   m_value.dValue));
            break;
        case    typeIsLongDouble:
            m_len = static_cast<size_t>(pantheios_util_snprintf(&m_sz[0]
                                    ,   STLSOFT_NUM_ELEMENTS(m_sz)
                                    ,   PANTHEIOS_LITERAL_STRING("%g")
                                    ,   double(m_value.ldValue)));
            break;
    }

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(0 != m_sz[0], "failed to set value to non-empty");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(static_cast<int>(m_len) > 0, "failed to set length");
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
