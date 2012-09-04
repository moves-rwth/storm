/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.fe.WindowsRegistry.controller/test.scratch.fe.WindowsRegistry.controller.cpp
 *
 * Purpose:     Implementation file for the test.scratch.fe.WindowsRegistry.controller project.
 *
 * Created:     2nd December 2007
 * Updated:     19th December 2008
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2007-2008, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS


/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>

/* PlatformSTL Header Files */
#include <platformstl/platformstl.hpp>


/* Standard C++ Header Files */
#include <exception>
#if 0
#include <algorithm>
#include <iterator>
#include <list>
#include <string>
#include <vector>
#endif /* 0 */

/* Standard C Header Files */
#include <stdio.h>
#include <stdlib.h>

#if defined(_MSC_VER) && \
    defined(_DEBUG)
# include <crtdbg.h>
#endif /* _MSC_VER) && _DEBUG */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

namespace pantheios
{
namespace control
{

    namespace fe
    {

        class WindowsRegistryController
        {
        public:
            typedef WindowsRegistryController   class_type;

        public:
            explicit WindowsRegistryController(char const *)
            {
            }
        };

    } // namespace fe

} // namespace control
} // namespace pantheios

/* ////////////////////////////////////////////////////////////////////// */

static int main_(int argc, char** argv)
{
    int bVerbose = true;

    { for(int i = 1; i < argc; ++i)
    {
        char const* const arg = argv[i];

        if(arg[0] == '-')
        {
        }
        else
        {
        }
    }}

    /* . */
//    pantheios::control::fe::WindowsRegistryController::;

    return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
    int             res;

#if defined(_MSC_VER) && \
    defined(_DEBUG)
    _CrtMemState    memState;
#endif /* _MSC_VER && _MSC_VER */

#if defined(_MSC_VER) && \
    defined(_DEBUG)
    _CrtMemCheckpoint(&memState);
#endif /* _MSC_VER && _MSC_VER */

    try
    {
        res = main_(argc, argv);
    }
    catch(std::exception &x)
    {
        fprintf(stderr, "Unhandled error: %s\n", x.what());

        res = EXIT_FAILURE;
    }
    catch(...)
    {
        fprintf(stderr, "Unhandled unknown error\n");

        res = EXIT_FAILURE;
    }

#if defined(_MSC_VER) && \
    defined(_DEBUG)
    _CrtMemDumpAllObjectsSince(&memState);
#endif /* _MSC_VER) && _DEBUG */

    return res;
}

/* ////////////////////////////////////////////////////////////////////// */
