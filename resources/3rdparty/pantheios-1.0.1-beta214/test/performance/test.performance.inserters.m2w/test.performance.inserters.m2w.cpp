/* /////////////////////////////////////////////////////////////////////////
 * File:        test/performance/test.performance.inserters.m2w/test.performance.inserters.m2w.cpp
 *
 * Purpose:     Implementation file for the test.performance.inserters.m2w project.
 *
 * Created:     22nd November 2010
 * Updated:     10th January 2011
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2010-2011, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS

/* Pantheios Header Files */
#include <pantheios/pantheios.h>
#ifndef PANTHEIOS_USE_WIDE_STRINGS
# error This program source only valid in wide string builds
#endif /* !PANTHEIOS_USE_WIDE_STRINGS */
#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/exception.hpp>
#include <pantheios/inserters/m2w.hpp>
#include <pantheios/frontends/fe.simple.h>

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>
#include <platformstl/platformstl.hpp>
#include <platformstl/performance/performance_counter.hpp>
#if defined(STLSOFT_OS_IS_WINDOWS)
# include <winstl/conversion/m2w.hpp>
#else /* ? OS */
# include <stlsoft/conversion/m2w.hpp>
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

extern "C" const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[]    =   L"test.performance.inserters.m2w";

/* ////////////////////////////////////////////////////////////////////// */

static int main_(int argc, char** argv)
{
    platformstl::performance_counter                counter;
    platformstl::performance_counter::interval_type tm_cvrt_small   =   1;
    platformstl::performance_counter::interval_type tm_insrt_small  =   1;
    platformstl::performance_counter::interval_type tm_cvrt_large   =   1;
    platformstl::performance_counter::interval_type tm_insrt_large  =   1;
    int                                             len_cvrt_small  =   1;
    int                                             len_insrt_small =   1;
    int                                             len_cvrt_large  =   1;
    int                                             len_insrt_large =   1;

    const char  mbstr1[]    =   "str1";
    const char  mbstr2[]    =   "the second wide string";
    const char  mbstr3[]    =   "the third wide string, which is quite a bit bigger than the first and second put together, but still is massively smaller than the fourth";
    char        mbstr4[10001];  std::fill(&mbstr4[0], &mbstr4[0] + STLSOFT_NUM_ELEMENTS(mbstr4) - 1, '~'); mbstr4[STLSOFT_NUM_ELEMENTS(mbstr4) - 1] = L'\0';

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
            fputs("USAGE: test.performance.inserters.m2w [{on|off}]", stderr);
        }
    }

    // Small (convert)

    { for(int WARMUPS = 2; 0 != WARMUPS; --WARMUPS)
    {
#if defined(STLSOFT_OS_IS_WINDOWS)
        using winstl::m2w;
#else /* ? OS */
        using stlsoft::m2w;
#endif /* OS */

        counter.start();
        { for(int i = 0; i != ITERATIONS; ++i)
        {
            len_cvrt_small = pantheios::log_NOTICE(L"abc ", m2w(mbstr1).c_str(), L" - ", m2w(mbstr2).c_str(), L" - ", m2w(mbstr3).c_str(), L".");
        }}
        counter.stop();
        tm_cvrt_small = counter.get_microseconds();
    }}

    // Small (inserter)

    { for(int WARMUPS = 2; 0 != WARMUPS; --WARMUPS)
    {
        using pantheios::m2w;

        counter.start();
        { for(int i = 0; i != ITERATIONS; ++i)
        {
            len_insrt_small = pantheios::log_NOTICE(L"abc ", m2w(mbstr1), L" - ", m2w(mbstr2), L" - ", m2w(mbstr3), L".");
        }}
        counter.stop();
        tm_insrt_small = counter.get_microseconds();
    }}

    fprintf(stderr, "small: winstl:pantheios: %2.4g\n", (double)tm_cvrt_small/(double)tm_insrt_small);

    // Large (inserter)

    { for(int WARMUPS = 2; 0 != WARMUPS; --WARMUPS)
    {
#if defined(STLSOFT_OS_IS_WINDOWS)
        using winstl::m2w;
#else /* ? OS */
        using stlsoft::m2w;
#endif /* OS */

        counter.start();
        { for(int i = 0; i != ITERATIONS; ++i)
        {
            len_cvrt_large = pantheios::log_NOTICE(L"abc ", m2w(mbstr1).c_str(), L" - ", m2w(mbstr2).c_str(), L" - ", m2w(mbstr3).c_str(), L" - ", m2w(mbstr4).c_str(), L".");
        }}
        counter.stop();
        tm_cvrt_large = counter.get_microseconds();
    }}

    // Large (inserter)

    { for(int WARMUPS = 2; 0 != WARMUPS; --WARMUPS)
    {
        using pantheios::m2w;

        counter.start();
        { for(int i = 0; i != ITERATIONS; ++i)
        {
            len_insrt_large = pantheios::log_NOTICE(L"abc ", m2w(mbstr1), L" - ", m2w(mbstr2), L" - ", m2w(mbstr3), L" - ", m2w(mbstr4), L".");
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
        res = main_(argc, argv);
    }
    catch(std::exception& x)
    {
        pantheios::log_ALERT(L"Unexpected general error: ", x, L". Application terminating");

        res = EXIT_FAILURE;
    }
    catch(...)
    {
        pantheios::logputs(pantheios::emergency, L"Unhandled unknown error");

        res = EXIT_FAILURE;
    }

#if defined(_MSC_VER) && \
    defined(_DEBUG)
    _CrtMemDumpAllObjectsSince(&memState);
#endif /* _MSC_VER) && _DEBUG */

    return res;
}

/* ///////////////////////////// end of file //////////////////////////// */
