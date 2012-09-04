/* /////////////////////////////////////////////////////////////////////////
 * File:        test/unit/test.unit.bec.file/test.unit.bec.file.cpp
 *
 * Purpose:     Implementation file for the test.unit.be.file project.
 *
 * Created:     19th January 2008
 * Updated:     14th November 2010
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2008-2010, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

/* Pantheios Header Files */
#include <pantheios/backends/bec.file.h>
#include <pantheios/init_codes.h>
#include <pantheios/util/system/threadid.h>

/* STLSoft Header Files */
#include <stlsoft/conversion/char_conversions.hpp>
#include <stlsoft/conversion/integer_to_string.hpp>
#include <platformstl/filesystem/file_lines.hpp>
#include <platformstl/filesystem/filesystem_traits.hpp>

/* xTests Header Files */
#include <xtests/xtests.h>

/* Standard C++ Header Files */
#include <exception>                    // for std::exception
#include <new>                          // for std::bad_alloc

/* Standard C Header Files */
#include <stdio.h>                      // for fprintf(), remove()
#include <stdlib.h>                     // for exit codes

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* /////////////////////////////////////////////////////////////////////////
 * Macros
 */

#define PANTHEIOS_SEV_LEVELS_EQUAL(x, y)    XTESTS_TEST_INTEGER_EQUAL(static_cast<int>(x), static_cast<int>(y))

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

typedef platformstl::filesystem_traits<PAN_CHAR_T>  fs_traits_t;
typedef platformstl::basic_file_lines<char>         lines_m_t;
typedef platformstl::basic_file_lines<wchar_t>      lines_w_t;
typedef platformstl::basic_file_lines<PAN_CHAR_T>   lines_t;
typedef std::basic_string<char>                     string_m_t;
typedef std::basic_string<wchar_t>                  string_w_t;
typedef std::basic_string<PAN_CHAR_T>               string_t;

/* /////////////////////////////////////////////////////////////////////////
 * Character encoding
 */

#ifdef PANTHEIOS_USE_WIDE_STRINGS

# define XTESTS_TEST_STRING_EQUAL       XTESTS_TEST_WIDE_STRING_EQUAL

# define P_TO_M_(x)                     static_cast<char const*>(stlsoft::w2m((x)))
# define P_TO_W_(x)                     (x)

# define gettidstr_()                   gettidstr_w_()

#else /* ? PANTHEIOS_USE_WIDE_STRINGS */

# define XTESTS_TEST_STRING_EQUAL       XTESTS_TEST_MULTIBYTE_STRING_EQUAL

# define P_TO_M_(x)                     (x)
# define P_TO_W_(x)                     static_cast<wchar_t const*>(stlsoft::m2w((x)))

# define gettidstr_()                   gettidstr_m_()

#endif /* PANTHEIOS_USE_WIDE_STRINGS */

#ifdef PSTR
# undef PSTR
#endif
#define PSTR                            PANTHEIOS_LITERAL_STRING

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.unit.be.file");

/* /////////////////////////////////////////////////////////////////////////
 * Forward declarations
 */

