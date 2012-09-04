/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/c/example.c.core.pantheios_logprintf/example.c.core.pantheios_logprintf.c
 *
 * Purpose:     C example program for Pantheios. Demonstrates:
 *
 *                - use of pantheios_logprintf() in C compilation units
 *
 * Created:     31st August 2006
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
#include <pantheios/pantheios.h>            /* Pantheios C main header */

/* Standard C Header Files */
#include <stdlib.h>                         /* for exit codes */

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.c.core.pantheios_logprintf");

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
  /* Must initialise Pantheios, when using from C (and there are no C++
   * compilation units in the link-unit).
   *
   * If this is not done, undefined behaviour will ensue ...
   */
  if(pantheios_init() < 0)
  {
    return EXIT_FAILURE;
  }
  else
  {
    int   i = 123;
    float f = 99.99f;

    /* Log the int and the float: Output: "int=123, float=99.99"
     */
    pantheios_logprintf(PANTHEIOS_SEV_INFORMATIONAL
                      , PANTHEIOS_LITERAL_STRING("int=%d, float=%g")
                      , i, f);

    /* Must uninitialise Pantheios.
     *
     * pantheios_uninit() must be called once for each successful (>=0)
     * invocation of pantheios_init().
     */
    pantheios_uninit();

    return EXIT_SUCCESS;
  }
}

/* ///////////////////////////// end of file //////////////////////////// */
