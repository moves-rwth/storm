/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/util/time/currenttime.h
 *
 * Purpose:     Functions for retrieving and formatting the current time.
 *
 * Created:     22nd August 2006
 * Updated:     10th August 2009
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2006-2009, Matthew Wilson and Synesis Software
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name(s) of Matthew Wilson and Synesis Software nor the
 *   names of any contributors may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ////////////////////////////////////////////////////////////////////// */


/** \file pantheios/util/time/currenttime.h
 *
 * [C, C++] Functions for retrieving and formatting the current time.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_UTIL_TIME_H_CURRENTTIME
#define PANTHEIOS_INCL_PANTHEIOS_UTIL_TIME_H_CURRENTTIME

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_UTIL_TIME_H_CURRENTTIME_MAJOR      2
# define PANTHEIOS_VER_PANTHEIOS_UTIL_TIME_H_CURRENTTIME_MINOR      1
# define PANTHEIOS_VER_PANTHEIOS_UTIL_TIME_H_CURRENTTIME_REVISION   1
# define PANTHEIOS_VER_PANTHEIOS_UTIL_TIME_H_CURRENTTIME_EDIT       18
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

/** Structure for use with the pantheios_util_getCurrentTime()
 *    function.
 *
 * \ingroup group__utility__backend
 */
struct pan_beutil_time_t
{
#if !defined(PANTHEIOS_NO_NAMESPACE) && \
    !defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION)
    typedef ::pantheios::pan_char_t pan_char_t;
#endif /* !PANTHEIOS_NO_NAMESPACE */

    size_t              capacity;       /*!< The capacity of the buffer in <code>str</code>. */
    size_t              len;            /*!< The length of the string written into <code>str</code>. */
    pan_char_t*         str;            /*!< The buffer into which the results are written. Its maximum length is <code>capacity</code>. */
    pan_char_t const*   strftimeFmt;    /*!< The time string format. Ignored on Windows, which uses the user/system locale time picture. May be NULL. */

#ifdef __cplusplus
    /** Initialises all structure members
     *
     * \param cap The capacity of the buffer
     * \param s The pointer to the buffer
     * \param f The time string format
     */
    pan_beutil_time_t(size_t cap, pan_char_t* s, pan_char_t const* f = NULL)
        : capacity(cap)
        , len(0)
        , str(s)
        , strftimeFmt(f)
    {}
#endif /* __cplusplus */
};
#ifndef __cplusplus
typedef struct pan_beutil_time_t pan_beutil_time_t;
#endif /* !__cplusplus */

/* /////////////////////////////////////////////////////////////////////////
 * Constants & definitions
 */

/** \def PANTHEIOS_GETCURRENTTIME_F_USE_SYSTEM_TIME
 *
 * Indicates that pantheios_util_getCurrentTime() should express the time
 * as system time (UTC)
 *
 * \ingroup group__utility__backend
 */
#define PANTHEIOS_GETCURRENTTIME_F_USE_SYSTEM_TIME          (0x00000001)

/** \def PANTHEIOS_GETCURRENTTIME_F_USE_UNIX_FORMAT
 *
 * Indicates that pantheios_util_getCurrentTime() should use UNIX format
 * regardless of actual operating system
 *
 * \ingroup group__utility__backend
 */
#define PANTHEIOS_GETCURRENTTIME_F_USE_UNIX_FORMAT          (0x00000002)

/** \def PANTHEIOS_GETCURRENTTIME_F_HIDE_DATE
 *
 * Indicates that pantheios_util_getCurrentTime() should hide the date
 * component of the date/time field
 *
 * \ingroup group__utility__backend
 */
#define PANTHEIOS_GETCURRENTTIME_F_HIDE_DATE                (0x00000010)

/** \def PANTHEIOS_GETCURRENTTIME_F_HIDE_TIME
 *
 * Indicates that pantheios_util_getCurrentTime() should hide the time
 * component of the date/time field
 *
 * \ingroup group__utility__backend
 */
#define PANTHEIOS_GETCURRENTTIME_F_HIDE_TIME                (0x00000020)

/** \def PANTHEIOS_GETCURRENTTIME_F_TIME_RES_SECONDS
 *
 * Indicates that pantheios_util_getCurrentTime() should use a time
 * resolution of seconds
 *
 * \ingroup group__utility__backend
 */
#define PANTHEIOS_GETCURRENTTIME_F_TIME_RES_SECONDS         (0x00000000)

