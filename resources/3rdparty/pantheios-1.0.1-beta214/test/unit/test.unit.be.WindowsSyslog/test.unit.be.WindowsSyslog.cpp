/* /////////////////////////////////////////////////////////////////////////
 * File:        test/unit/test.unit.be.WindowsSyslog/test.unit.be.WindowsSyslog.cpp
 *
 * Purpose:     Implementation file for the test.unit.be.WindowsSyslog project.
 *
 * Created:     17th October 2008
 * Updated:     10th January 2011
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2008-2011, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */



/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

/* Pantheios Header Files */
#include <pantheios/backends/bec.WindowsSyslog.h>

/* xTests Header Files */
#include <xtests/xtests.h>

/* STLSoft Header Files */
#include <stlsoft/smartptr/scoped_handle.hpp>

/* Standard C Header Files */
#include <stdio.h>
#include <stdlib.h>

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

extern "C" const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.unit.be.WindowsSyslog");

/* /////////////////////////////////////////////////////////////////////////
 * Forward declarations
 */

namespace
{

    static void test_getDefaultAppInit(void);
    static void test_initialisation(void);
    static void test_1_2(void);
    static void test_1_3(void);
    static void test_1_4(void);
    static void test_1_5(void);
    static void test_1_6(void);
    static void test_1_7(void);
    static void test_1_8(void);
    static void test_1_9(void);
    static void test_1_10(void);
    static void test_1_11(void);
    static void test_1_12(void);
    static void test_1_13(void);
    static void test_1_14(void);
    static void test_1_15(void);
    static void test_1_16(void);
    static void test_1_17(void);
    static void test_1_18(void);
    static void test_1_19(void);

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * Main
 */

int main(int argc, char **argv)
{
    int     retCode     =   EXIT_SUCCESS;
    int     verbosity   =   2;
    WSADATA wsadata;
    WORD    version     =   MAKEWORD(2, 2);

    int r = ::WSAStartup(version, &wsadata);

    if(0 != r)
    {
        fprintf(stderr, "failed to initialise the WinSock2 library: %d\n", r);

        retCode = EXIT_FAILURE;
    }
    else
    {
        stlsoft::scoped_handle<void>    scoper(::WSACleanup);

        XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

        if(XTESTS_START_RUNNER("test.unit.be.WindowsSyslog", verbosity))
        {
            XTESTS_RUN_CASE(test_getDefaultAppInit);
            XTESTS_RUN_CASE(test_initialisation);
            XTESTS_RUN_CASE(test_1_2);
            XTESTS_RUN_CASE(test_1_3);
            XTESTS_RUN_CASE(test_1_4);
            XTESTS_RUN_CASE(test_1_5);
            XTESTS_RUN_CASE(test_1_6);
            XTESTS_RUN_CASE(test_1_7);
            XTESTS_RUN_CASE(test_1_8);
            XTESTS_RUN_CASE(test_1_9);
            XTESTS_RUN_CASE(test_1_10);
            XTESTS_RUN_CASE(test_1_11);
            XTESTS_RUN_CASE(test_1_12);
            XTESTS_RUN_CASE(test_1_13);
            XTESTS_RUN_CASE(test_1_14);
            XTESTS_RUN_CASE(test_1_15);
            XTESTS_RUN_CASE(test_1_16);
            XTESTS_RUN_CASE(test_1_17);
            XTESTS_RUN_CASE(test_1_18);
            XTESTS_RUN_CASE(test_1_19);
        }

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* /////////////////////////////////////////////////////////////////////////
 * Test function implementations
 */

namespace
{



static void test_getDefaultAppInit()
{
    pan_be_WindowsSyslog_init_t init;

    pantheios_be_WindowsSyslog_getDefaultAppInit(&init);

    XTESTS_TEST_INTEGER_EQUAL(0u, unsigned(init.flags));
    XTESTS_TEST_INTEGER_EQUAL(4u, init.addrSize);
    XTESTS_TEST_INTEGER_EQUAL(255u, init.bytes[0]);
    XTESTS_TEST_INTEGER_EQUAL(255u, init.bytes[1]);
    XTESTS_TEST_INTEGER_EQUAL(255u, init.bytes[2]);
    XTESTS_TEST_INTEGER_EQUAL(255u, init.bytes[3]);
    XTESTS_TEST_POINTER_EQUAL(static_cast<char const*>(NULL), init.hostName);
    XTESTS_TEST_INTEGER_EQUAL(514u, init.port);
    XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_SYSLOG_FAC_USER, init.facility);
}

static void test_initialisation()
{
    pan_be_WindowsSyslog_init_t init;

    pantheios_be_WindowsSyslog_getDefaultAppInit(&init);

    init.flags      |=  PANTHEIOS_BE_WINDOWSSYSLOG_F_NDELAY;
    init.addrSize   =   0;
    init.hostName   =   "localhost";
    init.port       =   50505;

    void*   token;
    int     r = pantheios_be_WindowsSyslog_init(PANTHEIOS_FE_PROCESS_IDENTITY, 1, &init, NULL, &token);

    if(r < 0)
    {
        XTESTS_TEST_FAIL_WITH_QUALIFIER("could not initialise be.WindowsSyslog", pantheios::getStockSeverityString(r));
    }
    else
    {
        stlsoft::scoped_handle<void*> scoper(token, pantheios_be_WindowsSyslog_uninit);

        XTESTS_TEST_PASSED();
    }
}

static void test_1_2()
{
    pan_be_WindowsSyslog_init_t init;

    pantheios_be_WindowsSyslog_getDefaultAppInit(&init);

    init.flags      |=  PANTHEIOS_BE_WINDOWSSYSLOG_F_NDELAY;
    init.addrSize   =   0;
    init.hostName   =   "localhost";
    init.port       =   50505;

    void*   token;
    int     r = pantheios_be_WindowsSyslog_init(PANTHEIOS_FE_PROCESS_IDENTITY, 1, &init, NULL, &token);

    if(r < 0)
    {
        XTESTS_TEST_FAIL_WITH_QUALIFIER("could not initialise be.WindowsSyslog", pantheios::getStockSeverityString(r));
    }
    else
    {
        stlsoft::scoped_handle<void*> scoper(token, pantheios_be_WindowsSyslog_uninit);

        // TODO: create a listening thread, do the statements, stop the thread, verify the elements;

        XTESTS_TEST_PASSED();
    }
}

static void test_1_3()
{
}

static void test_1_4()
{
}

static void test_1_5()
{
}

static void test_1_6()
{
}

static void test_1_7()
{
}

static void test_1_8()
{
}

static void test_1_9()
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

static void test_1_13()
{
}

static void test_1_14()
{
}

static void test_1_15()
{
}

static void test_1_16()
{
}

static void test_1_17()
{
}

static void test_1_18()
{
}

static void test_1_19()
{
}


} // anonymous namespace

/* ///////////////////////////// end of file //////////////////////////// */
