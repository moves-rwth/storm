/* /////////////////////////////////////////////////////////////////////////
 * File:        test/component/test.component.be.file.threading/test.component.be.file.threading.cpp
 *
 * Purpose:     Implementation file for the test.component.be.file.threading project.
 *
 * Created:     3rd July 2009
 * Updated:     10th January 2011
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2009-2011, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

/* Pantheios Header Files */
#include <pantheios/pan.hpp>
#include <pantheios/backends/be.N.h>
#include <pantheios/backends/bec.console.h>
#include <pantheios/backends/bec.file.h>
#include <pantheios/frontends/fe.N.h>
#include <pantheios/inserters/args.hpp>
#include <pantheios/inserters/blob.hpp>
#include <pantheios/inserters/exception.hpp>
#include <pantheios/inserters/integer.hpp>

/* STLSoft Header Files */
#include <stlsoft/string/split_functions.hpp>
#include <stlsoft/string/string_view.hpp>
#include <platformstl/filesystem/file_lines.hpp>
#include <platformstl/filesystem/filesystem_traits.hpp>
#include <platformstl/synch/sleep_functions.h>

/* Standard C++ Header Files */
#include <exception>

#if defined(PLATFORMSTL_OS_IS_UNIX)

 /* PThreads Header Files */
# include <pthread.h>
# include <sched.h>

 /* UNIXEm Header Files */
# if defined(_WIN32) || \
     defined(_WIN64)
#  include <unixem/unixem.h>
# endif /* Win32 || Win64 */

#elif defined(PLATFORMSTL_OS_IS_WINDOWS)

# define WINSTL_ERROR_DESC_NO_IMPLICIT_CONVERSION
# include <winstl/error/error_desc.hpp>

# include <windows.h>

#else /* ? OS */
# error Operating system not discriminated
#endif /* OS */

/* Standard C Header Files */
#include <stdio.h>
#include <stdlib.h>

#if defined(_MSC_VER) && \
    defined(_DEBUG)
# define USE_MSC_VER_CRT_MEM_CHECKS
# include <crtdbg.h>
#endif /* _MSC_VER) && _DEBUG */

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

/* /////////////////////////////////////////////////////////////////////////
 * Character encoding
 */

#ifdef PANTHEIOS_USE_WIDE_STRINGS

# define XTESTS_TEST_STRING_EQUAL       XTESTS_TEST_WIDE_STRING_EQUAL

#else /* ? PANTHEIOS_USE_WIDE_STRINGS */

# define XTESTS_TEST_STRING_EQUAL       XTESTS_TEST_MULTIBYTE_STRING_EQUAL

#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* /////////////////////////////////////////////////////////////////////////
 * Constants and definitions
 */

#ifdef _DEBUG
const size_t    LOG_ITERATIONS      =   10;
const size_t    SET_PATH_DELAY      =   1000 * 100;
#else /* ? _DEBUG */
 /* Surprisingly, Windows takes massively more time than UNIX to write
  * out all the entries, so we need different ITERATIONS count otherwise
  * Windows users running the component tests may think the process has
  * hung and kill it
  */
# if defined(PLATFORMSTL_OS_IS_WINDOWS)
const size_t    LOG_ITERATIONS      =   5000;
# else /* ? OS */
const size_t    LOG_ITERATIONS      =   100000;
# endif /* OS */
const size_t    SET_PATH_DELAY      =   1000 * 1000 * 10;
#endif /* _DEBUG */

const PAN_CHAR_T LOG_FILE_NAME[]    =   PSTR("test.component.be.file.threading.log");

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

#if defined(PLATFORMSTL_OS_IS_UNIX)
static pthread_mutex_t  s_mx            =   PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t   s_cv            =   PTHREAD_COND_INITIALIZER;
static int              s_activeThreads =   0;
#endif /* OS */

static int              s_showNotices   =   true;

extern "C" const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[]    =   PSTR("test.component.be.file.threading");

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

#if defined(PLATFORMSTL_OS_IS_UNIX)

typedef pthread_t   thread_handle_t;

#elif defined(PLATFORMSTL_OS_IS_WINDOWS)

typedef HANDLE      thread_handle_t;

#else /* ? OS */

# error Operating system not discriminated

#endif /* OS */

typedef platformstl::filesystem_traits<PAN_CHAR_T>  fs_traits_t;
typedef platformstl::basic_file_lines<PAN_CHAR_T>   lines_t;
typedef stlsoft::basic_string_view<PAN_CHAR_T>          string_view_t;

/* /////////////////////////////////////////////////////////////////////////
 * Forward declarations
 */

#if defined(PLATFORMSTL_OS_IS_UNIX)

static void* thread_proc(void*);

#elif defined(PLATFORMSTL_OS_IS_WINDOWS)

static DWORD WINAPI thread_proc(void*);

#else /* ? OS */

# error Operating system not discriminated

#endif /* OS */