namespace
{

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
static void test_1_0a();
static void test_1_0b();
static void test_1_0c();
static void test_1_0d();
static void test_1_0e();
static void test_1_0f();

static void test_2_00();
static void test_2_01();
static void test_2_02();
static void test_2_03();
static void test_2_04();
static void test_2_05();
static void test_2_06();
static void test_2_07();
static void test_2_08();
static void test_2_09();
static void test_2_0a();
static void test_2_0b();
static void test_2_0c();
static void test_2_0d();
static void test_2_0e();
static void test_2_0f();

static void test_3_00();
static void test_3_01();
static void test_3_02();
static void test_3_03();
static void test_3_04();
static void test_3_05();
static void test_3_06();
static void test_3_07();
static void test_3_08();
static void test_3_09();
static void test_3_0a();
static void test_3_0b();
static void test_3_0c();
static void test_3_0d();
static void test_3_0e();
static void test_3_0f();

static void test_4_00();
static void test_4_01();

} // anonymous namespace

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.unit.be.file", verbosity))
    {
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
        XTESTS_RUN_CASE(test_1_0a);
        XTESTS_RUN_CASE(test_1_0b);
        XTESTS_RUN_CASE(test_1_0c);
        XTESTS_RUN_CASE(test_1_0d);
        XTESTS_RUN_CASE(test_1_0e);
        XTESTS_RUN_CASE(test_1_0f);

        XTESTS_RUN_CASE(test_2_00);
        XTESTS_RUN_CASE(test_2_01);
        XTESTS_RUN_CASE(test_2_02);
        XTESTS_RUN_CASE(test_2_03);
        XTESTS_RUN_CASE(test_2_04);
        XTESTS_RUN_CASE(test_2_05);
        XTESTS_RUN_CASE(test_2_06);
        XTESTS_RUN_CASE(test_2_07);
        XTESTS_RUN_CASE(test_2_08);
        XTESTS_RUN_CASE(test_2_09);
        XTESTS_RUN_CASE(test_2_0a);
        XTESTS_RUN_CASE(test_2_0b);
        XTESTS_RUN_CASE(test_2_0c);
        XTESTS_RUN_CASE(test_2_0d);
        XTESTS_RUN_CASE(test_2_0e);
        XTESTS_RUN_CASE(test_2_0f);

        XTESTS_RUN_CASE(test_3_00);
        XTESTS_RUN_CASE(test_3_01);
        XTESTS_RUN_CASE(test_3_02);
        XTESTS_RUN_CASE(test_3_03);
        XTESTS_RUN_CASE(test_3_04);
        XTESTS_RUN_CASE(test_3_05);
        XTESTS_RUN_CASE(test_3_06);
        XTESTS_RUN_CASE(test_3_07);
        XTESTS_RUN_CASE(test_3_08);
        XTESTS_RUN_CASE(test_3_09);
        XTESTS_RUN_CASE(test_3_0a);
        XTESTS_RUN_CASE(test_3_0b);
        XTESTS_RUN_CASE(test_3_0c);
        XTESTS_RUN_CASE(test_3_0d);
        XTESTS_RUN_CASE(test_3_0e);
        XTESTS_RUN_CASE(test_3_0f);

        XTESTS_RUN_CASE(test_4_00);
        XTESTS_RUN_CASE(test_4_01);

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* ////////////////////////////////////////////////////////////////////// */

namespace
{

static string_m_t gettidstr_m_()
{
    char        num[21];
    size_t      n;// = 0; STLSOFT_SUPPRESS_UNUSED(n); // Suppress Borland C/C++ warning (which is correct)
    char const* p = stlsoft::integer_to_string(&num[0], STLSOFT_NUM_ELEMENTS(num), pantheios::util::getCurrentThreadId(), n);

    return string_m_t(p, n);
}

static string_w_t gettidstr_w_()
{
    wchar_t         num[21];
    size_t          n;// = 0; STLSOFT_SUPPRESS_UNUSED(n); // Suppress Borland C/C++ warning (which is correct)
    wchar_t const*  p = stlsoft::integer_to_string(&num[0], STLSOFT_NUM_ELEMENTS(num), pantheios::util::getCurrentThreadId(), n);

    return string_w_t(p, n);
}

static PAN_CHAR_T const* strings[] =
{
        PSTR("abc")
    ,   PSTR("ABC")
    ,   PSTR("abcdefghijklmnopqrstuvwxyz")
    ,   PSTR("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
    ,   PSTR("abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")
    ,   PSTR("abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")
    ,   PSTR("00000000000000000000000000000000000000000000000000000000000000000000001111111111111111111111111111111111111111111111111111111111222222222222222222222222222222222222222223333333333333333333333333")
};




static void test_1_00()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_1_01.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_m_t           lines(FILE_PATH);
        string_m_t const    resultPrefix = "";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_MULTIBYTE_STRING_EQUAL((resultPrefix + P_TO_M_(strings[i])), lines[i]);
        }}
    }
}

static void test_1_01()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_1_02.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_m_t           lines(FILE_PATH);
        string_m_t const    resultPrefix = "[" + string_m_t(P_TO_M_(PANTHEIOS_FE_PROCESS_IDENTITY)) + "]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_MULTIBYTE_STRING_EQUAL((resultPrefix + P_TO_M_(strings[i])), lines[i]);
        }}
    }
}

static void test_1_02()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_1_01.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_m_t           lines(FILE_PATH);
        string_m_t const    resultPrefix = "[" + gettidstr_m_() + "]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_MULTIBYTE_STRING_EQUAL((resultPrefix + P_TO_M_(strings[i])), lines[i]);
        }}
    }
}

