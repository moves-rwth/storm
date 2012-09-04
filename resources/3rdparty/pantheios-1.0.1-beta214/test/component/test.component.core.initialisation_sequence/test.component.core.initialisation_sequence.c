/* /////////////////////////////////////////////////////////////////////////
 * File:        test/component/test.component.core.initialisation_sequence/test.component.core.initialisation_sequence.c
 *
 * Purpose:     Implementation file for the test.component.core.initialisation_sequence project.
 *
 * Created:     8th February 2008
 * Updated:     7th December 2010
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
#include <pantheios/pantheios.h>
#include <pantheios/backend.h>
#include <pantheios/frontend.h>
#include <pantheios/init_codes.h>

/* Standard C Header Files */
#include <stdlib.h>

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

static int  s_feInitValue   =   0;
static int  s_beInitValue   =   0;

/* /////////////////////////////////////////////////////////////////////////
 * Front-end functions
 */

PANTHEIOS_CALL(int) pantheios_fe_init(  void*   reserved
                                    ,   void**  ptoken)
{
    STLSOFT_SUPPRESS_UNUSED(reserved);
    STLSOFT_SUPPRESS_UNUSED(ptoken);

    if(0 != s_feInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("front-end initialisation failed", "front-end has already been initialised");
    }
    else if(0 != s_beInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("front-end initialisation failed", "back-end has already been initialised");
    }
    else
    {
        ++s_feInitValue;

        return 0;
    }

    return PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE;
}

PANTHEIOS_CALL(void) pantheios_fe_uninit(void* token)
{
    STLSOFT_SUPPRESS_UNUSED(token);

    if(0 == s_feInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("front-end uninitialisation failed", "front-end has not yet been initialised");
    }
    else if(0 == s_beInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("front-end uninitialisation failed", "back-end has not yet been initialised");
    }
    else if(1 == s_beInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("front-end uninitialisation failed", "back-end has not yet been uninitialised");
    }
    else if(2 != s_beInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("front-end uninitialisation failed", "back-end initialisation count suggests multiple initialisations");
    }
    else
    {
        ++s_feInitValue;
    }
}

PANTHEIOS_CALL(PAN_CHAR_T const*) pantheios_fe_getProcessIdentity(void* token)
{
    STLSOFT_SUPPRESS_UNUSED(token);

    if(0 == s_feInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("front-end process identity interrogation", "front-end has not yet been initialised");
    }
    else if(1 != s_feInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("front-end process identity interrogation", "front-end has been uninitialised");
    }
    else if(0 != s_beInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("front-end process identity interrogation", "back-end has already been initialised");
    }

    return PANTHEIOS_LITERAL_STRING("test.component.core.initialisation_sequence");
}

PANTHEIOS_CALL(int) pantheios_fe_isSeverityLogged(  void*   token
                                                ,   int     severity
                                                ,   int     backEndId)
{
    STLSOFT_SUPPRESS_UNUSED(token);
    STLSOFT_SUPPRESS_UNUSED(severity);
    STLSOFT_SUPPRESS_UNUSED(backEndId);

    if(0 == s_feInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("front-end severity test", "front-end has not yet been initialised");
    }
    else if(1 != s_feInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("front-end severity test", "front-end has been uninitialised");
    }
    else if(0 == s_beInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("front-end severity test", "back-end has not yet been initialised");
    }
    else if(1 != s_beInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("front-end severity test", "back-end has been uninitialised");
    }

    return 0;
}


/* /////////////////////////////////////////////////////////////////////////
 * Back-end functions
 */

PANTHEIOS_CALL(int) pantheios_be_init(
    PAN_CHAR_T const*   processIdentity
,   void*               reserved
,   void**              ptoken
)
{
    STLSOFT_SUPPRESS_UNUSED(processIdentity);
    STLSOFT_SUPPRESS_UNUSED(reserved);
    STLSOFT_SUPPRESS_UNUSED(ptoken);

    if(0 == s_feInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("back-end initialisation", "front-end has not yet been initialised");
    }
    else if(1 != s_feInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("back-end initialisation", "front-end has been uninitialised");
    }
    else if(0 != s_beInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("back-end initialisation", "back-end has already been initialised");
    }
    else
    {
        ++s_beInitValue;

        return 0;
    }

    return PANTHEIOS_BE_INIT_RC_INTENDED_FAILURE;
}

PANTHEIOS_CALL(void) pantheios_be_uninit(void* token)
{
    STLSOFT_SUPPRESS_UNUSED(token);

    if(0 == s_feInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("back-end uninitialisation failed", "front-end has not yet been initialised");
    }
    else if(0 == s_beInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("back-end uninitialisation failed", "back-end has not yet been initialised");
    }
    else if(1 != s_beInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("back-end uninitialisation failed", "back-end has already been uninitialised");
    }
    else
    {
        ++s_beInitValue;
    }
}

PANTHEIOS_CALL(int) pantheios_be_logEntry(
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

    if(0 == s_feInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("back-end entry logging", "front-end has not yet been initialised");
    }
    else if(1 != s_feInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("back-end entry logging", "front-end has been uninitialised");
    }
    else if(0 == s_beInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("back-end entry logging", "back-end has not been initialised");
    }
    else if(1 != s_beInitValue)
    {
        XTESTS_FAIL_WITH_QUALIFIER("back-end entry logging", "back-end has been uninitialised");
    }

        return (int)cchEntry;
}

/* /////////////////////////////////////////////////////////////////////////
 * main
 */

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.component.core.initialisation_sequence", verbosity))
    {
        if(XTESTS_CASE_BEGIN("Pantheios initialisation sequence", ""))
        {
            int r;

            XTESTS_TEST_INTEGER_EQUAL(0, s_feInitValue);
            XTESTS_TEST_INTEGER_EQUAL(0, s_beInitValue);

            r = pantheios_init();

            if(r < 0)
            {
                XTESTS_FAIL_WITH_QUALIFIER("failed to initialise Pantheios", pantheios_getInitCodeString(r));
            }
            else
            {
                XTESTS_TEST_INTEGER_EQUAL(1, s_feInitValue);
                XTESTS_TEST_INTEGER_EQUAL(1, s_beInitValue);

                pantheios_uninit();

                XTESTS_TEST_INTEGER_EQUAL(2, s_feInitValue);
                XTESTS_TEST_INTEGER_EQUAL(2, s_beInitValue);
            }

            XTESTS_CASE_END("Pantheios initialisation sequence");
        }

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* ///////////////////////////// end of file //////////////////////////// */
