/* /////////////////////////////////////////////////////////////////////////
 * File:        test/unit/test.unit.be.N/test.unit.be.N.c
 *
 * Purpose:     Implementation file for the test.unit.be.N project.
 *
 * Created:     29th January 2008
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


#include <pantheios/backends/be.N.h>
#include <pantheios/init_codes.h>

#include <xtests/xtests.h>

#include <stlsoft/stlsoft.h>        /* for STLSOFT_NUM_ELEMENTS */

#include <stdio.h>                  /* for fprintf() */
#include <stdlib.h>                 /* for EXIT_SUCCESS, EXIT_FAILURE */

const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.unit.be.N");


#define DECLARE_BACK_END_IMPL_N(n)                                          \
                                                                            \
    PANTHEIOS_CALL(int) pantheios_be_N_test_ ## n ## _init(                 \
        PAN_CHAR_T const*   processIdentity                                 \
    ,   int                 backEndId                                       \
    ,   void const*         init                                            \
    ,   void*               reserved                                        \
    ,   void**              ptoken                                          \
    );                                                                      \
    PANTHEIOS_CALL(void) pantheios_be_N_test_ ## n ## _uninit(void* token); \
    PANTHEIOS_CALL(int) pantheios_be_N_test_ ## n ## _logEntry(             \
        void*               beToken                                         \
    ,   void*               feToken                                         \
    ,   int                 severity                                        \
    ,   PAN_CHAR_T const*   entry                                           \
    ,   size_t              cchEntry                                        \
    );                                                                      \
                                                                            \
    PANTHEIOS_CALL(int) pantheios_be_N_test_ ## n ## _init(                 \
        PAN_CHAR_T const*   processIdentity                                 \
    ,   int                 backEndId                                       \
    ,   void const*         init                                            \
    ,   void*               reserved                                        \
    ,   void**              ptoken                                          \
    )                                                                       \
    {                                                                       \
        STLSOFT_SUPPRESS_UNUSED(processIdentity);                           \
        STLSOFT_SUPPRESS_UNUSED(backEndId);                                 \
        STLSOFT_SUPPRESS_UNUSED(init);                                      \
        STLSOFT_SUPPRESS_UNUSED(reserved);                                  \
        STLSOFT_SUPPRESS_UNUSED(ptoken);                                    \
                                                                            \
        if(s_retVals[n] >= 0)                                               \
        {                                                                   \
            ++s_initCounts[n];                                              \
        }                                                                   \
                                                                            \
        return s_retVals[n];                                                \
    }                                                                       \
                                                                            \
    PANTHEIOS_CALL(void) pantheios_be_N_test_ ## n ## _uninit(void* token)  \
    {                                                                       \
        STLSOFT_SUPPRESS_UNUSED(token);                                     \
                                                                            \
        --s_initCounts[n];                                                  \
    }                                                                       \
                                                                            \
    PANTHEIOS_CALL(int) pantheios_be_N_test_ ## n ## _logEntry(             \
        void*               beToken                                         \
    ,   void*               feToken                                         \
    ,   int                 severity                                        \
    ,   PAN_CHAR_T const*   entry                                           \
    ,   size_t              cchEntry                                        \
    )                                                                       \
    {                                                                       \
        STLSOFT_SUPPRESS_UNUSED(beToken);                                   \
        STLSOFT_SUPPRESS_UNUSED(feToken);                                   \
        STLSOFT_SUPPRESS_UNUSED(severity);                                  \
        STLSOFT_SUPPRESS_UNUSED(entry);                                     \
        STLSOFT_SUPPRESS_UNUSED(cchEntry);                                  \
                                                                            \
        return 0;                                                           \
    }

#define NUM_BACKENDS    (5)

static int s_retVals[1 + NUM_BACKENDS];     /* 0th element is not used. */
static int s_initCounts[1 + NUM_BACKENDS];  /* 0th element is not used. */
static int s_flags[1 + NUM_BACKENDS] =
{
    -1,     /* 0th element is not used. */
    0,
    PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE,
    0,
    0,
    PANTHEIOS_BE_N_F_INIT_ONLY_IF_PREVIOUS_FAILED
};

