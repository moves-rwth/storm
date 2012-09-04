/* /////////////////////////////////////////////////////////////////////////
 * File:        src/xtests.core.cpp (formerly part of Synesis' internal test codebase)
 *
 * Purpose:     Implementation for xTests core library.
 *
 * Created:     20th June 1999
 * Updated:     6th August 2012
 *
 * Home:        http://stlsoft.org/
 *
 * Copyright (c) 1999-2012, Matthew Wilson and Synesis Software
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


/* xTests Header Files */
#ifndef _XTESTS_NO_CPP_API
# define _XTESTS_NO_CPP_API
#endif /* !_XTESTS_NO_CPP_API */
#include <xtests/xtests.h>
#include <xtests/internal/safestr.h>

/* Warning suppressions */
#if defined(STLSOFT_COMPILER_IS_MSVC)
# ifdef XTESTS_USING_SAFE_STR_FUNCTIONS
#  pragma warning(disable : 4996)
# endif /* XTESTS_USING_SAFE_STR_FUNCTIONS */
# if defined(STLSOFT_CF_EXCEPTION_SUPPORT)
#  pragma warning(disable : 4702)
# else /* ? STLSOFT_CF_EXCEPTION_SUPPORT */
#  pragma warning(disable : 4530)
# endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
#endif /* compiler */

#if defined(XTESTS_USE_PANTHEIOS)
# include <pantheios/pantheios.h>
#endif /* XTESTS_USE_PANTHEIOS */

/* STLSoft Header Files */
#include <stlsoft/conversion/char_conversions.hpp>
#include <stlsoft/memory/auto_buffer.hpp>
#include <stlsoft/shims/access/string/std/c_string.h>
#include <stlsoft/string/case_functions.hpp>
#include <stlsoft/string/string_traits_fwd.hpp>
#if defined(STLSOFT_COMPILER_IS_WATCOM) || \
    (   defined(STLSOFT_COMPILER_IS_MSVC) && \
        _MSC_VER < 1300)
#else /* ? compiler */
# include <stlsoft/util/must_init.hpp>
#endif /* compiler */
#include <stlsoft/util/integral_printf_traits.hpp>

/* Standard C++ Header Files */
#include <map>
#include <string>

/* Standard C Header Files */
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* /////////////////////////////////////////////////////////////////////////
 * Compatiblity tests
 */

/* Warning suppressions */
#if defined(STLSOFT_COMPILER_IS_MSVC)
# if defined(STLSOFT_CF_EXCEPTION_SUPPORT)
# else /* ? STLSOFT_CF_EXCEPTION_SUPPORT */
#  pragma warning(default : 4530)
# endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
# if defined(STLSOFT_CF_RTTI_SUPPORT)
# else /* ? STLSOFT_CF_RTTI_SUPPORT */
#  pragma warning(default : 4541)
# endif /* STLSOFT_CF_RTTI_SUPPORT */
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * Implementation
 */

#define XTESTS_VERBOSITY_VALID_MISSING_CASES    \
                                                \
                case    5:                      \
                case    6:                      \
                case    7:                      \
                case    8:                      \
                                                \


/* /////////////////////////////////////////////////////////////////////////
 * Compatiblity
 */

/* Windows debugging support
 *
 * XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_ == 0   =>  no support
 * XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_ == 1   =>  Windows build
 * XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_ == 2   =>  Windows-emulation build
 */

#ifdef XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_
# undef XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_
#endif /* XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_ */

#if defined(WIN32) || \
    defined(WIN64)
# define XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_  1
#elif (   defined(STLSOFT_COMPILER_IS_MSVC) || \
          defined(STLSOFT_COMPILER_IS_UNKNOWN)) && \
      defined(_MSC_VER) && \
      !defined(WIN32) && \
      defined(UNIX) && \
      defined(_WIN32)
# define XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_  2
#else
# define XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_  0
#endif

#ifdef XTESTS_USING_SAFE_STR_FUNCTIONS
# define xtests_strcpy_(d, n, s)                    ::strcpy_s(d, n, s)
# define xtests_wcscpy_(d, n, s)                    ::wcscpy_s(d, n, s)
//# define xtests_strncpy_(d, n, s, l)                ::strncpy_s(d, n, s, l)
//# define xtests_wcsncpy_(d, n, s, l)                ::wcsncpy_s(d, n, s, l)
# define xtests_sprintf_2_(s, n, fmt, a0, a1)       ::sprintf_s(s, n, fmt, a0, a1)
# define xtests_sprintf_3_(s, n, fmt, a0, a1, a2)   ::sprintf_s(s, n, fmt, a0, a1, a2)
# define xtests_vsnprintf_(s, z, n, fmt, args)      ::vsnprintf_s(s, z, n, fmt, args)
#else /* ? XTESTS_USING_SAFE_STR_FUNCTIONS */
# define xtests_strcpy_(d, n, s)                    ::strcpy(d, s)
# define xtests_wcscpy_(d, n, s)                    ::wcscpy(d, s)
//# define xtests_strncpy_(d, n, s, l)                ::strncpy(d, s, l)
//# define xtests_wcsncpy_(d, n, s, l)                ::wcsncpy(d, s, l)
# define xtests_sprintf_2_(s, n, fmt, a0, a1)       ::sprintf(s, fmt, a0, a1)
# define xtests_sprintf_3_(s, n, fmt, a0, a1, a2)   ::sprintf(s, fmt, a0, a1, a2)
# if defined(STLSOFT_COMPILER_IS_DMC) || \
     (   (   defined(WIN32) || \
             defined(WIN64)) && \
         (   defined(STLSOFT_COMPILER_IS_INTEL) || \
             (   defined(STLSOFT_COMPILER_IS_COMO) && \
                 defined(_MSC_VER)))) || \
     defined(STLSOFT_COMPILER_IS_MSVC)
#  define xtests_vsnprintf_(s, z, n, fmt, args)     ::_vsnprintf(s, n, fmt, args)
# else /* ? compiler / OS */
#  define xtests_vsnprintf_(s, z, n, fmt, args)     ::vsnprintf(s, n, fmt, args)
# endif /* compiler / OS */
#endif /* XTESTS_USING_SAFE_STR_FUNCTIONS */

#if defined(STLSOFT_COMPILER_IS_DMC)
# define RETURN_UNUSED(x)                           return x
#else /* ? compiler */
# define RETURN_UNUSED(x)
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * Windows debugging support
 */

#if XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_
static void xtests_OutputDebugStringA_(char const*);
# define OutputDebugStringA     xtests_OutputDebugStringA_
#endif /* XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_ */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

namespace
{

    typedef stlsoft::auto_buffer<char>      char_buffer_t_;
    typedef stlsoft::auto_buffer<wchar_t>   char_buffer_w_t_;

} // anonymous namespace

#ifndef _STLSOFT_NO_NAMESPACE
namespace stlsoft
{
#endif /* _STLSOFT_NO_NAMESPACE */

    STLSOFT_TEMPLATE_SPECIALISATION
    struct string_traits< char_buffer_t_>
    {
        typedef char_buffer_t_::value_type      char_type;
    };

    STLSOFT_TEMPLATE_SPECIALISATION
    struct string_traits< char_buffer_w_t_>
    {
        typedef char_buffer_w_t_::value_type    char_type;
    };

#ifndef _STLSOFT_NO_NAMESPACE
} /* namespace stlsoft */
#endif /* _STLSOFT_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#ifndef _XTESTS_NO_NAMESPACE
namespace xtests
{
namespace c
{
#endif /* !_XTESTS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

#ifdef STLSOFT_CF_NAMESPACE_SUPPORT
namespace
{
#endif /* STLSOFT_CF_NAMESPACE_SUPPORT */

#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    typedef stlsoft::ss_sint32_t                sint32_t;
    typedef stlsoft::ss_uint32_t                uint32_t;
    typedef stlsoft::ss_sint64_t                sint64_t;
    typedef stlsoft::ss_uint64_t                uint64_t;
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */

    typedef std::string                         string_t;

    struct TestInfo
    {
        string_t    name;
        string_t    description;
        unsigned    totalConditions;
        unsigned    failedConditions;
        unsigned    unexpectedExceptions;
        unsigned    expectedExceptions;

        TestInfo(char const* name, char const* description)
            : name(name)
            , description(description)
            , totalConditions(0)
            , failedConditions(0)
            , unexpectedExceptions(0)
            , expectedExceptions(0)
        {}

        bool haveAllTestsPassed() const
        {
            return (0 == failedConditions && 0 ==  unexpectedExceptions && 0 == expectedExceptions);
        }
    };


    typedef std::map<string_t, TestInfo>        test_map_t;


    class RunnerInfo
    {
    public:
        RunnerInfo(
            char const*         name
        ,   int                 verbosity
        ,   xTests_Reporter_t*  reporter
        ,   void*               reporterParam
        ,   FILE*               stm
        ,   int                 flags
        ,   xTests_Setup_t      setup
        ,   xTests_Teardown_t   teardown
        ,   void*               setupParam
        );

        int BeginCase(char const* name, char const* description);
        int EndCase(char const* name);

        int RegisterSuccessfulCondition(
            char const* file
        ,   int         line
        ,   char const* function
        ,   char const* expr
        );
        int RegisterFailedCondition(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             expr
        );
        int RegisterFailedCondition_long(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             expr
        ,   long                    expected
        ,   long                    actual
        ,   xtests_comparison_t     comp
        );
        int RegisterFailedCondition_ulong(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             expr
        ,   unsigned long           expected
        ,   unsigned long           actual
        ,   xtests_comparison_t     comp
        );
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
        int RegisterFailedCondition_sint64(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             expr
        ,   sint64_t                expected
        ,   sint64_t                actual
        ,   xtests_comparison_t     comp
        );
        int RegisterFailedCondition_uint64(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             expr
        ,   uint64_t                expected
        ,   uint64_t                actual
        ,   xtests_comparison_t     comp
        );
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
        int RegisterFailedCondition_boolean(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             expr
        ,   bool                    expected
        ,   bool                    actual
        ,   xtests_comparison_t     comp
        );
        int RegisterFailedCondition_double(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             expr
        ,   double const&           expected
        ,   double const&           actual
        ,   xtests_comparison_t     comp
        );
        int TestMultibyteStrings(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             expr
        ,   char const*             expected
        ,   char const*             actual
        ,   xtests_comparison_t     comp
        );
        int TestMultibyteStringsN(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             expr
        ,   char const*             expected
        ,   char const*             actual
        ,   ptrdiff_t               n /* exact if +ve; limit if -ve */
        ,   size_t                  cchExpected
        ,   size_t                  cchActual
        ,   xtests_comparison_t     comp
        );
        int TestWideStrings(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             expr
        ,   wchar_t const*          expected
        ,   wchar_t const*          actual
        ,   xtests_comparison_t     comp
        );
        int TestWideStringsN(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             expr
        ,   wchar_t const*          expected
        ,   wchar_t const*          actual
        ,   int                     n /* exact if +ve; limit if -ve */
        ,   size_t                  cchExpected
        ,   size_t                  cchActual
        ,   xtests_comparison_t     comp
        );
        int TestMultibyteStringContains(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             expr
        ,   char const*             expected
        ,   char const*             actual
        ,   xtests_comparison_t     comp
        );
        int TestWideStringContains(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             expr
        ,   wchar_t const*          expected
        ,   wchar_t const*          actual
        ,   xtests_comparison_t     comp
        );
        int TestPointers(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             expr
        ,   void volatile const*    expected
        ,   void volatile const*    actual
        ,   xtests_comparison_t     comp
        );
        int TestFunctionPointers(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             expr
        ,   void                    (*expected)(void)
        ,   void                    (*actual)(void)
        ,   xtests_comparison_t     comp
        );
        int TestCharacters(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             expr
        ,   char                    expected
        ,   char                    actual
        ,   xtests_comparison_t     comp
        );
        int TestCharacters(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             expr
        ,   wchar_t                 expected
        ,   wchar_t                 actual
        ,   xtests_comparison_t     comp
        );
        int WriteFailMessage(
            char const*             file
        ,   int                     line
        ,   char const*             function
        ,   char const*             message
        ,   char const*             qualifyingInformation
        );
        void CaseExcepted(
            char const* exceptionType
        ,   char const* exceptionMessage
        );
        void CaseExceptionExpected(char const* exceptionType);

        void OnRequireFailed();
        int HasRequiredConditionFailed() const;

        void PrintStart();
        void PrintEnd();
        void PrintResults();
        void onAbend(char const* message);
        size_t NumberOfFailedTestCases() const;

    private:
#if defined(STLSOFT_INCL_STLSOFT_UTIL_HPP_MUST_INIT)
        typedef stlsoft::must_init<int>             int_type;
        typedef stlsoft::must_init<uint32_t>        unsigned_type;
#else /* ? STLSOFT_INCL_STLSOFT_UTIL_HPP_MUST_INIT */
        typedef int                                 int_type;
        typedef uint32_t                            unsigned_type;
#endif /* STLSOFT_INCL_STLSOFT_UTIL_HPP_MUST_INIT */

    private:
        /* static */ void Call_onTestFailed(
            xTests_Reporter_t* const    reporter
        ,   void*                       reporterParam
        ,   char const*                 file
        ,   int                         line
        ,   char const*                 function
        ,   char const*                 expr
        ,   xtests_variable_t const*    expectedValue
        ,   xtests_variable_t const*    actualValue
        ,   ptrdiff_t                   length
        ,   xtests_comparison_t         comparison
        ,   int                         verbosity
        );

    private:
        static xTests_Reporter_t*   get_reporter_(
            xTests_Reporter_t*  reporter
        ,   FILE*               stm
        ,   int                 flags
        );
        void report_unstartedCase_defect_();

    private:
        xTests_Reporter_t* const    m_reporter;
        void* const                 m_reporterParam;
        string_t const              m_name;
        int_type const              m_verbosity;
        int const                   m_flags;
        xTests_Setup_t const        m_setup;
        xTests_Teardown_t const     m_teardown;
        void* const                 m_setupParam;
        test_map_t                  m_testCases;
        unsigned_type               m_totalConditions;
        unsigned_type               m_failedConditions;
        unsigned_type               m_unexpectedExceptions;
        unsigned_type               m_expectedExceptions;
        test_map_t::iterator        m_currentCase;
        bool                        m_requireFailed;

    private:
        RunnerInfo &operator =(RunnerInfo const&);
    };

    ////////////////////////////////////////////////////////////////////////
    // Non-local variables

    static RunnerInfo*      s_runner;
    static double           s_fpApproximateFactor   =   XTESTS_FP_APPROXIMATE_FACTOR;

#ifdef STLSOFT_CF_NAMESPACE_SUPPORT
} // anonymous namespace
#endif /* STLSOFT_CF_NAMESPACE_SUPPORT */

/* /////////////////////////////////////////////////////////////////////////
 * Helper functions
 */

#ifdef STLSOFT_CF_NAMESPACE_SUPPORT
namespace
{
#endif /* STLSOFT_CF_NAMESPACE_SUPPORT */

    enum xtests_severity_t
    {
            xtestsEmergency     =   0
        ,   xtestsAlert         =   1
        ,   xtestsCritical      =   2
        ,   xtestsError         =   3
        ,   xtestsWarning       =   4
        ,   xtestsNotice        =   5
        ,   xtestsInformational =   6
        ,   xtestsDebug         =   7
    };

