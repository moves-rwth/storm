/* /////////////////////////////////////////////////////////////////////////
 * File:        test/component/test.component.inserters.integer/test.component.inserters.integer.cpp
 *
 * Purpose:     Implementation file for the test.component.inserters.integer project.
 *
 * Created:     19th October 2006
 * Updated:     7th August 2012
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
#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS
#include <pantheios/pantheios.hpp>          // Pantheios C++ main header
#include <pantheios/inserters/integer.hpp>  // for pantheios::integer
#include <pantheios/backends/bec.test.h>

/* STLSoft Header Files */
#include <stlsoft/util/limit_traits.h>

/* Standard C++ Header Files */
#include <exception>

/* Standard C Header Files */
#include <stdio.h>
#include <stdlib.h>

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* /////////////////////////////////////////////////////////////////////////
 * Forward declarations
 */

static void test_1_00();
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

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.component.inserters.integer");

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
#if 1
        XTESTS_RUN_CASE(test_1_00);
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
#else /* ? 0 */
        XTESTS_RUN_CASE(test_1_15);
        XTESTS_RUN_CASE(test_1_14);
        XTESTS_RUN_CASE(test_1_13);
        XTESTS_RUN_CASE(test_1_12);
        XTESTS_RUN_CASE(test_1_11);
        XTESTS_RUN_CASE(test_1_10);
        XTESTS_RUN_CASE(test_1_09);
        XTESTS_RUN_CASE(test_1_08);
        XTESTS_RUN_CASE(test_1_07);
        XTESTS_RUN_CASE(test_1_06);
        XTESTS_RUN_CASE(test_1_05);
        XTESTS_RUN_CASE(test_1_04);
        XTESTS_RUN_CASE(test_1_03);
        XTESTS_RUN_CASE(test_1_02);
        XTESTS_RUN_CASE(test_1_01);
        XTESTS_RUN_CASE(test_1_00);
#endif /* 0 */

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* ////////////////////////////////////////////////////////////////////// */

static void test_1_00()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data
    size_t n = 0;

    pantheios::log_NOTICE(pantheios::integer(0));
    pantheios::log_NOTICE(pantheios::integer(0L));
    n += 2;

    pantheios::log_NOTICE(pantheios::integer(static_cast< ::stlsoft::sint8_t>(0)));
    pantheios::log_NOTICE(pantheios::integer(static_cast< ::stlsoft::uint8_t>(0)));
    n += 2;
    pantheios::log_NOTICE(pantheios::integer(static_cast< ::stlsoft::sint16_t>(0)));
    pantheios::log_NOTICE(pantheios::integer(static_cast< ::stlsoft::uint16_t>(0)));
    n += 2;
    pantheios::log_NOTICE(pantheios::integer(static_cast< ::stlsoft::sint32_t>(0)));
    pantheios::log_NOTICE(pantheios::integer(static_cast< ::stlsoft::uint32_t>(0)));
    n += 2;
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    pantheios::log_NOTICE(pantheios::integer(static_cast< ::stlsoft::sint64_t>(0)));
    pantheios::log_NOTICE(pantheios::integer(static_cast< ::stlsoft::uint64_t>(0)));
    n += 2;
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
#ifdef STLSOFT_CF_INT_DISTINCT_INT_TYPE
    pantheios::log_NOTICE(pantheios::integer(static_cast<int>(0)));
    pantheios::log_NOTICE(pantheios::integer(static_cast<unsigned int>(0)));
    n += 2;
#endif /* STLSOFT_CF_INT_DISTINCT_INT_TYPE */
#ifdef STLSOFT_CF_LONG_DISTINCT_INT_TYPE
    pantheios::log_NOTICE(pantheios::integer(static_cast<long>(0)));
    pantheios::log_NOTICE(pantheios::integer(static_cast<unsigned long>(0)));
    n += 2;
#endif /* STLSOFT_CF_LONG_DISTINCT_INT_TYPE */
#ifdef STLSOFT_CF_BUILTIN_long_long_SUPPORT
    pantheios::log_NOTICE(pantheios::integer(static_cast<long long>(0)));
    pantheios::log_NOTICE(pantheios::integer(static_cast<unsigned long long>(0)));
    n += 2;
#endif /* STLSOFT_CF_BUILTIN_long_long_SUPPORT */



    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(n, results.size()));
    { for(size_t i = 0; i != n; ++i)
    {
        XTESTS_TEST_STRING_EQUAL(PSTR("0"), results[i].statement);
    }}
}

static void test_1_01()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::integer(static_cast<short>(0)));
    pantheios::log_NOTICE(pantheios::integer(static_cast<unsigned short>(0)));
    pantheios::log_NOTICE(pantheios::integer(static_cast<int>(0)));
    pantheios::log_NOTICE(pantheios::integer(static_cast<unsigned int>(0)));
    pantheios::log_NOTICE(pantheios::integer(static_cast<long>(0)));
    pantheios::log_NOTICE(pantheios::integer(static_cast<unsigned long>(0)));
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    pantheios::log_NOTICE(pantheios::integer(static_cast< ::pantheios::sint64_t>(0)));
    pantheios::log_NOTICE(pantheios::integer(static_cast< ::stlsoft::uint64_t>(0)));
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(8u, results.size()));
    XTESTS_TEST_STRING_EQUAL(PSTR("0"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0"), results[2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0"), results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0"), results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0"), results[5].statement);
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    XTESTS_TEST_STRING_EQUAL(PSTR("0"), results[6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0"), results[7].statement);
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
}

static void test_1_02()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::integer(0));
    pantheios::log_NOTICE(pantheios::integer(1));
    pantheios::log_NOTICE(pantheios::integer(-1));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(3u, results.size()));
    XTESTS_TEST_STRING_EQUAL(PSTR("0"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1"), results[2].statement);
}

static void test_1_03()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::integer(0, 5));
    pantheios::log_NOTICE(pantheios::integer(1, 5));
    pantheios::log_NOTICE(pantheios::integer(-1, 5));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(3u, results.size()));
    XTESTS_TEST_STRING_EQUAL(PSTR("    0"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("    1"), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("   -1"), results[2].statement);
}

static void test_1_04()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::fullHex));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::fullHex | 1));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::fullHex | 1));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::fullHex | 1));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::fullHex | 2));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::fullHex | 2));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::fullHex | 2));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::fullHex | 3));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::fullHex | 3));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::fullHex | 3));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::fullHex | 4));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::fullHex | 4));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::fullHex | 4));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::fullHex | 5));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::fullHex | 5));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::fullHex | 5));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::fullHex | 6));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::fullHex | 6));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::fullHex | 6));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::fullHex | 7));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::fullHex | 7));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::fullHex | 7));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::fullHex | 8));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::fullHex | 8));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::fullHex | 8));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::fullHex | 9));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::fullHex | 9));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::fullHex | 9));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::fullHex | 10));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::fullHex | 10));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::fullHex | 10));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(33u, results.size()));

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),    results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),    results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x8421"), results[2].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),    results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),    results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x8421"), results[5].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),    results[6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),    results[7].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x8421"), results[8].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),    results[9].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),    results[10].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x8421"), results[11].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x00"),   results[12].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x01"),   results[13].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x8421"), results[14].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x000"),  results[15].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x001"),  results[16].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x8421"), results[17].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000"), results[18].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0001"), results[19].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x8421"), results[20].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000"), results[21].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00001"), results[22].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x08421"), results[23].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000"), results[24].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x000001"), results[25].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x008421"), results[26].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000"), results[27].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000001"), results[28].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0008421"), results[29].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000"), results[30].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000001"), results[31].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00008421"), results[32].statement);
}

