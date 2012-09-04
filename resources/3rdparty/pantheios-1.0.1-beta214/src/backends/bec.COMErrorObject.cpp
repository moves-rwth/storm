/* /////////////////////////////////////////////////////////////////////////
 * File:        src/backends/bec.COMErrorObject.cpp
 *
 * Purpose:     Implementation for the COMErrorObject back-end
 *
 * Created:     9th April 2006
 * Updated:     27th December 2010
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


/* Pantheios Header Files */
#include <pantheios/pantheios.h>
#include <pantheios/internal/winlean.h>
#include <pantheios/internal/nox.h>
#define PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
#include <pantheios/backends/bec.COMErrorObject.h>

#include <pantheios/init_codes.h>
#include <pantheios/quality/contract.h>
#include <pantheios/util/core/apidefs.hpp>
#include <pantheios/util/backends/arguments.h>

/* STLSoft Header Files */
#include <pantheios/util/memory/auto_buffer_selector.hpp>
#include <stlsoft/shims/access/string/std/c_string.h>

  /* This is necessary because GCC's uuid.lib doesn't have IID_IErrorInfo. */
#include <comstl/comstl.h>

extern "C" const IID IID_IErrorInfo_;

#define IID_IErrorInfo  IID_IErrorInfo_

#include <comstl/error/errorinfo_functions.h>

#include <winstl/memory/processheap_allocator.hpp>
#include <winstl/conversion/char_conversions.hpp>

/* Standard C Header Files */
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
 * Namespace
 */

namespace
{

#if !defined(PANTHEIOS_NO_NAMESPACE)

    using ::pantheios::pan_char_t;
    using ::pantheios::pan_uint32_t;
    using ::pantheios::pan_slice_t;
    using ::pantheios::pan_sev_t;
    using ::pantheios::pantheios_getStockSeverityString;
    using ::pantheios::pantheios_getStockSeverityStringLength;

#endif /* !PANTHEIOS_NO_NAMESPACE */


#if !defined(PANTHEIOS_NO_NAMESPACE)
    typedef pantheios::util::auto_buffer_selector<
#else /* ? !PANTHEIOS_NO_NAMESPACE */
    typedef auto_buffer_selector<
#endif /* !PANTHEIOS_NO_NAMESPACE */
        pan_char_t
    ,   2048
    ,   winstl::processheap_allocator<pan_char_t>
    >::type                                             buffer_t;

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * Structures
 */

extern "C" const IID IID_IErrorInfo_ = { 0x1CF2B120, 0x547D, 0x101B, { 0x8E, 0x65, 0x08, 0x00, 0x2B, 0x2B, 0xD1, 0x19 } };

/* /////////////////////////////////////////////////////////////////////////
 * Structures
 */

namespace
{

    struct ErrorObject_Context
    {
    public:
        typedef pan_uint32_t uint32_t;

    public:
        union
        {
            size_t      len;
            double      d;
        } u;
        OLECHAR         sz[1];
        const uint32_t  flags;

    public:
        ErrorObject_Context(uint32_t flags)
            : flags(flags)
        {}

        void* operator new(size_t cb, pan_char_t const* s);
#if !defined(PANTHEIOS_NO_PLACEMENT_DELETE_)
        void operator delete(void* pv, pan_char_t const*);
#endif /* !PANTHEIOS_NO_PLACEMENT_DELETE_ */
        void operator delete(void* pv);

    private:
        ErrorObject_Context& operator =(ErrorObject_Context const&);
    };

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * API functions
 */

PANTHEIOS_CALL(void) pantheios_be_COMErrorObject_getDefaultAppInit(pan_be_COMErrorObject_init_t* init) /* throw() */
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != init, "initialisation structure pointer may not be null");

    init->version   =   PANTHEIOS_VER;
    init->flags     =   0;
}

