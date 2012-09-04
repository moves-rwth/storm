/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.bec.WindowsMessageBox/test.scratch.bec.WindowsMessageBox.cpp
 *
 * Purpose:     Implementation file for the test.scratch.bec.WindowsMessageBox project.
 *
 * Created:     14th March 2008
 * Updated:     6th August 2012
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



/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>
#include <winstl/winstl.h>
#include <winstl/error/error_desc.hpp>

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


/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.scratch.bec.WindowsMessageBox");

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

static int main_(int /* argc */, char** /*argv*/)
{
    pantheios::log_DEBUG("debug");
    pantheios::log_INFORMATIONAL("informational");
    pantheios::log_NOTICE("notice");
    pantheios::log_WARNING("warning");
    pantheios::log_ERROR("error");
    pantheios::log_CRITICAL("critical");
    pantheios::log_ALERT("alert");
    pantheios::log_EMERGENCY("emergency");

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
        puts("test.scratch.bec.WindowsMessageBox: " __STLSOFT_COMPILER_LABEL_STRING);
#endif /* debug */

        res = main_(argc, argv);
    }
    catch(std::exception &x)
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

/* ////////////////////////////////////////////////////////////////////// */
