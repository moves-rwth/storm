/* /////////////////////////////////////////////////////////////////////////
 * File:        test/unit/test.unit.fe.N/test.unit.fe.N.c
 *
 * Purpose:     Implementation file for the test.unit.fe.N project.
 *
 * Created:     24th August 2008
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


/* /////////////////////////////////////////////////////////////////////////
 * Test component header file include(s)
 */

#include <pantheios/frontends/fe.N.h>

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

/* xTests Header Files */
#include <xtests/xtests.h>

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>

/* Standard C Header Files */
#include <stdlib.h>
#include <string.h>

/* /////////////////////////////////////////////////////////////////////////
 * Forward declarations
 */

static void test_empty_array_with_negative_default_ceiling(void);
static void test_empty_array_with_EMERGENCY_default_ceiling(void);
static void test_empty_array_with_ALERT_default_ceiling(void);
static void test_array_with_one_specific_id(void);
static void test_array_with_three_specific_ids(void);

/* /////////////////////////////////////////////////////////////////////////
 * Main
 */

int main(int argc, char **argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.unit.fe.N", verbosity))
    {
        XTESTS_RUN_CASE(test_empty_array_with_negative_default_ceiling);
        XTESTS_RUN_CASE(test_empty_array_with_EMERGENCY_default_ceiling);
        XTESTS_RUN_CASE(test_empty_array_with_ALERT_default_ceiling);
        XTESTS_RUN_CASE(test_array_with_one_specific_id);
        XTESTS_RUN_CASE(test_array_with_three_specific_ids);

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* /////////////////////////////////////////////////////////////////////////
 * Constants
 */

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

#define s_defaultCeiling    -1

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.unit.fe.N");

const pan_fe_N_t PAN_FE_N_SEVERITY_CEILINGS_DEFAULT[10] =
{
    PANTHEIOS_FE_N_TERMINATOR_ENTRY(s_defaultCeiling),
    PANTHEIOS_FE_N_TERMINATOR_ENTRY(s_defaultCeiling),
    PANTHEIOS_FE_N_TERMINATOR_ENTRY(s_defaultCeiling),
    PANTHEIOS_FE_N_TERMINATOR_ENTRY(s_defaultCeiling),
    PANTHEIOS_FE_N_TERMINATOR_ENTRY(s_defaultCeiling),
    PANTHEIOS_FE_N_TERMINATOR_ENTRY(s_defaultCeiling),
    PANTHEIOS_FE_N_TERMINATOR_ENTRY(s_defaultCeiling),
    PANTHEIOS_FE_N_TERMINATOR_ENTRY(s_defaultCeiling),
    PANTHEIOS_FE_N_TERMINATOR_ENTRY(s_defaultCeiling),
    PANTHEIOS_FE_N_TERMINATOR_ENTRY(s_defaultCeiling),
};

pan_fe_N_t PAN_FE_N_SEVERITY_CEILINGS[10];

static void reinitialise_ceilings(void)
{
    STLSOFT_STATIC_ASSERT(sizeof(PAN_FE_N_SEVERITY_CEILINGS_DEFAULT) == sizeof(PAN_FE_N_SEVERITY_CEILINGS));
    memcpy(&PAN_FE_N_SEVERITY_CEILINGS[0], &PAN_FE_N_SEVERITY_CEILINGS_DEFAULT[0], sizeof(PAN_FE_N_SEVERITY_CEILINGS_DEFAULT));
}

/* /////////////////////////////////////////////////////////////////////////
 * Test function implementations
 */

static void test_empty_array_with_negative_default_ceiling()
{
    static const int severityLevelResults[] =
    {
            0
        ,   0
        ,   0
        ,   0
        ,   0
        ,   0
        ,   0
        ,   0
    };

    void*   token;
    int     res;

    reinitialise_ceilings();
    res = pantheios_fe_init(NULL, &token);

    if(0 != res)
    {
        XTESTS_TEST_FAIL_WITH_QUALIFIER("failed to initialise front-end", pantheios_getInitCodeString(res));
    }
    else
    {
        { size_t i; for(i = 0; i != STLSOFT_NUM_ELEMENTS(s_severityLevels); ++i)
        {
            XTESTS_TEST_INTEGER_EQUAL(severityLevelResults[i], pantheios_fe_isSeverityLogged(token, s_severityLevels[i], 0));
        }}

        pantheios_fe_uninit(token);
    }
}

static void test_empty_array_with_EMERGENCY_default_ceiling()
{
    static const int severityLevelResults[] =
    {
            1
        ,   0
        ,   0
        ,   0
        ,   0
        ,   0
        ,   0
        ,   0
    };
    void*   token;
    int     res;

    reinitialise_ceilings();
    PAN_FE_N_SEVERITY_CEILINGS[0].severityCeiling = PANTHEIOS_SEV_EMERGENCY;

    res = pantheios_fe_init(NULL, &token);

    if(0 != res)
    {
        XTESTS_TEST_FAIL_WITH_QUALIFIER("failed to initialise front-end", pantheios_getInitCodeString(res));
    }
    else
    {
        { size_t i; for(i = 0; i != STLSOFT_NUM_ELEMENTS(s_severityLevels); ++i)
        {
            XTESTS_TEST_INTEGER_EQUAL(severityLevelResults[i], pantheios_fe_isSeverityLogged(token, s_severityLevels[i], 0));
        }}

        pantheios_fe_uninit(token);
    }
}

static void test_empty_array_with_ALERT_default_ceiling()
{
    static const int severityLevelResults[] =
    {
            1
        ,   1
        ,   0
        ,   0
        ,   0
        ,   0
        ,   0
        ,   0
    };
    void*   token;
    int     res;

    reinitialise_ceilings();
    PAN_FE_N_SEVERITY_CEILINGS[0].severityCeiling = PANTHEIOS_SEV_ALERT;

    res = pantheios_fe_init(NULL, &token);

    if(0 != res)
    {
        XTESTS_TEST_FAIL_WITH_QUALIFIER("failed to initialise front-end", pantheios_getInitCodeString(res));
    }
    else
    {
        { size_t i; for(i = 0; i != STLSOFT_NUM_ELEMENTS(s_severityLevels); ++i)
        {
            XTESTS_TEST_INTEGER_EQUAL(severityLevelResults[i], pantheios_fe_isSeverityLogged(token, s_severityLevels[i], 0));
        }}

        pantheios_fe_uninit(token);
    }
}

static void test_array_with_one_specific_id()
{
#define BACKEND_0_ID    1001

    static const int severityLevelResults_for_0[] =
    {
            1
        ,   1
        ,   1
        ,   1
        ,   1
        ,   0
        ,   0
        ,   0
    };
    static const int severityLevelResults_for_N[] =
    {
            1
        ,   1
        ,   1
        ,   1
        ,   0
        ,   0
        ,   0
        ,   0
    };
    static const int severityLevelResults_for_all[] =
    {
            1
        ,   1
        ,   1
        ,   1
        ,   1
        ,   0
        ,   0
        ,   0
    };
    void*   token;
    int     res;

    reinitialise_ceilings();
    PAN_FE_N_SEVERITY_CEILINGS[0].backEndId         =   BACKEND_0_ID;
    PAN_FE_N_SEVERITY_CEILINGS[0].severityCeiling   =   PANTHEIOS_SEV_WARNING;
    PAN_FE_N_SEVERITY_CEILINGS[1].severityCeiling   =   PANTHEIOS_SEV_ERROR;

    res = pantheios_fe_init(NULL, &token);

    if(0 != res)
    {
        XTESTS_TEST_FAIL_WITH_QUALIFIER("failed to initialise front-end", pantheios_getInitCodeString(res));
    }
    else
    {
        { size_t i; for(i = 0; i != STLSOFT_NUM_ELEMENTS(s_severityLevels); ++i)
        {
            XTESTS_TEST_INTEGER_EQUAL(severityLevelResults_for_all[i], pantheios_fe_isSeverityLogged(token, s_severityLevels[i], 0));

            { int id; for(id = 1; id != 1000000; ++id)
            {
                switch(id)
                {
                    case    BACKEND_0_ID:
                        XTESTS_TEST_INTEGER_EQUAL(severityLevelResults_for_0[i], pantheios_fe_isSeverityLogged(token, s_severityLevels[i], id));
                        break;
                    default:
                        XTESTS_TEST_INTEGER_EQUAL(severityLevelResults_for_N[i], pantheios_fe_isSeverityLogged(token, s_severityLevels[i], id));
                        break;
                }
            }}
        }}

        pantheios_fe_uninit(token);
    }
}

static void test_array_with_three_specific_ids()
{
#define BACKEND_0_ID    1001
#define BACKEND_1_ID    3
#define BACKEND_2_ID    54321

    static const int severityLevelResults_for_0[] =
    {
            1
        ,   1
        ,   1
        ,   1
        ,   1
        ,   0
        ,   0
        ,   0
    };
    static const int severityLevelResults_for_1[] =
    {
            1
        ,   1
        ,   1
        ,   1
        ,   1
        ,   1
        ,   1
        ,   0
    };
    static const int severityLevelResults_for_2[] =
    {
            1
        ,   1
        ,   1
        ,   0
        ,   0
        ,   0
        ,   0
        ,   0
    };
    static const int severityLevelResults_for_N[] =
    {
            1
        ,   1
        ,   1
        ,   1
        ,   0
        ,   0
        ,   0
        ,   0
    };
    static const int severityLevelResults_for_all[] =
    {
            1
        ,   1
        ,   1
        ,   1
        ,   1
        ,   1
        ,   1
        ,   0
    };
    void*   token;
    int     res;

    reinitialise_ceilings();
    PAN_FE_N_SEVERITY_CEILINGS[0].backEndId         =   BACKEND_0_ID;
    PAN_FE_N_SEVERITY_CEILINGS[0].severityCeiling   =   PANTHEIOS_SEV_WARNING;
    PAN_FE_N_SEVERITY_CEILINGS[1].backEndId         =   BACKEND_1_ID;
    PAN_FE_N_SEVERITY_CEILINGS[1].severityCeiling   =   PANTHEIOS_SEV_INFORMATIONAL;
    PAN_FE_N_SEVERITY_CEILINGS[2].backEndId         =   BACKEND_2_ID;
    PAN_FE_N_SEVERITY_CEILINGS[2].severityCeiling   =   PANTHEIOS_SEV_CRITICAL;
    PAN_FE_N_SEVERITY_CEILINGS[3].severityCeiling   =   PANTHEIOS_SEV_ERROR;

    res = pantheios_fe_init(NULL, &token);

    if(0 != res)
    {
        XTESTS_TEST_FAIL_WITH_QUALIFIER("failed to initialise front-end", pantheios_getInitCodeString(res));
    }
    else
    {
        { size_t i; for(i = 0; i != STLSOFT_NUM_ELEMENTS(s_severityLevels); ++i)
        {
            XTESTS_TEST_INTEGER_EQUAL(severityLevelResults_for_all[i], pantheios_fe_isSeverityLogged(token, s_severityLevels[i], 0));

            { int id; for(id = 1; id != 1000000; ++id)
            {
                switch(id)
                {
                    case    BACKEND_0_ID:
                        XTESTS_TEST_INTEGER_EQUAL(severityLevelResults_for_0[i], pantheios_fe_isSeverityLogged(token, s_severityLevels[i], id));
                        break;
                    case    BACKEND_1_ID:
                        XTESTS_TEST_INTEGER_EQUAL(severityLevelResults_for_1[i], pantheios_fe_isSeverityLogged(token, s_severityLevels[i], id));
                        break;
                    case    BACKEND_2_ID:
                        XTESTS_TEST_INTEGER_EQUAL(severityLevelResults_for_2[i], pantheios_fe_isSeverityLogged(token, s_severityLevels[i], id));
                        break;
                    default:
                        XTESTS_TEST_INTEGER_EQUAL(severityLevelResults_for_N[i], pantheios_fe_isSeverityLogged(token, s_severityLevels[i], id));
                        break;
                }
            }}
        }}

        pantheios_fe_uninit(token);
    }
}

/* ///////////////////////////// end of file //////////////////////////// */
