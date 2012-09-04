/* /////////////////////////////////////////////////////////////////////////
 * File:        test/unit/test.unit.util.strnlen/test.unit.util.strnlen.cpp
 *
 * Purpose:     Implementation file for the test.unit.util.strnlen project.
 *
 * Created:     17th April 2009
 * Updated:     19th March 2012
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2008-2012, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

#include <pantheios/pantheios.h>
#include <pantheios/internal/lean.h>
#include <pantheios/init_codes.h>

#include <xtests/xtests.h>

#include <stlsoft/shims/access/string.hpp>
#include <stlsoft/util/limit_traits.h>
#include <stlsoft/util/minmax.hpp>

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* /////////////////////////////////////////////////////////////////////////
 * Forward declarations
 */

namespace
{

static void test_length_passed_through();
static void test_length_via_strlen();
static void test_length_calculated_forwards();
static void test_length_calculated_backwards();
static void test_strnlen_with_negatives();
static void test_1_06();
static void test_1_07();
static void test_1_08();
static void test_1_09();
static void test_1_10();
static void test_1_11();
static void test_1_12();

} // anonymous namespace

/* ////////////////////////////////////////////////////////////////////// */

const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.unit.util.strnlen");

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.unit.util.strnlen", verbosity))
    {
        XTESTS_RUN_CASE(test_length_passed_through);
        XTESTS_RUN_CASE(test_length_via_strlen);
        XTESTS_RUN_CASE(test_length_calculated_forwards);
        XTESTS_RUN_CASE(test_length_calculated_backwards);
        XTESTS_RUN_CASE(test_strnlen_with_negatives);
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
    PAN_CHAR_T const* strings[] = 
    {
            PANTHEIOS_LITERAL_STRING("")
        ,   PANTHEIOS_LITERAL_STRING("a")
        ,   PANTHEIOS_LITERAL_STRING("ab")
        ,   PANTHEIOS_LITERAL_STRING("abc")
        ,   PANTHEIOS_LITERAL_STRING("abcd")
        ,   PANTHEIOS_LITERAL_STRING("abcde")
        ,   PANTHEIOS_LITERAL_STRING("abcdef")
        ,   PANTHEIOS_LITERAL_STRING("abcdefghijklmnopqrstuvwxyz")
        ,   PANTHEIOS_LITERAL_STRING("abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789")
    };

    inline size_t get_min_max(size_t minLen, size_t maxLen)
    {
        return pantheios::pan_slice_t::get_lazy_length(minLen, maxLen);
    }

static void test_length_passed_through()
{
    /* Verify that when the top two bits are not set, the length returned
     * is the length given
     */

    size_t iterations = stlsoft::minimum(size_t(65536u * 16u), size_t(stlsoft::limit_traits<size_t>::maximum() / 4000));

    { for(size_t i = 0; i < iterations; ++i)
    {
        XTESTS_TEST_INTEGER_EQUAL(i, pantheios::util::strnlen(PANTHEIOS_LITERAL_STRING(""), i));
    }}
}

static void test_length_via_strlen()
{
    /* Verify that when the size given is size_t(-1) that the string length
     * is calculated
     */
    { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
    {
        PAN_CHAR_T const* const string = strings[i];

        XTESTS_TEST_INTEGER_EQUAL(stlsoft::c_str_len(string), pantheios::util::strnlen(string, ~size_t(0)));
    }}
}

static void test_length_calculated_forwards()
{
    /* Verify that when the size has the top-bit set, then the string length
     * is calculated between the given limits
     */

    const PAN_CHAR_T string[] = PANTHEIOS_LITERAL_STRING("abcdefghijklm\0nop\0qrs\0tuvwxyz"); // lengths: 13, 17, 21

    XTESTS_TEST_INTEGER_EQUAL( 0u, pantheios::util::strnlen(string, get_min_max( 0,  0)));
    XTESTS_TEST_INTEGER_EQUAL( 1u, pantheios::util::strnlen(string, get_min_max( 0,  1)));
    XTESTS_TEST_INTEGER_EQUAL( 2u, pantheios::util::strnlen(string, get_min_max( 0,  2)));
    XTESTS_TEST_INTEGER_EQUAL( 3u, pantheios::util::strnlen(string, get_min_max( 0,  3)));
    XTESTS_TEST_INTEGER_EQUAL( 4u, pantheios::util::strnlen(string, get_min_max( 0,  4)));
    XTESTS_TEST_INTEGER_EQUAL( 5u, pantheios::util::strnlen(string, get_min_max( 0,  5)));
    XTESTS_TEST_INTEGER_EQUAL( 6u, pantheios::util::strnlen(string, get_min_max( 0,  6)));
    XTESTS_TEST_INTEGER_EQUAL( 7u, pantheios::util::strnlen(string, get_min_max( 0,  7)));
    XTESTS_TEST_INTEGER_EQUAL( 8u, pantheios::util::strnlen(string, get_min_max( 0,  8)));
    XTESTS_TEST_INTEGER_EQUAL( 9u, pantheios::util::strnlen(string, get_min_max( 0,  9)));
    XTESTS_TEST_INTEGER_EQUAL(10u, pantheios::util::strnlen(string, get_min_max( 0, 10)));
    XTESTS_TEST_INTEGER_EQUAL(11u, pantheios::util::strnlen(string, get_min_max( 0, 11)));
    XTESTS_TEST_INTEGER_EQUAL(12u, pantheios::util::strnlen(string, get_min_max( 0, 12)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max( 0, 13)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max( 0, 14)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max( 0, 15)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max( 0, 16)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max( 0, 17)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max( 0, 18)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max( 0, 19)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max( 0, 20)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max( 0, 21)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max( 0, 22)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max( 0, 23)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max( 0, 24)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max( 0, 25)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max( 0, 26)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max( 0, 27)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max( 0, 28)));

    XTESTS_TEST_INTEGER_EQUAL(10u, pantheios::util::strnlen(string, get_min_max(10, 10)));
    XTESTS_TEST_INTEGER_EQUAL(11u, pantheios::util::strnlen(string, get_min_max(10, 11)));
    XTESTS_TEST_INTEGER_EQUAL(12u, pantheios::util::strnlen(string, get_min_max(10, 12)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max(10, 13)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max(10, 14)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max(10, 15)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max(10, 16)));

    XTESTS_TEST_INTEGER_EQUAL(14u, pantheios::util::strnlen(string, get_min_max(14, 14)));
    XTESTS_TEST_INTEGER_EQUAL(15u, pantheios::util::strnlen(string, get_min_max(14, 15)));
    XTESTS_TEST_INTEGER_EQUAL(16u, pantheios::util::strnlen(string, get_min_max(14, 16)));
    XTESTS_TEST_INTEGER_EQUAL(17u, pantheios::util::strnlen(string, get_min_max(14, 17)));
    XTESTS_TEST_INTEGER_EQUAL(17u, pantheios::util::strnlen(string, get_min_max(14, 18)));
    XTESTS_TEST_INTEGER_EQUAL(17u, pantheios::util::strnlen(string, get_min_max(14, 19)));
    XTESTS_TEST_INTEGER_EQUAL(17u, pantheios::util::strnlen(string, get_min_max(14, 20)));

    XTESTS_TEST_INTEGER_EQUAL(18u, pantheios::util::strnlen(string, get_min_max(18, 18)));
    XTESTS_TEST_INTEGER_EQUAL(19u, pantheios::util::strnlen(string, get_min_max(18, 19)));
    XTESTS_TEST_INTEGER_EQUAL(20u, pantheios::util::strnlen(string, get_min_max(18, 20)));
    XTESTS_TEST_INTEGER_EQUAL(21u, pantheios::util::strnlen(string, get_min_max(18, 21)));
    XTESTS_TEST_INTEGER_EQUAL(21u, pantheios::util::strnlen(string, get_min_max(18, 22)));
    XTESTS_TEST_INTEGER_EQUAL(21u, pantheios::util::strnlen(string, get_min_max(18, 23)));
    XTESTS_TEST_INTEGER_EQUAL(21u, pantheios::util::strnlen(string, get_min_max(18, 24)));
}

static void test_length_calculated_backwards()
{
    /* Verify that when the size has the top-bit set, then the string length
     * is calculated between the given limits
     */

    const PAN_CHAR_T string[] = PANTHEIOS_LITERAL_STRING("abcdefghijklm\0nop\0qrs\0tuvwxyz"); // lengths: 13, 17, 21

    XTESTS_TEST_INTEGER_EQUAL( 0u, pantheios::util::strnlen(string, get_min_max( 0,  0)));
    XTESTS_TEST_INTEGER_EQUAL( 1u, pantheios::util::strnlen(string, get_min_max( 1,  0)));
    XTESTS_TEST_INTEGER_EQUAL( 2u, pantheios::util::strnlen(string, get_min_max( 2,  0)));
    XTESTS_TEST_INTEGER_EQUAL( 3u, pantheios::util::strnlen(string, get_min_max( 3,  0)));
    XTESTS_TEST_INTEGER_EQUAL( 4u, pantheios::util::strnlen(string, get_min_max( 4,  0)));
    XTESTS_TEST_INTEGER_EQUAL( 5u, pantheios::util::strnlen(string, get_min_max( 5,  0)));
    XTESTS_TEST_INTEGER_EQUAL( 6u, pantheios::util::strnlen(string, get_min_max( 6,  0)));
    XTESTS_TEST_INTEGER_EQUAL( 7u, pantheios::util::strnlen(string, get_min_max( 7,  0)));
    XTESTS_TEST_INTEGER_EQUAL( 8u, pantheios::util::strnlen(string, get_min_max( 8,  0)));
    XTESTS_TEST_INTEGER_EQUAL( 9u, pantheios::util::strnlen(string, get_min_max( 9,  0)));
    XTESTS_TEST_INTEGER_EQUAL(10u, pantheios::util::strnlen(string, get_min_max(10,  0)));
    XTESTS_TEST_INTEGER_EQUAL(11u, pantheios::util::strnlen(string, get_min_max(11,  0)));
    XTESTS_TEST_INTEGER_EQUAL(12u, pantheios::util::strnlen(string, get_min_max(12,  0)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max(13,  0)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max(14,  0)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max(15,  0)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max(16,  0)));
    XTESTS_TEST_INTEGER_EQUAL(17u, pantheios::util::strnlen(string, get_min_max(17,  0)));
    XTESTS_TEST_INTEGER_EQUAL(17u, pantheios::util::strnlen(string, get_min_max(18,  0)));
    XTESTS_TEST_INTEGER_EQUAL(17u, pantheios::util::strnlen(string, get_min_max(19,  0)));
    XTESTS_TEST_INTEGER_EQUAL(17u, pantheios::util::strnlen(string, get_min_max(20,  0)));
    XTESTS_TEST_INTEGER_EQUAL(21u, pantheios::util::strnlen(string, get_min_max(21,  0)));
    XTESTS_TEST_INTEGER_EQUAL(21u, pantheios::util::strnlen(string, get_min_max(22,  0)));
    XTESTS_TEST_INTEGER_EQUAL(21u, pantheios::util::strnlen(string, get_min_max(23,  0)));
    XTESTS_TEST_INTEGER_EQUAL(21u, pantheios::util::strnlen(string, get_min_max(24,  0)));
    XTESTS_TEST_INTEGER_EQUAL(21u, pantheios::util::strnlen(string, get_min_max(25,  0)));
    XTESTS_TEST_INTEGER_EQUAL(21u, pantheios::util::strnlen(string, get_min_max(26,  0)));
    XTESTS_TEST_INTEGER_EQUAL(21u, pantheios::util::strnlen(string, get_min_max(27,  0)));
    XTESTS_TEST_INTEGER_EQUAL(21u, pantheios::util::strnlen(string, get_min_max(28,  0)));

    XTESTS_TEST_INTEGER_EQUAL(10u, pantheios::util::strnlen(string, get_min_max(10, 10)));
    XTESTS_TEST_INTEGER_EQUAL(11u, pantheios::util::strnlen(string, get_min_max(11, 10)));
    XTESTS_TEST_INTEGER_EQUAL(12u, pantheios::util::strnlen(string, get_min_max(12, 10)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max(13, 10)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max(14, 10)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max(15, 10)));
    XTESTS_TEST_INTEGER_EQUAL(13u, pantheios::util::strnlen(string, get_min_max(16, 10)));

    XTESTS_TEST_INTEGER_EQUAL(14u, pantheios::util::strnlen(string, get_min_max(14, 14)));
    XTESTS_TEST_INTEGER_EQUAL(15u, pantheios::util::strnlen(string, get_min_max(15, 14)));
    XTESTS_TEST_INTEGER_EQUAL(16u, pantheios::util::strnlen(string, get_min_max(16, 14)));
    XTESTS_TEST_INTEGER_EQUAL(17u, pantheios::util::strnlen(string, get_min_max(17, 14)));
    XTESTS_TEST_INTEGER_EQUAL(17u, pantheios::util::strnlen(string, get_min_max(18, 14)));
    XTESTS_TEST_INTEGER_EQUAL(17u, pantheios::util::strnlen(string, get_min_max(19, 14)));
    XTESTS_TEST_INTEGER_EQUAL(17u, pantheios::util::strnlen(string, get_min_max(20, 14)));

    XTESTS_TEST_INTEGER_EQUAL(18u, pantheios::util::strnlen(string, get_min_max(18, 18)));
    XTESTS_TEST_INTEGER_EQUAL(19u, pantheios::util::strnlen(string, get_min_max(19, 18)));
    XTESTS_TEST_INTEGER_EQUAL(20u, pantheios::util::strnlen(string, get_min_max(20, 18)));
    XTESTS_TEST_INTEGER_EQUAL(21u, pantheios::util::strnlen(string, get_min_max(21, 18)));
    XTESTS_TEST_INTEGER_EQUAL(21u, pantheios::util::strnlen(string, get_min_max(22, 18)));
    XTESTS_TEST_INTEGER_EQUAL(21u, pantheios::util::strnlen(string, get_min_max(23, 18)));
    XTESTS_TEST_INTEGER_EQUAL(21u, pantheios::util::strnlen(string, get_min_max(24, 18)));
}

static void test_strnlen_with_negatives()
{
    PAN_CHAR_T const abcdefghi[] = PANTHEIOS_LITERAL_STRING("abcdefghi");

    XTESTS_TEST_INTEGER_EQUAL(9u, pantheios::util::strnlen(abcdefghi, static_cast<size_t>(-1)));

    {
    signed char minus1 = -1;

    XTESTS_TEST_INTEGER_EQUAL(9u, pantheios::util::strnlen(abcdefghi, static_cast<size_t>(minus1)));
    }

    {
    short   minus1 = -1;

    XTESTS_TEST_INTEGER_EQUAL(9u, pantheios::util::strnlen(abcdefghi, static_cast<size_t>(minus1)));
    }

    {
    int minus1 = -1;

    XTESTS_TEST_INTEGER_EQUAL(9u, pantheios::util::strnlen(abcdefghi, static_cast<size_t>(minus1)));
    }

    {
    long    minus1 = -1;

    XTESTS_TEST_INTEGER_EQUAL(9u, pantheios::util::strnlen(abcdefghi, static_cast<size_t>(minus1)));
    }
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
