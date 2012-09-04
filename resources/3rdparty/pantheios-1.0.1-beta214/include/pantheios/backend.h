/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/backend.h
 *
 * Purpose:     Pantheios back end API
 *
 * Created:     21st June 2005
 * Updated:     26th November 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/backend.h
 *
 * [C, C++] Definition of the \ref group__backend.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_BACKEND
#define PANTHEIOS_INCL_PANTHEIOS_H_BACKEND

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_H_BACKEND_MAJOR      3
# define PANTHEIOS_VER_PANTHEIOS_H_BACKEND_MINOR      11
# define PANTHEIOS_VER_PANTHEIOS_H_BACKEND_REVISION   1
# define PANTHEIOS_VER_PANTHEIOS_H_BACKEND_EDIT       31
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

/* /////////////////////////////////////////////////////////////////////////
 * Back-end API
 */

/** \defgroup group__backend Pantheios Back-end API
 *
 * The Pantheios back-end API describes the required functionality of a back-end
 * library. The back-end is responsible solely for translating the log entry -
 * severity indicator + length-specified, nul-terminated string - into output,
 * as appropriate to its implementation.
 *
 * There are several stock back-ends supplied with the Pantheios distribution
 * (http://pantheios.org/), providing logging to stderr (using \c fprintf()),
 * SysLog (using UNIX syslog(), or an emulation library on Windows), Windows
 * Debugger, ACE output. You may also supply your own back-end by implementing
 * the three simple functions of the API: pantheios_be_init(),
 * pantheios_be_uninit(), and pantheios_be_logEntry().
 *
 * @{
 */

/** \defgroup group__backend__stock_backends Pantheios Stock Back-ends
 *
 * \ingroup group__backend
 *
 *  Pre-built back-ends supplied with the Pantheios library
 *
 * Pantheios comes with several pre-written stock back-end libraries, which
 * cover most common needs for diagnostic logging. They also serve as good
 * examples of how to write a custom back-end.
 */

/** \defgroup group__backend__stock_ids Pantheios Stock Back-end Ids
 *
 * \ingroup group__backend
 *
 *  Stock back-end identifiers used by the Pantheios back-end
 *   libraries.
 */

/** \def PANTHEIOS_BEID_ALL
 *
 * \ingroup group__backend__stock_ids
 *
 *  Indicates that the operation/query applies to all back-ends
 */

#define PANTHEIOS_BEID_ALL                      (0)

/** \def PANTHEIOS_BEID_LOCAL
 *
 * \ingroup group__backend__stock_ids
 *
 *  Identifies the local (or only) back-end in a link-unit
 */

#define PANTHEIOS_BEID_LOCAL                    (1)

/** \def PANTHEIOS_BEID_REMOTE
 *
 * \ingroup group__backend__stock_ids
 *
 *  Identifies the remote back-end in a link-unit using local/remote
 *   splitting.
 */

#define PANTHEIOS_BEID_REMOTE                   (2)

/* /////////////////////////////////////////////////////////////////////////
 * Constants
 */

/** \defgroup group__backend__init__flags Pantheios Back-end Initialisation Flags
 *
 * \ingroup group__backend
 *
 *  Flags for the \ref group__backend__stock_backends
 */

/** \def PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
 *  Causes the back-end to
 *   omit the process identity from emitted log statements.
 * \ingroup group__backend__init__flags
 */

/** \def PANTHEIOS_BE_INIT_F_NO_THREAD_ID
 *  Causes the back-end to
 *   omit the thread identity from emitted log statements.
 * \ingroup group__backend__init__flags
 */

/** \def PANTHEIOS_BE_INIT_F_NO_DATETIME
 *  Causes the back-end to
 *   omit the date/time field from emitted log statements.
 * \ingroup group__backend__init__flags
 */

/** \def PANTHEIOS_BE_INIT_F_NO_SEVERITY
 *  Causes the back-end to
 *   omit the severity from emitted log statements.
 * \ingroup group__backend__init__flags
 */

/** \def PANTHEIOS_BE_INIT_F_USE_SYSTEM_TIME
 *  Causes the back-end to
 *   use system time, rather than local time, from emitted log statements.
 * \ingroup group__backend__init__flags
 */