static void test_1_05()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::integer(0, -5));
    pantheios::log_NOTICE(pantheios::integer(1, -5));
    pantheios::log_NOTICE(pantheios::integer(-1, -5));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(3u, results.size()));
    XTESTS_TEST_STRING_EQUAL(PSTR("0    "), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1    "), results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1   "), results[2].statement);
}

static void test_1_06()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 1));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 1));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 1));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 2));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 2));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 2));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 3));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 3));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 3));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 4));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 4));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 4));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 5));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 5));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 5));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 6));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 6));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 6));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 7));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 7));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 7));

    pantheios::log_NOTICE(pantheios::integer(0x0000, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 8));
    pantheios::log_NOTICE(pantheios::integer(0x0001, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 8));
    pantheios::log_NOTICE(pantheios::integer(0x8421, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 8));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(27u, results.size()));

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),    results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),    results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x8421"), results[2].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),    results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),    results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x8421"), results[5].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),   results[6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),   results[7].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x8421"), results[8].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),  results[9].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),  results[10].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x8421"), results[11].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR(" 0x0"), results[12].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR(" 0x1"), results[13].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x8421"), results[14].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("  0x0"), results[15].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("  0x1"), results[16].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x8421"), results[17].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("   0x0"), results[18].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("   0x1"), results[19].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x8421"), results[20].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("    0x0"), results[21].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("    0x1"), results[22].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR(" 0x8421"), results[23].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("     0x0"), results[24].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("     0x1"), results[25].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("  0x8421"), results[26].statement);
}

static void test_1_07()
{
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    // 1. Setup

    pantheios::be::test::reset();

    // 2. Create test data

    pantheios::sint64_t   i0 = static_cast<pantheios::sint64_t>(0);
    pantheios::sint64_t   i1 = static_cast<pantheios::sint64_t>(1);
    pantheios::sint64_t   i2 = static_cast<pantheios::sint64_t>(STLSOFT_GEN_UINT64_SUFFIX(0x1234567812345678));


    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 1));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 1));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 1));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 2));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 2));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 2));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 3));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 3));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 3));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 4));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 4));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 4));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 5));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 5));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 5));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 6));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 6));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 6));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 7));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 7));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 7));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 8));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 8));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 8));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 9));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 9));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 9));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 10));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 10));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 10));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 11));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 11));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 11));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 12));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 12));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 12));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 13));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 13));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 13));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 14));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 14));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 14));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 15));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 15));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 15));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 16));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 16));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 16));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 17));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 17));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 17));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 18));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 18));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 18));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 19));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 19));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 19));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 20));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 20));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 20));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 21));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 21));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 21));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 22));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 22));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 22));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(69u, results.size()));

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),                results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),                results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[2].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),                results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),                results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[5].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),                results[6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),                results[7].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[8].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),                results[9].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),                results[10].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[11].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x00"),               results[12].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x01"),               results[13].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[14].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x000"),              results[15].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x001"),              results[16].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[17].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000"),             results[18].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0001"),             results[19].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[20].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000"),            results[21].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00001"),            results[22].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[23].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000"),           results[24].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x000001"),           results[25].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[26].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000"),          results[27].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000001"),          results[28].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[29].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000"),         results[30].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000001"),         results[31].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[32].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000000"),        results[33].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000001"),        results[34].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[35].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000000"),       results[36].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000001"),       results[37].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[38].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000000"),      results[39].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000001"),      results[40].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[41].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000000000"),     results[42].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000000001"),     results[43].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[44].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000000000"),    results[45].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000000001"),    results[46].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[47].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000000000"),   results[48].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000000001"),   results[49].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[50].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000000000000"),   results[51].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000000000001"),   results[52].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"),  results[53].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000000000000"),   results[54].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000000000001"),   results[55].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"),   results[56].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000000000000"),   results[57].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000000000001"),   results[58].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x01234567812345678"),   results[59].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000000000000000"),   results[60].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000000000000001"),   results[61].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x001234567812345678"),   results[62].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000000000000000"),   results[63].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000000000000001"),   results[64].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0001234567812345678"),   results[65].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000000000000000"),   results[66].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000000000000001"),   results[67].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00001234567812345678"),   results[68].statement);
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
}

static void test_1_08()
{
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    // 1. Setup

    pantheios::be::test::reset();

    // 2. Create test data

    stlsoft::uint64_t   i0 = static_cast<pantheios::sint64_t>(0);
    stlsoft::uint64_t   i1 = static_cast<pantheios::sint64_t>(1);
    stlsoft::uint64_t   i2 = static_cast<pantheios::sint64_t>(STLSOFT_GEN_UINT64_SUFFIX(0x1234567812345678));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 1));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 1));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 1));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 2));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 2));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 2));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 3));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 3));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 3));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 4));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 4));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 4));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 5));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 5));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 5));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 6));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 6));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 6));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 7));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 7));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 7));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 8));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 8));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 8));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 9));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 9));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 9));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 10));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 10));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 10));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 11));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 11));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 11));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 12));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 12));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 12));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 13));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 13));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 13));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 14));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 14));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 14));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 15));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 15));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 15));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 16));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 16));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 16));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 17));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 17));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 17));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 18));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 18));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 18));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 19));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 19));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 19));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 20));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 20));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 20));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 21));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 21));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 21));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::fullHex | 22));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::fullHex | 22));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::fullHex | 22));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(69u, results.size()));

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),                results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),                results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[2].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),                results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),                results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[5].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),                results[6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),                results[7].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[8].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),                results[9].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),                results[10].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[11].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x00"),               results[12].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x01"),               results[13].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[14].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x000"),              results[15].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x001"),              results[16].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[17].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000"),             results[18].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0001"),             results[19].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[20].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000"),            results[21].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00001"),            results[22].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[23].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000"),           results[24].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x000001"),           results[25].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[26].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000"),          results[27].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000001"),          results[28].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[29].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000"),         results[30].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000001"),         results[31].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[32].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000000"),        results[33].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000001"),        results[34].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[35].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000000"),       results[36].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000001"),       results[37].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[38].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000000"),      results[39].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000001"),      results[40].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[41].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000000000"),     results[42].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000000001"),     results[43].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[44].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000000000"),    results[45].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000000001"),    results[46].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[47].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000000000"),   results[48].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000000001"),   results[49].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[50].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000000000000"),   results[51].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000000000001"),   results[52].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"),  results[53].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000000000000"),   results[54].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000000000001"),   results[55].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"),   results[56].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000000000000"),   results[57].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000000000001"),   results[58].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x01234567812345678"),   results[59].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000000000000000"),   results[60].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x000000000000000001"),   results[61].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x001234567812345678"),   results[62].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000000000000000"),   results[63].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000000000000000001"),   results[64].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0001234567812345678"),   results[65].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000000000000000"),   results[66].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00000000000000000001"),   results[67].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00001234567812345678"),   results[68].statement);
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
}

