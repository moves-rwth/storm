/* /////////////////////////////////////////////////////////////////////////
 * File:        src/core/api.exitprocess.cpp
 *
 * Purpose:     Implementation file for Pantheios core API.
 *
 * Created:     21st June 2005
 * Updated:     20th March 2012
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

#include <platformstl/platformstl.h>

#if defined(PLATFORMSTL_OS_IS_WINDOWS)
# include <windows.h>
#else /* ? OS */
# include <stdlib.h>
# include <unistd.h>
#endif /* OS */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Core functions
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace core
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

PANTHEIOS_CALL(void) pantheios_exitProcess(int code)
{
#if defined(PLATFORMSTL_OS_IS_WINDOWS)

    // By rights, Windows programs should use exit() to close down. However,
    // pantheios_exit_process() is not a general substitute for exit(). It is
    // specifically intended to Get-Out-Of-Dodge asap, in the rare case where
    // Pantheios cannot be initialised.
    //
    // NOTE: May need to revisit this in the future, in cases where a
    // sophisticated back-end, e.g. log4cxx, does not initialise.

    ::ExitProcess(static_cast<DWORD>(code));

#else /* ? operating system */

     ::_exit(code);

#endif /* operating system */
}

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace core */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
