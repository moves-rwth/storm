/* /////////////////////////////////////////////////////////////////////////
 * File:        src/util/be.parse.cpp
 *
 * Purpose:     Utility functions for use in Pantheios back-ends.
 *
 * Created:     19th August 2007
 * Updated:     10th August 2009
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2007-2009, Matthew Wilson and Synesis Software
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


#include <pantheios/pantheios.h>
#include <pantheios/internal/nox.h>
#include <pantheios/backend.h>
#include <pantheios/quality/contract.h>
#include <pantheios/util/backends/arguments.h>

#include <stlsoft/string/string_view.hpp>
#include <stlsoft/string/split_functions.hpp>
#include <stlsoft/string/view_slice_functions.hpp>

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
# include <new>
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

#include <ctype.h>
#ifdef PANTHEIOS_USE_WIDE_STRINGS
# include <wchar.h>
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* /////////////////////////////////////////////////////////////////////////
 * Compiler compatibility
 */

#if defined(STLSOFT_COMPILER_IS_MSVC) && \
            _MSC_VER >= 1300
# pragma warning(disable : 4702)
#endif /* _MSC_VER >= 1400 */

/* /////////////////////////////////////////////////////////////////////////
 * String encoding compatibility
 */

#ifdef PANTHEIOS_USE_WIDE_STRINGS
# define pan_toupper_                   towupper
typedef stlsoft::wstring_view           string_view_t;
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
# define pan_toupper_                   toupper
typedef stlsoft::string_view            string_view_t;
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#ifndef PANTHEIOS_NO_NAMESPACE
using pantheios::pan_uint32_t;
using pantheios::pan_char_t;
using pantheios::pan_slice_t;
using pantheios::util::pantheios_onBailOut3;
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Forward declarations
 */

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
static int pantheios_be_parseBooleanArg_(   size_t              numArgs
                                        ,   pan_slice_t* const  args
                                        ,   pan_char_t const*   argName
                                        ,   int                 flagSuppressesAction
                                        ,   pan_uint32_t        flagValue
                                        ,   pan_uint32_t*       flags);

static int pantheios_be_parseStringArg_(    size_t              numArgs
                                        ,   pan_slice_t* const  args
                                        ,   pan_char_t const*   argName
                                        ,   pan_slice_t*        argValue);
static int pantheios_be_parseStockArgs_(    size_t              numArgs
                                        ,   pan_slice_t* const  args
                                        ,   pan_uint32_t*       flags);
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

/* /////////////////////////////////////////////////////////////////////////
 * Helper functions
 */

namespace
{
    bool has_boolean_flag_value(string_view_t const& value, bool &flagIsOn)
    {
        // Can be one of:   yes, true, on, 1, no, false, off, 0 (in any case)

        size_t  n = value.size();

        if(n < 6)
        {
            pan_char_t copy[6];

            ::memcpy(&copy[0], value.data(), n * sizeof(pan_char_t));
            ::memset(&copy[0] + n, '\0', (6 - n) * sizeof(pan_char_t));

            PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_INTERNAL('\0' == copy[5], "the character at [6] must be the nul-terminator");

            { for(size_t i = 0; '\0' != copy[i]; ++i)
            {
                copy[i] = static_cast<pan_char_t>(pan_toupper_(copy[i]));
            }}

            const string_view_t val2(&copy[0], n);

            if( PANTHEIOS_LITERAL_STRING("1") == val2 ||
                PANTHEIOS_LITERAL_STRING("ON") == val2 ||
                PANTHEIOS_LITERAL_STRING("YES") == val2 ||
                PANTHEIOS_LITERAL_STRING("TRUE") == val2)
            {
                flagIsOn = true;

                return true;
            }
            else if(PANTHEIOS_LITERAL_STRING("0") == val2 ||
                    PANTHEIOS_LITERAL_STRING("OFF") == val2 ||
                    PANTHEIOS_LITERAL_STRING("NO") == val2 ||
                    PANTHEIOS_LITERAL_STRING("FALSE") == val2)
            {
                flagIsOn = false;

                return true;
            }
        }

        return false;
    }

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * API functions
 */

PANTHEIOS_CALL(int) pantheios_be_parseBooleanArg(   size_t              numArgs
                                                ,   pan_slice_t* const  args
                                                ,   pan_char_t const*   argName
                                                ,   int                 flagSuppressesAction
                                                ,   pan_uint32_t        flagValue
                                                ,   pan_uint32_t*       flags)
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
{
    try
    {
        return pantheios_be_parseBooleanArg_(numArgs, args, argName, flagSuppressesAction, flagValue, flags);
    }
    catch(std::bad_alloc&)
    {
        pantheios_onBailOut3(PANTHEIOS_SEV_CRITICAL, "Out of memory when parsing boolean argument", NULL);

        return 0;
    }
    catch(std::exception&)
    {
        pantheios_onBailOut3(PANTHEIOS_SEV_CRITICAL, "Unspecified exception when parsing boolean argument", NULL);

        return 0;
    }
    catch(...)
    {
        pantheios_onBailOut3(PANTHEIOS_SEV_EMERGENCY, "Unknown error when parsing boolean argument", NULL);

        return 0;
    }
}

static int pantheios_be_parseBooleanArg_(   size_t              numArgs
                                        ,   pan_slice_t* const  args
                                        ,   pan_char_t const*   argName
                                        ,   int                 flagSuppressesAction
                                        ,   pan_uint32_t        flagValue
                                        ,   pan_uint32_t*       flags)
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API((NULL != args || 0 == numArgs), "arguments pointer may only be null if the number of arguments is 0");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != argName, "argument name may not be the null string");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API('\0' != 0[argName], "argument name may not be the empty string");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != flags, "flags pointer may not be null");

    int numProcessed = 0;

    { for(size_t i = 0; i < numArgs; ++i)
    {
        pan_slice_t& slice = *(args + i);

        if(0 != slice.len)
        {
            string_view_t   arg(slice.ptr, slice.len);
            string_view_t   name;
            string_view_t   value;

            if(!stlsoft::split(arg, PANTHEIOS_LITERAL_CHAR('='), name, value))
            {
                value = PANTHEIOS_LITERAL_STRING("yes");
            }

            if(name == argName)
            {
                // Now read what is required, compare it with the
                // default flag, and

                bool    flagIsOn;

                if(!has_boolean_flag_value(value, flagIsOn))
                {
                    continue; // Invalid value. Mark to ignore
                }

                ++numProcessed;

                if((!flagIsOn) != (!flagSuppressesAction))
                {
                    *flags |= flagValue;
                }

                slice.len = 0; // Mark this slice as having been processed successfully

                break;
            }
        }
    }}

    return numProcessed;
}

