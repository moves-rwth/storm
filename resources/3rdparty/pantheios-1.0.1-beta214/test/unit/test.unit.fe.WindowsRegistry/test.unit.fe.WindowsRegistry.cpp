/* /////////////////////////////////////////////////////////////////////////
 * File:        test/unit/test.unit.fe.WindowsRegistry/test.unit.fe.WindowsRegistry.cpp
 *
 * Purpose:     Implementation file for the test.unit.fe.WindowsRegistry project.
 *
 * Created:     14th May 2008
 * Updated:     4th January 2011
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


#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

#include <pantheios/frontends/fe.WindowsRegistry.h>
#include <pantheios/init_codes.h>

#include <xtests/xtests.h>

#include <stlsoft/stlsoft.h>                        /* for STLSOFT_NUM_ELEMENTS */
#include <stlsoft/conversion/integer_to_string.hpp> /* stlsoft::integer_to_string */
#include <stlsoft/synch/lock_scope.hpp>             /* stlsoft::lock_scope */
#include <winstl/synch/process_mutex.hpp>           /* winstl::process_mutex */
#include <winstl/registry/reg_key.hpp>              /* for Windows Registry Library reg_key class */

#include <stdio.h>                                  /* for fprintf() */
#include <stdlib.h>                                 /* for EXIT_SUCCESS, EXIT_FAILURE */

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* /////////////////////////////////////////////////////////////////////////
 * Character encoding
 */

#ifdef PANTHEIOS_USE_WIDE_STRINGS

# define XTESTS_TEST_STRING_EQUAL       XTESTS_TEST_WIDE_STRING_EQUAL

#else /* ? PANTHEIOS_USE_WIDE_STRINGS */

# define XTESTS_TEST_STRING_EQUAL       XTESTS_TEST_MULTIBYTE_STRING_EQUAL

#endif /* PANTHEIOS_USE_WIDE_STRINGS */

#ifndef XTESTS_TEST_NOTICE
# define XTESTS_TEST_NOTICE(x)          ::OutputDebugString(PANTHEIOS_LITERAL_STRING(x))
#endif // XTESTS_TEST_NOTICE

#ifdef PSTR
# undef PSTR
#endif
#define PSTR                            PANTHEIOS_LITERAL_STRING

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.unit.fe.WindowsRegistry");

/* /////////////////////////////////////////////////////////////////////////
 * Forward declarations
 */

namespace
{

