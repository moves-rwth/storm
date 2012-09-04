/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/backends/example.cpp.backends.callback/example.cpp.backends.callback.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of a back-end library that uses callbacks.
 *                - use of pantheios::logputs() in bail-out conditions
 *
 * Created:     25th August 2006
 * Updated:     11th December 2010
 *
 * www:         http://www.pantheios.org/
 *
 * License:     This source code is placed into the public domain 2006
 *              by Synesis Software Pty Ltd. There are no restrictions
 *              whatsoever to your use of the software.
 *
 *              This software is provided "as is", and any warranties,
 *              express or implied, of any kind and for any purpose, are
 *              disclaimed.
 *
 * ////////////////////////////////////////////////////////////////////// */


#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS // Faster compilation

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>                  // Pantheios C++ main header
#include <platformstl/platformstl.h>
#if defined(PLATFORMSTL_OS_IS_UNIX)
# include <pantheios/backends/bec.fprintf.h>        // Include the API for bec.fprintf
#elif defined(PLATFORMSTL_OS_IS_WINDOWS)
# include <pantheios/backends/bec.WindowsConsole.h> // Include the API for bec.WindowsConsole
#else /* ? OS */
# error Platform not discriminated
#endif /* OS */

/* Standard C/C++ Header Files */
#include <exception>                                // for std::exception
#include <new>                                      // for std::bad_alloc
#include <string>                                   // for std::string
#include <stdlib.h>                                 // for exit codes

#if defined(PLATFORMSTL_OS_IS_WINDOWS)
/* Windows Header Files */
# include <windows.h>                               // for console colour constants
#endif /* PLATFORMSTL_OS_IS_WINDOWS */

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.backends.callback");

/* ////////////////////////////////////////////////////////////////////// */

#if defined(PLATFORMSTL_OS_IS_UNIX)

PANTHEIOS_CALL(void) pantheios_be_fprintf_getAppInit(
  int                     /* backEndId */
, pan_be_fprintf_init_t*  init) /* throw() */
{
  init->flags |= PANTHEIOS_BE_INIT_F_NO_DATETIME; // Don't show date/time
}

#elif defined(PLATFORMSTL_OS_IS_WINDOWS)

PANTHEIOS_CALL(void) pantheios_be_WindowsConsole_getAppInit(
  int                           /* backEndId */
, pan_be_WindowsConsole_init_t* init
) /* throw() */
{
  init->flags |= PANTHEIOS_BE_INIT_F_NO_DATETIME; // Don't show date/time

  init->colours[pantheios::debug]   = FOREGROUND_BLUE | FOREGROUND_INTENSITY;              // Lose the white background
  init->colours[pantheios::notice]  = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED; // Lose the intensity
}

#else /* ? OS */
# error Platform not discriminated
#endif /* OS */

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
  try
  {
    pantheios::log_DEBUG(PSTR("debug"));
    pantheios::log_INFORMATIONAL(PSTR("informational"));
    pantheios::log_NOTICE(PSTR("notice"));
    pantheios::log_WARNING(PSTR("warning"));
    pantheios::log_ERROR(PSTR("error"));
    pantheios::log_CRITICAL(PSTR("critical"));
    pantheios::log_ALERT(PSTR("alert"));
    pantheios::log_EMERGENCY(PSTR("emergency"));

    return EXIT_SUCCESS;
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
    pantheios::logputs(pantheios::emergency, PSTR("Unexpected unknown error"));
  }

  return EXIT_FAILURE;
}

/* ///////////////////////////// end of file //////////////////////////// */
