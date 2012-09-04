/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.implicit_link/test.scratch.implicit_link.cpp
 *
 * Purpose:     Implicit link file for the test.scratch.implicit_link project.
 *
 * Created:     18th July 2007
 * Updated:     6th August 2012
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2008-2012, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>

#include <pantheios/implicit_link/core.h>

#include <pantheios/implicit_link/fe.simple.h>

#include <pantheios/implicit_link/be.fprintf.h>
//#include <pantheios/implicit_link/be.WindowsDebugger.h>

//#include <pantheios/implicit_link/be.lrsplit.h>
//#include <pantheios/implicit_link/bel.fprintf.h>

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.scratch.implicit_link");

int main()
{
    pantheios::log_ALERT("A simple log entry");

    pantheios::log_NOTICE("A simple log entry");

    return 0;
}