    static void test_can_init(void);
    static void test_cannot_init(void);
    static void test_levels_in_HKCU(void);
    static void test_levels_in_HKLM(void);
    static void test_HKCU_overrides_HKLM(void);
    static void test_star(void);
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
    static void test_cleanup(void);

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * Main
 */

static int main_(int argc, char** argv)
{
    // Lock out other test program instances, otherwise they'll interfere
    // with each other

    winstl::process_mutex                       mx("test.unit.fe.WindowsRegistry.MX");
    stlsoft::lock_scope<winstl::process_mutex>  lock(mx);

    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.unit.fe.WindowsRegistry", verbosity))
    {
        XTESTS_RUN_CASE(test_can_init);
        XTESTS_RUN_CASE(test_cannot_init);
        XTESTS_RUN_CASE(test_levels_in_HKCU);
        XTESTS_RUN_CASE(test_levels_in_HKLM);
        XTESTS_RUN_CASE(test_HKCU_overrides_HKLM);
        XTESTS_RUN_CASE(test_star);
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
        XTESTS_RUN_CASE(test_cleanup);

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

int main(int argc, char** argv)
{
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return main_(argc, argv);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc&)
    {
        fprintf(stderr, "out of memory\n");
    }
    catch(std::exception& x)
    {
        fprintf(stderr, "exception: %s\n", x.what());
    }
    catch(...)
    {
        fprintf(stderr, "unexpected condition\n");
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

    return EXIT_FAILURE;
}

namespace stub
{

    PANTHEIOS_CALL(void) pantheios_onBailOut3(int severity, char const* message, char const* processId);
    PANTHEIOS_CALL(void) pantheios_onBailOut3(int severity, char const* message, char const* processId)
    {
#if 0
        fprintf(stdout, "%d: %s: %s", severity, (NULL != processId) ? processId : "", message);
        fprintf(stderr, "%d: %s: %s", severity, (NULL != processId) ? processId : "", message);
#else /* ? 0 */
        STLSOFT_SUPPRESS_UNUSED(severity);
        STLSOFT_SUPPRESS_UNUSED(message);
        STLSOFT_SUPPRESS_UNUSED(processId);
#endif /* 0 */
    }

} /* namespace stub */

/* /////////////////////////////////////////////////////////////////////////
 * Test function implementations
 */

namespace
{
    static const PAN_CHAR_T ROOT_KEY_PATH[] = PANTHEIOS_LITERAL_STRING("SOFTWARE\\Synesis Software\\Logging Tools\\Pantheios\\fe.WindowsRegistry");

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


    static void ensure_HKCU(int debugLevel, int releaseLevel, int starLevel)
    {
        winstl::reg_key     key0 = winstl::reg_key::create_key(HKEY_CURRENT_USER, ROOT_KEY_PATH);
        winstl::reg_key     key = key0.create_sub_key(PANTHEIOS_FE_PROCESS_IDENTITY);

        if(debugLevel >= 0)
        {
            key.set_value(PSTR("Debug"), DWORD(debugLevel));
        }
        if(releaseLevel >= 0)
        {
            key.set_value(PSTR("Release"), DWORD(releaseLevel));
        }
        if(starLevel >= 0)
        {
            key.set_value(PSTR("*"), DWORD(starLevel));
        }
    }

    static bool ensure_HKLM(int debugLevel, int releaseLevel, int starLevel)
    {
        try
        {
            winstl::reg_key     key0 = winstl::reg_key::create_key(HKEY_LOCAL_MACHINE, ROOT_KEY_PATH);
            winstl::reg_key     key = key0.create_sub_key(PANTHEIOS_FE_PROCESS_IDENTITY);

            if(debugLevel >= 0)
            {
                key.set_value(PSTR("Debug"), DWORD(debugLevel));
            }
            if(releaseLevel >= 0)
            {
                key.set_value(PSTR("Release"), DWORD(releaseLevel));
            }
            if(starLevel >= 0)
            {
                key.set_value(PSTR("*"), DWORD(starLevel));
            }

            return true;
        }
        catch(winstl::windows_exception& x)
        {
            if(ERROR_ACCESS_DENIED == x.get_error_code())
            {
                return false;
            }
            else
            {
                throw;
            }
        }
    }

    static void delete_HKCU()
    {
        winstl::reg_key     key0 = winstl::reg_key::create_key(HKEY_CURRENT_USER, ROOT_KEY_PATH);

        key0.delete_sub_key(PANTHEIOS_FE_PROCESS_IDENTITY);
    }

    static bool delete_HKLM()
    {
        try
        {
            winstl::reg_key     key0 = winstl::reg_key::create_key(HKEY_LOCAL_MACHINE, ROOT_KEY_PATH);

            key0.delete_sub_key(PANTHEIOS_FE_PROCESS_IDENTITY);

            return true;
        }
        catch(winstl::windows_exception& x)
        {
            if(ERROR_ACCESS_DENIED == x.get_error_code())
            {
                return false;
            }
            else
            {
                throw;
            }
        }
    }

static void test_can_init()
{
    // Ensure that the HKCU key exists
    ensure_HKCU(0, 0, 0);
    if(!delete_HKLM())
    {
        XTESTS_TEST_NOTICE("could not change HKEY_LOCAL_MACHINE, so skipping test case");

        return;
    }

    void*   token;
    int     res;

    res = pantheios_fe_init(NULL, &token);

    XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_INIT_RC_SUCCESS, res);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        char    num[21];

        XTESTS_TEST_FAIL_WITH_QUALIFIER("failed to initialise front-end", stlsoft::integer_to_string(&num[0], STLSOFT_NUM_ELEMENTS(num), res));
    }
    else
    {
        XTESTS_TEST_PASSED();

        pantheios_fe_uninit(token);
    }
}

static void test_cannot_init()
{
    // Ensure that neither the HKCU key nor the HKLM key exist
    delete_HKCU();
    if(!delete_HKLM())
    {
        XTESTS_TEST_NOTICE("could not change HKEY_LOCAL_MACHINE, so skipping test case");

        return;
    }

    void*   token;
    int     res;

    res = pantheios_fe_init(NULL, &token);

    XTESTS_TEST_INTEGER_NOT_EQUAL(PANTHEIOS_INIT_RC_SUCCESS, res);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_TEST_PASSED();
    }
    else
    {
        XTESTS_TEST_FAIL("unexpectedly able to initialise front-end");

        pantheios_fe_uninit(token);
    }
}

static void test_levels_in_HKCU()
{
    // Ensure that the HKCU key exists, and has the right levels
    ensure_HKCU(0xff, 0x3f, -1);
    if(!delete_HKLM())
    {
        XTESTS_TEST_NOTICE("could not change HKEY_LOCAL_MACHINE, so skipping test case");

        return;
    }

    static const int s_severityLevelResults[] =
    {
            1
        ,   1
        ,   1
        ,   1
        ,   1
        ,   1
#ifdef NDEBUG
        ,   0
        ,   0
#else /* ? NDEBUG */
        ,   1
        ,   1
#endif /* NDEBUG */
    };

    STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS(s_severityLevels) == STLSOFT_NUM_ELEMENTS(s_severityLevelResults));

    void*   token;
    int     res;

    res = pantheios_fe_init(NULL, &token);

    XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_INIT_RC_SUCCESS, res);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        char    num[21];

        XTESTS_TEST_FAIL_WITH_QUALIFIER("failed to initialise front-end", stlsoft::integer_to_string(&num[0], STLSOFT_NUM_ELEMENTS(num), res));
    }
    else
    {
        XTESTS_TEST_PASSED();

        // Now verify the levels
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(s_severityLevels); ++i)
        {
            int b = pantheios_fe_isSeverityLogged(token, s_severityLevels[i], 0);

            XTESTS_TEST_INTEGER_EQUAL(s_severityLevelResults[i], b);
        }}

        pantheios_fe_uninit(token);
    }
}

static void test_levels_in_HKLM()
{
    // Ensure that the HKCU key exists, and has the right levels
    delete_HKCU();
    if(!ensure_HKLM(0xff, 0x3f, -1))
    {
        XTESTS_TEST_NOTICE("could not change HKEY_LOCAL_MACHINE, so skipping test case");

        return;
    }

    static const int s_severityLevelResults[] =
    {
            1
        ,   1
        ,   1
        ,   1
        ,   1
        ,   1
#ifdef NDEBUG
        ,   0
        ,   0
#else /* ? NDEBUG */
        ,   1
        ,   1
#endif /* NDEBUG */
    };

    STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS(s_severityLevels) == STLSOFT_NUM_ELEMENTS(s_severityLevelResults));

    void*   token;
    int     res;

    res = pantheios_fe_init(NULL, &token);

    XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_INIT_RC_SUCCESS, res);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        char    num[21];

        XTESTS_TEST_FAIL_WITH_QUALIFIER("failed to initialise front-end", stlsoft::integer_to_string(&num[0], STLSOFT_NUM_ELEMENTS(num), res));
    }
    else
    {
        XTESTS_TEST_PASSED();

        // Now verify the levels
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(s_severityLevels); ++i)
        {
            int b = pantheios_fe_isSeverityLogged(token, s_severityLevels[i], 0);

            XTESTS_TEST_INTEGER_EQUAL(s_severityLevelResults[i], b);
        }}

        pantheios_fe_uninit(token);
    }
}

static void test_HKCU_overrides_HKLM()
{
    // Ensure that the HKCU key exists, and has the right levels
    ensure_HKCU(0xff, 0x3f, -1);
    if(!ensure_HKLM(0, 0, 0))
    {
        XTESTS_TEST_NOTICE("could not change HKEY_LOCAL_MACHINE, so skipping test case");

        return;
    }

    static const int s_severityLevelResults[] =
    {
            1
        ,   1
        ,   1
        ,   1
        ,   1
        ,   1
#ifdef NDEBUG
        ,   0
        ,   0
#else /* ? NDEBUG */
        ,   1
        ,   1
#endif /* NDEBUG */
    };

    STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS(s_severityLevels) == STLSOFT_NUM_ELEMENTS(s_severityLevelResults));

    void*   token;
    int     res;

    res = pantheios_fe_init(NULL, &token);

    XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_INIT_RC_SUCCESS, res);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        char    num[21];

        XTESTS_TEST_FAIL_WITH_QUALIFIER("failed to initialise front-end", stlsoft::integer_to_string(&num[0], STLSOFT_NUM_ELEMENTS(num), res));
    }
    else
    {
        XTESTS_TEST_PASSED();

        // Now verify the levels
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(s_severityLevels); ++i)
        {
            int b = pantheios_fe_isSeverityLogged(token, s_severityLevels[i], 0);

            XTESTS_TEST_INTEGER_EQUAL(s_severityLevelResults[i], b);
        }}

        pantheios_fe_uninit(token);
    }
}

static void test_star()
{
    // Ensure that the HKCU key exists, and has the right levels
    delete_HKCU();
    ensure_HKCU(-1, -1, 0x0f);
    if(!delete_HKLM())
    {
        XTESTS_TEST_NOTICE("could not change HKEY_LOCAL_MACHINE, so skipping test case");

        return;
    }

    static const int s_severityLevelResults[] =
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

    STLSOFT_STATIC_ASSERT(STLSOFT_NUM_ELEMENTS(s_severityLevels) == STLSOFT_NUM_ELEMENTS(s_severityLevelResults));

    void*   token;
    int     res;

    res = pantheios_fe_init(NULL, &token);

    XTESTS_TEST_INTEGER_EQUAL(PANTHEIOS_INIT_RC_SUCCESS, res);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        char    num[21];

        XTESTS_TEST_FAIL_WITH_QUALIFIER("failed to initialise front-end", stlsoft::integer_to_string(&num[0], STLSOFT_NUM_ELEMENTS(num), res));
    }
    else
    {
        XTESTS_TEST_PASSED();

        // Now verify the levels
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(s_severityLevels); ++i)
        {
            int b = pantheios_fe_isSeverityLogged(token, s_severityLevels[i], 0);

            XTESTS_TEST_INTEGER_EQUAL(s_severityLevelResults[i], b);
        }}

        pantheios_fe_uninit(token);
    }
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

static void test_cleanup()
{
    delete_HKCU();
    delete_HKLM();

    XTESTS_TEST_PASSED();
}


} // anonymous namespace

/* ///////////////////////////// end of file //////////////////////////// */