static int pantheios_be_COMErrorObject_init_(
    pan_char_t const*                   processIdentity
,   int                                 backEndId
,   pan_be_COMErrorObject_init_t const* init
,   void*                               reserved
,   void**                              ptoken
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != processIdentity, "process identity may not be NULL");

    STLSOFT_SUPPRESS_UNUSED(backEndId);
    STLSOFT_SUPPRESS_UNUSED(reserved);

    /* (i) apply Null Object (Variable) pattern */

    pan_be_COMErrorObject_init_t init_;

    if(NULL == init)
    {
        pantheios_be_COMErrorObject_getDefaultAppInit(&init_);

        init = &init_;

#ifdef PANTHEIOS_BE_USE_CALLBACK
        pantheios_be_COMErrorObject_getAppInit(backEndId, &init_);
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

    ErrorObject_Context* ctxt = new(processIdentity) ErrorObject_Context(init->flags);

#ifndef STLSOFT_CF_THROW_BAD_ALLOC
    if(NULL == ctxt)
    {
        return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
    }
#endif /* !STLSOFT_CF_THROW_BAD_ALLOC */

    *ptoken = ctxt;

    return 0;
}

PANTHEIOS_CALL(int) pantheios_be_COMErrorObject_init(
    pan_char_t const*                   processIdentity
,   int                                 backEndId
,   pan_be_COMErrorObject_init_t const* init
,   void*                               reserved
,   void**                              ptoken
)
{
    return pantheios_call_be_X_init<pan_be_COMErrorObject_init_t>(pantheios_be_COMErrorObject_init_, processIdentity, backEndId, init, reserved, ptoken);
}

PANTHEIOS_CALL(void) pantheios_be_COMErrorObject_uninit(void* token)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != token, "token may not be null");

    ErrorObject_Context* ctxt = static_cast<ErrorObject_Context*>(token);

    delete ctxt;
}

PANTHEIOS_CALL(int) pantheios_be_COMErrorObject_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   pan_char_t const*   entry
,   size_t              cchEntry
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != beToken, "back-end token may not be null");

    STLSOFT_SUPPRESS_UNUSED(feToken);

    ErrorObject_Context* ctxt = static_cast<ErrorObject_Context*>(beToken);

    if(PANTHEIOS_BE_COMERROROBJECT_F_DONT_OVERWRITE_EXISTING & ctxt->flags)
    {
        IErrorInfo* pei =   NULL;
        HRESULT     hr  =   ::GetErrorInfo(0, &pei);

        if( SUCCEEDED(hr) &&
            NULL != pei)
        {
            ::SetErrorInfo(0, pei);

            return 0;
        }
    }

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(0 == (severity & 0x08), "be.COMErrorObject can only be used with the stock severity levels in the range [0, 8). Levels in the range [8, 16) are not allowed");

    severity &= 0x7;    /* This stock back-end ignores any custom severity information. */

    if(severity >= static_cast<int>(PANTHEIOS_SEV_INFORMATIONAL))
    {
        return static_cast<int>(cchEntry);
    }

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
        // TODO: Determine whether it's more efficient to prepare the
        // string first, and then a=>w it, or vice versa

        // Possible formats:
        //
        //  "[<severity>]: <entry>"
        //  "<entry>"

        int                 bShowSev    =   (0 == (ctxt->flags & PANTHEIOS_BE_INIT_F_NO_SEVERITY));
        pan_char_t const*   sevName     =   pantheios_getStockSeverityString(pan_sev_t(severity));
        const size_t        lenSeverity =   pantheios_getStockSeverityStringLength(pan_sev_t(severity));
        const size_t        len         =   (bShowSev ? (1 + lenSeverity + 3) : 0) + cchEntry;
        buffer_t            buff(1 + len);

#ifndef STLSOFT_CF_THROW_BAD_ALLOC
        if(0 == buff.size())
        {
            return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
        }
