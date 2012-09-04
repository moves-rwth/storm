/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/backends/bec.syslog.h
 *
 * Purpose:     Declaration of the Pantheios syslog Stock Back-end API.
 *
 * Created:     23rd July 2005
 * Updated:     30th April 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/backends/bec.syslog.h
 *
 * [C, C++] Pantheios SysLog Back-end Common API
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_SYSLOG_SRC
#define PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_SYSLOG_SRC

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_SYSLOG_MAJOR    3
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_SYSLOG_MINOR    1
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_SYSLOG_REVISION 1
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_SYSLOG_EDIT     22
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

/* /////////////////////////////////////////////////////////////////////////
 * Documentation
 */

/** \defgroup group__backend__stock_backends__syslog Pantheios UNIX SysLog Stock Back-end
 * \ingroup group__backend__stock_backends
 *  Back-end library built on top of the UNIX <b>syslog</b> API.
 */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

/** \defgroup group__backend__stock_backends__syslog__flags Pantheios UNIX syslog Stock Back-end Flags
 * \ingroup group__backend__stock_backends__syslog
 *  Flags for the \ref group__backend__stock_backends__syslog
 *
 * These flags are passed as the <code>options</code> parameter to the
 * SysLog API function <code>openlog()</code> during back-end
 * initialisation.
 */

/** \def PANTHEIOS_BE_SYSLOG_F_PERROR
 *  Causes the \ref group__backend__stock_backends__syslog to
 *   write to the calling process' standard error, in addition to submitting
 *   it to SysLog.
 * \ingroup group__backend__stock_backends__syslog__flags
 *
 * Specifying this flag causes the pan_be_syslog_init_t::options member to
 * contain the SysLog <code>LOG_PERROR</code> flag.
 *
 * \note On operating systems on which this non-standard feature is not
 *   available as part of the Syslog API, this will be provided by a
 *   distinct call to fprintf(stderr).
 */

#define PANTHEIOS_BE_SYSLOG_F_PERROR    (0x00100000)

/** \def PANTHEIOS_BE_SYSLOG_F_CONS
 *  Causes the \ref group__backend__stock_backends__syslog to
 *   write any message that fails to be submitted to SysLog to the system
 *   console.
 * \ingroup group__backend__stock_backends__syslog__flags
 *
 * Specifying this flag causes the pan_be_syslog_init_t::options member to
 * contain the SysLog <code>LOG_CONS</code> flag.
 */

#define PANTHEIOS_BE_SYSLOG_F_CONS      (0x00200000)

/** \def PANTHEIOS_BE_SYSLOG_F_PID
 *  Causes the \ref group__backend__stock_backends__syslog to include
 *   the process Id in the statement.
 * \ingroup group__backend__stock_backends__syslog__flags
 *
 * Specifying this flag causes the pan_be_syslog_init_t::options member to
 * contain the SysLog <code>LOG_PID</code> flag.
 */

#define PANTHEIOS_BE_SYSLOG_F_PID       (0x00400000)

/** \def PANTHEIOS_BE_SYSLOG_F_NDELAY
 *  Causes the \ref group__backend__stock_backends__syslog to
 *   connect to the syslog socket immediately, rather than on the first call
 *   to <code>syslog()</code>.
 * \ingroup group__backend__stock_backends__syslog__flags
 *
 * Specifying this flag causes the pan_be_syslog_init_t::options member to
 * contain the SysLog <code>LOG_NDELAY</code> flag.
 */

#define PANTHEIOS_BE_SYSLOG_F_NDELAY    (0x00800000)



/** Structure used for specifying initialisation information to the
 *    be.syslog library.
 * \ingroup group__backend__stock_backends__syslog
 */
struct pan_be_syslog_init_t
{
#if !defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION) && \
    !defined(PANTHEIOS_NO_NAMESPACE)
    typedef pantheios::pan_uint8_t  pan_uint8_t;
    typedef pantheios::pan_uint16_t pan_uint16_t;
    typedef pantheios::pan_uint32_t pan_uint32_t;
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION && !PANTHEIOS_NO_NAMESPACE */