    static int xtests_output_(xtests_severity_t sev, char const* message)
    {
#if defined(XTESTS_USE_PANTHEIOS)

# if PANTHEIOS_VER >= 0x0100018b
        pantheios::logputs(sev, message);
# else /* ? PANTHEIOS_VER */
        pantheios::puts(sev, message);
# endif /* PANTHEIOS_VER */

        return 1;

#else /* ? XTESTS_USE_PANTHEIOS */

        FILE*   stm = (sev < xtestsNotice) ? stderr : stdout;

        /* NOTE: the empty string is required to forestall GCC 4.3+'s over-eager
         * warnings
         */
        return ::fprintf(stm, message, "");

#endif /* XTESTS_USE_PANTHEIOS */
    }

    size_t xtests_strnlen_(
        char const* s
    ,   size_t      limit
    )
    {
        size_t n = 0;

        for(; 0 != limit && '\0' != *s; --limit, ++s)
        {
            ++n;
        }

        return n;
    }

    size_t xtests_wcsnlen_(
        wchar_t const*  s
    ,   size_t          limit
    )
    {
        size_t n = 0;

        for(; 0 != limit && L'\0' != *s; --limit, ++s)
        {
            ++n;
        }

        return n;
    }

    void xtests_strncpy_(char* d, size_t n, char const* s, size_t l)
    {
        if(l < n)
        {
            ::memcpy(d, s, sizeof(char) * l);
            d[l] = '\0';
        }
        else
        {
            ::memcpy(d, s, sizeof(char) * n);
        }
    }
    void xtests_wcsncpy_(wchar_t* d, size_t n, wchar_t const* s, size_t l)
    {
        if(l < n)
        {
            ::memcpy(d, s, sizeof(wchar_t) * l);
            d[l] = '\0';
        }
        else
        {
            ::memcpy(d, s, sizeof(wchar_t) * n);
        }
    }

    static size_t xtests_strnlen_a_(char const* s, size_t limit)
    {
        size_t  n = 0;

        for(; '\0' != *s && n != limit; ++n, ++s)
        {}

        return n;
    }

    static size_t xtests_strnlen_w_(wchar_t const* s, size_t limit)
    {
        size_t  n = 0;

        for(; '\0' != *s && n != limit; ++n, ++s)
        {}

        return n;
    }

    static int xtests_strcmp_a_(char const* str1, char const* str2)
    {
        if(str1 == str2)
        {
            return 0;
        }
        else if(NULL == str1)
        {
            STLSOFT_ASSERT(NULL != str2);

            return ('\0' == *str2) ? 0 : -1;
        }
        else if(NULL == str2)
        {
            STLSOFT_ASSERT(NULL != str1);

            return ('\0' == *str1) ? 0 : +1;
        }
        else
        {
            return ::strcmp(str1, str2);
        }
    }

    static int xtests_strcmp_w_(wchar_t const* str1, wchar_t const* str2)
    {
        if(str1 == str2)
        {
            return 0;
        }
        else if(NULL == str1)
        {
            STLSOFT_ASSERT(NULL != str2);

            return ('\0' == *str2) ? 0 : -1;
        }
        else if(NULL == str2)
        {
            STLSOFT_ASSERT(NULL != str1);

            return ('\0' == *str1) ? 0 : +1;
        }
        else
        {
            return ::wcscmp(str1, str2);
        }
    }

    static int xtests_stricmp_a_(char const* str1, char const* str2)
    {
        if(str1 == str2)
        {
            return 0;
        }
        else if(NULL == str1)
        {
            STLSOFT_ASSERT(NULL != str2);

            return -1;
        }
        else if(NULL == str2)
        {
            STLSOFT_ASSERT(NULL != str1);

            return +1;
        }
        else
        {
            const size_t    len1    =   ::strlen(str1);
            const size_t    len2    =   ::strlen(str2);
            char_buffer_t_  str1_(1 + len1);
            char_buffer_t_  str2_(1 + len2);

#ifndef STLSOFT_CF_EXCEPTION_SUPPORT
            if( str1_.empty() ||
                str2_.empty())
            {
                return 1;
            }
#endif /* !STLSOFT_CF_EXCEPTION_SUPPORT */

            xtests_strcpy_(&str1_[0], str1_.size(), str1);
            xtests_strcpy_(&str2_[0], str2_.size(), str2);

            ::stlsoft::make_upper(str1_);
            ::stlsoft::make_upper(str2_);

            return ::strcmp(str1_.data(), str2_.data());
        }
    }

    static int xtests_stricmp_w_(wchar_t const* str1, wchar_t const* str2)
    {
        if(str1 == str2)
        {
            return 0;
        }
        else if(NULL == str1)
        {
            STLSOFT_ASSERT(NULL != str2);

            return ('\0' == *str2) ? 0 : -1;
        }
        else if(NULL == str2)
        {
            STLSOFT_ASSERT(NULL != str1);

            return ('\0' == *str1) ? 0 : +1;
        }
        else
        {
            const size_t        len1    =   ::wcslen(str1);
            const size_t        len2    =   ::wcslen(str2);
            char_buffer_w_t_    str1_(1 + len1);
            char_buffer_w_t_    str2_(1 + len2);

#ifndef STLSOFT_CF_EXCEPTION_SUPPORT
            if( str1_.empty() ||
                str2_.empty())
            {
                return 1;
            }
#endif /* !STLSOFT_CF_EXCEPTION_SUPPORT */

            xtests_wcscpy_(&str1_[0], str1_.size(), str1);
            xtests_wcscpy_(&str2_[0], str2_.size(), str2);

            ::stlsoft::make_upper(str1_);
            ::stlsoft::make_upper(str2_);

            return ::wcscmp(str1_.data(), str2_.data());
        }
    }

    static int xtests_strncmp_a_(char const* str1, char const* str2, size_t n)
    {
        if(str1 == str2)
        {
            return 0;
        }
        else if(0 == n)
        {
            return 0;
        }
        else if(NULL == str1)
        {
            STLSOFT_ASSERT(NULL != str2);

            return ('\0' == *str2) ? 0 : -1;
        }
        else if(NULL == str2)
        {
            STLSOFT_ASSERT(NULL != str1);

            return ('\0' == *str1) ? 0 : +1;
        }
        else
        {
            return ::strncmp(str1, str2, n);
        }
    }

    static int xtests_strncmp_w_(wchar_t const* str1, wchar_t const* str2, size_t n)
    {
        if(str1 == str2)
        {
            return 0;
        }
        else if(0 == n)
        {
            return 0;
        }
        else if(NULL == str1)
        {
            STLSOFT_ASSERT(NULL != str2);

            return ('\0' == *str2) ? 0 : -1;
        }
        else if(NULL == str2)
        {
            STLSOFT_ASSERT(NULL != str1);

            return ('\0' == *str1) ? 0 : +1;
        }
        else
        {
            return ::wcsncmp(str1, str2, n);
        }
    }

    static int xtests_strnicmp_a_(char const* str1, char const* str2, size_t n)
    {
        if(str1 == str2)
        {
            return 0;
        }
        else if(0 == n)
        {
            return 0;
        }
        else if(NULL == str1)
        {
            return (NULL == str2 || '\0' == *str2) ? 0 : -1;
        }
        else if(NULL == str2)
        {
            STLSOFT_ASSERT(NULL != str1);

            return ('\0' == *str1) ? 0 : +1;
        }
        else
        {
            const size_t    len1    =   xtests_strnlen_a_(str1, n);
            const size_t    len2    =   xtests_strnlen_a_(str2, n);
            char_buffer_t_  str1_(len1);
            char_buffer_t_  str2_(len2);

#ifndef STLSOFT_CF_EXCEPTION_SUPPORT
            if( str1_.empty() ||
                str2_.empty())
            {
                return 1;
            }
#endif /* !STLSOFT_CF_EXCEPTION_SUPPORT */

            xtests_strncpy_(&str1_[0], str1_.size(), str1, len1);
            xtests_strncpy_(&str2_[0], str2_.size(), str2, len2);

            ::stlsoft::make_upper(str1_);
            ::stlsoft::make_upper(str2_);

            return ::strncmp(str1_.data(), str2_.data(), n);
        }
    }

    static int xtests_strnicmp_w_(wchar_t const* str1, wchar_t const* str2, size_t n)
    {
        if(str1 == str2)
        {
            return 0;
        }
        else if(0 == n)
        {
            return 0;
        }
        else if(NULL == str1)
        {
            return (NULL == str2 || '\0' == *str2) ? 0 : -1;
        }
        else if(NULL == str2)
        {
            STLSOFT_ASSERT(NULL != str1);

            return ('\0' == *str1) ? 0 : +1;
        }
        else
        {
            const size_t        len1    =   xtests_strnlen_w_(str1, n);
            const size_t        len2    =   xtests_strnlen_w_(str2, n);
            char_buffer_w_t_    str1_(len1);
            char_buffer_w_t_    str2_(len2);

#ifndef STLSOFT_CF_EXCEPTION_SUPPORT
            if( str1_.empty() ||
                str2_.empty())
            {
                return 1;
            }
#endif /* !STLSOFT_CF_EXCEPTION_SUPPORT */

            xtests_wcsncpy_(&str1_[0], str1_.size(), str1, len1);
            xtests_wcsncpy_(&str2_[0], str2_.size(), str2, len2);

            ::stlsoft::make_upper(str1_);
            ::stlsoft::make_upper(str2_);

            return ::wcsncmp(str1_.data(), str2_.data(), n);
        }
    }

    static char const* xtests_strstr_(char const* str1, char const* str2)
    {
        if(NULL == str1)
        {
            return NULL;
        }
        else if(NULL == str2)
        {
            return str1;
        }
        else
        {
            return ::strstr(str1, str2);
        }
    }

    static wchar_t const* xtests_strstr_w_(wchar_t const* str1, wchar_t const* str2)
    {
        if(NULL == str1)
        {
            return NULL;
        }
        else if(NULL == str2)
        {
            return str1;
        }
        else
        {
            return ::wcsstr(str1, str2);
        }
    }

    static char const* xtests_stristr_(char const* str1, char const* str2)
    {
        if(NULL == str1)
        {
            return NULL;
        }
        else if(NULL == str2)
        {
            return str1;
        }
        else
        {
            const size_t    len1    =   ::strlen(str1);
            const size_t    len2    =   ::strlen(str2);
            char_buffer_t_  str1_(1 + len1);
            char_buffer_t_  str2_(1 + len2);

#ifndef STLSOFT_CF_EXCEPTION_SUPPORT
            if( str1_.empty() ||
                str2_.empty())
            {
                return NULL;
            }
#endif /* !STLSOFT_CF_EXCEPTION_SUPPORT */

            xtests_strcpy_(&str1_[0], str1_.size(), str1);
            xtests_strcpy_(&str2_[0], str2_.size(), str2);

            ::stlsoft::make_upper(str1_);
            ::stlsoft::make_upper(str2_);

            return ::strstr(str1_.data(), str2_.data());
        }
    }

    static wchar_t const* xtests_stristr_w_(wchar_t const* str1, wchar_t const* str2)
    {
        if(NULL == str1)
        {
            return NULL;
        }
        else if(NULL == str2)
        {
            return str1;
        }
        else
        {
            const size_t        len1    =   ::wcslen(str1);
            const size_t        len2    =   ::wcslen(str2);
            char_buffer_w_t_    str1_(1 + len1);
            char_buffer_w_t_    str2_(1 + len2);

#ifndef STLSOFT_CF_EXCEPTION_SUPPORT
            if( str1_.empty() ||
                str2_.empty())
            {
                return NULL;
            }
#endif /* !STLSOFT_CF_EXCEPTION_SUPPORT */

            xtests_wcscpy_(&str1_[0], str1_.size(), str1);
            xtests_wcscpy_(&str2_[0], str2_.size(), str2);

            ::stlsoft::make_upper(str1_);
            ::stlsoft::make_upper(str2_);

            return ::wcsstr(str1_.data(), str2_.data());
        }
    }

    void adapt_fputs(char const* s, size_t /* n */, void* param)
    {
        stlsoft_static_cast(void, ::fputs(s, stlsoft_static_cast(FILE*, param)));
    }
#if XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_
    void adapt_OutputDebugStringA(char const* s, size_t /* n */, void* /* param */)
    {
        OutputDebugStringA(s);
    }
#endif /* XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_ */

    typedef void (*sink_pfn_t_)(char const* s, size_t n, void* param);

    struct xtests_sink_t_
    {
        sink_pfn_t_ pfn;
        void*       param;
    };

    int xtests_mxnprintf_(xtests_sink_t_ const* sinks, size_t numSinks, size_t requiredLen, char const* fmt, ...)
    {
        if(requiredLen < 100)
        {
            requiredLen = 100;
        }
        requiredLen += strlen(fmt);

        int                         r = -1;
        stlsoft::auto_buffer<char>  buff(1);

        { for(unsigned i = 0; i != 10; ++i)
        {
            if(!buff.resize(1 + 1 + requiredLen)) // adds two to allow x and n to be different (required by VC++ "safe" fns)
            {
                { for(size_t i = 0; i != numSinks; ++i)
                {
                    xtests_sink_t_ const&   sink = sinks[i];
                    static const char       oom[] = "out of memory\n";

                    sink.pfn(oom, STLSOFT_NUM_ELEMENTS(oom) - 1, sink.param);
                }}

                ::exit(EXIT_FAILURE);
            }

            va_list args;

            va_start(args, fmt);

            r = xtests_vsnprintf_(&buff[0], buff.size(), buff.size() - 1, fmt, args);

            va_end(args);

            if(r < 0)
            {
                requiredLen *= 1 + (1 + i); // adds 2 to avoid pointless first same-size repeat
            }
            else if(r < int(buff.size() - 1))
            {
                break;
            }
            else
            {
                requiredLen = size_t(r);
            }
        }}

        if(r >= 0)
        {
            size_t n = static_cast<size_t>(r);

            buff[n] = '\0';

            { for(size_t i = 0; i != numSinks; ++i)
            {
                xtests_sink_t_ const& sink = sinks[i];

                sink.pfn(&buff[0], n, sink.param);
            }}
        }

        return r;
    }


#ifdef STLSOFT_CF_NAMESPACE_SUPPORT
} // anonymous namespace
#endif /* STLSOFT_CF_NAMESPACE_SUPPORT */

/* /////////////////////////////////////////////////////////////////////////
 * API Implementation
 */

XTESTS_CALL(int) xtests_startRunner(
    char const*         name
,   int                 verbosity
,   xTests_Reporter_t*  reporter
,   void*               reporterParam
,   FILE*               stm
,   int                 flags
,   xTests_Setup_t      setup
,   xTests_Teardown_t   teardown
,   void*               setupParam
)
{
    STLSOFT_MESSAGE_ASSERT("Runner already initialised in this process!", NULL == s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        s_runner = new RunnerInfo(name, verbosity, reporter, reporterParam, stm, flags, setup, teardown, setupParam);

        if(NULL == s_runner)
        {
            return -2;
        }
        else
        {
            s_runner->PrintStart();

            return 0;
        }

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc&)
    {
        return -2;
    }
    catch(std::exception&)
    {
        return -1;
    }
    catch(...)
    {
        return -3;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_endRunner(int *retCode)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

    /* Bit of Null Object Pattern (Variable Variant) to assist */

    int retCode_ = EXIT_SUCCESS;

    if(NULL == retCode)
    {
        retCode = &retCode_;
    }

    s_runner->PrintEnd();

    if( 0 != s_runner->NumberOfFailedTestCases() &&
        EXIT_SUCCESS == *retCode)
    {
        *retCode = EXIT_FAILURE;
    }

    delete s_runner;

    s_runner = NULL;

    return *retCode;
}

XTESTS_CALL(void) xtests_printRunnerResults()
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

    s_runner->PrintResults();
}

XTESTS_CALL(void) xtests_abend(char const* message)
{
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        if(NULL != s_runner)
        {
            s_runner->onAbend(message);

            delete s_runner;

            s_runner = NULL;
        }

        xtests_output_(xtestsAlert, message);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(...)
    {}
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

    ::exit(EXIT_FAILURE);
}

XTESTS_CALL(int) xtests_beginTestCase(
    char const* name
,   char const* description
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->BeginCase(name, description);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot create test due to memory exhaustion\n");

        return 1;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "Cannot create test '";
        msg += name;
        msg += "': ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 1;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_endTestCase(char const* name)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->EndCase(name);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot end test due to memory exhaustion\n");

