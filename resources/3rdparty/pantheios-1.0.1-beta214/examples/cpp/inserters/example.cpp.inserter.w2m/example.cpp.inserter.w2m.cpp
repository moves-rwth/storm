/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/inserters/example.cpp.inserter.w2m/example.cpp.inserter.w2m.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of Pantheios inserters for w2m types
 *                - use of pantheios::logputs() in bail-out conditions
 *
 * Created:     10th December 2010
 * Updated:     6th August 2012
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



// The following four lines are used only to verify that we're compiling for
// multibyte strings, and would not be necessary in your program.
#include <pantheios/pantheios.h>
#ifdef PANTHEIOS_USE_WIDE_STRINGS
# error This example only compatible with multibyte string builds
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* Pantheios Header Files */
#include <pantheios/pan.hpp>
#include <pantheios/inserters/args.hpp>
#include <pantheios/inserters/w2m.hpp>

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.inserter.w2m");

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char** argv)
{
  try
  {

    pan::log_NOTICE("main(", pan::args(argc, argv, pan::args::arg0FileOnly), ")");

    pan::log_NOTICE("A multibyte string: ", pan::w2m(L"plain-old-me"));


    return EXIT_SUCCESS;
  }
  catch(std::bad_alloc&)
  {
    pantheios::log(pantheios::alert, "out of memory");
  }
  catch(std::exception& x)
  {
    pantheios::log_CRITICAL("Exception: ", x);
  }
  catch(...)
  {
    pantheios::logputs(pantheios::emergency, "Unexpected unknown error");
  }

  return EXIT_FAILURE;
}

/* ///////////////////////////// end of file //////////////////////////// */
