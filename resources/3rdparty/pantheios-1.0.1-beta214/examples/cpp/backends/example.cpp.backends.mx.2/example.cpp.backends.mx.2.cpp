/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/backends/example.cpp.backends.mx.2/example.cpp.backends.mx.2.cpp
 *
 * Purpose:     Implementation file for the example.cpp.backends.mx.2 project.
 *
 * Created:     19th September 2008
 * Updated:     11th December 2010
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



#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS // Faster compilation

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/args.hpp>
#include <pantheios/frontends/fe.simple.h>
#include <pantheios/backends/be.N.h>
#include <platformstl/platformstl.h>
#if defined(PLATFORMSTL_OS_IS_UNIX)
# include <pantheios/backends/bec.fprintf.h>
# include <pantheios/backends/bec.syslog.h>
#elif defined(PLATFORMSTL_OS_IS_WINDOWS)
# include <pantheios/backends/bec.WindowsConsole.h>
# include <pantheios/backends/bec.WindowsSyslog.h>
#else /* ? OS */
# error Platform not discriminated
#endif /* OS */

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>

/* Standard C++ Header Files */
#include <exception>

/* Standard C Header Files */
#include <stdio.h>
#include <stdlib.h>

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

extern "C" const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[]    =   PANTHEIOS_LITERAL_STRING("example.cpp.backends.mx.2");

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* /////////////////////////////////////////////////////////////////////////
 * Logging management
 */

enum
{
    beid_Console =   1,
    beid_Syslog  =   2
};

pan_be_N_t  PAN_BE_N_BACKEND_LIST[] =
{
#if defined(PLATFORMSTL_OS_IS_UNIX)
    PANTHEIOS_BE_N_STDFORM_ENTRY(beid_Console, pantheios_be_fprintf, PANTHEIOS_BE_N_F_IGNORE_NONMATCHED_CUSTOM28_ID),
    PANTHEIOS_BE_N_STDFORM_ENTRY(beid_Syslog, pantheios_be_syslog, PANTHEIOS_BE_N_F_ID_MUST_MATCH_CUSTOM28),
#elif defined(PLATFORMSTL_OS_IS_WINDOWS)
    PANTHEIOS_BE_N_STDFORM_ENTRY(beid_Console, pantheios_be_WindowsConsole, PANTHEIOS_BE_N_F_IGNORE_NONMATCHED_CUSTOM28_ID),
    PANTHEIOS_BE_N_STDFORM_ENTRY(beid_Syslog, pantheios_be_WindowsSyslog, PANTHEIOS_BE_N_F_ID_MUST_MATCH_CUSTOM28),
#else /* ? OS */
# error Platform not discriminated
#endif /* OS */

    PANTHEIOS_BE_N_TERMINATOR_ENTRY
};

/* ////////////////////////////////////////////////////////////////////// */

static int main_(int argc, char** argv)
{
  // This goes only to debugger
#ifndef PANTHEIOS_USE_WIDE_STRINGS
  pantheios::log_NOTICE(PSTR("main("), pantheios::args(argc, argv), PSTR(")"));
#else /* ? !PANTHEIOS_USE_WIDE_STRINGS */
  STLSOFT_SUPPRESS_UNUSED(argc); STLSOFT_SUPPRESS_UNUSED(argv);
#endif /* !PANTHEIOS_USE_WIDE_STRINGS */


  // This goes to console and debugger
  pantheios::log(pantheios::notice(beid_Syslog), PSTR("Isn't targeted multiplexing great?!"));

  return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
  try
  {
    return main_(argc, argv);
  }
    catch(std::bad_alloc&)
    {
        pantheios::log(pantheios::alert, PSTR("out of memory"));
    }
    catch(std::exception& x)
    {
        pantheios::log_CRITICAL(PSTR("Exception: "), x);
    }
  catch(...)
  {
    pantheios::logputs(pantheios::emergency, PSTR("Unhandled unknown error"));
  }

  return EXIT_FAILURE;
}

/* ///////////////////////////// end of file //////////////////////////// */
