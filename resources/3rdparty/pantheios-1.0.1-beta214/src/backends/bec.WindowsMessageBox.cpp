/* /////////////////////////////////////////////////////////////////////////
 * File:        src/backends/bec.WindowsMessageBox.cpp
 *
 * Purpose:     Implementation for the WindowsMessageBox back-end
 *
 * Created:     10th March 2008
 * Updated:     10th August 2009
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2008-2009, Matthew Wilson and Synesis Software
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
#include <pantheios/backends/bec.WindowsMessageBox.h>

#include <pantheios/init_codes.h>
#include <pantheios/quality/contract.h>
#include <pantheios/util/core/apidefs.hpp>

/* STLSoft Header Files */
#include <winstl/memory/processheap_allocator.hpp>

/* Standard C Header Files */
#include <stdio.h>
#include <string.h>
#include <windows.h>

/* /////////////////////////////////////////////////////////////////////////
 * Compiler features
 */

#if defined(STLSOFT_COMPILER_IS_BORLAND) || \
    defined(STLSOFT_COMPILER_IS_DMC) || \
    (   defined(STLSOFT_COMPILER_IS_MSVC) && \
        _MSC_VER < 1200)
# define    _PANTHEIOS_NO_PLACEMENT_DELETE
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * String encoding compatibility
 */

#ifdef PANTHEIOS_USE_WIDE_STRINGS
# define pan_MessageBox_                ::MessageBoxW
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
# define pan_MessageBox_                ::MessageBoxA
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
 * Structures
 */

class WindowsMessageBox_Context
{
public:
    explicit WindowsMessageBox_Context(int id)
        : id(id)
    {}

public:
    void* operator new(size_t cb, pan_char_t const* s);
#if !defined(_PANTHEIOS_NO_PLACEMENT_DELETE)
    void operator delete(void* pv, pan_char_t const*);
#endif /* !_PANTHEIOS_NO_PLACEMENT_DELETE */
    void operator delete(void* pv);

public:
    int ReportEvent(
        int                 severity
    ,   pan_char_t const*   entry
    ,   size_t              cchEntry
    );

private:
    const int   id;
    union
    {
        size_t  len;
        double  dummy;
    } u;
    pan_char_t  m_processIdentity[1];

private:
    WindowsMessageBox_Context(WindowsMessageBox_Context const&);
    WindowsMessageBox_Context &operator =(WindowsMessageBox_Context const&);
};

/* /////////////////////////////////////////////////////////////////////////
 * API functions
 */

static int pantheios_be_WindowsMessageBox_init_(
    pan_char_t const*   processIdentity
,   int                 id
,   void const*         unused
,   void*               reserved
,   void**              ptoken
)
{
    STLSOFT_SUPPRESS_UNUSED(unused);
    STLSOFT_SUPPRESS_UNUSED(reserved);

    // (iii) create the context

    WindowsMessageBox_Context* ctxt = new(processIdentity) WindowsMessageBox_Context(id);

#ifndef STLSOFT_CF_THROW_BAD_ALLOC
    if(NULL == ctxt)
    {
        return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
    }
#endif /* !STLSOFT_CF_THROW_BAD_ALLOC */

    *ptoken = ctxt;

    return 0;
}

PANTHEIOS_CALL(int) pantheios_be_WindowsMessageBox_init(
    pan_char_t const*   processIdentity
,   int                 id
,   void*               unused
,   void*               reserved
,   void**              ptoken
)
{
    return pantheios_call_be_X_init<void>(pantheios_be_WindowsMessageBox_init_, processIdentity, id, unused, reserved, ptoken);
}

PANTHEIOS_CALL(void) pantheios_be_WindowsMessageBox_uninit(void* token)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != token, "token must be non-null");

    WindowsMessageBox_Context* ctxt = static_cast<WindowsMessageBox_Context*>(token);

    delete ctxt;
}

PANTHEIOS_CALL(int) pantheios_be_WindowsMessageBox_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   pan_char_t const*   entry
,   size_t              cchEntry
)
{
    STLSOFT_SUPPRESS_UNUSED(feToken);

    WindowsMessageBox_Context* elc = static_cast<WindowsMessageBox_Context*>(beToken);

    return elc->ReportEvent(severity, entry, cchEntry);
}

/* /////////////////////////////////////////////////////////////////////////
 * WindowsMessageBox_Context
 */

void* WindowsMessageBox_Context::operator new(size_t /* cb */, pan_char_t const* s)
{
    const size_t    len         =   static_cast<size_t>(::lstrlen(s));
    const size_t    cbActual    =   sizeof(WindowsMessageBox_Context) + (1 + len) * sizeof(pan_char_t);
    void*           pv          =   ::operator new(cbActual);

    if(NULL != pv)
    {
        WindowsMessageBox_Context* ctxt = static_cast<WindowsMessageBox_Context*>(pv);

        ctxt->u.len =   len;
        ::lstrcpy(&ctxt->m_processIdentity[0], s);
    }

    return pv;
}

#if !defined(_PANTHEIOS_NO_PLACEMENT_DELETE)
void WindowsMessageBox_Context::operator delete(void*, pan_char_t const*)
{}
#endif /* !_PANTHEIOS_NO_PLACEMENT_DELETE */

void WindowsMessageBox_Context::operator delete(void* pv)
{
    ::operator delete(pv);
}

/* /////////////////////////////////////////////////////////////////////////
 * WindowsMessageBox_Context
 */

int WindowsMessageBox_Context::ReportEvent(
    int                 severity
,   pan_char_t const*   entry
,   size_t              cchEntry
)
{
    HWND    hwnd    =   NULL;
    UINT    type    =   MB_OK;

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(0 == (severity & 0x08), "be.WindowsMessageBox can only be used with the stock severity levels in the range [0, 8). Levels in the range [8, 16) are not allowed");

    switch(severity & 0x0f)
    {
        case    PANTHEIOS_SEV_EMERGENCY:
        case    PANTHEIOS_SEV_ALERT:
        case    PANTHEIOS_SEV_CRITICAL:
        case    PANTHEIOS_SEV_ERROR:
            type |= MB_ICONERROR;
            break;
        case    PANTHEIOS_SEV_WARNING:
            type |= MB_ICONWARNING;
            break;
        case    PANTHEIOS_SEV_NOTICE:
        case    PANTHEIOS_SEV_INFORMATIONAL:
        case    PANTHEIOS_SEV_DEBUG:
            type |= MB_ICONINFORMATION;
            break;
        default:
            type |= MB_ICONINFORMATION;
            break;
    }

    pan_MessageBox_(hwnd, entry, m_processIdentity, type);

    return static_cast<int>(cchEntry);
}

/* ///////////////////////////// end of file //////////////////////////// */