/** \def PANTHEIOS_BE_INIT_F_DETAILS_AT_START
 *  Causes the details
 *   to be emitted at the start of the statement, rather than after the
 *   process id, time, and so on.
 * \ingroup group__backend__init__flags
 */

/** \def PANTHEIOS_BE_INIT_F_USE_UNIX_FORMAT
 *  Causes the back-end to
 *   use UNIX format for the date/time field, even on other operating
 *   systems.
 * \ingroup group__backend__init__flags
 */

/** \def PANTHEIOS_BE_INIT_F_HIDE_DATE
 *  Causes the back-end to
 *   omit the date in the date/time field (if shown).
 * \ingroup group__backend__init__flags
 */

/** \def PANTHEIOS_BE_INIT_F_HIDE_TIME
 *  Causes the back-end to
 *   omit the time in the date/time field (if shown).
 * \ingroup group__backend__init__flags
 */

/** \def PANTHEIOS_BE_INIT_F_HIGH_RESOLUTION
 *  Causes the back-end to
 *   favour high-resolution in the date/time field (if shown).
 * \ingroup group__backend__init__flags
 */

/** \def PANTHEIOS_BE_INIT_F_LOW_RESOLUTION
 *  Causes the back-end to
 *   favour low-resolution in the date/time field (if shown).
 * \ingroup group__backend__init__flags
 */


#define PANTHEIOS_BE_INIT_F_NO_PROCESS_ID       (0x00000001)
#define PANTHEIOS_BE_INIT_F_NO_THREAD_ID        (0x00001000)
#define PANTHEIOS_BE_INIT_F_NO_DATETIME         (0x00000002)
#define PANTHEIOS_BE_INIT_F_NO_SEVERITY         (0x00000004)
#define PANTHEIOS_BE_INIT_F_USE_SYSTEM_TIME     (0x00000008)
#define PANTHEIOS_BE_INIT_F_DETAILS_AT_START    (0x00000010)
#define PANTHEIOS_BE_INIT_F_USE_UNIX_FORMAT     (0x00000020)
#define PANTHEIOS_BE_INIT_F_HIDE_DATE           (0x00000040)
#define PANTHEIOS_BE_INIT_F_HIDE_TIME           (0x00000080)
#define PANTHEIOS_BE_INIT_F_HIGH_RESOLUTION     (0x00000100)
#define PANTHEIOS_BE_INIT_F_LOW_RESOLUTION      (0x00000200)


/** \def PANTHEIOS_BE_INIT_F_COMMON_MASK
 *  Mask of stock back-end flags.
 * \ingroup group__backend__init__flags
 */

/** \def PANTHEIOS_BE_INIT_F_CUSTOM_MASK
 *  Mask of custom back-end flags.
 * \ingroup group__backend__init__flags
 */


#define PANTHEIOS_BE_INIT_F_COMMON_MASK         (0x000fffff)
#ifdef __cplusplus
# define PANTHEIOS_BE_INIT_F_CUSTOM_MASK        (~static_cast<int>(PANTHEIOS_BE_INIT_F_COMMON_MASK))
#else /* ? __cplusplus */
# define PANTHEIOS_BE_INIT_F_CUSTOM_MASK        (~((int)PANTHEIOS_BE_INIT_F_COMMON_MASK))
#endif /* __cplusplus */



/** Initialises the back-end API.
 *
 * \ingroup group__backend
 *
 * This function is called once by the Pantheios core library to initialise the
 * back-end library. It passes the process identity (in the form of a
 * nul-terminated C-style string) and a second parameter (reserved for future
 * use; currently always has value 0), which the back-end may use in its
 * initialisation. The third parameter is a pointer to a void*, with which the
 * back-end may store state, to be passed back to it in the pantheios_be_logEntry()
 * and pantheios_be_uninit() functions.
 *
 * \param processIdentity The identity of the process within which Pantheios is
 * being used. Should be a meaningful human-readable string. Must not be NULL.
 * Maximum length is limited solely by what the back-end library can accomodate.
 * The string pointed to by this parameter may not persist after the call is
 * complete, so the back-end should take a copy if required.
 * \param reserved Currently reserved. Will contain 0 in the current version of
 * the Pantheios library
 * \param ptoken Pointer to a variable to receive the back-end token, which will
 * be stored in the Pantheios library and passed to the pantheios_be_logEntry()
 * and pantheios_be_uninit() functions
 *
 * \note This function must be defined by each back-end implementation
 *
 * \note This function is called at most once per process.
 *
 * \return A status indicating whether the back-end is initialised, and
 * therefore whether the Pantheios library as a whole is initialised.
 * \retval <0 Initialisation failed.
 * \retval >=0 Initialisation succeeded
 */
