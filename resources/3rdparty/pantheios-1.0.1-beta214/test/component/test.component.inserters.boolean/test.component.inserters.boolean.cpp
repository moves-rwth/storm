/* /////////////////////////////////////////////////////////////////////////
 * File:        test/component/test.component.inserters.boolean/test.component.inserters.boolean.cpp
 *
 * Purpose:     Implementation file for the test.component.inserters.boolean project.
 *
 * Created:     7th August 2008
 * Updated:     23rd March 2010
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
#include <pantheios/pantheios.hpp>          // Pantheios C++ main header
#include <pantheios/inserters/boolean.hpp>  // for pantheios::boolean
#include <pantheios/backends/bec.test.h>

/* STLSoft Header Files */
#include <stlsoft/util/limit_traits.h>

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

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.component.inserters.boolean");

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

    if(XTESTS_START_RUNNER("test.component.inserters.integer", verbosity))
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

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* ////////////////////////////////////////////////////////////////////// */

static void test_1_01()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(false), PSTR("}"));
    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(true), PSTR("}"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(2u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("{false}"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{true}"), results[1].statement);
}

static void test_1_02()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::boolean::set_value_strings(NULL, NULL);

    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(false), PSTR("}"));
    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(true), PSTR("}"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(2u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("{false}"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{true}"), results[1].statement);
}

static void test_1_03()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::boolean::set_value_strings(PSTR("FALSE"), PSTR("TRUE"));

    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(false), PSTR("}"));
    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(true), PSTR("}"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(2u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("{FALSE}"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{TRUE}"), results[1].statement);
}

static void test_1_04()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::boolean::set_value_strings(PSTR("FALSE"), PSTR("TRUE"));

    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(false), PSTR("}"));
    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(true), PSTR("}"));

    pantheios::boolean::set_value_strings(NULL, NULL);

    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(false), PSTR("}"));
    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(true), PSTR("}"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(4u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("{FALSE}"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{TRUE}"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{false}"), results[2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{true}"), results[3].statement);
}

static void test_1_05()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::boolean::set_value_strings(PSTR("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"), PSTR("T"));

    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(false), PSTR("}"));
    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(true), PSTR("}"));

    pantheios::boolean::set_value_strings(NULL, NULL);

    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(false), PSTR("}"));
    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(true), PSTR("}"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(4u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("{ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff}"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{T}"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{false}"), results[2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{true}"), results[3].statement);
}

static void test_1_06()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::boolean::set_value_strings(PSTR("FALSE"), PSTR("TRUE"));

    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(false), PSTR("}"));
    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(true), PSTR("}"));

    pantheios::boolean::set_value_strings(NULL, NULL);

    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(false), PSTR("}"));
    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(true), PSTR("}"));

    pantheios::boolean::set_value_strings(PSTR("FALSE"), PSTR("TRUE"));

    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(false), PSTR("}"));
    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(true), PSTR("}"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(6u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("{FALSE}"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{TRUE}"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{false}"), results[2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{true}"), results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{FALSE}"), results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{TRUE}"), results[5].statement);
}

static void test_1_07()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::boolean::set_value_strings(PSTR("FALSE"), PSTR("TRUE"));

    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(false), PSTR("}"));
    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(true), PSTR("}"));

    pantheios::boolean::set_value_strings(NULL, NULL);

    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(false), PSTR("}"));
    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(true), PSTR("}"));

    pantheios::boolean::set_value_strings(PSTR("F"), PSTR("T"));

    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(false), PSTR("}"));
    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(true), PSTR("}"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(6u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("{FALSE}"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{TRUE}"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{false}"), results[2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{true}"), results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{F}"), results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{T}"), results[5].statement);
}

static void test_1_08()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::boolean::set_value_strings(PSTR("FALSE"), PSTR("TRUE"));

    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(false), PSTR("}"));
    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(true), PSTR("}"));

    pantheios::boolean::set_value_strings(NULL, NULL);

    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(false), PSTR("}"));
    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(true), PSTR("}"));

    pantheios::boolean::set_value_strings(PSTR("F"), PSTR("T"));

    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(false), PSTR("}"));
    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(true), PSTR("}"));

    pantheios::boolean::set_value_strings(NULL, NULL);

    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(false), PSTR("}"));
    pantheios::log_NOTICE(PSTR("{"), pantheios::boolean(true), PSTR("}"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(8u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("{FALSE}"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{TRUE}"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{false}"), results[2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{true}"), results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{F}"), results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{T}"), results[5].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{false}"), results[6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("{true}"), results[7].statement);
}

static void test_1_09()
{
}

static void test_1_10()
{
}

/* ///////////////////////////// end of file //////////////////////////// */
