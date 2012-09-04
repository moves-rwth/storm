/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/linking/example.cpp.linking.implicit_link_1/example.cpp.linking.implicit_link_1.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of implicit linking to bind in front-end and back-end
 *
 * Created:     31st August 2006
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
#include <pantheios/pantheios.hpp>              // Pantheios C++ main header
#include <pantheios/implicit_link/core.h>       // Implicitly link the core
#include <pantheios/implicit_link/fe.simple.h>  // Implicitly link the stock front-end fe.simple
#include <pantheios/implicit_link/be.fprintf.h> // Implicitly link the stock back-end be.fprintf

/* Standard C/C++ Header Files */
#include <string>                               // for std::string
#include <stdlib.h>                             // for exit codes

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.linking.implicit_link_1");

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
  pantheios::log_INFORMATIONAL(PANTHEIOS_LITERAL_STRING("Hello!"));

  return EXIT_SUCCESS;
}

/* ///////////////////////////// end of file //////////////////////////// */
