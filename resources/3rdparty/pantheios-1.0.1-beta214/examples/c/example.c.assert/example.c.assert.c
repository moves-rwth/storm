/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/c/example.c.assert/example.c.assert.c
 *
 * Purpose:     C example program for Pantheios. Demonstrates:
 *
 *                - how the Pantheios libraries must be explicitly
 *                  initialised in a C program; this is not the case in
 *                  C++ programs
 *                - use of PANTHEIOS_ASSERT()
 *
 * Created:     8th May 2009
 * Updated:     22nd March 2010
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
const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.c.assert");

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
  int i = pantheios_init();

  if(i >= 0)
  {
    PANTHEIOS_ASSERT(1);

    PANTHEIOS_ASSERT(0);


    PANTHEIOS_MESSAGE_ASSERT(1, "it was true");

    PANTHEIOS_MESSAGE_ASSERT(0, "it was false");

    pantheios_uninit();
  }

  return EXIT_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////// */
