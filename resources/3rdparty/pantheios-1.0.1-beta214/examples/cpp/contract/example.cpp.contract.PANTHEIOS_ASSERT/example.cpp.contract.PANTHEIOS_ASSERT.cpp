/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/contract/example.cpp.contract.PANTHEIOS_ASSERT/example.cpp.contract.PANTHEIOS_ASSERT.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of Pantheios inserter for thread ids
 *                - use of pantheios::logputs() in bail-out conditions
 *
 * Created:     8th May 2009
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
#include <pantheios/assert.h>

/* Standard C Header Files */
#include <stdlib.h>

/* /////////////////////////////////////////////////////////////////////////
 * Compiler compatibility
 */

#ifdef STLSOFT_COMPILER_IS_BORLAND
# pragma warn -8008
# pragma warn -8066
#endif

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.contract.PANTHEIOS_ASSERT");

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
  int i = pantheios::init();

  if(i >= 0)
  {
    PANTHEIOS_ASSERT(true);

    PANTHEIOS_ASSERT(false);

    pantheios::uninit();
  }

  return EXIT_SUCCESS;
}

/* ///////////////////////////// end of file //////////////////////////// */
