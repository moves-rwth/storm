/* /////////////////////////////////////////////////////////////////////////
 * File:        test/unit/test.unit.getversion/test.unit.getversion.c
 *
 * Purpose:     Implementation file for the test.unit.getversion project.
 *
 * Created:     28th August 2008
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
 * Test component header file include(s)
 */

#include <pantheios/pantheios.h>

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

/* xTests Header Files */
#include <xtests/xtests.h>

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>

/* Standard C Header Files */
#include <stdlib.h>

/* /////////////////////////////////////////////////////////////////////////
 * Forward declarations
 */

static void test_signature(void);
static void test_call(void);
static void test_version(void);
static void test_major(void);
static void test_minor(void);
static void test_revision(void);

/* /////////////////////////////////////////////////////////////////////////
 * Main
 */

int main(int argc, char **argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.unit.getversion", verbosity))
    {
        XTESTS_RUN_CASE(test_signature);
        XTESTS_RUN_CASE(test_call);
        XTESTS_RUN_CASE(test_version);
        XTESTS_RUN_CASE(test_major);
        XTESTS_RUN_CASE(test_minor);
        XTESTS_RUN_CASE(test_revision);

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* /////////////////////////////////////////////////////////////////////////
 * Test function implementations
 */

static void test_signature()
{
//  pan_uint32_t (PANTHEIOS_CALL *pfn)(void) = pantheios_getVersion;

    XTESTS_TEST_PASSED();
}

static void test_call()
{
    pantheios_getVersion();

    XTESTS_TEST_PASSED();
}

static void test_version()
{
    pan_uint32_t    ver = pantheios_getVersion();

    XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_VER, ver);
}

static void test_major()
{
    pan_uint32_t    verMajor = (pantheios_getVersion() & 0xff000000) >> 24;

    XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_VER_MAJOR, verMajor);
}

static void test_minor()
{
    pan_uint32_t    verMinor = (pantheios_getVersion() & 0x00ff0000) >> 16;

    XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_VER_MINOR, verMinor);
}

static void test_revision()
{
    pan_uint32_t    verRevision = (pantheios_getVersion() & 0x0000ff00) >> 8;

    XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_VER_REVISION, verRevision);
}

/* ///////////////////////////// end of file //////////////////////////// */
