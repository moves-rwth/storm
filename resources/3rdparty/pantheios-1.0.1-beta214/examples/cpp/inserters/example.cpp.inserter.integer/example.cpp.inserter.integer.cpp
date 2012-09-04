/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/inserters/example.cpp.inserter.integer/example.cpp.inserter.integer.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of Pantheios inserters for integral types
 *                - use of pantheios::logputs() in bail-out conditions
 *
 * Created:     22nd June 2006
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
#include <pantheios/inserters/integer.hpp>    // for pantheios::integer

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>                // for sized integer types

/* Standard C/C++ Header Files */
#include <exception>                        // for std::exception
#include <new>                              // for std::bad_alloc
#include <string>                           // for std::string
#include <stdlib.h>                         // for exit codes
#include <limits.h>                         // SHRT_MIN, etc.

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.inserter.integer");

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
    short             shortMin  = SHRT_MIN;
    short             shortMax  = SHRT_MAX;
    unsigned short    ushortMax = USHRT_MAX;
    int               intMin    = INT_MIN;
    int               intMax    = INT_MAX;
    unsigned int      uintMax   = UINT_MAX;
    long              longMin   = LONG_MIN;
    long              longMax   = LONG_MAX;
    unsigned long     ulongMax  = ULONG_MAX;
    stlsoft::sint32_t si32Val   = -123;
    stlsoft::uint32_t ui32Val   = 456;

    // Log a short in decimal; Output: "SHRT_MIN: [-32768]"

    pantheios::log_NOTICE(PSTR("SHRT_MIN: ["), pantheios::integer(shortMin), PSTR("]"));

    // Log a short in decimal; Output: "SHRT_MAX: [32767]"

    pantheios::log_NOTICE(PSTR("SHRT_MAX: ["), pantheios::integer(shortMax), PSTR("]"));


#ifdef PANTHEIOS_OBSOLETE // 2-parameter constructors are deprecated, and will be removed

    // Log a unsigned short as hexadecimal; Output: "USHRT_MAX: [ffff]"

    pantheios::log_NOTICE(PSTR("USHRT_MAX: ["), pantheios::integer(ushortMax, pantheios::fmt::hex), PSTR("]"));

    // Log an int, into a width of 20; Output: "INT_MIN: [-2147483648         ]"

    pantheios::log_NOTICE(PSTR("INT_MIN: ["), pantheios::integer(intMin, -20), PSTR("]"));

    // Log an int, into a width of 20; Output: "INT_MAX: [2147483647          ]"

    pantheios::log_NOTICE(PSTR("INT_MAX: ["), pantheios::integer(intMax, -20), PSTR("]"));

    // Log an unsigned int as hexadecimal with 0x prefix; Output: "UINT_MAX: [0xffffffff]"

    pantheios::log_NOTICE(PSTR("UINT_MAX: ["), pantheios::integer(uintMax, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix), PSTR("]"));

    // Log a long; Output: "LONG_MIN: [         -2147483648]"

    pantheios::log_NOTICE(PSTR("LONG_MIN: ["), pantheios::integer(longMin, 20), PSTR("]"));

    // Log a long; Output: "LONG_MAX: [          2147483647]"

    pantheios::log_NOTICE(PSTR("LONG_MAX: ["), pantheios::integer(longMax, 20), PSTR("]"));

