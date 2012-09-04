/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.WideString/test.scratch.WideString.cpp
 *
 * Purpose:     Implementation file for the test.scratch.WideString project.
 *
 * Created:     22nd March 2010
 * Updated:     10th January 2011
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2010-2011, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/args.hpp>
#include <pantheios/inserters/b64.hpp>
#include <pantheios/inserters/blob.hpp>
#include <pantheios/inserters/hex_ptr.hpp>
#include <pantheios/inserters/integer.hpp>
#include <pantheios/inserters/pointer.hpp>
#include <pantheios/inserters/processid.hpp>
#include <pantheios/inserters/real.hpp>
#include <pantheios/inserters/slice.hpp>
#include <pantheios/inserters/threadid.hpp>

#include <pantheios/frontends/stock.h>

#include <stdlib.h>
#include <tchar.h>

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

extern "C" PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.scratch.WideString");

/* /////////////////////////////////////////////////////////////////////////
 * main()
 */

int _tmain(int argc, _TCHAR* argv[])
{
    pantheios::log(pantheios::debug, PANTHEIOS_LITERAL_STRING("main(argc/v={"), pantheios::args(argc, argv), PANTHEIOS_LITERAL_STRING("})"));

    pantheios::log(pantheios::debug, PANTHEIOS_LITERAL_STRING("b64: {"), pantheios::b64("abcd", 5), PANTHEIOS_LITERAL_STRING("}"));

    pantheios::log(pantheios::debug, PANTHEIOS_LITERAL_STRING("i: {"), pantheios::integer(-10101, 4, pantheios::fmt::fullHex), PANTHEIOS_LITERAL_STRING("}"));

    pantheios::log(pantheios::debug, PANTHEIOS_LITERAL_STRING("xp: {"), pantheios::hex_ptr("abcd"), PANTHEIOS_LITERAL_STRING("}"));

    pantheios::log(pantheios::debug, PANTHEIOS_LITERAL_STRING("real: {"), pantheios::real(-1.2345), PANTHEIOS_LITERAL_STRING("}"));

    pantheios::log(pantheios::debug, PANTHEIOS_LITERAL_STRING("process-id: {"), pantheios::processId, PANTHEIOS_LITERAL_STRING("}"));

    pantheios::log(pantheios::debug, PANTHEIOS_LITERAL_STRING("thread-id: {"), pantheios::threadId, PANTHEIOS_LITERAL_STRING("}"));

    pantheios::log(pantheios::debug, PANTHEIOS_LITERAL_STRING("blob: {"), pantheios::blob("abcd", 5), PANTHEIOS_LITERAL_STRING("}"));

    pantheios::log(pantheios::debug, PANTHEIOS_LITERAL_STRING("pointer: {"), pantheios::pointer("abcd", pantheios::fmt::fullHex), PANTHEIOS_LITERAL_STRING("}"));

    pantheios::log(pantheios::debug, PANTHEIOS_LITERAL_STRING("slice: {"), pantheios::slice(PANTHEIOS_LITERAL_STRING("abcd"), 3), PANTHEIOS_LITERAL_STRING("}"));

    return 0;
}

/* ///////////////////////////// end of file //////////////////////////// */
