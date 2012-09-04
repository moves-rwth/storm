/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/frontends/example.cpp.frontends.custom/example.cpp.frontends.custom.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - definition of a custom front-end that supports tabbed output
 *                - use of pantheios::logputs() in bail-out conditions
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
#include <pantheios/frontend.h>

/* Standard C/C++ Header Files */
#include <exception>                            // for std::exception
#include <new>                                  // for std::bad_alloc
#include <string>                               // for std::string
#include <stdlib.h>                             // for exit codes, atoi()

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# if defined(STLSOFT_COMPILER_IS_MSVC)
#  pragma warning(disable : 4702)
# endif /* compiler */
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* ////////////////////////////////////////////////////////////////////// */

namespace
{
  // By default, we will log everything at NOTICE and below (remember that
  // they get more serious as the values get lower).

  static int  s_severityCeiling = pantheios::notice;

} // anonymous namespace

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* ////////////////////////////////////////////////////////////////////// */

// USAGE: [<severity-ceiling>]
//
// where:
// <severity-ceiling> - a number between 0 and 7, which sets the maximum level
//                      displayed, or -1 to suppress all output.

int main(int argc, char **argv)
{
  if(argc > 1)
  {
    s_severityCeiling = ::atoi(argv[1]);
  }

  try
  {
    pantheios::log_DEBUG(PSTR("debug statement"));
    pantheios::log_INFORMATIONAL(PSTR("informational statement"));
    pantheios::log_NOTICE(PSTR("notice statement"));
    pantheios::log_WARNING(PSTR("warning statement"));
    pantheios::log_ERROR(PSTR("error statement"));
    pantheios::log_CRITICAL(PSTR("critical statement"));
    pantheios::log_ALERT(PSTR("alert statement"));
    pantheios::log_EMERGENCY(PSTR("emergency statement"));


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

/* ////////////////////////////////////////////////////////////////////// */

PANTHEIOS_CALL(int) pantheios_fe_init(  void*   /* reserved */
                                    ,   void**  ptoken)
{
  *ptoken = NULL;

  return 0;
}

PANTHEIOS_CALL(void) pantheios_fe_uninit(void* /* token */)
{}

PANTHEIOS_CALL(PAN_CHAR_T const*) pantheios_fe_getProcessIdentity(void* /* token */)
{
  return PSTR("example.cpp.frontends.custom");
}

PANTHEIOS_CALL(int) pantheios_fe_isSeverityLogged(  void*   /* token */
                                                ,   int     severity
                                                ,   int     /* backEndId */)
{
  return severity <= s_severityCeiling;
}

/* ///////////////////////////// end of file //////////////////////////// */
