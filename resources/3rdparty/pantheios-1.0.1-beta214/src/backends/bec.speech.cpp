/* /////////////////////////////////////////////////////////////////////////
 * File:        src/backends/bec.speech.cpp
 *
 * Purpose:     Implementation for the speech back-end
 *
 * Created:     31st August 2006
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


/* Pantheios Header files */
#include <pantheios/pantheios.h>
#include <pantheios/internal/winlean.h>
#define PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
#include <pantheios/backends/bec.speech.h>

#include <pantheios/init_codes.h>
#include <pantheios/quality/contract.h>
#include <pantheios/util/core/apidefs.hpp>
#include <pantheios/util/backends/arguments.h>
#include <pantheios/util/backends/context.hpp>

/* STLSoft Header files */
#include <pantheios/util/memory/auto_buffer_selector.hpp>
#include <stlsoft/smartptr/ref_ptr.hpp>

#ifdef PANTHEIOS_BE_SPEECH_USE_MS_SAPI_HEADERS
# define COMSTL_SPEECH_SAPI_UTIL_USE_MS_SAPI_HEADERS
#else /* ? PANTHEIOS_BE_SPEECH_USE_MS_SAPI_HEADERS */
#endif /* PANTHEIOS_BE_SPEECH_USE_MS_SAPI_HEADERS */

#include <comstl/speech/sapi_util.hpp>
#include <comstl/util/creation_functions.hpp>
#include <comstl/util/initialisers.hpp>
#include <comstl/util/interface_traits.hpp>

#include <winstl/conversion/char_conversions.hpp>
#include <winstl/error/error_desc.hpp>
#include <winstl/memory/processheap_allocator.hpp>

/* Standard C++ Header files */
#include <string>

/* Standard C Header files */
#include <stdio.h>

/* /////////////////////////////////////////////////////////////////////////
 * Compiler compatibility
 */

#if (   defined(STLSOFT_COMPILER_IS_MSVC) && \
        _MSC_VER < 1200)
# define _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS
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
    using ::pantheios::util::backends::Context;
    using ::pantheios::util::pantheios_onBailOut3;

#endif /* !PANTHEIOS_NO_NAMESPACE */


#if !defined(PANTHEIOS_NO_NAMESPACE)
    typedef pantheios::util::auto_buffer_selector<
#else /* ? !PANTHEIOS_NO_NAMESPACE */
    typedef auto_buffer_selector<
#endif /* !PANTHEIOS_NO_NAMESPACE */
        pan_char_t
    ,   2048
    ,   winstl::processheap_allocator<pan_char_t>
    >::type                                     buffer_t;

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

namespace
{

    struct be_speech_context
        : public Context
    {
    public:
        typedef Context                     parent_class_type;
        typedef be_speech_context           class_type;
        typedef std::string                 string_type;
        typedef stlsoft::ref_ptr<ISpVoice>  voice_type;

    public:
        be_speech_context(
            pan_char_t const*   processIdentity
        ,   int                 id
        ,   pan_uint32_t        flags
        ,   voice_type          voice
        );
        ~be_speech_context() throw();

    // Overrides
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

    private:
        int speak(
            int                 severity
        ,   pan_char_t const*   entry
        ,   size_t              cchEntry
        );

    private:
        comstl::com_initializer coinit;
        voice_type              voice;
    };

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

PANTHEIOS_CALL(void) pantheios_be_speech_getDefaultAppInit(pan_be_speech_init_t* init) /* throw() */
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != init, "initialisation structure pointer may not be null");

    init->version   =   PANTHEIOS_VER;
    init->flags     =   0;
    init->flags     |=  PANTHEIOS_BE_INIT_F_NO_PROCESS_ID;
    init->flags     |=  PANTHEIOS_BE_INIT_F_NO_THREAD_ID;
    init->flags     |=  PANTHEIOS_BE_INIT_F_NO_DATETIME;
}

