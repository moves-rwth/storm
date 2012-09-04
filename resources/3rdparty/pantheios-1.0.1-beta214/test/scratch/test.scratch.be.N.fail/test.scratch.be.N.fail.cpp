/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.be.N.fail/test.scratch.be.N.fail.cpp
 *
 * Purpose:     Implementation file for the be.N.fail.test project.
 *
 * Created:     9th January 2007
 * Updated:     23rd March 2010
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2007-2010, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS

/* This inclusion required for suppressing warnings during NoX (No eXception-support) configurations. */
#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/exception.hpp>

#include <pantheios/frontends/fe.N.h>
#include <pantheios/backends/be.N.h>
#include <pantheios/backends/bec.fail.h>
#include <pantheios/backends/bec.file.h>
#include <pantheios/backends/bec.fprintf.h>

/* Standard C++ Header Files */
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
# include <exception>
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

/* Standard C Header Files */
#include <stdlib.h>

#if defined(_MSC_VER) && \
    defined(_DEBUG)
# include <crtdbg.h>
#endif /* _MSC_VER) && _DEBUG */

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

extern "C"
{

const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[]      =   PANTHEIOS_LITERAL_STRING("be.N.fail.test");
pan_fe_N_t PAN_FE_N_SEVERITY_CEILINGS[]   =
{
    {   0,  PANTHEIOS_SEV_DEBUG  }
};
pan_be_N_t      PAN_BE_N_BACKEND_LIST[]         =
{
    PANTHEIOS_BE_N_STDFORM_ENTRY(1, pantheios_be_fail, PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE),
    PANTHEIOS_BE_N_STDFORM_ENTRY(2, pantheios_be_file, 0),
    PANTHEIOS_BE_N_STDFORM_ENTRY(3, pantheios_be_fail, PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE),
    PANTHEIOS_BE_N_STDFORM_ENTRY(4, pantheios_be_fprintf, 0),
    PANTHEIOS_BE_N_TERMINATOR_ENTRY
};

} // extern "C"

/* ////////////////////////////////////////////////////////////////////// */

static int main_(int /* argc */, char ** /*argv*/)
{
    pantheios::log_NOTICE(PSTR("a notice"));

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

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        res = main_(argc, argv);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::exception &x)
    {
                pantheios::log_ALERT(PSTR("Unexpected general error: "), pantheios::exception(x), PSTR(". Application terminating"));

        res = EXIT_FAILURE;
    }
    catch(...)
    {
        pantheios::logputs(pantheios::emergency, PSTR("Unhandled unknown error"));

        res = EXIT_FAILURE;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

#if defined(_MSC_VER) && \
    defined(_DEBUG)
    _CrtMemDumpAllObjectsSince(&memState);
#endif /* _MSC_VER) && _DEBUG */

    return res;
}

/* ////////////////////////////////////////////////////////////////////// */