static void test_1_09()
{
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    // 1. Setup

    pantheios::be::test::reset();

    // 2. Create test data

    pantheios::sint64_t   i0 = static_cast<pantheios::sint64_t>(0);
    pantheios::sint64_t   i1 = static_cast<pantheios::sint64_t>(1);
    pantheios::sint64_t   i2 = static_cast<pantheios::sint64_t>(STLSOFT_GEN_UINT64_SUFFIX(0x1234567812345678));


    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 1));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 1));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 1));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 2));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 2));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 2));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 3));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 3));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 3));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 4));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 4));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 4));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 5));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 5));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 5));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 6));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 6));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 6));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 7));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 7));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 7));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 8));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 8));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 8));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 9));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 9));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 9));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 10));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 10));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 10));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 11));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 11));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 11));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 12));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 12));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 12));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 13));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 13));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 13));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 14));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 14));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 14));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 15));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 15));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 15));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 16));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 16));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 16));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 17));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 17));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 17));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 18));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 18));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 18));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 19));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 19));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 19));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 20));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 20));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 20));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 21));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 21));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 21));

    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 22));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 22));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex | 22));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(69u, results.size()));

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),                results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),                results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[2].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),                results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),                results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[5].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),                results[6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),                results[7].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[8].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),                results[9].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),                results[10].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[11].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR(" 0x0"),               results[12].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR(" 0x1"),               results[13].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[14].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("  0x0"),              results[15].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("  0x1"),              results[16].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[17].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("   0x0"),             results[18].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("   0x1"),             results[19].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[20].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("    0x0"),            results[21].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("    0x1"),            results[22].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[23].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("     0x0"),           results[24].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("     0x1"),           results[25].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[26].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("      0x0"),          results[27].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("      0x1"),          results[28].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[29].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("       0x0"),         results[30].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("       0x1"),         results[31].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[32].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("        0x0"),        results[33].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("        0x1"),        results[34].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[35].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("         0x0"),       results[36].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("         0x1"),       results[37].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[38].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("          0x0"),      results[39].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("          0x1"),      results[40].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[41].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("           0x0"),     results[42].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("           0x1"),     results[43].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[44].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("            0x0"),    results[45].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("            0x1"),    results[46].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[47].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("             0x0"),   results[48].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("             0x1"),   results[49].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[50].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("              0x0"),  results[51].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("              0x1"),  results[52].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[53].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("               0x0"),     results[54].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("               0x1"),     results[55].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"),     results[56].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("                0x0"),    results[57].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                0x1"),    results[58].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR(" 0x1234567812345678"),    results[59].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("                 0x0"),   results[60].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                 0x1"),   results[61].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("  0x1234567812345678"),   results[62].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("                  0x0"),  results[63].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                  0x1"),  results[64].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("   0x1234567812345678"),  results[65].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("                   0x0"), results[66].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                   0x1"), results[67].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("    0x1234567812345678"), results[68].statement);
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
}

static void test_1_10()
{
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    // 1. Setup

    pantheios::be::test::reset();

    // 2. Create test data

    pantheios::sint64_t   i0 = static_cast<pantheios::sint64_t>(0);
    pantheios::sint64_t   i1 = static_cast<pantheios::sint64_t>(1);
    pantheios::sint64_t   i2 = static_cast<pantheios::sint64_t>(STLSOFT_GEN_UINT64_SUFFIX(0x1234567812345678));


    pantheios::log_NOTICE(pantheios::integer(i0, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -1, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -2, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -3, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -3, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -3, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -4, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -4, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -4, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -5, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -5, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -5, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -6, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -6, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -6, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -7, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -7, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -7, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -8, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -8, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -8, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -9, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -9, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -9, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -10, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -10, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -10, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -11, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -11, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -11, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -12, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -12, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -12, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -13, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -13, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -13, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -14, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -14, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -14, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -15, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -15, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -15, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -16, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -16, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -16, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -17, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -17, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -17, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -18, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -18, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -18, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -19, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -19, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -19, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -20, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -20, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -20, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -21, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -21, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -21, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(i0, -22, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1, -22, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2, -22, pantheios::fmt::zeroXPrefix | pantheios::fmt::hex));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(69u, results.size()));

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),                results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),                results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[2].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),                results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),                results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[5].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),                results[6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),                results[7].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[8].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),                results[9].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1"),                results[10].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[11].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0 "),               results[12].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1 "),               results[13].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[14].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0  "),              results[15].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1  "),              results[16].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[17].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0   "),             results[18].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1   "),             results[19].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[20].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0    "),            results[21].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1    "),            results[22].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[23].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0     "),           results[24].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1     "),           results[25].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[26].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0      "),          results[27].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1      "),          results[28].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[29].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0       "),         results[30].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1       "),         results[31].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[32].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0        "),        results[33].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1        "),        results[34].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[35].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0         "),       results[36].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1         "),       results[37].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[38].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0          "),      results[39].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1          "),      results[40].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[41].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0           "),     results[42].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1           "),     results[43].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[44].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0            "),    results[45].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1            "),    results[46].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[47].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0             "),   results[48].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1             "),   results[49].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[50].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0              "),  results[51].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1              "),  results[52].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[53].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0               "), results[54].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1               "), results[55].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678"), results[56].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0                "),    results[57].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1                "),    results[58].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678 "),    results[59].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0                 "),   results[60].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1                 "),   results[61].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678  "),   results[62].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0                  "),  results[63].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1                  "),  results[64].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678   "),  results[65].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0x0                   "), results[66].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1                   "), results[67].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1234567812345678    "), results[68].statement);
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
}