PANTHEIOS_CALL(int) pantheios_be_parseStringArg(size_t              numArgs
                                            ,   pan_slice_t* const  args
                                            ,   pan_char_t const*   argName
                                            ,   pan_slice_t*        argValue)
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
{
    try
    {
        return pantheios_be_parseStringArg_(numArgs, args, argName, argValue);
    }
    catch(std::bad_alloc&)
    {
        pantheios_onBailOut3(PANTHEIOS_SEV_CRITICAL, "Out of memory when parsing string argument", NULL);
    }
    catch(std::exception&)
    {
        pantheios_onBailOut3(PANTHEIOS_SEV_CRITICAL, "Unspecified exception when parsing string argument", NULL);
    }
    catch(...)
    {
        pantheios_onBailOut3(PANTHEIOS_SEV_EMERGENCY, "Unknown error when parsing string argument", NULL);
    }

    return 0;
}

static int pantheios_be_parseStringArg_(    size_t              numArgs
                                        ,   pan_slice_t* const  args
                                        ,   pan_char_t const*   argName
                                        ,   pan_slice_t*        argValue)
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API((NULL != args || 0 == numArgs), "arguments pointer may only be null if the number of arguments is 0");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != argName, "argument name may not be the null string");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API('\0' != 0[argName], "argument name may not be the empty string");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != argValue, "argument value pointer may not be null");

    int numProcessed = 0;

    { for(size_t i = 0; i < numArgs; ++i)
    {
        pan_slice_t& slice = *(args + i);

        if(0 != slice.len)
        {
            string_view_t   arg(slice.ptr, slice.len);
            string_view_t   name;
            string_view_t   value;

            stlsoft::split(arg, PANTHEIOS_LITERAL_CHAR('='), name, value);

            if(name == argName)
            {
                ++numProcessed;

                argValue->len   =   value.size();
                argValue->ptr   =   value.data();

                slice.len = 0; // Mark this slice as having been processed successfully

                break;
            }
        }
    }}

    return numProcessed;
}

PANTHEIOS_CALL(int) pantheios_be_parseStockArgs(size_t              numArgs
                                            ,   pan_slice_t* const  args
                                            ,   pan_uint32_t*       flags)
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
{
    try
    {
        return pantheios_be_parseStockArgs_(numArgs, args, flags);
    }
    catch(std::bad_alloc&)
    {
        pantheios_onBailOut3(PANTHEIOS_SEV_CRITICAL, "Out of memory when parsing stock argument", NULL);
    }
    catch(std::exception&)
    {
        pantheios_onBailOut3(PANTHEIOS_SEV_CRITICAL, "Unspecified exception when parsing stock argument", NULL);
    }
    catch(...)
    {
        pantheios_onBailOut3(PANTHEIOS_SEV_EMERGENCY, "Unknown error when parsing stock argument", NULL);
    }

    return 0;
}

