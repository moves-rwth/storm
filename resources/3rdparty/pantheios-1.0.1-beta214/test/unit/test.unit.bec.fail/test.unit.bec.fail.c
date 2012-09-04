/* /////////////////////////////////////////////////////////////////////////
 * File:        test/unit/test.unit.bec.fail/test.unit.bec.fail.c
 *
 * Purpose:     Implementation file for the test.unit.bec.fail project.
 *
 * Created:     27th January 2008
 * Updated:     22nd March 2010
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


#include <pantheios/backends/bec.fail.h>
#include <pantheios/init_codes.h>

#include <xtests/xtests.h>

#include <stlsoft/stlsoft.h>        /* for STLSOFT_NUM_ELEMENTS */

#include <stdio.h>                  /* for fprintf() */
#include <stdlib.h>                 /* for EXIT_SUCCESS, EXIT_FAILURE */

const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.unit.bec.fail");

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.unit.bec.fail", verbosity))
    {
        /* Test-1 */
        if(XTESTS_CASE_BEGIN("Test-1", "Verify that it fails"))
        {
            void*   token;
            int     res = pantheios_be_fail_init(PANTHEIOS_FE_PROCESS_IDENTITY, 0, NULL, NULL, &token);

            XTESTS_TEST_INTEGER_LESS(0, res);
            XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_BE_INIT_RC_INTENDED_FAILURE, res);

            XTESTS_CASE_END("Test-1");
        }

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* ///////////////////////////// end of file //////////////////////////// */
