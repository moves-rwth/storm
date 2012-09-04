/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/frontends/stock.h
 *
 * Purpose:     Declaration of the Pantheios Stock Front-end API Common
 *              Elements.
 *
 * Created:     23rd November 2007
 * Updated:     6th August 2012
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2007-2012, Matthew Wilson and Synesis Software
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


/** \file pantheios/frontends/stock.h
 *
 * [C, C++] Declaration of the Pantheios Stock Front-end API Common
 *   Elements.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_FRONTENDS_H_STOCK
#define PANTHEIOS_INCL_PANTHEIOS_FRONTENDS_H_STOCK

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_FRONTENDS_H_STOCK_MAJOR    1
# define PANTHEIOS_VER_PANTHEIOS_FRONTENDS_H_STOCK_MINOR    2
# define PANTHEIOS_VER_PANTHEIOS_FRONTENDS_H_STOCK_REVISION 2
# define PANTHEIOS_VER_PANTHEIOS_FRONTENDS_H_STOCK_EDIT     13
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */
#ifndef PANTHEIOS_INCL_PANTHEIOS_H_FRONTEND
# include <pantheios/frontend.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_FRONTEND */

/** \defgroup group__frontend__stock_frontends Pantheios Stock Front-ends
 * \ingroup group__frontend Pantheios
 *  Pre-built front-ends supplied with the Pantheios library
 *
 * Pantheios comes with several pre-written stock front-end libraries, which
 * cover most common needs for diagnostic logging. They also serve as good
 * examples of how to write a custom front-end.
 */

/* /////////////////////////////////////////////////////////////////////////
 * Compiler compatibility
 */

#if defined(__DMC__) && \
    __DMC__ < 0x0850
/* Previous versions of Digital Mars compiler/linker did not operate
 * correctly with declaration of extern const char [].
 */
# error Not compatible with Digital Mars C/C++ prior to version 8.50. Download the latest free version at www.digitalmars.com
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * External Declarations
 */

/** The application must define this variable, to contain the
 *    application name/identity.
 *
 * \ingroup group__frontend__stock_frontends
 *
 * The variable is an immutable array of <code>PAN_CHAR_T</code> (i.e. 
 * <code>char</code> in multibyte string builds, <code>wchar_t</code> in
 * wide string builds) whose name is non-mangled and has external linkage.
 *
 * Therefore, when defining within a C++ compilation unit it must be
 * declared <code>extern "C"</code>; when defined within a C compilation
 * unit it must be extern (optionally declared <code>extern</code>; it
 * must not be declared <code>static</code>).
 *
 * Examples:
\htmlonly
<pre>
  // C: multibyte string build
  char const                            PANTHEIOS_FE_PROCESS_IDENTITY[] = "my.app";

  // C++: wide string build
  extern "C" wchar_t const              PANTHEIOS_FE_PROCESS_IDENTITY[] = L"my.app";
</pre>
\endhtmlonly
 *
 * For convenience, the constructs PANTHEIOS_EXTERN_C, PAN_CHAR_T, and
 * PANTHEIOS_LITERAL_STRING() may be employed to write the variable
 * definition in a manner independent of language and/or character encoding,
 * as in:
\htmlonly
<pre>
  /\* C or C++; multibyte or wide string build \*\/
  PANTHEIOS_EXTERN_C PAN_CHAR_T const   PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("my.app");

</pre>
\endhtmlonly
 *
 * \note The process identity must not contain any whitespace characters,
 *   otherwise Pantheios library initialisation will fail.
 *
 * \warn If you define the variable as a pointer
 *   (<code>PAN_CHAR_T const*</code>) the behaviour of the link unit is
 *   undefined.
 */
PANTHEIOS_EXTERN_C PAN_CHAR_T const     PANTHEIOS_FE_PROCESS_IDENTITY[];

/* /////////////////////////////////////////////////////////////////////////
 * Application-defined functions
 */

/** \ref page__frontend__callbacks "Callback" function defined by the
 *    application, invoked during API initialisation.
 *
 * \ingroup group__frontend__stock_frontends
 *
 * \note This function is only required when the
 *   \ref page__frontend__callbacks "callback" version of the front-end
 *   library is used.
 *
 * \note This function *MUST NOT* throw an exception, and *MUST NOT* return
 *   <code>NULL</code>. If the implementation fails to acquire/produce the
 *   identity, it must call <code>pantheios_exitProcess(1)</code>. It may
 *   optionally call <code>pantheios_util_onBailOut4()</code> first.
 */
PANTHEIOS_CALL(PAN_CHAR_T const*) pantheios_fe_getAppProcessIdentity(void) /* throw() */;

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_FRONTENDS_H_STOCK */

/* ///////////////////////////// end of file //////////////////////////// */
