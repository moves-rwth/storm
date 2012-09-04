/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.tstring/test.scratch.tstring.cpp
 *
 * Purpose:     Implementation file for the test.scratch.tstring project.
 *
 * Created:     23rd February 2010
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


#define PANTHEIOS_SAFE_ALLOW_SHIM_INTERMEDIATES


/* Pantheios Header Files */
#include <pantheios/pan.hpp>
#include <pantheios/inserters/w2m.hpp>

/* STLSoft Header Files */
#include <stlsoft/string/simple_string.hpp>
#include <platformstl/platformstl.hpp>

/* UNIXEm Header Files */
#if defined(_WIN32) || \
    defined(_WIN64)
# include <unixem/unixem.h>
#endif /* Win32 || Win64 */

/* Standard C++ Header Files */
#include <exception>
#if 0
#include <algorithm>
#include <iterator>
#include <list>
#include <string>
#include <vector>
#endif /* 0 */

#if !defined(__WATCOMC__) && \
    (   !defined(_MSC_VER) || \
        _MSC_VER >= 1100)

#else /* ? __WATCOMC__ */
namespace std
{
    using ::exception;
}
#endif /* __WATCOMC__ */

/* Standard C Header Files */
#include <stdio.h>
#include <stdlib.h>

#if defined(_MSC_VER) && \
    defined(_DEBUG)
# include <crtdbg.h>
#endif /* _MSC_VER) && _DEBUG */

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.scratch.tstring");

/* ////////////////////////////////////////////////////////////////////// */

static int main_(int /* argc */, char** /*argv*/)
{
    std::string             mbs("multibyte string");
    std::wstring            ws1(L"wide string #1");
    stlsoft::simple_wstring ws2(L"wide string #2");

    pan::log_NOTICE("mbs=", mbs, ", ws1=", pan::w2m(ws1), ", ws2=", pan::w2m(ws2));

#ifdef PANTHEIOS_SAFE_ALLOW_SHIM_INTERMEDIATES
    VARIANT         var;

    var.vt = VT_I4;
    var.lVal = -10;

    pan::log_DEBUG("var=", pan::w2m(var));
#endif /* PANTHEIOS_SAFE_ALLOW_SHIM_INTERMEDIATES */

    
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
        puts("test.scratch.tstring: " __STLSOFT_COMPILER_LABEL_STRING);
#endif /* debug */

        res = main_(argc, argv);
    }
    catch(std::exception& x)
    {
        pantheios::log_ALERT("Unexpected general error: ", x, ". Application terminating");

        res = EXIT_FAILURE;
    }
    catch(...)
    {
        pantheios::puts(pantheios::emergency, "Unhandled unknown error");

        res = EXIT_FAILURE;
    }

#if defined(_MSC_VER) && \
    defined(_DEBUG)
    _CrtMemDumpAllObjectsSince(&memState);
#endif /* _MSC_VER) && _DEBUG */

    return res;
}

/* ///////////////////////////// end of file //////////////////////////// */