static void test_1_03()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_1_02.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_m_t           lines(FILE_PATH);
        string_m_t const    resultPrefix = "[" + string_m_t(P_TO_M_(PANTHEIOS_FE_PROCESS_IDENTITY)) + "." + gettidstr_m_() + "]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_MULTIBYTE_STRING_EQUAL((resultPrefix + P_TO_M_(strings[i])), lines[i]);
        }}
    }
}

static void test_1_04()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_1_02.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_m_t           lines(FILE_PATH);
        string_m_t const    resultPrefix = "[" + string_m_t(P_TO_M_(PANTHEIOS_FE_PROCESS_IDENTITY)) + "." + gettidstr_m_() + "]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_MULTIBYTE_STRING_EQUAL((resultPrefix + P_TO_M_(strings[i])), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_1_05()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_1_03.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_m_t           lines(FILE_PATH);
        string_m_t const    resultPrefix = "[" + string_m_t(P_TO_M_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + "]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_MULTIBYTE_STRING_EQUAL((resultPrefix + P_TO_M_(strings[i])), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_1_06()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_1_03.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_m_t           lines(FILE_PATH);
        string_m_t const    resultPrefix = "[" + string_m_t(P_TO_M_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + "]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_MULTIBYTE_STRING_EQUAL((resultPrefix + P_TO_M_(strings[i])), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_1_07()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_1_03.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_m_t           lines(FILE_PATH);
        string_m_t const    resultPrefix = "[" + string_m_t(P_TO_M_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + "]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_MULTIBYTE_STRING_EQUAL((resultPrefix + P_TO_M_(strings[i])), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_1_08()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_1_04.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_m_t           lines(FILE_PATH);
        string_m_t const    resultPrefix = "[" + string_m_t(P_TO_M_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + "]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_MULTIBYTE_STRING_EQUAL((resultPrefix + P_TO_M_(strings[i])), lines[i]);
        }}
    }
}

static void test_1_09()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_1_03.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_m_t           lines(FILE_PATH);
        string_m_t const    resultPrefix = "[" + string_m_t(P_TO_M_(PANTHEIOS_FE_PROCESS_IDENTITY)) + "; " + string_m_t(P_TO_M_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + "]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_MULTIBYTE_STRING_EQUAL((resultPrefix + P_TO_M_(strings[i])), lines[i]);
        }}
    }
}

static void test_1_0a()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_1_04.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_m_t           lines(FILE_PATH);
        string_m_t const    resultPrefix = "[" + gettidstr_m_() + "; " + string_m_t(P_TO_M_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + "]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_MULTIBYTE_STRING_EQUAL((resultPrefix + P_TO_M_(strings[i])), lines[i]);
        }}
    }
}

static void test_1_0b()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_1_04.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_m_t           lines(FILE_PATH);
        string_m_t const    resultPrefix = "[" + string_m_t(P_TO_M_(PANTHEIOS_FE_PROCESS_IDENTITY)) + "." + gettidstr_m_() + "; " + string_m_t(P_TO_M_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + "]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_MULTIBYTE_STRING_EQUAL((resultPrefix + P_TO_M_(strings[i])), lines[i]);
        }}
    }
}

static void test_1_0c()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_1_04.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_m_t           lines(FILE_PATH);
        string_m_t const    resultPrefix = "[" + string_m_t(P_TO_M_(PANTHEIOS_FE_PROCESS_IDENTITY)) + "." + gettidstr_m_() + "; " + string_m_t(P_TO_M_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + "]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_MULTIBYTE_STRING_EQUAL((resultPrefix + P_TO_M_(strings[i])), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_1_0d()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_1_04.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_m_t           lines(FILE_PATH);
        string_m_t const    resultPrefix = "[" + string_m_t(P_TO_M_(PANTHEIOS_FE_PROCESS_IDENTITY)) + "." + gettidstr_m_() + "; " + string_m_t(P_TO_M_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + "]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_MULTIBYTE_STRING_EQUAL((resultPrefix + P_TO_M_(strings[i])), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_1_0e()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_1_04.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_m_t           lines(FILE_PATH);
        string_m_t const    resultPrefix = "[" + string_m_t(P_TO_M_(PANTHEIOS_FE_PROCESS_IDENTITY)) + "." + gettidstr_m_() + "; " + string_m_t(P_TO_M_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + "]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_MULTIBYTE_STRING_EQUAL((resultPrefix + P_TO_M_(strings[i])), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_1_0f()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_1_04.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_m_t           lines(FILE_PATH);
        string_m_t const    resultPrefix = "[" + string_m_t(P_TO_M_(PANTHEIOS_FE_PROCESS_IDENTITY)) + "." + gettidstr_m_() + "; " + string_m_t(P_TO_M_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + "]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_MULTIBYTE_STRING_EQUAL((resultPrefix + P_TO_M_(strings[i])), lines[i]);
#endif /* 0 */
        }}
    }
}



