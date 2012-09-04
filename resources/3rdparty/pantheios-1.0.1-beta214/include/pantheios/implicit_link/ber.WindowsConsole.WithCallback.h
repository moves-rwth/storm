/* /////////////////////////////////////////////////////////////////////////
 * File:    pantheios/implicit_link/ber.WindowsConsole.WithCallback.h
 *
 * Purpose: Implicitly links in the Pantheios Windows Console Remote Back-End Library
 *
 * Created: 4th January 2006
 * Updated: 10th August 2009
 *
 * Home:    http://pantheios.org/
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


/** \file pantheios/implicit_link/ber.WindowsConsole.WithCallback.h
 *
 * [C, C++] Implicitly links in the \ref page__backend__callbacks "callback" version of the
 *   \ref group__backend__stock_backends__WindowsConsole "Pantheios Windows Console Remote Back-End Library"
 *   as the remote back-end for the given link-unit.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_IMPLICIT_LINK_H_BER_WINDOWSCONSOLE_WITHCALLBACK
#define PANTHEIOS_INCL_PANTHEIOS_IMPLICIT_LINK_H_BER_WINDOWSCONSOLE_WITHCALLBACK

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_IMPLICIT_LINK_H_BER_WINDOWSCONSOLE_WITHCALLBACK_MAJOR      2
# define PANTHEIOS_VER_PANTHEIOS_IMPLICIT_LINK_H_BER_WINDOWSCONSOLE_WITHCALLBACK_MINOR      0
# define PANTHEIOS_VER_PANTHEIOS_IMPLICIT_LINK_H_BER_WINDOWSCONSOLE_WITHCALLBACK_REVISION   1
# define PANTHEIOS_VER_PANTHEIOS_IMPLICIT_LINK_H_BER_WINDOWSCONSOLE_WITHCALLBACK_EDIT       7
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */
#ifndef PANTHEIOS_INCL_PANTHEIOS_IMPLICIT_LINK_H_IMPLICIT_LINK_BASE_
# include <pantheios/implicit_link/implicit_link_base_.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_IMPLICIT_LINK_H_IMPLICIT_LINK_BASE_ */
#ifndef PANTHEIOS_INCL_PANTHEIOS_IMPLICIT_LINK_H_BEC_WINDOWSCONSOLE_WITHCALLBACK
# include <pantheios/implicit_link/bec.WindowsConsole.WithCallback.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_IMPLICIT_LINK_H_BEC_WINDOWSCONSOLE_WITHCALLBACK */

/* /////////////////////////////////////////////////////////////////////////
 * Implicit-linking directives
 */

#ifdef PANTHEIOS_IMPLICIT_LINK_SUPPORT

# if defined(__BORLANDC__)
# elif defined(__COMO__)
# elif defined(__DMC__)
# elif defined(__GNUC__)
# elif defined(__INTEL_COMPILER)

#  pragma comment(lib, PANTHEIOS_IMPL_LINK_LIBRARY_NAME_("ber.WindowsConsole"))
#  pragma message("     " PANTHEIOS_IMPL_LINK_LIBRARY_NAME_("ber.WindowsConsole"))

# elif defined(__MWERKS__)
# elif defined(__WATCOMC__)
# elif defined(_MSC_VER)

#  pragma comment(lib, PANTHEIOS_IMPL_LINK_LIBRARY_NAME_("ber.WindowsConsole"))
#  pragma message("     " PANTHEIOS_IMPL_LINK_LIBRARY_NAME_("ber.WindowsConsole"))

# else /* ? compiler */
#  error Compiler not recognised
# endif /* compiler */

#endif /* PANTHEIOS_IMPLICIT_LINK_SUPPORT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_IMPLICIT_LINK_H_BER_WINDOWSCONSOLE_WITHCALLBACK */

/* ///////////////////////////// end of file //////////////////////////// */
