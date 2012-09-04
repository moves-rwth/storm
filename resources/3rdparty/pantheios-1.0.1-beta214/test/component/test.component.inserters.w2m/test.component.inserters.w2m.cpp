/* /////////////////////////////////////////////////////////////////////////
 * File:        test/component/test.component.inserters.w2m/test.component.inserters.w2m.cpp
 *
 * Purpose:     Implementation file for the test.component.inserters.w2m project.
 *
 * Created:     19th October 2006
 * Updated:     6th August 2012
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2006-2012, Synesis Software Pty Ltd.
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
#include <stlsoft/string/simple_string.hpp>
#include <pantheios/pantheios.hpp>          // Pantheios C++ main header
#include <pantheios/inserters/w2m.hpp>      // for pantheios::w2m
#include <pantheios/backends/bec.test.h>

/* STLSoft Header Files */
#include <stlsoft/util/limit_traits.h>
#include <stlsoft/string/simple_string.hpp>

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

#ifndef PANTHEIOS_USE_WIDE_STRINGS

static void test_1_01();
static void test_1_02();
static void test_1_03();
static void test_1_04();
static void test_1_05();
static void test_1_06();
static void test_1_07();
static void test_1_08();
static void test_1_09();

static void test_2_01();
static void test_2_02();
static void test_2_03();
static void test_2_04();
static void test_2_05();
static void test_2_06();
static void test_2_07();
static void test_2_08();
static void test_2_09();

static void test_3_01();
static void test_3_02();
static void test_3_03();
static void test_3_04();
static void test_3_05();
static void test_3_06();
static void test_3_07();
static void test_3_08();
static void test_3_09();

#endif /* !PANTHEIOS_USE_WIDE_STRINGS */

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.component.inserters.w2m");

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.component.inserters.w2m", verbosity))
    {
#ifndef PANTHEIOS_USE_WIDE_STRINGS

        XTESTS_RUN_CASE(test_1_01);
        XTESTS_RUN_CASE(test_1_02);
        XTESTS_RUN_CASE(test_1_03);
        XTESTS_RUN_CASE(test_1_04);
        XTESTS_RUN_CASE(test_1_05);
        XTESTS_RUN_CASE(test_1_06);
        XTESTS_RUN_CASE(test_1_07);
        XTESTS_RUN_CASE(test_1_08);
        XTESTS_RUN_CASE(test_1_09);

        XTESTS_RUN_CASE(test_2_01);
        XTESTS_RUN_CASE(test_2_02);
        XTESTS_RUN_CASE(test_2_03);
        XTESTS_RUN_CASE(test_2_04);
        XTESTS_RUN_CASE(test_2_05);
        XTESTS_RUN_CASE(test_2_06);
        XTESTS_RUN_CASE(test_2_07);
        XTESTS_RUN_CASE(test_2_08);
        XTESTS_RUN_CASE(test_2_09);

        XTESTS_RUN_CASE(test_3_01);
        XTESTS_RUN_CASE(test_3_02);
        XTESTS_RUN_CASE(test_3_03);
        XTESTS_RUN_CASE(test_3_04);
        XTESTS_RUN_CASE(test_3_05);
        XTESTS_RUN_CASE(test_3_06);
        XTESTS_RUN_CASE(test_3_07);
        XTESTS_RUN_CASE(test_3_08);
        XTESTS_RUN_CASE(test_3_09);

#endif /* !PANTHEIOS_USE_WIDE_STRINGS */

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* ////////////////////////////////////////////////////////////////////// */

#ifndef PANTHEIOS_USE_WIDE_STRINGS

static void test_1_01()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(L""));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_1_02()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(L"a"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("a", results[0].statement);
}

static void test_1_03()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE("[", pantheios::w2m(L"abc"), "]");


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("[abc]", results[0].statement);
}

static void test_1_04()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(L""));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_1_05()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(L""));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_1_06()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(L""));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_1_07()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(L""));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_1_08()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(L""));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_1_09()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(L""));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_2_01()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(std::wstring(L"")));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_2_02()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(std::wstring(L"a")));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("a", results[0].statement);
}

static void test_2_03()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE("[", pantheios::w2m(std::wstring(L"abc")), "]");


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("[abc]", results[0].statement);
}

static void test_2_04()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(std::wstring(L"")));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_2_05()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(std::wstring(L"")));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_2_06()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(std::wstring(L"")));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_2_07()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(std::wstring(L"")));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_2_08()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(std::wstring(L"")));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_2_09()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(std::wstring(L"")));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_3_01()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(stlsoft::simple_wstring(L"")));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_3_02()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(stlsoft::simple_wstring(L"a")));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("a", results[0].statement);
}

static void test_3_03()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE("[", pantheios::w2m(stlsoft::simple_wstring(L"abc")), "]");


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("[abc]", results[0].statement);
}

static void test_3_04()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(stlsoft::simple_wstring(L"")));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_3_05()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(stlsoft::simple_wstring(L"")));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_3_06()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(stlsoft::simple_wstring(L"")));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_3_07()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(stlsoft::simple_wstring(L"")));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_3_08()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(stlsoft::simple_wstring(L"")));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_3_09()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::w2m(stlsoft::simple_wstring(L"")));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

#endif /* !PANTHEIOS_USE_WIDE_STRINGS */

/* ///////////////////////////// end of file //////////////////////////// */