        return 1;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "Cannot end test '";
        msg += name;
        msg += "': ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 1;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_testPassed(
    char const* file
,   int         line
,   char const* function
,   char const* expr
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        s_runner->RegisterSuccessfulCondition(file, line, function, expr);

        return 1;

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(void) xtests_caseExcepted(
    char const* exceptionType
,   char const* exceptionMessage
)
{
    STLSOFT_ASSERT(NULL != exceptionType);
    STLSOFT_ASSERT(NULL != exceptionMessage);

    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

    // Remove leading "class "
    if(exceptionType == ::strstr(exceptionType, "class "))
    {
        exceptionType += 6;
    }

    s_runner->CaseExcepted(exceptionType, exceptionMessage);
}

XTESTS_CALL(void) xtests_caseExceptionExpected(char const* exceptionType)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

    s_runner->CaseExceptionExpected(exceptionType);
}

XTESTS_CALL(int) xtests_testFailed(
    char const* file
,   int         line
,   char const* function
,   char const* expr
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->RegisterFailedCondition(file, line, function, expr);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_floatingPointClose(
    double  expected
,   double  actual
)
{
    if(expected == actual)
    {
        return 1;
    }
    else if(0.0 == expected)
    {
        return 0;
    }
    else
    {
        double result = actual / expected;

        if( result > (2.0 - s_fpApproximateFactor) &&
            result < s_fpApproximateFactor)
        {
            return true;
        }
    }

    return 0;
}

XTESTS_CALL(double) xtests_setFloatingPointCloseFactor(
    double  factor
,   double* old /* = NULL */
)
{
    STLSOFT_ASSERT(0.0 != factor);
    STLSOFT_ASSERT(factor >= 1.0 && factor < 2.0);

    double  old_ = s_fpApproximateFactor;

    s_fpApproximateFactor = factor;

    if(NULL != old)
    {
        *old = old_;
    }

    return old_;
}

XTESTS_CALL(int) xtests_testFailed_int(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   int                 expected
,   int                 actual
,   xtests_comparison_t comp
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->RegisterFailedCondition_long(file, line, function, expr, static_cast<long>(expected), static_cast<long>(actual), comp);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_testFailed_long(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   long                expected
,   long                actual
,   xtests_comparison_t comp
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->RegisterFailedCondition_long(file, line, function, expr, expected, actual, comp);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_testFailed_ulong(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   unsigned long       expected
,   unsigned long       actual
,   xtests_comparison_t comp
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->RegisterFailedCondition_ulong(file, line, function, expr, expected, actual, comp);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
XTESTS_CALL(int) xtests_testFailed_longlong(
    char const*                     file
,   int                             line
,   char const*                     function
,   char const*                     expr
,   stlsoft_ns_qual(ss_sint64_t)    expected
,   stlsoft_ns_qual(ss_sint64_t)    actual
,   xtests_comparison_t             comp
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

# ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
# endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->RegisterFailedCondition_sint64(file, line, function, expr, expected, actual, comp);

# ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
# endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_testFailed_ulonglong(
    char const*                    file
,   int                             line
,   char const*                     function
,   char const*                     expr
,   stlsoft_ns_qual(ss_uint64_t)    expected
,   stlsoft_ns_qual(ss_uint64_t)    actual
,   xtests_comparison_t             comp
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

# ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
# endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->RegisterFailedCondition_uint64(file, line, function, expr, expected, actual, comp);

# ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
# endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */

XTESTS_CALL(int) xtests_testFailed_boolean(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   int                 expected
,   int                 actual
,   xtests_comparison_t comp
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->RegisterFailedCondition_boolean(file, line, function, expr, 0 != expected, 0 != actual, comp);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_testFailed_double(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   double              expected
,   double              actual
,   xtests_comparison_t comp
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->RegisterFailedCondition_double(file, line, function, expr, expected, actual, comp);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_testMultibyteStrings(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   char const*         expected
,   char const*         actual
,   xtests_comparison_t comp
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->TestMultibyteStrings(file, line, function, expr, expected, actual, comp);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_testMultibyteStringsN_(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   char const*         expected
,   char const*         actual
,   ptrdiff_t           n /* exact if +ve; limit if -ve */
,   size_t              cchExpected
,   size_t              cchActual
,   xtests_comparison_t comp
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->TestMultibyteStringsN(file, line, function, expr, expected, actual, n, cchExpected, cchActual, comp);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_testMultibyteStringsN(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   char const*         expected
,   char const*         actual
,   ptrdiff_t           n /* exact if +ve; limit if -ve */
,   xtests_comparison_t comp
)
{
    return xtests_testMultibyteStringsN_(
        file
    ,   line
    ,   function
    ,   expr
    ,   expected
    ,   actual
    ,   n
    ,   (0 != n && NULL != expected) ? ::strlen(expected) : 0u
    ,   (0 != n && NULL != actual) ? xtests_strnlen_(actual, static_cast<size_t>(::abs(static_cast<int>(n)))) : 0u
    ,   comp);
}

XTESTS_CALL(int) xtests_testWideStrings(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   wchar_t const*      expected
,   wchar_t const*      actual
,   xtests_comparison_t comp
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->TestWideStrings(file, line, function, expr, expected, actual, comp);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_testWideStringsN_(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   wchar_t const*      expected
,   wchar_t const*      actual
,   int                 n /* exact if +ve; limit if -ve */
,   size_t              cchExpected
,   size_t              cchActual
,   xtests_comparison_t comp
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->TestWideStringsN(file, line, function, expr, expected, actual, n, cchExpected, cchActual, comp);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_testWideStringsN(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   wchar_t const*      expected
,   wchar_t const*      actual
,   int                 n /* exact if +ve; limit if -ve */
,   xtests_comparison_t comp
)
{
    return xtests_testWideStringsN_(
        file
    ,   line
    ,   function
    ,   expr
    ,   expected
    ,   actual
    ,   n
    ,   (0 != n && NULL != expected) ? ::wcslen(expected) : 0u
    ,   (0 != n && NULL != actual) ? xtests_wcsnlen_(actual, static_cast<size_t>(::abs(n))) : 0u
    ,   comp);
}

XTESTS_CALL(int) xtests_testMultibyteStringContains(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   char const*         expected
,   char const*         actual
,   xtests_comparison_t comp
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->TestMultibyteStringContains(file, line, function, expr, expected, actual, comp);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_testWideStringContains(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   wchar_t const*      expected
,   wchar_t const*      actual
,   xtests_comparison_t comp
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->TestWideStringContains(file, line, function, expr, expected, actual, comp);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_testPointers(
    char const*             file
,   int                     line
,   char const*             function
,   char const*             expr
,   void volatile const*    expected
,   void volatile const*    actual
,   xtests_comparison_t     comp
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->TestPointers(file, line, function, expr, expected, actual, comp);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_testFunctionPointers(
    char const*             file
,   int                     line
,   char const*             function
,   char const*             expr
,   void                    (*expected)(void)
,   void                    (*actual)(void)
,   xtests_comparison_t     comp
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->TestFunctionPointers(file, line, function, expr, expected, actual, comp);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_testCharactersA(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   char                expected
,   char                actual
,   xtests_comparison_t comp
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->TestCharacters(file, line, function, expr, expected, actual, comp);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_testCharactersW(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   wchar_t             expected
,   wchar_t             actual
,   xtests_comparison_t comp
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->TestCharacters(file, line, function, expr, expected, actual, comp);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xtests_writeFailMessage(
    char const* file
,   int         line
,   char const* function
,   char const* message
,   char const* qualifyingInformation
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->WriteFailMessage(file, line, function, message, qualifyingInformation);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return -2;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}


XTESTS_CALL(int) xtests_require_C(
    int success
)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        if(!success)
        {
            s_runner->OnRequireFailed();
        }

        return success;

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return 0;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 0;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

XTESTS_CALL(int) xTests_hasRequiredConditionFailed(void)
{
    STLSOFT_MESSAGE_ASSERT("runner not initialised in this process!", NULL != s_runner);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return s_runner->HasRequiredConditionFailed();

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        xtests_output_(xtestsCritical, "cannot update test due to memory exhaustion\n");

        return 1;
    }
    catch(std::exception& x)
    {
        std::string msg;

        msg.reserve(200);

        msg += "cannot update test: ";
        msg += x.what();
        msg += "\n";

        xtests_output_(xtestsCritical, msg.c_str());

        return 1;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

/* /////////////////////////////////////////////////////////////////////////
 * Helper functions
 */

XTESTS_CALL(int) xtests_commandLine_parseVerbosity(
    int     argc
,   char**  argv
,   int*    verbosity
)
{
    STLSOFT_ASSERT(NULL != verbosity);

    *verbosity = XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR;

    static const char   s_verb[]    =   "--verbosity=";
    static const size_t s_cchVerb   =   STLSOFT_NUM_ELEMENTS(s_verb) - 1;

    { for(int i = 1; i < argc; ++i)
    {
        STLSOFT_ASSERT(NULL != argv[i]);

        if(argv[i] == ::strstr(argv[i], s_verb))
        {
            *verbosity = ::atoi(argv[i] + s_cchVerb);

            return i;
        }
    }}

    return 0;
}

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

xtests_variable_value_t::xtests_variable_value_t(bool b)
    : booleanValue(b)
{}

xtests_variable_value_t::xtests_variable_value_t(int i)
    : intValue(i)
{}

xtests_variable_value_t::xtests_variable_value_t(long i)
    : longValue(i)
{}

xtests_variable_value_t::xtests_variable_value_t(unsigned long i)
    : ulongValue(i)
{}

xtests_variable_value_t::xtests_variable_value_t(sint64_t const& i)
    : longlongValue(i)
{}

xtests_variable_value_t::xtests_variable_value_t(uint64_t const& i)
    : ulonglongValue(i)
{}

xtests_variable_value_t::xtests_variable_value_t(char ch)
    : multibyteCharacterValue(ch)
{}

xtests_variable_value_t::xtests_variable_value_t(wchar_t ch, xtests_variable_type_t)
    : wideCharacterValue(ch)
{}

xtests_variable_value_t::xtests_variable_value_t(double const& d)
    : doubleValue(d)
{}

xtests_variable_value_t::xtests_variable_value_t(void const volatile* pv)
    : opaquePointerValue(pv)
{}

xtests_variable_value_t::xtests_variable_value_t(char const* s)
    : multibyteStringValue(s)
{}

xtests_variable_value_t::xtests_variable_value_t(wchar_t const* s)
    : wideStringValue(s)
{}


xtests_variable_t::xtests_variable_t(bool b)
    : variableType(xtestsVariableBoolean)
    , testType(xtestsTestFullComparison)
    , value(b)
    , valueLen(0u)
{}

xtests_variable_t::xtests_variable_t(char ch)
    : variableType(xtestsVariableMultibyteCharacter)
    , testType(xtestsTestFullComparison)
    , value(ch)
    , valueLen(0u)
{}

xtests_variable_t::xtests_variable_t(wchar_t ch, xtests_variable_type_t /* type */)
    : variableType(xtestsVariableWideCharacter)
    , testType(xtestsTestFullComparison)
    , value(ch, xtestsVariableWideCharacter)
    , valueLen(0u)
{}

xtests_variable_t::xtests_variable_t(long i)
    : variableType(xtestsVariableLong)
    , testType(xtestsTestFullComparison)
    , value(i)
    , valueLen(0u)
{}

xtests_variable_t::xtests_variable_t(unsigned long i)
    : variableType(xtestsVariableUnsignedLong)
    , testType(xtestsTestFullComparison)
    , value(i)
    , valueLen(0u)
{}

xtests_variable_t::xtests_variable_t(sint64_t const& i)
    : variableType(xtestsVariableLongLong)
    , testType(xtestsTestFullComparison)
    , value(i)
    , valueLen(0u)
{}

xtests_variable_t::xtests_variable_t(uint64_t const& i)
    : variableType(xtestsVariableUnsignedLongLong)
    , testType(xtestsTestFullComparison)
    , value(i)
    , valueLen(0u)
{}

xtests_variable_t::xtests_variable_t(double const& d)
    : variableType(xtestsVariableDouble)
    , testType(xtestsTestFullComparison)
    , value(d)
    , valueLen(0u)
{}

xtests_variable_t::xtests_variable_t(void const volatile* pv)
    : variableType(xtestsVariableOpaquePointer)
    , testType(xtestsTestFullComparison)
    , value(pv)
    , valueLen(0u)
{}

xtests_variable_t::xtests_variable_t(char const* s, xtests_test_type_t testType)
    : variableType(xtestsVariableMultibyteString)
    , testType(testType)
    , value(s)
    , valueLen(::stlsoft::c_str_len_a(s))
{}

xtests_variable_t::xtests_variable_t(wchar_t const* s, xtests_test_type_t testType)
    : variableType(xtestsVariableWideString)
    , testType(testType)
    , value(s)
    , valueLen(::stlsoft::c_str_len_w(s))
{}

xtests_variable_t::xtests_variable_t(char const* s, size_t n, xtests_test_type_t testType)
    : variableType(xtestsVariableMultibyteString)
    , testType(testType)
    , value(s)
    , valueLen(n)
{}

xtests_variable_t::xtests_variable_t(wchar_t const* s, size_t n, xtests_test_type_t testType)
    : variableType(xtestsVariableWideString)
    , testType(testType)
    , value(s)
    , valueLen(n)
{}

#ifdef STLSOFT_CF_NAMESPACE_SUPPORT
namespace
{
#endif /* STLSOFT_CF_NAMESPACE_SUPPORT */

/* static */ xTests_Reporter_t* RunnerInfo::get_reporter_(xTests_Reporter_t* reporter, FILE* stm, int flags)
{
#ifdef STLSOFT_COMPILER_IS_BORLAND
# define XTESTS_VERBOSITY_SILENT                        ::xtests::c::XTESTS_VERBOSITY_SILENT
# define XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR       ::xtests::c::XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR
# define XTESTS_VERBOSITY_RUNNER_SUMMARY                ::xtests::c::XTESTS_VERBOSITY_RUNNER_SUMMARY
# define XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR   ::xtests::c::XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR
# define XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR         ::xtests::c::XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR
# define XTESTS_VERBOSITY_CASE_SUMMARY                  ::xtests::c::XTESTS_VERBOSITY_CASE_SUMMARY
# define XTESTS_VERBOSITY_VERBOSE                       ::xtests::c::XTESTS_VERBOSITY_VERBOSE
# define xtestsRunnerFlagsNoWindowsDebugString          ::xtests::c::xtestsRunnerFlagsNoWindowsDebugString
# define xtests_variable_type_t                         ::xtests::c::xtests_variable_type_t
# define xtests_test_type_t                             ::xtests::c::xtests_test_type_t

# define xtestsComparisonEqual                          ::xtests::c::xtestsComparisonEqual
# define xtestsComparisonNotEqual                       ::xtests::c::xtestsComparisonNotEqual
# define xtestsComparisonGreaterThan                    ::xtests::c::xtestsComparisonGreaterThan
# define xtestsComparisonLessThan                       ::xtests::c::xtestsComparisonLessThan
# define xtestsComparisonGreaterThanOrEqual             ::xtests::c::xtestsComparisonGreaterThanOrEqual
# define xtestsComparisonLessThanOrEqual                ::xtests::c::xtestsComparisonLessThanOrEqual
# define xtestsComparisonApproxEqual                    ::xtests::c::xtestsComparisonApproxEqual
# define xtestsComparisonApproxNotEqual                 ::xtests::c::xtestsComparisonApproxNotEqual
# define xtestsComparison_max_enumerator                ::xtests::c::xtestsComparison_max_enumerator

# define xtestsTestFullComparison                       ::xtests::c::xtestsTestFullComparison
# define xtestsTestPartialComparison                    ::xtests::c::xtestsTestPartialComparison
# define xtestsTestContainment                          ::xtests::c::xtestsTestContainment

# define xtestsVariableNone                             ::xtests::c::xtestsVariableNone
# define xtestsVariableNone                             ::xtests::c::xtestsVariableNone
# define xtestsVariableBoolean                          ::xtests::c::xtestsVariableBoolean
# define xtestsVariableOpaquePointer                    ::xtests::c::xtestsVariableOpaquePointer
# define xtestsVariableMultibyteCharacter               ::xtests::c::xtestsVariableMultibyteCharacter
# define xtestsVariableWideCharacter                    ::xtests::c::xtestsVariableWideCharacter
# define xtestsVariableMultibyteString                  ::xtests::c::xtestsVariableMultibyteString
# define xtestsVariableWideString                       ::xtests::c::xtestsVariableWideString
# define xtestsVariableSignedChar                       ::xtests::c::xtestsVariableSignedChar
# define xtestsVariableUnsignedChar                     ::xtests::c::xtestsVariableUnsignedChar
# define xtestsVariableShort                            ::xtests::c::xtestsVariableShort
# define xtestsVariableUnsignedShort                    ::xtests::c::xtestsVariableUnsignedShort
# define xtestsVariableInt                              ::xtests::c::xtestsVariableInt
# define xtestsVariableUnsignedInt                      ::xtests::c::xtestsVariableUnsignedInt
# define xtestsVariableLong                             ::xtests::c::xtestsVariableLong
# define xtestsVariableUnsignedLong                     ::xtests::c::xtestsVariableUnsignedLong
# ifdef STLSOFT_CF_64BIT_INT_SUPPORT
#  define xtestsVariableLongLong                        ::xtests::c::xtestsVariableLongLong
#  define xtestsVariableUnsignedLongLong                ::xtests::c::xtestsVariableUnsignedLongLong
# endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
# define xtestsVariableDouble                           ::xtests::c::xtestsVariableDouble

# define adapt_fputs                                    ::xtests::c::adapt_fputs
# define adapt_OutputDebugStringA                       ::xtests::c::adapt_OutputDebugStringA
#endif /* STLSOFT_COMPILER_IS_BORLAND */

    if(NULL == reporter)
    {
        if(NULL == stm)
        {
            stm = stdout;
        }

        // NOTE: Even though this is thread-unsafe in that there's a race
        // condition for the initialisation of the s_reporter instance (and
        // its vtable), there is no problem because multiple initialisation
        // will be benign.

        class fprintf_reporter
            : public xTests_Reporter_t
        {
        public: // Construction
            explicit fprintf_reporter(FILE* stm, int flags)
                : m_flags(flags)
#if XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_
                , m_numSinks((xtestsRunnerFlagsNoWindowsDebugString & flags) ? 1u : 2u)
#else /* ? XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_ */
                , m_numSinks(1)
#endif /* XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_ */
            {
                m_sinks[0].pfn      =   adapt_fputs;
                m_sinks[0].param    =   stm;

#if XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_
                if(0 == (xtestsRunnerFlagsNoWindowsDebugString & flags))
                {
                    m_sinks[1].pfn      =   adapt_OutputDebugStringA;
                    m_sinks[1].param    =   NULL;
                }
#endif /* XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_ */
            }

        private: // Overrides
            virtual void onStartRunner(void* /* reporterParam */, char const* name, int verbosity)
            {
                switch(verbosity)
                {
                    case    XTESTS_VERBOSITY_SILENT:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                    case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                        break;
                    default:
                        STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                    case    XTESTS_VERBOSITY_CASE_SUMMARY:
                    XTESTS_VERBOSITY_VALID_MISSING_CASES
                    case    XTESTS_VERBOSITY_VERBOSE:
                        xtests_mxnprintf_(  m_sinks, m_numSinks, stlsoft::c_str_len(name)
                                        ,   "Test runner '%s' starting:\n", name);
                        break;
                }
            }

            virtual void onBeginTestCase(void* /* reporterParam */, char const* /* name */, char const* /* desc */, int /* verbosity */)
            {
            }

            virtual void onTestPassed(void* /* reporterParam */, char const* /* file */, int /* line */, char const* /* function */, char const* /* expr */, xtests_comparison_t /* comparison */, int /* verbosity */)
            {
            }

            virtual void onTestFailed(
                void*                       /* reporterParam */
            ,   char const*                 file
            ,   int                         line
            ,   char const*                 function
            ,   char const*                 expr
            ,   xtests_variable_t const*    expectedValue
            ,   xtests_variable_t const*    actualValue
            ,   ptrdiff_t                   length
            ,   xtests_comparison_t         comparison
            ,   int                         verbosity
            )
            {
                STLSOFT_MESSAGE_ASSERT("values must have same variable type", NULL == expectedValue || NULL == actualValue || actualValue->variableType == expectedValue->variableType);
                STLSOFT_MESSAGE_ASSERT("values must have same test type", NULL == expectedValue || NULL == actualValue || actualValue->testType == expectedValue->testType);

                xtests_variable_type_t variableType = xtestsVariableNone;

                if(NULL != expectedValue)
                {
                    variableType = expectedValue->variableType;
                }

                xtests_test_type_t testType = xtestsTestFullComparison;

                if(NULL != expectedValue)
                {
                    testType = expectedValue->testType;
                }

                switch(variableType)
                {
                    case    xtestsVariableBoolean:
                        onTestFailed_Boolean_(file, line, function, expr, static_cast<bool>(0 != expectedValue->value.booleanValue), static_cast<bool>(0 != actualValue->value.booleanValue), comparison, verbosity);
                        break;
                    case    xtestsVariableOpaquePointer:
                        onTestFailed_OpaquePointer_(file, line, function, expr, expectedValue->value.opaquePointerValue, actualValue->value.opaquePointerValue, comparison, verbosity);
                        break;
                    case    xtestsVariableMultibyteCharacter:
                        onTestFailed_MultibyteCharacter_(file, line, function, expr, expectedValue->value.multibyteCharacterValue, actualValue->value.multibyteCharacterValue, comparison, verbosity);
                        break;
                    case    xtestsVariableWideCharacter:
                        onTestFailed_WideCharacter_(file, line, function, expr, expectedValue->value.multibyteCharacterValue, actualValue->value.multibyteCharacterValue, comparison, verbosity);
                        break;
                    case    xtestsVariableMultibyteString:
                        onTestFailed_MultibyteString_(file, line, function, expr, expectedValue->value.multibyteStringValue, expectedValue->valueLen, actualValue->value.multibyteStringValue, actualValue->valueLen, length, testType, comparison, verbosity);
                        break;
                    case    xtestsVariableWideString:
                        onTestFailed_WideString_(file, line, function, expr, expectedValue->value.wideStringValue, expectedValue->valueLen, actualValue->value.wideStringValue, actualValue->valueLen, length, testType, comparison, verbosity);
                        break;
                    case    xtestsVariableLong:
                        onTestFailed_SignedLong_(file, line, function, expr, expectedValue->value.longValue, actualValue->value.longValue, comparison, verbosity);
                        break;
                    case    xtestsVariableUnsignedLong:
                        onTestFailed_UnsignedLong_(file, line, function, expr, expectedValue->value.ulongValue, actualValue->value.ulongValue, comparison, verbosity);
                        break;
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
                    case    xtestsVariableLongLong:
                        onTestFailed_sint64_(file, line, function, expr, expectedValue->value.longlongValue, actualValue->value.longlongValue, comparison, verbosity);
                        break;
                    case    xtestsVariableUnsignedLongLong:
                        onTestFailed_uint64_(file, line, function, expr, expectedValue->value.ulonglongValue, actualValue->value.ulonglongValue, comparison, verbosity);
                        break;
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
                    case    xtestsVariableDouble:
                        onTestFailed_Double_(file, line, function, expr, expectedValue->value.doubleValue, actualValue->value.doubleValue, comparison, verbosity);
                        break;
                    default:
                        STLSOFT_MESSAGE_ASSERT("not currently defined for this type", 0);
                    case    xtestsVariableNone:
                        onTestFailed_(file, line, function, expr, comparison, verbosity);
                        break;
                }
            }

            virtual void onTestFailed_Boolean_(char const* file, int line, char const* function, char const* /* expr */, bool expectedValue, bool actualValue, xtests_comparison_t comparison, int verbosity)
            {
                char const* fmt = "%s(%d): test condition failed: actual value %s should %sequal the expected value %s%s%s\n";

                switch(verbosity)
                {
                    case    XTESTS_VERBOSITY_SILENT:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                        fmt = "";
                        break;
                    default:
                        STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                    case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY:
                    XTESTS_VERBOSITY_VALID_MISSING_CASES
                    case    XTESTS_VERBOSITY_VERBOSE:
                        break;
                }

                xtests_mxnprintf_(  m_sinks, m_numSinks, 20
                                ,   fmt
                                ,   file, line, (actualValue ? "true" : "false"), (xtestsComparisonEqual == comparison) ? "" : "not ", (expectedValue ? "true" : "false"), (NULL != function) ? " in function " : "", stlsoft_ns_qual(c_str_ptr)(function));
            }

            virtual void onTestFailed_Double_(char const* file, int line, char const* function, char const* /* expr */, double const& expectedValue, double const& actualValue, xtests_comparison_t comparison, int verbosity)
            {
                static char const*  s_fmts[] =
                {
                        "%s(%d): test condition failed: actual value %G should equal the expected value %G%s%s\n"
                    ,   "%s(%d): test condition failed: actual value %G should not equal expected value %G%s%s\n"
                    ,   "%s(%d): test condition failed: actual value %G should be greater than expected value %G%s%s\n"
                    ,   "%s(%d): test condition failed: actual value %G should be less than expected value %G%s%s\n"
                    ,   "%s(%d): test condition failed: actual value %G should be greater than or equal to the expected value %G%s%s\n"
                    ,   "%s(%d): test condition failed: actual value %G should be less than or equal to the expected value %G%s%s\n"
                    ,   "%s(%d): test condition failed: actual value %G should approximately equal to the expected value %G%s%s\n"
                    ,   "%s(%d): test condition failed: actual value %G should approximately not equal to the expected value %G%s%s\n"
                };
                STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS_(s_fmts) == xtestsComparison_max_enumerator);
                char const*         fmt = s_fmts[comparison];

                switch(verbosity)
                {
                    case    XTESTS_VERBOSITY_SILENT:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                        fmt = "";
                        break;
                    default:
                        STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                    case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY:
                    XTESTS_VERBOSITY_VALID_MISSING_CASES
                    case    XTESTS_VERBOSITY_VERBOSE:
                        break;
                }

                xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                ,   fmt
                                ,   file, line, actualValue, expectedValue, (NULL != function) ? " in function " : "", stlsoft_ns_qual(c_str_ptr)(function));
            }

            virtual void onTestFailed_MultibyteCharacter_(char const* file, int line, char const* function, char const* /* expr */, char expectedValue, char actualValue, xtests_comparison_t comparison, int verbosity)
            {
                static char const*  s_fmts[] =
                {
                        "%s(%d): test condition failed: actual character value '%c' (0x%02x) should equal the expected value '%c' (0x%02x)%s%s\n"
                    ,   "%s(%d): test condition failed: actual character value '%c' (0x%02x) should not equal expected value '%c' (0x%02x)%s%s\n"
                    ,   "%s(%d): test condition failed: actual character value '%c' (0x%02x) should be greater than expected value '%c' (0x%02x)%s%s\n"
                    ,   "%s(%d): test condition failed: actual character value '%c' (0x%02x) should be less than expected value '%c' (0x%02x)%s%s\n"
                    ,   "%s(%d): test condition failed: actual character value '%c' (0x%02x) should be greater than or equal to the expected value '%c' (0x%02x)%s%s\n"
                    ,   "%s(%d): test condition failed: actual character value '%c' (0x%02x) should be less than or equal to the expected value '%c' (0x%02x)%s%s\n"
                    ,   "%s(%d): test condition failed: actual character value '%c' (0x%02x) should be approximately equal to the expected value '%c' (0x%02x)%s%s\n"
                    ,   "%s(%d): test condition failed: actual character value '%c' (0x%04x) should be approximately not equal to the expected value '%c' (0x%04x)%s%s\n"
                };
                STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS_(s_fmts) == xtestsComparison_max_enumerator);
                char const*         fmt = s_fmts[comparison];

                switch(verbosity)
                {
                    case    XTESTS_VERBOSITY_SILENT:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                        fmt = "";
                        break;
                    default:
                        STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                    case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY:
                    XTESTS_VERBOSITY_VALID_MISSING_CASES
                    case    XTESTS_VERBOSITY_VERBOSE:
                        break;
                }

                xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                ,   fmt
                                ,   file, line, actualValue, actualValue, expectedValue, expectedValue, (NULL != function) ? " in function " : "", stlsoft_ns_qual(c_str_ptr)(function));
            }

            virtual void onTestFailed_WideCharacter_(char const* file, int line, char const* function, char const* /* expr */, char expectedValue, char actualValue, xtests_comparison_t comparison, int verbosity)
            {
                static char const*  s_fmts[] =
                {
                        "%s(%d): test condition failed: actual character value '%c' (0x%04x) should equal the expected value '%c' (0x%04x)%s%s\n"
                    ,   "%s(%d): test condition failed: actual character value '%c' (0x%04x) should not equal expected value '%c' (0x%04x)%s%s\n"
                    ,   "%s(%d): test condition failed: actual character value '%c' (0x%04x) should be greater than expected value '%c' (0x%04x)%s%s\n"
                    ,   "%s(%d): test condition failed: actual character value '%c' (0x%04x) should be less than expected value '%c' (0x%04x)%s%s\n"
                    ,   "%s(%d): test condition failed: actual character value '%c' (0x%04x) should be greater than or equal to the expected value '%c' (0x%04x)%s%s\n"
                    ,   "%s(%d): test condition failed: actual character value '%c' (0x%04x) should be less than or equal to the expected value '%c' (0x%04x)%s%s\n"
                    ,   "%s(%d): test condition failed: actual character value '%c' (0x%04x) should be approximately equal to the expected value '%c' (0x%04x)%s%s\n"
                    ,   "%s(%d): test condition failed: actual character value '%c' (0x%04x) should be approximately not equal to the expected value '%c' (0x%04x)%s%s\n"
                };
                STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS_(s_fmts) == xtestsComparison_max_enumerator);
                char const*         fmt = s_fmts[comparison];

                switch(verbosity)
                {
                    case    XTESTS_VERBOSITY_SILENT:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                        fmt = "";
                        break;
                    default:
                        STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                    case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY:
                    XTESTS_VERBOSITY_VALID_MISSING_CASES
                    case    XTESTS_VERBOSITY_VERBOSE:
                        break;
                }

                xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                ,   fmt
                                ,   file, line, static_cast<char>(actualValue), actualValue, static_cast<char>(expectedValue), expectedValue, (NULL != function) ? " in function " : "", stlsoft_ns_qual(c_str_ptr)(function));
            }

            virtual void onTestFailed_MultibyteString_(char const* file, int line, char const* function, char const* /* expr */, char const* expectedValue, size_t expectedValueLen, char const* actualValue, size_t actualValueLen, ptrdiff_t length, xtests_test_type_t testType, xtests_comparison_t comparison, int verbosity)
            {
                if(xtestsTestFullComparison == testType)
                {
                    // Eliminate NULL pointers
                    expectedValue   =   stlsoft::c_str_ptr_a(expectedValue);
                    actualValue     =   stlsoft::c_str_ptr_a(actualValue);

                    static char const*  s_fmts[] =
                    {
                            "%s(%d): test condition failed: actual string value '%s' should equal the expected value '%s'%s%s\n"
                        ,   "%s(%d): test condition failed: actual string value '%s' should not equal expected value '%s'%s%s\n"
                        ,   "%s(%d): test condition failed: actual string value '%s' should be greater than expected value '%s'%s%s\n"
                        ,   "%s(%d): test condition failed: actual string value '%s' should be less than expected value '%s'%s%s\n"
                        ,   "%s(%d): test condition failed: actual string value '%s' should be greater than or equal to the expected value '%s'%s%s\n"
                        ,   "%s(%d): test condition failed: actual string value '%s' should be less than or equal to the expected value '%s'%s%s\n"
                        ,   "%s(%d): test condition failed: actual string value '%s' should approximately equal to the expected value '%s'%s%s\n"
                        ,   "%s(%d): test condition failed: actual string value '%s' should approximately not equal to the expected value '%s'%s%s\n"
                    };
                    STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS_(s_fmts) == xtestsComparison_max_enumerator);
                    char const*         fmt = s_fmts[comparison];

                    switch(verbosity)
                    {
                        case    XTESTS_VERBOSITY_SILENT:
                        case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                        case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                            fmt = "";
                            break;
                        default:
                            STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                        case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                        case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                        case    XTESTS_VERBOSITY_CASE_SUMMARY:
                        XTESTS_VERBOSITY_VALID_MISSING_CASES
                        case    XTESTS_VERBOSITY_VERBOSE:
                            break;
                    }

                xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                ,   fmt
                                ,   file, line, actualValue, expectedValue, (NULL != function) ? " in function " : "", stlsoft_ns_qual(c_str_ptr)(function));
                }
                else if(xtestsTestPartialComparison == testType)
                {
                    // Eliminate NULL pointers
                    expectedValue   =   stlsoft::c_str_data_a(expectedValue);
                    actualValue     =   stlsoft::c_str_data_a(actualValue);

                    static char const*  s_fmts[] =
                    {
                            "%s(%d): test condition failed: actual string value '%.*s' should equal the expected value '%.*s' to the length %d%s%s\n"
                        ,   "%s(%d): test condition failed: actual string value '%.*s' should not equal expected value '%.*s' to the length %d%s%s\n"
                        ,   "%s(%d): test condition failed: actual string value '%.*s' should be greater than expected value '%.*s' to the length %d%s%s\n"
                        ,   "%s(%d): test condition failed: actual string value '%.*s' should be less than expected value '%.*s' to the length %d%s%s\n"
                        ,   "%s(%d): test condition failed: actual string value '%.*s' should be greater than or equal to the expected value '%.*s' to the length %d%s%s\n"
                        ,   "%s(%d): test condition failed: actual string value '%.*s' should be less than or equal to the expected value '%.*s' to the length %d%s%s\n"
                        ,   "%s(%d): test condition failed: actual string value '%.*s' should approximately equal to the expected value '%.*s' to the length %d%s%s\n"
                        ,   "%s(%d): test condition failed: actual string value '%.*s' should approximately not equal to the expected value '%.*s' to the length %d%s%s\n"
                    };
                    STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS_(s_fmts) == xtestsComparison_max_enumerator);
                    char const*         fmt = s_fmts[comparison];

                    switch(verbosity)
                    {
                        case    XTESTS_VERBOSITY_SILENT:
                        case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                        case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                            fmt = "";
                            break;
                        default:
                            STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                        case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                        case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                        case    XTESTS_VERBOSITY_CASE_SUMMARY:
                        XTESTS_VERBOSITY_VALID_MISSING_CASES
                        case    XTESTS_VERBOSITY_VERBOSE:
                            break;
                    }

                    if(length < 0)
                    {
                        length = -length;
                    }

                xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                ,   fmt
                                ,   file, line, int(actualValueLen), actualValue, int(expectedValueLen), expectedValue, length, (NULL != function) ? " in function " : "", stlsoft_ns_qual(c_str_ptr)(function));
                }
                else if(xtestsTestContainment == testType)
                {
                    // Eliminate NULL pointers
                    expectedValue   =   stlsoft::c_str_ptr_a(expectedValue);
                    actualValue     =   stlsoft::c_str_ptr_a(actualValue);

                    static char const*  s_fmts[] =
                    {
                            "%s(%d): test condition failed: actual string value '%s' should contain the expected value '%s'%s%s\n"
                        ,   "%s(%d): test condition failed: actual string value '%s' should not contain expected value '%s'%s%s\n"
                        ,   ""
                        ,   ""
                        ,   ""
                        ,   ""
                        ,   "%s(%d): test condition failed: actual string value '%s' should approximately contain to the expected value '%s'%s%s\n"
                        ,   "%s(%d): test condition failed: actual string value '%s' should approximately not contain to the expected value '%s'%s%s\n"
                    };
                    STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS_(s_fmts) == xtestsComparison_max_enumerator);
                    char const*         fmt = s_fmts[comparison];

                    switch(verbosity)
                    {
                        case    XTESTS_VERBOSITY_SILENT:
                        case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                        case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                            fmt = "";
                            break;
                        default:
                            STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                        case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                        case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                        case    XTESTS_VERBOSITY_CASE_SUMMARY:
                        XTESTS_VERBOSITY_VALID_MISSING_CASES
                        case    XTESTS_VERBOSITY_VERBOSE:
                            break;
                    }

                xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                ,   fmt
                                ,   file, line, actualValue, expectedValue, (NULL != function) ? " in function " : "", stlsoft_ns_qual(c_str_ptr)(function));
                }
                else
                {
                    STLSOFT_MESSAGE_ASSERT("unrecognised test type", 0);
                }
            }

            virtual void onTestFailed_WideString_(char const* file, int line, char const* function, char const* expr, wchar_t const* expectedValue, size_t expectedValueLen, wchar_t const* actualValue, size_t actualValueLen, ptrdiff_t length, xtests_test_type_t testType, xtests_comparison_t comparison, int verbosity)
            {
                stlsoft::w2a    expected(expectedValue, expectedValueLen);
                stlsoft::w2a    actual(actualValue, actualValueLen);

                onTestFailed_MultibyteString_(file, line, function, expr, expected, expected.size(), actual, actual.size(), length, testType, comparison, verbosity);
            }

            virtual void onTestFailed_OpaquePointer_(char const* file, int line, char const* function, char const* /* expr */, void const volatile* expectedValue, void const volatile* actualValue, xtests_comparison_t comparison, int verbosity)
            {
                static char const*  s_fmts_[] =
                {
                        "%s(%d): test condition failed: actual pointer value '%p' should equal the expected value '%p'%s%s\n"
                    ,   "%s(%d): test condition failed: actual pointer value '%p' should not equal expected value '%p'%s%s\n"
                };

                static char const*  s_fmts[] =
                {
                        s_fmts_[0]
                    ,   s_fmts_[1]
                    ,   "%s(%d): test condition failed: actual pointer value '%p' should be greater than expected value '%p'%s%s\n"
                    ,   "%s(%d): test condition failed: actual pointer value '%p' should be less than expected value '%p'%s%s\n"
                    ,   "%s(%d): test condition failed: actual pointer value '%p' should be greater than or equal to the expected value '%p'%s%s\n"
                    ,   "%s(%d): test condition failed: actual pointer value '%p' should be less than or equal to the expected value '%p'%s%s\n"
                    ,   s_fmts_[0]
                    ,   s_fmts_[1]
                };
                STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS_(s_fmts) == xtestsComparison_max_enumerator);
                char const*         fmt = s_fmts[comparison];

                switch(verbosity)
                {
                    case    XTESTS_VERBOSITY_SILENT:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                        fmt = "";
                        break;
                    default:
                        STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                    case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY:
                    XTESTS_VERBOSITY_VALID_MISSING_CASES
                    case    XTESTS_VERBOSITY_VERBOSE:
                        break;
                }

                xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                ,   fmt
                                ,   file, line, actualValue, expectedValue, (NULL != function) ? " in function " : "", stlsoft_ns_qual(c_str_ptr)(function));
            }

            virtual void onTestFailed_SignedLong_(char const* file, int line, char const* function, char const* /* expr */, signed long expectedValue, signed long actualValue, xtests_comparison_t comparison, int verbosity)
            {
                static char const*  s_fmts_[] =
                {
                        "%s(%d): test condition failed: actual value %ld should equal the expected value %ld%s%s\n"
                    ,   "%s(%d): test condition failed: actual value %ld should not equal expected value %ld%s%s\n"
                };

                static char const*  s_fmts[] =
                {
                        s_fmts_[0]
                    ,   s_fmts_[1]
                    ,   "%s(%d): test condition failed: actual value %ld should be greater than expected value %ld%s%s\n"
                    ,   "%s(%d): test condition failed: actual value %ld should be less than expected value %ld%s%s\n"
                    ,   "%s(%d): test condition failed: actual value %ld should be greater than or equal to the expected value %ld%s%s\n"
                    ,   "%s(%d): test condition failed: actual value %ld should be less than or equal to the expected value %ld%s%s\n"
                    ,   s_fmts_[0]
                    ,   s_fmts_[1]
                };
                STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS_(s_fmts) == xtestsComparison_max_enumerator);
                char const*         fmt = s_fmts[comparison];

                switch(verbosity)
                {
                    case    XTESTS_VERBOSITY_SILENT:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                        fmt = "";
                        break;
                    default:
                        STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                    case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY:
                    XTESTS_VERBOSITY_VALID_MISSING_CASES
                    case    XTESTS_VERBOSITY_VERBOSE:
                        break;
                }

                xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                ,   fmt
                                ,   file, line, actualValue, expectedValue, (NULL != function) ? " in function " : "", stlsoft_ns_qual(c_str_ptr)(function));
            }

            virtual void onTestFailed_UnsignedLong_(char const* file, int line, char const* function, char const* /* expr */, unsigned long expectedValue, unsigned long actualValue, xtests_comparison_t comparison, int verbosity)
            {
                static char const*  s_fmts_[] =
                {
                        "%s(%d): test condition failed: actual value %lu should equal the expected value %lu%s%s\n"
                    ,   "%s(%d): test condition failed: actual value %lu should not equal expected value %lu%s%s\n"
                };
                static char const*  s_fmts[] =
                {
                        s_fmts_[0]
                    ,   s_fmts_[1]
                    ,   "%s(%d): test condition failed: actual value %lu should be greater than expected value %lu%s%s\n"
                    ,   "%s(%d): test condition failed: actual value %lu should be less than expected value %lu%s%s\n"
                    ,   "%s(%d): test condition failed: actual value %lu should be greater than or equal to the expected value %lu%s%s\n"
                    ,   "%s(%d): test condition failed: actual value %lu should be less than or equal to the expected value %lu%s%s\n"
                    ,   s_fmts_[0]
                    ,   s_fmts_[1]
                };
                STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS_(s_fmts) == xtestsComparison_max_enumerator);
                char const*         fmt = s_fmts[comparison];

                switch(verbosity)
                {
                    case    XTESTS_VERBOSITY_SILENT:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                        fmt = "";
                        break;
                    default:
                        STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                    case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY:
                    XTESTS_VERBOSITY_VALID_MISSING_CASES
                    case    XTESTS_VERBOSITY_VERBOSE:
                        break;
                }

                xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                ,   fmt
                                ,   file, line, actualValue, expectedValue, (NULL != function) ? " in function " : "", stlsoft_ns_qual(c_str_ptr)(function));
            }

#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
            virtual void onTestFailed_sint64_(char const* file, int line, char const* function, char const* /* expr */, sint64_t expectedValue, sint64_t actualValue, xtests_comparison_t comparison, int verbosity)
            {
# if defined(STLSOFT_COMPILER_IS_BORLAND)
#  define static
# endif /* compiler */

                // Note: The following code has a race condition, but it is entirely benign, so does not matter


                static char const*  s_fmts_[] =
                {
                        "%%s(%%d): test condition failed: actual value %s should equal the expected value %s%%s%%s\n"
                    ,   "%%s(%%d): test condition failed: actual value %s should not equal expected value %s%%s%%s\n"
                };
                static char const*  s_fmtBases[] =
                {
                        s_fmts_[0]
                    ,   s_fmts_[1]
                    ,   "%%s(%%d): test condition failed: actual value %s should be greater than expected value %s%%s%%s\n"
                    ,   "%%s(%%d): test condition failed: actual value %s should be less than expected value %s%%s%%s\n"
                    ,   "%%s(%%d): test condition failed: actual value %s should be greater than or equal to the expected value %s%%s%%s\n"
                    ,   "%%s(%%d): test condition failed: actual value %s should be less than or equal to the expected value %s%%s%%s\n"
                    ,   s_fmts_[0]
                    ,   s_fmts_[1]
                };
                enum { bufferSize_ = 115 };
                STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS_(s_fmtBases) == xtestsComparison_max_enumerator);
                static char         s_fmts[STLSOFT_NUM_ELEMENTS_(s_fmtBases)][bufferSize_];
                STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS_(s_fmts) == xtestsComparison_max_enumerator);
                STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS_(s_fmts) == STLSOFT_NUM_ELEMENTS_(s_fmtBases));
                STLSOFT_ASSERT(strlen(s_fmtBases[0]) < bufferSize_);
                STLSOFT_ASSERT(strlen(s_fmtBases[1]) < bufferSize_);
                STLSOFT_ASSERT(strlen(s_fmtBases[2]) < bufferSize_);
                STLSOFT_ASSERT(strlen(s_fmtBases[3]) < bufferSize_);
                STLSOFT_ASSERT(strlen(s_fmtBases[4]) < bufferSize_);
                STLSOFT_ASSERT(strlen(s_fmtBases[5]) < bufferSize_);
                STLSOFT_ASSERT(strlen(s_fmtBases[6]) < bufferSize_);
                STLSOFT_ASSERT(strlen(s_fmtBases[7]) < bufferSize_);
                static char const*  s_fmt64 =   stlsoft::integral_printf_traits<stlsoft::sint64_t>::decimal_format_a();
                static const int    s_len   =   0
                                            +   xtests_sprintf_2_(&s_fmts[0][0], STLSOFT_NUM_ELEMENTS_(s_fmts[0]), s_fmtBases[0], s_fmt64, s_fmt64)
                                            +   xtests_sprintf_2_(&s_fmts[1][0], STLSOFT_NUM_ELEMENTS_(s_fmts[1]), s_fmtBases[1], s_fmt64, s_fmt64)
                                            +   xtests_sprintf_2_(&s_fmts[2][0], STLSOFT_NUM_ELEMENTS_(s_fmts[2]), s_fmtBases[2], s_fmt64, s_fmt64)
                                            +   xtests_sprintf_2_(&s_fmts[3][0], STLSOFT_NUM_ELEMENTS_(s_fmts[3]), s_fmtBases[3], s_fmt64, s_fmt64)
                                            +   xtests_sprintf_2_(&s_fmts[4][0], STLSOFT_NUM_ELEMENTS_(s_fmts[4]), s_fmtBases[4], s_fmt64, s_fmt64)
                                            +   xtests_sprintf_2_(&s_fmts[5][0], STLSOFT_NUM_ELEMENTS_(s_fmts[5]), s_fmtBases[5], s_fmt64, s_fmt64)
                                            +   xtests_sprintf_2_(&s_fmts[6][0], STLSOFT_NUM_ELEMENTS_(s_fmts[6]), s_fmtBases[6], s_fmt64, s_fmt64)
                                            +   0;

# if defined(STLSOFT_COMPILER_IS_BORLAND)
#  undef static
# endif /* compiler */

                char const*         fmt = s_fmts[comparison];

                switch(verbosity)
                {
                    case    XTESTS_VERBOSITY_SILENT:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                        fmt = "";
                        break;
                    default:
                        STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                    case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY:
                    XTESTS_VERBOSITY_VALID_MISSING_CASES
                    case    XTESTS_VERBOSITY_VERBOSE:
                        break;
                }

                xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                ,   fmt
                                ,   file, line, actualValue, expectedValue, (NULL != function) ? " in function " : "", stlsoft_ns_qual(c_str_ptr)(function));

                STLSOFT_SUPPRESS_UNUSED(s_len);
            }

