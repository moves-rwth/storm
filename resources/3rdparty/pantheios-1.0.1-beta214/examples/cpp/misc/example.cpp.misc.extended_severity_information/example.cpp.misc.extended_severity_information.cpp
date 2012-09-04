/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/misc/example.cpp.misc.extended_severity_information/example.cpp.misc.extended_severity_information.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of extended severity level information for tabbing output
 *                - definition of a custom back-end that supports tabbed output
 *                - use of pantheios::logputs() in bail-out conditions
 *
 * Created:     31st August 2006
 * Updated:     26th December 2010
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
#include <pantheios/pantheios.hpp>                          // Pantheios C++ main header
#include <pantheios/backend.h>
#include <pantheios/util/string/strdup.h>

#include <pantheios/init_codes.h>

/* STLSoft Header Files */
#include <pantheios/util/memory/auto_buffer_selector.hpp>   // for stlsoft::auto_buffer
#include <platformstl/system/system_traits.hpp>

/* Standard C/C++ Header Files */
#include <exception>                                        // for std::exception
#include <new>                                              // for std::bad_alloc
#include <string>                                           // for std::string
#include <stdio.h>                                          // for fprintf()
#include <stdlib.h>                                         // for exit codes
#include <string.h>                                         // for memset()

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# if defined(STLSOFT_COMPILER_IS_MSVC)
#  pragma warning(disable : 4702)
# endif /* compiler */
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.misc.extended_severity_information");

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
  try
  {
    // Logging at standard "notice" level; Output: "Hello"

    pantheios::log(pantheios::notice, PSTR("Hello"));

    // Logging at standard "notice" level, with additional
    // severity level information; Output: "  Hello"

    pantheios::log(pantheios::notice(2), PSTR("Hello"));


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

PANTHEIOS_CALL(int) pantheios_be_init(
    PAN_CHAR_T const*   processIdentity
,   void*               /* reserved */
,   void**              ptoken
)
{
  *ptoken = pantheios::util::strdup_nothrow(processIdentity);

  return (NULL == *ptoken) ? PANTHEIOS_INIT_RC_OUT_OF_MEMORY : PANTHEIOS_INIT_RC_SUCCESS;
}

PANTHEIOS_CALL(void) pantheios_be_uninit(void *token)
{
  pantheios::util::strfree(static_cast<PAN_CHAR_T*>(token));
}

PANTHEIOS_CALL(int) pantheios_be_logEntry(  
    void*               /* feToken */
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
)
{
  int severityLevel = severity & 0x0f;
  int customInfo28  = severity >> 4;

  try
  {
    pantheios::util::auto_buffer_selector<char, 256>::type  prefixes(static_cast<size_t>(customInfo28));
    PAN_CHAR_T const*                                       severity   = pantheios::getStockSeverityString(severityLevel);
    FILE*                                                   stm        = (severityLevel < pantheios::notice) ? stderr : stdout;
    PAN_CHAR_T const*                                       processId  = static_cast<PAN_CHAR_T const*>(beToken);

    platformstl::system_traits<char>::str_set(&prefixes[0], prefixes.size(), ' ');

#ifdef PANTHEIOS_USE_WIDE_STRINGS
    return ::fwprintf(stm
                    , L"[%s; %s]:%.*s %.*s\n"
                    , processId, severity
                    , int(prefixes.size()), prefixes.data()
                    , int(cchEntry), entry);
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    return ::fprintf( stm
                    , "[%s; %s]:%.*s %.*s\n"
                    , processId, severity
                    , int(prefixes.size()), prefixes.data()
                    , int(cchEntry), entry);
#endif /* PANTHEIOS_USE_WIDE_STRINGS */
  }
  catch(std::exception &)
  {
    return 0;
  }
}

/* ///////////////////////////// end of file //////////////////////////// */
