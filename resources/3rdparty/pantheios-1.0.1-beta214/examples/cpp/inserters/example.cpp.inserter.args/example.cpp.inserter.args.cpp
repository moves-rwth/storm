/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/inserters/example.cpp.inserter.args/example.cpp.inserter.args.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of Pantheios inserter for command-line arguments
 *                - use of pantheios::logputs() in bail-out conditions
 *
 * Created:     21st October 2006
 * Updated:     6th December 2010
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


/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>              // Pantheios C++ main header
#include <pantheios/inserters/args.hpp>         // for pantheios::args

/* Standard C/C++ Header Files */
#include <exception>                            // for std::exception
#include <new>                                  // for std::bad_alloc
#include <string>                               // for std::string
#include <stdlib.h>                             // for exit codes

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# if defined(STLSOFT_COMPILER_IS_MSVC)
#  pragma warning(disable : 4702)
# endif /* compiler */
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.inserter.args");

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char** argv)
{
  try
  {
    // Log the command-line arguments; Output like: "example.cpp.inserter.args, abc, def"

#ifndef PANTHEIOS_USE_WIDE_STRINGS
    pantheios::log_NOTICE(PSTR("args: ["), pantheios::args(argc, argv), PSTR("]"));
#else /* ? !PANTHEIOS_USE_WIDE_STRINGS */
    STLSOFT_SUPPRESS_UNUSED(argc); STLSOFT_SUPPRESS_UNUSED(argv);
#endif /* !PANTHEIOS_USE_WIDE_STRINGS */


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