/** \def PANTHEIOS_GETCURRENTTIME_F_TIME_RES_TENTHS
 *
 * Indicates that pantheios_util_getCurrentTime() should use a time
 * resolution of tenths of seconds
 *
 * \ingroup group__utility__backend
 */
#define PANTHEIOS_GETCURRENTTIME_F_TIME_RES_TENTHS          (0x00000100)

/** \def PANTHEIOS_GETCURRENTTIME_F_TIME_RES_HUNDREDTHS
 *
 * Indicates that pantheios_util_getCurrentTime() should use a time
 * resolution of hundredths of seconds
 *
 * \ingroup group__utility__backend
 */
#define PANTHEIOS_GETCURRENTTIME_F_TIME_RES_HUNDREDTHS      (0x00000200)

/** \def PANTHEIOS_GETCURRENTTIME_F_TIME_RES_MILLISECS
 *
 * Indicates that pantheios_util_getCurrentTime() should use a time
 * resolution of milliseconds
 *
 * \ingroup group__utility__backend
 */
#define PANTHEIOS_GETCURRENTTIME_F_TIME_RES_MILLISECS       (0x00000300)

/** \def PANTHEIOS_GETCURRENTTIME_F_TIME_RES_MICROSECS
 *
 * Indicates that pantheios_util_getCurrentTime() should use a time
 * resolution of microseconds
 *
 * \ingroup group__utility__backend
 */
#define PANTHEIOS_GETCURRENTTIME_F_TIME_RES_MICROSECS       (0x00000600)

/** \def PANTHEIOS_GETCURRENTTIME_F_TIME_RES_MAX
 *
 * The maximum time resolution available from
 * pantheios_util_getCurrentTime()
 *
 * \ingroup group__utility__backend
 */
#define PANTHEIOS_GETCURRENTTIME_F_TIME_RES_MAX             PANTHEIOS_GETCURRENTTIME_F_TIME_RES_MICROSECS

/** \def PANTHEIOS_GETCURRENTTIME_F_TIME_RES_MASK
 *
 * The time resolution mask used by pantheios_util_getCurrentTime()
 *
 * \ingroup group__utility__backend
 */
#define PANTHEIOS_GETCURRENTTIME_F_TIME_RES_MASK            (0x00000f00)

/** \def PANTHEIOS_GETCURRENTTIME_F_FAVOUR_SPEED
 *
 * Indicates that pantheios_util_getCurrentTime() should prefer execution
 * speed over accuracy when preparing the time
 *
 * \ingroup group__utility__backend
 */
#define PANTHEIOS_GETCURRENTTIME_F_FAVOUR_SPEED             (0x00001000)

/** \def PANTHEIOS_GETCURRENTTIME_F_FAVOUR_ACCURACY
 *
 * Indicates that pantheios_util_getCurrentTime() should prefer accuracy
 * over execution speed when preparing the time
 *
 * \ingroup group__utility__backend
 */
#define PANTHEIOS_GETCURRENTTIME_F_FAVOUR_ACCURACY          (0x00002000)

/* /////////////////////////////////////////////////////////////////////////
 * Functions
 */

/** Gets the current time in a suitable format.
 *
 * \ingroup group__utility__backend
 *
 * This function gets the current time, and formats it in an operating
 * system-specific manner into the given pan_beutil_time_t instance, using
 * either the local time or the system time (if the
 * <code>flags</code> parameter includes
 * PANTHEIOS_GETCURRENTTIME_F_USE_SYSTEM_TIME).
 *
 * Specifically, on Windows platforms the function formats according to the
 * current locale settings. On UNIX, the function uses the syslog-standard
 * format (<code>strftimeFmt()</code> format: <code>"%b %d %I:%M:%S"</code>)
 * or the <code>pan_beutil_time_t::strftimeFmt</code> member if it is
 * non-NULL.
 *
 * \param tm Pointer to a pan_beutil_time_t instance, whose <code>str</code>
 *   and <code>capacity</code> members must be valid. May not be NULL.
 * \param flags One or more of the
 *   <code>PANTHEIOS_GETCURRENTTIME_F_*</code> flags.
 *
 * \return The number of characters written into the buffer, or 0 on failure
 */
PANTHEIOS_CALL(size_t) pantheios_util_getCurrentTime(pan_beutil_time_t* tm, int flags);

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_UTIL_TIME_H_CURRENTTIME */

/* ///////////////////////////// end of file //////////////////////////// */