#endif /* PANTHEIOS_OBSOLETE */


    // Log a unsigned short as hexadecimal; Output: "USHRT_MAX: [ffff]"

    pantheios::log_NOTICE(PSTR("USHRT_MAX: ["), pantheios::integer(ushortMax, 0, pantheios::fmt::hex), PSTR("]"));

    // Log an int, into a width of -20; Output: "INT_MIN: [-2147483648         ]"

    pantheios::log_NOTICE(PSTR("INT_MIN: ["), pantheios::integer(intMin, -20, 0), PSTR("]"));

    // Log an int, into a width of +20; Output: "INT_MIN: [         -2147483648]"

    pantheios::log_NOTICE(PSTR("INT_MIN: ["), pantheios::integer(intMin, +20, 0), PSTR("]"));

    // Log an int, into a width of -20; Output: "INT_MAX: [2147483647          ]"

    pantheios::log_NOTICE(PSTR("INT_MAX: ["), pantheios::integer(intMax, -20, 0), PSTR("]"));

    // Log an int, into a width of +20; Output: "INT_MAX: [          2147483647]"

    pantheios::log_NOTICE(PSTR("INT_MAX: ["), pantheios::integer(intMax, +20, 0), PSTR("]"));

    // Log an unsigned int as hexadecimal with 0x prefix; Output: "UINT_MAX: [0xffffffff]"

    pantheios::log_NOTICE(PSTR("UINT_MAX: ["), pantheios::integer(uintMax, 0, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix), PSTR("]"));

    // Log an unsigned int as hexadecimal with 0x prefix, into a width of -20; Output: "UINT_MAX: [0xffffffff          ]"

    pantheios::log_NOTICE(PSTR("UINT_MAX: ["), pantheios::integer(uintMax, -20, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix), PSTR("]"));

    // Log an unsigned int as hexadecimal with 0x prefix, into a width of +20; Output: "UINT_MAX: [          0xffffffff]"

    pantheios::log_NOTICE(PSTR("UINT_MAX: ["), pantheios::integer(uintMax, +20, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix), PSTR("]"));

    // Log an unsigned int as hexadecimal with 0x prefix, into a width of -10; Output: "UINT_MAX: [0xffffffff]"

    pantheios::log_NOTICE(PSTR("UINT_MAX: ["), pantheios::integer(uintMax, -10, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix), PSTR("]"));

    // Log an unsigned int as hexadecimal with 0x prefix, into a width of +10; Output: "UINT_MAX: [0xffffffff]"

    pantheios::log_NOTICE(PSTR("UINT_MAX: ["), pantheios::integer(uintMax, +10, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix), PSTR("]"));

    // Log an unsigned int as hexadecimal with 0x prefix, into a width of -5; Output: "UINT_MAX: [0xffffffff]"

    pantheios::log_NOTICE(PSTR("UINT_MAX: ["), pantheios::integer(uintMax, -5, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix), PSTR("]"));

    // Log an unsigned int as hexadecimal with 0x prefix, into a width of +5; Output: "UINT_MAX: [0xffffffff]"

    pantheios::log_NOTICE(PSTR("UINT_MAX: ["), pantheios::integer(uintMax, +5, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix), PSTR("]"));

    // Log a long, into a width of +20; Output: "LONG_MIN: [         -2147483648]"

    pantheios::log_NOTICE(PSTR("LONG_MIN: ["), pantheios::integer(longMin, 20, 0), PSTR("]"));

    // Log a long, into a width of +20; Output: "LONG_MAX: [          2147483647]"

    pantheios::log_NOTICE(PSTR("LONG_MAX: ["), pantheios::integer(longMax, 20, 0), PSTR("]"));

    // Log a long, into a width of +10; Output: "LONG_MIN: [-2147483648]"

    pantheios::log_NOTICE(PSTR("LONG_MIN: ["), pantheios::integer(longMin, 10, 0), PSTR("]"));

    // Log a long, into a width of +10; Output: "LONG_MAX: [2147483647]"

    pantheios::log_NOTICE(PSTR("LONG_MAX: ["), pantheios::integer(longMax, 10, 0), PSTR("]"));

    // Log a long, into a width of +5; Output: "LONG_MIN: [-2147483648]"

    pantheios::log_NOTICE(PSTR("LONG_MIN: ["), pantheios::integer(longMin, 5, 0), PSTR("]"));

    // Log a long, into a width of +5; Output: "LONG_MAX: [2147483647]"

    pantheios::log_NOTICE(PSTR("LONG_MAX: ["), pantheios::integer(longMax, 5, 0), PSTR("]"));


    // Log an unsigned long; Output: "ULONG_MAX: [4294967295]"

    pantheios::log_NOTICE(PSTR("ULONG_MAX: ["), pantheios::integer(ulongMax), PSTR("]"));

    // Log a signed 32-bit integer; Output: "sint32_t: [-123]"

    pantheios::log_NOTICE(PSTR("sint32_t: ["), pantheios::integer(si32Val), PSTR("]"));

    // Log an unsigned 32-bit integer; Output: "uint32_t: [     456]"

    pantheios::log_NOTICE(PSTR("uint32_t: ["), pantheios::integer(ui32Val, 8, 0), PSTR("]"));


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
