/* /////////////////////////////////////////////////////////////////////////
 * File:        test/unit/test.unit.util.getcurrenttime/test.unit.util.getcurrenttime.cpp
 *
 * Purpose:     Implementation file for the test.unit.util.getcurrenttime project.
 *
 * Created:     10th November 2007
 * Updated:     4th January 2011
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2007-2011, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#ifdef __BORLANDC__
 /* Borland 6.1 is so unbelievably stupid that even when
  * given '#pragma warn -8026' multiple places within the code 
  * and '-winl' on the command it _still_ insists on reporting
  * that "Functions with exception specifications are not expanded 
  * inline in function test_time(const char *,int,unsigned int,int,unsigned int,const char *)",
  * so I am forced to such extreme measures! What a peice of crap.
  *
  * Bring back Turbo C++ 3.1!
  */

# define throw()
#  include <shwild/shwild.h>
# undef throw
#endif /* compiler */


#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

/* Pantheios Header Files */
#include <pantheios/util/time/currenttime.h>

/* xTests Header Files */
#include <xtests/xtests.h>

/* shwild Header Files */
#include <shwild/shwild.h>

#if SHWILD_VER < 0x00090900
# error Requires shwild 0.9.9 or later; earlier versions have a defect that cause these tests to report false errors
#endif /* SHWILD_VER */

/* STLSoft Header Files */
#include <stlsoft/conversion/integer_to_string.hpp>
#include <stlsoft/conversion/char_conversions.hpp>
#include <pantheios/util/memory/auto_buffer_selector.hpp>
#include <stlsoft/shims/access/string/std/c_string.h>
#include <stlsoft/string/container_slice_functions.hpp>
#include <platformstl/platformstl.hpp>


/* Standard C++ Header Files */
#include <exception>
#include <string>

/* Standard C Header Files */
#include <stdio.h>
#include <stdlib.h>

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

typedef std::basic_string<PAN_CHAR_T>   string_t;

/* /////////////////////////////////////////////////////////////////////////
 * Macros and definitions
 */

#define PANTHEIOS_TEST_TIME(buffSize, flags, result, pattern)   test_time(__FILE__, __LINE__, buffSize, flags, result, pattern)

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.unit.util.getcurrenttime");

/* /////////////////////////////////////////////////////////////////////////
 * Forward declarations
 */

static int      test_time(char const* file, int line, size_t cchBuff, int flags, size_t expectedResult, PAN_CHAR_T const* pattern);
static string_t translate_pattern(PAN_CHAR_T const* pattern);

