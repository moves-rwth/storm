/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.fe.WindowsRegistry/test.scratch.fe.WindowsRegistry.cpp
 *
 * Purpose:     Implementation file for the test.scratch.fe.WindowsRegistry project.
 *
 * Created:     28th October 2007
 * Updated:     6th August 2012
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2007-2012, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */



#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>

/* STLSoft Header Files */
#include <winstl/error/error_desc.hpp>

/* Standard C++ Header Files */
#include <exception>
#include <iostream>

/* Standard C Header Files */
#include <stdlib.h>

#if defined(_MSC_VER) && \
    defined(_DEBUG)
# include <crtdbg.h>
#endif /* _MSC_VER) && _DEBUG */

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.scratch.fe.WindowsRegistry");

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
        pantheios::log_DEBUG("test.scratch.fe.WindowsRegistry: " __STLSOFT_COMPILER_LABEL_STRING);
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
        pantheios::logputs(pantheios::emergency, "Unhandled unknown error");

        res = EXIT_FAILURE;
    }

#if defined(_MSC_VER) && \
    defined(_DEBUG)
    _CrtMemDumpAllObjectsSince(&memState);
#endif /* _MSC_VER) && _DEBUG */

    return res;
}

/* ////////////////////////////////////////////////////////////////////// */
