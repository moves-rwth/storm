/* /////////////////////////////////////////////////////////////////////////
 * File:        src/backends/bec.WindowsDebugger.cpp
 *
 * Purpose:     Implementation for the WindowsDebugger back-end
 *
 * Created:     18th July 2005
 * Updated:     23rd May 2011
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2011, Matthew Wilson and Synesis Software
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


/* Pantheios Header files */
#include <pantheios/pantheios.h>
#include <pantheios/internal/winlean.h>
#define PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
#include <pantheios/backends/bec.WindowsDebugger.h>

#include <pantheios/init_codes.h>
#include <pantheios/quality/contract.h>
#include <pantheios/util/core/apidefs.hpp>
#include <pantheios/util/backends/context.hpp>

/* STLSoft Header files */
#include <pantheios/util/memory/auto_buffer_selector.hpp>
#include <winstl/memory/processheap_allocator.hpp>

/* Standard C Header files */
#include <stdio.h>

/* /////////////////////////////////////////////////////////////////////////
 * String encoding compatibility
 */

#ifdef PANTHEIOS_USE_WIDE_STRINGS
# define pan_OutputDebugString_         ::OutputDebugStringW
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
# define pan_OutputDebugString_         ::OutputDebugStringA
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

namespace
{

#if !defined(PANTHEIOS_NO_NAMESPACE)

    using ::pantheios::pan_char_t;
    using ::pantheios::pan_uint16_t;
    using ::pantheios::pan_uint32_t;
    using ::pantheios::pan_slice_t;
    using ::pantheios::util::backends::Context;

#endif /* !PANTHEIOS_NO_NAMESPACE */


#if !defined(PANTHEIOS_NO_NAMESPACE)
    typedef pantheios::util::auto_buffer_selector<
#else /* ? !PANTHEIOS_NO_NAMESPACE */
    typedef auto_buffer_selector<
#endif /* !PANTHEIOS_NO_NAMESPACE */
        pan_char_t
    ,   2048
    ,   winstl_ns_qual(processheap_allocator)<pan_char_t>
    >::type                                     buffer_t;

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * Structures
 */

namespace
{

    struct WindowsDebugger_Context
        : public Context
    {
    /// \name Member Types
    /// @{
    public:
        typedef Context                     parent_class_type;
        typedef WindowsDebugger_Context     class_type;
    /// @}

    /// \name Member Constants
    /// @{
    public:
        enum
        {
            severityMask    =   0x0f
        };
    /// @}

    /// \name Construction
    /// @{
    public:
        WindowsDebugger_Context(
            pan_char_t const*                       processIdentity
        ,   int                                     backEndId
        ,   pan_be_WindowsDebugger_init_t const*    init
        );
        ~WindowsDebugger_Context() throw();
    /// @}

    /// \name Overrides
    /// @{
    private:
        virtual int rawLogEntry(
            int                 severity4
        ,   int                 severityX
        ,   const pan_slice_t (&ar)[rawLogArrayDimension]
        ,   size_t              cchTotal
        );
        virtual int rawLogEntry(
            int                 severity4
        ,   int                 severityX
        ,   pan_char_t const*   entry
        ,   size_t              cchEntry
        );
    /// @}
    };

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * API functions
 */

PANTHEIOS_CALL(void) pantheios_be_WindowsDebugger_getDefaultAppInit(pan_be_WindowsDebugger_init_t* init) /* throw() */
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != init, "initialisation structure pointer may not be null");

    init->version   =   PANTHEIOS_VER;
    init->flags     =   0;
}

