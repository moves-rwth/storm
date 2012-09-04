/* /////////////////////////////////////////////////////////////////////////
 * File:        test/component/test.component.log.1/test.component.log.1.cpp
 *
 * Purpose:     Implementation file for the test.component.log.1 project.
 *
 * Created:     25th November 2007
 * Updated:     23rd March 2010
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2007-2010, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

/* xTests Header Files */
#include <xtests/xtests.h>

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>
#include <pantheios/backends/bec.test.h>
#include <pantheios/frontends/stock.h>

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* ////////////////////////////////////////////////////////////////////// */

PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.component.log.1");

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

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

typedef std::basic_string<PAN_CHAR_T>               string_t;

/* /////////////////////////////////////////////////////////////////////////
 * Forward Declarations
 */

static void test_01();
static void test_02();
static void test_03();
static void test_04();
static void test_05();
static void test_06();
static void test_07();
static void test_08();
static void test_09();
static void test_10();
static void test_11();
static void test_12();
static void test_13();
static void test_14();
static void test_15();
static void test_16();
static void test_17();
static void test_18();
static void test_19();
static void test_20();
static void test_21();
static void test_22();
static void test_23();
static void test_24();
static void test_25();
static void test_26();
static void test_27();
static void test_28();
static void test_29();

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.component.log.1", verbosity))
    {
        XTESTS_RUN_CASE(test_01);
        XTESTS_RUN_CASE(test_02);
        XTESTS_RUN_CASE(test_03);
        XTESTS_RUN_CASE(test_04);
        XTESTS_RUN_CASE(test_05);
        XTESTS_RUN_CASE(test_06);
        XTESTS_RUN_CASE(test_07);
        XTESTS_RUN_CASE(test_08);
        XTESTS_RUN_CASE(test_09);
        XTESTS_RUN_CASE(test_10);
        XTESTS_RUN_CASE(test_11);
        XTESTS_RUN_CASE(test_12);
        XTESTS_RUN_CASE(test_13);
        XTESTS_RUN_CASE(test_14);
        XTESTS_RUN_CASE(test_15);
        XTESTS_RUN_CASE(test_16);
        XTESTS_RUN_CASE(test_17);
        XTESTS_RUN_CASE(test_18);
        XTESTS_RUN_CASE(test_19);
        XTESTS_RUN_CASE(test_20);
        XTESTS_RUN_CASE(test_21);
        XTESTS_RUN_CASE(test_22);
        XTESTS_RUN_CASE(test_23);
        XTESTS_RUN_CASE(test_24);
        XTESTS_RUN_CASE(test_25);
        XTESTS_RUN_CASE(test_26);
        XTESTS_RUN_CASE(test_27);
        XTESTS_RUN_CASE(test_28);
        XTESTS_RUN_CASE(test_29);

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }
    else
    {
        retCode = EXIT_FAILURE;
    }

    return retCode;
}

/* ////////////////////////////////////////////////////////////////////// */

static void test_01()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST_BOOLEAN_TRUE(results.empty());
    XTESTS_TEST_INTEGER_EQUAL(0u, results.size());
}

static void test_02()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log(pantheios::notice, PSTR(""));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST_BOOLEAN_FALSE(results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR(""), results[0].statement);
    XTESTS_TEST(pantheios::notice == results[0].severity);
}

static void test_03()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log(pantheios::notice, PSTR("abc"));


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST_BOOLEAN_FALSE(results.empty());
    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("abc"), results[0].statement);
    XTESTS_TEST(pantheios::notice == results[0].severity);
}

static void test_04()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log(pantheios::informational, PSTR("abc"), PSTR("def"));



    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("abcdef"), results[0].statement);
    XTESTS_TEST(pantheios::informational == results[0].severity);
}

static void test_05()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log(pantheios::informational, PSTR("abc"), PSTR("def"), PSTR("ghi"));



    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("abcdefghi"), results[0].statement);
    XTESTS_TEST(pantheios::informational == results[0].severity);
}

static void test_06()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log(pantheios::informational, PSTR("abc"), PSTR("def"), PSTR("ghi"), PSTR("jk"), PSTR("lm"), PSTR("no"), PSTR("pq"), PSTR("rs"), PSTR("tu"), PSTR("vw"), PSTR("xy"), PSTR("z"));



    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("abcdefghijklmnopqrstuvwxyz"), results[0].statement);
    XTESTS_TEST(pantheios::informational == results[0].severity);
}

static void test_07()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log(pantheios::informational, PSTR("a"), PSTR("b"), PSTR("c"), PSTR("d"), PSTR("e"), PSTR("f"), PSTR("g"), PSTR("h"), PSTR("i"), PSTR("j"), PSTR("k"), PSTR("l"), PSTR("m"), PSTR("n"), PSTR("o"), PSTR("p"), PSTR("q"), PSTR("r"), PSTR("s"), PSTR("t"), PSTR("u"), PSTR("v"), PSTR("w"), PSTR("x"), PSTR("y"), PSTR("z"));



    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("abcdefghijklmnopqrstuvwxyz"), results[0].statement);
    XTESTS_TEST(pantheios::informational == results[0].severity);
}

