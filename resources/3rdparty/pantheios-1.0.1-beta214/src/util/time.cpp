/* /////////////////////////////////////////////////////////////////////////
 * File:        src/util/time.cpp
 *
 * Purpose:     Time functions for use in Pantheios back-ends.
 *
 * Created:     22nd August 2006
 * Updated:     17th March 2012
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2006-2012, Matthew Wilson and Synesis Software
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
#include <pantheios/internal/lean.h>

#include <pantheios/util/time/currenttime.h>

#include <pantheios/quality/contract.h>
#include <pantheios/internal/safestr.h>

#include <stlsoft/algorithms/std/alt.hpp>
#include <stlsoft/conversion/integer_to_string.hpp>
#include <stlsoft/util/minmax.hpp>

#include <platformstl/platformstl.h>

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
# include <new>
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

#include <ctype.h>
#if defined(PLATFORMSTL_OS_IS_UNIX)
# include <sys/time.h>                          // for gettimeofday()
#elif defined(WIN32)
# include <ole2.h>                              // Need this for GCC & DMC++, because of "lean" hiding
# include <winstl/time/conversion_functions.h>  // for FILETIMEToUNIXTime()
# include <winstl/time/format_functions.hpp>    // for winstl::GetTimeFormat_msA()
# if WINSTL_VER_WINSTL_TIME_HPP_FORMAT_FUNCTIONS_MAJOR < 5 || \
     (  WINSTL_VER_WINSTL_TIME_HPP_FORMAT_FUNCTIONS_MAJOR == 5 && \
        WINSTL_VER_WINSTL_TIME_HPP_FORMAT_FUNCTIONS_MINOR == 0 && \
        WINSTL_VER_WINSTL_TIME_HPP_FORMAT_FUNCTIONS_REVISION < 3)
#  error This requires version 5.0.3+ of winstl/time/format_functions.hpp
# endif /* WINSTL_VER_WINSTL_TIME_HPP_FORMAT_FUNCTIONS_???? */
#else /* ? OS */
# error Platform not discriminated
#endif /* ? OS */
#include <stdio.h>
#include <string.h>                             // for memcpy(), memset()
#include <time.h>                               // for time(), strftime(), localtime(), gmtime()

/* /////////////////////////////////////////////////////////////////////////
 * Helper functions
 */

namespace
{
#if !defined(PANTHEIOS_NO_NAMESPACE)
    using pantheios::pan_char_t;
#endif /* !PANTHEIOS_NO_NAMESPACE */

    int calcDecPlaces_(int flags)
    {
        int decPlaces = (PANTHEIOS_GETCURRENTTIME_F_TIME_RES_MASK & flags) >> 8;

        PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(decPlaces < 7);

        return decPlaces % 7;
    }

    time_t getCurrentTime_(int flags, long* microseconds)
    {
        PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != microseconds, "microseconds pointer may not be null");

        int const   favoursSpeed    =   PANTHEIOS_GETCURRENTTIME_F_FAVOUR_SPEED == (PANTHEIOS_GETCURRENTTIME_F_FAVOUR_SPEED & flags);
        int const   favoursAccuracy =   PANTHEIOS_GETCURRENTTIME_F_FAVOUR_ACCURACY == (PANTHEIOS_GETCURRENTTIME_F_FAVOUR_ACCURACY & flags);

        STLSOFT_SUPPRESS_UNUSED(favoursAccuracy); // We assume accuracy, unless specified otherwise

        if( favoursSpeed ||
            (0 == (PANTHEIOS_GETCURRENTTIME_F_TIME_RES_MASK & flags)))
        {
            *microseconds = 0;

            return ::time(NULL);
        }
#if defined(PLATFORMSTL_OS_IS_UNIX)

        struct timeval  tv;

        ::gettimeofday(&tv, NULL);

        *microseconds = tv.tv_usec;

        return tv.tv_sec;

#elif defined(PLATFORMSTL_OS_IS_WINDOWS)

        SYSTEMTIME  st;
        FILETIME    ft;

        ::GetSystemTime(&st);
        ::SystemTimeToFileTime(&st, &ft);

        winstl::sint32_t    usec;
        time_t              t = winstl::FILETIMEToUNIXTime(ft, &usec);

        *microseconds = usec;

        return t;

#else /* ? OS */
# error Platform not discriminated
#endif /* ? OS */
    }

#ifdef PANTHEIOS_USE_WIDE_STRINGS

# define pan_strftime_          wcsftime

#else /* ? PANTHEIOS_USE_WIDE_STRINGS */

# define pan_strftime_          strftime

#endif /* PANTHEIOS_USE_WIDE_STRINGS */

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * API functions
 */

PANTHEIOS_CALL(size_t) pantheios_util_getCurrentTime(pan_beutil_time_t* tm, int flags)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != tm, "time structure pointer may not be null");

