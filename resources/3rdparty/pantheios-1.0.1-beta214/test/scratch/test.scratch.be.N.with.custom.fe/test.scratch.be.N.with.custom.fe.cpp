/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.be.N.with.custom.fe/test.scratch.be.N.with.custom.fe.cpp
 *
 * Purpose:     Implementation file for the test.scratch.be.N.with.custom.fe project.
 *
 * Created:     23rd December 2010
 * Updated:     6th August 2012
 *
 * Thanks:      To wassime, for submitting the original program definition.
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2010-2012, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS

/* Pantheios Header Files */
#include <pantheios/pan.hpp>
#include <pantheios/backends/be.N.h>
#include <pantheios/backends/bec.file.h>
#include <pantheios/backends/bec.console.h>
#include <pantheios/frontend.h>

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>
#include <platformstl/platformstl.hpp>

/* Standard C++ Header Files */
#include <exception>
#include <iostream>

/* Standard C Header Files */
#include <stdio.h>
#include <stdlib.h>

/* /////////////////////////////////////////////////////////////////////////
 * Macros and definitions
 */

#define WG_LOG_FILE_ID          1
#define WG_LOG_CONSOLE_ID       2
#define WG_LOG_FILE_ERROR_ID    3

namespace
{
static int iCeilingConsole  = PANTHEIOS_SEV_DEBUG;
static int iCeilingMain     = PANTHEIOS_SEV_NOTICE;
static int iCeilingError    = PANTHEIOS_SEV_ERROR;
} /* anonymous namespace */

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.scratch.be.N.with.custom.fe");

/* /////////////////////////////////////////////////////////////////////////
 * main()
 */

static int main_(int /* argc */, char** /*argv*/)
{
    pantheios_be_file_setFilePath("normal.log", 0, 0, WG_LOG_FILE_ID);
    pantheios_be_file_setFilePath("error.log", 0, 0, WG_LOG_FILE_ERROR_ID);

    pantheios::log_WARNING("hello there all of you");
    pantheios::log_NOTICE("hello there console and main log");
    pantheios::log_DEBUG("hello there console");

    return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
    try
    {
        return main_(argc, argv);
    }
    catch(std::exception& x)
    {
        pantheios::log_ALERT("Unexpected general error: ", x, ". Application terminating");
    }
    catch(...)
    {
        pantheios::logputs(pantheios::emergency, "Unhandled unknown error");
    }

    return EXIT_FAILURE;
}

/* /////////////////////////////////////////////////////////////////////////
 * Back-end configuration
 */

pan_be_N_t PAN_BE_N_BACKEND_LIST[] =
{
    PANTHEIOS_BE_N_STDFORM_ENTRY(WG_LOG_CONSOLE_ID,     pantheios_be_console,   0),
    PANTHEIOS_BE_N_STDFORM_ENTRY(WG_LOG_FILE_ID,        pantheios_be_file,      0),
    PANTHEIOS_BE_N_STDFORM_ENTRY(WG_LOG_FILE_ERROR_ID,  pantheios_be_file,      0),

    PANTHEIOS_BE_N_TERMINATOR_ENTRY
};

/* /////////////////////////////////////////////////////////////////////////
 * Custom front-end
 */

PANTHEIOS_CALL(int) pantheios_fe_init(  void*,void**  ptoken)
{
    *ptoken = NULL;

    return 0;
}

PANTHEIOS_CALL(void) pantheios_fe_uninit(void*)
{}

PANTHEIOS_CALL(PAN_CHAR_T const*) pantheios_fe_getProcessIdentity(void*)
{
    return "BE.N.Experimenting";
}

PANTHEIOS_CALL(int) pantheios_fe_isSeverityLogged(void*, int severity, int beid)
{
    switch(beid)
    {
        // Must handle PANTHEIOS_BEID_ALL, as that's the Application Layer's
        // (initial) enquiry as to whether anything should be logged at all
        case    PANTHEIOS_BEID_ALL: 
#if 0
            // The inefficient way to do this is to just 'return true'
            return true;
#else /* ? 0 */
            // The efficient (but complicated) way to do this is to see if
            // *any* back-end wants output, in which case we say yes
            return  severity <= iCeilingMain || 
                    severity <= iCeilingConsole ||
                    severity <= iCeilingError;
#endif /* 0 */

        // Now handle each specified back-end, which will come from be.N
        // multiplexing the output(s)

        case    WG_LOG_FILE_ID:
            return severity <= iCeilingMain;
        case    WG_LOG_CONSOLE_ID:
            return severity <= iCeilingConsole;
        case    WG_LOG_FILE_ERROR_ID:
            return severity <= iCeilingError;

        // Don't know about anything else
        default:
            PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_API("unexpected back-end identifier");
            return false;
    }
}

/* ///////////////////////////// end of file //////////////////////////// */