static void test_1_11()
{
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    // 1. Setup

    pantheios::be::test::reset();

    // 2. Create test data

    pantheios::sint64_t   i0 = static_cast<pantheios::sint64_t>(0);
    pantheios::sint64_t   i1 = static_cast<pantheios::sint64_t>(1);
    pantheios::sint64_t   i2 = static_cast<pantheios::sint64_t>(STLSOFT_GEN_UINT64_SUFFIX(1234567812345678));


    pantheios::log_NOTICE(pantheios::integer(i0));
    pantheios::log_NOTICE(pantheios::integer(i1));
    pantheios::log_NOTICE(pantheios::integer(i2));

    pantheios::log_NOTICE(pantheios::integer(i0, -1));
    pantheios::log_NOTICE(pantheios::integer(i1, -1));
    pantheios::log_NOTICE(pantheios::integer(i2, -1));

    pantheios::log_NOTICE(pantheios::integer(i0, -2));
    pantheios::log_NOTICE(pantheios::integer(i1, -2));
    pantheios::log_NOTICE(pantheios::integer(i2, -2));

    pantheios::log_NOTICE(pantheios::integer(i0, -3));
    pantheios::log_NOTICE(pantheios::integer(i1, -3));
    pantheios::log_NOTICE(pantheios::integer(i2, -3));

    pantheios::log_NOTICE(pantheios::integer(i0, -4));
    pantheios::log_NOTICE(pantheios::integer(i1, -4));
    pantheios::log_NOTICE(pantheios::integer(i2, -4));

    pantheios::log_NOTICE(pantheios::integer(i0, -5));
    pantheios::log_NOTICE(pantheios::integer(i1, -5));
    pantheios::log_NOTICE(pantheios::integer(i2, -5));

    pantheios::log_NOTICE(pantheios::integer(i0, -6));
    pantheios::log_NOTICE(pantheios::integer(i1, -6));
    pantheios::log_NOTICE(pantheios::integer(i2, -6));

    pantheios::log_NOTICE(pantheios::integer(i0, -7));
    pantheios::log_NOTICE(pantheios::integer(i1, -7));
    pantheios::log_NOTICE(pantheios::integer(i2, -7));

    pantheios::log_NOTICE(pantheios::integer(i0, -8));
    pantheios::log_NOTICE(pantheios::integer(i1, -8));
    pantheios::log_NOTICE(pantheios::integer(i2, -8));

    pantheios::log_NOTICE(pantheios::integer(i0, -9));
    pantheios::log_NOTICE(pantheios::integer(i1, -9));
    pantheios::log_NOTICE(pantheios::integer(i2, -9));

    pantheios::log_NOTICE(pantheios::integer(i0, -10));
    pantheios::log_NOTICE(pantheios::integer(i1, -10));
    pantheios::log_NOTICE(pantheios::integer(i2, -10));

    pantheios::log_NOTICE(pantheios::integer(i0, -11));
    pantheios::log_NOTICE(pantheios::integer(i1, -11));
    pantheios::log_NOTICE(pantheios::integer(i2, -11));

    pantheios::log_NOTICE(pantheios::integer(i0, -12));
    pantheios::log_NOTICE(pantheios::integer(i1, -12));
    pantheios::log_NOTICE(pantheios::integer(i2, -12));

    pantheios::log_NOTICE(pantheios::integer(i0, -13));
    pantheios::log_NOTICE(pantheios::integer(i1, -13));
    pantheios::log_NOTICE(pantheios::integer(i2, -13));

    pantheios::log_NOTICE(pantheios::integer(i0, -14));
    pantheios::log_NOTICE(pantheios::integer(i1, -14));
    pantheios::log_NOTICE(pantheios::integer(i2, -14));

    pantheios::log_NOTICE(pantheios::integer(i0, -15));
    pantheios::log_NOTICE(pantheios::integer(i1, -15));
    pantheios::log_NOTICE(pantheios::integer(i2, -15));

    pantheios::log_NOTICE(pantheios::integer(i0, -16));
    pantheios::log_NOTICE(pantheios::integer(i1, -16));
    pantheios::log_NOTICE(pantheios::integer(i2, -16));

    pantheios::log_NOTICE(pantheios::integer(i0, -17));
    pantheios::log_NOTICE(pantheios::integer(i1, -17));
    pantheios::log_NOTICE(pantheios::integer(i2, -17));

    pantheios::log_NOTICE(pantheios::integer(i0, -18));
    pantheios::log_NOTICE(pantheios::integer(i1, -18));
    pantheios::log_NOTICE(pantheios::integer(i2, -18));

    pantheios::log_NOTICE(pantheios::integer(i0, -19));
    pantheios::log_NOTICE(pantheios::integer(i1, -19));
    pantheios::log_NOTICE(pantheios::integer(i2, -19));

    pantheios::log_NOTICE(pantheios::integer(i0, -20));
    pantheios::log_NOTICE(pantheios::integer(i1, -20));
    pantheios::log_NOTICE(pantheios::integer(i2, -20));

    pantheios::log_NOTICE(pantheios::integer(i0, -21));
    pantheios::log_NOTICE(pantheios::integer(i1, -21));
    pantheios::log_NOTICE(pantheios::integer(i2, -21));

    pantheios::log_NOTICE(pantheios::integer(i0, -22));
    pantheios::log_NOTICE(pantheios::integer(i1, -22));
    pantheios::log_NOTICE(pantheios::integer(i2, -22));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(69u, results.size()));

    XTESTS_TEST_STRING_EQUAL(PSTR("0"),                results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1"),                results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[2].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0"),                results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1"),                results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[5].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0 "),               results[6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1 "),               results[7].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[8].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0  "),              results[9].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1  "),              results[10].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[11].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0   "),             results[12].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1   "),             results[13].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[14].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0    "),            results[15].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1    "),            results[16].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[17].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0     "),           results[18].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1     "),           results[19].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[20].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0      "),          results[21].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1      "),          results[22].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[23].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0       "),         results[24].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1       "),         results[25].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[26].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0        "),        results[27].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1        "),        results[28].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[29].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0         "),       results[30].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1         "),       results[31].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[32].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0          "),      results[33].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1          "),      results[34].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[35].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0           "),     results[36].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1           "),     results[37].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[38].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0            "),    results[39].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1            "),    results[40].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[41].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0             "),   results[42].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1             "),   results[43].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[44].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0              "),  results[45].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1              "),  results[46].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[47].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0               "), results[48].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1               "), results[49].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[50].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0                "), results[51].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1                "), results[52].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678 "), results[53].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0                 "), results[54].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1                 "), results[55].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678  "), results[56].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0                  "), results[57].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1                  "), results[58].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678   "), results[59].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0                   "), results[60].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1                   "), results[61].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678    "), results[62].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0                    "), results[63].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1                    "), results[64].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678     "), results[65].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0                     "), results[66].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1                     "), results[67].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678      "), results[68].statement);
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
}

