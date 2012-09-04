/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.fe.simple.WithCallback/test.scratch.fe.simple.WithCallback.cpp
 *
 * Purpose:     Implementation file for the test.scratch.fe.simple.WithCallback project.
 *
 * Created:     15th November 2010
 * Updated:     29th November 2010
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2010, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */



#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS

#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

/* Pantheios Header Files */
#include <pantheios/pan.hpp>
#include <pantheios/frontends/stock.h>

/* Standard C++ Header Files */
#include <exception>
#include <iostream>

/* Standard C Header Files */
#include <stdlib.h>

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* /////////////////////////////////////////////////////////////////////////
 * Application-defined functions
 */

PANTHEIOS_CALL(PAN_CHAR_T const*) pantheios_fe_getAppProcessIdentity(void) /* throw() */
{
    return PANTHEIOS_LITERAL_STRING("test.scratch.fe.simple.WithCallback");
}

/* ////////////////////////////////////////////////////////////////////// */

static int main_(int /* argc */, char** /*argv*/)
{
    pan::log_DEBUG(PANTHEIOS_LITERAL_STRING("debug"));
    pan::log_INFORMATIONAL(PANTHEIOS_LITERAL_STRING("informational"));
    pan::log_NOTICE(PANTHEIOS_LITERAL_STRING("notice"));
    pan::log_WARNING(PANTHEIOS_LITERAL_STRING("warning"));
    pan::log_ERROR(PANTHEIOS_LITERAL_STRING("error"));
    pan::log_CRITICAL(PANTHEIOS_LITERAL_STRING("critical"));
    pan::log_ALERT(PANTHEIOS_LITERAL_STRING("alert"));
    pan::log_EMERGENCY(PANTHEIOS_LITERAL_STRING("emergency"));

    return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
    try
    {
        return main_(argc, argv);
    }
    catch(std::exception &x)
    {
        pan::log_ALERT(PANTHEIOS_LITERAL_STRING("Unexpected general error: "), x, PANTHEIOS_LITERAL_STRING(". Application terminating"));
    }
    catch(...)
    {
        pan::logputs(pan::emergency, PANTHEIOS_LITERAL_STRING("Unhandled unknown error"));
    }

    return EXIT_FAILURE;
}

/* ///////////////////////////// end of file //////////////////////////// */
