/* /////////////////////////////////////////////////////////////////////////
 * File:        src/inserters/boolean.cpp
 *
 * Purpose:     Implementation of the pantheios::boolean inserter class.
 *
 * Created:     7th August 2008
 * Updated:     18th June 2012
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2008-2012, Matthew Wilson and Synesis Software
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
#include <pantheios/inserters/boolean.hpp>
#include <pantheios/util/system/hostname.h>
#include <pantheios/util/string/strdup.h>
#include <pantheios/util/memory/auto_buffer_selector.hpp>

/* STLSoft Header Files */
#include <stlsoft/string/char_traits.hpp>

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

    using ::pantheios::core::pantheios_malloc;

#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

namespace
{

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

// Define pan_slice_pod_t, equivalent to pan_slice_t, in order to get
// static initialisation of value.

struct pan_slice_pod_t
{
    size_t              len;
    pan_char_t const*   ptr;
};

static const pan_char_t         s_falseString[]     =   { 'f', 'a', 'l', 's', 'e', '\0' };
static const pan_char_t         s_trueString[]      =   { 't', 'r', 'u', 'e', '\0' };

static const pan_char_t         s_falseVbString[]   =   { 'V', 'A', 'R', 'I', 'A', 'N', 'T', '_', 'F', 'A', 'L', 'S', 'E', '\0' };
static const pan_char_t         s_trueVbString[]    =   { 'V', 'A', 'R', 'I', 'A', 'N', 'T', '_', 'T', 'R', 'U', 'E', '\0' };

static const pan_slice_pod_t    s_defaultSlices[2] =
{
    {   STLSOFT_NUM_ELEMENTS(s_falseString) - 1,    s_falseString   },
    {   STLSOFT_NUM_ELEMENTS(s_trueString) - 1,     s_trueString    },
};
static const pan_slice_pod_t    s_defaultVbSlices[2] =
{
    {   STLSOFT_NUM_ELEMENTS(s_falseVbString) - 1,  s_falseVbString },
    {   STLSOFT_NUM_ELEMENTS(s_trueVbString) - 1,   s_trueVbString  },
};

static pan_slice_t  s_falseCustomSlice;
static pan_slice_t  s_trueCustomSlice;

static pan_slice_t const*       s_slices[2][2] =
{
    {
        reinterpret_cast<pan_slice_t const*>(&s_defaultSlices[0]),
        reinterpret_cast<pan_slice_t const*>(&s_defaultSlices[1])
    },
    {
        reinterpret_cast<pan_slice_t const*>(&s_defaultVbSlices[0]),
        reinterpret_cast<pan_slice_t const*>(&s_defaultVbSlices[1])
    }
};

typedef stlsoft::stlsoft_char_traits<pan_char_t>    char_traits_t;

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * Inserter classes
 */

/* static */ void boolean::set_value_strings(pan_char_t const* falseName, pan_char_t const* trueName) /* throw(std::bad_alloc) */
{
    if(NULL == falseName)
    {
#ifndef STLSOFT_CF_THROW_BAD_ALLOC
no_falseName:
#endif /* STLSOFT_CF_THROW_BAD_ALLOC */

        s_slices[0][0] = reinterpret_cast<pan_slice_t const*>(&s_defaultSlices[0]);
    }
    else
    {
        const size_t        cchFalseName    =   char_traits_t::length(falseName);
        pan_char_t* const   falseNameCopy   =   static_cast<pan_char_t*>(pantheios_malloc((1 + cchFalseName) * sizeof(pan_char_t)));

        if(NULL == falseNameCopy)
        {
#ifdef STLSOFT_CF_THROW_BAD_ALLOC
            throw std::bad_alloc();
#else /* ? STLSOFT_CF_THROW_BAD_ALLOC */
            goto no_falseName;
#endif /* STLSOFT_CF_THROW_BAD_ALLOC */
        }
        else
        {
//          pantheios_atExit(pantheios_free, falseNameCopy);

            ::memcpy(falseNameCopy, falseName, sizeof(pan_char_t) * (1 + cchFalseName));

            s_falseCustomSlice = pan_slice_t(falseNameCopy, cchFalseName);

            s_slices[0][0] = &s_falseCustomSlice;

            PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_INTERNAL((cchFalseName == s_slices[0][0]->len && 0 == char_traits_t::compare(falseName, s_slices[0][0]->ptr, cchFalseName)), "failed to duplicate false name");
        }
    }

    if(NULL == trueName)
    {
#ifndef STLSOFT_CF_THROW_BAD_ALLOC
no_trueName:
#endif /* STLSOFT_CF_THROW_BAD_ALLOC */

        s_slices[0][1] = reinterpret_cast<pan_slice_t const*>(&s_defaultSlices[1]);
    }
    else
    {
        const size_t        cchTrueName     =   char_traits_t::length(trueName);
        pan_char_t* const   trueNameCopy    =   static_cast<pan_char_t*>(pantheios_malloc((1 + cchTrueName) * sizeof(pan_char_t)));

        if(NULL == trueNameCopy)
        {
#ifdef STLSOFT_CF_THROW_BAD_ALLOC
            throw std::bad_alloc();
#else /* ? STLSOFT_CF_THROW_BAD_ALLOC */
            goto no_trueName;
#endif /* STLSOFT_CF_THROW_BAD_ALLOC */
        }
        else
        {
            ::memcpy(trueNameCopy, trueName, sizeof(pan_char_t) * (1 + cchTrueName));

            s_trueCustomSlice = pan_slice_t(trueNameCopy, cchTrueName);

            s_slices[0][1] = &s_trueCustomSlice;

            PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_INTERNAL((cchTrueName == s_slices[0][1]->len && 0 == char_traits_t::compare(trueName, s_slices[0][1]->ptr, cchTrueName)), "failed to duplicate true name");
        }
    }
}

/* static */ pan_slice_t const* boolean::get_slice_(int index, bool value)
{
    STLSOFT_STATIC_ASSERT(sizeof(pan_slice_pod_t) == sizeof(pan_slice_t));
    STLSOFT_ASSERT(index >= 0);
    STLSOFT_ASSERT(index <= 1);

    index = index & 0x01;

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_APPL_LAYER(NULL != s_slices[index][0], "the 'false' slice pointer may not be NULL");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_APPL_LAYER(NULL != s_slices[index][1], "the 'true' slice pointer may not be NULL");

    return s_slices[index][false != value];
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