static void test_2_00()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_2_01.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_w_t           lines(FILE_PATH);
        string_w_t const    resultPrefix = L"";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_WIDE_STRING_EQUAL((resultPrefix + P_TO_W_(strings[i])), lines[i]);
        }}
    }
}

static void test_2_01()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_2_02.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_w_t           lines(FILE_PATH);
        string_w_t const    resultPrefix = L"[" + string_w_t(P_TO_W_(PANTHEIOS_FE_PROCESS_IDENTITY)) + L"]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_WIDE_STRING_EQUAL((resultPrefix + P_TO_W_(strings[i])), lines[i]);
        }}
    }
}

static void test_2_02()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_2_01.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_w_t           lines(FILE_PATH);
        string_w_t const    resultPrefix = L"[" + gettidstr_w_() + L"]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_WIDE_STRING_EQUAL((resultPrefix + P_TO_W_(strings[i])), lines[i]);
        }}
    }
}

static void test_2_03()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_2_02.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_w_t           lines(FILE_PATH);
        string_w_t const    resultPrefix = L"[" + string_w_t(P_TO_W_(PANTHEIOS_FE_PROCESS_IDENTITY)) + L"." + gettidstr_w_() + L"]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_WIDE_STRING_EQUAL((resultPrefix + P_TO_W_(strings[i])), lines[i]);
        }}
    }
}

static void test_2_04()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_2_02.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_w_t           lines(FILE_PATH);
        string_w_t const    resultPrefix = L"[" + string_w_t(P_TO_W_(PANTHEIOS_FE_PROCESS_IDENTITY)) + L"." + gettidstr_w_() + L"]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_WIDE_STRING_EQUAL((resultPrefix + P_TO_W_(strings[i])), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_2_05()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_2_03.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_w_t           lines(FILE_PATH);
        string_w_t const    resultPrefix = L"[" + string_w_t(P_TO_W_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + L"]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_WIDE_STRING_EQUAL((resultPrefix + P_TO_W_(strings[i])), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_2_06()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_2_03.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_w_t           lines(FILE_PATH);
        string_w_t const    resultPrefix = L"[" + string_w_t(P_TO_W_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + L"]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_WIDE_STRING_EQUAL((resultPrefix + P_TO_W_(strings[i])), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_2_07()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_2_03.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_w_t           lines(FILE_PATH);
        string_w_t const    resultPrefix = L"[" + string_w_t(P_TO_W_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + L"]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_WIDE_STRING_EQUAL((resultPrefix + P_TO_W_(strings[i])), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_2_08()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_2_04.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_w_t           lines(FILE_PATH);
        string_w_t const    resultPrefix = L"[" + string_w_t(P_TO_W_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + L"]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_WIDE_STRING_EQUAL((resultPrefix + P_TO_W_(strings[i])), lines[i]);
        }}
    }
}

static void test_2_09()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_2_03.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_w_t           lines(FILE_PATH);
        string_w_t const    resultPrefix = L"[" + string_w_t(P_TO_W_(PANTHEIOS_FE_PROCESS_IDENTITY)) + L"; " + string_w_t(P_TO_W_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + L"]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_WIDE_STRING_EQUAL((resultPrefix + P_TO_W_(strings[i])), lines[i]);
        }}
    }
}

static void test_2_0a()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_2_04.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_w_t           lines(FILE_PATH);
        string_w_t const    resultPrefix = L"[" + gettidstr_w_() + L"; " + string_w_t(P_TO_W_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + L"]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_WIDE_STRING_EQUAL((resultPrefix + P_TO_W_(strings[i])), lines[i]);
        }}
    }
}