#endif /* !STLSOFT_CF_THROW_BAD_ALLOC */

        size_t          cchTotal    =   0;
        pan_char_t*     p           =   &buff[0];

        if(bShowSev)
        {
            // "[%s]: %s" + (bShowSev ? 0 : 6), bShowSev ? sevName : entry, entry

            *p++    =   '[';
            ++cchTotal;

            ::memcpy(p, sevName, lenSeverity * sizeof(pan_char_t));
            cchTotal += lenSeverity;
            p += lenSeverity;

            *p++    =   ']';
            ++cchTotal;

            *p++    =   ':';
            ++cchTotal;

            *p++    =   ' ';
            ++cchTotal;
        }

        ::memcpy(p, entry, cchEntry * sizeof(pan_char_t));
        cchTotal += cchEntry;
        buff[cchTotal] = '\0';
        p += cchEntry;

        if(bShowSev)
        {
            PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(cchTotal + 1 == buff.size());
            PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(cchTotal == size_t(p - buff.data()));
        }
        else
        {
            PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(cchTotal == cchEntry);
            PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(cchTotal + 1 <= buff.size());
            PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(p < &*buff.end());
        }

        STLSOFT_SUPPRESS_UNUSED(p);

#ifdef PANTHEIOS_USE_WIDE_STRINGS
        return static_cast<int>(comstl::set_error_info(&buff[0], ctxt->sz));
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
        return static_cast<int>(comstl::set_error_info(winstl::a2w(&buff[0], cchTotal).c_str(), ctxt->sz));
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc&)
    {
        return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
    }
    catch(std::exception&)
    {
        return PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
    }
    catch(...)
    {
        return PANTHEIOS_INIT_RC_UNKNOWN_FAILURE;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

PANTHEIOS_CALL(int) pantheios_be_COMErrorObject_parseArgs(
    size_t                          numArgs
,   pan_slice_t* const              args
,   pan_be_COMErrorObject_init_t*   init
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API((NULL != args || 0 == numArgs), "argument pointer must be non-null, or number of arguments must be 0");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != init, "initialisation structure pointer may not be null");

    pantheios_be_COMErrorObject_getDefaultAppInit(init);

    // 1. Parse the stock arguments
    int res = pantheios_be_parseStockArgs(numArgs, args, &init->flags);

    if(res >= 0)
    {
        // 2. Parse the custom argument: "overwriteExisting"
        res = pantheios_be_parseBooleanArg(numArgs, args, PANTHEIOS_LITERAL_STRING("overwriteExisting"), false, PANTHEIOS_BE_COMERROROBJECT_F_DONT_OVERWRITE_EXISTING, &init->flags);
    }

    return res;
}

/* /////////////////////////////////////////////////////////////////////////
 *
 */

void* ErrorObject_Context::operator new(size_t /* cb */, pan_char_t const* s)
{
    const size_t    len         =   static_cast<size_t>(stlsoft::c_str_len(s));
    const size_t    cbActual    =   sizeof(ErrorObject_Context) + sizeof(OLECHAR) * (1 + len);
    void*           pv          =   ::operator new(cbActual);

    if(NULL != pv)
    {
        ErrorObject_Context* ctxt = static_cast<ErrorObject_Context*>(pv);

        ctxt->u.len =   len;
#ifdef PANTHEIOS_USE_WIDE_STRINGS
        ::memcpy(&ctxt->sz[0], s, (1 + len) * sizeof(pan_char_t));
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
        (void)::MultiByteToWideChar(0, 0, s, static_cast<int>(len), &ctxt->sz[0], static_cast<int>(1 + len));
#endif /* PANTHEIOS_USE_WIDE_STRINGS */
        ctxt->u.len[ctxt->sz] = L'\0';
    }

    return pv;
}

#if !defined(PANTHEIOS_NO_PLACEMENT_DELETE_)
void ErrorObject_Context::operator delete(void*, pan_char_t const*)
{}
#endif /* !PANTHEIOS_NO_PLACEMENT_DELETE_ */

void ErrorObject_Context::operator delete(void* pv)
{
    ::operator delete(pv);
}

/* ///////////////////////////// end of file //////////////////////////// */
