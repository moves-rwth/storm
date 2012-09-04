/* /////////////////////////////////////////////////////////////////////////
 * File:        test/unit/inserters/test.unit.inserter.w2m/test.unit.inserter.w2m.cpp
 *
 * Purpose:     Implementation file for the test.unit.inserter.w2m project.
 *
 * Created:     21st December 2010
 * Updated:     6th August 2012
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2010-2012, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

#include <pantheios/pantheios.h>

#ifdef PANTHEIOS_USE_WIDE_STRINGS
# error This project only valid in multibyte-string builds
#endif

#include <pantheios/inserters/w2m.hpp>

#include <xtests/xtests.h>

#include <stlsoft/conversion/char_conversions.hpp>
#include <stlsoft/shims/access/string.hpp>
#include <stlsoft/util/limit_traits.h>
#include <stlsoft/util/minmax.hpp>

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

} // anonymous namespace

/* ////////////////////////////////////////////////////////////////////// */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.unit.inserter.w2m");

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.unit.inserter.w2m", verbosity))
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

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* ////////////////////////////////////////////////////////////////////// */

namespace
{
    wchar_t const* strings[] = 
    {
            L""
        ,   L"a"
        ,   L"ab"
        ,   L"abc"
        ,   L"abcd"
        ,   L"abcde"
        ,   L"abcdef"
        ,   L"abcdefghijklmnopqrstuvwxyz"
        ,   L"abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789"
    };

    inline size_t get_min_max(size_t minLen, size_t maxLen)
    {
        return pantheios::pan_slice_t::get_lazy_length(minLen, maxLen);
    }

static void test_1_01()
{
    { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
    {
        XTESTS_TEST_MULTIBYTE_STRING_EQUAL(stlsoft::w2m(strings[i]), pantheios::w2m(strings[i]));
    }}
}

static void test_1_02()
{
}

static void test_1_03()
{
}

static void test_1_04()
{
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

} // anonymous namespace

/* ////////////////////////////////////////////////////////////////////// */
