/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/misc/example.cpp.misc.no_namespace/example.cpp.misc.no_namespace.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of Pantheios with the namespace suppressed
 *
 * Created:     15th March 2008
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
#define PANTHEIOS_NO_NAMESPACE                            // Suppress the namespace
#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS // Faster compilation
#include <pantheios/pantheios.hpp>

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>

/* Standard C Header Files */
#include <stdlib.h>                         // for exit codes

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
extern "C" const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[]    =   PANTHEIOS_LITERAL_STRING("example.cpp.misc.no_namespace");

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
  log_DEBUG(PSTR("debug"));
  log_INFORMATIONAL(PSTR("informational"));
  log_NOTICE(PSTR("notice"));
  log_WARNING(PSTR("warning"));
  log_ERROR(PSTR("error"));
  log_CRITICAL(PSTR("critical"));
  log_ALERT(PSTR("alert"));
  log_EMERGENCY(PSTR("emergency"));

  return EXIT_SUCCESS;
}

/* ///////////////////////////// end of file //////////////////////////// */
