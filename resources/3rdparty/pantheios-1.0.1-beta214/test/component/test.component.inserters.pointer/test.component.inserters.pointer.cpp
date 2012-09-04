/* /////////////////////////////////////////////////////////////////////////
 * File:        test/component/test.component.inserters.pointer/test.component.inserters.pointer.cpp
 *
 * Purpose:     Implementation file for the test.component.inserters.pointer project.
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
#include <pantheios/pantheios.hpp>          // Pantheios C++ main header
#include <pantheios/inserters/pointer.hpp>  // for pantheios::pointer
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

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.component.inserters.pointer");

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

    if(XTESTS_START_RUNNER("test.component.inserters.pointer", verbosity))
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

static void test_1_01()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::pointer(static_cast<void*>(0), 1));
    pantheios::log_NOTICE(pantheios::pointer(static_cast<void const*>(0), 1));
    pantheios::log_NOTICE(pantheios::pointer(static_cast<void volatile*>(0), 1));
    pantheios::log_NOTICE(pantheios::pointer(static_cast<void const volatile*>(0), 1));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(4u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("(null)"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("(null)"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("(null)"), results[2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("(null)"), results[3].statement);
}

static void test_1_02()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::pointer(static_cast<void*>(0), 4));
    pantheios::log_NOTICE(pantheios::pointer(static_cast<void*>(0), 8));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(2u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("(null)"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("(null)"), results[1].statement);
}

static void test_1_03()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data
    void*   p1 = reinterpret_cast<void*>(0x1234);
    void*   p2 = reinterpret_cast<void*>(0x12345678);

    pantheios::log_NOTICE(pantheios::pointer(p1, 4));
    pantheios::log_NOTICE(pantheios::pointer(p2, 8));
    pantheios::log_NOTICE(pantheios::pointer(p1, 8));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(3u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("1234"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("12345678"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("    1234"), results[2].statement);
}

static void test_1_04()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data
    void*   p1 = reinterpret_cast<void*>(0x1234);
    void*   p2 = reinterpret_cast<void*>(0x12345678);

    pantheios::log_NOTICE(pantheios::pointer(p1, 4 | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::pointer(p2, 8 | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::pointer(p1, 8 | pantheios::fmt::hex));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(3u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("1234"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("12345678"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("    1234"), results[2].statement);
}

static void test_1_05()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data
    void*   p1 = reinterpret_cast<void*>(0x1234);
    void*   p2 = reinterpret_cast<void*>(0x12345678);

    pantheios::log_NOTICE(pantheios::pointer(p1, 4 | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::pointer(p2, 8 | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::pointer(p1, 8 | pantheios::fmt::zeroXPrefix));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(3u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x12345678"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00001234"), results[2].statement);
}

static void test_1_06()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data
    void*   p1 = reinterpret_cast<void*>(0x1234);
    void*   p2 = reinterpret_cast<void*>(0x12345678);

    pantheios::log_NOTICE(pantheios::pointer(p1, 4 | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::pointer(p2, 8 | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::pointer(p1, 8 | pantheios::fmt::zeroPad));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(3u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("1234"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("12345678"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("00001234"), results[2].statement);
}

static void test_1_07()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data
    void*   p1 = reinterpret_cast<void*>(0x1234);
    void*   p2 = reinterpret_cast<void*>(0x12345678);

    pantheios::log_NOTICE(pantheios::pointer(p1, 4 | pantheios::fmt::zeroPad | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::pointer(p2, 8 | pantheios::fmt::zeroPad | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::pointer(p1, 8 | pantheios::fmt::zeroPad | pantheios::fmt::zeroXPrefix));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(3u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x12345678"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00001234"), results[2].statement);
}

static void test_1_08()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data
    void*   p1 = reinterpret_cast<void*>(0x1234);
    void*   p2 = reinterpret_cast<void*>(0x12345678);

    pantheios::log_NOTICE(pantheios::pointer(p1, 4 | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::pointer(p2, 8 | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::pointer(p1, 8 | pantheios::fmt::zeroXPrefix));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(3u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x12345678"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00001234"), results[2].statement);
}

static void test_1_09()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data
    void*   p1 = reinterpret_cast<void*>(0x1234);
    void*   p2 = reinterpret_cast<void*>(0x12345678);

    pantheios::log_NOTICE(pantheios::pointer(p1, 4  | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::pointer(p1, 8  | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::pointer(p1, 16 | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::pointer(p2, 8  | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::pointer(p2, 8  | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::pointer(p2, 16 | pantheios::fmt::hex));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(6u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("1234"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("    1234"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("            1234"), results[2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("12345678"), results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("12345678"), results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("        12345678"), results[5].statement);
}

static void test_1_10()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data
    void*   p1 = reinterpret_cast<void*>(0x1234);
    void*   p2 = reinterpret_cast<void*>(0x12345678);

    pantheios::log_NOTICE(pantheios::pointer(p1, 4,  pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::pointer(p1, 8,  pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::pointer(p1, 16, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::pointer(p2, 8,  pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::pointer(p2, 8,  pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::pointer(p2, 16, pantheios::fmt::hex));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(6u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("1234"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("    1234"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("            1234"), results[2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("12345678"), results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("12345678"), results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("        12345678"), results[5].statement);
}

static void test_1_11()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data
    void*   p1 = reinterpret_cast<void*>(0x1234);
    void*   p2 = reinterpret_cast<void*>(0x12345678);

    pantheios::log_NOTICE(pantheios::pointer(p1, -4,  pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::pointer(p1, -8,  pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::pointer(p1, -16, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::pointer(p2, -8,  pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::pointer(p2, -8,  pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::pointer(p2, -16, pantheios::fmt::hex));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(6u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("1234"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234    "), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234            "), results[2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("12345678"), results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("12345678"), results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("12345678        "), results[5].statement);
}

static void test_1_12()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data
    void*   p1 = reinterpret_cast<void*>(0x1234);
    void*   p2 = reinterpret_cast<void*>(0x12345678);

    pantheios::log_NOTICE(pantheios::pointer(p1, 4,  pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::pointer(p1, 8,  pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::pointer(p1, 16, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::pointer(p2, 8,  pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::pointer(p2, 8,  pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::pointer(p2, 16, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(6u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("    0x1234"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("            0x1234"), results[2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x12345678"), results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x12345678"), results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("        0x12345678"), results[5].statement);
}

static void test_1_13()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data
    void*   p1 = reinterpret_cast<void*>(0x1234);
    void*   p2 = reinterpret_cast<void*>(0x12345678);

    pantheios::log_NOTICE(pantheios::pointer(p1, -4,  pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::pointer(p1, -8,  pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::pointer(p1, -16, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::pointer(p2, -8,  pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::pointer(p2, -8,  pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::pointer(p2, -16, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST_INTEGER_EQUAL(6u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234    "), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234            "), results[2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x12345678"), results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x12345678"), results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x12345678        "), results[5].statement);
}

/* ///////////////////////////// end of file //////////////////////////// */