static void     test_unix_0();
static void     test_unix_F_USE_SYSTEM_TIME();
static void     test_unix_F_HIDE_DATE();
static void     test_unix_F_HIDE_TIME();
#if defined(PLATFORMSTL_OS_IS_WINDOWS)
static void     test_win_0();
static void     test_win_F_USE_SYSTEM_TIME();
static void     test_win_F_HIDE_DATE();
static void     test_win_F_HIDE_TIME();
#endif /* PLATFORMSTL_OS_IS_WINDOWS */

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.unit.util.getcurrenttime", verbosity))
    {
        XTESTS_RUN_CASE_WITH_DESC(test_unix_0, "testing UNIX format; flags=0");
        XTESTS_RUN_CASE_WITH_DESC(test_unix_F_USE_SYSTEM_TIME, "testing UNIX format; flags=F_USE_SYSTEM_TIME");
        XTESTS_RUN_CASE_WITH_DESC(test_unix_F_HIDE_DATE, "testing UNIX format; flags=F_HIDE_DATE");
        XTESTS_RUN_CASE_WITH_DESC(test_unix_F_HIDE_TIME, "testing UNIX format; flags=F_HIDE_TIME");
#if defined(PLATFORMSTL_OS_IS_WINDOWS)
        XTESTS_RUN_CASE_WITH_DESC(test_win_0, "testing Windows format; flags=0");
        XTESTS_RUN_CASE_WITH_DESC(test_win_F_USE_SYSTEM_TIME, "testing Windows format; flags=F_USE_SYSTEM_TIME");
        XTESTS_RUN_CASE_WITH_DESC(test_win_F_HIDE_DATE, "testing Windows format; flags=F_HIDE_DATE");
        XTESTS_RUN_CASE_WITH_DESC(test_win_F_HIDE_TIME, "testing Windows format; flags=0");
#endif /* PLATFORMSTL_OS_IS_WINDOWS */

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

static void test_unix_0()
{
    const int   timeFlags   =   PANTHEIOS_GETCURRENTTIME_F_USE_UNIX_FORMAT
                            |   0
                            |   0
                            |   0;

    PANTHEIOS_TEST_TIME( 0,   timeFlags, 15, PANTHEIOS_LITERAL_STRING(""));
    PANTHEIOS_TEST_TIME( 1,   timeFlags,  1, PANTHEIOS_LITERAL_STRING("[ADFJMNOS]"));
    PANTHEIOS_TEST_TIME( 2,   timeFlags,  2, PANTHEIOS_LITERAL_STRING("[ADFJMNOS][abceglnoprtuvy]"));
    PANTHEIOS_TEST_TIME( 3,   timeFlags,  3, PANTHEIOS_LITERAL_STRING("?<Mon>"));
    PANTHEIOS_TEST_TIME( 4,   timeFlags,  4, PANTHEIOS_LITERAL_STRING("?<Mon> "));
    PANTHEIOS_TEST_TIME( 5,   timeFlags,  5, PANTHEIOS_LITERAL_STRING("?<Mon> <D>"));
    PANTHEIOS_TEST_TIME( 6,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME( 7,   timeFlags,  7, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> "));
    PANTHEIOS_TEST_TIME( 8,   timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <D>"));
    PANTHEIOS_TEST_TIME( 9,   timeFlags,  9, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>"));
    PANTHEIOS_TEST_TIME(10,   timeFlags, 10, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:"));
    PANTHEIOS_TEST_TIME(11,   timeFlags, 11, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<D>"));
    PANTHEIOS_TEST_TIME(12,   timeFlags, 12, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>"));
    PANTHEIOS_TEST_TIME(13,   timeFlags, 13, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:"));
    PANTHEIOS_TEST_TIME(14,   timeFlags, 14, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<D>"));
    PANTHEIOS_TEST_TIME(15,   timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(16,   timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(16,   timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(17,   timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(18,   timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(19,   timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(20,   timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(100,  timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(1000, timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
}

static void test_unix_F_USE_SYSTEM_TIME()
{
    const int   timeFlags   =   PANTHEIOS_GETCURRENTTIME_F_USE_UNIX_FORMAT
                            |   0
                            |   PANTHEIOS_GETCURRENTTIME_F_USE_SYSTEM_TIME
                            |   0;

    PANTHEIOS_TEST_TIME( 0,   timeFlags, 15, PANTHEIOS_LITERAL_STRING(""));
    PANTHEIOS_TEST_TIME( 1,   timeFlags,  1, PANTHEIOS_LITERAL_STRING("[ADFJMNOS]"));
    PANTHEIOS_TEST_TIME( 2,   timeFlags,  2, PANTHEIOS_LITERAL_STRING("[ADFJMNOS][abceglnoprtuvy]"));
    PANTHEIOS_TEST_TIME( 3,   timeFlags,  3, PANTHEIOS_LITERAL_STRING("?<Mon>"));
    PANTHEIOS_TEST_TIME( 4,   timeFlags,  4, PANTHEIOS_LITERAL_STRING("?<Mon> "));
    PANTHEIOS_TEST_TIME( 5,   timeFlags,  5, PANTHEIOS_LITERAL_STRING("?<Mon> <D>"));
    PANTHEIOS_TEST_TIME( 6,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME( 7,   timeFlags,  7, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> "));
    PANTHEIOS_TEST_TIME( 8,   timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <D>"));
    PANTHEIOS_TEST_TIME( 9,   timeFlags,  9, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>"));
    PANTHEIOS_TEST_TIME(10,   timeFlags, 10, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:"));
    PANTHEIOS_TEST_TIME(11,   timeFlags, 11, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<D>"));
    PANTHEIOS_TEST_TIME(12,   timeFlags, 12, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>"));
    PANTHEIOS_TEST_TIME(13,   timeFlags, 13, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:"));
    PANTHEIOS_TEST_TIME(14,   timeFlags, 14, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<D>"));
    PANTHEIOS_TEST_TIME(15,   timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(16,   timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(16,   timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(17,   timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(18,   timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(19,   timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(20,   timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(100,  timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(1000, timeFlags, 15, PANTHEIOS_LITERAL_STRING("?<Mon> <DD> <DD>:<DD>:<DD>"));
}

static void test_unix_F_HIDE_DATE()
{
    const int   timeFlags   =   PANTHEIOS_GETCURRENTTIME_F_USE_UNIX_FORMAT
                            |   0
                            |   PANTHEIOS_GETCURRENTTIME_F_HIDE_DATE
                            |   0;

    PANTHEIOS_TEST_TIME( 0,   timeFlags,  8, PANTHEIOS_LITERAL_STRING(""));
    PANTHEIOS_TEST_TIME( 1,   timeFlags,  1, PANTHEIOS_LITERAL_STRING("?<D>"));
    PANTHEIOS_TEST_TIME( 2,   timeFlags,  2, PANTHEIOS_LITERAL_STRING("?<DD>"));
    PANTHEIOS_TEST_TIME( 3,   timeFlags,  3, PANTHEIOS_LITERAL_STRING("?<DD>:"));
    PANTHEIOS_TEST_TIME( 4,   timeFlags,  4, PANTHEIOS_LITERAL_STRING("?<DD>:<D>"));
    PANTHEIOS_TEST_TIME( 5,   timeFlags,  5, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>"));
    PANTHEIOS_TEST_TIME( 6,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:"));
    PANTHEIOS_TEST_TIME( 7,   timeFlags,  7, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:<D>"));
    PANTHEIOS_TEST_TIME( 8,   timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME( 9,   timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(10,   timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(11,   timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(12,   timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(13,   timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(14,   timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(15,   timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(16,   timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(16,   timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(17,   timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(18,   timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(19,   timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(20,   timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(100,  timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:<DD>"));
    PANTHEIOS_TEST_TIME(1000, timeFlags,  8, PANTHEIOS_LITERAL_STRING("?<DD>:<DD>:<DD>"));
}

static void test_unix_F_HIDE_TIME()
{
    const int   timeFlags   =   PANTHEIOS_GETCURRENTTIME_F_USE_UNIX_FORMAT
                            |   0
                            |   PANTHEIOS_GETCURRENTTIME_F_HIDE_TIME
                            |   0;

    PANTHEIOS_TEST_TIME( 0,   timeFlags,  6, PANTHEIOS_LITERAL_STRING(""));
    PANTHEIOS_TEST_TIME( 1,   timeFlags,  1, PANTHEIOS_LITERAL_STRING("[ADFJMNOS]"));
    PANTHEIOS_TEST_TIME( 2,   timeFlags,  2, PANTHEIOS_LITERAL_STRING("[ADFJMNOS][abceglnoprtuvy]"));
    PANTHEIOS_TEST_TIME( 3,   timeFlags,  3, PANTHEIOS_LITERAL_STRING("?<Mon>"));
    PANTHEIOS_TEST_TIME( 4,   timeFlags,  4, PANTHEIOS_LITERAL_STRING("?<Mon> "));
    PANTHEIOS_TEST_TIME( 5,   timeFlags,  5, PANTHEIOS_LITERAL_STRING("?<Mon> <D>"));
    PANTHEIOS_TEST_TIME( 6,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME( 7,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME( 8,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME( 9,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME(10,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME(11,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME(12,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME(13,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME(14,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME(15,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME(16,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME(16,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME(17,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME(18,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME(19,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME(20,   timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME(100,  timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
    PANTHEIOS_TEST_TIME(1000, timeFlags,  6, PANTHEIOS_LITERAL_STRING("?<Mon> <DD>"));
}

/* ////////////////////////////////////////////////////////////////////// */

#if defined(PLATFORMSTL_OS_IS_WINDOWS)
static void test_win_0()
{
    const int   timeFlags   =   0
                            |   0
                            |   0
                            |   0;

    pan_beutil_time_t   tm(0, NULL);
    size_t              expected = pantheios_util_getCurrentTime(&tm, timeFlags);

        PANTHEIOS_TEST_TIME(            0, timeFlags, expected, PANTHEIOS_LITERAL_STRING(""));
    { for(size_t i = 1; i < expected + 100; ++i)
    {
        PANTHEIOS_TEST_TIME( expected + i, timeFlags, expected, PANTHEIOS_LITERAL_STRING("*"));
    }}
}

static void test_win_F_USE_SYSTEM_TIME()
{
    const int   timeFlags   =   0
                            |   0
                            |   PANTHEIOS_GETCURRENTTIME_F_USE_SYSTEM_TIME
                            |   0;

    pan_beutil_time_t   tm(0, NULL);
    size_t              expected = pantheios_util_getCurrentTime(&tm, timeFlags);

        PANTHEIOS_TEST_TIME(            0, timeFlags, expected, PANTHEIOS_LITERAL_STRING(""));
    { for(size_t i = 1; i < expected + 100; ++i)
    {
        PANTHEIOS_TEST_TIME( expected + i, timeFlags, expected, PANTHEIOS_LITERAL_STRING("*"));
    }}
}

static void test_win_F_HIDE_DATE()
{
    const int   timeFlags   =   0
                            |   0
                            |   PANTHEIOS_GETCURRENTTIME_F_HIDE_DATE
                            |   0;

    pan_beutil_time_t   tm(0, NULL);
    size_t              expected = pantheios_util_getCurrentTime(&tm, timeFlags);

        PANTHEIOS_TEST_TIME(            0, timeFlags, expected, PANTHEIOS_LITERAL_STRING(""));
    { for(size_t i = 1; i < expected + 100; ++i)
    {
        PANTHEIOS_TEST_TIME( expected + i, timeFlags, expected, PANTHEIOS_LITERAL_STRING("*"));
    }}
}

static void test_win_F_HIDE_TIME()
{
    const int   timeFlags   =   0
                            |   0
                            |   PANTHEIOS_GETCURRENTTIME_F_HIDE_TIME
                            |   0;

    pan_beutil_time_t   tm(0, NULL);
    size_t              expected = pantheios_util_getCurrentTime(&tm, timeFlags);

        PANTHEIOS_TEST_TIME(            0, timeFlags, expected, PANTHEIOS_LITERAL_STRING(""));
    { for(size_t i = 1; i < expected + 100; ++i)
    {
        PANTHEIOS_TEST_TIME( expected + i, timeFlags, expected, PANTHEIOS_LITERAL_STRING("*"));
    }}
}
#endif /* PLATFORMSTL_OS_IS_WINDOWS */

/* ////////////////////////////////////////////////////////////////////// */

static string_t replace_all(string_t const& str, PAN_CHAR_T const* srch, PAN_CHAR_T const* repl)
{
    string_t    str2(str);
    size_t      srchLen = stlsoft::c_str_len(srch);

    for(;;)
    {
        size_t  lpos = str2.find(srch);
        size_t  rpos = lpos + srchLen;

        if(string_t::npos == lpos)
        {
            break;
        }

        str2 = stlsoft::left(str2, lpos) + repl + stlsoft::right(str2, str2.length() - rpos);
    }

    return str2;
}

static int test_time(char const* file, int line, size_t cchBuff, int flags, size_t expectedResult, PAN_CHAR_T const* pattern)
{
    if('?' == 0[pattern])
    {
        string_t str = translate_pattern(pattern);

        return test_time(file, line, cchBuff, flags, expectedResult, str.c_str());
    }

    pantheios::util::auto_buffer_selector<PAN_CHAR_T, 256>::type    buff(cchBuff + 1);
    pan_beutil_time_t                                               tm(buff.size() - 1, (buff.size() > 1) ? &buff[0] : NULL);
    size_t                                                          actualResult = pantheios_util_getCurrentTime(&tm, flags);
    int                                                             matchRes = 0;

    buff[cchBuff] = '\0';
    if(cchBuff > 0)
    {
        buff[actualResult] = '\0';
    }

    if( expectedResult == actualResult &&
#ifdef PANTHEIOS_USE_WIDE_STRINGS
        0 == (matchRes = shwild::match(stlsoft::w2m(pattern), stlsoft::w2m(buff.data()))))
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
        0 == (matchRes = shwild::match(pattern, buff.data())))
#endif /* PANTHEIOS_USE_WIDE_STRINGS */
    {
        return true;
    }
    else
    {
        PAN_CHAR_T  sz[21];
        string_t    message;

        message += PANTHEIOS_LITERAL_STRING("time test failed: ");
        message += PANTHEIOS_LITERAL_STRING("expected length='");
        message += stlsoft::integer_to_string(&sz[0], STLSOFT_NUM_ELEMENTS(sz), expectedResult);
        message += PANTHEIOS_LITERAL_STRING("';");
        message += PANTHEIOS_LITERAL_STRING("pattern='");
        message += stlsoft::c_str_ptr(pattern);
        message += PANTHEIOS_LITERAL_STRING("';");
        message += PANTHEIOS_LITERAL_STRING("actual length='");
        message += stlsoft::integer_to_string(&sz[0], STLSOFT_NUM_ELEMENTS(sz), actualResult);
        message += PANTHEIOS_LITERAL_STRING("';");
        message += PANTHEIOS_LITERAL_STRING("result='");
        message += buff.data();
        message += PANTHEIOS_LITERAL_STRING("';");
        message += PANTHEIOS_LITERAL_STRING("match result='");
        message += stlsoft::integer_to_string(&sz[0], STLSOFT_NUM_ELEMENTS(sz), matchRes);
        message += PANTHEIOS_LITERAL_STRING("'");
        message += PANTHEIOS_LITERAL_STRING(";");

#ifdef PANTHEIOS_USE_WIDE_STRINGS
        xtests::c::xtests_writeFailMessage(file, line, NULL, stlsoft::w2m(message), "");
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
        xtests::c::xtests_writeFailMessage(file, line, NULL, message, "");
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

        return false;
    }
}

static string_t translate_pattern(PAN_CHAR_T const* pattern)
{
    STLSOFT_ASSERT('?' == 0[pattern]);

    string_t str(pattern + 1);

    str = replace_all(str, PANTHEIOS_LITERAL_STRING("<Mon>"), PANTHEIOS_LITERAL_STRING("[JFMASOND][aepuco][nbrylgptvc]"));

    str = replace_all(str, PANTHEIOS_LITERAL_STRING("<D>"), PANTHEIOS_LITERAL_STRING("[0-9]"));
    str = replace_all(str, PANTHEIOS_LITERAL_STRING("<DD>"), PANTHEIOS_LITERAL_STRING("[0-9][0-9]"));

    return str;
}

/* ///////////////////////////// end of file //////////////////////////// */