DECLARE_BACK_END_IMPL_N(1)
DECLARE_BACK_END_IMPL_N(2)
DECLARE_BACK_END_IMPL_N(3)
DECLARE_BACK_END_IMPL_N(4)
DECLARE_BACK_END_IMPL_N(5)

pan_be_N_t PAN_BE_N_BACKEND_LIST[] =
{
    /* NOTE: The flags in this one are ignored, as they're set up every time
     * by the reset_state_() function (below) and the test setup code.
     */

    PANTHEIOS_BE_N_STDFORM_ENTRY(1, pantheios_be_N_test_1, 0),
    PANTHEIOS_BE_N_STDFORM_ENTRY(2, pantheios_be_N_test_2, 0),
    PANTHEIOS_BE_N_STDFORM_ENTRY(3, pantheios_be_N_test_3, 0),
    PANTHEIOS_BE_N_STDFORM_ENTRY(4, pantheios_be_N_test_4, 0),
    PANTHEIOS_BE_N_STDFORM_ENTRY(5, pantheios_be_N_test_5, 0),
    PANTHEIOS_BE_N_TERMINATOR_ENTRY
};


static void reset_state_(void)
{
    int i;
    for(i = 0; i != NUM_BACKENDS; ++i)
    {
        PAN_BE_N_BACKEND_LIST[i].flags  =   s_flags[1 + i]; /* Replace the desired flags */
        s_retVals[i]                    =   0;              /* Mark to pass */
        s_initCounts[0]                 =   0;              /* Reset init ref count */
    }
}

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);


    STLSOFT_STATIC_ASSERT(NUM_BACKENDS + 1 == STLSOFT_NUM_ELEMENTS(s_retVals));
    STLSOFT_STATIC_ASSERT(NUM_BACKENDS + 1 == STLSOFT_NUM_ELEMENTS(s_initCounts));
    STLSOFT_STATIC_ASSERT(NUM_BACKENDS + 1 == STLSOFT_NUM_ELEMENTS(s_flags));


    if(XTESTS_START_RUNNER("test.unit.be.N", verbosity))
    {
        /* Test-1 */
        if(XTESTS_CASE_BEGIN("Test-1", "Verify that all five succeed"))
        {
            void*   token;
            int     res;

            reset_state_();

            res = pantheios_be_init(PANTHEIOS_FE_PROCESS_IDENTITY, NULL, &token);

            XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_INIT_RC_SUCCESS, res);
            XTESTS_TEST_INTEGER_EQUAL(1, s_initCounts[1]);
            XTESTS_TEST_INTEGER_EQUAL(1, s_initCounts[2]);
            XTESTS_TEST_INTEGER_EQUAL(1, s_initCounts[3]);
            XTESTS_TEST_INTEGER_EQUAL(1, s_initCounts[4]);
            XTESTS_TEST_INTEGER_EQUAL(0, s_initCounts[5]);

            pantheios_be_uninit(token);

            XTESTS_CASE_END("Test-1");
        }


        /* Test-2 */
        if(XTESTS_CASE_BEGIN("Test-2", "Verify that #1 failure causes all to fail"))
        {
            void*   token;
            int     res;

            reset_state_();

            s_retVals[1]    =   PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE;

            res = pantheios_be_init(PANTHEIOS_FE_PROCESS_IDENTITY, NULL, &token);

            XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE, res);
            XTESTS_TEST_INTEGER_EQUAL(0, s_initCounts[1]);
            XTESTS_TEST_INTEGER_EQUAL(0, s_initCounts[2]);
            XTESTS_TEST_INTEGER_EQUAL(0, s_initCounts[3]);
            XTESTS_TEST_INTEGER_EQUAL(0, s_initCounts[4]);
            XTESTS_TEST_INTEGER_EQUAL(0, s_initCounts[5]);

            /* pantheios_be_uninit(token); */

            XTESTS_CASE_END("Test-2");
        }


        /* Test-3 */
        if(XTESTS_CASE_BEGIN("Test-3", "Verify that #2 failure is benign"))
        {
            void*   token;
            int     res;

            reset_state_();

            s_retVals[2]    =   PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE;

            res = pantheios_be_init(PANTHEIOS_FE_PROCESS_IDENTITY, NULL, &token);

            XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_INIT_RC_SUCCESS, res);
            XTESTS_TEST_INTEGER_EQUAL(1, s_initCounts[1]);
            XTESTS_TEST_INTEGER_EQUAL(0, s_initCounts[2]);
            XTESTS_TEST_INTEGER_EQUAL(1, s_initCounts[3]);
            XTESTS_TEST_INTEGER_EQUAL(1, s_initCounts[4]);
            XTESTS_TEST_INTEGER_EQUAL(0, s_initCounts[5]);

            pantheios_be_uninit(token);

            XTESTS_CASE_END("Test-3");
        }


        /* Test-4 */
        if(XTESTS_CASE_BEGIN("Test-4", "Verify that failure of 4 out of five, where all ignore init-failure, does not cause failure"))
        {
            void*   token;
            int     res;

            reset_state_();

            PAN_BE_N_BACKEND_LIST[0].flags  =   PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE;
            PAN_BE_N_BACKEND_LIST[1].flags  =   PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE;
            PAN_BE_N_BACKEND_LIST[2].flags  =   PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE;
            PAN_BE_N_BACKEND_LIST[3].flags  =   PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE;
            PAN_BE_N_BACKEND_LIST[4].flags  =   PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE;

            s_retVals[1]                    =   PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE;
            s_retVals[2]                    =   PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE;
            s_retVals[3]                    =   PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE;
            s_retVals[4]                    =   PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE;
            s_retVals[5]                    =   0;

            res = pantheios_be_init(PANTHEIOS_FE_PROCESS_IDENTITY, NULL, &token);

            XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_INIT_RC_SUCCESS, res);
            XTESTS_TEST_INTEGER_EQUAL(0, s_initCounts[1]);
            XTESTS_TEST_INTEGER_EQUAL(0, s_initCounts[2]);
            XTESTS_TEST_INTEGER_EQUAL(0, s_initCounts[3]);
            XTESTS_TEST_INTEGER_EQUAL(0, s_initCounts[4]);
            XTESTS_TEST_INTEGER_EQUAL(1, s_initCounts[5]);

            pantheios_be_uninit(token);

            XTESTS_CASE_END("Test-4");
        }


        /* Test-5 */
        if(XTESTS_CASE_BEGIN("Test-5", "Verify that failure of 5 out of five, where all ignore init-failure, does cause failure"))
        {
            void*   token;
            int     res;

            reset_state_();

            PAN_BE_N_BACKEND_LIST[0].flags  =   PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE;
            PAN_BE_N_BACKEND_LIST[1].flags  =   PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE;
            PAN_BE_N_BACKEND_LIST[2].flags  =   PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE;
            PAN_BE_N_BACKEND_LIST[3].flags  =   PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE;
            PAN_BE_N_BACKEND_LIST[4].flags  =   PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE;

            s_retVals[1]                    =   PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE;
            s_retVals[2]                    =   PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE;
            s_retVals[3]                    =   PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE;
            s_retVals[4]                    =   PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE;
            s_retVals[5]                    =   PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE;

            res = pantheios_be_init(PANTHEIOS_FE_PROCESS_IDENTITY, NULL, &token);

            XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_BE_INIT_RC_ALL_BACKEND_INITS_FAILED, res);
            XTESTS_TEST_INTEGER_EQUAL(0, s_initCounts[1]);
            XTESTS_TEST_INTEGER_EQUAL(0, s_initCounts[2]);
            XTESTS_TEST_INTEGER_EQUAL(0, s_initCounts[3]);
            XTESTS_TEST_INTEGER_EQUAL(0, s_initCounts[4]);
            XTESTS_TEST_INTEGER_EQUAL(0, s_initCounts[5]);

            /* pantheios_be_uninit(token); */

            XTESTS_CASE_END("Test-5");
        }


        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* ///////////////////////////// end of file //////////////////////////// */
