/* /////////////////////////////////////////////////////////////////////////
 * File:        test/unit/test.unit.be.lrsplit/test.unit.be.lrsplit.c
 *
 * Purpose:     Implementation file for the test.unit.be.lrsplit project.
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


#include <pantheios/backends/be.lrsplit.h>
#include <pantheios/init_codes.h>

#include <xtests/xtests.h>

#include <stlsoft/stlsoft.h>        /* for STLSOFT_NUM_ELEMENTS */

#include <stdio.h>                  /* for fprintf() */
#include <stdlib.h>                 /* for EXIT_SUCCESS, EXIT_FAILURE */

const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.unit.be.lrsplit");



static int s_localRetVal    =   PANTHEIOS_INIT_RC_SUCCESS;
static int s_remoteRetVal   =   PANTHEIOS_INIT_RC_SUCCESS;


PANTHEIOS_CALL(int) pantheios_be_local_init(
    PAN_CHAR_T const*   processIdentity
,   void*               reserved
,   void**              ptoken
)
{
    STLSOFT_SUPPRESS_UNUSED(processIdentity);
    STLSOFT_SUPPRESS_UNUSED(reserved);
    STLSOFT_SUPPRESS_UNUSED(ptoken);

    return s_localRetVal;
}

PANTHEIOS_CALL(void) pantheios_be_local_uninit(void* token)
{
    STLSOFT_SUPPRESS_UNUSED(token);
}

PANTHEIOS_CALL(int) pantheios_be_local_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
)
{
    STLSOFT_SUPPRESS_UNUSED(feToken);
    STLSOFT_SUPPRESS_UNUSED(beToken);
    STLSOFT_SUPPRESS_UNUSED(severity);
    STLSOFT_SUPPRESS_UNUSED(entry);

    return (int)cchEntry;
}


PANTHEIOS_CALL(int) pantheios_be_remote_init(
    PAN_CHAR_T const*   processIdentity
,   void*               reserved
,   void**              ptoken
)
{
    STLSOFT_SUPPRESS_UNUSED(processIdentity);
    STLSOFT_SUPPRESS_UNUSED(reserved);
    STLSOFT_SUPPRESS_UNUSED(ptoken);

    return s_remoteRetVal;
}

PANTHEIOS_CALL(void) pantheios_be_remote_uninit(void* token)
{
    STLSOFT_SUPPRESS_UNUSED(token);
}

PANTHEIOS_CALL(int) pantheios_be_remote_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
)
{
    STLSOFT_SUPPRESS_UNUSED(feToken);
    STLSOFT_SUPPRESS_UNUSED(beToken);
    STLSOFT_SUPPRESS_UNUSED(severity);
    STLSOFT_SUPPRESS_UNUSED(entry);

    return (int)cchEntry;
}




PANTHEIOS_CALL(void) pantheios_onBailOut3(int severity, char const* message, char const* processId)
{
    STLSOFT_SUPPRESS_UNUSED(severity);
    STLSOFT_SUPPRESS_UNUSED(message);
    STLSOFT_SUPPRESS_UNUSED(processId);
}

PANTHEIOS_CALL(void) pantheios_onBailOut4(int severity, char const* message, char const* processId, char const* qualifier)
{
    STLSOFT_SUPPRESS_UNUSED(severity);
    STLSOFT_SUPPRESS_UNUSED(message);
    STLSOFT_SUPPRESS_UNUSED(processId);
    STLSOFT_SUPPRESS_UNUSED(qualifier);
}

PANTHEIOS_CALL(int) pantheios_isInitialising(void)
{
    return 1;
}

PANTHEIOS_CALL(int) pantheios_isInitialised(void)
{
    return 0;
}




int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.unit.be.lrsplit", verbosity))
    {
        /* Test-1 */
        if(XTESTS_CASE_BEGIN("Test-1", "Verify that both succeed"))
        {
            void*   token;
            int     res;

            s_localRetVal    =   PANTHEIOS_INIT_RC_SUCCESS;
            s_remoteRetVal   =   PANTHEIOS_INIT_RC_SUCCESS;

            res = pantheios_be_init(PANTHEIOS_FE_PROCESS_IDENTITY, NULL, &token);

            XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_INIT_RC_SUCCESS, res);

            pantheios_be_uninit(token);


            XTESTS_CASE_END("Test-1");
        }

        /* Test-2 */
        if(XTESTS_CASE_BEGIN("Test-2", "Verify that only local can succeed"))
        {
            void*   token;
            int     res;


            s_localRetVal   =   PANTHEIOS_INIT_RC_SUCCESS;
            s_remoteRetVal  =   PANTHEIOS_BE_INIT_RC_INTENDED_FAILURE;

            res = pantheios_be_init(PANTHEIOS_FE_PROCESS_IDENTITY, NULL, &token);

            XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_BE_INIT_RC_INTENDED_FAILURE, res);

            /* pantheios_be_uninit(token); */


            XTESTS_CASE_END("Test-2");
        }

        /* Test-3 */
        if(XTESTS_CASE_BEGIN("Test-3", "Verify that only remote can succeed"))
        {
            void*   token;
            int     res;


            s_localRetVal   =   PANTHEIOS_BE_INIT_RC_INTENDED_FAILURE;
            s_remoteRetVal  =   PANTHEIOS_INIT_RC_SUCCESS;

            res = pantheios_be_init(PANTHEIOS_FE_PROCESS_IDENTITY, NULL, &token);

            XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_BE_INIT_RC_INTENDED_FAILURE, res);

            /* pantheios_be_uninit(token); */


            XTESTS_CASE_END("Test-3");
        }

        /* Test-4 */
        if(XTESTS_CASE_BEGIN("Test-4", "Verify that neither can succeed"))
        {
            void*   token;
            int     res;


            s_localRetVal   =   PANTHEIOS_BE_INIT_RC_INTENDED_FAILURE;
            s_remoteRetVal  =   PANTHEIOS_INIT_RC_SUCCESS;

            res = pantheios_be_init(PANTHEIOS_FE_PROCESS_IDENTITY, NULL, &token);

            XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_BE_INIT_RC_INTENDED_FAILURE, res);

            /* pantheios_be_uninit(token); */


            XTESTS_CASE_END("Test-4");
        }

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* ///////////////////////////// end of file //////////////////////////// */
