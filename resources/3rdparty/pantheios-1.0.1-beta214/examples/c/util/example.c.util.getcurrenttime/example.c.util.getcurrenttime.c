/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/c/util/example.c.util.getcurrenttime/example.c.util.getcurrenttime.c
 *
 * Purpose:     Implementation file for the example.c.util.getcurrenttime project.
 *
 * Created:     30th August 2008
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
#include <pantheios/util/time/currenttime.h>

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
    STLSOFT_SUPPRESS_UNUSED(argc);
    STLSOFT_SUPPRESS_UNUSED(argv);

    /* Using local time with the default representation for the platform */
    {
        PAN_CHAR_T          buff[101];
        pan_beutil_time_t   tm;
        size_t              n;

        tm.capacity     =   STLSOFT_NUM_ELEMENTS(buff);
        tm.len          =   0;
        tm.str          =   &buff[0];
        tm.strftimeFmt  =   NULL;

        n = pantheios_util_getCurrentTime(&tm, 0);

        printf("time with 0 flags: %.*s\n", (int)n, buff);
    }

    /* Using the current user time, with UNIX format */
    {
        PAN_CHAR_T          buff[101];
        pan_beutil_time_t   tm;
        int                 flags   =   0
                                    |   PANTHEIOS_GETCURRENTTIME_F_USE_UNIX_FORMAT
                                    ;
        size_t              n;

        tm.capacity     =   STLSOFT_NUM_ELEMENTS(buff);
        tm.len          =   0;
        tm.str          =   &buff[0];
        tm.strftimeFmt  =   NULL;

        n = pantheios_util_getCurrentTime(&tm, flags);

        printf("time, with UNIX format: %.*s\n", (int)n, buff);
    }

    /* Using system time with the default representation for the platform */
    {
        PAN_CHAR_T          buff[101];
        pan_beutil_time_t   tm;
        int                 flags   =   0
                                    |   PANTHEIOS_GETCURRENTTIME_F_USE_SYSTEM_TIME
                                    ;
        size_t              n;

        tm.capacity     =   STLSOFT_NUM_ELEMENTS(buff);
        tm.len          =   0;
        tm.str          =   &buff[0];
        tm.strftimeFmt  =   NULL;

        n = pantheios_util_getCurrentTime(&tm, flags);

        printf("time, using system time: %.*s\n", (int)n, buff);
    }

    /* Using local time with the default representation for the platform, favouring speed */
    {
        PAN_CHAR_T          buff[101];
        pan_beutil_time_t   tm;
        int                 flags   =   0
                                    |   PANTHEIOS_GETCURRENTTIME_F_USE_SYSTEM_TIME
                                    |   PANTHEIOS_GETCURRENTTIME_F_FAVOUR_SPEED
                                    ;
        size_t              n;

        tm.capacity     =   STLSOFT_NUM_ELEMENTS(buff);
        tm.len          =   0;
        tm.str          =   &buff[0];
        tm.strftimeFmt  =   NULL;

        n = pantheios_util_getCurrentTime(&tm, flags);

        printf("time, using system time, favouring speed: %.*s\n", (int)n, buff);
    }

    /* Using local time with the default representation for the platform, favouring accuracy */
    {
        PAN_CHAR_T          buff[101];
        pan_beutil_time_t   tm;
        int                 flags   =   0
                                    |   PANTHEIOS_GETCURRENTTIME_F_USE_SYSTEM_TIME
                                    |   PANTHEIOS_GETCURRENTTIME_F_FAVOUR_ACCURACY
                                    ;
        size_t              n;

        tm.capacity     =   STLSOFT_NUM_ELEMENTS(buff);
        tm.len          =   0;
        tm.str          =   &buff[0];
        tm.strftimeFmt  =   NULL;

        n = pantheios_util_getCurrentTime(&tm, flags);

        printf("time, using system time, favouring accuracy: %.*s\n", (int)n, buff);
    }

    /* Using local time with the default representation for the platform, microsecond resolution, favouring speed */
    {
        PAN_CHAR_T          buff[101];
        pan_beutil_time_t   tm;
        int                 flags   =   0
                                    |   PANTHEIOS_GETCURRENTTIME_F_TIME_RES_MICROSECS
                                    |   PANTHEIOS_GETCURRENTTIME_F_FAVOUR_SPEED
                                    ;
        size_t              n;

        tm.capacity     =   STLSOFT_NUM_ELEMENTS(buff);
        tm.len          =   0;
        tm.str          =   &buff[0];
        tm.strftimeFmt  =   NULL;

        n = pantheios_util_getCurrentTime(&tm, flags);

        printf("time, showing microseconds, favouring speed: %.*s\n", (int)n, buff);
    }

    /* Using local time with the default representation for the platform, microsecond resolution, favouring accuracy */
    {
        PAN_CHAR_T          buff[101];
        pan_beutil_time_t   tm;
        int                 flags   =   0
                                    |   PANTHEIOS_GETCURRENTTIME_F_TIME_RES_MICROSECS
                                    |   PANTHEIOS_GETCURRENTTIME_F_FAVOUR_ACCURACY
                                    ;
        size_t              n;

        tm.capacity     =   STLSOFT_NUM_ELEMENTS(buff);
        tm.len          =   0;
        tm.str          =   &buff[0];
        tm.strftimeFmt  =   NULL;

        n = pantheios_util_getCurrentTime(&tm, flags);

        printf("time, showing microseconds, favouring accuracy: %.*s\n", (int)n, buff);
    }

    return EXIT_SUCCESS;
}

/* ///////////////////////////// end of file //////////////////////////// */
