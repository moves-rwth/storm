/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/c/example.c.101/example.c.101.c
 *
 * Purpose:     C example program for introducing the basic essential
 *              features of Pantheios when using only the C API.
 *              Demonstrates:
 *
 *                - how the Pantheios libraries must be explicitly
 *                  initialised in a C program; this is not the case in
 *                  C++ programs
 *                - use of pantheios_logputs()
 *                - use of pantheios_logprintf()
 *                - the statement size limitation imposed by
 *                  pantheios_logprintf()
 *
 * Created:     17th January 2008
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


/* Pantheios Header Files */
#include <pantheios/pantheios.h>        /* The root header for Panthieos when using the C-API. */
#include <pantheios/frontends/stock.h>  /* Declares the process identity symbol PANTHEIOS_FE_PROCESS_IDENTITY */

/* Standard C Header Files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.c.101");

/* /////////////////////////////////////////////////////////////////////////
 * main
 */

int main()
{
    int retCode = EXIT_SUCCESS;
    int res;

    /* Initialise the Pantheios libraries. This will cause the Core to
     * initialise, which will itself initialise the front-end and back-end
     * libraries.
     *
     * Note: This is not required when your program contains 1 or more
     * C++ compilation units that #include <pantheios/pantheios.hpp>, since
     * that file contains mechanisms within it that cause the initialisation
     * to be done automatically. However, it is still best practice to do so
     * because that other compilation unit might be removed or replaced with
     * a C compilation unit at a future time.
     */
    res = pantheios_init();

    if(0 != res)
    {
        /* If initialisation failed, we report why using
         * pantheios_getInitCodeString()
         */

        fprintf(stderr, "Failed to initialise the Pantheios libraries: %s\n", pantheios_getInitCodeString(res));

        retCode = EXIT_FAILURE;
    }
    else
    {
        /* The libraries are now initialised, so we can output diagnostic
         * logging statements.
         */

        PAN_CHAR_T  bigBuff[5000];
        size_t      n;

        int         i = 10;
        double      d = 20.20;
        PAN_CHAR_T  c = 'c';


        /* 1. use pantheios_logputs(), which takes a severity level and a
         * single string
         */

        pantheios_logputs(PANTHEIOS_SEV_NOTICE, PANTHEIOS_LITERAL_STRING("pantheios_logputs() can output a single C-style string"));



        /* 2. use pantheios_logprintf() to output some fundamental type
         * instances
         */

        pantheios_logprintf(PANTHEIOS_SEV_WARNING, PANTHEIOS_LITERAL_STRING("pantheios_logprintf() uses a format string like this one, and can output all the types compatible with the printf() family, including ints (e.g. i = %d), doubles (e.g. d = %G), and chars (e.g. c = %c)"), i, d, c);



        /* 3. use pantheios_logprintf() to illustrate the size limit imposed by
         * this API function
         *
         * Note: the C++ API log() method overloads do not impose a
         * statement size limit; nor does pantheios_logputs()
         */

        for(n = 0; n != (sizeof(bigBuff) / sizeof(0[bigBuff])) - 1; ++n)
        {
            bigBuff[n] = '-';
        }
        bigBuff[n] = '\0';


        pantheios_logprintf(PANTHEIOS_SEV_WARNING, PANTHEIOS_LITERAL_STRING("NOTE: pantheios_logprintf() output is limited to 4095 characters. The rest of this statement is a very long string of '-' characters, enclosed in quotations. As you can see, it will not be fully emitted: \"%s\""), bigBuff);



        /* Uninitialise the Pantheios libraries.
         *
         * Note: This is not required when your program contains 1 or more
         * C++ compilation units that #include <pantheios/pantheios.hpp>,
         * since that file contains mechanisms within it that cause the
         * initialisation/uninitialisation to occur automatically.
         */
        pantheios_uninit();
    }

    return retCode;
}

/* ///////////////////////////// end of file //////////////////////////// */