PANTHEIOS_CALL(int) pantheios_be_init(
    PAN_CHAR_T const*   processIdentity
,   void*               reserved
,   void**              ptoken
);

/** Uninitialises the back-end API.
 *
 * \ingroup group__backend
 *
 * This function is called to uninitialise the back-end library during the
 * uninitialisation of the Pantheios core library. It is passed the value of the
 * token stored on its behalf by the Pantheios core library.
 *
 * \note This function must be defined by each back-end implementation
 *
 * \note This function is called at most once per process.
 *
 * \param token The back-end token, created by the pantheios_be_init() function
 */
PANTHEIOS_CALL(void) pantheios_be_uninit(void* token);

/** Passes a log-entry to the back-end API.
 *
 * \ingroup group__backend
 *
 * This function is called by the Pantheios core library to emit a log entry.
 * It is passed five parameters. The \c severity, \c entry and \c cchEntry
 * parameters describe the severity level, and the nul-terminated contents of
 * the log entry. The \c feToken and \c beToken parameters hold the
 * library-specific state of the front-end and back-end librarys, respectively.
 *
 * \note This function must be defined by each back-end implementation
 *
 * \note This may be called from any thread in a multi-threaded process.
 *
 * \param feToken The front-end token, created by the pantheios_fe_init()
 *   function. This value does not hold any meaning to the back-end library, and
 *   may be used only to passed back to the front-end in calls to
 *   pantheios_fe_isSeverityLogged().
 * \param beToken The back-end token, created by the pantheios_be_init() function
 * \param severity The severity level.
 * \param entry The nul-terminated string containing the entry information
 * \param cchEntry The number of bytes in the string, not including the
 * nul-terminating character
 */
PANTHEIOS_CALL(int) pantheios_be_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
);

/** @} group__backend */

/* /////////////////////////////////////////////////////////////////////////
 * Generation Macros
 */

/** \def PANTHEIOS_BE_DEFINE_BE_FUNCTIONS(expr)
 *
 *  Back-end generation macro for the Pantheios API
 *
 * Generates the functions <code>pantheios_be_init()</code>,
 * <code>pantheios_be_uninit()</code> and
 * <code>pantheios_be_logEntry()</code> from the given back-end
 * implementation. The given id is assumed to be common to all three
 * back-end API functions for the given back-end implementation. In other
 * words, for the back-end "be.loader" one would specify the id to be
 * <code>loader</code>, from which the macro assumes the existence of the
 * three functions <code>pantheios_be_loader_init()</code>,
 * <code>pantheios_be_loader_uninit()</code> and
 * <code>pantheios_be_loader_logEntry()</code>.
 *
 * \param id The back-end identifier, e.g. <code>loader</code> for the
 *   "be.loader" back-end.
 */
