/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.util.onbailout/test.scratch.util.onbailout.cpp
 *
 * Purpose:     Implementation file for the test.scratch.util.onbailout project.
 *
 * Created:     21st June 2005
 * Updated:     6th August 2012
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              www:        http://www.synesis.com.au/software
 *
 *              This source code is placed into the public domain 2005
 *              by Synesis Software Pty Ltd. There are no restrictions
 *              whatsoever to your use of the software. 
 *
 *              This source code is provided by Synesis Software Pty Ltd "as is"
 *              and any warranties, whether expressed or implied, including, but
 *              not limited to, the implied warranties of merchantability and
 *              fitness for a particular purpose are disclaimed. In no event
 *              shall the Synesis Software Pty Ltd be liable for any direct,
 *              indirect, incidental, special, exemplary, or consequential
 *              damages (including, but not limited to, procurement of
 *              substitute goods or services; loss of use, data, or profits; or
 *              business interruption) however caused and on any theory of
 *              liability, whether in contract, strict liability, or tort
 *              (including negligence or otherwise) arising in any way out of
 *              the use of this software, even if advised of the possibility of
 *              such damage. 
 *
 *              Neither the name of Synesis Software Pty Ltd nor the names of
 *              any subdivisions, employees or agents of Synesis Software Pty
 *              Ltd, nor the names of any other contributors to this software
 *              may be used to endorse or promote products derived from this
 *              software without specific prior written permission. 
 *
 * ////////////////////////////////////////////////////////////////////// */


/* This inclusion required for suppressing warnings during NoX (No eXception-support) configurations. */
#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

/* Pantheios Header Files */
#include <pantheios/pantheios.h>
#include <pantheios/init_codes.h>

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>

/* Standard C++ Header Files */
#include <exception>
#include <iostream>
#include <string>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;

/* Standard C Header Files */
#include <stdio.h>

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>


PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.scratch.util.onbailout");

static int main_(int /* argc */, char ** /*argv*/)
{
#if 0
    pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, "abc");

    pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, "abc", PANTHEIOS_FE_PROCESS_IDENTITY);

    pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, "abc", PANTHEIOS_FE_PROCESS_IDENTITY, pantheios::getInitErrorString(PANTHEIOS_INIT_RC_CANNOT_CREATE_TSS_INDEX));

// no longer valid  pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, "abc", PANTHEIOS_FE_PROCESS_IDENTITY, (char const*)(pantheios::uint16_t)PANTHEIOS_INIT_RC_CANNOT_CREATE_TSS_INDEX);


    {
        std::string message(1200, 'm');
        std::string qualifier(1200, 'q');

        pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, message.c_str(), PANTHEIOS_FE_PROCESS_IDENTITY);

        pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, message.c_str(), PANTHEIOS_FE_PROCESS_IDENTITY, qualifier.c_str());
    }

    {
        std::string message(2000, 'm');
        std::string qualifier(100, 'q');

        pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, message.c_str(), PANTHEIOS_FE_PROCESS_IDENTITY);

        pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, message.c_str(), PANTHEIOS_FE_PROCESS_IDENTITY, qualifier.c_str());
    }

    {
        std::string message(100, 'm');
        std::string qualifier(2000, 'q');

        pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, message.c_str(), PANTHEIOS_FE_PROCESS_IDENTITY);

        pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, message.c_str(), PANTHEIOS_FE_PROCESS_IDENTITY, qualifier.c_str());
    }

    {
        std::string message(2500, 'm');
        std::string qualifier(1200, 'q');

        pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, message.c_str(), PANTHEIOS_FE_PROCESS_IDENTITY);

        pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, message.c_str(), PANTHEIOS_FE_PROCESS_IDENTITY, qualifier.c_str());
    }

    {
        std::string message(2500, 'm');
        std::string qualifier(100, 'q');

        pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, message.c_str(), PANTHEIOS_FE_PROCESS_IDENTITY);

        pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, message.c_str(), PANTHEIOS_FE_PROCESS_IDENTITY, qualifier.c_str());
    }

    {
        std::string message(2500, 'm');

        pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, message.c_str(), PANTHEIOS_FE_PROCESS_IDENTITY);

        pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, message.c_str(), PANTHEIOS_FE_PROCESS_IDENTITY, "");
    }

#endif /* 0 */

    const size_t INCREMENT  =   150;

    STLSOFT_STATIC_ASSERT(0 == (3000 % INCREMENT));

    { for(size_t i = 0; i != 3000; i += INCREMENT)
    {
        { for(size_t j = 0; j != 3000; j += INCREMENT)
        {
            if(0 == i)
            {
                std::string qualifier(j, 'q');

                pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, NULL, PANTHEIOS_FE_PROCESS_IDENTITY, qualifier.c_str());
            }
            else if(0 == j)
            {
                std::string message(i, 'm');

                pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, message.c_str(), PANTHEIOS_FE_PROCESS_IDENTITY, "");
            }
            else
            {
                std::string message(i, 'm');
                std::string qualifier(j, 'q');

                pantheios::util::onBailOut(PANTHEIOS_SEV_DEBUG, message.c_str(), PANTHEIOS_FE_PROCESS_IDENTITY, qualifier.c_str());
            }
        }}
    }}


    return EXIT_SUCCESS;
}


int main(int argc, char *argv[])
{
#if 0
    for(;;) {}
#endif /* 0 */

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return main_(argc, argv);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::exception &x)
    {
        fprintf(stderr, "Unhandled error: %s\n", x.what());
    }
    catch(...)
    {
        pantheios::puts(pantheios::emergency, "Unhandled unknown error");
    }

    return EXIT_FAILURE;
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

