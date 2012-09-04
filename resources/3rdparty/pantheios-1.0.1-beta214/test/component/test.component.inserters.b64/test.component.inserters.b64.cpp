/* /////////////////////////////////////////////////////////////////////////
 * File:        test/component/test.component.inserters.b64/test.component.inserters.b64.cpp
 *
 * Purpose:     Implementation file for the test.component.inserters.b64 project.
 *
 * Created:     31st July 2006
 * Updated:     23rd March 2010
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2006-2010, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

/* xTests Header Files */
#include <xtests/xtests.h>

/* b64 Header Files */
#include <b64/b64.h>

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>          // Pantheios C++ main header
#include <pantheios/inserters/b64.hpp>      // for pantheios::b64
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
static void test_1_11();
static void test_1_12();
static void test_1_13();
static void test_1_14();
static void test_1_15();
static void test_1_16();
static void test_1_17();
static void test_1_18();
static void test_1_19();
static void test_1_20();

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.component.inserters.b64");

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
        XTESTS_RUN_CASE(test_1_11);
        XTESTS_RUN_CASE(test_1_12);
        XTESTS_RUN_CASE(test_1_13);
        XTESTS_RUN_CASE(test_1_14);
        XTESTS_RUN_CASE(test_1_15);
        XTESTS_RUN_CASE(test_1_16);
        XTESTS_RUN_CASE(test_1_17);
        XTESTS_RUN_CASE(test_1_18);
        XTESTS_RUN_CASE(test_1_19);
        XTESTS_RUN_CASE(test_1_20);

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

    pantheios::uint8_t  ui8 = 0;

    pantheios::log_NOTICE(pantheios::b64(&ui8, sizeof(ui8)));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("AA=="), results[0].statement);
}

static void test_1_02()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::uint8_t  ui8 = 1;

    pantheios::log_NOTICE(pantheios::b64(&ui8, sizeof(ui8)));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("AQ=="), results[0].statement);
}

static void test_1_03()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::uint8_t  ui8 = 10;

    pantheios::log_NOTICE(pantheios::b64(&ui8, sizeof(ui8)));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("Cg=="), results[0].statement);
}

static void test_1_04()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::uint8_t  ui8 = 100;

    pantheios::log_NOTICE(pantheios::b64(&ui8, sizeof(ui8)));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("ZA=="), results[0].statement);
}

static void test_1_05()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::uint8_t  ui8 = 128;

    pantheios::log_NOTICE(pantheios::b64(&ui8, sizeof(ui8)));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("gA=="), results[0].statement);
}

static void test_1_06()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::uint8_t  ui8 = 255;

    pantheios::log_NOTICE(pantheios::b64(&ui8, sizeof(ui8)));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("/w=="), results[0].statement);
}

static void test_1_07()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::b64("This is a test string", 21));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("VGhpcyBpcyBhIHRlc3Qgc3RyaW5n"), results[0].statement);
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
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::sint8_t bytes[] = { 7, 6, 5, 4, 3, 2, 1, 0, -1, -2, -3, -4, -5, -6, -7 };

    pantheios::log_NOTICE(pantheios::b64(&bytes[0], sizeof(bytes)));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("BwYFBAMCAQD//v38+/r5"), results[0].statement);
}

static void test_1_13()
{
}

static void test_1_14()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    int ints[50];

    ::memset(&ints[0], 0, sizeof(ints));

    pantheios::log_NOTICE(pantheios::b64(&ints[0], sizeof(ints), ::b64::B64_F_LINE_LEN_64));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(
        PSTR("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n")
        PSTR("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n")
        PSTR("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n")
        PSTR("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n")
        PSTR("AAAAAAAAAAA="), results[0].statement);
}

static void test_1_15()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    int ints[50];

    ::memset(&ints[0], 0, sizeof(ints));

    pantheios::log_NOTICE(pantheios::b64(&ints[0], sizeof(ints), ::b64::B64_F_LINE_LEN_INFINITE));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA="), results[0].statement);
}

static void test_1_16()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    int ints[50];

    ::memset(&ints[0], 0, sizeof(ints));

    pantheios::log_NOTICE(pantheios::b64(&ints[0], sizeof(ints), ::b64::B64_F_LINE_LEN_INFINITE));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA="), results[0].statement);
}

static void test_1_17()
{
}

static void test_1_18()
{
}

static void test_1_19()
{
}

static void test_1_20()
{
}

/* ///////////////////////////// end of file //////////////////////////// */
