/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/internal/stock_levels.hpp
 *
 * Purpose:     Custom level class.
 *
 * Created:     22nd July 2006
 * Updated:     10th August 2009
 *
 * Home:        http://www.pantheios.org/
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


/** \file pantheios/internal/stock_levels.hpp
 *
 * [C, C++] INTERNAL FILE: Defines the stock severity level pseudo-constants
 *   debug, informational, notice, warning, error, critical, alert,
 *   and
 *   emergency.
 */

#ifndef PANTHEIOS_INCLUDING_STOCKLEVELS
# error This file may only be included from within the Pantheios main C header file.
#endif /* !PANTHEIOS_INCLUDING_STOCKLEVELS */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INTERNAL_HPP_STOCK_LEVELS
#define PANTHEIOS_INCL_PANTHEIOS_INTERNAL_HPP_STOCK_LEVELS

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_SEVERITY_HPP_LEVELS
# include <pantheios/severity/levels.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_SEVERITY_HPP_LEVELS */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Constants
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
namespace
{
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

    /** Instance of \link pantheios::level level\endlink that holds the SEV_DEBUG value. */
    static /* const */ level<SEV_DEBUG>           debug;
    /** Instance of \link pantheios::level level\endlink that holds the SEV_INFORMATIONAL value. */
    static /* const */ level<SEV_INFORMATIONAL>   informational;
    /** Instance of \link pantheios::level level\endlink that holds the SEV_NOTICE value. */
    static /* const */ level<SEV_NOTICE>          notice;
    /** Instance of \link pantheios::level level\endlink that holds the SEV_WARNING value. */
    static /* const */ level<SEV_WARNING>         warning;
    /** Instance of \link pantheios::level level\endlink that holds the SEV_ERROR value. */
    static /* const */ level<SEV_ERROR>           error;
    /** Instance of \link pantheios::level level\endlink that holds the SEV_CRITICAL value. */
    static /* const */ level<SEV_CRITICAL>        critical;
    /** Instance of \link pantheios::level level\endlink that holds the SEV_ALERT value. */
    static /* const */ level<SEV_ALERT>           alert;
    /** Instance of \link pantheios::level level\endlink that holds the SEV_EMERGENCY value. */
    static /* const */ level<SEV_EMERGENCY>       emergency;

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
} // anonymous namespace
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INTERNAL_HPP_STOCK_LEVELS */

/* ///////////////////////////// end of file //////////////////////////// */
