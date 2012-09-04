/* /////////////////////////////////////////////////////////////////////////
 * File:        src/util/util.strnlen.c
 *
 * Purpose:     Implementation file for pantheios_strnlen() and related
 *              functions.
 *
 * Created:     21st June 2005
 * Updated:     31st July 2012
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2012, Matthew Wilson and Synesis Software
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


/* Pantheios Header Files
 *
 * NOTE: We do _not_ include pantheios/pantheios.hpp here, since we are
 *  not using any of the Application Layer.
 */

#include <pantheios/pantheios.h>

#if defined(STLSOFT_COMPILER_IS_MWERKS)
# include <string.h>
#endif /* compiler */

#include <pantheios/quality/contract.h>
#include <pantheios/internal/safestr.h>

/* STLSoft header files */

#include <stlsoft/stlsoft.h>

/* Standard C++ header files */

/* Standard C header files */

#include <string.h>
#ifdef PANTHEIOS_USE_WIDE_STRINGS
# include <wchar.h>
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* /////////////////////////////////////////////////////////////////////////
 * Warning suppression
 */

#if defined(STLSOFT_COMPILER_IS_BORLAND)
# pragma warn -8080
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * String encoding compatibility
 */

#ifdef PANTHEIOS_USE_WIDE_STRINGS
# define pan_strlen_                    wcslen
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
# define pan_strlen_                    strlen
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
namespace util
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Util API
 */

/* TODO: put this declaration into pantheios/pantheios.h */
#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
/** [Currently undocumented] Calculates the lazy length of a slice.
 *
 * \note THIS FUNCTION IS NOT PART OF THE PUBLICLY DOCUMENTED API OF
 *   PANTHEIOS, AND IS SUBJECT TO REMOVAL/CHANGE IN A FUTURE RELEASE.
 */
PANTHEIOS_CALL(size_t) pantheios_util_getSliceLazyLength(
    size_t  fromLen
,   size_t  toLen
);
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

PANTHEIOS_CALL(size_t) pantheios_util_getSliceLazyLength(size_t fromLen, size_t toLen)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_INTERNAL(fromLen <= 32767, "from length is too large");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(toLen <= 32767, "to length is too large");

    {

    static size_t const topBit = stlsoft_static_cast(size_t, 0x01) << (sizeof(size_t) * 8 - 1);

    return topBit | (fromLen & 0x7fff) | ((toLen & 0x7fff) << 15);

    }
}

/* deprecated */
PANTHEIOS_CALL(size_t) pantheios_strnlen(pan_char_t const* s, size_t len)
{
    return pantheios_util_strnlen(s, len);
}

PANTHEIOS_CALL(size_t) pantheios_util_strnlen(pan_char_t const* s, size_t len)
{
    static size_t const topBit     =   stlsoft_static_cast(size_t, 0x01) << (sizeof(size_t) * 8 - 1);
    static size_t const nextTopBit =   stlsoft_static_cast(size_t, 0x01) << (sizeof(size_t) * 8 - 2);

    if(topBit & len)
    {
        /* Top bit is set. This can mean one of two things:
         *
         * 1. strlen() is requested. This is indicated by next top bit
         *    being set (e.g. if len == (size_t)-1
         * 2. min/max strlen is requested. This is indicated by next top
         *    bit being not set.
         */

        if(nextTopBit & len)
        {
            /* 1. strlen() is requested. */
            return (NULL != s) ? pan_strlen_(s) : 0;
        }
        else
        {
            /* 2. min/max strlen is requested. */

            /* This requires some smarts */

            /* fromLen is lowest 15 bits */
            size_t const        fromLen =   len & 0x7fff;

            /* toLen is bit16-bit30 */
            size_t const        toLen   =   (len >> 15)  & 0x7fff;

            pan_char_t const*   from    =   s + fromLen;
            pan_char_t const*   to      =   s + toLen;
            pan_char_t const*   nul;

            if(fromLen < toLen)
            {
                /* Search forwards */
                for(nul = from; nul != to; ++nul)
                {
                    if('\0' == *nul)
                    {
                        break;
                    }
                }
            }
            else if(fromLen == toLen)
            {
                nul = s + fromLen;
            }
            else
            {
                nul = from;

                /* Search backwards */
                do
                {
                    if('\0' == *from)
                    {
                        nul = from;

                        break;
                    }
                }
                while(--from != to);
            }

            return stlsoft_static_cast(size_t, nul - s);
        }
    }
    else
    {
        /* No top bit set, so nothing special. Just return
         * the length as given.
         */

        return len;
    }
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace util */
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
