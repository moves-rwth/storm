/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/format/example.cpp.format.pad/example.cpp.format.pad.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of Pantheios inserters for padding
 *                - use of pantheios::logputs() in bail-out conditions
 *
 * Created:     29th June 2009
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


#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS // Faster compilation

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>              // Pantheios C++ main header
#include <pantheios/inserters/pad.hpp>          // for pantheios::pad
#include <pantheios/internal/string_encoding.h> // for PANTHEIOS_LITERAL_STRING

/* Standard C/C++ Header Files */
#include <exception>                            // for std::exception
#include <new>                                  // for std::bad_alloc
#include <string>                               // for std::string
#include <stdio.h>                              // for printf()
#include <stdlib.h>                             // for exit codes

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.format.pad");

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
  try
  {
    std::basic_string<pantheios::pan_char_t>  name = PANTHEIOS_LITERAL_STRING("John Smith");

    printf("Name: %40s.\n", name.c_str()); 

    pantheios::log_NOTICE(PSTR("Name: "), PANTHEIOS_LPAD(name, 40), PSTR("."));

    printf("Name: %-40s.\n", name.c_str()); 

    pantheios::log_NOTICE(PSTR("Name: "), PANTHEIOS_RPAD(name, 40), PSTR("."));

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
