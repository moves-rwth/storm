/* /////////////////////////////////////////////////////////////////////////
 * File:        test/component/test.component.inserters.processId/test.component.inserters.processId.cpp
 *
 * Purpose:     Implementation file for the test.component.inserters.processId project.
 *
 * Created:     17th October 2006
 * Updated:     23rd March 2010
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2006-2010, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

/* xTests Header Files */
#include <xtests/xtests.h>

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>      // Pantheios C++ main header
#include <pantheios/inserters/ids.hpp>  // for pantheios::processId
#include <pantheios/backends/bec.test.h>

/* STLSoft Header Files */
#include <stlsoft/conversion/integer_to_string.hpp>
#include <platformstl/platformstl.h>

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
 * Forward declarations
 */

static void test_1_01();

static int pan_get_pid_();

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.component.inserters.processId");

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* /////////////////////////////////////////////////////////////////////////
 * Character encoding
 */

#ifdef PANTHEIOS_USE_WIDE_STRINGS

# define XTESTS_TEST_STRING_EQUAL       XTESTS_TEST_WIDE_STRING_EQUAL

#else /* ? PANTHEIOS_USE_WIDE_STRINGS */

# define XTESTS_TEST_STRING_EQUAL       XTESTS_TEST_MULTIBYTE_STRING_EQUAL

#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.component.inserters.processId", verbosity))
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

    const PAN_CHAR_T  prefix[]    =   PSTR("process: ");
    PAN_CHAR_T        pid_[21 + STLSOFT_NUM_ELEMENTS(prefix)];
    PAN_CHAR_T const* pid         =   stlsoft::integer_to_string(&pid_[0], STLSOFT_NUM_ELEMENTS(pid_), pan_get_pid_());
    PAN_CHAR_T const* stmt        =   pid - (STLSOFT_NUM_ELEMENTS(prefix) - 1);

#ifdef PANTHEIOS_USE_WIDE_STRINGS
    ::wcsncpy(const_cast<PAN_CHAR_T*>(stmt), prefix, (STLSOFT_NUM_ELEMENTS(prefix) - 1));
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    ::strncpy(const_cast<PAN_CHAR_T*>(stmt), prefix, (STLSOFT_NUM_ELEMENTS(prefix) - 1));
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::processId);
    pantheios::log_NOTICE(prefix, pantheios::processId);


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST_BOOLEAN_FALSE(results.empty());
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(2u, results.size()));
    XTESTS_TEST_STRING_EQUAL(pid, results[0].statement);
    XTESTS_TEST_STRING_EQUAL(stmt, results[1].statement);
}

static int pan_get_pid_()
{
#if defined(PLATFORMSTL_OS_IS_UNIX)
    return static_cast<int>(::getpid());
#elif defined(PLATFORMSTL_OS_IS_WINDOWS)
    return static_cast<int>(::GetCurrentProcessId());
#else /* ? OS */
# error Not discriminated for platforms other than UNIX and Windows
#endif /* OS */
}

/* ///////////////////////////// end of file //////////////////////////// */