static void test_1_12()
{
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    // 1. Setup

    pantheios::be::test::reset();

    // 2. Create test data

    pantheios::sint64_t   i0 = static_cast<pantheios::sint64_t>(0);
    pantheios::sint64_t   i1 = static_cast<pantheios::sint64_t>(1);
    pantheios::sint64_t   i2 = static_cast<pantheios::sint64_t>(STLSOFT_GEN_UINT64_SUFFIX(1234567812345678));


    pantheios::log_NOTICE(pantheios::integer(i0));
    pantheios::log_NOTICE(pantheios::integer(i1));
    pantheios::log_NOTICE(pantheios::integer(i2));

    pantheios::log_NOTICE(pantheios::integer(i0, 1));
    pantheios::log_NOTICE(pantheios::integer(i1, 1));
    pantheios::log_NOTICE(pantheios::integer(i2, 1));

    pantheios::log_NOTICE(pantheios::integer(i0, 2));
    pantheios::log_NOTICE(pantheios::integer(i1, 2));
    pantheios::log_NOTICE(pantheios::integer(i2, 2));

    pantheios::log_NOTICE(pantheios::integer(i0, 3));
    pantheios::log_NOTICE(pantheios::integer(i1, 3));
    pantheios::log_NOTICE(pantheios::integer(i2, 3));

    pantheios::log_NOTICE(pantheios::integer(i0, 4));
    pantheios::log_NOTICE(pantheios::integer(i1, 4));
    pantheios::log_NOTICE(pantheios::integer(i2, 4));

    pantheios::log_NOTICE(pantheios::integer(i0, 5));
    pantheios::log_NOTICE(pantheios::integer(i1, 5));
    pantheios::log_NOTICE(pantheios::integer(i2, 5));

    pantheios::log_NOTICE(pantheios::integer(i0, 6));
    pantheios::log_NOTICE(pantheios::integer(i1, 6));
    pantheios::log_NOTICE(pantheios::integer(i2, 6));

    pantheios::log_NOTICE(pantheios::integer(i0, 7));
    pantheios::log_NOTICE(pantheios::integer(i1, 7));
    pantheios::log_NOTICE(pantheios::integer(i2, 7));

    pantheios::log_NOTICE(pantheios::integer(i0, 8));
    pantheios::log_NOTICE(pantheios::integer(i1, 8));
    pantheios::log_NOTICE(pantheios::integer(i2, 8));

    pantheios::log_NOTICE(pantheios::integer(i0, 9));
    pantheios::log_NOTICE(pantheios::integer(i1, 9));
    pantheios::log_NOTICE(pantheios::integer(i2, 9));

    pantheios::log_NOTICE(pantheios::integer(i0, 10));
    pantheios::log_NOTICE(pantheios::integer(i1, 10));
    pantheios::log_NOTICE(pantheios::integer(i2, 10));

    pantheios::log_NOTICE(pantheios::integer(i0, 11));
    pantheios::log_NOTICE(pantheios::integer(i1, 11));
    pantheios::log_NOTICE(pantheios::integer(i2, 11));

    pantheios::log_NOTICE(pantheios::integer(i0, 12));
    pantheios::log_NOTICE(pantheios::integer(i1, 12));
    pantheios::log_NOTICE(pantheios::integer(i2, 12));

    pantheios::log_NOTICE(pantheios::integer(i0, 13));
    pantheios::log_NOTICE(pantheios::integer(i1, 13));
    pantheios::log_NOTICE(pantheios::integer(i2, 13));

    pantheios::log_NOTICE(pantheios::integer(i0, 14));
    pantheios::log_NOTICE(pantheios::integer(i1, 14));
    pantheios::log_NOTICE(pantheios::integer(i2, 14));

    pantheios::log_NOTICE(pantheios::integer(i0, 15));
    pantheios::log_NOTICE(pantheios::integer(i1, 15));
    pantheios::log_NOTICE(pantheios::integer(i2, 15));

    pantheios::log_NOTICE(pantheios::integer(i0, 16));
    pantheios::log_NOTICE(pantheios::integer(i1, 16));
    pantheios::log_NOTICE(pantheios::integer(i2, 16));

    pantheios::log_NOTICE(pantheios::integer(i0, 17));
    pantheios::log_NOTICE(pantheios::integer(i1, 17));
    pantheios::log_NOTICE(pantheios::integer(i2, 17));

    pantheios::log_NOTICE(pantheios::integer(i0, 18));
    pantheios::log_NOTICE(pantheios::integer(i1, 18));
    pantheios::log_NOTICE(pantheios::integer(i2, 18));

    pantheios::log_NOTICE(pantheios::integer(i0, 19));
    pantheios::log_NOTICE(pantheios::integer(i1, 19));
    pantheios::log_NOTICE(pantheios::integer(i2, 19));

    pantheios::log_NOTICE(pantheios::integer(i0, 20));
    pantheios::log_NOTICE(pantheios::integer(i1, 20));
    pantheios::log_NOTICE(pantheios::integer(i2, 20));

    pantheios::log_NOTICE(pantheios::integer(i0, 21));
    pantheios::log_NOTICE(pantheios::integer(i1, 21));
    pantheios::log_NOTICE(pantheios::integer(i2, 21));

    pantheios::log_NOTICE(pantheios::integer(i0, 22));
    pantheios::log_NOTICE(pantheios::integer(i1, 22));
    pantheios::log_NOTICE(pantheios::integer(i2, 22));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(69u, results.size()));

    XTESTS_TEST_STRING_EQUAL(PSTR("0"),                results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1"),                results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[2].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0"),                results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1"),                results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[5].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR(" 0"),               results[6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR(" 1"),               results[7].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[8].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("  0"),              results[9].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("  1"),              results[10].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[11].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("   0"),             results[12].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("   1"),             results[13].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[14].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("    0"),            results[15].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("    1"),            results[16].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[17].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("     0"),           results[18].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("     1"),           results[19].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[20].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("      0"),          results[21].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("      1"),          results[22].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[23].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("       0"),         results[24].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("       1"),         results[25].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[26].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("        0"),        results[27].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("        1"),        results[28].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[29].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("         0"),       results[30].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("         1"),       results[31].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[32].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("          0"),      results[33].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("          1"),      results[34].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[35].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("           0"),     results[36].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("           1"),     results[37].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[38].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("            0"),    results[39].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("            1"),    results[40].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[41].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("             0"),   results[42].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("             1"),   results[43].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[44].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("              0"),  results[45].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("              1"),  results[46].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[47].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("               0"), results[48].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("               1"), results[49].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"), results[50].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("                0"), results[51].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                1"), results[52].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR(" 1234567812345678"), results[53].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("                 0"), results[54].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                 1"), results[55].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("  1234567812345678"), results[56].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("                  0"), results[57].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                  1"), results[58].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("   1234567812345678"), results[59].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("                   0"), results[60].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                   1"), results[61].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("    1234567812345678"), results[62].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("                    0"), results[63].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                    1"), results[64].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("     1234567812345678"), results[65].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("                     0"), results[66].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                     1"), results[67].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("      1234567812345678"), results[68].statement);
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
}

