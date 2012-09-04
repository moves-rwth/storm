/* /////////////////////////////////////////////////////////////////////////
 * File:        test/unit/test.unit.util.onbailout/test.unit.util.onbailout.cpp
 *
 * Purpose:     Implementation file for the test.unit.util.onbailout project.
 *
 * Created:     29th April 2008
 * Updated:     6th August 2012
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2008-2012, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

#define PANTHEIOS_NO_NAMESPACE

#define PANTHEIOS_BAILOUT_NO_OPERATING_SYSTEM_SPECIFICS
#define PANTHEIOS_BAILOUT_NO_UNIX_SYSLOG
#define PANTHEIOS_BAILOUT_NO_WINDOWS_EVENTLOG

#ifdef UNIX
# define _WINSOCKAPI_
# define _WINSOCK2API_
# define _WINSOCK2_H
#endif

#include <pantheios/pantheios.h>
#include <pantheios/internal/lean.h>
#include <pantheios/init_codes.h>

#include <xtests/xtests.h>

#include <shwild/shwild.hpp>

#include <stlsoft/shims/access/string/fwd.h>
#include <stlsoft/shims/access/string/std/basic_string.hpp>
#include <platformstl/platformstl.h>

#include <string>
#include <vector>

#include <stdlib.h>
#if defined(PLATFORMSTL_OS_IS_UNIX)
# undef _WINSOCKAPI_
# undef _WINSOCK2API_
# undef _WINSOCK2_H
# include <sys/time.h>                          // for gettimeofday()
#endif /* PLATFORMSTL_OS_IS_UNIX */

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* /////////////////////////////////////////////////////////////////////////
 * xTests extensions
 */

#define XTESTS_TEST_MULTIBYTE_STRING_MATCHES(pattern, value)        \
                                                                    \
    ((0 == shwild::match(pattern, stlsoft::c_str_ptr(value), 0))    \
        ?   XTESTS_TEST_PASSED()                                    \
        :   XTESTS_TEST_FAIL_WITH_QUALIFIER(std::string("the actual value did not match the pattern '") + pattern + "'", value))

/* /////////////////////////////////////////////////////////////////////////
 * Forward declarations
 */

namespace
{

static void test_1_01();
static void test_1_02();
static void test_1_03();
static void test_1_04();
static void test_1_05();
static void test_1_06();
static void test_1_07();
static void test_1_08();
static void test_1_09();
static void test_1_10();
static void test_1_11();
static void test_1_12();

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

namespace
{

    class OnBailOutReceiver
    {
    public:
        typedef OnBailOutReceiver           class_type;
        typedef std::string                 string_type;
        struct entry_type
        {
            string_type message;
            string_type processId;
            string_type qualifier;

            entry_type(string_type const& msg, string_type const& prId, string_type const& qual)
                : message(msg)
                , processId(prId)
                , qualifier(qual)
            {}
        };
        typedef std::vector<entry_type>     entries_type;

    public:
        void Clear()
        {
            entries_type().swap(ConsoleResults);
#if defined(PLATFORMSTL_OS_IS_WINDOWS) && \
    !defined(PANTHEIOS_BAILOUT_NO_WINDOWS_EVENTLOG)
            entries_type().swap(EventLogResults);
#endif /* PLATFORMSTL_OS_IS_WINDOWS */
            entries_type().swap(LoggingBailoutTxtResults);
#if defined(PLATFORMSTL_OS_IS_WINDOWS)
            entries_type().swap(OutputDebugStringResults);
#endif /* PLATFORMSTL_OS_IS_WINDOWS */
#if defined(PLATFORMSTL_OS_IS_UNIX) && \
    !defined(PANTHEIOS_BAILOUT_NO_UNIX_SYSLOG)
            entries_type().swap(SyslogResults);
#endif /* PLATFORMSTL_OS_IS_UNIX */
        }

        void onfprintf(string_type const& msg)
        {
            ConsoleResults.push_back(entry_type(msg, "", ""));
        }

#if defined(PLATFORMSTL_OS_IS_WINDOWS)
        void onOutputDebugString(char const* s)
        {
            OutputDebugStringResults.push_back(entry_type(s, "", ""));
        }
#endif /* PLATFORMSTL_OS_IS_WINDOWS */