            virtual void onTestFailed_uint64_(char const* file, int line, char const* function, char const* /* expr */, uint64_t expectedValue, uint64_t actualValue, xtests_comparison_t comparison, int verbosity)
            {
# if defined(STLSOFT_COMPILER_IS_BORLAND)
#  define static
# endif /* compiler */

                // Note: The following code has a race condition, but it is entirely benign, so does not matter

                static char const*  s_fmts_[] =
                {
                        "%%s(%%d): test condition failed: actual value %s should equal the expected value %s%%s%%s\n"
                    ,   "%%s(%%d): test condition failed: actual value %s should not equal expected value %s%%s%%s\n"
                };

                static char const*  s_fmtBases[] =
                {
                        s_fmts_[0]
                    ,   s_fmts_[1]
                    ,   "%%s(%%d): test condition failed: actual value %s should be greater than expected value %s%%s%%s\n"
                    ,   "%%s(%%d): test condition failed: actual value %s should be less than expected value %s%%s%%s\n"
                    ,   "%%s(%%d): test condition failed: actual value %s should be greater than or equal to the expected value %s%%s%%s\n"
                    ,   "%%s(%%d): test condition failed: actual value %s should be less than or equal to the expected value %s%%s%%s\n"
                    ,   s_fmts_[0]
                    ,   s_fmts_[1]
                };
                enum { bufferSize_ = 115 };
                STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS_(s_fmtBases) == xtestsComparison_max_enumerator);
                static char         s_fmts[STLSOFT_NUM_ELEMENTS_(s_fmtBases)][bufferSize_];
                STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS_(s_fmts) == xtestsComparison_max_enumerator);
                STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS_(s_fmts) == STLSOFT_NUM_ELEMENTS_(s_fmtBases));
                STLSOFT_ASSERT(strlen(s_fmtBases[0]) < bufferSize_);
                STLSOFT_ASSERT(strlen(s_fmtBases[1]) < bufferSize_);
                STLSOFT_ASSERT(strlen(s_fmtBases[2]) < bufferSize_);
                STLSOFT_ASSERT(strlen(s_fmtBases[3]) < bufferSize_);
                STLSOFT_ASSERT(strlen(s_fmtBases[4]) < bufferSize_);
                STLSOFT_ASSERT(strlen(s_fmtBases[5]) < bufferSize_);
                STLSOFT_ASSERT(strlen(s_fmtBases[6]) < bufferSize_);
                STLSOFT_ASSERT(strlen(s_fmtBases[7]) < bufferSize_);
                static char const*  s_fmt64 =   stlsoft::integral_printf_traits<stlsoft::uint64_t>::decimal_format_a();
                static const int    s_len   =   0
                                            +   xtests_sprintf_2_(&s_fmts[0][0], STLSOFT_NUM_ELEMENTS_(s_fmts[0]), s_fmtBases[0], s_fmt64, s_fmt64)
                                            +   xtests_sprintf_2_(&s_fmts[1][0], STLSOFT_NUM_ELEMENTS_(s_fmts[1]), s_fmtBases[1], s_fmt64, s_fmt64)
                                            +   xtests_sprintf_2_(&s_fmts[2][0], STLSOFT_NUM_ELEMENTS_(s_fmts[2]), s_fmtBases[2], s_fmt64, s_fmt64)
                                            +   xtests_sprintf_2_(&s_fmts[3][0], STLSOFT_NUM_ELEMENTS_(s_fmts[3]), s_fmtBases[3], s_fmt64, s_fmt64)
                                            +   xtests_sprintf_2_(&s_fmts[4][0], STLSOFT_NUM_ELEMENTS_(s_fmts[4]), s_fmtBases[4], s_fmt64, s_fmt64)
                                            +   xtests_sprintf_2_(&s_fmts[5][0], STLSOFT_NUM_ELEMENTS_(s_fmts[5]), s_fmtBases[5], s_fmt64, s_fmt64)
                                            +   xtests_sprintf_2_(&s_fmts[6][0], STLSOFT_NUM_ELEMENTS_(s_fmts[6]), s_fmtBases[6], s_fmt64, s_fmt64)
                                            +   0;

