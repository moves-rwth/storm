/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.be.pantheios.COM/test.scratch.be.pantheios.COM.cpp
 *
 * Purpose:     Implementation file for the test.scratch.be.pantheios.COM project.
 *
 * Created:     9th October 2006
 * Updated:     6th August 2012
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2006-2012, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS

/* Open-RJ Header Files */
//#include <openrj/implicit_link.h>

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>
#include <pantheios/implicit_link/core.h>
#include <pantheios/implicit_link/fe.simple.h>
#if defined(unix) || \
    defined(__unix) || \
    defined(__unix__)
# include <pantheios/implicit_link/be.syslog.h>
#elif defined(_WIN32) || \
      defined(WIN32)
# include <pantheios/implicit_link/be.lrsplit.h>
# include <pantheios/implicit_link/bel.WindowsDebugger.h>
# include <pantheios/implicit_link/ber.WindowsSyslog.h>
#endif /* operating system */

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>

/* PlatformSTL Header Files */
#include <platformstl/platformstl.hpp>


/* UNIXEm Header Files */
#include <unixem/unixem.h>
#include <unixem/implicit_link.h>

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
#include <stdlib.h>

#if defined(_MSC_VER) && \
    defined(_DEBUG)
# include <crtdbg.h>
#endif /* _MSC_VER) && _DEBUG */

/* /////////////////////////////////////////////////////////////////////////
 * Macros and definitions
 */


/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.scratch.be.pantheios.COM");

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

static int main_(int /* argc */, char ** /*argv*/)
{
    {
        pantheios::log_DEBUG("debug");

    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
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
#endif /* debug */

        res = main_(argc, argv);
    }
    catch(std::exception &x)
    {
        fprintf(stderr, "Unhandled error: %s\n", x.what());

        res = EXIT_FAILURE;
    }
    catch(...)
    {
        fprintf(stderr, "Unhandled unknown error\n");

        res = EXIT_FAILURE;
    }

#if defined(_MSC_VER) && \
    defined(_DEBUG)
    _CrtMemDumpAllObjectsSince(&memState);
#endif /* _MSC_VER) && _DEBUG */

    return res;
}

/* ////////////////////////////////////////////////////////////////////// */