static void test_2_0b()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_2_04.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_w_t           lines(FILE_PATH);
        string_w_t const    resultPrefix = L"[" + string_w_t(P_TO_W_(PANTHEIOS_FE_PROCESS_IDENTITY)) + L"." + gettidstr_w_() + L"; " + string_w_t(P_TO_W_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + L"]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_WIDE_STRING_EQUAL((resultPrefix + P_TO_W_(strings[i])), lines[i]);
        }}
    }
}

static void test_2_0c()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_2_04.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_w_t           lines(FILE_PATH);
        string_w_t const    resultPrefix = L"[" + string_w_t(P_TO_W_(PANTHEIOS_FE_PROCESS_IDENTITY)) + L"." + gettidstr_w_() + L"; " + string_w_t(P_TO_W_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + L"]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_WIDE_STRING_EQUAL((resultPrefix + P_TO_W_(strings[i])), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_2_0d()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_2_04.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_w_t           lines(FILE_PATH);
        string_w_t const    resultPrefix = L"[" + string_w_t(P_TO_W_(PANTHEIOS_FE_PROCESS_IDENTITY)) + L"." + gettidstr_w_() + L"; " + string_w_t(P_TO_W_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + L"]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_WIDE_STRING_EQUAL((resultPrefix + P_TO_W_(strings[i])), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_2_0e()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_2_04.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_w_t           lines(FILE_PATH);
        string_w_t const    resultPrefix = L"[" + string_w_t(P_TO_W_(PANTHEIOS_FE_PROCESS_IDENTITY)) + L"." + gettidstr_w_() + L"; " + string_w_t(P_TO_W_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + L"]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_WIDE_STRING_EQUAL((resultPrefix + P_TO_W_(strings[i])), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_2_0f()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_2_04.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_w_t           lines(FILE_PATH);
        string_w_t const    resultPrefix = L"[" + string_w_t(P_TO_W_(PANTHEIOS_FE_PROCESS_IDENTITY)) + L"." + gettidstr_w_() + L"; " + string_w_t(P_TO_W_(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY))) + L"]: ";

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_WIDE_STRING_EQUAL((resultPrefix + P_TO_W_(strings[i])), lines[i]);
#endif /* 0 */
        }}
    }
}



static void test_3_00()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_3_01.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_t             lines(FILE_PATH);
        string_t const      resultPrefix = PSTR("");

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_STRING_EQUAL((resultPrefix + strings[i]), lines[i]);
        }}
    }
}

static void test_3_01()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_3_02.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_t             lines(FILE_PATH);
        string_t const      resultPrefix = PSTR("[") + string_t(PANTHEIOS_FE_PROCESS_IDENTITY) + PSTR("]: ");

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_STRING_EQUAL((resultPrefix + strings[i]), lines[i]);
        }}
    }
}

static void test_3_02()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_3_01.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_t             lines(FILE_PATH);
        string_t const      resultPrefix = PSTR("[") + gettidstr_() + PSTR("]: ");

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_STRING_EQUAL(resultPrefix + strings[i], lines[i]);
        }}
    }
}

static void test_3_03()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_3_02.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_t             lines(FILE_PATH);
        string_t const      resultPrefix = PSTR("[") + string_t(PANTHEIOS_FE_PROCESS_IDENTITY) + PSTR(".") + gettidstr_() + PSTR("]: ");

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_STRING_EQUAL((resultPrefix + strings[i]), lines[i]);
        }}
    }
}

static void test_3_04()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_3_02.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_t             lines(FILE_PATH);
        string_t const      resultPrefix = PSTR("[") + string_t(PANTHEIOS_FE_PROCESS_IDENTITY) + PSTR(".") + gettidstr_() + PSTR("]: ");

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_STRING_EQUAL((resultPrefix + strings[i]), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_3_05()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_3_03.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_t             lines(FILE_PATH);
        string_t const      resultPrefix = PSTR("[") + string_t(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY)) + PSTR("]: ");

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_STRING_EQUAL((resultPrefix + strings[i]), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_3_06()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_3_03.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_t             lines(FILE_PATH);
        string_t const      resultPrefix = PSTR("[") + string_t(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY)) + PSTR("]: ");

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_STRING_EQUAL((resultPrefix + strings[i]), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_3_07()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_3_03.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_t             lines(FILE_PATH);
        string_t const      resultPrefix = PSTR("[") + string_t(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY)) + PSTR("]: ");

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_STRING_EQUAL((resultPrefix + strings[i]), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_3_08()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_3_04.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_t             lines(FILE_PATH);
        string_t const      resultPrefix = PSTR("[") + string_t(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY)) + PSTR("]: ");

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_STRING_EQUAL((resultPrefix + strings[i]), lines[i]);
        }}
    }
}