    pan_uint32_t    version;    /*!< Must be initialised to the value of PANTHEIOS_VER */
    pan_uint32_t    flags;      /*!<  \ref group__backend__stock_backends__syslog__flags "Flags" that control the information displayed. */
    pan_uint32_t    options;    /*!<  Options passed to <code>openlog()</code>. Will be overridden by \link pan_be_syslog_init_t::flags flags\endlink. */
    pan_uint8_t     facility;   /*!<  The facility used by the process. Must be <= 124. Defaults to LOG_USER */

#ifdef __cplusplus
public: /* Construction */
# ifndef PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
    pan_be_syslog_init_t();
# endif /* !PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT */
#endif /* __cplusplus */
};
#if !defined(__cplusplus)
typedef struct pan_be_syslog_init_t     pan_be_syslog_init_t;
#endif /* !__cplusplus */


/* /////////////////////////////////////////////////////////////////////////
 * Application-defined functions
 */

/** \ref page__backend__callbacks "Callback" function defined by
 *    the application, invoked when the
 *    API is initialised with a NULL <code>init</code> parameter.
 * \ingroup group__backend__stock_backends__syslog
 *
 * \note This function is only required when the
 *   \ref page__backend__callbacks "callback" version of the library is
 *   used.
 */
PANTHEIOS_CALL(void) pantheios_be_syslog_getAppInit(
    int                     backEndId
,   pan_be_syslog_init_t*   init
) /* throw() */;

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/** Fills out a copy of the initialisation structure with default
 *    values (representative of the default behaviour of the library),
 *    ready to be customised and passed to the API initialiser function
 *    pantheios_be_syslog_init().
 * \ingroup group__backend__stock_backends__syslog
 *
 * \note This function should <b>not</b> be called on an
 *   already-initialised instance, as is the case in the implementation
 *   of the pantheios_be_syslog_getAppInit() function, as it will
 *   already have been called by pantheios_be_syslog_init() prior
 *   to the callback.
 */
PANTHEIOS_CALL(void) pantheios_be_syslog_getDefaultAppInit(
    pan_be_syslog_init_t* init
) /* throw() */;

/** Implements the functionality for pantheios_be_init() over the UNIX SysLog API.
 * \ingroup group__backend__stock_backends__syslog
 */
PANTHEIOS_CALL(int) pantheios_be_syslog_init(
    PAN_CHAR_T const*           processIdentity
,   int                         id
,   pan_be_syslog_init_t const* init
,   void*                       reserved
,   void**                      ptoken
);

/** Implements the functionality for pantheios_be_uninit() over the UNIX SysLog API.
 * \ingroup group__backend__stock_backends__syslog
 */
PANTHEIOS_CALL(void) pantheios_be_syslog_uninit(
    void* token
);

/** Implements the functionality for pantheios_be_logEntry() over the UNIX SysLog API.
 * \ingroup group__backend__stock_backends__syslog
 */
PANTHEIOS_CALL(int) pantheios_be_syslog_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
);

/** Parses the be.syslog back-end flags
 *
 * \ingroup group__backend
 *
 * Processes an argument list in the same way as
 * pantheios_be_parseStockArgs(), filling out the
 * pan_be_COMErrorObject_init_t in accordance with the arguments
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
 * - "useStderr"                (Boolean)
 * - "useConsole"               (Boolean)
 * - "showPid"                  (Boolean)
 * - "connectImmediately"       (Boolean)
 */
PANTHEIOS_CALL(int) pantheios_be_syslog_parseArgs(
    size_t                          numArgs
#ifdef PANTHEIOS_NO_NAMESPACE
,   struct pan_slice_t* const       args
#else /* ? PANTHEIOS_NO_NAMESPACE */
,   pantheios::pan_slice_t* const   args
#endif /* PANTHEIOS_NO_NAMESPACE */
,   pan_be_syslog_init_t*           init
);


/* ////////////////////////////////////////////////////////////////////// */

#ifdef __cplusplus
# ifndef PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
inline pan_be_syslog_init_t::pan_be_syslog_init_t()
{
    pantheios_be_syslog_getDefaultAppInit(this);
}
# endif /* !PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT */
#endif /* __cplusplus */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_SYSLOG_SRC */

/* ///////////////////////////// end of file //////////////////////////// */
