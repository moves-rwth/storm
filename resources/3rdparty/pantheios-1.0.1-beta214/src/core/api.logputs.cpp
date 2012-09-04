/* /////////////////////////////////////////////////////////////////////////
 * File:        src/core/api.logputs.cpp
 *
 * Purpose:     Implementation file for Pantheios core API.
 *
 * Created:     21st June 2005
 * Updated:     18th March 2012
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


/* Pantheios header files
 *
 * NOTE: We do _not_ include pantheios/pantheios.hpp here, since we are
 *  not using any of the Application Layer.
 */
#include <pantheios/pantheios.h>
#include <pantheios/internal/lean.h>

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Core API
 *
 * Note: for those compilers that object to instantiating templates within
 * extern "C" functions, the actual functions and their implementations are
 * separated by the use of intermediary extern "C++" forms. For example,
 * pantheios_init() is implemented in terms of pantheios_init__cpp(). For
 * compilers that do not have such a problem, the intermediary forms are not
 * used
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace core
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* Defined here, for the moment, as not currently declared in pantheios.h */
PANTHEIOS_CALL(int) pantheios_dispatch(
    pan_sev_t           severity
,   size_t              cchEntry
,   pan_char_t const*   entry
);

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace core */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Core functions
 */

PANTHEIOS_CALL(void) pantheios_logputs(pan_sev_t severity, pan_char_t const* s)
{
#if !defined(PANTHEIOS_NO_NAMESPACE)
    using pantheios::core::pantheios_dispatch;
#endif /* !PANTHEIOS_NO_NAMESPACE */

    if(pantheios_isSeverityLogged(severity))
    {
        pan_slice_t slice(-1, s);

        pantheios_dispatch(severity, slice.len, slice.ptr);
    }
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
