/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/frontends/fe.N.h
 *
 * Purpose:     Declaration of the Pantheios fe.N Stock Front-end API.
 *
 * Created:     18th October 2006
 * Updated:     20th March 2012
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2006-2012, Matthew Wilson and Synesis Software
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


/** \file pantheios/frontends/fe.N.h
 *
 * [C,C++] Pantheios fe.N Stock Front-end API
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_FRONTENDS_H_FE_N
#define PANTHEIOS_INCL_PANTHEIOS_FRONTENDS_H_FE_N

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_FRONTENDS_H_FE_N_MAJOR     2
# define PANTHEIOS_VER_PANTHEIOS_FRONTENDS_H_FE_N_MINOR     2
# define PANTHEIOS_VER_PANTHEIOS_FRONTENDS_H_FE_N_REVISION  3
# define PANTHEIOS_VER_PANTHEIOS_FRONTENDS_H_FE_N_EDIT      15
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

/** \defgroup group__frontend__stock_frontends__N Pantheios N Stock Front-end
 * \ingroup group__frontend__stock_frontends
 *  Front-end library that can filter N back-ends.
 */

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

/** Structure that describes a filter entry.
 *
 * \ingroup group__frontend__stock_frontends__N
 *
 *
\htmlonly
<pre>
  PANTHEIOS_EXTERN_C pan_fe_N_t PAN_FE_N_SEVERITY_CEILINGS[] =
  {
    { 1, PANTHEIOS_SEV_WARNING }, // Back-end 1 has a ceiling of WARNING (so will log WARNING, ERROR, CRITICAL, ALERT, and EMERGENCY)
    { 2, PANTHEIOS_SEV_ERROR   }, // Back-end 2 has a ceiling of ERROR (so will log ERROR, CRITICAL, ALERT, and EMERGENCY)

    PANTHEIOS_FE_N_TERMINATOR_ENTRY(PANTHEIOS_SEV_NOTICE) // default severity of NOTICE
  };
</pre>
\endhtmlonly
 */
struct pan_fe_N_t
{
    /** The back-end identifier. Specify 0 to terminate the array. */
    int backEndId;
    /** The severity ceiling. When backEndId is 0, this specifies the default ceiling */
    int severityCeiling;
};
#ifndef __cplusplus
typedef struct pan_fe_N_t   pan_fe_N_t;
#endif /* __cplusplus */

/** \def PANTHEIOS_FE_N_TERMINATOR_ENTRY
 *
 * Defines a terminating entry in an array of pan_fe_N_t
 *
 * \ingroup group__frontend__stock_frontends__N
 *
 * \see pan_fe_N_t
 */
#define PANTHEIOS_FE_N_TERMINATOR_ENTRY(defaultCeiling)         { 0, defaultCeiling }

/* /////////////////////////////////////////////////////////////////////////
 * External Declarations
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
struct Pantheios_no_longer_uses_the_symbol_PAN_FE_N_PROCESS_IDENTITY_all_front_ends_now_recognise_PANTHEIOS_FE_PROCESS_IDENTITY_;
# define PAN_FE_N_PROCESS_IDENTITY (Pantheios_no_longer_uses_the_symbol_PAN_FE_N_PROCESS_IDENTITY_all_front_ends_now_recognise_PANTHEIOS_FE_PROCESS_IDENTITY_)
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/** The application must define this variable, to contain an array
 *    of filter entries whose last, "null", element has a <code>0</code>
 *    \link pan_fe_N_t::backEndId backEndId\endlink.
 *
 * \warning Due to the inherent ordering imperfection in C++ - it's not
 *    predictable between compilation units; see Chapter 11 of Imperfect
 *    C++ (http://imperfectcplusplus.com) - you should <b>not</b> use
 *    the pseudo-constants (pantheios::debug, pantheios::informational, ...
 *    pantheios::emergency) in the initialisers for this array. Instead,
 *    use the enumerators in
 *    \link pantheios::pan_severity_t pan_severity_t\endlink.
 *
 * \note This array is declared mutable (non-const), but that is only as
 *    an expedient for testing. Its contents <b>must not be changed</b>\
 *    at any time
 *
 * \ingroup group__frontend__stock_frontends__N
 */
PANTHEIOS_EXTERN_C pan_fe_N_t PAN_FE_N_SEVERITY_CEILINGS[];

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_FRONTENDS_H_FE_N */

/* ///////////////////////////// end of file //////////////////////////// */