/* /////////////////////////////////////////////////////////////////////////
 * Pantheios configuration
 */

enum
{
    beid_console    =   1,
    beid_file       =   2,
};

extern "C"
{

pan_fe_N_t PAN_FE_N_SEVERITY_CEILINGS[] =
{
#ifdef _DEBUG
    { beid_console, PANTHEIOS_SEV_NOTICE },
#else /* ? _DEBUG */
    { beid_console, PANTHEIOS_SEV_NOTICE },
#endif /* _DEBUG */
    { beid_file, PANTHEIOS_SEV_INFORMATIONAL },

    PANTHEIOS_FE_N_TERMINATOR_ENTRY(PANTHEIOS_SEV_INFORMATIONAL)
};

pan_be_N_t PAN_BE_N_BACKEND_LIST[] =
{
    PANTHEIOS_BE_N_STDFORM_ENTRY(beid_console, pantheios_be_console, PANTHEIOS_BE_N_F_IGNORE_NONMATCHED_CUSTOM28_ID),
    PANTHEIOS_BE_N_STDFORM_ENTRY(beid_file, pantheios_be_file, PANTHEIOS_BE_N_F_ID_MUST_MATCH_CUSTOM28),

    PANTHEIOS_BE_N_TERMINATOR_ENTRY
};

} // extern "C"

/* ////////////////////////////////////////////////////////////////////// */

static int main_(int /*argc*/, char** /*argv*/)
{
    thread_handle_t   threads[32];

    pan::log_INFORMATIONAL(PSTR("main(): creating "), pan::integer(STLSOFT_NUM_ELEMENTS(threads)), PSTR(" threads"));

    { for(size_t i = 0; i < STLSOFT_NUM_ELEMENTS(threads); ++i)
    {
        void* arg = NULL;

#if defined(PLATFORMSTL_OS_IS_UNIX)

        pthread_mutex_lock(&s_mx);

        if(0 != pthread_create(&threads[i], NULL, thread_proc, arg))
        {
            pan::log_ALERT(PSTR("Failed to create thread "), pan::integer(i));

            pthread_mutex_unlock(&s_mx);

            return EXIT_FAILURE;
        }
        else
        {
            pthread_detach(threads[i]);
            ++s_activeThreads;
        }

        pthread_mutex_unlock(&s_mx);
#elif defined(PLATFORMSTL_OS_IS_WINDOWS)

        // NOTE: We are calling the Windows API function CreateThread()
        // directly, rather than going through beginthreadex() (or
        // equivalent), which is not the correct thing to do in a real
        // program, as CRT resources can be leaked.
        //
        // We do it this way in this case simply for portability of the
        // test program

        DWORD   threadId;

        threads[i] = CreateThread(NULL, 0, thread_proc, arg, 0, &threadId);

        if(NULL == threads[i])
        {
            winstl::error_desc  err;

            pan::log_ALERT(PSTR("Failed to create thread "), pan::integer(i), PSTR(": "), err);

            return EXIT_FAILURE;
        }

#else /* ? OS */
# error Operating system not discriminated
#endif /* 0 */
    }}

    s_showNotices &&
    pan::log_NOTICE(PSTR("main(): all "), pan::integer(STLSOFT_NUM_ELEMENTS(threads)), PSTR(" threads started"));

    platformstl::micro_sleep(SET_PATH_DELAY);

    s_showNotices &&
    pan::log_NOTICE(PSTR("main(): setting log file path"));

    pantheios_be_file_setFilePath(LOG_FILE_NAME, PANTHEIOS_BE_FILE_F_TRUNCATE, PANTHEIOS_BE_FILE_F_TRUNCATE, 0);

    s_showNotices &&
    pan::log_NOTICE(PSTR("main(): waiting for threads to complete; this could take several minutes"));

#if defined(PLATFORMSTL_OS_IS_UNIX)

    for(;;)
    {
        pthread_mutex_lock(&s_mx);
        if(0 == s_activeThreads)
        {
            pthread_mutex_unlock(&s_mx);
            break;
        }
        else
        {
            pthread_cond_wait(&s_cv, &s_mx);
        }
        pthread_mutex_unlock(&s_mx);
    }

#elif defined(PLATFORMSTL_OS_IS_WINDOWS)

    ::WaitForMultipleObjects(STLSOFT_NUM_ELEMENTS(threads), &threads[0], true, INFINITE);

#else /* ? OS */
# error Operating system not discriminated
#endif /* 0 */

    s_showNotices &&
    pan::log_NOTICE(PSTR("main(): all "), pan::integer(STLSOFT_NUM_ELEMENTS(threads)), PSTR(" threads completed"));

    pantheios_be_file_setFilePath(NULL);

    int retVal = EXIT_SUCCESS;

    lines_t lines(LOG_FILE_NAME);

    { for(size_t i = 0; i != lines.size(); ++i)
    {
        lines_t::value_type const&  line = lines[i];

        string_view_t               scratch1;
        string_view_t               scratch2;
                                    
        string_view_t               prefix;
        string_view_t               left;
        string_view_t               middle;
        string_view_t               right;

        if( !stlsoft::split(line, PSTR('|'), prefix, scratch1) ||
            !stlsoft::split(scratch1, PSTR('|'), left, scratch2) ||
            !stlsoft::split(scratch2, PSTR('|'), middle, right))
        {
            pan::log_CRITICAL(PSTR("line does not have required format: ["), pan::integer(i), PSTR(": "), line, PSTR("]"));

            retVal = EXIT_FAILURE;
        }
        else
        {
            if(left != right)
            {
                pan::log_CRITICAL(PSTR("line prefix and suffix do not match: ["), pan::integer(i), PSTR(": "), line, PSTR("]"));

                retVal = EXIT_FAILURE;
            }
        }
    }}

    if(EXIT_SUCCESS == retVal)
    {
        s_showNotices &&
        pan::log_NOTICE(PSTR("all lines logged to file correctly"));
    }
    else
    {
        pan::log_CRITICAL(PSTR("multi-threaded use of be.file has been defective; please report to the Pantheios project"));
    }

    return retVal;
}