static void test_08()
{}

static void test_09()
{}

static void test_10()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    pantheios::log(pantheios::informational, PSTR("a"), PSTR("b"), PSTR("c"), PSTR("d"), PSTR("e"), PSTR("f"), PSTR("g"), PSTR("h"), PSTR("i"), PSTR("j"), PSTR("k"), PSTR("l"), PSTR("m"), PSTR("n"), PSTR("o"), PSTR("p"), PSTR("q"), PSTR("r"), PSTR("s"), PSTR("t"), PSTR("u"), PSTR("v"), PSTR("w"), PSTR("x"), PSTR("y"), PSTR("z"));
    pantheios::log(pantheios::informational, PSTR("A"), PSTR("B"), PSTR("C"), PSTR("D"), PSTR("E"), PSTR("F"), PSTR("G"), PSTR("H"), PSTR("I"), PSTR("J"), PSTR("K"), PSTR("L"), PSTR("M"), PSTR("N"), PSTR("O"), PSTR("P"), PSTR("Q"), PSTR("R"), PSTR("S"), PSTR("T"), PSTR("U"), PSTR("V"), PSTR("W"), PSTR("X"), PSTR("Y"), PSTR("Z"));



    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST_INTEGER_EQUAL(2u, results.size());
    XTESTS_TEST_STRING_EQUAL(PSTR("abcdefghijklmnopqrstuvwxyz"), results[0].statement);
    XTESTS_TEST_STRING_EQUAL(PSTR("ABCDEFGHIJKLMNOPQRSTUVWXYZ"), results[1].statement);
}

static void test_11()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data
    const size_t    numEntries  =   1000;

    { for(size_t i = 0; i < numEntries; ++i)
    {
        if(0 == (i % 2))
        {
            pantheios::log(pantheios::informational, PSTR("a"), PSTR("b"), PSTR("c"), PSTR("d"), PSTR("e"), PSTR("f"), PSTR("g"), PSTR("h"), PSTR("i"), PSTR("j"), PSTR("k"), PSTR("l"), PSTR("m"), PSTR("n"), PSTR("o"), PSTR("p"), PSTR("q"), PSTR("r"), PSTR("s"), PSTR("t"), PSTR("u"), PSTR("v"), PSTR("w"), PSTR("x"), PSTR("y"), PSTR("z"));
        }
        else
        {
            pantheios::log(pantheios::informational, PSTR("A"), PSTR("B"), PSTR("C"), PSTR("D"), PSTR("E"), PSTR("F"), PSTR("G"), PSTR("H"), PSTR("I"), PSTR("J"), PSTR("K"), PSTR("L"), PSTR("M"), PSTR("N"), PSTR("O"), PSTR("P"), PSTR("Q"), PSTR("R"), PSTR("S"), PSTR("T"), PSTR("U"), PSTR("V"), PSTR("W"), PSTR("X"), PSTR("Y"), PSTR("Z"));
        }
    }}



    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST_INTEGER_EQUAL(numEntries, results.size());
    { for(size_t i = 0; i < numEntries; ++i)
    {
        if(0 == (i % 2))
        {
            XTESTS_TEST_STRING_EQUAL(PSTR("abcdefghijklmnopqrstuvwxyz"), results[i].statement);
        }
        else
        {
            XTESTS_TEST_STRING_EQUAL(PSTR("ABCDEFGHIJKLMNOPQRSTUVWXYZ"), results[i].statement);
        }
    }}
}

static void test_12()
{}

static void test_13()
{}

static void test_14()
{}

static void test_15()
{}

static void test_16()
{}

static void test_17()
{}

static void test_18()
{}

static void test_19()
{}

static void test_20()
{}

static void test_21()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    string_t msgEl1(256, '~');
    string_t msgEl2(4096, '#');


    pantheios::log(pantheios::informational, msgEl1, msgEl2);



    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(msgEl1 + msgEl2, results[0].statement);
    XTESTS_TEST(pantheios::informational == results[0].severity);
}

static void test_22()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    string_t msgEl1(2560, '~');
    string_t msgEl2(40960, '#');

    pantheios::log(pantheios::informational, msgEl1, msgEl2);


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST_INTEGER_EQUAL(1u, results.size());
    XTESTS_TEST_STRING_EQUAL(msgEl1 + msgEl2, results[0].statement);
    XTESTS_TEST(pantheios::informational == results[0].severity);
}

static void test_23()
{}

static void test_24()
{}

static void test_25()
{}

static void test_26()
{}

static void test_27()
{}

static void test_28()
{}

static void test_29()
{}

/* ///////////////////////////// end of file //////////////////////////// */
