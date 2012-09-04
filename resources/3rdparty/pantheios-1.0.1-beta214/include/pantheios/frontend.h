/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/frontend.h
 *
 * Purpose:     Pantheios front end API
 *
 * Created:     21st June 2005
 * Updated:     7th August 2012
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


/** \file pantheios/frontend.h
 *
 * [C, C++] Definition of the \ref group__frontend.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_FRONTEND
#define PANTHEIOS_INCL_PANTHEIOS_H_FRONTEND

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_H_FRONTEND_MAJOR       3
# define PANTHEIOS_VER_PANTHEIOS_H_FRONTEND_MINOR       2
# define PANTHEIOS_VER_PANTHEIOS_H_FRONTEND_REVISION    1
# define PANTHEIOS_VER_PANTHEIOS_H_FRONTEND_EDIT        20
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

/* /////////////////////////////////////////////////////////////////////////
 * Front-end API
 */

/** \defgroup group__frontend Pantheios Front-end API
 *
 * The Pantheios front-end API describes the required functionality of a
 * front-end library. The front-end is responsible solely for providing
 * process identity information and arbitrating logging requests, based on
 * severity level (and local/remote destination).
 *
 * Process identity is defined by the front-end according to the
 * return-value of pantheios_fe_getProcessIdentity(), which should return a
 * nul-terminated C-style string representing the host process,
 * e.g. \c "MyDaemon"
 *
 * Filtering is carried out by pantheios_fe_isSeverityLogged(), which
 * receives three parameters: the front-end state, the log entry severity
 * level, and a back-end identifier. The back-end identifier value
 * represents one of three 'questions' that are to be asked of the
 * front-end. If \c backEndId is 0, then the question is whether any logging
 * should occur for the given severity level. If \c backEndId is 1, then the
 * question is whether logging should occur for the local back-end. If
 * \c backEndId is 2, then the question is whether logging should occur for
 * the remote back-end.
 *
 * @{
 */

/** Initialises the front-end API.
 *
 * \param reserved Currently reserved. Will be NULL in the current version
 *   of the Pantheios library
 * \param ptoken Pointer to a variable to receive the front-end token, which
 *   will be stored in the Pantheios library and passed to the
 *   pantheios_fe_getProcessIdentity(), pantheios_fe_isSeverityLogged()
 *   and pantheios_fe_uninit() functions.
 *
 * This function must be defined by each front-end implementation. It
 * initialises the front-end. If the implementation requires any context,
 * e.g. a front-end class instance, this can be allocated and placed into
 * <code>*ptoken</code>, and it will be passed back to the other functions.
 *
 * \note This function is called at most once per process.
 *
 * \return A status indicating whether the front-end is initialised, and
 *   therefore whether the Pantheios library as a whole is initialised.
 * \retval <0 Initialisation failed.
 * \retval >=0 Initialisation succeeded
 */
PANTHEIOS_CALL(int) pantheios_fe_init(
    void*   reserved
,   void**  ptoken
);

/** Uninitialises the front-end API.
 *
 * This function must be defined by each front-end implementation. It
 * uninitialises the front-end.
 *
 * \param token The front-end token, created by the pantheios_fe_init()
 *   function.
 *
 * \note This function is called at most once per process.
 */
PANTHEIOS_CALL(void) pantheios_fe_uninit(void* token);

/** Defines the process identity
 *
 * This function must be defined by each front-end implementation. It
 * defines the name of the process, that may be used in the emission of log
 * statements by back-ends.
 *
 * \param token The front-end token, created by the pantheios_fe_init()
 *   function.There is no strict maximum length as far as Pantheios is
 *   concerned, but it should not exceed the limitation provided by the
 *   implementing back-end library.
 *
 * \return A pointer to a nul-terminated C-style string containing the
 *   process identity. May not be \c NULL, except on the first invocation,
 *   which is an indication to the Pantheios core library (and congnisant
 *   custom back-ends; no stock back-ends use his behaviour) that the
 *   front-end may provide changing process identity at different times and
 *   that multiple calls are supported (and, presumably, desired). The
 *   maximum length of the returned string is 1000 characters.
 *
 * \note When the first invocation returns \c NULL, to denote that a dynamic
 *   process identity, the second invocation (which must
 *   <strong>not</strong> return \c NULL) will be called on the same thread,
 *   and at a time when no other (Pantheios-related) threads will call it,
 *   enabling implementations to use simple static flag variables to handle
 *   their switching logic (should they so wish).
 *
 * \note When the first invocation returns non-<code>NULL</code>, to denote
 *   a "classic" non-dynamic process identity, then the function is called
 *   at most once during successful
 *   initialisation, but in the case where initialisation fails it may be
 *   called additional times by the bail out handler
 *   (\link pantheios::util::pantheios_onBailOut4 pantheios_onBailOut4()\endlink).
 *   It may also be called if the application, or the front-end or
 *   back-end(s) invoke the bail out handler at any other time.
 */
PANTHEIOS_CALL(PAN_CHAR_T const*) pantheios_fe_getProcessIdentity(void* token);

/** Determines whether a given severity is being logged
 *
 * This function must be defined by each front-end implementation. It
 * is called by the Pantheios core library and, optionally, a
 * back-end library, to determine whether, for the given \c severity,
 * any/all logging is enabled (\c backEndId == 0), or whether local
 * (\c backEndId == 1) or remote (\c backEndId == 2) logging is enabled. It
 * is passed the token returned in pantheios_fe_init(), which the front-end
 * implementation may use to access its state
 *
 * \param token The front-end token, created by the pantheios_fe_init()
 *   function.
 * \param severity The severity level
 * \param backEndId A front-end+back-end specific parameter. This is used to
 *   enable a back-end, such as the be.lrsplit library, to simultaneously
 *   provide multiple actual back-ends and query the front-end for their
 *   respective. The value 0 is used within the Pantheios library and
 *   stands for every back-end. All other values indicate specific
 *   back-end splits, although by convention 1 indicates local logging
 *   and 2 indicates remote logging.
 *
 * \return A status indicating whether the given severity level is being
 *   logged.
 * \retval non-0 Severity is being logged.
 * \retval 0 Severity is not being logged.
 *
 * \note This may be called from any thread in a multi-threaded process, and
 *   so it must be safely callable by multiple threads concurrently.
 */
PANTHEIOS_CALL(int) pantheios_fe_isSeverityLogged(
    void*   token
,   int     severity
,   int     backEndId
);

/** @} // group__frontend */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_FRONTEND */

/* ///////////////////////////// end of file //////////////////////////// */