#if !defined(PANTHEIOS_NO_NAMESPACE)
    using pantheios::util::pantheios_onBailOut3;
#endif /* !PANTHEIOS_NO_NAMESPACE */

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        int const   bUseSystemTime  =   PANTHEIOS_GETCURRENTTIME_F_USE_SYSTEM_TIME == (PANTHEIOS_GETCURRENTTIME_F_USE_SYSTEM_TIME & flags);
# if !defined(PLATFORMSTL_OS_IS_UNIX)
        int const   usesUnixFormat  =   PANTHEIOS_GETCURRENTTIME_F_USE_UNIX_FORMAT == (PANTHEIOS_GETCURRENTTIME_F_USE_UNIX_FORMAT & flags);
# endif /* !PLATFORMSTL_OS_IS_UNIX */
        int const   hidesDate       =   PANTHEIOS_GETCURRENTTIME_F_HIDE_DATE == (PANTHEIOS_GETCURRENTTIME_F_HIDE_DATE & flags);
        int const   hidesTime       =   PANTHEIOS_GETCURRENTTIME_F_HIDE_TIME == (PANTHEIOS_GETCURRENTTIME_F_HIDE_TIME & flags);
        int const   numDecPlaces    =   calcDecPlaces_(flags);

        PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API((!hidesDate || !hidesTime), "Should not call with flags to hide both date and time");

