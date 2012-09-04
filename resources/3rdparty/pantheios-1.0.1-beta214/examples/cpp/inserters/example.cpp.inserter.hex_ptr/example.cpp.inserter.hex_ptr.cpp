/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/inserters/example.cpp.inserter.hex_ptr/example.cpp.inserter.hex_ptr.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of Pantheios::hex_ptr inserter class for pointer
 *                  types
 *                - use of pantheios::logputs() in bail-out conditions
 *
 * Created:     11th November 2008
 * Updated:     6th August 2012
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
#include <pantheios/inserters/hex_ptr.hpp>    // for pantheios::hex_ptr

/* Standard C/C++ Header Files */
#include <exception>                          // for std::exception
#include <new>                                // for std::bad_alloc
#include <string>                             // for std::string
#include <stdlib.h>                           // for exit codes

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.inserter.hex_ptr");

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
  try
  {
    void* pv = &pv;

    // Log a hex_ptr of precise length with 0x prefix, a length
    // of 8/16 (depending on architecture), and zero-padded;
    // Output: "pv: [0x0012fed0]"

    pantheios::log_NOTICE(PSTR("pv: ["), pantheios::hex_ptr(pv), PSTR("]"));

    // Log a hex_ptr of precise length with 0x prefix;
    // Output: "pv: [0x12fed0]"

    pantheios::log_NOTICE(PSTR("pv: ["), pantheios::hex_ptr(pv, 0, pantheios::fmt::zeroXPrefix), PSTR("]"));

    // Log a hex_ptr into 8 spaces, right justified, with 0x prefix;
    // Output: "pv: [  0x12fed0]"

    pantheios::log_NOTICE(PSTR("pv: ["), pantheios::hex_ptr(pv, 8, pantheios::fmt::zeroXPrefix), PSTR("]"));

    // Log a hex_ptr into 8 spaces, left justified, with 0x prefix;
    // Output: "pv: [0x12fed0  ]"

    pantheios::log_NOTICE(PSTR("pv: ["), pantheios::hex_ptr(pv, -8, pantheios::fmt::zeroXPrefix), PSTR("]"));

    // Log a hex_ptr of precise length with 0x prefix, a length of 16,
    // and zero-padded; Output: "pv: [0x000000000012fed0]"

    pantheios::log_NOTICE(PSTR("pv: ["), pantheios::hex_ptr(pv, 16, pantheios::fmt::zeroPad | pantheios::fmt::zeroXPrefix), PSTR("]"));

    // Log a hex_ptr of precise length with 0x prefix, a length
    // appropriate to the architecture, and zero-padded;
    // Output: "pv: [0x0012fed0]"

    pantheios::log_NOTICE(PSTR("pv: ["), pantheios::hex_ptr(pv, pantheios::hex_ptr::pointerHexWidth, pantheios::fmt::fullHex), PSTR("]"));

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
