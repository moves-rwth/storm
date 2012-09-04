/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.be.speech/test.scratch.be.speech.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of custom severity level information for tabbing output
 *                - definition of a custom back-end that supports tabbed output
 *                - use of pantheios::logputs() in bail-out conditions
 *
 * Created:     31st August 2006
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


/* This inclusion required for suppressing warnings during NoX (No eXception-support) configurations. */
#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>
#include <pantheios/backend.h>
#include <pantheios/implicit_link/core.h>
#include <pantheios/implicit_link/fe.simple.h>
#include <pantheios/implicit_link/be.lrsplit.h>
#include <pantheios/implicit_link/bel.WindowsConsole.h>
#include <pantheios/implicit_link/bec.speech.WithCallback.h>
#include <pantheios/backends/bec.speech.h>

/* Standard C/C++ Header Files */
#include <exception>                        // for std::exception
#include <string>                           // for std::string
#include <stdio.h>                          // for fprintf()
#include <stdlib.h>                         // for exit codes
#include <string.h>                         // for memset()

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* ////////////////////////////////////////////////////////////////////// */

// Define the fe.simple process identity, so that it links when using fe.simple
PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.scratch.speech");

/* ////////////////////////////////////////////////////////////////////// */

//PANTHEIOS_BE_DEFINE_BE_FUNCTIONS(speech)
PANTHEIOS_BE_DEFINE_BER_FUNCTIONS(speech)

PANTHEIOS_CALL(void) pantheios_be_speech_getAppInit(int backEndId, pan_be_speech_init_t* init) /* throw() */
{
//  init->flags |= PANTHEIOS_BE_SPEECH_F_SYNCHRONOUS;
//  init->flags |= PANTHEIOS_BE_SPEECH_F_PURGE_BEFORE_SPEAK;
//  init->flags |= PANTHEIOS_BE_SPEECH_F_SPEAK_PUNCTUATION;
//  init->flags |= PANTHEIOS_BE_SPEECH_F_SYNCHRONOUS_ON_CRITICAL;
}

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
    DWORD   shortPause = 1250;

    try
    {
//      pantheios::log(pantheios::notice, "Hello");

//      ::Sleep(shortPause);

//      pantheios::log(pantheios::notice(2), "Hello");

//      ::Sleep(shortPause);

//      pantheios::log(pantheios::notice(2), "Hello, boys. This is your daddy, telling you to turn around and eat your dinner. Now!");

//      ::Sleep(shortPause);

        short           s   = SHRT_MIN;
        unsigned short  us  = USHRT_MAX;
        int             i   = INT_MIN;
        unsigned int    ui  = UINT_MAX;
        long            l   = LONG_MIN;
        unsigned long   ul  = ULONG_MAX;

#if 0
        // Log a short in decimal; Output: "s: [-32768]"

        pantheios::log_NOTICE("s: [", pantheios::integer(s), "]");

        ::Sleep(shortPause);

        // Log a unsigned short as hexadecimal; Output: "us: [ffff]"

        pantheios::log_NOTICE("us: [", pantheios::integer(us, pantheios::fmt::hex), "]");

        ::Sleep(shortPause);

        // Log an int, into a width of 20; Output: "i: [-2147483648         ]"

        pantheios::log_NOTICE("i: [", pantheios::integer(i, -20), "]");

        ::Sleep(shortPause);

        // Log an unsigned int as hexadecimal with 0x prefix; Output: "ui: [0xffffffff]"

        pantheios::log_NOTICE("ui: [", pantheios::integer(ui, pantheios::fmt::hex | pantheios::fmt::zeroXPrefix), "]");

        ::Sleep(shortPause);

        // Log a long; Output: "l: [         -2147483648]"

        pantheios::log_NOTICE("l: [", pantheios::integer(l, 20), "]");

        ::Sleep(shortPause);

        // Log an unsigned long; Output: "ul: [4294967295]"

        pantheios::log_NOTICE("ul: [", pantheios::integer(ul), "]");

        ::Sleep(shortPause);
#else /* ? 0 */

        pantheios::log_NOTICE("Hi!");

        ::Sleep(shortPause);

        pantheios::log_NOTICE("This is your logger, calling.");

        ::Sleep(shortPause);

        pantheios::log_NOTICE("Here come some diagnostic logging statements ...");

        ::Sleep(shortPause);

#endif /* 0 */

        pantheios::log_DEBUG("just being pedantic");

        ::Sleep(shortPause);

        pantheios::log_INFORMATIONAL("you can ignore this");

        ::Sleep(shortPause);

        pantheios::log_NOTICE("this is noteworthy");

        ::Sleep(shortPause);

        pantheios::log_WARNING("there may be a problem");

        ::Sleep(shortPause);

        pantheios::log_ERROR("there is a problem");

        ::Sleep(shortPause);

        pantheios::log_CRITICAL("there is a serious problem");

        ::Sleep(shortPause);

        pantheios::log_ALERT("there is a very serious problem");

        ::Sleep(shortPause);

        pantheios::log_EMERGENCY("aargh! I'm operating in contradiction to my design!");

        ::Sleep(90000);

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

/* ////////////////////////////////////////////////////////////////////// */