# if defined(STLSOFT_COMPILER_IS_BORLAND)
#  undef static
# endif /* compiler */

                char const*         fmt = s_fmts[comparison];

                switch(verbosity)
                {
                    case    XTESTS_VERBOSITY_SILENT:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                        fmt = "";
                        break;
                    default:
                        STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                    case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY:
                    XTESTS_VERBOSITY_VALID_MISSING_CASES
                    case    XTESTS_VERBOSITY_VERBOSE:
                        break;
                }

                xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                ,   fmt
                                ,   file, line, actualValue, expectedValue, (NULL != function) ? " in function " : "", stlsoft_ns_qual(c_str_ptr)(function));

                STLSOFT_SUPPRESS_UNUSED(s_len);
            }
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */

            virtual void onTestFailed_(char const* file, int line, char const* function, char const* expr, xtests_comparison_t /* comparison */, int verbosity)
            {
                static const char*  s_fmts[] =
                {
                        "%s(%d): test condition \"%s\" failed\n"
                    ,   "%s(%d): test condition \"%s\" failed in function %s\n"
                };
                char const*         fmt = s_fmts[NULL != function];

                switch(verbosity)
                {
                    case    XTESTS_VERBOSITY_SILENT:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                        fmt = "";
                        break;
                    default:
                        STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                    case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY:
                    XTESTS_VERBOSITY_VALID_MISSING_CASES
                    case    XTESTS_VERBOSITY_VERBOSE:
                        break;
                }

                xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                ,   fmt
                                ,   file, line, expr, function);
            }

            virtual void onWriteFailMessage(void* /* reporterParam */, char const* file, int line, char const* function, char const* message, char const* qualifyingInformation, int verbosity)
            {
                static const char  s_fmt[] = "%s(%d): %s%s%s%s%s\n";

                switch(verbosity)
                {
                    case    XTESTS_VERBOSITY_SILENT:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                        break;
                    default:
                        STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                    case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY:
                    XTESTS_VERBOSITY_VALID_MISSING_CASES
                    case    XTESTS_VERBOSITY_VERBOSE:
                        xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                        ,   s_fmt
                                        ,   file, line, message, (NULL != function) ? " in function " : "", (NULL != function) ? function : "", (NULL != qualifyingInformation) ? ": " : "", (NULL != qualifyingInformation) ? qualifyingInformation : "");
                        break;
                }
            }

            virtual void onCaseExcepted(void* /* reporterParam */, char const* caseName, char const* exceptionType, char const* exceptionMessage, int verbosity)
            {
                int level = 0;

                switch(verbosity)
                {
                    case    XTESTS_VERBOSITY_SILENT:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                        break;
                    case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                        level = 1;
                        break;
                    default:
                            STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                    case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY:
                    XTESTS_VERBOSITY_VALID_MISSING_CASES
                    case    XTESTS_VERBOSITY_VERBOSE:
                        level = 2;
                        break;
                }

                STLSOFT_ASSERT(level >= 0 && level < 3);

                static char const*  s_fmts[] =
                {
                        ""
                    ,   "\t%s: UX '%s' rx; msg='%s'\n"
                    ,   "\tTest case '%s': received unexpected exception of type '%s', with message '%s'\n"
                };
                char const*         fmt = s_fmts[level];

                xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                ,   fmt
                                ,   caseName, exceptionType, exceptionMessage);
            }

            virtual void onCaseExceptionExpected(void* /* reporterParam */, char const* caseName, char const* exceptionType, int verbosity)
            {
                int level = 0;

                switch(verbosity)
                {
                    case    XTESTS_VERBOSITY_SILENT:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                        break;
                    case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                        level = 1;
                        break;
                    default:
                            STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                    case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY:
                    XTESTS_VERBOSITY_VALID_MISSING_CASES
                    case    XTESTS_VERBOSITY_VERBOSE:
                        level = 2;
                        break;
                }

                STLSOFT_ASSERT(level >= 0 && level < 3);

                static char const*  s_fmts[] =
                {
                        ""
                    ,   "\t%s: EX '%s' not rx\n"
                    ,   "\tTest case '%s': expected exception of type '%s' was not received\n"
                };
                char const*         fmt = s_fmts[level];

                xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                ,   fmt
                                ,   caseName, exceptionType);
            }

            virtual void onEndTestCase(
                void*                           /* reporterParam */
            ,   char const*                     /* name */
            ,   xTests_runner_results_t const*  results
            ,   int                             verbosity
            )
            {
                int level = 0;
                int allTestsHavePassed =
                    ( 0 == results->numFailedTests &&
                      0 == results->numUnexpectedExceptions &&
                      0 == results->numMissingExpectedExceptions);

                switch(verbosity)
                {
                    case    XTESTS_VERBOSITY_SILENT:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                        level = 0;
                        break;

                    case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                        if(!allTestsHavePassed)
                        {
                            if(0 == results[1].numFailedCases)
                            {
                                level = 2;
                            }
                        }
                        break;
                    case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                        if(!allTestsHavePassed)
                        {
                            level = 2;
                        }
                        break;

                    case    XTESTS_VERBOSITY_CASE_SUMMARY:
                        level = 2;
                        // Fall through
                        break;

                    default:
                            STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                    XTESTS_VERBOSITY_VALID_MISSING_CASES
                    case    XTESTS_VERBOSITY_VERBOSE:
                        level = 2;
                        break;
                }
                STLSOFT_ASSERT(level >= 0 && level < 3);

                static char const*  s_fmts[] =
                {
                        ""
                    ,   "\t%s: %u / %u / %u / %u / %u; result=%s\n"
                    ,   "\tTest case '%s': %u assertions; %u succeeded; %u failed; %u unexpected exceptions; %u missing expected exceptions; result=%s\n"
                };
                char const*         fmt = s_fmts[level];

                xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                ,   fmt
                                ,   results->name
                                ,   results->numTests
                                ,   results->numTests - results->numFailedTests
                                ,   results->numFailedTests
                                ,   results->numUnexpectedExceptions
                                ,   results->numMissingExpectedExceptions
                                ,   allTestsHavePassed ? "SUCCESS" : "FAILURE"
                                );
            }

            virtual void onPrintRunnerResults(void* /* reporterParam */, xTests_runner_results_t const* results, int verbosity)
            {
                int level = 0;

                switch(verbosity)
                {
                    case    XTESTS_VERBOSITY_SILENT:
                        level = 0;
                        break;
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY_ON_ERROR:
                        if(0 != results->numFailedCases)
                        {
                            level = 1;
                        }
                        break;
                    case    XTESTS_VERBOSITY_RUNNER_SUMMARY:
                        level = 1;
                        break;
                    case    XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR:
                    case    XTESTS_VERBOSITY_CASE_SUMMARY_ON_ERROR:
                        level = 1;
                        if(0 != results->numFailedCases)
                        {
                            level = 2;
                        }
                        break;
                    case    XTESTS_VERBOSITY_CASE_SUMMARY:
                        level = 2;
                        break;
                    default:
                        STLSOFT_MESSAGE_ASSERT("verbosity not recognised", 0);
                    XTESTS_VERBOSITY_VALID_MISSING_CASES
                    case    XTESTS_VERBOSITY_VERBOSE:
                        level = 2;
                        break;
                }

                STLSOFT_ASSERT(level >= 0 && level < 3);

                static char const*  s_fmts[] =
                {
                        ""
                    ,   "Test runner '%s' complete:"
                        "\t%u / %u / %u / %u / %u / %u; result=%s\n"
                        "\n"
                    ,   "------------------------------------------------------------\n"
                        "Test runner '%s' complete:\n"
                        "\t%u test case(s)\n"
                        "\t%u total assertion(s)\n"
                        "\t%u total assertion(s) succeeded\n"
                        "\t%u total assertion(s) failed\n"
                        "\t%u total unexpected exception(s)\n"
                        "\t%u total missing expected exception(s)\n"
                        "\tresult=%s\n"
                        "\n"
                };
                char const*         fmt = s_fmts[level];

                xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                ,   fmt
                                ,   results->name
                                ,   static_cast<unsigned>(results->numCases)
                                ,   static_cast<unsigned>(results->numTests)
                                ,   static_cast<unsigned>(results->numTests - results->numFailedTests)
                                ,   static_cast<unsigned>(results->numFailedTests)
                                ,   static_cast<unsigned>(results->numUnexpectedExceptions)
                                ,   static_cast<unsigned>(results->numMissingExpectedExceptions)
                                ,   (   0u == results->numFailedTests &&
                                        0u == results->numUnexpectedExceptions &&
                                        0u == results->numMissingExpectedExceptions) ? "SUCCESS" : "FAILURE"
                                );
            }

            virtual void onAbend(void* /* reporterParam */, char const* /* message */, int /* verbosity */)
            {
            }

            virtual void onDefect(void* /* reporterParam */, char const* message, char const* qualifier, int /* verbosity */)
            {
                if( NULL != qualifier &&
                    '\0' == qualifier)
                {
                    qualifier = NULL;
                }

                xtests_mxnprintf_(  m_sinks, m_numSinks, 50
                                ,   (NULL != qualifier) ? "defect: %s: %s\n" : "defect: %s\n"
                                ,   message
                                ,   qualifier);
            }

            virtual void onEndRunner(void* /* reporterParam */, char const* /* name */, int /* verbosity */)
            {
            }

        private: // Member variables
            const int       m_flags;
            const size_t    m_numSinks;
            xtests_sink_t_  m_sinks[2];

        private: // Not to be implemented
            void operator =(fprintf_reporter const&)
            {}
        };

