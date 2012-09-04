/* /////////////////////////////////////////////////////////////////////////
 * File:        test/component/test.component.inserters.args/test.component.inserters.args.cpp
 *
 * Purpose:     Implementation file for the test.component.inserters.args project.
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
#include <pantheios/pantheios.hpp>      // Pantheios C++ main header
#include <pantheios/inserters/args.hpp> // for pantheios::args
#include <pantheios/backends/bec.test.h>

/* Standard C++ Header Files */
#include <exception>

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

static void test_1_11();
static void test_1_12();
static void test_1_13();
static void test_1_14();
static void test_1_15();
static void test_1_16();
static void test_1_17();
static void test_1_18();
static void test_1_19();

#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.component.inserters.args");

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.component.inserters.args", verbosity))
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

        XTESTS_RUN_CASE(test_1_11);
        XTESTS_RUN_CASE(test_1_12);
        XTESTS_RUN_CASE(test_1_13);
        XTESTS_RUN_CASE(test_1_14);
        XTESTS_RUN_CASE(test_1_15);
        XTESTS_RUN_CASE(test_1_16);
        XTESTS_RUN_CASE(test_1_17);
        XTESTS_RUN_CASE(test_1_18);
        XTESTS_RUN_CASE(test_1_19);

#endif /* PANTHEIOS_USE_WIDE_STRINGS */

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

    pantheios::log_NOTICE(pantheios::args(0, static_cast<char**>(NULL)));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_1_02()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    char const* argv[] =
    {
       "abc"
    };

    pantheios::log_NOTICE(pantheios::args(STLSOFT_NUM_ELEMENTS(argv), argv));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("abc", results[0].statement);
}

static void test_1_03()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    char const* argv[] =
    {
            "abc"
        ,   "def"
    };

    pantheios::log_NOTICE(pantheios::args(STLSOFT_NUM_ELEMENTS(argv), argv));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("abc, def", results[0].statement);
}

static void test_1_04()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    char const* argv[] =
    {
            "abc"
        ,   "def"
        ,   "ghi"
        ,   "jkl"
    };

    pantheios::log_NOTICE(pantheios::args(STLSOFT_NUM_ELEMENTS(argv), argv));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("abc, def, ghi, jkl", results[0].statement);
}

static void test_1_05()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    char const* argv[] =
    {
            "abc"
        ,   "def"
        ,   "ghi"
        ,   "jkl"
    };

    pantheios::log_NOTICE(pantheios::args(STLSOFT_NUM_ELEMENTS(argv), argv, pantheios::args::alwaysQuoteArgs));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("\"abc\", \"def\", \"ghi\", \"jkl\"", results[0].statement);
}

static void test_1_06()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    char const* argv[] =
    {
            "abc"
        ,   "def ghi"
        ,   "jkl"
    };

    pantheios::log_NOTICE(pantheios::args(STLSOFT_NUM_ELEMENTS(argv), argv, pantheios::args::neverQuoteArgs));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("abc, def ghi, jkl", results[0].statement);
}

static void test_1_07()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    char const* argv[] =
    {
            "abc"
        ,   "def ghi"
        ,   "jkl"
    };

    pantheios::log_NOTICE(pantheios::args(STLSOFT_NUM_ELEMENTS(argv), argv, pantheios::args::quoteArgsWithSpaces));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("abc, \"def ghi\", jkl", results[0].statement);
}

static void test_1_08()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    char const* argv[] =
    {
            "abc"
        ,   "def ghi"
        ,   "jkl"
    };

    pantheios::log_NOTICE(pantheios::args(STLSOFT_NUM_ELEMENTS(argv), argv, pantheios::args::alwaysQuoteArgs));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("\"abc\", \"def ghi\", \"jkl\"", results[0].statement);
}

static void test_1_09()
{
}



static void test_1_11()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::args(0, static_cast<char**>(NULL), pantheios::args::quoteArgsWithSpaces, "|"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", results[0].statement);
}

static void test_1_12()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    char const* argv[] =
    {
       "abc"
    };

    pantheios::log_NOTICE(pantheios::args(STLSOFT_NUM_ELEMENTS(argv), argv, pantheios::args::quoteArgsWithSpaces, "|"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("abc", results[0].statement);
}

static void test_1_13()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    char const* argv[] =
    {
            "abc"
        ,   "def"
    };

    pantheios::log_NOTICE(pantheios::args(STLSOFT_NUM_ELEMENTS(argv), argv, pantheios::args::quoteArgsWithSpaces, "|"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("abc|def", results[0].statement);
}

static void test_1_14()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    char const* argv[] =
    {
            "abc"
        ,   "def"
        ,   "ghi"
        ,   "jkl"
    };

    pantheios::log_NOTICE(pantheios::args(STLSOFT_NUM_ELEMENTS(argv), argv, pantheios::args::quoteArgsWithSpaces, "|"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("abc|def|ghi|jkl", results[0].statement);
}

static void test_1_15()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    char const* argv[] =
    {
            "abc"
        ,   "def"
        ,   "ghi"
        ,   "jkl"
    };

    pantheios::log_NOTICE(pantheios::args(STLSOFT_NUM_ELEMENTS(argv), argv, pantheios::args::alwaysQuoteArgs, "|"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("\"abc\"|\"def\"|\"ghi\"|\"jkl\"", results[0].statement);
}

static void test_1_16()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    char const* argv[] =
    {
            "abc"
        ,   "def ghi"
        ,   "jkl"
    };

    pantheios::log_NOTICE(pantheios::args(STLSOFT_NUM_ELEMENTS(argv), argv, pantheios::args::neverQuoteArgs, "|"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("abc|def ghi|jkl", results[0].statement);
}

static void test_1_17()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    char const* argv[] =
    {
            "abc"
        ,   "def ghi"
        ,   "jkl"
    };

    pantheios::log_NOTICE(pantheios::args(STLSOFT_NUM_ELEMENTS(argv), argv, pantheios::args::quoteArgsWithSpaces, "|"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("abc|\"def ghi\"|jkl", results[0].statement);
}

static void test_1_18()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    char const* argv[] =
    {
            "abc"
        ,   "def ghi"
        ,   "jkl"
    };

    pantheios::log_NOTICE(pantheios::args(STLSOFT_NUM_ELEMENTS(argv), argv, pantheios::args::alwaysQuoteArgs, "|"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("\"abc\"|\"def ghi\"|\"jkl\"", results[0].statement);
}

static void test_1_19()
{
}

#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* ///////////////////////////// end of file //////////////////////////// */
