/* /////////////////////////////////////////////////////////////////////////
 * File:        test/performance/test.performance.inserters.w2m/test.performance.inserters.w2m.cpp
 *
 * Purpose:     Implementation file for the test.performance.inserters.w2m project.
 *
 * Created:     2nd September 2008
 * Updated:     10th January 2011
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2008-2011, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */



/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/exception.hpp>
#include <pantheios/inserters/w2m.hpp>
#include <pantheios/frontends/fe.simple.h>

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>
#include <platformstl/platformstl.hpp>
#include <platformstl/performance/performance_counter.hpp>
#if defined(STLSOFT_OS_IS_WINDOWS)
# include <winstl/conversion/w2m.hpp>
#else /* ? OS */
# include <stlsoft/conversion/w2m.hpp>
#endif /* OS */

/* Standard C++ Header Files */
#include <exception>

/* Standard C Header Files */
#include <stdio.h>
#include <stdlib.h>

#if defined(_MSC_VER) && \
    defined(_DEBUG)
# include <crtdbg.h>
#endif /* _MSC_VER) && _DEBUG */

/* /////////////////////////////////////////////////////////////////////////
 * Macros and definitions
 */

#ifdef _DEBUG
const int       ITERATIONS  =   1;
#else /* ? _DEBUG */
const int       ITERATIONS  =   10000;
#endif /* _DEBUG */

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

extern "C" const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[]    =   PANTHEIOS_LITERAL_STRING("test.performance.inserters.w2m");

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

#if 0
typedef std::string     string_t;
#endif /* 0 */

/* /////////////////////////////////////////////////////////////////////////
 * Forward declarations
 */

/* ////////////////////////////////////////////////////////////////////// */

