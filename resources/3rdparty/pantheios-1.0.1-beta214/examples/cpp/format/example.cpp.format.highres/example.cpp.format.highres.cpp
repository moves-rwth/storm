/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/format/example.cpp.format.highres/example.cpp.format.highres.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - high-resolution date/time fields in statements
 *
 * Created:     12th November 2007
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
#include <pantheios/pantheios.hpp>            // Pantheios C++ main header
#include <pantheios/inserters/integer.hpp>    // for pantheios::integer
#include <pantheios/backends/bec.fprintf.h>   // for be.fprintf

/* Standard C/C++ Header Files */
#include <exception>                          // for std::exception
#include <new>                                // for std::bad_alloc
#include <string>                             // for std::string
#include <stdlib.h>                           // for exit codes

/* ////////////////////////////////////////////////////////////////////// */

PANTHEIOS_CALL(void) pantheios_be_fprintf_getAppInit(int /* backEndId */, pan_be_fprintf_init_t* init) /* throw() */
{
    /* init->flags |= PANTHEIOS_BE_INIT_F_USE_SYSTEM_TIME; */
    init->flags |= PANTHEIOS_BE_INIT_F_USE_UNIX_FORMAT;
    init->flags |= PANTHEIOS_BE_INIT_F_HIDE_DATE;
    init->flags |= PANTHEIOS_BE_INIT_F_HIGH_RESOLUTION;
}

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.format.highres");

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
  try
  {
    { for(size_t i = 0; i != 10; ++i)
    {
      { for(size_t j = 0; j != 0x7ffffff; ++j) {} }

      pantheios::log_NOTICE(PSTR("log ("), pantheios::integer(i), PSTR(")"));
    }}

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
