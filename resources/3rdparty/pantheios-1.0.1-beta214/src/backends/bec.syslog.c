/* /////////////////////////////////////////////////////////////////////////
 * File:        src/backends/bec.syslog.c
 *
 * Purpose:     Implementation for the UNIX SysLog back-end
 *
 * Created:     29th June 2005
 * Updated:     26th August 2009
 *
 * Thanks to:   Jonathan Wakely for detecting Solaris compilation defects &
 *              fixes.
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2009, Matthew Wilson and Synesis Software
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


/* Pantheios Header Files */
#include <pantheios/pantheios.h>
#define PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
#include <pantheios/backends/bec.syslog.h>

#include <pantheios/init_codes.h>
#include <pantheios/quality/contract.h>
#include <pantheios/util/backends/arguments.h>
#include <pantheios/util/severity/syslog.h>

/* Standard Header Files */
#include <string.h>
#include <syslog.h>

/* /////////////////////////////////////////////////////////////////////////
 * Platform compatibility
 */

#if defined(_WIN32) && \
    defined(_STLSOFT_FORCE_ANY_COMPILER) && \
    defined(_MT)
# undef LOG_PERROR
#endif /* LOG_PERROR */

/* Standard Header Files */
#ifndef LOG_PERROR
# include <stdio.h>
#endif /* !LOG_PERROR */

/* /////////////////////////////////////////////////////////////////////////
 * API
 *
 * Some man pages advise that:
 *
 * - the processIdentity is
 */

PANTHEIOS_CALL(void) pantheios_be_syslog_getDefaultAppInit(pan_be_syslog_init_t* init) /* throw() */
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != init, "initialisation structure pointer may not be null");

    init->version       =   PANTHEIOS_VER;
    init->flags         =   0;
    init->options       =   PANTHEIOS_BE_SYSLOG_F_PERROR;
    init->facility      =   LOG_USER;

    /* Take account of use of Synesis Software's WinSysLog for testing */

#ifdef WINSYSLOG_F_EVENTLOG
    init->options       |=  WINSYSLOG_F_EVENTLOG;
#endif /* WINSYSLOG_F_EVENTLOG */

#ifdef WINSYSLOG_F_UDP514
    init->options       |=  WINSYSLOG_F_UDP514;
#endif /* WINSYSLOG_F_UDP514 */
}

PANTHEIOS_CALL(int) pantheios_be_syslog_init(
    char const*                 processIdentity
,   int                         id
,   pan_be_syslog_init_t const* init
,   void*                       reserved
,   void**                      ptoken
)
{
    pan_be_syslog_init_t    init_;
    int                     options = 0;

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != processIdentity, "process identity may not be the null string");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API('\0' != 0[processIdentity], "process identity may not be the empty string");
    STLSOFT_SUPPRESS_UNUSED(id);
    STLSOFT_SUPPRESS_UNUSED(reserved);
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != ptoken, "token pointer may not be null");

    *ptoken = NULL;

    /* (i) apply Null Object (Variable) pattern */

    if(NULL == init)
    {
        pantheios_be_syslog_getDefaultAppInit(&init_);

#ifdef PANTHEIOS_BE_USE_CALLBACK
        pantheios_be_syslog_getAppInit(id, &init_);
#endif /* PANTHEIOS_BE_USE_CALLBACK */

        init = &init_;
    }

    /* (ii) verify the version */

    if(init->version < 0x010001b8)
    {
        return PANTHEIOS_BE_INIT_RC_OLD_VERSION_NOT_SUPPORTED;
    }
    else if(init->version > PANTHEIOS_VER)
    {
        return PANTHEIOS_BE_INIT_RC_FUTURE_VERSION_REQUESTED;
    }

    /* (iii) create the context */

    if(init->options & PANTHEIOS_BE_SYSLOG_F_PERROR)
    {
#ifdef LOG_PERROR
        options |= LOG_PERROR;
#else /* ? LOG_PERROR */
        *ptoken = ptoken; /* An arbitrary non-NULL value. */
#endif /* LOG_PERROR */
    }

    if(init->options & PANTHEIOS_BE_SYSLOG_F_CONS)
    {
        options |= LOG_CONS;
    }

    if(init->options & PANTHEIOS_BE_SYSLOG_F_PID)
    {
        options |= LOG_PID;
    }

    if(init->options & PANTHEIOS_BE_SYSLOG_F_NDELAY)
    {
        options |= LOG_NDELAY;
    }


    /* Take account of use of Synesis Software's WinSysLog for testing */
