/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/backends/bec.WindowsSyslog.h
 *
 * Purpose:     Declaration of the Pantheios Windows-SysLog Stock Back-end API.
 *
 * Created:     23rd September 2005
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


/** \file pantheios/backends/bec.WindowsSyslog.h
 *
 * [C, C++] Pantheios Windows-SysLog Back-end Common API
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BEC_WINDOWSSYSLOG
#define PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BEC_WINDOWSSYSLOG

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_WINDOWSSYSLOG_MAJOR     4
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_WINDOWSSYSLOG_MINOR     1
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_WINDOWSSYSLOG_REVISION  2
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_WINDOWSSYSLOG_EDIT      22
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

/** \defgroup group__backend__stock_backends__WindowsSyslog Pantheios Windows-SysLog Stock Back-end
 * \ingroup group__backend__stock_backends
 * Back-end library that provides a custom implementation of the
 *   <b>SysLog</b>-protocol for Windows.
 */

/* /////////////////////////////////////////////////////////////////////////
 * Constants
 */

/** \defgroup group__backend__stock_backends__WindowsSyslog__flags Pantheios Windows syslog Stock Back-end Flags
 * \ingroup group__backend__stock_backends__WindowsSyslog
 * Flags for the \ref group__backend__stock_backends__WindowsSyslog
 */

/** \def PANTHEIOS_BE_WINDOWSSYSLOG_F_USE_SYSTEM_TIME
 * Causes the \ref group__backend__stock_backends__WindowsSyslog to
 *   use system time, rather than local time, from emitted log statements.
 * \ingroup group__backend__stock_backends__WindowsSyslog__flags
 */

#define PANTHEIOS_BE_WINDOWSSYSLOG_F_USE_SYSTEM_TIME  PANTHEIOS_BE_INIT_F_USE_SYSTEM_TIME


/** \def PANTHEIOS_BE_WINDOWSSYSLOG_F_PERROR
 * Causes the \ref group__backend__stock_backends__WindowsSyslog to
 *   write to the calling process' standard error, in addition to submitting
 *   it to SysLog.
 * \ingroup group__backend__stock_backends__WindowsSyslog__flags
 *
 * Specifying this flag causes the pan_be_WindowsSyslog_init_t::options member to
 * contain the SysLog <code>LOG_PERROR</code> flag.
 */

#define PANTHEIOS_BE_WINDOWSSYSLOG_F_PERROR    (0x00100000)

/** \def PANTHEIOS_BE_WINDOWSSYSLOG_F_CONS
 * Causes the \ref group__backend__stock_backends__WindowsSyslog to
 *   write any message that fails to be submitted to SysLog to the system
 *   console.
 * \ingroup group__backend__stock_backends__WindowsSyslog__flags
 *
 * Specifying this flag causes the pan_be_WindowsSyslog_init_t::options member to
 * contain the SysLog <code>LOG_CONS</code> flag.
 */

#define PANTHEIOS_BE_WINDOWSSYSLOG_F_CONS      (0x00200000)

/** \def PANTHEIOS_BE_WINDOWSSYSLOG_F_PID
 * Causes the \ref group__backend__stock_backends__WindowsSyslog to include
 *   the process Id in the statement.
 * \ingroup group__backend__stock_backends__WindowsSyslog__flags
 *
 * Specifying this flag causes the pan_be_WindowsSyslog_init_t::options member to
 * contain the SysLog <code>LOG_PID</code> flag.
 */

#define PANTHEIOS_BE_WINDOWSSYSLOG_F_PID       (0x00400000)

/** \def PANTHEIOS_BE_WINDOWSSYSLOG_F_NDELAY
 * Causes the \ref group__backend__stock_backends__WindowsSyslog to
 *   connect to the syslog socket immediately, rather than on the first call
 *   to <code>syslog()</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__flags
 *
 * Specifying this flag causes the pan_be_WindowsSyslog_init_t::options member to
 * contain the SysLog <code>LOG_NDELAY</code> flag.
 */

#define PANTHEIOS_BE_WINDOWSSYSLOG_F_NDELAY    (0x00800000)



/** \defgroup group__backend__stock_backends__WindowsSyslog__facilities Pantheios Windows syslog Facility Codes
 * \ingroup group__backend__stock_backends__WindowsSyslog
 * Facility levels for the \ref group__backend__stock_backends__WindowsSyslog
 */

