/* /////////////////////////////////////////////////////////////////////////
 * File:        test/unit/test.unit.util.gethostname/test.unit.util.gethostname.cpp
 *
 * Purpose:     Implementation file for the test.unit.util.gethostname project.
 *
 * Created:     14th April 2008
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

/* xTests Header Files */
#include <xtests/xtests.h>

/* Pantheios Header Files */
#include <pantheios/pantheios.h>            // Pantheios C++ main header
#include <pantheios/util/system/hostname.h> // for pantheios::getHostName()
#include <pantheios/backends/bec.test.h>

/* STLSoft Header Files */
#include <pantheios/util/memory/auto_buffer_selector.hpp>
#include <platformstl/platformstl.h>

/* Standard C++ Header Files */
#include <string>

/* Standard C Header Files */
#include <stdlib.h>                     // for exit codes
#if defined(PLATFORMSTL_OS_IS_UNIX)
# include <unistd.h>
#elif defined(PLATFORMSTL_OS_IS_WINDOWS)
# include <windows.h>
#else /* ? OS */
# error Not discriminated for platforms other than UNIX and Windows
#endif /* OS */


#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

typedef std::basic_string<PAN_CHAR_T>   string_t;

/* /////////////////////////////////////////////////////////////////////////
 * Character encoding
 */

#ifdef PANTHEIOS_USE_WIDE_STRINGS

# define pantheios_GetComputerName_     ::GetComputerNameW

# define XTESTS_TEST_STRING_EQUAL       XTESTS_TEST_WIDE_STRING_EQUAL

#else /* ? PANTHEIOS_USE_WIDE_STRINGS */

# define pantheios_GetComputerName_     ::GetComputerNameA

# define XTESTS_TEST_STRING_EQUAL       XTESTS_TEST_MULTIBYTE_STRING_EQUAL

#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* /////////////////////////////////////////////////////////////////////////
 * Forward declarations
 */

static void test_1_01();
static void test_1_02();

static string_t pan_get_hid_();

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.unit.util.gethostname");

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.unit.util.gethostname", verbosity))
    {
        XTESTS_RUN_CASE(test_1_01);
        XTESTS_RUN_CASE(test_1_02);

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* ////////////////////////////////////////////////////////////////////// */

static void test_1_01()
{
    PAN_CHAR_T      hostname[1000];
    const string_t  hid = pan_get_hid_();

    { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(hostname); ++i)
    {
        ::memset(&hostname[0], 0, sizeof(hostname));

        const size_t len = pantheios::getHostName(&hostname[0], i);

        if(len == i)
        {
            // The function did not have enough space to write in, so it
            // will return the length passed to it ...
            XTESTS_TEST_INTEGER_EQUAL(i, len);
            // ... and will not have written anything to the file
            XTESTS_TEST_STRING_EQUAL(PANTHEIOS_LITERAL_STRING(""), hostname);
        }
        else
        {
            // The function had enough space, so it will return the length
            // of the intended hostname ...
            XTESTS_TEST_INTEGER_EQUAL(hid.size(), len);
            // ... and will have written the hostname
            XTESTS_TEST_STRING_EQUAL(hid, hostname);
        }
    }}
}

static void test_1_02()
{
    const string_t hid = pan_get_hid_();

    { for(size_t i = 1; i != 1001; ++i)
    {
        pantheios::util::auto_buffer_selector<PAN_CHAR_T, 256>::type    hostname(i);

        const size_t len = pantheios::getHostName(hostname);

        // The function had enough space, so it will return the length
        // of the intended hostname ...
        XTESTS_TEST_INTEGER_EQUAL(hid.size(), len);
        // ... and will have written the hostname
        XTESTS_TEST_STRING_EQUAL(hid, hostname.data());
    }}
}

/* ////////////////////////////////////////////////////////////////////// */

static string_t pan_get_hid_()
{
#if defined(PLATFORMSTL_OS_IS_UNIX)

    PAN_CHAR_T  szHostName[1001];

    if(0 != ::gethostname(&szHostName[0], STLSOFT_NUM_ELEMENTS(szHostName)))
    {
        return PANTHEIOS_LITERAL_STRING("localhost");
    }
    else
    {
        return szHostName;
    }

#elif defined(PLATFORMSTL_OS_IS_WINDOWS)

    PAN_CHAR_T  szHostName[1001];
    DWORD       cchHostName = STLSOFT_NUM_ELEMENTS(szHostName);

    if(!pantheios_GetComputerName_(&szHostName[0], &cchHostName))
    {
        return PANTHEIOS_LITERAL_STRING("localhost");
    }
    else
    {
        return string_t(szHostName, static_cast<size_t>(cchHostName));
    }

#else /* ? OS */

# error Not discriminated for platforms other than UNIX and Windows

#endif /* OS */
}

/* ///////////////////////////// end of file //////////////////////////// */
