/* /////////////////////////////////////////////////////////////////////////
 * File:        src/backends/bec.WindowsEventLog.cpp
 *
 * Purpose:     Implementation for the WindowsEventLog back-end
 *
 * Created:     8th May 2006
 * Updated:     23rd May 2011
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2006-2011, Matthew Wilson and Synesis Software
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
#include <pantheios/internal/winlean.h>
#define PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
#include <pantheios/backends/bec.WindowsEventLog.h>

#include <pantheios/init_codes.h>
#include <pantheios/quality/contract.h>
#include <pantheios/util/core/apidefs.hpp>
#include <pantheios/util/severity/WindowsEventLog.h>

/* STLSoft Header Files */
#include <winstl/memory/processheap_allocator.hpp>

/* Standard C Header Files */
#include <stdio.h>
#include <string.h>

/* /////////////////////////////////////////////////////////////////////////
 * Compiler features
 */

#if defined(STLSOFT_COMPILER_IS_BORLAND) || \
    defined(STLSOFT_COMPILER_IS_DMC) || \
    (   defined(STLSOFT_COMPILER_IS_MSVC) && \
        _MSC_VER < 1200)
# define PANTHEIOS_NO_PLACEMENT_DELETE_
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * String encoding compatibility
 */

#ifdef PANTHEIOS_USE_WIDE_STRINGS
# define pan_RegisterEventSource_       ::RegisterEventSourceW
# define pan_ReportEvent_               ::ReportEventW
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
# define pan_RegisterEventSource_       ::RegisterEventSourceA
# define pan_ReportEvent_               ::ReportEventA
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

namespace
{
#if !defined(PANTHEIOS_NO_NAMESPACE)

    using ::pantheios::pan_char_t;

#endif /* !PANTHEIOS_NO_NAMESPACE */
} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

namespace
{
#if !defined(PANTHEIOS_NO_NAMESPACE)

    using ::pantheios::pan_uint16_t;
    using ::pantheios::pan_uint32_t;
    using ::pantheios::pantheios_severity_to_WindowsEventLog_type;

#endif /* !PANTHEIOS_NO_NAMESPACE */
} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * Structures
 */

class WindowsEventLog_Context
{
public:
    explicit WindowsEventLog_Context(int id)
        : hEvLog(NULL)
        , id(id)
    {}

public:
    int     Register(pan_char_t const* processIdentity);
    void    Deregister();
    int     ReportEvent(
        int                 severity
    ,   pan_char_t const*   entry
    ,   size_t              cchEntry
    );

private:
    HANDLE      hEvLog;
    const int   id;

private:
    WindowsEventLog_Context(WindowsEventLog_Context const&);
    WindowsEventLog_Context &operator =(WindowsEventLog_Context const&);
};

/* /////////////////////////////////////////////////////////////////////////
 * API functions
 */

static int pantheios_be_WindowsEventLog_init_(
    pan_char_t const*   processIdentity
,   int                 id
,   void const*         unused
,   void*               reserved
,   void**              ptoken
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != processIdentity, "process identity may not be NULL");

    STLSOFT_SUPPRESS_UNUSED(unused);
    STLSOFT_SUPPRESS_UNUSED(reserved);

    // (iii) create the context

    WindowsEventLog_Context* ctxt   =   new WindowsEventLog_Context(id);

#ifndef STLSOFT_CF_THROW_BAD_ALLOC
    if(NULL == ctxt)
    {
        delete ctxt;

        return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
    }
    else
#endif /* !STLSOFT_CF_THROW_BAD_ALLOC */
    {
        int res =   ctxt->Register(processIdentity);

        if(0 != res)
        {
            delete ctxt;

            return res;
        }
    }

    *ptoken = ctxt;

    return 0;
}

PANTHEIOS_CALL(int) pantheios_be_WindowsEventLog_init(
    pan_char_t const*   processIdentity
,   int                 id
,   void*               unused
,   void*               reserved
,   void**              ptoken
)
{
    return pantheios_call_be_X_init<void>(pantheios_be_WindowsEventLog_init_, processIdentity, id, unused, reserved, ptoken);
}

PANTHEIOS_CALL(void) pantheios_be_WindowsEventLog_uninit(void* token)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != token, "token must be non-null");

    WindowsEventLog_Context* ctxt = static_cast<WindowsEventLog_Context*>(token);

    delete ctxt;
}

PANTHEIOS_CALL(int) pantheios_be_WindowsEventLog_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   pan_char_t const*   entry
,   size_t              cchEntry
)
{
    STLSOFT_SUPPRESS_UNUSED(feToken);

    WindowsEventLog_Context* elc = static_cast<WindowsEventLog_Context*>(beToken);

    return elc->ReportEvent(severity, entry, cchEntry);
}

/* /////////////////////////////////////////////////////////////////////////
 * WindowsEventLog_Context
 */

int WindowsEventLog_Context::Register(pan_char_t const* processIdentity)
{
    this->hEvLog = pan_RegisterEventSource_(NULL, processIdentity);

    return (NULL != this->hEvLog) ? 0 : -static_cast<int>(::GetLastError());
}

void WindowsEventLog_Context::Deregister()
{
    ::DeregisterEventSource(hEvLog);
}

int WindowsEventLog_Context::ReportEvent(
    int                 severity
,   pan_char_t const*   entry
,   size_t              /* cchEntry */
)
{
    WORD            wType;
    pan_uint16_t    category;
    pan_uint32_t    eventId;
    PSID            lpUserSid   =   NULL;
    WORD            wNumStrings =   1;
    DWORD           dwDataSize  =   0;
    LPCTSTR*        lpStrings   =   &entry;
    LPVOID          lpRawData   =   NULL;

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(0 == (severity & 0x08), "be.WindowsEventLog can only be used with the stock severity levels in the range [0, 8). Levels in the range [8, 16) are not allowed");

    pantheios_be_WindowsEventLog_calcCategoryAndEventId(this->id, severity, &category, &eventId);

    severity &= 0x7;    /* This stock back-end ignores any custom severity information. */

    wType = pantheios_severity_to_WindowsEventLog_type(severity);

    if(!pan_ReportEvent_(   hEvLog
                        ,   wType
                        ,   category
                        ,   eventId
                        ,   lpUserSid
                        ,   wNumStrings
                        ,   dwDataSize
                        ,   lpStrings
                        ,   lpRawData))
    {
        return PANTHEIOS_INIT_RC_UNSPECIFIED_FAILURE;
    }

    return 0;
}

/* ///////////////////////////// end of file //////////////////////////// */
