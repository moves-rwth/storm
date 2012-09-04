/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/backends/bec.console.h
 *
 * Purpose:     Platform-specific console back-end
 *
 * Created:     3rd July 2009
 * Updated:     26th November 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2009-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/backends/bec.console.h
 *
 * [C, C++] Platform-specific console back-end
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BEC_CONSOLE
#define PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BEC_CONSOLE

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_CONSOLE_MAJOR       1
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_CONSOLE_MINOR       0
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_CONSOLE_REVISION    1
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_CONSOLE_EDIT        2
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */
#ifndef PANTHEIOS_INCL_PANTHEIOS_H_BACKEND
# include <pantheios/backend.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_BACKEND */

#ifndef PLATFORMSTL_INCL_PLATFORMSTL_H_PLATFORMSTL
# include <platformstl/platformstl.h>
#endif /* !PLATFORMSTL_INCL_PLATFORMSTL_H_PLATFORMSTL */

#if defined(PLATFORMSTL_OS_IS_WINDOWS)
# ifndef PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BEC_WINDOWSCONSOLE
#  include <pantheios/backends/bec.WindowsConsole.h>
# endif /* !PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BEC_WINDOWSCONSOLE */
#else /* ? OS */
# ifndef PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BEC_FPRINTF
#  include <pantheios/backends/bec.fprintf.h>
# endif /* !PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BEC_FPRINTF */
#endif /* OS */

/* /////////////////////////////////////////////////////////////////////////
 * Documentation
 */

/** \defgroup group__backend__stock_backends__console Pantheios Console Stock Back-end
 * \ingroup group__backend__stock_backends
 *  Back-end library that outputs to the console, providing colour-coding
 *  options as available on the given platform.
 *
 * \note The Console back-end does not actually exist; rather it is the
 *   \ref group__backend__stock_backends__WindowsConsole for Windows, or the
 *   \ref group__backend__stock_backends__fprintf otherwise. All types,
 *   functions and constants are actually those of the underlying
 *   platform-specific back-end.
 */

/* /////////////////////////////////////////////////////////////////////////
 * Constants
 */

/** \defgroup group__backend__stock_backends__console__flags Pantheios Console Stock Back-end Flags
 * \ingroup group__backend__stock_backends__console
 *  Flags for the \ref group__backend__stock_backends__console
 */

/** \def PANTHEIOS_BE_CONSOLE_F_NO_COLOURS
 *  Causes the \ref group__backend__stock_backends__console to
 *   emit log statements without colouring based on severity.
 * \ingroup group__backend__stock_backends__console__flags
 */

#if defined(PLATFORMSTL_OS_IS_WINDOWS)
# define PANTHEIOS_BE_CONSOLE_F_NO_COLOURS          PANTHEIOS_BE_WINDOWSCONSOLE_F_NO_COLOURS
#else /* ? OS */
# define PANTHEIOS_BE_CONSOLE_F_NO_COLOURS          (0)
#endif /* OS */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

/** Structure used for specifying initialisation information to the
 *    be.console library.
 * \ingroup group__backend__stock_backends__console
 */

#if defined(PLATFORMSTL_OS_IS_WINDOWS)
typedef pan_be_WindowsConsole_init_t        pan_be_console_init_t;
#else /* ? OS */
typedef pan_be_fprintf_init_t               pan_be_console_init_t;
#endif /* OS */

/* /////////////////////////////////////////////////////////////////////////
 * Application-defined functions
 */

#if defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION)
/** \ref page__backend__callbacks "Callback" function defined by
 *    the application, invoked when the
 *    API is initialised with a NULL <code>init</code> parameter.
 * \ingroup group__backend__stock_backends__console
 *
 * \param backEndId The back-end identifier passed to the back-end
 *   during its initialisation.
 * \param init A pointer to an already-initialised instance of
 *   <code>pan_be_console_init_t</code>.
 *
 * If any application-specific changes are required they can be made to
 * the structure to which <code>init</code> points, which will already
 * have been initialised. These changes will then be incorporated into
 * the back-end state, and reflected in its behaviour.
 *
 * If no changes are required, then the function can be a simple stub,
 * containing no instructions.
 *
 * \note This function is only required when the
 *   \ref page__backend__callbacks "callback" version of the library is
 *   used.
 *
 * \exception "throw()" This function must <b>not</b> throw any exceptions!
 *
 * \warning This function will be called during the initialisation of
 *   Pantheios, and so <b>must not</b> make any calls into Pantheios, either
 *   directly or indirectly!
 *
 * \note This function is actually an alias for the actual console required,
 *   depending on your operating system. If your operating system is
 *   Windows, then you are actually using
 *   the \ref group__backend__stock_backends__WindowsConsole, which means
 *   that the name of the callback function will be
 *   <code>pantheios_be_WindowsConsole_getAppInit()</code>; if your
 *   operating system is UNIX, then you are actually using
 *   the  \ref group__backend__stock_backends__fprintf, which means that the
 *   name of the callback function will be
 *   <code>pantheios_be_fprintf_getAppInit()</code>.
 */

