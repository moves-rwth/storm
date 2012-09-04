/* /////////////////////////////////////////////////////////////////////////
 * File:        src/util/strnlen.cpp
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

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace util */
#endif /* !PANTHEIOS_NO_NAMESPACE */
/* static */ size_t pan_slice_t::get_lazy_length(size_t fromLen, size_t toLen)
{
#if !defined(PANTHEIOS_NO_NAMESPACE)
    using namespace ::pantheios::util;
#endif /* !PANTHEIOS_NO_NAMESPACE */

    return pantheios_util_getSliceLazyLength(fromLen, toLen);
}
#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace util
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace util */
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
