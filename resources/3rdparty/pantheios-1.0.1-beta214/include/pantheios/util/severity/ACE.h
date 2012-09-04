/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/util/severity/ACE.h
 *
 * Purpose:     Translations from Pantheios stock severity to ACE priority.
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


/** \file pantheios/util/severity/ACE.h
 *
 * [C, C++] Translations from Pantheios stock severity to ACE priority.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_UTIL_SEVERITY_H_ACE
#define PANTHEIOS_INCL_PANTHEIOS_UTIL_SEVERITY_H_ACE

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_UTIL_SEVERITY_H_ACE_MAJOR      1
# define PANTHEIOS_VER_PANTHEIOS_UTIL_SEVERITY_H_ACE_MINOR      2
# define PANTHEIOS_VER_PANTHEIOS_UTIL_SEVERITY_H_ACE_REVISION   2
# define PANTHEIOS_VER_PANTHEIOS_UTIL_SEVERITY_H_ACE_EDIT       14
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

#ifndef PANTHEIOS_INCL_ACE_H_LOG_PRIORITY
# define PANTHEIOS_INCL_ACE_H_LOG_PRIORITY
# include <ace/Log_Priority.h>
#endif /* !PANTHEIOS_INCL_ACE_H_LOG_PRIORITY */

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

/** Converts a Pantheios severity level to an ACE priority level
 *
 * \param severity The Pantheios \link pantheios::pan_severity_t severity level\endlink
 *
 * \return The ACE priority
 */
#ifdef __cplusplus
inline
#else /* ? __cplusplus */
static
#endif /* __cplusplus */
       ACE_Log_Priority pantheios_severity_to_ACE_priority(int severity)
{
    ACE_Log_Priority priority;

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API((severity >= 0 && severity < 16), "invalid severity");

    switch(severity)
    {
        case    PANTHEIOS_SEV_EMERGENCY:        priority = LM_EMERGENCY;    break;
        case    PANTHEIOS_SEV_ALERT:            priority = LM_ALERT;        break;
        case    PANTHEIOS_SEV_CRITICAL:         priority = LM_CRITICAL;     break;
        case    PANTHEIOS_SEV_ERROR:            priority = LM_ERROR;        break;
        case    PANTHEIOS_SEV_WARNING:          priority = LM_WARNING;      break;
        case    PANTHEIOS_SEV_NOTICE:           priority = LM_NOTICE;       break;
        case    PANTHEIOS_SEV_INFORMATIONAL:    priority = LM_INFO;         break;
        default:
        case    PANTHEIOS_SEV_DEBUG:            priority = LM_DEBUG;        break;
    }

    return priority;
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

#endif /* !PANTHEIOS_INCL_PANTHEIOS_UTIL_SEVERITY_H_ACE */

/* ///////////////////////////// end of file //////////////////////////// */
