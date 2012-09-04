/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/backends/bec.WindowsEventLog.h
 *
 * Purpose:     Declaration of the Pantheios WindowsEventLog Stock Back-end API.
 *
 * Created:     8th May 2006
 * Updated:     26th December 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2006-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/backends/bec.WindowsEventLog.h
 *
 * [C, C++] Pantheios Windows EventLog Back-end Common API
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_WINDOWSEVENTLOG
#define PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_WINDOWSEVENTLOG

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_WINDOWSEVENTLOG_MAJOR       1
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_WINDOWSEVENTLOG_MINOR       2
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_WINDOWSEVENTLOG_REVISION    2
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_WINDOWSEVENTLOG_EDIT        27
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

/** \defgroup group__backend__stock_backends__WindowsEventLog Pantheios Windows EventLog Stock Back-end
 * \ingroup group__backend__stock_backends
 *  Back-end library that writes to the Windows event log.
 */

/* /////////////////////////////////////////////////////////////////////////
 * Application-defined Functions
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

struct Pantheios_be_WindowsEventLog_no_longer_uses_the_symbol_BE_WINDOWSEVENTLOG_EVENTID_it_now_uses_the_function_pantheios_be_WindowsEventLog_calcCategoryAndEventId_;
# define BE_WINDOWSEVENTLOG_EVENTID Pantheios_be_WindowsEventLog_no_longer_uses_the_symbol_BE_WINDOWSEVENTLOG_EVENTID_it_now_uses_the_function_pantheios_be_WindowsEventLog_calcCategoryAndEventId_()

struct Pantheios_be_WindowsEventLog_no_longer_uses_the_symbol_pantheios_be_WindowsEventLog_calcCategory_it_now_uses_the_function_pantheios_be_WindowsEventLog_calcCategoryAndEventId_;
# define pantheios_be_WindowsEventLog_calcCategory  struct Pantheios_be_WindowsEventLog_no_longer_uses_the_symbol_pantheios_be_WindowsEventLog_calcCategory_it_now_uses_the_function_pantheios_be_WindowsEventLog_calcCategoryAndEventId_

struct Pantheios_be_WindowsEventLog_no_longer_uses_the_symbol_pantheios_be_WindowsEventLog_calcEventId_it_now_uses_the_function_pantheios_be_WindowsEventLog_calcCategoryAndEventId_;
# define pantheios_be_WindowsEventLog_calcEventId struct Pantheios_be_WindowsEventLog_no_longer_uses_the_symbol_pantheios_be_WindowsEventLog_calcEventId_it_now_uses_the_function_pantheios_be_WindowsEventLog_calcCategoryAndEventId_

#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */


/** Evaluates the appropriate category and event identifiers for a given
 *   back-end identifier and Pantheios severity level.
 *
 * \note This is an application-specified function.
 *
 * \ingroup group__backend__stock_backends__WindowsEventLog
 */
PANTHEIOS_CALL(void) pantheios_be_WindowsEventLog_calcCategoryAndEventId(
    int                     backEndId
,   int                     severity
#if !defined(PANTHEIOS_NO_NAMESPACE)
,   pantheios::uint16_t*    category
,   pantheios::uint32_t*    eventId
#else /* ? !PANTHEIOS_NO_NAMESPACE */
,   pan_uint16_t*           category
,   pan_uint32_t*           eventId
#endif /* !PANTHEIOS_NO_NAMESPACE */
) /* throw() */;

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/** Implements the functionality for pantheios_be_init() over the Windows EventLog API.
 *
 * \ingroup group__backend__stock_backends__WindowsEventLog
 */
PANTHEIOS_CALL(int) pantheios_be_WindowsEventLog_init(
    PAN_CHAR_T const*   processIdentity
,   int                 id
,   void*               unused
,   void*               reserved
,   void**              ptoken
);

/** Implements the functionality for pantheios_be_uninit() over the Windows EventLog API.
 * \ingroup group__backend__stock_backends__WindowsEventLog
 */
PANTHEIOS_CALL(void) pantheios_be_WindowsEventLog_uninit(
    void* token
);

/** Implements the functionality for pantheios_be_logEntry() over the Windows EventLog API.
 * \ingroup group__backend__stock_backends__WindowsEventLog
 */
PANTHEIOS_CALL(int) pantheios_be_WindowsEventLog_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
);


#if 0

/* TODO: Implement these in 1.0.1 beta 162+
 */

/** This helper function can be used to defer 
 */
PANTHEIOS_CALL(void) pantheios_be_WindowsEventLog_getPantheiosDotComCategoryAndEventId(
        int                     backEndId
    ,   int                     severity
#if !defined(PANTHEIOS_NO_NAMESPACE)
    ,   pantheios::uint16_t*    category
    ,   pantheios::uint32_t*    eventId
#else /* ? !PANTHEIOS_NO_NAMESPACE */
    ,   pan_uint16_t*           category
    ,   pan_uint32_t*           eventId
#endif /* !PANTHEIOS_NO_NAMESPACE */
) /* throw() */;

/** Registers an event source for use with Pantheios.COM
 */
PANTHEIOS_CALL(int) pantheios_be_WindowsEventLog_registerEventSourceUsingPantheiosDotCom(PAN_CHAR_T const* sourceName, PAN_CHAR_T const* pantheiosDotComPath);

/** Unregisters an event source for use with Pantheios.COM
 */
PANTHEIOS_CALL(int) pantheios_be_WindowsEventLog_unregisterEventSourceUsingPantheiosDotCom(PAN_CHAR_T const* sourceName, PAN_CHAR_T const* pantheiosDotComPath);

#endif /* 0 */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_WINDOWSEVENTLOG */

/* ///////////////////////////// end of file //////////////////////////// */