static int pantheios_be_parseStockArgs_(size_t              numArgs
                                    ,   pan_slice_t* const  args
                                    ,   pan_uint32_t*       flags)
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API((NULL != args || 0 == numArgs), "arguments pointer may only be null if the number of arguments is 0");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != flags, "flags pointer may not be null");

    int numProcessed = 0;

    { for(size_t i = 0; i < numArgs; ++i)
    {
        pan_slice_t& slice = *(args + i);

        if(0 != slice.len)
        {
            string_view_t   arg(slice.ptr, slice.len);
            string_view_t   name;
            string_view_t   value;
            bool            flagSuppresses;
            int             flagValue;

            if(!stlsoft::split(arg, PANTHEIOS_LITERAL_CHAR('='), name, value))
            {
                value = PANTHEIOS_LITERAL_STRING("yes");
            }

            if(!name.empty())
            {
                // PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                if(name == PANTHEIOS_LITERAL_STRING("showProcessId"))
                {
                    flagSuppresses  =   true;
                    flagValue       =   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID;
                }
                // PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                else if(name == PANTHEIOS_LITERAL_STRING("showThreadId"))
                {
                    flagSuppresses  =   true;
                    flagValue       =   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID;
                }
                // PANTHEIOS_BE_INIT_F_NO_DATETIME
                else if(name == PANTHEIOS_LITERAL_STRING("showDateTime"))
                {
                    flagSuppresses  =   true;
                    flagValue       =   PANTHEIOS_BE_INIT_F_NO_DATETIME;
                }
                // PANTHEIOS_BE_INIT_F_NO_SEVERITY
                else if(name == PANTHEIOS_LITERAL_STRING("showSeverity"))
                {
                    flagSuppresses  =   true;
                    flagValue       =   PANTHEIOS_BE_INIT_F_NO_SEVERITY;
                }
                // PANTHEIOS_BE_INIT_F_USE_SYSTEM_TIME
                else if(name == PANTHEIOS_LITERAL_STRING("useSystemTime"))
                {
                    flagSuppresses  =   false;
                    flagValue       =   PANTHEIOS_BE_INIT_F_USE_SYSTEM_TIME;
                }
                // PANTHEIOS_BE_INIT_F_DETAILS_AT_START
                else if(name == PANTHEIOS_LITERAL_STRING("showDetailsAtStart"))
                {
                    flagSuppresses  =   false;
                    flagValue       =   PANTHEIOS_BE_INIT_F_DETAILS_AT_START;
                }
                // PANTHEIOS_BE_INIT_F_USE_UNIX_FORMAT
                else if(name == PANTHEIOS_LITERAL_STRING("useUnixFormat") ||
                        name == PANTHEIOS_LITERAL_STRING("useUNIXFormat"))
                {
                    flagSuppresses  =   false;
                    flagValue       =   PANTHEIOS_BE_INIT_F_USE_UNIX_FORMAT;
                }
                // PANTHEIOS_BE_INIT_F_HIDE_DATE
                else if(name == PANTHEIOS_LITERAL_STRING("showDate"))
                {
                    flagSuppresses  =   true;
                    flagValue       =   PANTHEIOS_BE_INIT_F_HIDE_DATE;
                }
                // PANTHEIOS_BE_INIT_F_HIDE_TIME
                else if(name == PANTHEIOS_LITERAL_STRING("showTime"))
                {
                    flagSuppresses  =   true;
                    flagValue       =   PANTHEIOS_BE_INIT_F_HIDE_TIME;
                }
                // PANTHEIOS_BE_INIT_F_HIGH_RESOLUTION
                else if(name == PANTHEIOS_LITERAL_STRING("highResolution"))
                {
                    flagSuppresses  =   false;
                    flagValue       =   PANTHEIOS_BE_INIT_F_HIGH_RESOLUTION;
                }
                // PANTHEIOS_BE_INIT_F_LOW_RESOLUTION
                else if(name == PANTHEIOS_LITERAL_STRING("lowResolution"))
                {
                    flagSuppresses  =   false;
                    flagValue       =   PANTHEIOS_BE_INIT_F_LOW_RESOLUTION;
                }
                else
                {
                    continue; // We ignore any non-stock flags
                }

                // Now read what is required, compare it with the
                // default flag, and

                bool flagIsOn;

                if(!has_boolean_flag_value(value, flagIsOn))
                {
                    continue; // Invalid value. Mark to ignore
                }

                ++numProcessed;

                if((!flagIsOn) != (!flagSuppresses))
                {
                    *flags |= flagValue;
                }

                slice.len = 0; // Mark this slice as having been processed successfully
            }
        }
    }}

    return numProcessed;
}

/* ///////////////////////////// end of file //////////////////////////// */
