/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.be.WindowsSyslog/test.scratch.be.WindowsSyslog.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of custom severity level information for tabbing output
 *                - definition of a custom back-end that supports tabbed output
 *                - use of pantheios::logputs() in bail-out conditions
 *
 * Created:     3rd August 2012
 * Updated:     6th August 2012
 *
 * www:         http://www.pantheios.org/
 *
 * License:     This source code is placed into the public domain 2010
 *              by Synesis Software Pty Ltd. There are no restrictions
 *              whatsoever to your use of the software.
 *
 *              This software is provided "as is", and any warranties,
 *              express or implied, of any kind and for any purpose, are
 *              disclaimed.
 *
 * ////////////////////////////////////////////////////////////////////// */


/* This inclusion required for suppressing warnings during NoX (No eXception-support) configurations. */
#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>
#include <pantheios/backend.h>
#include <pantheios/backends/bec.WindowsSyslog.h>

/* STLSoft Header Files */
#include <platformstl/synch/sleep_functions.h>

/* Standard C/C++ Header Files */
#include <exception>                        // for std::exception
#include <string>                           // for std::string
#include <stdio.h>                          // for fprintf()
#include <stdlib.h>                         // for exit codes
#include <string.h>                         // for memset()

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* ////////////////////////////////////////////////////////////////////// */

// Define the fe.simple process identity, so that it links when using fe.simple
PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.scratch.be.WindowsSyslog");

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
    unsigned shortPause = 1250;

    try
    {

        pantheios::log_NOTICE("Hi!");

        platformstl::micro_sleep(shortPause);

        pantheios::log_NOTICE("This is your logger, calling.");

        platformstl::micro_sleep(shortPause);

        pantheios::log_NOTICE("Here come some diagnostic logging statements ...");

        platformstl::micro_sleep(shortPause);

        pantheios::log_DEBUG("just being pedantic");

        platformstl::micro_sleep(shortPause);

        pantheios::log_INFORMATIONAL("you can ignore this");

        platformstl::micro_sleep(shortPause);

        pantheios::log_NOTICE("this is noteworthy");

        platformstl::micro_sleep(shortPause);

        pantheios::log_WARNING("there may be a problem");

        platformstl::micro_sleep(shortPause);

        pantheios::log_ERROR("there is a problem");

        platformstl::micro_sleep(shortPause);

        pantheios::log_CRITICAL("there is a serious problem");

        platformstl::micro_sleep(shortPause);

        pantheios::log_ALERT("there is a very serious problem");

        platformstl::micro_sleep(shortPause);

        pantheios::log_EMERGENCY("aargh! I'm operating in contradiction to my design!");

        platformstl::micro_sleep(90000);

        return EXIT_SUCCESS;
    }
    catch(std::bad_alloc &)
    {
        pantheios::log_CRITICAL("out of memory");
    }
    catch(std::exception &x)
    {
        pantheios::log_ALERT("Exception: ", x);
    }
    catch(...)
    {
        pantheios::logputs(pantheios::emergency, "Unexpected unknown error");
    }

    return EXIT_FAILURE;
}

/* ///////////////////////////// end of file //////////////////////////// */
