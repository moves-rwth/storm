/* /////////////////////////////////////////////////////////////////////////
 * File:        src/frontends/fe.WindowsRegistry.cpp
 *
 * Purpose:     Implementation of the fe.WindowsRegistry front-end.
 *
 * Created:     28th October 2007
 * Updated:     23rd May 2011
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2007-2011, Matthew Wilson and Synesis Software
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
#include <pantheios/frontends/fe.WindowsRegistry.h>

#include <pantheios/init_codes.h>
#include <pantheios/frontend.h>
#include <pantheios/quality/contract.h>

/* STLSoft Header Files */
#include <stlsoft/conversion/char_conversions.hpp>
#include <winstl/registry/reg_key.hpp>
#include <winstl/registry/reg_value.hpp>
#include <winstl/registry/reg_value_sequence.hpp>

#include <new>

/* /////////////////////////////////////////////////////////////////////////
 * Constants & definitions
 */

#ifndef PANTHEIOS_NO_NAMESPACE
namespace
{

    using ::pantheios::pan_char_t;
    using ::pantheios::util::pantheios_onBailOut3;

} // anonymous namespace
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Constants & definitions
 */

namespace
{

    PAN_CHAR_T const    PANTHEIOS_FE_WINDOWSREGISTRY_REG_KEY[]      =   PANTHEIOS_LITERAL_STRING("SOFTWARE\\Synesis Software\\Logging Tools\\Pantheios\\fe.WindowsRegistry");
#ifdef _DEBUG
    PAN_CHAR_T const    PANTHEIOS_FE_WINDOWSREGISTRY_BUILD_LABEL[]  =   PANTHEIOS_LITERAL_STRING("Debug");
    int const           PANTHEIOS_FE_WINDOWSREGISTRY_DEFAULT_FILTER =   0xff;
#else /* ? _DEBUG */
    PAN_CHAR_T const    PANTHEIOS_FE_WINDOWSREGISTRY_BUILD_LABEL[]  =   PANTHEIOS_LITERAL_STRING("Release");
    int const           PANTHEIOS_FE_WINDOWSREGISTRY_DEFAULT_FILTER =   0x7f;
#endif /* _DEBUG */

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * Context
 */

namespace
{

    union fe_WindowsRegistry_Context
    {
    public:
        fe_WindowsRegistry_Context();

    public:
        int isSeverityLogged(int severity) const;

    private:
        struct // NOTE: do we care that this is endian-specific?
        {
            DWORD   showEmergency       :   1;
            DWORD   showAlert           :   1;
            DWORD   showCritical        :   1;
            DWORD   showError           :   1;
            DWORD   showWarning         :   1;
            DWORD   showNotice          :   1;
            DWORD   showInformational   :   1;
            DWORD   showDebug           :   1;
        } levels;
        DWORD   levels_;
    };

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

PANTHEIOS_CALL(int) pantheios_fe_init(
    void*   reserved
,   void**  ptoken
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != ptoken, "token pointer may not be null");
    /* PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(pantheios::isInitialising(), "This can only be called when Pantheios is initialising"); */

    STLSOFT_SUPPRESS_UNUSED(reserved);

    try
    {
        *ptoken = new fe_WindowsRegistry_Context();

        return 0;
    }
    catch(std::bad_alloc&)
    {
        return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
    }
#if 0
    catch(pantheios::init_exception& x)
    {
        pantheios_onBailOut3(x.get_severity(), x.what(), NULL);

        return x.get_return_code();
    }
#endif /* 0 */
    catch(winstl::windows_exception& x)
    {
        pantheios_onBailOut3(PANTHEIOS_SEV_ALERT, x.what(), NULL);

        return (ERROR_FILE_NOT_FOUND == x.get_error_code()) ? PANTHEIOS_FE_INIT_RC_INIT_CONFIG_REQUIRED : PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
    }
    catch(std::exception& x)
    {
        pantheios_onBailOut3(PANTHEIOS_SEV_ALERT, x.what(), NULL);

        return PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
    }
    catch(...)
    {
        return PANTHEIOS_INIT_RC_UNKNOWN_FAILURE;
    }
}

PANTHEIOS_CALL(void) pantheios_fe_uninit(
    void* token
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != token, "token must be non-null");

    fe_WindowsRegistry_Context* ctxt = static_cast<fe_WindowsRegistry_Context*>(token);

    delete ctxt;
}

PANTHEIOS_CALL(PAN_CHAR_T const*) pantheios_fe_getProcessIdentity(
    void* token
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != token, "token must be non-null");
    /* PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(pantheios::isInitialising(), "This can only be called when Pantheios is initialising"); */

    STLSOFT_SUPPRESS_UNUSED(token);

#ifdef PANTHEIOS_BE_USE_CALLBACK
    return pantheios_fe_getAppProcessIdentity();
#else /* ? PANTHEIOS_BE_USE_CALLBACK */
    return PANTHEIOS_FE_PROCESS_IDENTITY;
#endif /* PANTHEIOS_BE_USE_CALLBACK */
}