#if defined(STLSOFT_COMPILER_IS_BORLAND) && \
    __BORLANDC__ >= 0x0610
# pragma warn -8104
#endif

        static fprintf_reporter s_reporter(stm, flags);

#if defined(STLSOFT_COMPILER_IS_BORLAND) && \
    __BORLANDC__ >= 0x0610
# pragma warn .8104
#endif

        reporter = &s_reporter;
    }

    return reporter;
}

void RunnerInfo::report_unstartedCase_defect_()
{
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
        throw std::runtime_error("not in a test case; call XTESTS_CASE_BEGIN() ?");
#else /* ? STLSOFT_CF_EXCEPTION_SUPPORT */
        m_reporter->onDefect(m_reporterParam, "not in a test case; call XTESTS_CASE_BEGIN() ?", NULL, m_verbosity);
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}


RunnerInfo::RunnerInfo(
    char const*             name
,   int                     verbosity
,   xTests_Reporter_t*      reporter
,   void*                   reporterParam
,   FILE*                   stm
,   int                     flags
,   xTests_Setup_t const    setup
,   xTests_Teardown_t const teardown
,   void* const             setupParam
)
    : m_reporter(get_reporter_(reporter, stm, flags))
    , m_reporterParam(reporterParam)
    , m_name(name)
    , m_verbosity(verbosity)
    , m_flags(flags)
    , m_setup(setup)
    , m_teardown(teardown)
    , m_setupParam(setupParam)
    , m_testCases()
    , m_totalConditions(0)
    , m_failedConditions(0)
    , m_unexpectedExceptions(0)
    , m_expectedExceptions(0)
    , m_currentCase(m_testCases.end())
    , m_requireFailed(false)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    STLSOFT_MESSAGE_ASSERT("Invalid verbosity; must be in range [XTESTS_VERBOSITY_SILENT, XTESTS_VERBOSITY_VERBOSE]", verbosity >= XTESTS_VERBOSITY_SILENT && verbosity <= XTESTS_VERBOSITY_VERBOSE);
}

