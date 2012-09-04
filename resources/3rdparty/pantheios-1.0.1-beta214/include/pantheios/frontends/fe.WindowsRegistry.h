/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/frontends/fe.WindowsRegistry.h
 *
 * Purpose:     Declaration of the Pantheios fe.WindowsRegistry Stock Front-end API.
 *
 * Created:     28th October 2007
 * Updated:     26th December 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2007-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/frontends/fe.WindowsRegistry.h
 *
 * [C,C++] Pantheios fe.WindowsRegistry Stock Front-end API
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_FRONTENDS_H_FE_WINDOWSREGISTRY
#define PANTHEIOS_INCL_PANTHEIOS_FRONTENDS_H_FE_WINDOWSREGISTRY

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_FRONTENDS_H_FE_WINDOWSREGISTRY_MAJOR       2
# define PANTHEIOS_VER_PANTHEIOS_FRONTENDS_H_FE_WINDOWSREGISTRY_MINOR       1
# define PANTHEIOS_VER_PANTHEIOS_FRONTENDS_H_FE_WINDOWSREGISTRY_REVISION    4
# define PANTHEIOS_VER_PANTHEIOS_FRONTENDS_H_FE_WINDOWSREGISTRY_EDIT        10
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */
#ifndef PANTHEIOS_INCL_PANTHEIOS_FRONTENDS_H_STOCK
# include <pantheios/frontends/stock.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_FRONTENDS_H_STOCK */

/** \defgroup group__frontend__stock_frontends__WindowsRegistry Pantheios Windows Registry Stock Front-end
 * \ingroup group__frontend__stock_frontends
 *  Front-end library that allows all severity levels in debug mode
 *   and \link pantheios::SEV_NOTICE NOTICE\endlink and higher in
 *   release mode.
 */

/* /////////////////////////////////////////////////////////////////////////
 * External Declarations
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
struct Pantheios_no_longer_uses_the_symbol_PAN_FE_PROCESS_IDENTITY_all_front_ends_now_recognise_PANTHEIOS_FE_PROCESS_IDENTITY_;
# define PAN_FE_PROCESS_IDENTITY (Pantheios_no_longer_uses_the_symbol_PAN_FE_PROCESS_IDENTITY_all_front_ends_now_recognise_PANTHEIOS_FE_PROCESS_IDENTITY_)
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_FRONTENDS_H_FE_WINDOWSREGISTRY */

/* ///////////////////////////// end of file //////////////////////////// */