/** \def PANTHEIOS_SYSLOG_FAC_KERN
 * Kernel messages
 * \remarks Equivalent to the SysLog facility <code>LOG_KERN</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_KERN
# define PANTHEIOS_SYSLOG_FAC_KERN          LOG_KERN
#else /* ? LOG_KERN */
# define PANTHEIOS_SYSLOG_FAC_KERN          (0)
#endif /* LOG_KERN */

/** \def PANTHEIOS_SYSLOG_FAC_USER
 * Arbitrary user-level messages (the default)
 * \remarks Equivalent to the SysLog facility <code>LOG_USER</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_USER
# define PANTHEIOS_SYSLOG_FAC_USER          LOG_USER
#else /* ? LOG_USER */
# define PANTHEIOS_SYSLOG_FAC_USER          (1)
#endif /* LOG_USER */

/** \def PANTHEIOS_SYSLOG_FAC_MAIL
 * Mail system messages
 * \remarks Equivalent to the SysLog facility <code>LOG_MAIL</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_MAIL
# define PANTHEIOS_SYSLOG_FAC_MAIL          LOG_MAIL
#else /* ? LOG_MAIL */
# define PANTHEIOS_SYSLOG_FAC_MAIL          (2)
#endif /* LOG_MAIL */

/** \def PANTHEIOS_SYSLOG_FAC_DAEMON
 * System daemon messages
 * \remarks Equivalent to the SysLog facility <code>LOG_DAEMON</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_DAEMON
# define PANTHEIOS_SYSLOG_FAC_DAEMON        LOG_DAEMON
#else /* ? LOG_DAEMON */
# define PANTHEIOS_SYSLOG_FAC_DAEMON        (3)
#endif /* LOG_DAEMON */

/** \def PANTHEIOS_SYSLOG_FAC_AUTH
 * Security / authorisation messages
 * \remarks Equivalent to the SysLog facility <code>LOG_AUTH</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_AUTH
# define PANTHEIOS_SYSLOG_FAC_AUTH          LOG_AUTH
#else /* ? LOG_AUTH */
# define PANTHEIOS_SYSLOG_FAC_AUTH          (4)
#endif /* LOG_AUTH */

/** \def PANTHEIOS_SYSLOG_FAC_SYSLOG
 * SysLog (or Pantheios) messages
 * \remarks Equivalent to the SysLog facility <code>LOG_SYSLOG</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_SYSLOG
# define PANTHEIOS_SYSLOG_FAC_SYSLOG        LOG_SYSLOG
#else /* ? LOG_SYSLOG */
# define PANTHEIOS_SYSLOG_FAC_SYSLOG        (5)
#endif /* LOG_SYSLOG */

/** \def PANTHEIOS_SYSLOG_FAC_LPR
 * Printer messages
 * \remarks Equivalent to the SysLog facility <code>LOG_LPR</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_LPR
# define PANTHEIOS_SYSLOG_FAC_LPR           LOG_LPR
#else /* ? LOG_LPR */
# define PANTHEIOS_SYSLOG_FAC_LPR           (6)
#endif /* LOG_LPR */

/** \def PANTHEIOS_SYSLOG_FAC_NEWS
 * Network news subsystem messages
 * \remarks Equivalent to the SysLog facility <code>LOG_NEWS</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_NEWS
# define PANTHEIOS_SYSLOG_FAC_NEWS          LOG_NEWS
#else /* ? LOG_NEWS */
# define PANTHEIOS_SYSLOG_FAC_NEWS          (7)
#endif /* LOG_NEWS */

/** \def PANTHEIOS_SYSLOG_FAC_UUCP
 * UUCP subsystem messages
 * \remarks Equivalent to the SysLog facility <code>LOG_UUCP</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_UUCP
# define PANTHEIOS_SYSLOG_FAC_UUCP          LOG_UUCP
#else /* ? LOG_UUCP */
# define PANTHEIOS_SYSLOG_FAC_UUCP          (8)
#endif /* LOG_UUCP */

/** \def PANTHEIOS_SYSLOG_FAC_CRON
 * Scheduler daemon messages
 * \remarks Equivalent to the SysLog facility <code>LOG_CRON</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_CRON
# define PANTHEIOS_SYSLOG_FAC_CRON      LOG_CRON
#else /* ? LOG_CRON */
# define PANTHEIOS_SYSLOG_FAC_CRON      (9)
#endif /* LOG_CRON */

