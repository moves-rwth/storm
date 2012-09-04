/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/c/example.c.log_n/example.c.log_n.c
 *
 * Purpose:     C example program for Pantheios. Demonstrates:
 *
 *                - use of pantheios_log_?() in C compilation units
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
#include <pantheios/util/string/snprintf.h> /* for pantheios_util_snprintf() */
#include <pantheios/internal/safestr.h>

/* Standard C Header Files */
#include <stdio.h>                    /* for sprintf() */
#include <stdlib.h>                   /* for exit codes */

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.c.log_n");

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
    int         numUsers = 1000000;
    PAN_CHAR_T  szNumUsers[101];

    /* Log a three part statement. Note that specifying -1
     * for the length causes the corresponding string argument to be
     * interpreted as a nul-terminated string, and its length to be
     * calculated (via strlen()); Output:
     * "We're sure there're likely to be >00000000000001000000 satisfied users of Pantheios"
     */
    pantheios_log_3(PANTHEIOS_SEV_CRITICAL
                  , PANTHEIOS_LITERAL_STRING("We're sure there're likely to be >"),     -1
                  , szNumUsers,                                                         pantheios_util_snprintf(&szNumUsers[0], STLSOFT_NUM_ELEMENTS(szNumUsers), PANTHEIOS_LITERAL_STRING("%020d"), numUsers)
                  , PANTHEIOS_LITERAL_STRING(" satisfied users of Pantheios"),          -1);

    /* Must uninitialise Pantheios.
     *
     * pantheios_uninit() must be called once for each successful (>=0)
     * invocation of pantheios_init().
     */
    pantheios_uninit();

    return EXIT_SUCCESS;
  }
}

/* ////////////////////////////////////////////////////////////////////// */
