/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/inserters/example.cpp.inserter.blob/example.cpp.inserter.blob.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of Pantheios inserters for blob types
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
#include <pantheios/inserters/blob.hpp>       // for pantheios::blob

/* Standard C/C++ Header Files */
#include <exception>                        // for std::exception
#include <new>                              // for std::bad_alloc
#include <string>                           // for std::string
#include <stdlib.h>                         // for exit codes

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.inserter.blob");

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

    // Log the blob with default formatting; Output: "bytes: [03020100070605040b0a09080f0e0d0c13121110]"

    pantheios::log_NOTICE(PSTR("bytes: ["), pantheios::blob(bytes, sizeof(bytes)), PSTR("]"));


    // Log the blob, splitting into groups of 1 (8-bits) without a separator; Output: "bytes: [000102030405060708090a0b0c0d0e0f10111213]"

    pantheios::log_NOTICE(PSTR("bytes: ["), pantheios::blob(bytes, sizeof(bytes), 1, NULL), PSTR("]"));

    // Log the blob, splitting into groups of 1 (8-bits) separated by a space; Output: "bytes: [00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13]"

    pantheios::log_NOTICE(PSTR("bytes: ["), pantheios::blob(bytes, sizeof(bytes), 1, PSTR(" ")), PSTR("]"));

    // Log the blob, splitting into groups of 2 (16-bits), separated by a space; Output: "bytes: [0100 0302 0504 0706 0908 0b0a 0d0c 0f0e 1110 1312]"

    pantheios::log_NOTICE(PSTR("bytes: ["), pantheios::blob(bytes, sizeof(bytes), 2, PSTR(" ")), PSTR("]"));

    // Log the blob, splitting into groups of 4 (32-bits), separated by a space; Output: "bytes: [03020100 07060504 0b0a0908 0f0e0d0c 13121110]"

    pantheios::log_NOTICE(PSTR("bytes: ["), pantheios::blob(bytes, sizeof(bytes), 4, PSTR(" ")), PSTR("]"));

    // Log the blob, splitting into groups of 8 (64-bits), separated by a space; Output: "bytes: [0706050403020100 0f0e0d0c0b0a0908 13121110]"

    pantheios::log_NOTICE(PSTR("bytes: ["), pantheios::blob(bytes, sizeof(bytes), 8, PSTR(" ")), PSTR("]"));


    // Log the blob, splitting into groups of 1 (8-bits) with a space separator,
    // with 4 groups per line, where each line is separated by a line-feed
    // and two spaces; Output:
    // "bytes: [00 01 02 03
    //   04 05 06 07
    //   08 09 0a 0b
    //   0c 0d 0e 0f
    //   10 11 12 13]"

    pantheios::log_NOTICE(PSTR("bytes: ["), pantheios::blob(bytes, sizeof(bytes), 1, PSTR(" "), 4, PSTR("\n  ")), PSTR("]"));

    // Log the blob, splitting into groups of 1 (8-bits) without a separator,
    // with 4 groups per line, where each line is separated by a line-feed
    // and two spaces; Output:
    // "bytes: [03020100-07060504-0b0a0908-0f0e0d0c-13121110]"

    pantheios::log_NOTICE(PSTR("bytes: ["), pantheios::blob(bytes, sizeof(bytes), 4, NULL, 1, PSTR("-")), PSTR("]"));


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
