/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.be.file.rolling/test.scratch.be.file.rolling.cpp
 *
 * Purpose:     Implementation file for the test.scratch.be.file.rolling project.
 *
 * Created:     24th October 2007
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



#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/args.hpp>
#include <pantheios/inserters/integer.hpp>

#include <pantheios/backends/bec.file.h>

/* STLSoft Header Files */
#include <platformstl/filesystem/file_lines.hpp>
#include <platformstl/filesystem/filesystem_traits.hpp>
#include <platformstl/filesystem/path.hpp>

/* Standard C++ Header Files */
#include <exception>

/* Standard C Header Files */
#include <stdio.h>
#include <stdlib.h>

#if defined(_MSC_VER) && \
    defined(_DEBUG)
# include <crtdbg.h>
#endif /* _MSC_VER) && _DEBUG */

/* /////////////////////////////////////////////////////////////////////////
 * Constants & definitions
 */

//static const char FILENAME_REMOTE[]   =   "rolling-log.test.remote";

static const int ENTRY_QUANTUM = 10;

/* /////////////////////////////////////////////////////////////////////////
 * Function implementations
 */

PANTHEIOS_CALL(void) pantheios_be_file_getAppInit(int backEndId, pan_be_file_init_t* init) /* throw() */
{
    switch(backEndId)
    {
        case    PANTHEIOS_BEID_REMOTE:
            init->roll.entryCount = ENTRY_QUANTUM;
            break;
        default:
            break;
    }
}

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.scratch.be.file.rolling");

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

typedef platformstl::filesystem_traits<char>    fs_traits_t;
typedef platformstl::basic_path<char>           path_t;
typedef platformstl::file_lines                 file_lines_t;

/* /////////////////////////////////////////////////////////////////////////
 * Forward declarations
 */

static void test_ROLL_ON_ENTRIES();

/* ////////////////////////////////////////////////////////////////////// */

static int main_(int argc, char** argv)
{

    /* . */
    pantheios::log_DEBUG("main_(", pantheios::args(argc, argv), ")");

    test_ROLL_ON_ENTRIES();

    pantheios::log_DEBUG("exiting main_()");

    return EXIT_SUCCESS;
}


/* ////////////////////////////////////////////////////////////////////// */

static void test_ROLL_ON_ENTRIES()
{
    // Create a file, with rolling quantum of

    unsigned    flags   =   0
                        |   PANTHEIOS_BE_FILE_F_DISCARD_CACHED_CONTENTS
                        |   PANTHEIOS_BE_FILE_F_TRUNCATE
                        |   PANTHEIOS_BE_FILE_F_ROLL_ON_ENTRY_COUNT
                        |   0;

    pantheios_be_file_setFilePath("rolling-log.test.remote", flags, flags, PANTHEIOS_BEID_REMOTE);

    const int   NUM_ENTRIES = 104;

    { for(size_t i = 0; i < NUM_ENTRIES; ++i)
    {
        pantheios::log_NOTICE("stmt #", pantheios::integer(i));
    }}

    pantheios_be_file_setFilePath(NULL, PANTHEIOS_BEID_REMOTE);

    { for(size_t i = 0; i < NUM_ENTRIES; i += ENTRY_QUANTUM)
    {
        size_t  index   =   i / ENTRY_QUANTUM;

        path_t  path    =   "rolling-log.test.remote";

        path /= pantheios::integer(index).c_str();


        file_lines_t    remote_lines(path);
        size_t          numRemote   =   remote_lines.size();

        STLSOFT_ASSERT(index < (NUM_ENTRIES / ENTRY_QUANTUM) ? ENTRY_QUANTUM == numRemote : (NUM_ENTRIES % ENTRY_QUANTUM) == numRemote);

        fs_traits_t::delete_file(path.c_str());
        fs_traits_t::get_last_error();
    }}
}

/* ////////////////////////////////////////////////////////////////////// */

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
        pantheios::log_ALERT("Unexpected general error: ", x, ". Application terminating");

        res = EXIT_FAILURE;
    }
    catch(...)
    {
        pantheios::logputs(pantheios::emergency, "Unhandled unknown error");

        res = EXIT_FAILURE;
    }

#if defined(_MSC_VER) && \
    defined(_DEBUG)
    _CrtMemDumpAllObjectsSince(&memState);
#endif /* _MSC_VER) && _DEBUG */

    return res;
}

/* ////////////////////////////////////////////////////////////////////// */