static void test_3_09()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_3_03.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_t             lines(FILE_PATH);
        string_t const      resultPrefix = PSTR("[") + string_t(PANTHEIOS_FE_PROCESS_IDENTITY) + PSTR("; ") + string_t(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY)) + PSTR("]: ");

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_STRING_EQUAL((resultPrefix + strings[i]), lines[i]);
        }}
    }
}

static void test_3_0a()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_3_0a.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_t             lines(FILE_PATH);
        string_t const      resultPrefix = PSTR("[") + gettidstr_() + PSTR("; ") + string_t(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY)) + PSTR("]: ");

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_STRING_EQUAL((resultPrefix + strings[i]), lines[i]);
        }}
    }
}

static void test_3_0b()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_3_0b.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_t             lines(FILE_PATH);
        string_t const      resultPrefix = PSTR("[") + string_t(PANTHEIOS_FE_PROCESS_IDENTITY) + PSTR(".") + gettidstr_() + PSTR("; ") + string_t(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY)) + PSTR("]: ");

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            XTESTS_TEST_STRING_EQUAL((resultPrefix + strings[i]), lines[i]);
        }}
    }
}

static void test_3_0c()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_3_0c.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_t             lines(FILE_PATH);
        string_t const      resultPrefix = PSTR("[") + string_t(PANTHEIOS_FE_PROCESS_IDENTITY) + PSTR(".") + gettidstr_() + PSTR("; ") + string_t(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY)) + PSTR("]: ");

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_STRING_EQUAL((resultPrefix + strings[i]), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_3_0d()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_3_0d.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_t             lines(FILE_PATH);
        string_t const      resultPrefix = PSTR("[") + string_t(PANTHEIOS_FE_PROCESS_IDENTITY) + PSTR(".") + gettidstr_() + PSTR("; ") + string_t(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY)) + PSTR("]: ");

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_STRING_EQUAL((resultPrefix + strings[i]), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_3_0e()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_3_0e.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_t             lines(FILE_PATH);
        string_t const      resultPrefix = PSTR("[") + string_t(PANTHEIOS_FE_PROCESS_IDENTITY) + PSTR(".") + gettidstr_() + PSTR("; ") + string_t(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY)) + PSTR("]: ");

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_STRING_EQUAL((resultPrefix + strings[i]), lines[i]);
#endif /* 0 */
        }}
    }
}

static void test_3_0f()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
//              |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
//              |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
//              |   PANTHEIOS_BE_INIT_F_NO_DATETIME
//              |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_3_0f.results");

        // create the output file
        pantheios_be_file_setFilePath(FILE_PATH);

        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, strings[i], stlsoft::c_str_len(strings[i]));
        }}

        pantheios_be_file_setFilePath(NULL);

        // close the output file
        lines_t             lines(FILE_PATH);
        string_t const      resultPrefix = PSTR("[") + string_t(PANTHEIOS_FE_PROCESS_IDENTITY) + PSTR(".") + gettidstr_() + PSTR("; ") + string_t(pantheios::getSeverityString(PANTHEIOS_SEV_EMERGENCY)) + PSTR("]: ");

        fs_traits_t::unlink_file(FILE_PATH);

        // Start the tests
        XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(STLSOFT_NUM_ELEMENTS(strings), lines.size()));
        { for(size_t i = 0; i != STLSOFT_NUM_ELEMENTS(strings); ++i)
        {
#if 0
            XTESTS_TEST_STRING_EQUAL((resultPrefix + strings[i]), lines[i]);
#endif /* 0 */
        }}
    }
}


