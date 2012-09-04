/* /////////////////////////////////////////////////////////////////////////
 * File:        inserters/slice.cpp
 *
 * Purpose:     Implementation of the inserter classes.
 *
 * Created:     14th February 2010
 * Updated:     5th August 2012
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2010-2012, Matthew Wilson and Synesis Software
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
#include <pantheios/internal/safestr.h>

#if defined(STLSOFT_COMPILER_IS_MWERKS)
 /* For some to-be-determined reason, CodeWarrior can't see a whole lot of
  * things in STLSoft header files from <string.h>, even though those
  * files #include it.
  */
# include <string.h>
#endif /* compiler */

#include <pantheios/inserters/slice.hpp>

#include <pantheios/quality/contract.h>

/* STLSoft header files */

#include <stlsoft/conversion/integer_to_string.hpp>
#include <stlsoft/iterators/cstring_concatenator_iterator.hpp>
#include <stlsoft/iterators/member_selector_iterator.hpp>

/* Standard C++ header files */

#include <algorithm>
#include <numeric>

/* Standard C Header Files */

#include <string.h>

/* /////////////////////////////////////////////////////////////////////////
 * Warning suppression
 */

#if defined(STLSOFT_COMPILER_IS_BORLAND)
# pragma warn -8008
# pragma warn -8066
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * Compatibility
 */

#ifdef PANTHEIOS_USING_SAFE_STR_FUNCTIONS
# include <stlsoft/algorithms/std/alt.hpp>
namespace std
{
    using stlsoft::std_copy;

# define copy std_copy

} // namespace std
#endif /* PANTHEIOS_USING_SAFE_STR_FUNCTIONS */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * slice
 */

slice_inserter::slice_inserter(
    pan_char_t const*   str
,   size_t              len
,   pan_char_t const*   strName
,   pan_char_t const*   lenName
,   pan_char_t const*   equals
,   pan_char_t const*   separator
)
    : m_str(str)
    , m_len(len)
    , m_strName(strName)
    , m_lenName(lenName)
    , m_equals(equals)
    , m_separator(separator)
    , m_buffer(0u)
{}

inline slice_inserter::slice_inserter(slice_inserter::class_type const& rhs)
    : m_str(rhs.m_str)
    , m_len(rhs.m_len)
    , m_strName(rhs.m_strName)
    , m_lenName(rhs.m_lenName)
    , m_equals(rhs.m_equals)
    , m_separator(rhs.m_separator)
    , m_buffer(0u)
{}

inline void slice_inserter::construct_() const
{
    const_cast<class_type*>(this)->construct_();
}

pan_char_t const* slice_inserter::data() const
{
    if(0u == m_buffer.size())
    {
        construct_();
    }

    return m_buffer.data();
}

pan_char_t const* slice_inserter::c_str() const
{
    return data();
}

size_t slice_inserter::length() const
{
    if(0u == m_buffer.size())
    {
        construct_();
    }

    return m_buffer.size();
}

void slice_inserter::construct_()
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(0u == m_buffer.size(), "cannot construct if value is non-empty");

    pan_slice_t         slices[7];
    pan_slice_t const   equals(-1, (NULL != m_equals) ? m_equals : PANTHEIOS_LITERAL_STRING("="));
    pan_char_t          num[21];

    slices[0] = pan_slice_t(-1, m_strName);         // 0: str name
    slices[2] = pan_slice_t(m_str, m_len);          // 2: str
    slices[4] = pan_slice_t(-1, m_lenName);         // 4: len name

    if(0u != slices[0].len)
    {
        slices[1] = equals;                         // 1: equals (for str)
    }
    if(0u != slices[4].len)
    {
        pan_char_t const* separator = (NULL != m_separator) ? m_separator : PANTHEIOS_LITERAL_STRING(", ");

        slices[3] = pan_slice_t(-1, separator);     // 3: separator
        slices[5] = equals;                         // 5: equals (for len)

        size_t              lenLen;
        pan_char_t const*   lenPtr = stlsoft::integer_to_string(&num[0], STLSOFT_NUM_ELEMENTS(num), m_len, &lenLen);

        slices[6] = pan_slice_t(lenPtr, lenLen);
    }

    // calc buffer size, resize, and write all slices into buffer

#if !defined(STLSOFT_COMPILER_IS_BORLAND)

    const size_t n = std::accumulate(   stlsoft::member_selector(slices, &pan_slice_t::len)
                                    ,   stlsoft::member_selector(slices + STLSOFT_NUM_ELEMENTS(slices), &pan_slice_t::len)
                                    ,   size_t(0));

#else /* ? compiler */

    // The crappy way, for less-than compilers
    size_t n = 0;

    { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(slices); ++i)
    {
        n += slices[i].len;
    }}

#endif /* compiler */

    m_buffer.resize(n);

    size_t nWritten = 0;

    std::copy(  slices
            ,   slices + STLSOFT_NUM_ELEMENTS(slices)
            ,   stlsoft::cstring_concatenator(&m_buffer[0], &nWritten));

    PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_INTERNAL(nWritten == n, "Written length differs from allocated length");

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(0 != m_buffer[0], "failed to set value to non-empty");
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
