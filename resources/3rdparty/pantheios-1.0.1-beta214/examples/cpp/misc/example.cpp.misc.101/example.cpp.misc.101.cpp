/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/misc/example.cpp.misc.101/example.cpp.misc.101.cpp
 *
 * Purpose:     C++ example program for introducing the basic essential
 *              features of Pantheios when using the C++ API.
 *              Demonstrates:
 *
 *                - how the Pantheios libraries do not need to be explicitly
 *                  initialised in a C++ program
 *                - use of pantheios::log()
 *
 * Created:     17th January 2008
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
#include <pantheios/pantheios.hpp>      /* The root header for Panthieos when using the C++-API. */
#include <pantheios/inserters.hpp>      /* Includes all headers for inserters, incl. integer, real, character */
#include <pantheios/frontends/stock.h>  /* Declares the process identity symbol PANTHEIOS_FE_PROCESS_IDENTITY */

/* Standard C++ Header Files */
#include <algorithm>

/* Standard C Header Files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.misc.101");


/* /////////////////////////////////////////////////////////////////////////
 * Macros
 */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)


/* /////////////////////////////////////////////////////////////////////////
 * main
 */

int main()
{
    /* Note: there is no need to explicitly initialise the Pantheios
     * libraries when the program contains 1 or more C++ compilation units
     * that, like this one, #include <pantheios/pantheios.hpp>, since that
     * file contains mechanisms within it that cause the initialisation to
     * be done automatically.
     */


    /* The libraries are now initialised, so we can output diagnostic
     * logging statements.
     */

    /* 1. use pantheios::log(), which takes a severity level and between
     * 1 and 32 statement element parameters
     */

    pantheios::log(PANTHEIOS_SEV_NOTICE, PSTR("The"));
    pantheios::log(PANTHEIOS_SEV_NOTICE, PSTR("log() "), PSTR("method"));
    pantheios::log(PANTHEIOS_SEV_NOTICE, PSTR("can "), PSTR("output "), PSTR("any"));
    pantheios::log(PANTHEIOS_SEV_NOTICE, PSTR("number "), PSTR("of "), PSTR("parameters "), PSTR("between"));
    pantheios::log(PANTHEIOS_SEV_NOTICE, PSTR("1"));
    pantheios::log(PANTHEIOS_SEV_NOTICE, PSTR("and"));
    pantheios::log(PANTHEIOS_SEV_NOTICE, PSTR("01, "), PSTR("02, "), PSTR("03, "), PSTR("04, "), PSTR("05, "), PSTR("06, "), PSTR("07, "), PSTR("08, "), PSTR("09, "), PSTR("10, "), PSTR("11, "), PSTR("12, "), PSTR("13, "), PSTR("14, "), PSTR("15, "), PSTR("16, "), PSTR("17, "), PSTR("18, "), PSTR("19, "), PSTR("20, "), PSTR("21, "), PSTR("22, "), PSTR("23, "), PSTR("24, "), PSTR("25, "), PSTR("26, "), PSTR("27, "), PSTR("28, "), PSTR("29, "), PSTR("30, "), PSTR("31, "), PSTR("32"));



    /* 2. use pantheios::log() to output some fundamental type
     * instances
     */

    int     i = 10;
    double  d = 20.20;
    char    c = 'c';


    pantheios::log(PANTHEIOS_SEV_WARNING, PSTR("pantheios::log() is flexible, and output all fundamental types, including ints (e.g. i = "), pantheios::integer(i), PSTR(", doubles (e.g. d = "), pantheios::real(d), PSTR("), and chars (e.g. c = "), pantheios::character(c), PSTR(")"));



    /* 3. use pantheios::log() to illustrate that there is no size limit
     * imposed by the C++ API
     */

    PAN_CHAR_T bigBuff[5000];

    std::fill_n(bigBuff, STLSOFT_NUM_ELEMENTS(bigBuff), '-');
    bigBuff[STLSOFT_NUM_ELEMENTS(bigBuff) - 1] = '\0';


    pantheios::log(PANTHEIOS_SEV_WARNING, PSTR("NOTE: pantheios::log() output is not limited to any number of characters. The rest of this statement is a very long string of '-' characters, enclosed in quotations. As you can see, it will be fully emitted: \""), bigBuff, PSTR("\""));



    /* Note: there is no need to explicitly uninitialise the Pantheios
     * libraries when the program contains 1 or more C++ compilation units
     * that, like this one, #include <pantheios/pantheios.hpp>, since that
     * file contains mechanisms within it that cause the initialisation to
     * be done automatically.
     */

    return EXIT_SUCCESS;
}

/* ///////////////////////////// end of file //////////////////////////// */