static void test_1_13()
{
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    // 1. Setup

    pantheios::be::test::reset();

    // 2. Create test data

    pantheios::sint64_t   i0 = static_cast<pantheios::sint64_t>(0);
    pantheios::sint64_t   i1 = static_cast<pantheios::sint64_t>(1);
    pantheios::sint64_t   i2 = -static_cast<pantheios::sint64_t>(STLSOFT_GEN_UINT64_SUFFIX(1234567812345678));


    pantheios::log_NOTICE(pantheios::integer(i0));
    pantheios::log_NOTICE(pantheios::integer(i1));
    pantheios::log_NOTICE(pantheios::integer(i2));

    pantheios::log_NOTICE(pantheios::integer(i0, 1));
    pantheios::log_NOTICE(pantheios::integer(i1, 1));
    pantheios::log_NOTICE(pantheios::integer(i2, 1));

    pantheios::log_NOTICE(pantheios::integer(i0, 2));
    pantheios::log_NOTICE(pantheios::integer(i1, 2));
    pantheios::log_NOTICE(pantheios::integer(i2, 2));

    pantheios::log_NOTICE(pantheios::integer(i0, 3));
    pantheios::log_NOTICE(pantheios::integer(i1, 3));
    pantheios::log_NOTICE(pantheios::integer(i2, 3));

    pantheios::log_NOTICE(pantheios::integer(i0, 4));
    pantheios::log_NOTICE(pantheios::integer(i1, 4));
    pantheios::log_NOTICE(pantheios::integer(i2, 4));

    pantheios::log_NOTICE(pantheios::integer(i0, 5));
    pantheios::log_NOTICE(pantheios::integer(i1, 5));
    pantheios::log_NOTICE(pantheios::integer(i2, 5));

    pantheios::log_NOTICE(pantheios::integer(i0, 6));
    pantheios::log_NOTICE(pantheios::integer(i1, 6));
    pantheios::log_NOTICE(pantheios::integer(i2, 6));

    pantheios::log_NOTICE(pantheios::integer(i0, 7));
    pantheios::log_NOTICE(pantheios::integer(i1, 7));
    pantheios::log_NOTICE(pantheios::integer(i2, 7));

    pantheios::log_NOTICE(pantheios::integer(i0, 8));
    pantheios::log_NOTICE(pantheios::integer(i1, 8));
    pantheios::log_NOTICE(pantheios::integer(i2, 8));

    pantheios::log_NOTICE(pantheios::integer(i0, 9));
    pantheios::log_NOTICE(pantheios::integer(i1, 9));
    pantheios::log_NOTICE(pantheios::integer(i2, 9));

    pantheios::log_NOTICE(pantheios::integer(i0, 10));
    pantheios::log_NOTICE(pantheios::integer(i1, 10));
    pantheios::log_NOTICE(pantheios::integer(i2, 10));

    pantheios::log_NOTICE(pantheios::integer(i0, 11));
    pantheios::log_NOTICE(pantheios::integer(i1, 11));
    pantheios::log_NOTICE(pantheios::integer(i2, 11));

    pantheios::log_NOTICE(pantheios::integer(i0, 12));
    pantheios::log_NOTICE(pantheios::integer(i1, 12));
    pantheios::log_NOTICE(pantheios::integer(i2, 12));

    pantheios::log_NOTICE(pantheios::integer(i0, 13));
    pantheios::log_NOTICE(pantheios::integer(i1, 13));
    pantheios::log_NOTICE(pantheios::integer(i2, 13));

    pantheios::log_NOTICE(pantheios::integer(i0, 14));
    pantheios::log_NOTICE(pantheios::integer(i1, 14));
    pantheios::log_NOTICE(pantheios::integer(i2, 14));

    pantheios::log_NOTICE(pantheios::integer(i0, 15));
    pantheios::log_NOTICE(pantheios::integer(i1, 15));
    pantheios::log_NOTICE(pantheios::integer(i2, 15));

    pantheios::log_NOTICE(pantheios::integer(i0, 16));
    pantheios::log_NOTICE(pantheios::integer(i1, 16));
    pantheios::log_NOTICE(pantheios::integer(i2, 16));

    pantheios::log_NOTICE(pantheios::integer(i0, 17));
    pantheios::log_NOTICE(pantheios::integer(i1, 17));
    pantheios::log_NOTICE(pantheios::integer(i2, 17));

    pantheios::log_NOTICE(pantheios::integer(i0, 18));
    pantheios::log_NOTICE(pantheios::integer(i1, 18));
    pantheios::log_NOTICE(pantheios::integer(i2, 18));

    pantheios::log_NOTICE(pantheios::integer(i0, 19));
    pantheios::log_NOTICE(pantheios::integer(i1, 19));
    pantheios::log_NOTICE(pantheios::integer(i2, 19));

    pantheios::log_NOTICE(pantheios::integer(i0, 20));
    pantheios::log_NOTICE(pantheios::integer(i1, 20));
    pantheios::log_NOTICE(pantheios::integer(i2, 20));

    pantheios::log_NOTICE(pantheios::integer(i0, 21));
    pantheios::log_NOTICE(pantheios::integer(i1, 21));
    pantheios::log_NOTICE(pantheios::integer(i2, 21));

    pantheios::log_NOTICE(pantheios::integer(i0, 22));
    pantheios::log_NOTICE(pantheios::integer(i1, 22));
    pantheios::log_NOTICE(pantheios::integer(i2, 22));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(69u, results.size()));

    XTESTS_TEST_STRING_EQUAL(PSTR("0"),                 results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1"),                 results[1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[2].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0"),                 results[3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1"),                 results[4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[5].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR(" 0"),                results[6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR(" 1"),                results[7].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[8].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("  0"),               results[9].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("  1"),               results[10].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[11].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("   0"),              results[12].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("   1"),              results[13].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[14].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("    0"),             results[15].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("    1"),             results[16].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[17].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("     0"),            results[18].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("     1"),            results[19].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[20].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("      0"),           results[21].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("      1"),           results[22].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[23].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("       0"),          results[24].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("       1"),          results[25].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[26].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("        0"),         results[27].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("        1"),         results[28].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[29].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("         0"),        results[30].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("         1"),        results[31].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[32].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("          0"),       results[33].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("          1"),       results[34].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[35].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("           0"),      results[36].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("           1"),      results[37].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[38].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("            0"),     results[39].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("            1"),     results[40].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[41].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("             0"),    results[42].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("             1"),    results[43].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[44].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("              0"),   results[45].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("              1"),   results[46].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[47].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("               0"),  results[48].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("               1"),  results[49].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[50].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("                0"), results[51].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                1"), results[52].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"), results[53].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("                 0"), results[54].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                 1"), results[55].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR(" -1234567812345678"), results[56].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("                  0"), results[57].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                  1"), results[58].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("  -1234567812345678"), results[59].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("                   0"), results[60].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                   1"), results[61].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("   -1234567812345678"), results[62].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("                    0"), results[63].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                    1"), results[64].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("    -1234567812345678"), results[65].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("                     0"), results[66].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                     1"), results[67].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("     -1234567812345678"), results[68].statement);
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
}