    public:
        entries_type        ConsoleResults;
#if defined(PLATFORMSTL_OS_IS_WINDOWS) && \
    !defined(PANTHEIOS_BAILOUT_NO_WINDOWS_EVENTLOG)
        entries_type        EventLogResults;
#endif /* PLATFORMSTL_OS_IS_WINDOWS */
        entries_type        LoggingBailoutTxtResults;
#if defined(PLATFORMSTL_OS_IS_WINDOWS)
        entries_type        OutputDebugStringResults;
#endif /* PLATFORMSTL_OS_IS_WINDOWS */
#if defined(PLATFORMSTL_OS_IS_UNIX) && \
    !defined(PANTHEIOS_BAILOUT_NO_UNIX_SYSLOG)
        entries_type        SyslogResults;
#endif /* PLATFORMSTL_OS_IS_UNIX */
    };

    OnBailOutReceiver   receiver;

} // anonymous namespace

/* ////////////////////////////////////////////////////////////////////// */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.unit.util.onbailout");

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER(PANTHEIOS_FE_PROCESS_IDENTITY, verbosity))
    {
        XTESTS_RUN_CASE(test_1_01);
        XTESTS_RUN_CASE(test_1_02);
        XTESTS_RUN_CASE(test_1_03);
        XTESTS_RUN_CASE(test_1_04);
        XTESTS_RUN_CASE(test_1_05);
        XTESTS_RUN_CASE(test_1_06);
        XTESTS_RUN_CASE(test_1_07);
        XTESTS_RUN_CASE(test_1_08);
        XTESTS_RUN_CASE(test_1_09);
        XTESTS_RUN_CASE(test_1_10);
        XTESTS_RUN_CASE(test_1_11);
        XTESTS_RUN_CASE(test_1_12);

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* ////////////////////////////////////////////////////////////////////// */

namespace
{

static void test_1_01()
{
    receiver.Clear();

    XTESTS_TEST_INTEGER_EQUAL(0u, receiver.ConsoleResults.size());
#if defined(PLATFORMSTL_OS_IS_WINDOWS) && \
    !defined(PANTHEIOS_BAILOUT_NO_WINDOWS_EVENTLOG)
    XTESTS_TEST_INTEGER_EQUAL(0u, receiver.EventLogResults.size());
#endif /* PLATFORMSTL_OS_IS_WINDOWS */
    XTESTS_TEST_INTEGER_EQUAL(0u, receiver.LoggingBailoutTxtResults.size());
#if defined(PLATFORMSTL_OS_IS_WINDOWS)
    XTESTS_TEST_INTEGER_EQUAL(0u, receiver.OutputDebugStringResults.size());
#endif /* PLATFORMSTL_OS_IS_WINDOWS */
#if defined(PLATFORMSTL_OS_IS_UNIX) && \
    !defined(PANTHEIOS_BAILOUT_NO_UNIX_SYSLOG)
    XTESTS_TEST_INTEGER_EQUAL(0u, receiver.SyslogResults.size());
#endif /* PLATFORMSTL_OS_IS_UNIX */
}

static void test_1_02()
{
    receiver.Clear();

    const char  message[]   =   "hello";
    const char  pattern[]   =   "2[0-9][0-9][0-9][0-9][0-9][0-9][0-9]-[0-9][0-9][0-9][0-9][0-9][0-9].[0-9][0-9][0-9]: hello";

    pantheios_onBailOut3(PANTHEIOS_SEV_DEBUG, message, "process-#1");

    XTESTS_TEST_INTEGER_EQUAL(1u, receiver.ConsoleResults.size());
    XTESTS_TEST_MULTIBYTE_STRING_MATCHES(pattern, receiver.ConsoleResults[0].message);
#if defined(PLATFORMSTL_OS_IS_WINDOWS) && \
    !defined(PANTHEIOS_BAILOUT_NO_WINDOWS_EVENTLOG)
    XTESTS_TEST_INTEGER_EQUAL(1u, receiver.EventLogResults.size());
#endif /* PLATFORMSTL_OS_IS_WINDOWS */
//  XTESTS_TEST_INTEGER_EQUAL(1u, receiver.LoggingBailoutTxtResults.size());
#if defined(PLATFORMSTL_OS_IS_WINDOWS)
    XTESTS_TEST_INTEGER_EQUAL(1u, receiver.OutputDebugStringResults.size());
    XTESTS_TEST_MULTIBYTE_STRING_MATCHES(pattern, receiver.ConsoleResults[0].message);
#endif /* PLATFORMSTL_OS_IS_WINDOWS */
#if defined(PLATFORMSTL_OS_IS_UNIX) && \
    !defined(PANTHEIOS_BAILOUT_NO_UNIX_SYSLOG)
    XTESTS_TEST_INTEGER_EQUAL(1u, receiver.SyslogResults.size());
#endif /* PLATFORMSTL_OS_IS_UNIX */
}

static void test_1_03()
{
}

static void test_1_04()
{
}

static void test_1_05()
{
}

static void test_1_06()
{
}

static void test_1_07()
{
}

static void test_1_08()
{
}

static void test_1_09()
{
}

static void test_1_10()
{
}

static void test_1_11()
{
}

static void test_1_12()
{
}

} // anonymous namespace

/* ////////////////////////////////////////////////////////////////////// */

#include <stdio.h>

#if defined(PLATFORMSTL_OS_IS_UNIX)

# include <unistd.h>

# define fprintf                bailout_test_fprintf_
void bailout_test_fprintf_(FILE*, char const*, int, char const*);

# define fopen(fileName, mode)  bailout_test_fopen_()
FILE* bailout_test_fopen_(void);

#endif /* PLATFORMSTL_OS_IS_UNIX */

#if defined(PLATFORMSTL_OS_IS_WINDOWS)

# include <windows.h>

# define OutputDebugStringA     bailout_test_OutputDebugStringA_
void bailout_test_OutputDebugStringA_(char const*);

# define WriteFile              bailout_test_WriteFile_
void bailout_test_WriteFile_(FILE*, char const*, DWORD, DWORD*, void*);
void bailout_test_WriteFile_(HANDLE, char const*, DWORD, DWORD*, void*);

# define GetStdHandle(x)        bailout_test_GetStdHandle_()
FILE* bailout_test_GetStdHandle_(void);

# define CreateFileA            bailout_test_CreateFileA
HANDLE bailout_test_CreateFileA(char const*, DWORD, DWORD, void*, DWORD, DWORD, void*);

#endif /* PLATFORMSTL_OS_IS_WINDOWS */

#include <../src/util/bailout.c>
#include <../src/util/snprintf.c>

/* ////////////////////////////////////////////////////////////////////// */

#if defined(PLATFORMSTL_OS_IS_WINDOWS)

void bailout_test_OutputDebugStringA_(char const* msg)
{
    receiver.onOutputDebugString(msg);
}

void bailout_test_WriteFile_(FILE*, char const* msg, DWORD cchMsg, DWORD*, void*)
{
    receiver.onfprintf(OnBailOutReceiver::string_type(msg, cchMsg - 2));
}

void bailout_test_WriteFile_(HANDLE, char const*, DWORD, DWORD*, void*)
{
}

FILE* bailout_test_GetStdHandle_(void)
{
    return NULL;
}

HANDLE bailout_test_CreateFileA(char const*, DWORD, DWORD, void*, DWORD, DWORD, void*)
{
    return INVALID_HANDLE_VALUE;
}

#endif /* PLATFORMSTL_OS_IS_WINDOWS */

#if defined(PLATFORMSTL_OS_IS_UNIX)

void bailout_test_fprintf_(FILE*, char const*, int cchMsg, char const* msg)
{
    receiver.onfprintf(OnBailOutReceiver::string_type(msg, size_t(cchMsg)));
}

FILE* bailout_test_fopen_(void)
{
    return NULL;
}

#endif /* PLATFORMSTL_OS_IS_UNIX */


/* ////////////////////////////////////////////////////////////////////// */
