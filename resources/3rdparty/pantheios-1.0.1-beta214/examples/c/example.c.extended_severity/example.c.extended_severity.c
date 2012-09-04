/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/c/example.c.extended_severity/example.c.extended_severity.c
 *
 * Purpose:     C example program for Pantheios. Demonstrates:
 *
 *                - how the Pantheios libraries must be explicitly
 *                  initialised in a C program; this is not the case in
 *                  C++ programs
 *                - use of extended severity information, via
 *                  PANTHEIOS_MAKE_EXTENDED_SEVERITY()
 *
 * Created:     21st May 2009
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
#include <pantheios/pantheios.h>

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
const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.c.extended_severity");

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
  int i = pantheios_init();

  if(i >= 0)
  {
    /* 1. Using the Pantheios API macro PANTHEIOS_MAKE_EXTENDED_SEVERITY() */
    pantheios_logprintf(PANTHEIOS_MAKE_EXTENDED_SEVERITY(PANTHEIOS_SEV_NOTICE, 10), PANTHEIOS_LITERAL_STRING("hello"));


    /* 2 (a). In your code, you'd define your own, most succinct, symbol, e.g. ACME_XSEV(sev, xi28) */
#define ACME_XSEV(sev, xi28)    PANTHEIOS_MAKE_EXTENDED_SEVERITY(PANTHEIOS_SEV_ ## sev, xi28)

    /* 2 (b). and use it as follows */
    pantheios_logprintf(ACME_XSEV(NOTICE, 10), PANTHEIOS_LITERAL_STRING("hello"));


    pantheios_uninit();
  }

  return EXIT_SUCCESS;
}

/* ///////////////////////////// end of file //////////////////////////// */
