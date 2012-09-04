/* /////////////////////////////////////////////////////////////////////////
 * File:        test/unit/test.unit.levels.dynamic_initialisation/test.unit.levels.dynamic_initialisation.cpp
 *
 * Purpose:     Implementation file for the test.unit.levels.dynamic_initialisation project.
 *
 * Created:     20th October 2007
 * Updated:     6th August 2012
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2007-2012, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

class DynamicInit
{
public:
    DynamicInit();

public:
    const int debug;
    const int informational;
    const int notice;
    const int warning;
    const int error;
    const int critical;
    const int alert;
    const int emergency;

private:
    DynamicInit(DynamicInit const&);
    DynamicInit& operator =(DynamicInit const&);

} levels;

#define PANTHEIOS_NO_INCLUDE_COMSTL_STRING_ACCESS
#define PANTHEIOS_NO_INCLUDE_UNIXSTL_STRING_ACCESS
#define PANTHEIOS_NO_INCLUDE_WINSTL_STRING_ACCESS

/* Pantheios Header Files */
#include <pantheios/pantheios.h>

/* xTests Header Files */
#include <xtests/xtests.h>

/* Standard C++ Header Files */
#include <exception>                    // for std::exception
#include <new>                          // for std::bad_alloc

/* Standard C Header Files */
#include <stdlib.h>                     // for exit codes

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* /////////////////////////////////////////////////////////////////////////
 * Macros
 */

#define PANTHEIOS_SEV_LEVELS_EQUAL(x, y)    XTESTS_TEST_INTEGER_EQUAL(static_cast<int>(x), static_cast<int>(y))

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.unit.levels.dynamic_initialisation");

/* ////////////////////////////////////////////////////////////////////// */

const int   debug           =   ::pantheios::debug;
const int   informational   =   ::pantheios::informational;
const int   notice          =   ::pantheios::notice;
const int   warning         =   ::pantheios::warning;
const int   error           =   ::pantheios::error;
const int   critical        =   ::pantheios::critical;
const int   alert           =   ::pantheios::alert;
const int   emergency       =   ::pantheios::emergency;

DynamicInit::DynamicInit()
    : debug         (::pantheios::debug)
    , informational (::pantheios::informational)
    , notice        (::pantheios::notice)
    , warning       (::pantheios::warning)
    , error         (::pantheios::error)
    , critical      (::pantheios::critical)
    , alert         (::pantheios::alert)
    , emergency     (::pantheios::emergency)
{}

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if(XTESTS_START_RUNNER("test.unit.levels.dynamic_initialisation", verbosity))
    {
        /* Case 1 - verifying Pantheios levels instances */
        if(!XTESTS_CASE_BEGIN("case-1", "verifying Pantheios levels instances"))
        {
            retCode = EXIT_FAILURE;
        }
        else
        {
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_EMERGENCY, ::pantheios::emergency);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_ALERT, ::pantheios::alert);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_CRITICAL, ::pantheios::critical);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_ERROR, ::pantheios::error);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_WARNING, ::pantheios::warning);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_NOTICE, ::pantheios::notice);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_INFORMATIONAL, ::pantheios::informational);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_DEBUG, ::pantheios::debug);

            XTESTS_CASE_END("");
        }

        /* Case 2 - verifying non-local constants */
        if(!XTESTS_CASE_BEGIN("case-2", "verifying non-local constants"))
        {
            retCode = EXIT_FAILURE;
        }
        else
        {
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_EMERGENCY, ::emergency);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_ALERT, ::alert);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_CRITICAL, ::critical);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_ERROR, ::error);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_WARNING, ::warning);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_NOTICE, ::notice);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_INFORMATIONAL, ::informational);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_DEBUG, ::debug);

            XTESTS_CASE_END("");
        }

        /* Case 3 - verifying dynamic initialisation levels instances */
        if(!XTESTS_CASE_BEGIN("case-3", "verifying dynamic initialisation levels instances"))
        {
            retCode = EXIT_FAILURE;
        }
        else
        {
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_EMERGENCY, levels.emergency);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_ALERT, levels.alert);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_CRITICAL, levels.critical);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_ERROR, levels.error);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_WARNING, levels.warning);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_NOTICE, levels.notice);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_INFORMATIONAL, levels.informational);
            PANTHEIOS_SEV_LEVELS_EQUAL(PANTHEIOS_SEV_DEBUG, levels.debug);

            XTESTS_CASE_END("");
        }

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}

/* ///////////////////////////// end of file //////////////////////////// */