/** \def PANTHEIOS_SYSLOG_FAC_AUTHPRIV
 * Private security / authorisation messages
 * \remarks Equivalent to the SysLog facility <code>LOG_AUTHPRIV</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_AUTHPRIV
# define PANTHEIOS_SYSLOG_FAC_AUTHPRIV      LOG_AUTHPRIV
#else /* ? LOG_AUTHPRIV */
# define PANTHEIOS_SYSLOG_FAC_AUTHPRIV      (10)
#endif /* LOG_AUTHPRIV */

/** \def PANTHEIOS_SYSLOG_FAC_FTP
 * FTP daemon messages
 * \remarks Equivalent to the SysLog facility <code>LOG_FTP</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_FTP
# define PANTHEIOS_SYSLOG_FAC_FTP           LOG_FTP
#else /* ? LOG_FTP */
# define PANTHEIOS_SYSLOG_FAC_FTP           (11)
#endif /* LOG_FTP */

/** \def PANTHEIOS_SYSLOG_FAC_NETINFO
 * NetInfo subsystem messages
 * \remarks Equivalent to the SysLog facility <code>LOG_NETINFO</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_NETINFO
# define PANTHEIOS_SYSLOG_FAC_NETINFO       LOG_NETINFO
#else /* ? LOG_NETINFO */
# define PANTHEIOS_SYSLOG_FAC_NETINFO       (12)
#endif /* LOG_NETINFO */

/** \def PANTHEIOS_SYSLOG_FAC_REMOTEAUTH
 * Remote authentication / authorisation subsystem messages
 * \remarks Equivalent to the SysLog facility <code>LOG_REMOTEAUTH</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_REMOTEAUTH
# define PANTHEIOS_SYSLOG_FAC_REMOTEAUTH    LOG_REMOTEAUTH
#else /* ? LOG_REMOTEAUTH */
# define PANTHEIOS_SYSLOG_FAC_REMOTEAUTH    (13)
#endif /* LOG_REMOTEAUTH */

/** \def PANTHEIOS_SYSLOG_FAC_INSTALL
 * Installer subsystem messages
 * \remarks Equivalent to the SysLog facility <code>LOG_INSTALL</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_INSTALL
# define PANTHEIOS_SYSLOG_FAC_INSTALL       LOG_INSTALL
#else /* ? LOG_INSTALL */
# define PANTHEIOS_SYSLOG_FAC_INSTALL       (14)
#endif /* LOG_INSTALL */

/** \def PANTHEIOS_SYSLOG_FAC_RAS
 * RAS subsystem messages
 * \remarks Equivalent to the SysLog facility <code>LOG_RAS</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_RAS
# define PANTHEIOS_SYSLOG_FAC_RAS           LOG_RAS
#else /* ? LOG_RAS */
# define PANTHEIOS_SYSLOG_FAC_RAS           (15)
#endif /* LOG_RAS */

/** \def PANTHEIOS_SYSLOG_FAC_LOCAL0
 * Reserved for local use
 * \remarks Equivalent to the SysLog facility <code>LOG_LOCAL0</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_LOCAL0
# define PANTHEIOS_SYSLOG_FAC_LOCAL0        LOG_LOCAL0
#else /* ? LOG_LOCAL0 */
# define PANTHEIOS_SYSLOG_FAC_LOCAL0        (16)
#endif /* LOG_LOCAL0 */

/** \def PANTHEIOS_SYSLOG_FAC_LOCAL1
 * Reserved for local use
 * \remarks Equivalent to the SysLog facility <code>LOG_LOCAL1</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_LOCAL1
# define PANTHEIOS_SYSLOG_FAC_LOCAL1        LOG_LOCAL1
#else /* ? LOG_LOCAL1 */
# define PANTHEIOS_SYSLOG_FAC_LOCAL1        (17)
#endif /* LOG_LOCAL1 */

/** \def PANTHEIOS_SYSLOG_FAC_LOCAL2
 * Reserved for local use
 * \remarks Equivalent to the SysLog facility <code>LOG_LOCAL2</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_LOCAL2
# define PANTHEIOS_SYSLOG_FAC_LOCAL2        LOG_LOCAL2
#else /* ? LOG_LOCAL2 */
# define PANTHEIOS_SYSLOG_FAC_LOCAL2        (18)
#endif /* LOG_LOCAL2 */

