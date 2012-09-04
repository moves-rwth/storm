/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/c/util/example.c.util.gethostname/example.c.util.gethostname.c
 *
 * Purpose:     Implementation file for the example.c.util.gethostname project.
 *
 * Created:     25th August 2008
 * Updated:     27th December 2010
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2008-2010, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */

/* Pantheios Header Files */
#include <pantheios/pantheios.h>
#include <pantheios/util/system/hostname.h>

#include <pantheios/internal/safestr.h>

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>
#include <platformstl/platformstl.h>
#if defined(PLATFORMSTL_OS_IS_WINDOWS)
# include <winstl/error/error_functions.h>
#endif /* OS */

/* Standard C Header Files */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER) && \
    defined(_DEBUG)
# include <crtdbg.h>
#endif /* _MSC_VER) && _DEBUG */

/* ////////////////////////////////////////////////////////////////////// */

int main(int argc, char** argv)
{
    PAN_CHAR_T  name[101];
    size_t      cch = pantheios_getHostName(&name[0], STLSOFT_NUM_ELEMENTS(name));

    /* There are three possible return values:
     *
     * 0 - indicates that no host name is available, or there was an error
     *     when retrieving it
     * 101 - the buffer is not long enough
     * [1, 101) - the name was successfully retrieved
     *
     * The only complication when processing this is that the last error
     * information is held in an operating-system-specific form. On Windows,
     * it is available via GetLastError(); otherwise use errno
     */

    if(0 == cch)
    {
        /* not available, or a problem retrieving it */

#if defined(PLATFORMSTL_OS_IS_WINDOWS)

        DWORD   error = GetLastError();
        char*   message;

        if(0 == winstl_C_format_message_alloc_a(error, NULL, &message))
        {
            /* Could not retrieve a string-form of the error, so print
             * out the error code (in case that might help)
             */

            fprintf(stderr, "could not elicit hostname: %lu\n", error);
        }
        else
        {
            /* Print out the error string, and release it.
             *
             * Note: in C++, these function names are a lot cleaner
             */

            fprintf(stderr, "could not elicit hostname: %s\n", message);

            winstl_C_format_message_free_buff_a(message);
        }

#else /* ? OS */

# ifdef PANTHEIOS_USING_SAFE_STR_FUNCTIONS
        char    err[1001];
        int     r = strerror_s(err, STLSOFT_NUM_ELEMENTS(err) - 1, errno);

        if(0 != r)
        {
            err[0] = '\0';
        }
        else
        {
            err[STLSOFT_NUM_ELEMENTS(err) - 1] = '\0';
        }
 
        fprintf(stderr, "could not elicit hostname: %s\n", err);

# else /* ? PANTHEIOS_USING_SAFE_STR_FUNCTIONS */
        fprintf(stderr, "could not elicit hostname: %s\n", strerror(errno));
# endif /* PANTHEIOS_USING_SAFE_STR_FUNCTIONS */


#endif /* OS */
    }
    else if(STLSOFT_NUM_ELEMENTS(name) == cch)
    {
        /* buffer too short */

        fprintf(stderr, "could not elicit hostname: name longer than given buffer\n");
    }
    else
    {
        /* Success. Print it out! */

        fprintf(stdout, "host name: %s\n", name);

        return EXIT_SUCCESS;
    }

    STLSOFT_SUPPRESS_UNUSED(argc);
    STLSOFT_SUPPRESS_UNUSED(argv);

    return EXIT_FAILURE;
}

/* ///////////////////////////// end of file //////////////////////////// */