#ifdef USE_MSC_VER_CRT_MEM_CHECKS
int main__(int argc, char** argv)
{
    _CrtMemState memState;

    _CrtMemCheckpoint(&memState);

    int r = main_(argc, argv);

    _CrtMemDumpAllObjectsSince(&memState);

    return r;
}
#else /* ? USE_MSC_VER_CRT_MEM_CHECKS */
# define main__     main_
#endif /* USE_MSC_VER_CRT_MEM_CHECKS */


int main(int argc, char** argv)
{
    if(2 == argc)
    {
        char const* arg = argv[1];

        if(0 == ::strncmp(arg, "--verbosity=", 12))
        {
            int verbosity = atoi(arg + 12);

            if(verbosity < 0)
            {
                verbosity = 0;
            }

            switch(verbosity)
            {
                case    0:
                case    1:
                    s_showNotices = false;
                    break;
                default:
                    s_showNotices = true;
                    break;
            }
        }
        else
        {
            fprintf(stderr, "unrecognised argument: '%s'\n", arg);

            return EXIT_FAILURE;
        }
    }

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* !STLSOFT_CF_EXCEPTION_SUPPORT */

        int r = main_(argc, argv);

        fs_traits_t::unlink_file(LOG_FILE_NAME);

        return r;

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::exception& x)
    {
        pan::log_ALERT(PSTR("Unexpected general error: "), pantheios::exception(x), PSTR(". Application terminating"));
    }
    catch(...)
    {
        pan::logputs(pan::emergency, PSTR("Unhandled unknown error"));
    }
#endif /* !STLSOFT_CF_EXCEPTION_SUPPORT */

    fs_traits_t::unlink_file(LOG_FILE_NAME);

    return EXIT_FAILURE;
}

/* ////////////////////////////////////////////////////////////////////// */

#if defined(PLATFORMSTL_OS_IS_UNIX)
static void* thread_proc(void*)
#elif defined(PLATFORMSTL_OS_IS_WINDOWS)
static DWORD WINAPI thread_proc(void*)
#endif /* OS */
{
#if defined(PLATFORMSTL_OS_IS_UNIX)
    thread_handle_t self = pthread_self();
#elif defined(PLATFORMSTL_OS_IS_WINDOWS)
    thread_handle_t self = GetCurrentThread();
#endif /* OS */

    pan::log_INFORMATIONAL(PSTR("thread_proc("), pan::blob(&self, sizeof(self)), PSTR("): entering"));

    // TODO: Do some threading stuff

    { for(size_t i = 0; i != LOG_ITERATIONS; ++i)
    {
        pan::log(pan::informational(beid_file), PSTR("|"), pan::integer(i), PSTR("|"), PSTR("this is"), PSTR(" "), PSTR("multipart log "), PSTR("statement"), PSTR("|"), pan::integer(i));
    }}

#if defined(PLATFORMSTL_OS_IS_UNIX)
    pthread_mutex_lock(&s_mx);
    --s_activeThreads;
    pthread_cond_signal(&s_cv);
    pthread_mutex_unlock(&s_mx);
#endif /* OS */

    pan::log_INFORMATIONAL(PSTR("thread_proc("), pan::blob(&self, sizeof(self)), PSTR("): exiting"));

    return 0;
}

/* ////////////////////////////////////////////////////////////////////// */

#if defined(PLATFORMSTL_OS_IS_UNIX) && \
    defined(_WIN32) && \
    defined(_STLSOFT_FORCE_ANY_COMPILER)
# include <windows.h>
extern "C" void syslog(char const* s)
{
    OutputDebugStringA(s);
}
#endif

/* ///////////////////////////// end of file //////////////////////////// */