#ifdef _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS
extern "C++" int pantheios_be_speech_init__cpp(
    pan_char_t const*           processIdentity
,   int                         backEndId
,   pan_be_speech_init_t const* init
,   void*                       reserved
,   void**                      ptoken)
;
    #endif /* _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS */



PANTHEIOS_CALL(int) pantheios_be_speech_init(
    pan_char_t const*           processIdentity
,   int                         backEndId
,   pan_be_speech_init_t const* init
,   void*                       reserved
,   void**                      ptoken
)
#ifdef _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS
{
    return pantheios_be_speech_init__cpp(processIdentity, backEndId, init, reserved, ptoken);
}
int pantheios_be_speech_init__cpp(
    pan_char_t const*           processIdentity
,   int                         backEndId
,   pan_be_speech_init_t const* init
,   void*                       reserved
,   void**                      ptoken
)
#endif /* _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS */
{
    STLSOFT_SUPPRESS_UNUSED(reserved);

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != ptoken, "token pointer may not be null");

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        /* (i) apply Null Object (Variable) pattern */

        pan_be_speech_init_t init_;

        if(NULL == init)
        {
            pantheios_be_speech_getDefaultAppInit(&init_);

            init = &init_;

#ifdef PANTHEIOS_BE_USE_CALLBACK
            pantheios_be_speech_getAppInit(backEndId, &init_);
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

        *ptoken = NULL;

        comstl::com_initializer         coinit;
        be_speech_context::voice_type   voice;
        HRESULT                         hr = comstl::co_create_instance(CLSID_SpVoice, voice);

        if(E_OUTOFMEMORY == hr)
        {
            return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
        }
        else if(FAILED(hr))
        {
            std::string msg("Failed to create the speech server component: ");

            msg += winstl::error_desc_a(hr).c_str();

            pantheios_onBailOut3(PANTHEIOS_SEV_CRITICAL, msg.c_str(), NULL);

            return PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
        }
        else
        {
            be_speech_context* ctxt = new be_speech_context(processIdentity, backEndId, init->flags, voice);

            if(NULL == ctxt)
            {
                return PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
            }

            *ptoken = ctxt;

            return 0;
        }

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
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

PANTHEIOS_CALL(void) pantheios_be_speech_uninit(void* token)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != token, "token must be non-null");

    delete static_cast<be_speech_context*>(token);
}

static int pantheios_be_speech_logEntry_(
    void*               /* feToken */
,   void*               beToken
,   int                 severity
,   pan_char_t const*   entry
,   size_t              cchEntry
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != beToken, "back-end token must be non-null");

    be_speech_context* ctxt = static_cast<be_speech_context*>(beToken);

    return ctxt->logEntry(severity, entry, cchEntry);
}

PANTHEIOS_CALL(int) pantheios_be_speech_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   pan_char_t const*   entry
,   size_t              cchEntry
)
{
    return pantheios_call_be_logEntry(pantheios_be_speech_logEntry_, feToken, beToken, severity, entry, cchEntry);
}

PANTHEIOS_CALL(int) pantheios_be_speech_parseArgs(
    size_t                  numArgs
,   pan_slice_t* const      args
,   pan_be_speech_init_t*   init
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API((NULL != args || 0 == numArgs), "argument pointer must be non-null, or number of arguments must be 0");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != init, "initialisation structure pointer may not be null");

    pantheios_be_speech_getDefaultAppInit(init);

    // 1. Parse the stock arguments
    int res = pantheios_be_parseStockArgs(numArgs, args, &init->flags);

    if(res >= 0)
    {
        // 2.a Parse the custom argument: "synchronous"
        res = pantheios_be_parseBooleanArg(numArgs, args, PANTHEIOS_LITERAL_STRING("synchronous"), true, PANTHEIOS_BE_SPEECH_F_SYNCHRONOUS, &init->flags);
    }

    if(res >= 0)
    {
        // 2.b Parse the custom argument: "purgeBeforeSpeak"
        res = pantheios_be_parseBooleanArg(numArgs, args, PANTHEIOS_LITERAL_STRING("purgeBeforeSpeak"), true, PANTHEIOS_BE_SPEECH_F_PURGE_BEFORE_SPEAK, &init->flags);
    }

    if(res >= 0)
    {
        // 2.c Parse the custom argument: "speakPunctuation"
        res = pantheios_be_parseBooleanArg(numArgs, args, PANTHEIOS_LITERAL_STRING("speakPunctuation"), true, PANTHEIOS_BE_SPEECH_F_SPEAK_PUNCTUATION, &init->flags);
    }

    if(res >= 0)
    {
        // 2.d Parse the custom argument: "synchronousOnCritical"
        res = pantheios_be_parseBooleanArg(numArgs, args, PANTHEIOS_LITERAL_STRING("synchronousOnCritical"), true, PANTHEIOS_BE_SPEECH_F_SYNCHRONOUS_ON_CRITICAL, &init->flags);
    }

    return res;
}