static void test_4_00()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_DELETE_IF_EMPTY
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_4_00.results");


        { // (i) Writing something (none-0 length)
            pantheios_be_file_setFilePath(FILE_PATH);

            XTESTS_TEST_BOOLEAN_TRUE(fs_traits_t::file_exists(FILE_PATH));

            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, PANTHEIOS_LITERAL_STRING("abc"), 3u);

            pantheios_be_file_setFilePath(NULL);

            XTESTS_TEST_BOOLEAN_TRUE(fs_traits_t::file_exists(FILE_PATH));
        }

        { // (i) Writing something (0 length)
            pantheios_be_file_setFilePath(FILE_PATH);

            XTESTS_TEST_BOOLEAN_TRUE(fs_traits_t::file_exists(FILE_PATH));

            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, PANTHEIOS_LITERAL_STRING("abc"), 0u);

            pantheios_be_file_setFilePath(NULL);

            XTESTS_TEST_BOOLEAN_TRUE(fs_traits_t::file_exists(FILE_PATH));
        }

        { // (i) Writing nothing
            pantheios_be_file_setFilePath(FILE_PATH);

            XTESTS_TEST_BOOLEAN_TRUE(fs_traits_t::file_exists(FILE_PATH));

            pantheios_be_file_setFilePath(NULL);

            XTESTS_TEST_BOOLEAN_FALSE(fs_traits_t::file_exists(FILE_PATH));
        }


        // ... do it anyway ...
        fs_traits_t::unlink_file(FILE_PATH);
    }
}

static void test_4_01()
{
    pan_be_file_init_t  init;

    pantheios_be_file_getDefaultAppInit(&init);

    init.flags  |=  0
                |   PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
                |   PANTHEIOS_BE_INIT_F_NO_THREAD_ID
                |   PANTHEIOS_BE_INIT_F_NO_DATETIME
                |   PANTHEIOS_BE_INIT_F_NO_SEVERITY
                |   0
                |   PANTHEIOS_BE_FILE_F_TRUNCATE
                |   PANTHEIOS_BE_FILE_F_DELETE_IF_EMPTY
                ;

    void*   token;
    int     res = pantheios_be_file_init(PANTHEIOS_FE_PROCESS_IDENTITY, PANTHEIOS_BEID_ALL, &init, NULL, &token);

    if(PANTHEIOS_INIT_RC_SUCCESS != res)
    {
        XTESTS_FAIL_WITH_QUALIFIER("failed to initialise bec.file", pantheios::getInitErrorString(res));
    }
    else
    {
        stlsoft::scoped_handle<void*>   uninit_(token, pantheios_be_file_uninit);

        static const PAN_CHAR_T FILE_PATH[] = PANTHEIOS_LITERAL_STRING("test_4_00.results");


        { // (i) Writing something (none-0 length)
            pantheios_be_file_setFilePath(FILE_PATH);

            XTESTS_TEST_BOOLEAN_TRUE(fs_traits_t::file_exists(FILE_PATH));

            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, PANTHEIOS_LITERAL_STRING("abc"), 3u);

            pantheios_be_file_setFilePath(NULL);

            XTESTS_TEST_BOOLEAN_TRUE(fs_traits_t::file_exists(FILE_PATH));
        }

        { // (i) Writing something (0 length)
            pantheios_be_file_setFilePath(FILE_PATH);

            XTESTS_TEST_BOOLEAN_TRUE(fs_traits_t::file_exists(FILE_PATH));

            pantheios_be_file_logEntry(NULL, token, PANTHEIOS_SEV_EMERGENCY, PANTHEIOS_LITERAL_STRING("abc"), 0u);

            pantheios_be_file_setFilePath(NULL);

            XTESTS_TEST_BOOLEAN_TRUE(fs_traits_t::file_exists(FILE_PATH));
        }

        { // (i) Writing nothing
            pantheios_be_file_setFilePath(FILE_PATH);

            XTESTS_TEST_BOOLEAN_TRUE(fs_traits_t::file_exists(FILE_PATH));

            pantheios_be_file_setFilePath(NULL);

            XTESTS_TEST_BOOLEAN_FALSE(fs_traits_t::file_exists(FILE_PATH));
        }


        // ... do it anyway ...
        fs_traits_t::unlink_file(FILE_PATH);
    }
}


} // anonymous namespace

/* ///////////////////////////// end of file //////////////////////////// */
