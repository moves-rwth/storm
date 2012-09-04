/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/frontends/fe.simple.h
 *
 * Purpose:     Declaration of the Pantheios fe.simple Stock Front-end API.
 *
 * Created:     8th May 2006
 * Updated:     26th December 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2006-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/frontends/fe.simple.h
 *
 * [C, C++] Pantheios fe.simple Stock Front-end API
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_FRONTENDS_H_FE_SIMPLE
#define PANTHEIOS_INCL_PANTHEIOS_FRONTENDS_H_FE_SIMPLE

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_FRONTENDS_H_FE_SIMPLE_MAJOR    2
# define PANTHEIOS_VER_PANTHEIOS_FRONTENDS_H_FE_SIMPLE_MINOR    1
# define PANTHEIOS_VER_PANTHEIOS_FRONTENDS_H_FE_SIMPLE_REVISION 5
# define PANTHEIOS_VER_PANTHEIOS_FRONTENDS_H_FE_SIMPLE_EDIT     15
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

/** \defgroup group__frontend__stock_frontends__simple Pantheios simple Stock Front-end
 * \ingroup group__frontend__stock_frontends
 *  Front-end library that allows all severity levels in debug mode
 *   and \link pantheios::SEV_NOTICE NOTICE\endlink and higher in
 *   release mode.
 */

/* /////////////////////////////////////////////////////////////////////////
 * External Declarations
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
struct Pantheios_no_longer_uses_the_symbol_FE_SIMPLE_PROCESS_IDENTITY_all_front_ends_now_recognise_PANTHEIOS_FE_PROCESS_IDENTITY_;
# define FE_SIMPLE_PROCESS_IDENTITY Pantheios_no_longer_uses_the_symbol_FE_SIMPLE_PROCESS_IDENTITY_all_front_ends_now_recognise_PANTHEIOS_FE_PROCESS_IDENTITY_()
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/** Retrieves the current value of the fe.simple severity ceiling.
 *
 * \ingroup group__frontend__stock_frontends__simple
 */
PANTHEIOS_CALL(int) pantheios_fe_simple_getSeverityCeiling(void);

/** Sets the fe.simple severity ceiling, returning the previous value
 *
 * \ingroup group__frontend__stock_frontends__simple
 *
 * \warning This function is <b>not</b> thread-safe. If called by two
 *   threads concurrently, the return value in one thread may indicate the
 *   recently set ceiling in the other, rather than the original value at
 *   the time of the call.
 */
PANTHEIOS_CALL(int) pantheios_fe_simple_setSeverityCeiling(int ceiling);

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_FRONTENDS_H_FE_SIMPLE */

/* ///////////////////////////// end of file //////////////////////////// */
