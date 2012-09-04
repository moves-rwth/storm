/* /////////////////////////////////////////////////////////////////////////
 * File:        test/unit/inserters/test.unit.inserter.m2w/test.unit.inserter.m2w.cpp
 *
 * Purpose:     Implementation file for the test.unit.inserter.m2w project.
 *
 * Created:     22nd November 2010
 * Updated:     21st December 2010
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2010, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

#include <pantheios/pantheios.h>

#ifndef PANTHEIOS_USE_WIDE_STRINGS
# error This project only valid in wide-string builds
#endif

#include <pantheios/inserters/m2w.hpp>

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

extern "C" PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = L"test.unit.inserter.m2w";

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.unit.inserter.m2w", verbosity))
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
    char const* strings[] = 
    {
            ""
        ,   "a"
        ,   "ab"
        ,   "abc"
        ,   "abcd"
        ,   "abcde"
        ,   "abcdef"
        ,   "abcdefghijklmnopqrstuvwxyz"
        ,   "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789"
    };

    inline size_t get_min_max(size_t minLen, size_t maxLen)
    {
        return pantheios::pan_slice_t::get_lazy_length(minLen, maxLen);
    }

static void test_1_01()
{
    { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
    {
        XTESTS_TEST_WIDE_STRING_EQUAL(stlsoft::m2w(strings[i]), pantheios::m2w(strings[i]));
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