# if !defined(PLATFORMSTL_OS_IS_UNIX)
        if(usesUnixFormat)
        {
# endif /* !PLATFORMSTL_OS_IS_UNIX */

            pan_char_t                  szTime[101]                     =   { '\0' };
            static pan_char_t const*    tm_fmts[]                       =
            {
                PANTHEIOS_LITERAL_STRING("%b %d %H:%M:%S"),
                PANTHEIOS_LITERAL_STRING("%H:%M:%S"),
                PANTHEIOS_LITERAL_STRING("%b %d"),
                NULL
            };
# ifdef PANTHEIOS_USING_SAFE_STR_FUNCTIONS
            static errno_t      (*fns[2])(struct tm*, time_t const*)    =   { ::localtime_s, ::gmtime_s };
# else /* ? PANTHEIOS_USING_SAFE_STR_FUNCTIONS */
            static struct tm*   (*fns[2])(time_t const*)                =   { ::localtime, ::gmtime };
# endif /* PANTHEIOS_USING_SAFE_STR_FUNCTIONS */

            long        usecs;
            time_t      secs        =   getCurrentTime_(flags, &usecs);
# ifdef PANTHEIOS_USING_SAFE_STR_FUNCTIONS
            struct tm   tm__;
            struct tm*  tm_         =   &tm__;
            errno_t     err         =   fns[0 != bUseSystemTime](tm_, &secs);

            if(0 != err)
            {
                char msg[1001];

                if(0 != ::strerror_s(&msg[0], STLSOFT_NUM_ELEMENTS(msg), err))
                {
                    msg[0] = '\0';
                }

                pantheios_onBailOut3(pantheios::critical, "failed to elicit time", msg);

                return 0;
            }
# else /* ? PANTHEIOS_USING_SAFE_STR_FUNCTIONS */
            struct tm*          tm_         =   fns[0 != bUseSystemTime](&secs);
# endif /* PANTHEIOS_USING_SAFE_STR_FUNCTIONS */
            pan_char_t const*   strftimeFmt =   (NULL != tm->strftimeFmt) ? tm->strftimeFmt : tm_fmts[hidesDate + 2 * hidesTime];
            size_t              cchTime     =   pan_strftime_(szTime, STLSOFT_NUM_ELEMENTS(szTime), strftimeFmt, tm_);

            if( 0 != numDecPlaces &&
                !hidesTime)
            {
                // Algorithm:
                //
                // 1. divide the microseconds by the appropriate power of 10
                // 2. Add '.' to the end of the string
                // 3. Efficiently write the integer into exactly the right position
                // 4. Fill up any gap with leading '0's

                static long const s_divisors[] =
                {
                    0,      // Should never be used; divide-by-0 will trap such a case
                    100000, // Want 10ths: divide by 100,000
                    10000,  // Want 100ths: divide by 10,000
                    1000,   // Want milliseconds: divide by 1,000
                    100,
                    10,
                    1,      // Want microseconds: divide by 1
                    0,      // Should never be used; divide-by-0 will trap such a case
                    0,      // Should never be used; divide-by-0 will trap such a case
                    0,      // Should never be used; divide-by-0 will trap such a case
                    0,      // Should never be used; divide-by-0 will trap such a case
                    0,      // Should never be used; divide-by-0 will trap such a case
                    0,      // Should never be used; divide-by-0 will trap such a case
                    0,      // Should never be used; divide-by-0 will trap such a case
                    0,      // Should never be used; divide-by-0 will trap such a case
                };
                pan_char_t*         e       =   &szTime[0] + cchTime;
                long                divisor =   s_divisors[numDecPlaces];
                pan_char_t const*   r       =   stlsoft::integer_to_string(e + 1, static_cast<size_t>(1 + numDecPlaces), usecs / divisor);

                0[e] = '.';
                stlsoft::std_fill_n(e + 1, static_cast<size_t>(r - (e + 1)), '0');
                { for(int i = 0; i < numDecPlaces; ++i)
                {
                    PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(isdigit(i[e + 1]));
                }}

                cchTime += 1 + numDecPlaces;
            }

            if(0 == tm->capacity)
            {
                tm->len = cchTime;
            }
            else
            {
                tm->len = stlsoft::minimum(tm->capacity, cchTime);

                ::memcpy(tm->str, szTime, tm->len * sizeof(pan_char_t));
                if(tm->len < tm->capacity)
                {
                    tm->str[tm->len] = '\0';
                }
            }

# if defined(PLATFORMSTL_OS_IS_UNIX)
# elif defined(PLATFORMSTL_OS_IS_WINDOWS)
        }
        else
        {
            int (STDAPICALLTYPE *fns[2])(LCID, DWORD, SYSTEMTIME const*, LPCTSTR, LPTSTR, int) =
            {
                    ::GetTimeFormat
#ifdef PANTHEIOS_USE_WIDE_STRINGS
                ,   winstl::GetTimeFormat_msW
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
                ,   winstl::GetTimeFormat_msA
#endif /* PANTHEIOS_USE_WIDE_STRINGS */
            };

            SYSTEMTIME  st;
            size_t      len     =   0;
            LCID        locale  =   bUseSystemTime ? LOCALE_SYSTEM_DEFAULT : LOCALE_USER_DEFAULT;
            size_t      capacity=   tm->capacity;

            ::GetLocalTime(&st);

            if(0 == capacity)
            {
                int total = 0;

                if(!hidesDate)
                {
                    int lenDate =   ::GetDateFormat(locale, 0, &st, NULL, NULL, 0);

                    if(lenDate > 0)
                    {
                        --lenDate;
                    }

                    total += lenDate;
                }

                if(!hidesTime)
                {
                    if(!hidesDate)
                    {
                        ++total; // For the space
                    }

                    int lenTime = fns[(numDecPlaces >= 3)](locale, 0, &st, NULL, NULL, 0);

                    if(lenTime > 0)
                    {
                        --lenTime;
                    }

                    total += lenTime;
                }

                tm->len = static_cast<size_t>(total);
            }
            else
            {
                if(!hidesDate)
                {
                    // Always try and write something, because the function works
                    // if capacity is 0 and returns the number required

                    size_t lenDate = static_cast<size_t>(::GetDateFormat(   locale
                                                                        ,   0
                                                                        ,   &st
                                                                        ,   NULL
                                                                        ,   tm->str + len
                                                                        ,   static_cast<int>(capacity)));

                    if(lenDate > 0)
                    {
                        --lenDate;
                    }

                    len += lenDate;
                }

                if(!hidesTime)
                {
                    if( len > 0 &&
                        len < capacity)
                    {
                        // capacity was !0, and we had some space, so write as much
                        // as possible

                        tm->str[len] = ' ';

                        ++len;
                    }
                    else if(len > 0)
                    {
                        len = 0;
                        capacity = 0;
                    }

                    if(len < capacity)
                    {
                        size_t lenTime = static_cast<size_t>(fns[(numDecPlaces >= 3)](  locale
                                                                                    ,   0
                                                                                    ,   &st
                                                                                    ,   NULL
                                                                                    ,   tm->str + len
                                                                                    ,   static_cast<int>(capacity - len)));

                        if(0 == lenTime)
                        {
                            len = 0; //  Insufficient space, so make sure we don't emit any date info either.
                        }
                        else
                        {
                            --lenTime;

                            len += lenTime;
                        }
                    }
                    else
                    {
                        len = 0;
                    }
                }

                tm->len = len;

                PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(tm->len <= capacity);

                if(tm->len < capacity)
                {
                    tm->str[tm->len] = '\0';
                }
            }
        }
# else /* ? OS */
#  error Operating system not discriminated
# endif /* OS */

        return tm->len;

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc&)
    {
        pantheios_onBailOut3(PANTHEIOS_SEV_CRITICAL, "Out of memory when eliciting timestamp", NULL);
    }
    catch(std::exception&)
    {
        pantheios_onBailOut3(PANTHEIOS_SEV_CRITICAL, "Unspecified exception when eliciting timestamp", NULL);
    }
    catch(...)
    {
        pantheios_onBailOut3(PANTHEIOS_SEV_EMERGENCY, "Unknown error when eliciting timestamp", NULL);
    }

    return 0;
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

/* ///////////////////////////// end of file //////////////////////////// */
