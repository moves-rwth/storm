/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/util/severity/WindowsEventLog.h
 *
 * Purpose:     Translations from Pantheios stock severity to Windows Event
 *              Log type.
 *
 * Created:     13th November 2007
 * Updated:     14th February 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2007-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/util/severity/WindowsEventLog.h
 *
 * [C, C++] Translations from Pantheios stock severity to Windows Event Log
 *   type.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_UTIL_SEVERITY_H_WINDOWSEVENTLOG
#define PANTHEIOS_INCL_PANTHEIOS_UTIL_SEVERITY_H_WINDOWSEVENTLOG

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_UTIL_SEVERITY_H_WINDOWSEVENTLOG_MAJOR      1
# define PANTHEIOS_VER_PANTHEIOS_UTIL_SEVERITY_H_WINDOWSEVENTLOG_MINOR      2
# define PANTHEIOS_VER_PANTHEIOS_UTIL_SEVERITY_H_WINDOWSEVENTLOG_REVISION   2
# define PANTHEIOS_VER_PANTHEIOS_UTIL_SEVERITY_H_WINDOWSEVENTLOG_EDIT       15
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */
#ifndef PANTHEIOS_INCL_PANTHEIOS_QUALITY_H_CONTRACT
# include <pantheios/quality/contract.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_QUALITY_H_CONTRACT */

#ifndef WINSTL_INCL_WINSTL_H_WINSTL
# include <winstl/winstl.h>
#endif /* !WINSTL_INCL_WINSTL_H_WINSTL */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{

#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Functions
 */

/** Converts a Pantheios severity level to a Windows Event Log event type
 *
 * \param severity The Pantheios \link pantheios::pan_severity_t severity level\endlink
 *
 * \return The Windows Event Log event type
 */
#ifdef __cplusplus
inline
#else /* ? __cplusplus */
static
#endif /* __cplusplus */
       WORD pantheios_severity_to_WindowsEventLog_type(int severity)
{
    WORD type;

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API((severity >= 0 && severity < 16), "invalid severity");

    switch(severity)
    {
        case    PANTHEIOS_SEV_EMERGENCY:     type = EVENTLOG_ERROR_TYPE;        break;
        case    PANTHEIOS_SEV_ALERT:         type = EVENTLOG_ERROR_TYPE;        break;
        case    PANTHEIOS_SEV_CRITICAL:      type = EVENTLOG_ERROR_TYPE;        break;
        case    PANTHEIOS_SEV_ERROR:         type = EVENTLOG_ERROR_TYPE;        break;
        case    PANTHEIOS_SEV_WARNING:       type = EVENTLOG_WARNING_TYPE;      break;
        case    PANTHEIOS_SEV_NOTICE:        type = EVENTLOG_INFORMATION_TYPE;  break;
        case    PANTHEIOS_SEV_INFORMATIONAL: type = EVENTLOG_INFORMATION_TYPE;  break;
        default:
        case    PANTHEIOS_SEV_DEBUG:         type = EVENTLOG_INFORMATION_TYPE;  break;
    }

    return type;
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Inclusion
 */

#ifdef STLSOFT_PPF_pragma_once_SUPPORT
# pragma once
#endif /* STLSOFT_PPF_pragma_once_SUPPORT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_UTIL_SEVERITY_H_WINDOWSEVENTLOG */

/* ///////////////////////////// end of file //////////////////////////// */
