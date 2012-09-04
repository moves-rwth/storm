/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/inserters/example.cpp.inserter.xi/example.cpp.inserter.xi.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of Pantheios inserters for hex integral types
 *                - use of pantheios::logputs() in bail-out conditions
 *                - use of shorthand pan for namespace qualification
 *                - use of shorthand xi for hex integer inserter
 *
 * Created:     30th October 2010
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
#include <pantheios/pan.hpp>                  // Pantheios C++ main header
#include <pantheios/inserters/xi.hpp>         // for pan::xi

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>                // for sized integer types
#include <stlsoft/util/limit_traits.h>

/* Standard C/C++ Header Files */
#include <exception>                        // for std::exception
#include <new>                              // for std::bad_alloc
#include <string>                           // for std::string
#include <stdlib.h>                         // for exit codes
#include <limits.h>                         // SHRT_MIN, etc.

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.inserter.xi");

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* We define PANTHEIOS_OBSOLETE here, while still in beta, but it will be
 * defined properly, and removed from here, in release candidate phase.
 */
#ifndef PANTHEIOS_OBSOLETE
# define PANTHEIOS_OBSOLETE
#endif /* !PANTHEIOS_OBSOLETE */

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
  try
  {
    unsigned char     ucharMax  = UCHAR_MAX;
    unsigned short    ushortMax = USHRT_MAX;
    unsigned int      uintMax   = UINT_MAX;
    unsigned long     ulongMax  = ULONG_MAX;
    stlsoft::uint64_t uint64Max = STLSOFT_LIMIT_TRAITS__UINT64_MAX;

    // Log a unsigned short as full hexadecimal; Output: "unsigned char max: [ffff]"

    pantheios::log_NOTICE(PSTR("unsigned char max:  ["), pan::xi(ucharMax), PSTR("]"));

    // Log a unsigned short as full hexadecimal; Output: "unsigned short max: [ffff]"

    pantheios::log_NOTICE(PSTR("unsigned short max: ["), pan::xi(ushortMax), PSTR("]"));

    // Log an unsigned int as full hexadecimal; Output: "unsigned int max: [0xffffffff]"

    pantheios::log_NOTICE(PSTR("unsigned int max:   ["), pan::xi(uintMax), PSTR("]"));

    // Log a unsigned short as full hexadecimal; Output: "unsigned long max: [ffff]"

    pantheios::log_NOTICE(PSTR("unsigned long max:  ["), pan::xi(ulongMax), PSTR("]"));

    // Log an unsigned int as full hexadecimal; Output: "unsigned int64 max: [0xffffffff]"

    pantheios::log_NOTICE(PSTR("unsigned int64 max: ["), pan::xi(uint64Max), PSTR("]"));



    // Log an unsigned int as full hexadecimal, into a width of -5; Output: "unsigned int max: [0xffffffff]"

    pantheios::log_NOTICE(PSTR("unsigned int max: ["), pan::xi(uintMax, -5), PSTR("]"));

    // Log an unsigned int as full hexadecimal, into a width of +5; Output: "unsigned int max: [0xffffffff]"

    pantheios::log_NOTICE(PSTR("unsigned int max: ["), pan::xi(uintMax, +5), PSTR("]"));

    // Log an unsigned int as full hexadecimal, into a width of -10; Output: "unsigned int max: [0xffffffff]"

    pantheios::log_NOTICE(PSTR("unsigned int max: ["), pan::xi(uintMax, -10), PSTR("]"));

    // Log an unsigned int as full hexadecimal, into a width of +10; Output: "unsigned int max: [0xffffffff]"

    pantheios::log_NOTICE(PSTR("unsigned int max: ["), pan::xi(uintMax, +10), PSTR("]"));

    // Log an unsigned int as full hexadecimal, into a width of -15; Output: "unsigned int max: [0xffffffff     ]"

    pantheios::log_NOTICE(PSTR("unsigned int max: ["), pan::xi(uintMax, -15), PSTR("]"));

    // Log an unsigned int as full hexadecimal, into a width of +15; Output: "unsigned int max: [0x00000ffffffff]"

    pantheios::log_NOTICE(PSTR("unsigned int max: ["), pan::xi(uintMax, +15), PSTR("]"));

    // Log an unsigned int as full hexadecimal, into a width of -20; Output: "unsigned int max: [0xffffffff          ]"

    pantheios::log_NOTICE(PSTR("unsigned int max: ["), pan::xi(uintMax, -20), PSTR("]"));

    // Log an unsigned int as full hexadecimal, into a width of +20; Output: "unsigned int max: [0x0000000000ffffffff]"

    pantheios::log_NOTICE(PSTR("unsigned int max: ["), pan::xi(uintMax, +20), PSTR("]"));


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