static void test_1_14()
{
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    // 1. Setup

    pantheios::be::test::reset();

    // 2. Create test data

    pantheios::sint8_t    i0 =  static_cast<pantheios::sint8_t>(-127);
    pantheios::uint8_t    i1 =  static_cast<pantheios::uint8_t>(11);
    pantheios::sint16_t   i2 =  static_cast<pantheios::sint16_t>(-300);
    pantheios::uint16_t   i3 =  static_cast<pantheios::uint16_t>(0);
    pantheios::sint32_t   i4 =  static_cast<pantheios::sint32_t>(-100000);
    pantheios::uint32_t   i5 =  static_cast<pantheios::sint32_t>(+123456);
    pantheios::sint64_t   i6 = -static_cast<pantheios::sint64_t>(STLSOFT_GEN_UINT64_SUFFIX(1234567812345678));
    pantheios::uint64_t   i7 =  static_cast<pantheios::uint64_t>(STLSOFT_GEN_UINT64_SUFFIX(1234567812345678));


    pantheios::log_NOTICE(pantheios::integer(i0));
    pantheios::log_NOTICE(pantheios::integer(i1));
    pantheios::log_NOTICE(pantheios::integer(i2));
    pantheios::log_NOTICE(pantheios::integer(i3));
    pantheios::log_NOTICE(pantheios::integer(i4));
    pantheios::log_NOTICE(pantheios::integer(i5));
    pantheios::log_NOTICE(pantheios::integer(i6));
    pantheios::log_NOTICE(pantheios::integer(i7));

    pantheios::log_NOTICE(pantheios::integer(i0, +10, 0));
    pantheios::log_NOTICE(pantheios::integer(i1, +10, 0));
    pantheios::log_NOTICE(pantheios::integer(i2, +10, 0));
    pantheios::log_NOTICE(pantheios::integer(i3, +10, 0));
    pantheios::log_NOTICE(pantheios::integer(i4, +10, 0));
    pantheios::log_NOTICE(pantheios::integer(i5, +10, 0));
    pantheios::log_NOTICE(pantheios::integer(i6, +10, 0));
    pantheios::log_NOTICE(pantheios::integer(i7, +10, 0));

    pantheios::log_NOTICE(pantheios::integer(i0, -10, 0));
    pantheios::log_NOTICE(pantheios::integer(i1, -10, 0));
    pantheios::log_NOTICE(pantheios::integer(i2, -10, 0));
    pantheios::log_NOTICE(pantheios::integer(i3, -10, 0));
    pantheios::log_NOTICE(pantheios::integer(i4, -10, 0));
    pantheios::log_NOTICE(pantheios::integer(i5, -10, 0));
    pantheios::log_NOTICE(pantheios::integer(i6, -10, 0));
    pantheios::log_NOTICE(pantheios::integer(i7, -10, 0));

    pantheios::log_NOTICE(pantheios::integer(i0, +20, 0));
    pantheios::log_NOTICE(pantheios::integer(i1, +20, 0));
    pantheios::log_NOTICE(pantheios::integer(i2, +20, 0));
    pantheios::log_NOTICE(pantheios::integer(i3, +20, 0));
    pantheios::log_NOTICE(pantheios::integer(i4, +20, 0));
    pantheios::log_NOTICE(pantheios::integer(i5, +20, 0));
    pantheios::log_NOTICE(pantheios::integer(i6, +20, 0));
    pantheios::log_NOTICE(pantheios::integer(i7, +20, 0));

    pantheios::log_NOTICE(pantheios::integer(i0, -20, 0));
    pantheios::log_NOTICE(pantheios::integer(i1, -20, 0));
    pantheios::log_NOTICE(pantheios::integer(i2, -20, 0));
    pantheios::log_NOTICE(pantheios::integer(i3, -20, 0));
    pantheios::log_NOTICE(pantheios::integer(i4, -20, 0));
    pantheios::log_NOTICE(pantheios::integer(i5, -20, 0));
    pantheios::log_NOTICE(pantheios::integer(i6, -20, 0));
    pantheios::log_NOTICE(pantheios::integer(i7, -20, 0));


    pantheios::log_NOTICE(pantheios::integer(i0,  0, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i1,  0, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i2,  0, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i3,  0, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i4,  0, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i5,  0, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i6,  0, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(i7,  0, pantheios::fmt::hex));


    pantheios::log_NOTICE(pantheios::integer(i0,  0, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i1,  0, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i2,  0, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i3,  0, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i4,  0, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i5,  0, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i6,  0, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i7,  0, pantheios::fmt::hex | pantheios::fmt::zeroPad));


    pantheios::log_NOTICE(pantheios::integer(i0,  2, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i1,  2, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i2,  4, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i3,  4, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i4,  8, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i5,  8, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i6, 16, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i7, 16, pantheios::fmt::hex | pantheios::fmt::zeroPad));


    pantheios::log_NOTICE(pantheios::integer(i0,  0, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i1,  0, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i2,  0, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i3,  0, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i4,  0, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i5,  0, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i6,  0, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i7,  0, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));


    pantheios::log_NOTICE(pantheios::integer(i0,  2, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i1,  2, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i2,  4, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i3,  4, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i4,  8, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i5,  8, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i6, 16, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i7, 16, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));


    pantheios::log_NOTICE(pantheios::integer(i0,  4, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i1,  4, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i2,  6, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i3,  6, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i4, 10, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i5, 10, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i6, 18, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));
    pantheios::log_NOTICE(pantheios::integer(i7, 18, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix));


    pantheios::log_NOTICE(pantheios::integer(i0,  2, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i1,  2, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i2,  4, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i3,  4, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i4,  8, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i5,  8, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i6, 16, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(i7, 16, pantheios::fmt::hex | pantheios::fmt::zeroPad));


    pantheios::log_NOTICE(pantheios::integer(i0,  2, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i1,  2, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i2,  4, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i3,  4, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i4,  8, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i5,  8, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i6, 16, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i7, 16, pantheios::fmt::fullHex));


    pantheios::log_NOTICE(pantheios::integer(i0,  4, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i1,  4, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i2,  6, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i3,  6, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i4, 10, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i5, 10, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i6, 18, pantheios::fmt::fullHex));
    pantheios::log_NOTICE(pantheios::integer(i7, 18, pantheios::fmt::fullHex));



    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(112u, results.size()));

    size_t base = 0;

    XTESTS_TEST_STRING_EQUAL(PSTR("-127"),                    results[base + 0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("11"),                      results[base + 1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-300"),                    results[base + 2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0"),                       results[base + 3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-100000"),                 results[base + 4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("123456"),                  results[base + 5].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"),       results[base + 6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"),        results[base + 7].statement);

    base += 8;
    XTESTS_TEST_STRING_EQUAL(PSTR("      -127"),              results[base + 0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("        11"),              results[base + 1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("      -300"),              results[base + 2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("         0"),              results[base + 3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("   -100000"),              results[base + 4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("    123456"),              results[base + 5].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"),       results[base + 6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"),        results[base + 7].statement);

    base += 8;
    XTESTS_TEST_STRING_EQUAL(PSTR("-127      "),              results[base + 0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("11        "),              results[base + 1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-300      "),              results[base + 2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0         "),              results[base + 3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-100000   "),              results[base + 4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("123456    "),              results[base + 5].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678"),       results[base + 6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678"),        results[base + 7].statement);

    base += 8;
    XTESTS_TEST_STRING_EQUAL(PSTR("                -127"),    results[base + 0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                  11"),    results[base + 1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                -300"),    results[base + 2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("                   0"),    results[base + 3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("             -100000"),    results[base + 4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("              123456"),    results[base + 5].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("   -1234567812345678"),    results[base + 6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("    1234567812345678"),    results[base + 7].statement);

    base += 8;
    XTESTS_TEST_STRING_EQUAL(PSTR("-127                "),    results[base + 0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("11                  "),    results[base + 1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-300                "),    results[base + 2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0                   "),    results[base + 3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-100000             "),    results[base + 4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("123456              "),    results[base + 5].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-1234567812345678   "),    results[base + 6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1234567812345678    "),    results[base + 7].statement);


    base += 8;
    XTESTS_TEST_STRING_EQUAL(PSTR("81"),                      results[base + 0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("b"),                       results[base + 1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fed4"),                    results[base + 2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0"),                       results[base + 3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fffe7960"),                results[base + 4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1e240"),                   results[base + 5].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fffb9d2ac81810b2"),        results[base + 6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("462d537e7ef4e"),           results[base + 7].statement);


    base += 8;
    XTESTS_TEST_STRING_EQUAL(PSTR("81"),                      results[base + 0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("b"),                       results[base + 1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fed4"),                    results[base + 2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0"),                       results[base + 3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fffe7960"),                results[base + 4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("1e240"),                   results[base + 5].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fffb9d2ac81810b2"),        results[base + 6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("462d537e7ef4e"),           results[base + 7].statement);


    base += 8;
    XTESTS_TEST_STRING_EQUAL(PSTR("81"),                      results[base + 0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0b"),                      results[base + 1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fed4"),                    results[base + 2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0000"),                    results[base + 3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fffe7960"),                results[base + 4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0001e240"),                results[base + 5].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fffb9d2ac81810b2"),        results[base + 6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("000462d537e7ef4e"),        results[base + 7].statement);


    base += 8;
    XTESTS_TEST_STRING_EQUAL(PSTR("0x81"),                      results[base + 0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xb"),                       results[base + 1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xfed4"),                    results[base + 2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0"),                       results[base + 3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xfffe7960"),                results[base + 4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x1e240"),                   results[base + 5].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xfffb9d2ac81810b2"),        results[base + 6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x462d537e7ef4e"),           results[base + 7].statement);


    base += 8;
    XTESTS_TEST_STRING_EQUAL(PSTR("0x81"),                      results[base + 0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xb"),                       results[base + 1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xfed4"),                    results[base + 2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR(" 0x0"),                      results[base + 3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xfffe7960"),                results[base + 4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR(" 0x1e240"),                  results[base + 5].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xfffb9d2ac81810b2"),        results[base + 6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR(" 0x462d537e7ef4e"),          results[base + 7].statement);


    base += 8;
    XTESTS_TEST_STRING_EQUAL(PSTR("0x81"),                      results[base + 0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR(" 0xb"),                      results[base + 1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xfed4"),                    results[base + 2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("   0x0"),                    results[base + 3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xfffe7960"),                results[base + 4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("   0x1e240"),                results[base + 5].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xfffb9d2ac81810b2"),        results[base + 6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("   0x462d537e7ef4e"),        results[base + 7].statement);


    base += 8;
    XTESTS_TEST_STRING_EQUAL(PSTR("81"),                        results[base + 0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0b"),                        results[base + 1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fed4"),                      results[base + 2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0000"),                      results[base + 3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fffe7960"),                  results[base + 4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0001e240"),                  results[base + 5].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fffb9d2ac81810b2"),          results[base + 6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("000462d537e7ef4e"),          results[base + 7].statement);


    base += 8;
    XTESTS_TEST_STRING_EQUAL(PSTR("0x81"),                      results[base + 0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xb"),                       results[base + 1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xfed4"),                    results[base + 2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x00"),                      results[base + 3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xfffe7960"),                results[base + 4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x01e240"),                  results[base + 5].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xfffb9d2ac81810b2"),        results[base + 6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0462d537e7ef4e"),          results[base + 7].statement);


    base += 8;
    XTESTS_TEST_STRING_EQUAL(PSTR("0x81"),                      results[base + 0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0b"),                      results[base + 1].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xfed4"),                    results[base + 2].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0000"),                    results[base + 3].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xfffe7960"),                results[base + 4].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x0001e240"),                results[base + 5].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0xfffb9d2ac81810b2"),        results[base + 6].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("0x000462d537e7ef4e"),        results[base + 7].statement);

#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
}

static void test_1_15()
{
    // 1. Setup

    pantheios::be::test::reset();

    // 2. Create test data
    pantheios::log_NOTICE(pantheios::integer( 15));
    pantheios::log_NOTICE(pantheios::integer(-15));

    pantheios::log_NOTICE(pantheios::integer( 15,  0, 0));
    pantheios::log_NOTICE(pantheios::integer(-15,  0, 0));

    pantheios::log_NOTICE(pantheios::integer( 15,  1, 0));
    pantheios::log_NOTICE(pantheios::integer(-15,  1, 0));

    pantheios::log_NOTICE(pantheios::integer( 15,  2, 0));
    pantheios::log_NOTICE(pantheios::integer(-15,  2, 0));

    pantheios::log_NOTICE(pantheios::integer( 15,  3, 0));
    pantheios::log_NOTICE(pantheios::integer(-15,  3, 0));

    pantheios::log_NOTICE(pantheios::integer( 15, -3, 0));
    pantheios::log_NOTICE(pantheios::integer(-15, -3, 0));

    pantheios::log_NOTICE(pantheios::integer( 15,  4, 0));
    pantheios::log_NOTICE(pantheios::integer(-15,  4, 0));

    pantheios::log_NOTICE(pantheios::integer( 15, -4, 0));
    pantheios::log_NOTICE(pantheios::integer(-15, -4, 0));

    pantheios::log_NOTICE(pantheios::integer( 15,  4, pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(-15,  4, pantheios::fmt::zeroPad));

    pantheios::log_NOTICE(pantheios::integer( 15, -4, pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(-15, -4, pantheios::fmt::zeroPad));

    pantheios::log_NOTICE(pantheios::integer(                   15 ,   0, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer( stlsoft::sint8_t(-15),   0, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(stlsoft::sint16_t(-15),   0, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(stlsoft::sint32_t(-15),   0, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(stlsoft::sint64_t(-15),   0, pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(                   15 ,   2, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer( stlsoft::sint8_t(-15),   2, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(stlsoft::sint16_t(-15),   2, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(stlsoft::sint32_t(-15),   2, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(stlsoft::sint64_t(-15),   2, pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(                   15 ,   2, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer( stlsoft::sint8_t(-15),   2, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(stlsoft::sint16_t(-15),   4, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(stlsoft::sint32_t(-15),   8, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(stlsoft::sint64_t(-15),  16, pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(                   15 ,  16, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer( stlsoft::sint8_t(-15),  16, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(stlsoft::sint16_t(-15),  16, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(stlsoft::sint32_t(-15),  16, pantheios::fmt::hex));
    pantheios::log_NOTICE(pantheios::integer(stlsoft::sint64_t(-15),  16, pantheios::fmt::hex));

    pantheios::log_NOTICE(pantheios::integer(                   15 ,  16, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer( stlsoft::sint8_t(-15),  16, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(stlsoft::sint16_t(-15),  16, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(stlsoft::sint32_t(-15),  16, pantheios::fmt::hex | pantheios::fmt::zeroPad));
    pantheios::log_NOTICE(pantheios::integer(stlsoft::sint64_t(-15),  16, pantheios::fmt::hex | pantheios::fmt::zeroPad));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
//    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(80u, results.size()));

    size_t base = 0;

    XTESTS_TEST_STRING_EQUAL(PSTR("15"),                       results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-15"),                      results[base++].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("15"),                       results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-15"),                      results[base++].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("15"),                       results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-15"),                      results[base++].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("15"),                       results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-15"),                      results[base++].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR(" 15"),                      results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-15"),                      results[base++].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("15 "),                      results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-15"),                      results[base++].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("  15"),                     results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR(" -15"),                     results[base++].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("15  "),                     results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-15 "),                     results[base++].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("0015"),                     results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-015"),                     results[base++].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("15  "),                     results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("-15 "),                     results[base++].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("f"),                        results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("f1"),                       results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fff1"),                     results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fffffff1"),                 results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fffffffffffffff1"),         results[base++].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR(" f"),                       results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("f1"),                       results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fff1"),                     results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fffffff1"),                 results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fffffffffffffff1"),         results[base++].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR(" f"),                       results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("f1"),                       results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fff1"),                     results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fffffff1"),                 results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fffffffffffffff1"),         results[base++].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("               f"),         results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("              f1"),         results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("            fff1"),         results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("        fffffff1"),         results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fffffffffffffff1"),         results[base++].statement);

    XTESTS_TEST_STRING_EQUAL(PSTR("000000000000000f"),         results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("00000000000000f1"),         results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("000000000000fff1"),         results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("00000000fffffff1"),         results[base++].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("fffffffffffffff1"),         results[base++].statement);
}

/* ///////////////////////////// end of file //////////////////////////// */