#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_BE_DEFINE_BE_FUNCTIONS_(fullId)                                                                               \
                                                                                                                                \
    PANTHEIOS_CALL(int) pantheios_be_init(PAN_CHAR_T const* processIdentity, void* reserved, void** ptoken)                           \
    { return fullId##_init(processIdentity, PANTHEIOS_BEID_LOCAL, NULL, reserved, ptoken); }                                    \
    PANTHEIOS_CALL(void) pantheios_be_uninit(void* token)                                                                       \
    { fullId##_uninit(token); }                                                                                                 \
    PANTHEIOS_CALL(int) pantheios_be_logEntry(void* feToken, void* beToken, int severity, PAN_CHAR_T const* entry, size_t cchEntry)   \
    { STLSOFT_SUPPRESS_UNUSED(feToken); return fullId##_logEntry(feToken, beToken, severity, entry, cchEntry); }
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

#define PANTHEIOS_BE_DEFINE_BE_FUNCTIONS(id)    PANTHEIOS_BE_DEFINE_BE_FUNCTIONS_(pantheios_be_##id)




/** \def PANTHEIOS_BE_DEFINE_BEL_FUNCTIONS(expr)
 *
 *  Local back-end generation macro for the Pantheios API
 *
 * Generates the functions <code>pantheios_be_local_init()</code>,
 * <code>pantheios_be_local_uninit()</code> and
 * <code>pantheios_be_local_logEntry()</code> from the given back-end
 * implementation. The given id is assumed to be common to all three
 * back-end API functions for the given back-end implementation. In other
 * words, for the back-end "be.loader" one would specify the id to be
 * <code>loader</code>, from which the macro assumes the existence of the
 * three functions <code>pantheios_be_loader_init()</code>,
 * <code>pantheios_be_loader_uninit()</code> and
 * <code>pantheios_be_loader_logEntry()</code>.
 *
 * \param id The back-end identifier, e.g. <code>loader</code> for the
 *   "be.loader" back-end.
 */
#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_BE_DEFINE_BEL_FUNCTIONS_(fullId)                                                                                  \
                                                                                                                                    \
    PANTHEIOS_CALL(int) pantheios_be_local_init(PAN_CHAR_T const* processIdentity, void* reserved, void** ptoken)                         \
    { return fullId##_init(processIdentity, PANTHEIOS_BEID_LOCAL, NULL, reserved, ptoken); }                                        \
    PANTHEIOS_CALL(void) pantheios_be_local_uninit(void* token)                                                                     \
    { fullId##_uninit(token); }                                                                                                     \
    PANTHEIOS_CALL(int) pantheios_be_local_logEntry(void* feToken, void* beToken, int severity, PAN_CHAR_T const* entry, size_t cchEntry) \
    { STLSOFT_SUPPRESS_UNUSED(feToken); return fullId##_logEntry(feToken, beToken, severity, entry, cchEntry); }
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

#define PANTHEIOS_BE_DEFINE_BEL_FUNCTIONS(id)   PANTHEIOS_BE_DEFINE_BEL_FUNCTIONS_(pantheios_be_##id)

/** \def PANTHEIOS_BE_DEFINE_BER_FUNCTIONS(expr)
 *
 *  Remote back-end generation macro for the Pantheios API
 *
 * Generates the functions <code>pantheios_be_remote_init()</code>,
 * <code>pantheios_be_remote_uninit()</code> and
 * <code>pantheios_be_remote_logEntry()</code> from the given back-end
 * implementation. The given id is assumed to be common to all three
 * back-end API functions for the given back-end implementation. In other
 * words, for the back-end "be.loader" one would specify the id to be
 * <code>loader</code>, from which the macro assumes the existence of the
 * three functions <code>pantheios_be_loader_init()</code>,
 * <code>pantheios_be_loader_uninit()</code> and
 * <code>pantheios_be_loader_logEntry()</code>.
 *
 * \param id The back-end identifier, e.g. <code>loader</code> for the
 *   "be.loader" back-end.
 */
#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_BE_DEFINE_BER_FUNCTIONS_(fullId)                                                                                      \
                                                                                                                                        \
    PANTHEIOS_CALL(int) pantheios_be_remote_init(PAN_CHAR_T const* processIdentity, void* reserved, void** ptoken)                            \
    { return fullId##_init(processIdentity, PANTHEIOS_BEID_REMOTE, NULL, reserved, ptoken); }                                           \
    PANTHEIOS_CALL(void) pantheios_be_remote_uninit(void* token)                                                                        \
    { fullId##_uninit(token); }                                                                                                         \
    PANTHEIOS_CALL(int) pantheios_be_remote_logEntry(void* feToken, void* beToken, int severity, PAN_CHAR_T const* entry, size_t cchEntry)    \
    { STLSOFT_SUPPRESS_UNUSED(feToken); return fullId##_logEntry(feToken, beToken, severity, entry, cchEntry); }
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

#define PANTHEIOS_BE_DEFINE_BER_FUNCTIONS(id)   PANTHEIOS_BE_DEFINE_BER_FUNCTIONS_(pantheios_be_##id)

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_BACKEND */

/* ///////////////////////////// end of file //////////////////////////// */