/** \def PANTHEIOS_SYSLOG_FAC_LOCAL3
 * Reserved for local use
 * \remarks Equivalent to the SysLog facility <code>LOG_LOCAL3</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_LOCAL3
# define PANTHEIOS_SYSLOG_FAC_LOCAL3        LOG_LOCAL3
#else /* ? LOG_LOCAL3 */
# define PANTHEIOS_SYSLOG_FAC_LOCAL3        (19)
#endif /* LOG_LOCAL3 */

/** \def PANTHEIOS_SYSLOG_FAC_LOCAL4
 * Reserved for local use
 * \remarks Equivalent to the SysLog facility <code>LOG_LOCAL4</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_LOCAL4
# define PANTHEIOS_SYSLOG_FAC_LOCAL4        LOG_LOCAL4
#else /* ? LOG_LOCAL4 */
# define PANTHEIOS_SYSLOG_FAC_LOCAL4        (20)
#endif /* LOG_LOCAL4 */

/** \def PANTHEIOS_SYSLOG_FAC_LOCAL5
 * Reserved for local use
 * \remarks Equivalent to the SysLog facility <code>LOG_LOCAL5</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_LOCAL5
# define PANTHEIOS_SYSLOG_FAC_LOCAL5        LOG_LOCAL5
#else /* ? LOG_LOCAL5 */
# define PANTHEIOS_SYSLOG_FAC_LOCAL5        (21)
#endif /* LOG_LOCAL5 */

/** \def PANTHEIOS_SYSLOG_FAC_LOCAL6
 * Reserved for local use
 * \remarks Equivalent to the SysLog facility <code>LOG_LOCAL6</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_LOCAL6
# define PANTHEIOS_SYSLOG_FAC_LOCAL6        LOG_LOCAL6
#else /* ? LOG_LOCAL6 */
# define PANTHEIOS_SYSLOG_FAC_LOCAL6        (22)
#endif /* LOG_LOCAL6 */

/** \def PANTHEIOS_SYSLOG_FAC_LOCAL7
 * Reserved for local use
 * \remarks Equivalent to the SysLog facility <code>LOG_LOCAL7</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_LOCAL7
# define PANTHEIOS_SYSLOG_FAC_LOCAL7        LOG_LOCAL7
#else /* ? LOG_LOCAL7 */
# define PANTHEIOS_SYSLOG_FAC_LOCAL7        (23)
#endif /* LOG_LOCAL7 */

/** \def PANTHEIOS_SYSLOG_FAC_LAUNCHD
 * Boostrap daemon subsystem messages
 * \remarks Equivalent to the SysLog facility <code>LOG_LAUNCHD</code>.
 * \ingroup group__backend__stock_backends__WindowsSyslog__facilities
 */
#ifdef LOG_LAUNCHD
# define PANTHEIOS_SYSLOG_FAC_LAUNCHD       LOG_LAUNCHD
#else /* ? LOG_LAUNCHD */
# define PANTHEIOS_SYSLOG_FAC_LAUNCHD       (24)
#endif /* LOG_LAUNCHD */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

/** Structure used for specifying initialisation information to the
 *    be.WindowsSyslog library.
 * \ingroup group__backend__stock_backends__WindowsSyslog
 */
struct pan_be_WindowsSyslog_init_t
{
#if !defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION) && \
    !defined(PANTHEIOS_NO_NAMESPACE)
    typedef pantheios::pan_uint8_t  pan_uint8_t;
    typedef pantheios::pan_uint16_t pan_uint16_t;
    typedef pantheios::pan_uint32_t pan_uint32_t;
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION && !PANTHEIOS_NO_NAMESPACE */

    pan_uint32_t    version;    /*!< Must be initialised to the value of PANTHEIOS_VER */
    pan_uint32_t    flags;      /*!< \ref group__backend__stock_backends__WindowsSyslog__flags "Flags" that control the information displayed. */
    size_t          addrSize;   /*!< Number of elements in array. Must be 0, 4 or 16. 0 means hostName is used; 4 means IPv4; 16 means IPv6. \note IPv6 is not currently supported. */
    pan_uint8_t     bytes[16];  /*!< IPv4 or IPv6 address elements. */
    char const*     hostName;   /*!< Host name. Must be non-NULL when 0 == addrSize. */
    pan_uint16_t    port;       /*!< Port to be used for transmission. */
    pan_uint8_t     facility;   /*!< The facility used by the process. Must be <= 124. Defaults to PANTHEIOS_SYSLOG_FAC_USER */
    char            hostNameBuff[101]; /*!< Buffer for use by client to write host name, to which \link pan_be_WindowsSyslog_init_t::hostName hostName\endlink can be pointed. */

#ifdef __cplusplus
public: /* Construction */
# ifndef PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
    pan_be_WindowsSyslog_init_t();
# endif /* !PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT */
#endif /* __cplusplus */
};
#if !defined(__cplusplus)
typedef struct pan_be_WindowsSyslog_init_t    pan_be_WindowsSyslog_init_t;
#endif /* !__cplusplus */


