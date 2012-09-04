/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/backends/example.cpp.backends.file.callback/example.cpp.backends.file.callback.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of pantheios_be_file_setFilePath()
 *                - use of pantheios::logputs() in bail-out conditions
 *
 * Created:     29th November 2006
 * Updated:     7th December 2010
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


#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS // Faster compilation

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>            // Pantheios C++ main header
#include <pantheios/inserters/args.hpp>       // for pantheios::args

#include <pantheios/backends/bec.file.h>      // be.file header

/* Standard C/C++ Header Files */
#include <exception>                          // for std::exception
#include <new>                                // for std::bad_alloc
#include <string>                             // for std::string
#include <stdlib.h>                           // for exit codes

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.backends.file.callback");

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

#ifdef PANTHEIOS_USE_WIDE_STRINGS
# define pan_strcpy_    wcscpy
#else
# define pan_strcpy_    strcpy
#endif

/* /////////////////////////////////////////////////////////////////////////
 * Application-defined functions
 */

/** Cause the file to be opened when the application is starting up.
 */
PANTHEIOS_CALL(void) pantheios_be_file_getAppInit(
  int                 /* backEndId */
, pan_be_file_init_t* init
) /* throw() */
{
  init->flags |= PANTHEIOS_BE_FILE_F_TRUNCATE;        // Truncate the contents
  init->flags |= PANTHEIOS_BE_FILE_F_DELETE_IF_EMPTY; // Delete the file if nothing written

  init->fileName = pan_strcpy_(init->buff, PSTR("callback-test-%D-%T.log"));
}

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char **argv)
{
  // Use of be.file involves several steps:
  //
  // 1. Linking to the back-end, either explicitly or implicitly
  // 2. Setting the log file path for the given back-end(s)
  // 3. Making log statements
  // 4. Changing the log file path for the given back-end(s)
  // 5. Closing the log file for the given back-end(s)

  // In this case, linking is performed either by the build makefile or the
  // IDE project file, to the be.file back-end.
  //
  // In this case, the log file path via the application-defined callback
  // function pantheios_be_file_getAppInit(), which is called during program
  // initialisation, prior to main() being called. At the same time, the
  // flags PANTHEIOS_BE_FILE_F_TRUNCATE and
  // PANTHEIOS_BE_FILE_F_DELETE_IF_EMPTY are specified
  //
  // In this case, the file is closed automatically during program
  // uninitialisation.

  try
  {
#ifndef PANTHEIOS_USE_WIDE_STRINGS
    pantheios::log_DEBUG("main(", pantheios::args(argc, argv), ")");
#else /* ? !PANTHEIOS_USE_WIDE_STRINGS */
    STLSOFT_SUPPRESS_UNUSED(argc); STLSOFT_SUPPRESS_UNUSED(argv);
#endif /* !PANTHEIOS_USE_WIDE_STRINGS */

    pantheios::log_NOTICE(PSTR("stmt 1"));


    pantheios::log_NOTICE(PSTR("stmt 2"));


    pantheios::log_NOTICE(PSTR("stmt 3"));


    pantheios::log_NOTICE(PSTR("stmt 4"));

    pantheios::log_DEBUG(PSTR("exiting main()"));

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
