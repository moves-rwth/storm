/* /////////////////////////////////////////////////////////////////////////
 * File:        test/component/test.component.inserters.pad/test.component.inserters.pad.cpp
 *
 * Purpose:     Implementation file for the test.component.inserters.pad project.
 *
 * Created:     29th June 2009
 * Updated:     6th August 2012
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2009-2012, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

/* xTests Header Files */
#include <xtests/xtests.h>

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>          // Pantheios C++ main header
#include <pantheios/inserters/pad.hpp>      // for pantheios::pad
#include <pantheios/backends/bec.test.h>

/* Standard C++ Header Files */
#include <exception>
#include <string>

/* Standard C Header Files */
#include <stdio.h>
#include <stdlib.h>

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* /////////////////////////////////////////////////////////////////////////
 * Forward declarations
 */

namespace
{

    static void test_1_01();
    static void test_1_02();
    static void test_1_03();
    static void test_1_04();
    static void test_1_05();
    static void test_1_06();
    static void test_1_07();
    static void test_1_08();
    static void test_1_09();
    static void test_1_10();
    static void test_1_11();
    static void test_1_12();
    static void test_1_13();

} /* anonymous namespace */

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.component.inserters.pad");

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

    if(XTESTS_START_RUNNER_WITH_FLAGS("test.component.inserters.pad", verbosity, XTESTS_NS_C_QUAL(xtestsReportOnlyNonEmptyCases)))
    {
        XTESTS_RUN_CASE(test_1_01);
        XTESTS_RUN_CASE(test_1_02);
        XTESTS_RUN_CASE(test_1_03);
        XTESTS_RUN_CASE(test_1_04);
        XTESTS_RUN_CASE(test_1_05);
        XTESTS_RUN_CASE(test_1_06);
        XTESTS_RUN_CASE(test_1_07);
        XTESTS_RUN_CASE(test_1_08);
        XTESTS_RUN_CASE(test_1_09);
        XTESTS_RUN_CASE(test_1_10);
        XTESTS_RUN_CASE(test_1_11);
        XTESTS_RUN_CASE(test_1_12);
        XTESTS_RUN_CASE(test_1_13);

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* ////////////////////////////////////////////////////////////////////// */

namespace
{
    typedef std::basic_string<pantheios::pan_char_t>    string_t;

static void test_1_01()
{
    { for(size_t i = 0; i != 1000; ++i)
    {
        // 1. Setup

        pantheios::be::test::reset();


        // 2. Create test data

        pantheios::log_NOTICE(pantheios::pad("", i));


        // 3. Verification

        pantheios::be::test::Results  results = pantheios::be::test::results();

        XTESTS_TEST(!results.empty());
        XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
        XTESTS_TEST_INTEGER_EQUAL(i, results[0].statement.size());
        XTESTS_TEST_STRING_EQUAL(string_t(i, ' '), results[0].statement);
    }}
}

static void test_1_02()
{
    { for(size_t i = 0; i != 1000; ++i)
    {
        // 1. Setup

        pantheios::be::test::reset();


        // 2. Create test data

        pantheios::log_NOTICE(pantheios::pad(string_t(), i));


        // 3. Verification

        pantheios::be::test::Results  results = pantheios::be::test::results();

        XTESTS_TEST(!results.empty());
        XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
        XTESTS_TEST_INTEGER_EQUAL(i, results[0].statement.size());
        XTESTS_TEST_STRING_EQUAL(string_t(i, ' '), results[0].statement);
    }}
}

static void test_1_03()
{
    { for(size_t i = 0; i != 1/* 000 */; ++i)
    {
        // 1. Setup

        pantheios::be::test::reset();


        // 2. Create test data

        pantheios::log_NOTICE(pantheios::pad(" ", i));


        // 3. Verification

        pantheios::be::test::Results  results = pantheios::be::test::results();

        XTESTS_TEST(!results.empty());
        XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
        if(i <= 1)
        {
            XTESTS_TEST_INTEGER_EQUAL(0u, results[0].statement.size());
            XTESTS_TEST_STRING_EQUAL(PSTR(""), results[0].statement);
        }
        else
        {
            XTESTS_TEST_INTEGER_EQUAL(i - 1u, results[0].statement.size());
            XTESTS_TEST_STRING_EQUAL(string_t(i - 1u, ' '), results[0].statement);
        }
    }}
}

static void test_1_04()
{
    { for(size_t i = 0; i != 1000; ++i)
    {
        const string_t value(i, '~');

        STLSOFT_ASSERT(i == value.size());

        { for(size_t j = 0; j != 1000; ++j)
        {
            // 1. Setup

            pantheios::be::test::reset();


            // 2. Create test data

            pantheios::log_NOTICE(pantheios::pad(value, j), value);


            // 3. Verification

            pantheios::be::test::Results  results = pantheios::be::test::results();

            XTESTS_TEST(!results.empty());
            XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
            if(i < j)
            {
                /* Some padding */
                const string_t stmt = string_t(j - i, ' ') + value;

                XTESTS_TEST_INTEGER_EQUAL(stmt.size(), results[0].statement.size());
                XTESTS_TEST_STRING_EQUAL(stmt, results[0].statement);
            }
            else
            {
                /* No padding */

                XTESTS_TEST_INTEGER_EQUAL(value.size(), results[0].statement.size());
                XTESTS_TEST_STRING_EQUAL(value, results[0].statement);
            }
        }}
    }}
}

static void test_1_05()
{
}

static void test_1_06()
{
}

static void test_1_07()
{
}

static void test_1_08()
{
}

static void test_1_09()
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

} /* anonymous namespace */

/* ///////////////////////////// end of file //////////////////////////// */