#ifdef WINSYSLOG_F_EVENTLOG
    if(init->options & WINSYSLOG_F_EVENTLOG)
    {
        options |= WINSYSLOG_F_EVENTLOG;
    }
#endif /* WINSYSLOG_F_EVENTLOG */

#ifdef WINSYSLOG_F_UDP514
    if(init->options & WINSYSLOG_F_UDP514)
    {
        options |= WINSYSLOG_F_UDP514;
    }
#endif /* WINSYSLOG_F_UDP514 */

    openlog(processIdentity, options, init->facility);

    return 0;
}

PANTHEIOS_CALL(void) pantheios_be_syslog_uninit(void* token)
{
    STLSOFT_SUPPRESS_UNUSED(token);

    closelog();
}

PANTHEIOS_CALL(int) pantheios_be_syslog_logEntry(
    void*       feToken
,   void*       beToken
,   int         severity
,   char const* entry
,   size_t      cchEntry
)
{
#ifdef LOG_PERROR
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL == beToken, "back-end token must be null");
#endif /* LOG_PERROR */
    STLSOFT_SUPPRESS_UNUSED(feToken);
    STLSOFT_SUPPRESS_UNUSED(cchEntry);

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(0 == (severity & 0x08), "be.syslog can only be used with the stock severity levels in the range [0, 8). Levels in the range [8, 16) are not allowed");

    severity &= 0x7;    /* This stock back-end ignores any custom severity information. */

    syslog(pantheios_severity_to_syslog_severity(severity), "%s", entry);

#ifndef LOG_PERROR
    if(NULL != beToken)
    {
        fprintf(stderr, "%.*s\n", (int)cchEntry, entry);
    }
#endif /* !LOG_PERROR */

    return 0;
}

PANTHEIOS_CALL(int) pantheios_be_syslog_parseArgs(
    size_t                      numArgs
,   struct pan_slice_t* const   args
,   pan_be_syslog_init_t*       init
)
{
    int res;

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API((NULL != args || 0 == numArgs), "argument pointer must be non-null, or number of arguments must be 0");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != init, "initialisation structure pointer may not be null");

    pantheios_be_syslog_getDefaultAppInit(init);

    /* 1. Parse the stock arguments */
    res = pantheios_be_parseStockArgs(numArgs, args, &init->flags);

    if(res >= 0)
    {
        /* 2.d Parse the custom argument: "useStderr" */
        res = pantheios_be_parseBooleanArg(numArgs, args, "useStderr", 0, PANTHEIOS_BE_SYSLOG_F_PERROR, &init->flags);
    }

    if(res >= 0)
    {
        /* 2.e Parse the custom argument: "useConsole" */
        res = pantheios_be_parseBooleanArg(numArgs, args, "useConsole", 0, PANTHEIOS_BE_SYSLOG_F_CONS, &init->flags);
    }

    if(res >= 0)
    {
        /* 2.f Parse the custom argument: "showPid" */
        res = pantheios_be_parseBooleanArg(numArgs, args, "showPid", 0, PANTHEIOS_BE_SYSLOG_F_PID, &init->flags);
    }

    if(res >= 0)
    {
        /* 2.g Parse the custom argument: "connectImmediately" */
        res = pantheios_be_parseBooleanArg(numArgs, args, "connectImmediately", 0, PANTHEIOS_BE_SYSLOG_F_NDELAY, &init->flags);
    }

    return res;
}

/* ///////////////////////////// end of file //////////////////////////// */