/* ////////////////////////////////////////////////////////////////////// */

be_speech_context::be_speech_context(
    pan_char_t const*   processIdentity
,   int                 id
,   pan_uint32_t        flags
,   voice_type          voice
)
    : parent_class_type(processIdentity, id, flags, 0x0f)
    , voice(voice)
{}

be_speech_context::~be_speech_context() throw()
{
    if(PANTHEIOS_BE_SPEECH_F_UNINIT_DISCARD_WORKAROUND & m_flags)
    {
        voice.detach();
    }
    else
    {
        voice->WaitUntilDone(10000);
    }
}

int be_speech_context::rawLogEntry(int severity4, int /* severityX */, const pan_slice_t (&ar)[rawLogArrayDimension], size_t cchTotal)
{
    // Define auto_buffer to use Windows process heap

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

        return this->speak(severity4, buff.data(), nWritten);
    }
}

int be_speech_context::rawLogEntry(
    int                 severity4
,   int                 /* severityX */
,   pan_char_t const*   entry
,   size_t              cchEntry
)
{
    return this->speak(severity4, entry, cchEntry);
}

int be_speech_context::speak(
    int                 severity
,   pan_char_t const*   entry
,   size_t              cchEntry
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_INTERNAL(0 == (severity & 0x08), "be.speech can only be used with the stock severity levels in the range [0, 8). Levels in the range [8, 16) are not allowed");

    const int   severityLevel   =   severity & 0x0f;
    DWORD       flags           =   0
                                |   SPF_IS_NOT_XML
                                |   SPF_ASYNC
                                ;

    if(PANTHEIOS_BE_SPEECH_F_SYNCHRONOUS & this->m_flags)
    {
        flags &= ~SPF_ASYNC;
    }

    if(PANTHEIOS_BE_SPEECH_F_PURGE_BEFORE_SPEAK & this->m_flags)
    {
        flags |= SPF_PURGEBEFORESPEAK;
    }

    if(PANTHEIOS_BE_SPEECH_F_SPEAK_PUNCTUATION & this->m_flags)
    {
        flags |= SPF_NLP_SPEAK_PUNC;
    }

    if(PANTHEIOS_BE_SPEECH_F_SYNCHRONOUS_ON_CRITICAL & this->m_flags)
    {
        switch(severityLevel)
        {
            case    PANTHEIOS_SEV_EMERGENCY:
            case    PANTHEIOS_SEV_ALERT:
            case    PANTHEIOS_SEV_CRITICAL:
                flags &= SPF_ASYNC;
                flags |= SPF_PURGEBEFORESPEAK;
                break;
            default:
                break;
        }
    }


    try
    {
#ifdef PANTHEIOS_USE_WIDE_STRINGS
        voice->Speak(entry, flags, 0);
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
        voice->Speak(winstl::a2w(entry, cchEntry), flags, 0);
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

        return static_cast<int>(cchEntry);
    }
    catch(std::exception &)
    {
        return 0;
    }
}

/* ///////////////////////////// end of file //////////////////////////// */
