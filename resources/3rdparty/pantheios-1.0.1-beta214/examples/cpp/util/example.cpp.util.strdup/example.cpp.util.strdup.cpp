/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/util/example.cpp.util.strdup/example.cpp.util.strdup.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of pantheios::util::strdup_throw() and 
 *                  pantheios::util::strdup_nothrow() for creating C-style
 *                  strings
 *
 * Created:     27th December 2010
 * Updated:     4th January 2011
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


#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

/* Pantheios Header Files */
#include <pantheios/util/string/strdup.h>       // for pantheios::util::strdup_throw/strdup_nothrow()

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>                    // for STLSOFT_CF_THROW_BAD_ALLOC

/* Standard C/C++ Header Files */
#include <new>                                  // for std::bad_alloc
#include <stdlib.h>                             // for exit codes

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* ////////////////////////////////////////////////////////////////////// */

int main(int /* argc */, char** /* argv */)
{
  { // nothrow form

    PAN_CHAR_T* s = pantheios::util::strdup_nothrow(PSTR("abc"));

    if(NULL == s)
    {
      // ... failed to allocate
    }
    else
    {
      // ... allocated successfully


      // free string after using it

      pantheios::util::strfree(s);
    }
  }

#ifdef STLSOFT_CF_THROW_BAD_ALLOC

  { // throw form

    try
    {
      PAN_CHAR_T* s = pantheios::util::strdup_throw(PSTR("abc"));

      // ... allocated successfully


      // free string after using it

      pantheios::util::strfree(s);
    }
    catch(std::bad_alloc&)
    {
      // ... failed to allocate
    }
  }

#endif /* STLSOFT_CF_THROW_BAD_ALLOC */

  return EXIT_SUCCESS;
}

/* ///////////////////////////// end of file //////////////////////////// */
