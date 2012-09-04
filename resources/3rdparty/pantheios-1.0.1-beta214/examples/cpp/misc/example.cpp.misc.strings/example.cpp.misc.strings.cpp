/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/misc/example.cpp.misc.strings/example.cpp.misc.strings.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of Pantheios logging statements for string types
 *                - use of pantheios::logputs() in bail-out conditions
 *
 * Created:     17th May 2006
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
#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS // Faster compilation
#include <pantheios/pantheios.hpp>          // Pantheios C++ main header

/* Standard C/C++ Header Files */
#include <exception>                        // for std::exception
#include <new>                              // for std::bad_alloc
#include <string>                           // for std::string
#include <stdlib.h>                         // for exit codes

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.misc.strings");

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* ////////////////////////////////////////////////////////////////////// */

typedef std::basic_string<PAN_CHAR_T>   string_t;

/* ////////////////////////////////////////////////////////////////////// */

static string_t concat(string_t const& s1, string_t const& s2)
{
    return s1 + s2;
}

int main()
{
  try
  {
    PAN_CHAR_T        s1[]  = PSTR("abc");
    PAN_CHAR_T const* s2    = PSTR("def");

    // Log two C-style strings

    pantheios::log_NOTICE(PSTR("Concatenating '"), s1, PSTR("' and '"), s2, PSTR("'"));

    string_t result = concat(s1, s2);

    // Log two C-style strings and a std::string

    pantheios::log_NOTICE(PSTR("Concatenation of '"), s1, PSTR("' and '"), s2, PSTR("' succeeded; result="), result);


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
