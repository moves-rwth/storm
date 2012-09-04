/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/misc/example.cpp.misc.hetero1/example.cpp.misc.hetero1.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of implicit support for heterogeneous non-string types
 *                - use of pantheios::logputs() in bail-out conditions
 *
 * Created:     31st August 2006
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
#include <pantheios/pantheios.hpp>            // Pantheios C++ main header
#include <pantheios/internal/safestr.h>

/* STLSoft Header Files */
#include <platformstl/platformstl.h>        // PlatformSTL main C header

/* Standard C/C++ Header Files */
#include <exception>                        // for std::exception
#include <new>                              // for std::bad_alloc
#include <string>                           // for std::string
#include <stdlib.h>                         // for exit codes

#if defined(PLATFORMSTL_OS_IS_UNIX)
# include <dirent.h>
#endif /* OS */

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.misc.hetero1");

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
  try
  {

#if defined(PLATFORMSTL_OS_IS_UNIX)

    // Demonstrates handling of heterogeneous types by using UNIX/std
    // types: struct tm, struct dirent
    //
    // Output:
    // "Heterogeneous values: tm=Aug 31 15:12:18 2006; de=."

    time_t          t   =   ::time(NULL);
# ifdef PANTHEIOS_USING_SAFE_STR_FUNCTIONS
    struct tm       tm_;
    errno_t         e   =   ::localtime_s(&tm_, &t);
    struct tm       *tm =   (0== e) ? &tm_ : NULL;
# else /* ? PANTHEIOS_USING_SAFE_STR_FUNCTIONS */
    struct tm       *tm =   ::localtime(&t);
# endif /* PANTHEIOS_USING_SAFE_STR_FUNCTIONS */
    DIR             *d  =   ::opendir(".");
    struct dirent   *de =   (NULL != d) ? readdir(d) : NULL;


    pantheios::log_NOTICE("Heterogeneous values: tm=", tm, "; de=", de);

    if(NULL != d)
    {
      ::closedir(d);
    }

#elif defined(PLATFORMSTL_OS_IS_WINDOWS)

    // Demonstrates handling of heterogeneous types by using Win32
    // types: GUID, SYSTEMTIME, and VARIANT
    //
    // Output:
    // "Heterogeneous values: g={00000000-0000-0000-0000-000000000000}; s=31/08/2006 2:48:41 pm; v=123456789"

    GUID        g   =   GUID_NULL;
    SYSTEMTIME  s;
    VARIANT     v;

    ::GetLocalTime(&s);

    ::VariantInit(&v);
    v.vt    =   VT_I4;
    v.lVal  =   123456789;

    pantheios::log_NOTICE(PSTR("Heterogeneous values: g="), g, PSTR("; s="), s, PSTR("; v="), v);

#else /* ? OS */
# error Operating system not discriminated
#endif /* OS */

    return EXIT_SUCCESS;
  }
  catch(std::bad_alloc&)
  {
    pantheios::logputs(pantheios::alert, PSTR("out of memory"));
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

/* ///////////////////////////// end of file //////////////////////////// */