PANTHEIOS_CALL(void) pantheios_be_console_getAppInit(
    int                     backEndId
,   pan_be_console_init_t*  init
) /* throw() */;

#else /* ? PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
# if defined(PLATFORMSTL_OS_IS_WINDOWS)
#  define pantheios_be_console_getAppInit(backEndId, init)  pantheios_be_WindowsConsole_getAppInit(backEndId, init)
# else /* ? OS */
#  define pantheios_be_console_getAppInit(backEndId, init)  pantheios_be_fprintf_getAppInit(backEndId, init)
# endif /* OS */
#endif /* PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * API functions
 */

/** Fills out a copy of the initialisation structure with default
 *    values (representative of the default behaviour of the library),
 *    ready to be customised and passed to the API initialiser function
 *    pantheios_be_console_init().
 * \ingroup group__backend__stock_backends__console
 *
 * \note This function should <b>not</b> be called on an
 *   already-initialised instance, as is the case in the implementation
 *   of the pantheios_be_console_getAppInit() function, as it will
 *   already have been called by pantheios_be_console_init() prior
 *   to the callback.
 */

#if defined(PLATFORMSTL_OS_IS_WINDOWS)
# define pantheios_be_console_getDefaultAppInit(init)   pantheios_be_WindowsConsole_getAppInit(init)
#else /* ? OS */
# define pantheios_be_console_getDefaultAppInit(init)   pantheios_be_fprintf_getAppInit(init)
#endif /* OS */


/** \def pantheios_be_console_init(processIdentity, id, init, param, ptoken)
 *
 * Implements the functionality for pantheios_be_init() over the Console API.
 * \ingroup group__backend__stock_backends__console
 */

#if defined(PLATFORMSTL_OS_IS_WINDOWS)
# define pantheios_be_console_init      pantheios_be_WindowsConsole_init
#else /* ? OS */
# define pantheios_be_console_init      pantheios_be_fprintf_init
#endif /* OS */


/** \def pantheios_be_console_uninit(token)
 *
 * Implements the functionality for pantheios_be_uninit() over the Console API.
 * \ingroup group__backend__stock_backends__console
 */

#if defined(PLATFORMSTL_OS_IS_WINDOWS)
# define pantheios_be_console_uninit    pantheios_be_WindowsConsole_uninit
#else /* ? OS */
# define pantheios_be_console_uninit    pantheios_be_fprintf_uninit
#endif /* OS */


/** \def pantheios_be_console_logEntry(feToken, beToken, severity, entry, cchEntry)
 *
 * Implements the functionality for pantheios_be_logEntry() over the Console API.
 * \ingroup group__backend__stock_backends__console
 */

#if defined(PLATFORMSTL_OS_IS_WINDOWS)
# define pantheios_be_console_logEntry  pantheios_be_WindowsConsole_logEntry
#else /* ? OS */
# define pantheios_be_console_logEntry  pantheios_be_fprintf_logEntry
#endif /* OS */


/** \def pantheios_be_console_parseArgs(numArgs, args, init)
 *
 * Parses the be.console back-end flags
 *
 * \ingroup group__backend
 *
 * Processes an argument list in the same way as
 * pantheios_be_parseStockArgs(), filling out the
 * pan_be_console_init_t in accordance with the arguments
 * found.
 *
 * Recognises the following standard argument names:
 * - "showProcessId"            (Boolean)
 * - "showTime"                 (Boolean)
 * - "showSeverity"             (Boolean)
 * - "useSystemTime"            (Boolean)
 * - "showDetailsAtStart"       (Boolean)
 * - "useUnixFormat"            (Boolean)
 * - "showDate"                 (Boolean)
 * - "showTime"                 (Boolean)
 * - "highResolution"           (Boolean)
 * - "lowResolution"            (Boolean)
 *
 * Recognises the following back-end specific argument names:
 * - "noColours"                (Boolean) - Windows-only
 * - "noColors"                 (Boolean) - Windows-only
 */

#if defined(PLATFORMSTL_OS_IS_WINDOWS)
# define pantheios_be_console_parseArgs(numArgs, args, init)        pantheios_be_WindowsConsole_parseArgs(numArgs, args, init)
#else /* ? OS */
# define pantheios_be_console_parseArgs(numArgs, args, init)        pantheios_be_fprintf_parseArgs(numArgs, args, init)
#endif /* OS */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BEC_CONSOLE */

/* ///////////////////////////// end of file //////////////////////////// */
