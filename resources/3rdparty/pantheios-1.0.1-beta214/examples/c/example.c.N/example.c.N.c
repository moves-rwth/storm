/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/c/example.c.N/example.c.N.c
 *
 * Purpose:     C example program for Pantheios. Demonstrates:
 *
 *                - use of be.N back-end library that multiplexes output to
 *                  various concrete back-ends
 *                - use of fe.N front-end library, that arbitrates output
 *                  for be.N
 *
 * Created:     5th December 2006
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


/* PlatformSTL Header Files */
#include <platformstl/platformstl.h>                /* for platform discrimination */

/* Pantheios Header Files */
#include <pantheios/pantheios.h>                    /* main Pantheios C header file */
#include <pantheios/frontends/fe.N.h>
#include <pantheios/backends/be.N.h>
#include <pantheios/backends/bec.file.h>
#include <pantheios/backends/bec.fprintf.h>
#include <pantheios/backends/bec.null.h>
#if defined(PLATFORMSTL_OS_IS_UNIX)
# include <pantheios/backends/bec.syslog.h>
#elif defined(PLATFORMSTL_OS_IS_WINDOWS)
# include <pantheios/backends/bec.WindowsSyslog.h>
#else /* ? OS */
# error Operating system not discriminated
#endif /* OS */

/* Standard C Header Files */
#include <stdlib.h>                                 /* for exit codes */

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[]      =   PANTHEIOS_LITERAL_STRING("example.c.N");

pan_fe_N_t PAN_FE_N_SEVERITY_CEILINGS[]  =
{
    { 1,  PANTHEIOS_SEV_NOTICE    } /* Filters out everything below 'notice' */
  , { 2,  PANTHEIOS_SEV_DEBUG     } /* Allows all severities */
  , { 3,  PANTHEIOS_SEV_ERROR     } /* Allows only 'error', 'critical', 'alert', 'emergency' */
  , { 0,  PANTHEIOS_SEV_NOTICE    } /* Terminates the array; sets the default ceiling to 'notice' */
};

pan_be_N_t PAN_BE_N_BACKEND_LIST[] =
{
    PANTHEIOS_BE_N_STDFORM_ENTRY(1, pantheios_be_file, 0)
  , PANTHEIOS_BE_N_STDFORM_ENTRY(2, pantheios_be_fprintf, 0)
  , PANTHEIOS_BE_N_STDFORM_ENTRY(3, pantheios_be_null, 0)
#if defined(PLATFORMSTL_OS_IS_UNIX)
  , PANTHEIOS_BE_N_STDFORM_ENTRY(4, pantheios_be_syslog, 0)
#elif defined(PLATFORMSTL_OS_IS_WINDOWS)
  , PANTHEIOS_BE_N_STDFORM_ENTRY(4, pantheios_be_WindowsSyslog, 0)
#endif /* OS */
  , PANTHEIOS_BE_N_STDFORM_ENTRY(5, pantheios_be_file, 0)
  , PANTHEIOS_BE_N_TERMINATOR_ENTRY
};

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
    pantheios_logputs(PANTHEIOS_SEV_DEBUG, PANTHEIOS_LITERAL_STRING("debug"));
    pantheios_logputs(PANTHEIOS_SEV_INFORMATIONAL, PANTHEIOS_LITERAL_STRING("info"));
    pantheios_logputs(PANTHEIOS_SEV_NOTICE, PANTHEIOS_LITERAL_STRING("notice"));
    pantheios_logputs(PANTHEIOS_SEV_WARNING, PANTHEIOS_LITERAL_STRING("warn"));

    pantheios_be_file_setFilePath(PANTHEIOS_LITERAL_STRING("file-1.log"), PANTHEIOS_BE_FILE_F_TRUNCATE, PANTHEIOS_BE_FILE_F_TRUNCATE, 1);

    pantheios_logputs(PANTHEIOS_SEV_ERROR, PANTHEIOS_LITERAL_STRING("error"));
    pantheios_logputs(PANTHEIOS_SEV_CRITICAL, PANTHEIOS_LITERAL_STRING("critical"));
    pantheios_logputs(PANTHEIOS_SEV_ALERT, PANTHEIOS_LITERAL_STRING("alert"));
    pantheios_logputs(PANTHEIOS_SEV_EMERGENCY, PANTHEIOS_LITERAL_STRING("emergency"));

    pantheios_be_file_setFilePath(PANTHEIOS_LITERAL_STRING("file-5.log"), 0, 0, 5);

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