static int pantheios_be_WindowsDebugger_init_(
    pan_char_t const*                       processIdentity
,   int                                     backEndId
,   pan_be_WindowsDebugger_init_t const*    init
,   void*                                   reserved
,   void**                                  ptoken
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != processIdentity, "process identity may not be the null string");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API('\0' != 0[processIdentity], "process identity may not be the empty string");
    STLSOFT_SUPPRESS_UNUSED(reserved);
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != ptoken, "token pointer may not be null");

    /* (i) apply Null Object (Variable) pattern */

    pan_be_WindowsDebugger_init_t init_;

    if(NULL == init)
    {
        pantheios_be_WindowsDebugger_getDefaultAppInit(&init_);

        init = &init_;

#ifdef PANTHEIOS_BE_USE_CALLBACK
        pantheios_be_WindowsDebugger_getAppInit(backEndId, &init_);
#endif /* PANTHEIOS_BE_USE_CALLBACK */
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

    WindowsDebugger_Context* ctxt = new WindowsDebugger_Context(processIdentity, backEndId, init);

#ifndef STLSOFT_CF_THROW_BAD_ALLOC
    if( NULL == ctxt ||
        NULL == ctxt->getProcessIdentity())
    {
        delete ctxt;

        return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
    }
#endif /* !STLSOFT_CF_THROW_BAD_ALLOC */

    *ptoken = ctxt;

    return 0;
}

PANTHEIOS_CALL(int) pantheios_be_WindowsDebugger_init(
    pan_char_t const*                       processIdentity
,   int                                     backEndId
,   pan_be_WindowsDebugger_init_t const*    init
,   void*                                   reserved
,   void**                                  ptoken
)
{
    return pantheios_call_be_X_init<pan_be_WindowsDebugger_init_t>(pantheios_be_WindowsDebugger_init_, processIdentity, backEndId, init, reserved, ptoken);
}

PANTHEIOS_CALL(void) pantheios_be_WindowsDebugger_uninit(void* token)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != token, "token must be non-null");

    WindowsDebugger_Context* ctxt = static_cast<WindowsDebugger_Context*>(token);

    delete ctxt;
}

static int pantheios_be_WindowsDebugger_logEntry_(
    void*               feToken
,   void*               beToken
,   int                 severity
,   pan_char_t const*   entry
,   size_t              cchEntry
)
{
    STLSOFT_SUPPRESS_UNUSED(feToken);

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != beToken, "back-end token must be non-null");

    Context* ctxt = static_cast<Context*>(beToken);

    return ctxt->logEntry(severity, entry, cchEntry);

}

PANTHEIOS_CALL(int) pantheios_be_WindowsDebugger_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   pan_char_t const*   entry
,   size_t              cchEntry
)
{
    return pantheios_call_be_logEntry(pantheios_be_WindowsDebugger_logEntry_, feToken, beToken, severity, entry, cchEntry);
}

/* /////////////////////////////////////////////////////////////////////////
 * WindowsDebugger_Context
 */

WindowsDebugger_Context::WindowsDebugger_Context(
    pan_char_t const*                       processIdentity
,   int                                     backEndId
,   pan_be_WindowsDebugger_init_t const*    init
)
    : parent_class_type(processIdentity, backEndId, init->flags, WindowsDebugger_Context::severityMask)
{}

WindowsDebugger_Context::~WindowsDebugger_Context() throw()
{}

int WindowsDebugger_Context::rawLogEntry(int /* severity4 */, int /* severityX */, const pan_slice_t (&ar)[rawLogArrayDimension], size_t cchTotal)
{
    // Allocate the buffer

    buffer_t buff(cchTotal + 1 + 1);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    // When exception support is not enabled, failure to allocate will yield an empty instance
    if(buff.empty())
    {
        return 0;
    }
    else
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
    {
        size_t nWritten = concatenateSlices(&buff[0], buff.size(), rawLogArrayDimension, &ar[0]);

        PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_INTERNAL(nWritten == cchTotal, "Written length differs from allocated length");

        buff[nWritten++] = '\n';
        buff[nWritten] = '\0';

        PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(nWritten + 1 == buff.size());

        // Output

        pan_OutputDebugString_(&buff[0]);

        return static_cast<int>(nWritten);
    }
}

int WindowsDebugger_Context::rawLogEntry(
    int                 /* severity4 */
,   int                 /* severityX */
,   pan_char_t const*   entry
,   size_t              cchEntry
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_INTERNAL('\0' == entry[cchEntry], "(off-the-)end character must be the nul-terminator");

    // Output

    pan_OutputDebugString_(entry);

    return static_cast<int>(cchEntry);
}

/* ///////////////////////////// end of file //////////////////////////// */
