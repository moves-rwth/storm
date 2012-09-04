/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/inserters/example.cpp.inserter.b64/example.cpp.inserter.b64.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of Pantheios b64 inserter for blob types
 *                - use of pantheios::logputs() in bail-out conditions
 *
 * Created:     25th August 2006
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
#include <pantheios/inserters/b64.hpp>        // for pantheios::b64

/* Standard C/C++ Header Files */
#include <exception>                          // for std::exception
#include <new>                                // for std::bad_alloc
#include <string>                             // for std::string
#include <stdlib.h>                           // for exit codes

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.inserter.b64");

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
  try
  {
    // Make a blob with some arbitrary values

    pantheios::uint8_t  bytes[20];

    { for(size_t i = 0; i < STLSOFT_NUM_ELEMENTS(bytes); ++i)
    {
      //bytes[i] = static_cast<pantheios::uint8_t>((i << 8) | (i & 0x0f));
      bytes[i] = static_cast<pantheios::uint8_t>(i);
    }}

    // Log the blob with default formatting; Output:
    //
    // "bytes: [AAECAwQFBgcICQoLDA0ODxAREhM=]"

    pantheios::log_NOTICE(PSTR("bytes: ["), pantheios::b64(bytes, sizeof(bytes)), PSTR("]"));


    // Log the blob with a maximum line length of 16; Output:
    // "bytes: [AAECAwQFBgcICQoL
    // DA0ODxAREhM=]"

    pantheios::log_NOTICE(PSTR("bytes: ["), pantheios::b64(bytes, sizeof(bytes), ::b64::B64_F_LINE_LEN_USE_PARAM, 16), PSTR("]"));


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