int RunnerInfo::BeginCase(
    char const* name
,   char const* description
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() != m_testCases.find(name))
    {
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
        throw std::runtime_error("test case already exists");
#else /* ? STLSOFT_CF_EXCEPTION_SUPPORT */
        m_reporter->onDefect(m_reporterParam, "test case already exists", name, m_verbosity);

        return -1;
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
    }
    else if(m_testCases.end() != m_currentCase)
    {
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
        throw std::runtime_error("a test case is already in progress; call XTESTS_CASE_END() ?");
#else /* ? STLSOFT_CF_EXCEPTION_SUPPORT */
        m_reporter->onDefect(m_reporterParam, "a test case is already in progress; call XTESTS_CASE_END() ?", NULL, m_verbosity);

        return -1;
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
    }
    else
    {
        m_requireFailed = false;

        m_currentCase = m_testCases.insert(std::make_pair(string_t(name), TestInfo(name, description))).first;

        if(NULL != m_setup)
        {
            int r = (*m_setup)(m_setupParam);

            if(0 != r)
            {
                m_reporter->onDefect(m_reporterParam, "setup function returned a non-zero value", NULL, m_verbosity);

                m_testCases.erase(m_currentCase);
                m_currentCase = m_testCases.end();

                return -1;
            }
        }

        m_reporter->onBeginTestCase(m_reporterParam, name, description, m_verbosity);
    }

    return 0;
}

int RunnerInfo::EndCase(char const* /* name */)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo const& testInfo = (*m_currentCase).second;

        xTests_runner_results_t results[2];

        results[0].name                         =   testInfo.name.c_str();
        results[0].numCases                     =   0;
        results[0].numTests                     =   testInfo.totalConditions;
        results[0].numFailedCases               =   0;
        results[0].numFailedTests               =   testInfo.failedConditions;
        results[0].numUnexpectedExceptions      =   testInfo.unexpectedExceptions;
        results[0].numMissingExpectedExceptions =   testInfo.expectedExceptions;

        results[1].name                         =   "*";
        results[1].numCases                     =   static_cast<uint32_t>(m_testCases.size());
        results[1].numTests                     =   m_totalConditions;
        results[1].numFailedCases               =   static_cast<uint32_t>(NumberOfFailedTestCases());
        results[1].numFailedTests               =   m_failedConditions;
        results[1].numUnexpectedExceptions      =   m_expectedExceptions;
        results[1].numMissingExpectedExceptions =   m_unexpectedExceptions;


        // NOTE: The following are not x += y because Intel C/C++ gets awfully confused by that
        m_totalConditions       =   m_totalConditions + testInfo.totalConditions;
        m_failedConditions      =   m_failedConditions + testInfo.failedConditions;
        m_unexpectedExceptions  =   m_unexpectedExceptions + testInfo.unexpectedExceptions;
        m_expectedExceptions    =   m_expectedExceptions + testInfo.expectedExceptions;

        if(NULL != m_teardown)
        {
            (*m_teardown)(m_setupParam);
        }

        m_currentCase = m_testCases.end();

        if( 0 != testInfo.totalConditions ||
            0 == (xtestsReportOnlyNonEmptyCases & m_flags))
        {
            m_reporter->onEndTestCase(m_reporterParam, results[0].name, &results[0], m_verbosity);
        }

        return 0;
    }

    RETURN_UNUSED(-1);
}

int RunnerInfo::RegisterSuccessfulCondition(
    char const* file
,   int         line
,   char const* function
,   char const* expr
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.totalConditions;

        m_reporter->onTestPassed(m_reporterParam, file, line, function, expr, xtestsComparisonEqual, m_verbosity);

        return 0;
    }

    RETURN_UNUSED(-1);
}

int RunnerInfo::RegisterFailedCondition(
    char const* file
,   int         line
,   char const* function
,   char const* expr
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.failedConditions;
        ++testInfo.totalConditions;

        Call_onTestFailed(m_reporter, m_reporterParam, file, line, function, expr, NULL, NULL, -1, xtestsComparisonEqual, m_verbosity);

        return 0;
    }

    RETURN_UNUSED(-1);
}

int RunnerInfo::RegisterFailedCondition_long(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   long                expected
,   long                actual
,   xtests_comparison_t comp
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.failedConditions;
        ++testInfo.totalConditions;

        xtests_variable_t   expectedValue(expected);
        xtests_variable_t   actualValue(actual);

        Call_onTestFailed(m_reporter, m_reporterParam, file, line, function, expr, &expectedValue, &actualValue, -1, comp, m_verbosity);

        return 0;
    }

    RETURN_UNUSED(-1);
}

int RunnerInfo::RegisterFailedCondition_ulong(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   unsigned long       expected
,   unsigned long       actual
,   xtests_comparison_t comp
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.failedConditions;
        ++testInfo.totalConditions;

        xtests_variable_t   expectedValue(expected);
        xtests_variable_t   actualValue(actual);

        Call_onTestFailed(m_reporter, m_reporterParam, file, line, function, expr, &expectedValue, &actualValue, -1, comp, m_verbosity);

        return 0;
    }

    RETURN_UNUSED(-1);
}

#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
int RunnerInfo::RegisterFailedCondition_sint64(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   sint64_t            expected
,   sint64_t            actual
,   xtests_comparison_t comp
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.failedConditions;
        ++testInfo.totalConditions;

        xtests_variable_t   expectedValue(expected);
        xtests_variable_t   actualValue(actual);

        Call_onTestFailed(m_reporter, m_reporterParam, file, line, function, expr, &expectedValue, &actualValue, -1, comp, m_verbosity);

        return 0;
    }

    RETURN_UNUSED(-1);
}

int RunnerInfo::RegisterFailedCondition_uint64(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   uint64_t            expected
,   uint64_t            actual
,   xtests_comparison_t comp
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.failedConditions;
        ++testInfo.totalConditions;

        xtests_variable_t   expectedValue(expected);
        xtests_variable_t   actualValue(actual);

        Call_onTestFailed(m_reporter, m_reporterParam, file, line, function, expr, &expectedValue, &actualValue, -1, comp, m_verbosity);

        return 0;
    }

    RETURN_UNUSED(-1);
}
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */


int RunnerInfo::RegisterFailedCondition_boolean(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   bool                expected
,   bool                actual
,   xtests_comparison_t comp
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.failedConditions;
        ++testInfo.totalConditions;

        xtests_variable_t   expectedValue(expected);
        xtests_variable_t   actualValue(actual);

        Call_onTestFailed(m_reporter, m_reporterParam, file, line, function, expr, &expectedValue, &actualValue, -1, comp, m_verbosity);

        return 0;
    }

    RETURN_UNUSED(-1);
}

int RunnerInfo::RegisterFailedCondition_double(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   double const&       expected
,   double const&       actual
,   xtests_comparison_t comp
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.failedConditions;
        ++testInfo.totalConditions;

        xtests_variable_t   expectedValue(expected);
        xtests_variable_t   actualValue(actual);

        Call_onTestFailed(m_reporter, m_reporterParam, file, line, function, expr, &expectedValue, &actualValue, -1, comp, m_verbosity);

        return 0;
    }

    RETURN_UNUSED(-1);
}

