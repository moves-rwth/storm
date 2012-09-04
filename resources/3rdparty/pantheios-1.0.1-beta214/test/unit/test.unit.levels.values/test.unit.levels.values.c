/* /////////////////////////////////////////////////////////////////////////
 * File:        test/unit/test.unit.levels.values/test.unit.levels.values.c
 *
 * Purpose:     Implementation file for the test.unit.levels.values project.
 *
 * Created:     29th November 2007
 * Updated:     7th December 2010
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2007-2010, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#include <pantheios/pantheios.h>

#include <xtests/xtests.h>

#include <stlsoft/stlsoft.h>        /* for STLSOFT_NUM_ELEMENTS */

#include <stdio.h>                  /* for fprintf() */
#include <stdlib.h>                 /* for EXIT_SUCCESS, EXIT_FAILURE */


static const int s_severityLevels[] =
{
        PANTHEIOS_SEV_EMERGENCY
    ,   PANTHEIOS_SEV_ALERT
    ,   PANTHEIOS_SEV_CRITICAL
    ,   PANTHEIOS_SEV_ERROR
    ,   PANTHEIOS_SEV_WARNING
    ,   PANTHEIOS_SEV_NOTICE
    ,   PANTHEIOS_SEV_INFORMATIONAL
    ,   PANTHEIOS_SEV_DEBUG
};

static const int s_severityLevelValues[] =
{
        0
    ,   1
    ,   2
    ,   3
    ,   4
    ,   5
    ,   6
    ,   7
};

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS(s_severityLevels) == STLSOFT_NUM_ELEMENTS(s_severityLevelValues));

    if(XTESTS_START_RUNNER("test.unit.levels.values", verbosity))
    {
        /* Test-1 */
        if(XTESTS_CASE_BEGIN("Test-1", "Severity-level enumerator values"))
        {
            size_t  i;

            for(i = 0; i != STLSOFT_NUM_ELEMENTS(s_severityLevels); ++i)
            {
                XTESTS_TEST_INTEGER_EQUAL(s_severityLevels[i], s_severityLevelValues[i]);
            }

            XTESTS_CASE_END("Test-1");
        }

        /* Test-2 */
        if(XTESTS_CASE_BEGIN("Test-2", "Severity-level enumerator values"))
        {
            size_t  i;

            for(i = 0; i != STLSOFT_NUM_ELEMENTS(s_severityLevels); ++i)
            {
                XTESTS_TEST_INTEGER_EQUAL(s_severityLevels[i], s_severityLevelValues[i]);
            }

            XTESTS_CASE_END("Test-2");
        }

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* ////////////////////////////////////////////////////////////////////// */
