/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/inserters/example.cpp.inserter.xp/example.cpp.inserter.xp.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of Pantheios inserters for pointer types
 *                - use of pantheios::logputs() in bail-out conditions
 *
 * Created:     9th November 2010
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
#include <pantheios/pantheios.hpp>            // Pantheios C++ main header
#include <pantheios/inserters/xp.hpp>         // for pantheios::xp
#include <pantheios/inserters/hex_ptr.hpp>    // for pantheios::hex_ptr

/* Standard C/C++ Header Files */
#include <exception>                        // for std::exception
#include <new>                              // for std::bad_alloc
#include <string>                           // for std::string
#include <stdlib.h>                         // for exit codes

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.inserter.xp");

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
  try
  {
    void* pv = &pv;

    // Log a pointer with full hex formatting; Output: "pv: [0x0012fed0]"

    pantheios::log_NOTICE(PSTR("pv: ["), pantheios::hex_ptr(pv), PSTR("]"));

    // Log a pointer with full hex formatting; Output: "pv: [0x0012fed0]"

    pantheios::log_NOTICE(PSTR("pv: ["), pantheios::xp(pv), PSTR("]"));


    // Log a pointer with full hex formatting, into a width of 20; Output: "pv: [0x0012fed0]"

    pantheios::log_NOTICE(PSTR("pv: ["), pantheios::hex_ptr(pv, 20, pantheios::fmt::fullHex), PSTR("]"));

    // Log a pointer with full hex formatting, into a width of 20; Output: "pv: [0x0012fed0]"

    pantheios::log_NOTICE(PSTR("pv: ["), pantheios::xp(pv, 20), PSTR("]"));


    // Log a pointer with full hex formatting, into a width of -20; Output: "pv: [0x0012fed0]"

    pantheios::log_NOTICE(PSTR("pv: ["), pantheios::hex_ptr(pv, -20, pantheios::fmt::fullHex), PSTR("]"));

    // Log a pointer with full hex formatting, into a width of -20; Output: "pv: [0x0012fed0]"

    pantheios::log_NOTICE(PSTR("pv: ["), pantheios::xp(pv, -20), PSTR("]"));


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