int RunnerInfo::TestMultibyteStrings(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   char const*         expected
,   char const*         actual
,   xtests_comparison_t comp
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.totalConditions;

        // Now do test, taking into account NULL pointers
        int comparisonSucceeded = false;

        switch(comp)
        {
            case    xtestsComparisonEqual:
                if(0 == xtests_strcmp_a_(expected, actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonApproxEqual:
                if(0 == xtests_stricmp_a_(expected, actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonNotEqual:
                if(0 != xtests_strcmp_a_(expected, actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonApproxNotEqual:
                if(0 != xtests_stricmp_a_(expected, actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonGreaterThan:
                if(0 > xtests_strcmp_a_(expected, actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonLessThan:
                if(0 < xtests_strcmp_a_(expected, actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonGreaterThanOrEqual:
                if(0 >= xtests_strcmp_a_(expected, actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonLessThanOrEqual:
                if(0 <= xtests_strcmp_a_(expected, actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparison_max_enumerator:
                xtests_abend("invalid test comparison type: test framework may be out of date!");
                break;
        }

        if(comparisonSucceeded)
        {
            return 1;
        }
        else
        {
            ++testInfo.failedConditions;

            xtests_variable_t   expectedValue(expected);
            xtests_variable_t   actualValue(actual);

            Call_onTestFailed(m_reporter, m_reporterParam, file, line, function, expr, &expectedValue, &actualValue, -1, comp, m_verbosity);

            return 0;
        }
    }

    RETURN_UNUSED(-1);
}

int RunnerInfo::TestMultibyteStringsN(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   char const*         expected
,   char const*         actual
,   ptrdiff_t           n
,   size_t              cchExpected
,   size_t              cchActual
,   xtests_comparison_t comp)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.totalConditions;

        // Now do test, taking into account NULL pointers
        int     comparisonSucceeded =   false;
        size_t  ncmp;

        if(n >= 0)
        {
            ncmp = static_cast<size_t>(n);
        }
        else
        {
            // -ve n is means an upper limit, rather than an exact
            // correspondence, so we trim to size if necessary

            ncmp = static_cast<size_t>(-n);

            if(ncmp > cchExpected)
            {
                ncmp = cchExpected;
            }
        }

        if( cchActual < ncmp ||
            cchExpected < ncmp)
        {
            // actual string is less than required length, so comparison
            // fails
            switch(comp)
            {
                case    xtestsComparisonEqual:
                case    xtestsComparisonApproxEqual:
                    break;
                case    xtestsComparisonNotEqual:
                case    xtestsComparisonApproxNotEqual:
                    comparisonSucceeded = true;
                    break;
                case    xtestsComparisonGreaterThan:
                case    xtestsComparisonLessThan:
                case    xtestsComparisonGreaterThanOrEqual:
                case    xtestsComparisonLessThanOrEqual:
                    break;
                default:
                    STLSOFT_ASSERT(0);
                case    xtestsComparison_max_enumerator:
                    xtests_abend("invalid test comparison type: test framework may be out of date!");
                    break;
            }
        }
        else
        {
            switch(comp)
            {
                case    xtestsComparisonEqual:
                    if(0 == xtests_strncmp_a_(expected, actual, ncmp))
                    {
                        comparisonSucceeded = true;
                    }
                    break;
                case    xtestsComparisonApproxEqual:
                    if(0 == xtests_strnicmp_a_(expected, actual, ncmp))
                    {
                        comparisonSucceeded = true;
                    }
                    break;
                case    xtestsComparisonNotEqual:
                    if(0 != xtests_strncmp_a_(expected, actual, ncmp))
                    {
                        comparisonSucceeded = true;
                    }
                    break;
                case    xtestsComparisonApproxNotEqual:
                    if(0 != xtests_strnicmp_a_(expected, actual, ncmp))
                    {
                        comparisonSucceeded = true;
                    }
                    break;
                case    xtestsComparisonGreaterThan:
                    if(0 > xtests_strncmp_a_(expected, actual, ncmp))
                    {
                        comparisonSucceeded = true;
                    }
                    break;
                case    xtestsComparisonLessThan:
                    if(0 < xtests_strncmp_a_(expected, actual, ncmp))
                    {
                        comparisonSucceeded = true;
                    }
                    break;
                case    xtestsComparisonGreaterThanOrEqual:
                    if(0 >= xtests_strncmp_a_(expected, actual, ncmp))
                    {
                        comparisonSucceeded = true;
                    }
                    break;
                case    xtestsComparisonLessThanOrEqual:
                    if(0 <= xtests_strncmp_a_(expected, actual, ncmp))
                    {
                        comparisonSucceeded = true;
                    }
                    break;
                case    xtestsComparison_max_enumerator:
                    xtests_abend("invalid test comparison type: test framework may be out of date!");
                    break;
            }
        }

        if(comparisonSucceeded)
        {
            return 1;
        }
        else
        {
            ++testInfo.failedConditions;

            xtests_variable_t   expectedValue(expected, cchExpected, xtestsTestPartialComparison);
            xtests_variable_t   actualValue(actual, cchActual, xtestsTestPartialComparison);

            Call_onTestFailed(m_reporter, m_reporterParam, file, line, function, expr, &expectedValue, &actualValue, n, comp, m_verbosity);

            return 0;
        }
    }

    RETURN_UNUSED(-1);
}

int RunnerInfo::TestWideStrings(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   wchar_t const*      expected
,   wchar_t const*      actual
,   xtests_comparison_t comp
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.totalConditions;

        // Now do test, taking into account NULL pointers
        int comparisonSucceeded = false;

        switch(comp)
        {
            case    xtestsComparisonEqual:
                if(0 == xtests_strcmp_w_(expected, actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonApproxEqual:
                if(0 == xtests_stricmp_w_(expected, actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonNotEqual:
                if(0 != xtests_strcmp_w_(expected, actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonApproxNotEqual:
                if(0 != xtests_stricmp_w_(expected, actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonGreaterThan:
                if(0 > xtests_strcmp_w_(expected, actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonLessThan:
                if(0 < xtests_strcmp_w_(expected, actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonGreaterThanOrEqual:
                if(0 >= xtests_strcmp_w_(expected, actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonLessThanOrEqual:
                if(0 <= xtests_strcmp_w_(expected, actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparison_max_enumerator:
                xtests_abend("invalid test comparison type: test framework may be out of date!");
                break;
        }

        if(comparisonSucceeded)
        {
            return 1;
        }
        else
        {
            ++testInfo.failedConditions;

            xtests_variable_t   expectedValue(expected);
            xtests_variable_t   actualValue(actual);

            Call_onTestFailed(m_reporter, m_reporterParam, file, line, function, expr, &expectedValue, &actualValue, -1, comp, m_verbosity);

            return 0;
        }
    }

    RETURN_UNUSED(-1);
}

int RunnerInfo::TestWideStringsN(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   wchar_t const*      expected
,   wchar_t const*      actual
,   int                 n
,   size_t              cchExpected
,   size_t              cchActual
,   xtests_comparison_t comp)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.totalConditions;

        // Now do test, taking into account NULL pointers
        int     comparisonSucceeded = false;
        size_t  ncmp;

        if(n >= 0)
        {
            ncmp = static_cast<size_t>(n);
        }
        else
        {
            // -ve n is means an upper limit, rather than an exact
            // correspondence, so we trim to size if necessary

            ncmp = static_cast<size_t>(-n);

            if(ncmp > cchExpected)
            {
                ncmp = cchExpected;
            }
        }

        if( cchActual < ncmp ||
            cchExpected < ncmp)
        {
            // actual string is less than required length, so comparison
            // fails
            switch(comp)
            {
                case    xtestsComparisonEqual:
                case    xtestsComparisonApproxEqual:
                    break;
                case    xtestsComparisonNotEqual:
                case    xtestsComparisonApproxNotEqual:
                    comparisonSucceeded = true;
                    break;
                case    xtestsComparisonGreaterThan:
                case    xtestsComparisonLessThan:
                case    xtestsComparisonGreaterThanOrEqual:
                case    xtestsComparisonLessThanOrEqual:
                    break;
                case    xtestsComparison_max_enumerator:
                    xtests_abend("invalid test comparison type: test framework may be out of date!");
                    break;
            }
        }
        else
        {
            switch(comp)
            {
                case    xtestsComparisonEqual:
                    if(0 == xtests_strncmp_w_(expected, actual, ncmp))
                    {
                        comparisonSucceeded = true;
                    }
                    break;
                case    xtestsComparisonApproxEqual:
                    if(0 == xtests_strnicmp_w_(expected, actual, ncmp))
                    {
                        comparisonSucceeded = true;
                    }
                    break;
                case    xtestsComparisonNotEqual:
                    if(0 != xtests_strncmp_w_(expected, actual, ncmp))
                    {
                        comparisonSucceeded = true;
                    }
                    break;
                case    xtestsComparisonApproxNotEqual:
                    if(0 != xtests_strnicmp_w_(expected, actual, ncmp))
                    {
                        comparisonSucceeded = true;
                    }
                    break;
                case    xtestsComparisonGreaterThan:
                    if(0 > xtests_strncmp_w_(expected, actual, ncmp))
                    {
                        comparisonSucceeded = true;
                    }
                    break;
                case    xtestsComparisonLessThan:
                    if(0 < xtests_strncmp_w_(expected, actual, ncmp))
                    {
                        comparisonSucceeded = true;
                    }
                    break;
                case    xtestsComparisonGreaterThanOrEqual:
                    if(0 >= xtests_strncmp_w_(expected, actual, ncmp))
                    {
                        comparisonSucceeded = true;
                    }
                    break;
                case    xtestsComparisonLessThanOrEqual:
                    if(0 <= xtests_strncmp_w_(expected, actual, ncmp))
                    {
                        comparisonSucceeded = true;
                    }
                    break;
                case    xtestsComparison_max_enumerator:
                    xtests_abend("invalid test comparison type: test framework may be out of date!");
                    break;
            }
        }

        if(comparisonSucceeded)
        {
            return 1;
        }
        else
        {
            ++testInfo.failedConditions;

            xtests_variable_t   expectedValue(expected, cchExpected, xtestsTestPartialComparison);
            xtests_variable_t   actualValue(actual, cchActual, xtestsTestPartialComparison);

            Call_onTestFailed(m_reporter, m_reporterParam, file, line, function, expr, &expectedValue, &actualValue, n, comp, m_verbosity);

            return 0;
        }
    }

    RETURN_UNUSED(-1);
}

int RunnerInfo::TestMultibyteStringContains(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   char const*         expected
,   char const*         actual
,   xtests_comparison_t comp
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.totalConditions;

        // Now do test, taking into account NULL pointers
        int comparisonSucceeded = false;

        switch(comp)
        {
            case    xtestsComparisonEqual:
                if(NULL != xtests_strstr_(actual, expected))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonApproxEqual:
                if(NULL != xtests_stristr_(actual, expected))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonNotEqual:
                if(NULL == xtests_strstr_(actual, expected))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonApproxNotEqual:
                if(NULL == xtests_stristr_(actual, expected))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonGreaterThan:
            case    xtestsComparisonLessThan:
            case    xtestsComparisonGreaterThanOrEqual:
            case    xtestsComparisonLessThanOrEqual:
                xtests_abend("comparison type not valid for string containing tests");
                break;
            case    xtestsComparison_max_enumerator:
                xtests_abend("invalid test comparison type: test framework may be out of date!");
                break;
        }

        if(comparisonSucceeded)
        {
            return 1;
        }
        else
        {
            ++testInfo.failedConditions;

            xtests_variable_t   expectedValue(expected, xtestsTestContainment);
            xtests_variable_t   actualValue(actual, xtestsTestContainment);

            Call_onTestFailed(m_reporter, m_reporterParam, file, line, function, expr, &expectedValue, &actualValue, -1, comp, m_verbosity);

            return 0;
        }
    }

    RETURN_UNUSED(-1);
}


int RunnerInfo::TestWideStringContains(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   wchar_t const*      expected
,   wchar_t const*      actual
,   xtests_comparison_t comp
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.totalConditions;

        // Now do test, taking into account NULL pointers
        int comparisonSucceeded = false;

        switch(comp)
        {
            case    xtestsComparisonEqual:
                if(NULL != xtests_strstr_w_(actual, expected))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonApproxEqual:
                if(NULL != xtests_stristr_w_(actual, expected))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonNotEqual:
                if(NULL == xtests_strstr_w_(actual, expected))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonApproxNotEqual:
                if(NULL == xtests_stristr_w_(actual, expected))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonGreaterThan:
            case    xtestsComparisonLessThan:
            case    xtestsComparisonGreaterThanOrEqual:
            case    xtestsComparisonLessThanOrEqual:
                xtests_abend("comparison type not valid for string containing tests");
                break;
            case    xtestsComparison_max_enumerator:
                xtests_abend("invalid test comparison type: test framework may be out of date!");
                break;
        }

        if(comparisonSucceeded)
        {
            return 1;
        }
        else
        {
            ++testInfo.failedConditions;

            xtests_variable_t   expectedValue(expected, xtestsTestContainment);
            xtests_variable_t   actualValue(actual, xtestsTestContainment);

            Call_onTestFailed(m_reporter, m_reporterParam, file, line, function, expr, &expectedValue, &actualValue, -1, comp, m_verbosity);

            return 0;
        }
    }

    RETURN_UNUSED(-1);
}

int RunnerInfo::TestPointers(
    char const*             file
,   int                     line
,   char const*             function
,   char const*             expr
,   void volatile const*    expected
,   void volatile const*    actual
,   xtests_comparison_t     comp
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.totalConditions;

        // Now do test, taking into account NULL pointers
        int comparisonSucceeded = false;

        switch(comp)
        {
            case    xtestsComparisonEqual:
            case    xtestsComparisonApproxEqual:
                if(expected == actual)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonNotEqual:
            case    xtestsComparisonApproxNotEqual:
                if(expected != actual)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonGreaterThan:
                if(actual > expected)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonLessThan:
                if(actual < expected)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonGreaterThanOrEqual:
                if(actual >= expected)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonLessThanOrEqual:
                if(actual <= expected)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparison_max_enumerator:
                xtests_abend("invalid test comparison type: test framework may be out of date!");
                break;
        }

        if(comparisonSucceeded)
        {
            return 1;
        }
        else
        {
            ++testInfo.failedConditions;

            xtests_variable_t   expectedValue(expected);
            xtests_variable_t   actualValue(actual);

            Call_onTestFailed(m_reporter, m_reporterParam, file, line, function, expr, &expectedValue, &actualValue, -1, comp, m_verbosity);

            return 0;
        }
    }

    RETURN_UNUSED(-1);
}

int RunnerInfo::TestFunctionPointers(
    char const*             file
,   int                     line
,   char const*             function
,   char const*             expr
,   void                    (*expected)(void)
,   void                    (*actual)(void)
,   xtests_comparison_t     comp
)
{
    STLSOFT_ASSERT(xtestsComparisonEqual == comp || xtestsComparisonNotEqual == comp);

    STLSOFT_STATIC_ASSERT(sizeof(void*) == sizeof(void(*)(void)));

    union
    {
        void*   p;
        void    (*pfn)(void);

    } x, a;

    x.pfn = expected;
    a.pfn = actual;

    return TestPointers(
        file
    ,   line
    ,   function
    ,   expr
    ,   x.p
    ,   a.p
    ,   comp
    );
}

int RunnerInfo::TestCharacters(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   char                expected
,   char                actual
,   xtests_comparison_t comp
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.totalConditions;

        // Now do test, taking into account NULL pointers
        int comparisonSucceeded = false;

        switch(comp)
        {
            case    xtestsComparisonEqual:
                if(expected == actual)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonApproxEqual:
                if(::toupper(expected) == ::toupper(actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonNotEqual:
                if(expected != actual)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonApproxNotEqual:
                if(::toupper(expected) != ::toupper(actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonGreaterThan:
                if(actual > expected)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonLessThan:
                if(actual < expected)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonGreaterThanOrEqual:
                if(actual >= expected)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonLessThanOrEqual:
                if(actual <= expected)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparison_max_enumerator:
                xtests_abend("invalid test comparison type: test framework may be out of date!");
                break;
        }

        if(comparisonSucceeded)
        {
            return 1;
        }
        else
        {
            ++testInfo.failedConditions;

            xtests_variable_t   expectedValue(expected);
            xtests_variable_t   actualValue(actual);

            Call_onTestFailed(m_reporter, m_reporterParam, file, line, function, expr, &expectedValue, &actualValue, -1, comp, m_verbosity);

            return 0;
        }
    }

    RETURN_UNUSED(-1);
}

int RunnerInfo::TestCharacters(
    char const*         file
,   int                 line
,   char const*         function
,   char const*         expr
,   wchar_t             expected
,   wchar_t             actual
,   xtests_comparison_t comp
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.totalConditions;

        // Now do test, taking into account NULL pointers
        int comparisonSucceeded = false;

        switch(comp)
        {
            case    xtestsComparisonEqual:
                if(expected == actual)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonApproxEqual:
                if(::towupper(expected) == ::towupper(actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonNotEqual:
                if(expected != actual)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonApproxNotEqual:
                if(::towupper(expected) != ::towupper(actual))
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonGreaterThan:
                if(actual > expected)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonLessThan:
                if(actual < expected)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonGreaterThanOrEqual:
                if(actual >= expected)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparisonLessThanOrEqual:
                if(actual <= expected)
                {
                    comparisonSucceeded = true;
                }
                break;
            case    xtestsComparison_max_enumerator:
                xtests_abend("invalid test comparison type: test framework may be out of date!");
                break;
        }

        if(comparisonSucceeded)
        {
            return 1;
        }
        else
        {
            ++testInfo.failedConditions;

            xtests_variable_t   expectedValue(expected, xtestsVariableWideCharacter);
            xtests_variable_t   actualValue(actual, xtestsVariableWideCharacter);

            Call_onTestFailed(m_reporter, m_reporterParam, file, line, function, expr, &expectedValue, &actualValue, -1, comp, m_verbosity);

            return 0;
        }
    }

    RETURN_UNUSED(-1);
}


int RunnerInfo::WriteFailMessage(
    char const* file
,   int         line
,   char const* function
,   char const* message
,   char const* qualifyingInformation
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();

        return -1;
    }
    else
    {
        TestInfo& testInfo = (*m_currentCase).second;

        ++testInfo.failedConditions;
        ++testInfo.totalConditions;

        m_reporter->onWriteFailMessage(m_reporterParam, file, line, function, message, qualifyingInformation, m_verbosity);

        return 0;
    }

    RETURN_UNUSED(-1);
}

void RunnerInfo::CaseExcepted(
    char const* exceptionType
,   char const* exceptionMessage
)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();
    }
    else
    {
        TestInfo&   testInfo = (*m_currentCase).second;

        ++testInfo.unexpectedExceptions;

        m_reporter->onCaseExcepted(m_reporterParam, testInfo.name.c_str(), exceptionType, exceptionMessage, m_verbosity);
    }
}

void RunnerInfo::CaseExceptionExpected(char const* exceptionType)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if(m_testCases.end() == m_currentCase)
    {
        report_unstartedCase_defect_();
    }
    else
    {
        TestInfo&   testInfo = (*m_currentCase).second;

        ++testInfo.expectedExceptions;

        m_reporter->onCaseExceptionExpected(m_reporterParam, testInfo.name.c_str(), exceptionType, m_verbosity);
    }
}

void RunnerInfo::OnRequireFailed()
{
    m_requireFailed = true;
}

int RunnerInfo::HasRequiredConditionFailed() const
{
    return m_requireFailed;
}

void RunnerInfo::PrintStart()
{
    STLSOFT_ASSERT(NULL != m_reporter);

    m_reporter->onStartRunner(m_reporterParam, m_name.c_str(), m_verbosity);
}

void RunnerInfo::PrintEnd()
{
    STLSOFT_ASSERT(NULL != m_reporter);

    m_reporter->onEndRunner(m_reporterParam, m_name.c_str(), m_verbosity);
}

void RunnerInfo::PrintResults()
{
    STLSOFT_ASSERT(NULL != m_reporter);

    xTests_runner_results_t results;

    results.name                            =   m_name.c_str();
    results.numCases                        =   static_cast<uint32_t>(m_testCases.size());
    results.numTests                        =   m_totalConditions;
    results.numFailedCases                  =   static_cast<uint32_t>(NumberOfFailedTestCases());
    results.numFailedTests                  =   m_failedConditions;
    results.numMissingExpectedExceptions    =   m_expectedExceptions;
    results.numUnexpectedExceptions         =   m_unexpectedExceptions;

    m_reporter->onPrintRunnerResults(m_reporterParam, &results, m_verbosity);
}

void RunnerInfo::onAbend(char const* message)
{
    STLSOFT_ASSERT(NULL != m_reporter);

    m_reporter->onAbend(m_reporterParam, message, m_verbosity);
}

size_t RunnerInfo::NumberOfFailedTestCases() const
{
    STLSOFT_ASSERT(NULL != m_reporter);

    if( 0u == m_failedConditions &&
        0u == m_unexpectedExceptions &&
        0u == m_expectedExceptions)
    {
        return 0u;
    }
    else
    {
        size_t numFailed = 0;

        { for(test_map_t::const_iterator b = m_testCases.begin(); b != m_testCases.end(); ++b)
        {
            TestInfo const& testInfo = (*b).second;

            if(!testInfo.haveAllTestsPassed())
            {
                ++numFailed;
            }
        }}

        return numFailed;
    }
}

void RunnerInfo::Call_onTestFailed(
    xTests_Reporter_t* const    reporter
,   void*                       reporterParam
,   char const*                 file
,   int                         line
,   char const*                 function
,   char const*                 expr
,   xtests_variable_t const*    expectedValue
,   xtests_variable_t const*    actualValue
,   ptrdiff_t                   length
,   xtests_comparison_t         comparison
,   int                         verbosity
)
{
    if( verbosity != XTESTS_VERBOSITY_FIRST_CASE_SUMMARY_ON_ERROR ||
        0 == NumberOfFailedTestCases())
    {
        reporter->onTestFailed(reporterParam, file, line, function, expr, expectedValue, actualValue, length, comparison, verbosity);
    }
}


#ifdef STLSOFT_CF_NAMESPACE_SUPPORT
} // anonymous namespace
#endif /* STLSOFT_CF_NAMESPACE_SUPPORT */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#ifndef _XTESTS_NO_NAMESPACE
} /* namespace c */
} /* namespace xtests */
#endif /* !_XTESTS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Windows debugging support
 */

#if XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_
# undef OutputDebugStringA
# include <windows.h>
static void xtests_OutputDebugStringA_(char const*s)
{
    OutputDebugStringA(s);
}
#endif /* XTESTS_SUPPORT_WINDOWS_OUTPUTDEBUGSTRING_ */

/* ///////////////////////////// end of file //////////////////////////// */
