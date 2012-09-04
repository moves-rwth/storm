/* /////////////////////////////////////////////////////////////////////////
 * File:        test/component/test.component.inserters.hostId/test.component.inserters.hostId.cpp
 *
 * Purpose:     Implementation file for the test.component.inserters.hostId project.
 *
 * Created:     14th April 2008
 * Updated:     23rd March 2010
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2008-2010, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

/* xTests Header Files */
#include <xtests/xtests.h>

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>          // Pantheios C++ main header
#include <pantheios/inserters/hostid.hpp>   // for pantheios::hostId
#include <pantheios/backends/bec.test.h>

/* STLSoft Header Files */
#include <stlsoft/conversion/integer_to_string.hpp>
#include <platformstl/platformstl.h>

/* Standard C++ Header Files */
#include <string>

/* Standard C Header Files */
#include <stdlib.h>                         // for exit codes
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
 * Forward declarations
 */

static void test_1_01();

static string_t pan_get_hid_();

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.component.inserters.hostId");

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

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

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.component.inserters.hostId", verbosity))
    {
        XTESTS_RUN_CASE(test_1_01);

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* ////////////////////////////////////////////////////////////////////// */

static void test_1_01()
{
    // 1. Setup

    const PAN_CHAR_T  prefix[]  = PSTR("host: ");
    const string_t    hid       = pan_get_hid_();
    const string_t    stmt      = prefix + hid;



    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::hostId);
    pantheios::log_NOTICE(prefix, pantheios::hostId);


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(2 == results.size());
    XTESTS_TEST_STRING_EQUAL(hid, results[0].statement);
    XTESTS_TEST_STRING_EQUAL(stmt, results[1].statement);
}

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
