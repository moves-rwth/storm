/* /////////////////////////////////////////////////////////////////////////
 * File:        test/component/test.component.trace.1/test.component.trace.1.cpp
 *
 * Purpose:     Implementation file for the test.component.trace.1 project.
 *
 * Created:     25th November 2007
 * Updated:     6th August 2012
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2007-2012, Synesis Software Pty Ltd.
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
#include <pantheios/trace.h>
#include <pantheios/backends/bec.test.h>
#include <pantheios/frontends/stock.h>

/* STLSoft Header Files */
#include <stlsoft/conversion/integer_to_string.hpp>

/* Standard C++ Header Files */
#include <string>

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* /////////////////////////////////////////////////////////////////////////
 * Macros
 */

#define PANTHEIOS_TEST_STRING_OBJECTS_EQUAL(s1, s2)     XTESTS_TEST_MULTIBYTE_STRING_EQUAL((s1), (s2))

/* ////////////////////////////////////////////////////////////////////// */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.component.trace.1");

namespace
{
    static const char   THIS_FILE[] =   __FILE__;

    inline std::string fileline_stmt_(char const* stmt, int line)
    {
        char        sz[21];
        char const* num = stlsoft::integer_to_string(&sz[0], STLSOFT_NUM_ELEMENTS(sz), line);
        std::string s(THIS_FILE);

        s += '(';
        s += num;
        s += "): ";
        s += stmt;

        return s;
    }

} // anonymous namespace

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

    if(XTESTS_START_RUNNER("test.component.trace.1", verbosity))
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

    XTESTS_TEST(results.empty());
    XTESTS_TEST(0 == results.size());
}

static void test_02()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    PANTHEIOS_TRACE_NOTICE(""); int LINE = __LINE__;


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST(pantheios::notice == results[0].severity);
    PANTHEIOS_TEST_STRING_OBJECTS_EQUAL(fileline_stmt_("", LINE), results[0].statement);
}

static void test_03()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    PANTHEIOS_TRACE_NOTICE("abc");  int LINE = __LINE__;


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(!results.empty());
    XTESTS_TEST(1 == results.size());
    XTESTS_TEST(pantheios::notice == results[0].severity);
    PANTHEIOS_TEST_STRING_OBJECTS_EQUAL(fileline_stmt_("abc", LINE), results[0].statement);
}

static void test_04()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    PANTHEIOS_TRACE_INFORMATIONAL("abc", "def"); int LINE = __LINE__;



    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(1 == results.size());
    XTESTS_TEST(pantheios::informational == results[0].severity);
    PANTHEIOS_TEST_STRING_OBJECTS_EQUAL(fileline_stmt_("abcdef", LINE), results[0].statement);
}

static void test_05()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    PANTHEIOS_TRACE_INFORMATIONAL("abc", "def", "ghi"); int LINE = __LINE__;



    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(1 == results.size());
    XTESTS_TEST(pantheios::informational == results[0].severity);
    PANTHEIOS_TEST_STRING_OBJECTS_EQUAL(fileline_stmt_("abcdefghi", LINE), results[0].statement);
}

static void test_06()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    PANTHEIOS_TRACE_INFORMATIONAL("abc", "def", "ghi", "jk", "lm", "no", "pq", "rs", "tu", "vw", "xy", "z"); int LINE = __LINE__;



    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(1 == results.size());
    XTESTS_TEST(pantheios::informational == results[0].severity);
    PANTHEIOS_TEST_STRING_OBJECTS_EQUAL(fileline_stmt_("abcdefghijklmnopqrstuvwxyz", LINE), results[0].statement);
}

static void test_07()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    PANTHEIOS_TRACE_INFORMATIONAL("a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"); int LINE = __LINE__;



    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(1 == results.size());
    XTESTS_TEST(pantheios::informational == results[0].severity);
    PANTHEIOS_TEST_STRING_OBJECTS_EQUAL(fileline_stmt_("abcdefghijklmnopqrstuvwxyz", LINE), results[0].statement);
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

    PANTHEIOS_TRACE_INFORMATIONAL("a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"); int LINE1 = __LINE__;
    PANTHEIOS_TRACE_INFORMATIONAL("A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"); int LINE2 = __LINE__;



    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(2 == results.size());
    XTESTS_TEST(pantheios::informational == results[0].severity);
    PANTHEIOS_TEST_STRING_OBJECTS_EQUAL(fileline_stmt_("abcdefghijklmnopqrstuvwxyz", LINE1), results[0].statement);
    XTESTS_TEST(pantheios::informational == results[1].severity);
    PANTHEIOS_TEST_STRING_OBJECTS_EQUAL(fileline_stmt_("ABCDEFGHIJKLMNOPQRSTUVWXYZ", LINE2), results[1].statement);
}

static void test_11()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data
    const size_t    numEntries  =   1000;
    int             LINE1       =   -1;
    int             LINE2       =   -1;

    { for(size_t i = 0; i < numEntries; ++i)
    {
        if(0 == (i % 2))
        {
            PANTHEIOS_TRACE_INFORMATIONAL("a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"); LINE1 = __LINE__;
        }
        else
        {
            PANTHEIOS_TRACE_INFORMATIONAL("A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"); LINE2 = __LINE__;
        }
    }}



    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(numEntries == results.size());
    { for(size_t i = 0; i < numEntries; ++i)
    {
        XTESTS_TEST(pantheios::informational == results[i].severity);
        if(0 == (i % 2))
        {
            PANTHEIOS_TEST_STRING_OBJECTS_EQUAL(fileline_stmt_("abcdefghijklmnopqrstuvwxyz", LINE1), results[i].statement);
        }
        else
        {
            PANTHEIOS_TEST_STRING_OBJECTS_EQUAL(fileline_stmt_("ABCDEFGHIJKLMNOPQRSTUVWXYZ", LINE2), results[i].statement);
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

    std::string msgEl1(256, '~');
    std::string msgEl2(4096, '#');


    PANTHEIOS_TRACE_INFORMATIONAL(msgEl1, msgEl2); int LINE = __LINE__;



    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(1 == results.size());
    XTESTS_TEST(pantheios::informational == results[0].severity);
    PANTHEIOS_TEST_STRING_OBJECTS_EQUAL(fileline_stmt_((msgEl1 + msgEl2).c_str(), LINE), results[0].statement);
}

static void test_22()
{
    // 1. Setup

    pantheios::be::test::reset();


    // 2. Create test data

    std::string msgEl1(2560, '~');
    std::string msgEl2(40960, '#');

    PANTHEIOS_TRACE_INFORMATIONAL(msgEl1, msgEl2); int LINE = __LINE__;


    // 3. Verification

    pantheios::be::test::Results  results = pantheios::be::test::results();

    XTESTS_TEST(1 == results.size());
    XTESTS_TEST(pantheios::informational == results[0].severity);
    PANTHEIOS_TEST_STRING_OBJECTS_EQUAL(fileline_stmt_((msgEl1 + msgEl2).c_str(), LINE), results[0].statement);
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