static int main_(int argc, char** argv)
{
#ifndef PANTHEIOS_USE_WIDE_STRINGS

    platformstl::performance_counter                counter;
    platformstl::performance_counter::interval_type tm_cvrt_small   =   1;
    platformstl::performance_counter::interval_type tm_insrt_small  =   1;
    platformstl::performance_counter::interval_type tm_cvrt_large   =   1;
    platformstl::performance_counter::interval_type tm_insrt_large  =   1;
    int                                             len_cvrt_small  =   1;
    int                                             len_insrt_small =   1;
    int                                             len_cvrt_large  =   1;
    int                                             len_insrt_large =   1;

    const wchar_t   wstr1[]     =   L"str1";
    const wchar_t   wstr2[]     =   L"the second wide string";
    const wchar_t   wstr3[]     =   L"the third wide string, which is quite a bit bigger than the first and second put together, but still is massively smaller than the fourth";
    wchar_t         wstr4[10001];   std::fill(&wstr4[0], &wstr4[0] + STLSOFT_NUM_ELEMENTS(wstr4) - 1, L'~'); wstr4[STLSOFT_NUM_ELEMENTS(wstr4) - 1] = L'\0';

    if(1 != argc)
    {
        if(0 == ::strcmp("on", argv[1]))
        {
            pantheios_fe_simple_setSeverityCeiling(PANTHEIOS_SEV_DEBUG);
        }
        else if(0 == ::strcmp("off", argv[1]))
        {
            pantheios_fe_simple_setSeverityCeiling(PANTHEIOS_SEV_WARNING);
        }
        else
        {
            fputs("USAGE: test.performance.inserters.w2m [{on|off}]", stderr);
        }
    }

    // Small (convert)

    { for(int WARMUPS = 2; 0 != WARMUPS; --WARMUPS)
    {
#if defined(STLSOFT_OS_IS_WINDOWS)
        using winstl::w2m;
#else /* ? OS */
        using stlsoft::w2m;
#endif /* OS */

        counter.start();
        { for(int i = 0; i != ITERATIONS; ++i)
        {
            len_cvrt_small = pantheios::log_NOTICE("abc ", w2m(wstr1).c_str(), " - ", w2m(wstr2).c_str(), " - ", w2m(wstr3).c_str(), ".");
        }}
        counter.stop();
        tm_cvrt_small = counter.get_microseconds();
    }}

    // Small (inserter)

    { for(int WARMUPS = 2; 0 != WARMUPS; --WARMUPS)
    {
        using pantheios::w2m;

        counter.start();
        { for(int i = 0; i != ITERATIONS; ++i)
        {
            len_insrt_small = pantheios::log_NOTICE("abc ", w2m(wstr1), " - ", w2m(wstr2), " - ", w2m(wstr3), ".");
        }}
        counter.stop();
        tm_insrt_small = counter.get_microseconds();
    }}

    fprintf(stderr, "small: winstl:pantheios: %2.4g\n", (double)tm_cvrt_small/(double)tm_insrt_small);

    // Large (inserter)

    { for(int WARMUPS = 2; 0 != WARMUPS; --WARMUPS)
    {
#if defined(STLSOFT_OS_IS_WINDOWS)
        using winstl::w2m;
#else /* ? OS */
        using stlsoft::w2m;
#endif /* OS */

        counter.start();
        { for(int i = 0; i != ITERATIONS; ++i)
        {
            len_cvrt_large = pantheios::log_NOTICE("abc ", w2m(wstr1).c_str(), " - ", w2m(wstr2).c_str(), " - ", w2m(wstr3).c_str(), " - ", w2m(wstr4).c_str(), ".");
        }}
        counter.stop();
        tm_cvrt_large = counter.get_microseconds();
    }}

    // Large (inserter)

    { for(int WARMUPS = 2; 0 != WARMUPS; --WARMUPS)
    {
        using pantheios::w2m;

        counter.start();
        { for(int i = 0; i != ITERATIONS; ++i)
        {
            len_insrt_large = pantheios::log_NOTICE("abc ", w2m(wstr1), " - ", w2m(wstr2), " - ", w2m(wstr3), " - ", w2m(wstr4), ".");
        }}
        counter.stop();
        tm_insrt_large = counter.get_microseconds();
    }}

    if(len_cvrt_small != len_insrt_small)
    {
        fprintf(stderr, "small lengths don't match!\n");
    }
    if(len_cvrt_large != len_insrt_large)
    {
        fprintf(stderr, "small lengths don't match!\n");
    }

    fprintf(stderr, "large: winstl:pantheios: %2.4g\n", (double)tm_cvrt_large/(double)tm_insrt_large);

#else /* ? !PANTHEIOS_USE_WIDE_STRINGS */

    STLSOFT_SUPPRESS_UNUSED(argc);
    STLSOFT_SUPPRESS_UNUSED(argv);

#endif /* !PANTHEIOS_USE_WIDE_STRINGS */

    return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
    int             res;

#if defined(_MSC_VER) && \
    defined(_DEBUG)
    _CrtMemState    memState;
#endif /* _MSC_VER && _MSC_VER */

#if defined(_MSC_VER) && \
    defined(_DEBUG)
    _CrtMemCheckpoint(&memState);
#endif /* _MSC_VER && _MSC_VER */

#if 0
    { for(size_t i = 0; i < 0xffffffff; ++i){} }
#endif /* 0 */

    try
    {
#if defined(_DEBUG) || \
    defined(__SYNSOFT_DBS_DEBUG)
        puts("test.performance.inserters.w2m: " __STLSOFT_COMPILER_LABEL_STRING);
#endif /* debug */

        res = main_(argc, argv);
    }
    catch(std::exception& x)
    {
                pantheios::log_ALERT(PSTR("Unexpected general error: "), pantheios::exception(x), PSTR(". Application terminating"));

        res = EXIT_FAILURE;
    }
    catch(...)
    {
        pantheios::logputs(pantheios::emergency, PSTR("Unhandled unknown error"));

        res = EXIT_FAILURE;
    }

#if defined(_MSC_VER) && \
    defined(_DEBUG)
    _CrtMemDumpAllObjectsSince(&memState);
#endif /* _MSC_VER) && _DEBUG */

    return res;
}

/* ///////////////////////////// end of file //////////////////////////// */