/* /////////////////////////////////////////////////////////////////////////
 * Application-defined functions
 */

/** \ref page__backend__callbacks "Callback" function defined by
 *    the application, invoked when the
 *    API is initialised with a NULL <code>init</code> parameter.
 * \ingroup group__backend__stock_backends__WindowsSyslog
 *
 * \note This function is only required when the
 *   \ref page__backend__callbacks "callback" version of the library is
 *   used.
 */
PANTHEIOS_CALL(void) pantheios_be_WindowsSyslog_getAppInit(
    int                             backEndId
,   pan_be_WindowsSyslog_init_t*    init
) /* throw() */;

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/** Fills out a copy of the initialisation structure with default
 *    values (representative of the default behaviour of the library),
 *    ready to be customised and passed to the API initialiser function
 *    pantheios_be_WindowsSyslog_init().
 * \ingroup group__backend__stock_backends__WindowsSyslog
 *
 * \note This function should <b>not</b> be called on an
 *   already-initialised instance, as is the case in the implementation
 *   of the pantheios_be_WindowsSyslog_getAppInit() function, as it will
 *   already have been called by pantheios_be_WindowsSyslog_init() prior
 *   to the callback.
 */
PANTHEIOS_CALL(void) pantheios_be_WindowsSyslog_getDefaultAppInit(
    pan_be_WindowsSyslog_init_t* init
) /* throw() */;

/** Implements the functionality for pantheios_be_init() over the Windows-SysLog API.
 * \ingroup group__backend__stock_backends__WindowsSyslog
 *
 * \remarks This function will fail to initialise if the host does not
 *   have network connectivity. Users should consider using with be.N
 *   and applying the
 *   \c PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE
 *   and 
 *   \c PANTHEIOS_BE_N_F_INIT_ONLY_IF_PREVIOUS_FAILED
 *   flags, so that initialisation failure will not prevent the
 *   application from initialising.
 */
PANTHEIOS_CALL(int) pantheios_be_WindowsSyslog_init(
    PAN_CHAR_T const*                   processIdentity
,   int                                 id
,   pan_be_WindowsSyslog_init_t const*  init
,   void*                               reserved
,   void**                              ptoken
);

/** Implements the functionality for pantheios_be_uninit() over the Windows-SysLog API.
 * \ingroup group__backend__stock_backends__WindowsSyslog
 */
PANTHEIOS_CALL(void) pantheios_be_WindowsSyslog_uninit(
    void* token
);

/** Implements the functionality for pantheios_be_logEntry() over the Windows-SysLog API.
 * \ingroup group__backend__stock_backends__WindowsSyslog
 */
PANTHEIOS_CALL(int) pantheios_be_WindowsSyslog_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
);

/** Parses the be.WindowsSyslog back-end flags
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
 * - "address"                  (String: hostname or dotted IPv4 address)
 * - "port"                     (Number)
 * - "facility"                 (Number)
 * - "useStderr"                (Boolean)
 * - "useConsole"               (Boolean)
 * - "showPid"                  (Boolean)
 * - "connectImmediately"       (Boolean)
 */
PANTHEIOS_CALL(int) pantheios_be_WindowsSyslog_parseArgs(
    size_t                          numArgs
#ifdef PANTHEIOS_NO_NAMESPACE
,   struct pan_slice_t* const       args
#else /* ? PANTHEIOS_NO_NAMESPACE */
,   pantheios::pan_slice_t* const   args
#endif /* PANTHEIOS_NO_NAMESPACE */
,   pan_be_WindowsSyslog_init_t*    init
);

/* ////////////////////////////////////////////////////////////////////// */

#ifdef __cplusplus
# ifndef PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
inline pan_be_WindowsSyslog_init_t::pan_be_WindowsSyslog_init_t()
{
    pantheios_be_WindowsSyslog_getDefaultAppInit(this);
}
# endif /* !PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT */
#endif /* __cplusplus */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BEC_WINDOWSSYSLOG */

/* ///////////////////////////// end of file //////////////////////////// */