PANTHEIOS_CALL(int) pantheios_fe_isSeverityLogged(
    void*   token
,   int     severity
,   int     backEndId
)
{
    // Precondition enforcements
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != token, "token must be non-null");
    /* PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(!pantheios::isInitialising(), "This cannot be called when Pantheios is initialising"); */
    /* PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(pantheios::isInitialised(), "This can only be called when Pantheios is initialised"); */

    STLSOFT_SUPPRESS_UNUSED(severity);
    STLSOFT_SUPPRESS_UNUSED(backEndId);

    fe_WindowsRegistry_Context* ctxt = static_cast<fe_WindowsRegistry_Context*>(token);

    return ctxt->isSeverityLogged(severity);
}

/* ////////////////////////////////////////////////////////////////////// */

namespace
{
    using winstl::reg_key;
    using winstl::reg_value;
    using winstl::reg_value_sequence;
    using winstl::reg_string_t;


    fe_WindowsRegistry_Context::fe_WindowsRegistry_Context()
        : levels_(0)
    {
        try
        {
            reg_key     processKey;
            reg_value   value;

            try
            {
                reg_key baseKey = reg_key::create_key(HKEY_CURRENT_USER, PANTHEIOS_FE_WINDOWSREGISTRY_REG_KEY);

                processKey = reg_key(baseKey, PANTHEIOS_FE_PROCESS_IDENTITY);
            }
            catch(winstl::registry_exception& x)
            {
                if(ERROR_FILE_NOT_FOUND != x.get_error_code())
                {
                    throw;
                }

                reg_key baseKey = reg_key::create_key(HKEY_LOCAL_MACHINE, PANTHEIOS_FE_WINDOWSREGISTRY_REG_KEY);

                processKey = reg_key(baseKey, PANTHEIOS_FE_PROCESS_IDENTITY);
            }

            try
            {
                // TODO: Update this to report failures. Needs Windows Registry
                // library to be re-written to behave properly, and:
                // - to throw (when exceptions enabled) when can't open keys
                //   or values
                // - to have has_value() method
                // - use underlying functions that do the tests

                reg_value_sequence  values(processKey);

                { for(reg_value_sequence::const_iterator b = values.begin(); b != values.end(); ++b)
                {
                    reg_value const v(*b);

                    if( REG_NONE == value.type() &&
                        PANTHEIOS_LITERAL_STRING("*") == v.name())
                    {
                        value = v;
                    }
                    else if(PANTHEIOS_FE_WINDOWSREGISTRY_BUILD_LABEL == v.name())
                    {
                        value = v;
                        break;
                    }
                }}
            }
            catch(winstl::registry_exception& x)
            {
                if(ERROR_FILE_NOT_FOUND != x.get_error_code())
                {
                    throw;
                }
            }

            if(REG_NONE == value.type())
            {
                reg_string_t    message;

                message.reserve(255);

                message += PANTHEIOS_LITERAL_STRING("fe.WindowsRegistry: Expected to find values '*' or '");
                message += PANTHEIOS_FE_WINDOWSREGISTRY_BUILD_LABEL;
                message += PANTHEIOS_LITERAL_STRING("' in the registry for this process, at '");
                message += PANTHEIOS_FE_WINDOWSREGISTRY_REG_KEY;
                message += PANTHEIOS_LITERAL_STRING("\\");
                message += PANTHEIOS_FE_PROCESS_IDENTITY;

                pantheios_onBailOut3(
                    PANTHEIOS_SEV_WARNING
#ifdef PANTHEIOS_USE_WIDE_STRINGS
                ,   stlsoft::w2m(message)
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
                ,   message.c_str()
#endif /* PANTHEIOS_USE_WIDE_STRINGS */
                ,   NULL
                );

                levels_ = PANTHEIOS_FE_WINDOWSREGISTRY_DEFAULT_FILTER;
            }
            else
            {
                levels_ = value.value_dword();
            }
        }
        catch(winstl::windows_exception& x)
        {
            if(ERROR_FILE_NOT_FOUND == x.get_error_code())
            {
                reg_string_t message2;

                message2.reserve(255);

                message2 += PANTHEIOS_LITERAL_STRING("LOGGING INIT FAILURE: Could not open the required registry key '");
                message2 += PANTHEIOS_FE_WINDOWSREGISTRY_REG_KEY;
                message2 += '\\';
                message2 += PANTHEIOS_FE_PROCESS_IDENTITY;
                message2 += PANTHEIOS_LITERAL_STRING("' for the process '");
                message2 += PANTHEIOS_FE_PROCESS_IDENTITY;
                message2 += PANTHEIOS_LITERAL_STRING("' in either user hive or machine hive");

#ifdef PANTHEIOS_USE_WIDE_STRINGS
                throw winstl::windows_exception(stlsoft::w2m(message2), x.get_error_code());
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
                throw winstl::windows_exception(message2.c_str(), x.get_error_code());
#endif /* PANTHEIOS_USE_WIDE_STRINGS */
            }

            throw;
        }
    }

    int fe_WindowsRegistry_Context::isSeverityLogged(int severity) const
    {
        int const index = 1 << (severity & 0x0f);

        PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(index >= 1 && index <= 128);

        return 0 != (levels_ & index);
    }

} // anonymous namespace

/* ///////////////////////////// end of file //////////////////////////// */
