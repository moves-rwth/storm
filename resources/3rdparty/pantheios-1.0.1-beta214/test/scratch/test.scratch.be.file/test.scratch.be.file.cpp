/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.be.file/test.scratch.be.file.cpp
 *
 * Purpose:     Implementation file for the test.scratch.be.file project.
 *
 * Created:     27th November 2006
 * Updated:     27th December 2010
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2006-2010, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/args.hpp>
#include <pantheios/backends/bec.file.h>

/* STLSoft Header Files */
#include <platformstl/filesystem/file_lines.hpp>
#include <platformstl/filesystem/filesystem_traits.hpp>

/* Standard C++ Header Files */
#include <exception>

/* Standard C Header Files */
#include <stdio.h>
#include <stdlib.h>

#if defined(_MSC_VER) && \
    defined(_DEBUG)
# include <crtdbg.h>
#endif /* _MSC_VER) && _DEBUG */


#define PSTR        PANTHEIOS_LITERAL_STRING

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PSTR("test.scratch.be.file");

/* /////////////////////////////////////////////////////////////////////////
 * Constants & definitions
 */

static const char   FILENAME_LOCAL[]    =   "log.test.local";
static const char   FILENAME_REMOTE[]   =   "log.test.remote";

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

typedef platformstl::filesystem_traits<char>    fs_traits_t;
typedef platformstl::file_lines                 file_lines_t;

/* ////////////////////////////////////////////////////////////////////// */

static int main_(int argc, char **argv)
{
#ifndef PANTHEIOS_USE_WIDE_STRINGS
    pantheios::log_DEBUG("main_(", pantheios::args(argc, argv), ")");
#endif

    {
        unsigned    flags = PANTHEIOS_BE_FILE_F_DISCARD_CACHED_CONTENTS | PANTHEIOS_BE_FILE_F_TRUNCATE;

        int res1 = pantheios_be_file_setFilePath(PSTR("test-%T-%D.log"), flags, flags, PANTHEIOS_BEID_LOCAL);

        pantheios_be_file_flush(PANTHEIOS_BEID_LOCAL);

        pantheios::log_NOTICE(PSTR("stmt 1"));
    }

#ifndef PANTHEIOS_USE_WIDE_STRINGS
    {
        pantheios::log_NOTICE("stmt 1");
        unsigned    flags = PANTHEIOS_BE_FILE_F_DISCARD_CACHED_CONTENTS | PANTHEIOS_BE_FILE_F_TRUNCATE;

        int res1 = pantheios_be_file_setFilePath(FILENAME_LOCAL, flags, flags, PANTHEIOS_BEID_LOCAL);

        pantheios::log_NOTICE("stmt 2");
        pantheios::log_NOTICE("stmt 3");

        int res2 = pantheios_be_file_setFilePath(FILENAME_REMOTE, flags, flags, PANTHEIOS_BEID_REMOTE);

        pantheios::log_NOTICE("stmt 4");
        pantheios::log_NOTICE("stmt 5");

        pantheios_be_file_setFilePath(NULL, PANTHEIOS_BEID_LOCAL);
        pantheios_be_file_setFilePath(NULL, PANTHEIOS_BEID_REMOTE);

        pantheios::log_NOTICE("stmt 6");
        pantheios::log_NOTICE("stmt 7");

        file_lines_t    local_lines(FILENAME_LOCAL);
        file_lines_t    remote_lines(FILENAME_REMOTE);

        const size_t    numLocal    =   local_lines.size();
        const size_t    numRemote   =   remote_lines.size();

        STLSOFT_ASSERT(4 == numLocal);
        STLSOFT_ASSERT(2 == numRemote);

        fs_traits_t::delete_file(FILENAME_LOCAL);
        fs_traits_t::get_last_error();
        fs_traits_t::delete_file(FILENAME_REMOTE);
        fs_traits_t::get_last_error();

        STLSOFT_SUPPRESS_UNUSED(res1);
        STLSOFT_SUPPRESS_UNUSED(res2);
    }
#endif

    pantheios::log_DEBUG(PSTR("exiting main_()"));

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
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
