/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/trace.h
 *
 * Purpose:     Pantheios Tracing API.
 *
 * Created:     11th November 2007
 * Updated:     23rd July 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2007-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/trace.h
 *
 * [C, C++] Include file for the \ref group__tracing
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_TRACE
#define PANTHEIOS_INCL_PANTHEIOS_H_TRACE

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_H_TRACE_MAJOR      1
# define PANTHEIOS_VER_PANTHEIOS_H_TRACE_MINOR      3
# define PANTHEIOS_VER_PANTHEIOS_H_TRACE_REVISION   1
# define PANTHEIOS_VER_PANTHEIOS_H_TRACE_EDIT       16
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */
#ifndef PANTHEIOS_INCL_PANTHEIOS_H_FILELINE
# include <pantheios/fileline.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_FILELINE */

#ifdef PANTHEIOS_USE_WIDE_STRINGS
# error The Pantheios Tracing API is currently only supported in multibyte builds
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

#include <stlsoft/stlsoft.h>

/* /////////////////////////////////////////////////////////////////////////
 * Tracing features
 */

/** \defgroup group__tracing Pantheios Tracing API
 *
 * Pantheios Tracing API
 *
 * If your compiler supports variadic macros, you may use this to log by
 * "tracing", where your log statement is preceded by
 * "<file>(<line>): ", or "<file>: ", and so on, or a custom value.
 *
 * The Pantheios Tracing API consists of:
 *
 *  - PANTHEIOS_TRACE_PRINTF()        -   uses pantheios::printf
 *  - PANTHEIOS_TRACE()               -   uses pantheios::log
 *  - PANTHEIOS_TRACE_EMERGENCY()     -   uses pantheios::log_EMERGENCY
 *  - PANTHEIOS_TRACE_ALERT()         -   uses pantheios::log_ALERT
 *  - PANTHEIOS_TRACE_CRITICAL()      -   uses pantheios::log_CRITICAL
 *  - PANTHEIOS_TRACE_ERROR()         -   uses pantheios::log_ERROR
 *  - PANTHEIOS_TRACE_WARNING()       -   uses pantheios::log_WARNING
 *  - PANTHEIOS_TRACE_NOTICE()        -   uses pantheios::log_NOTICE
 *  - PANTHEIOS_TRACE_INFORMATIONAL() -   uses pantheios::log_INFORMATIONAL
 *  - PANTHEIOS_TRACE_DEBUG()         -   uses pantheios::log_DEBUG
 *
 * By default, support for variadic macros is via the STLSoft
 * preprocessor symbol, STLSOFT_CF_SUPPORTS_VARIADIC_MACROS. If that is
 * defined, and you have *not* defined PANTHEIOS_NO_USE_VARIADIC_MACROS,
 * then the symbol PANTHEIOS_USES_VARIADIC_MACROS will be defined, and
 * the various aspects of the Pantheios tracing api described above will be
 * available to you.
 *
 * If you _know_ that your compiler supports variadics even when STLSoft
 * thinks that it does not, you can just define
 * PANTHEIOS_USES_VARIADIC_MACROS yourself.
 *
 * By default, the prefix is of the form "<file>(<line>): ", but you can
 * override it to whatever you want by redefining PANTHEIOS_TRACE_PREFIX.
 */

#if defined(STLSOFT_CF_SUPPORTS_VARIADIC_MACROS) && \
    !defined(PANTHEIOS_NO_USE_VARIADIC_MACROS)
# define PANTHEIOS_USES_VARIADIC_MACROS
#endif /* STLSOFT_CF_SUPPORTS_VARIADIC_MACROS && !PANTHEIOS_NO_USE_VARIADIC_MACROS */



/** \def PANTHEIOS_TRACE_PREFIX
 *
 * The file-line prefix uses by the \ref group__tracing for C constructs. It
 *  defaults to PANTHEIOS_FILELINE, but may be \#define'd by the user to be
 *  something else, as desired.
 *
 * \ingroup group__tracing
 */

/** \def PANTHEIOS_TRACE_LOG_PREFIX
 *
 * The file-line prefix uses by the \ref group__tracing for C++ constructs.
 *  It defaults to <code>pantheios::pan_slice_t(PANTHEIOS_FILELINE)</code>,
 *  but may be \#define'd by the user to be something else, as desired.
 *
 * \ingroup group__tracing
 */


 /* Now define the prefix, if the user has not already done so. */
# if !defined(PANTHEIOS_TRACE_PREFIX)
#  define PANTHEIOS_TRACE_PREFIX                PANTHEIOS_FILELINE
#  ifdef __cplusplus
#   define PANTHEIOS_TRACE_LOG_PREFIX           ::pantheios::pan_slice_t(PANTHEIOS_FILELINE, STLSOFT_NUM_ELEMENTS(PANTHEIOS_FILELINE) - 1)
#  else /* ? __cplusplus */
#   define PANTHEIOS_TRACE_LOG_PREFIX           PANTHEIOS_FILELINE
#  endif /* __cplusplus */
# else /* ? PANTHEIOS_TRACE_PREFIX */
#  define PANTHEIOS_TRACE_LOG_PREFIX            PANTHEIOS_TRACE_PREFIX
# endif /* !PANTHEIOS_TRACE_PREFIX */

 /* Now define the Pantheios Tracing API */

#if defined(PANTHEIOS_USES_VARIADIC_MACROS) || \
    defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION)

/** \def PANTHEIOS_TRACE_PRINTF
 *
 * Logs a printf()-formatted statement with a file+line prefix, at the
 * given severity level
 *
 * \ingroup group__tracing
 *
 * \param sev The severity level
 * \param fmt The format string
 */

# if !defined(PANTHEIOS_NO_NAMESPACE) || \
     defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION)
#  define PANTHEIOS_TRACE_PRINTF(sev, fmt, ...) ::pantheios::pantheios_logprintf((sev), "%s" fmt, stlsoft_static_cast(PANTHEIOS_NS_QUAL(pan_char_t) const*, PANTHEIOS_TRACE_PREFIX), __VA_ARGS__)
# else /* ? __cplusplus */
#  define PANTHEIOS_TRACE_PRINTF(sev, fmt, ...)              pantheios_logprintf((sev), "%s" fmt, stlsoft_static_cast(PANTHEIOS_NS_QUAL(pan_char_t) const*, PANTHEIOS_TRACE_PREFIX), __VA_ARGS__)
# endif /* __cplusplus */

# if !defined(PANTHEIOS_NO_NAMESPACE) || \
     defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION)

/** \def PANTHEIOS_TRACE
 *
 * Logs a multi-argument statement with a file+line prefix, at the
 * given severity level
 *
 * \ingroup group__tracing
 *
 * \param sev The severity level
 */

/** \def PANTHEIOS_TRACE_EMERGENCY
 *
 * Logs a multi-argument statement with a file+line prefix, at the
 * pantheios::EMERGENCY severity level
 *
 * \ingroup group__tracing
 */

/** \def PANTHEIOS_TRACE_ALERT
 *
 * Logs a multi-argument statement with a file+line prefix, at the
 * pantheios::ALERT severity level
 *
 * \ingroup group__tracing
 */

/** \def PANTHEIOS_TRACE_CRITICAL
 *
 * Logs a multi-argument statement with a file+line prefix, at the
 * pantheios::CRITICAL severity level
 *
 * \ingroup group__tracing
 */

/** \def PANTHEIOS_TRACE_ERROR
 *
 * Logs a multi-argument statement with a file+line prefix, at the
 * pantheios::ERROR severity level
 *
 * \ingroup group__tracing
 */

/** \def PANTHEIOS_TRACE_WARNING
 *
 * Logs a multi-argument statement with a file+line prefix, at the
 * pantheios::WARNING severity level
 *
 * \ingroup group__tracing
 */

/** \def PANTHEIOS_TRACE_NOTICE
 *
 * Logs a multi-argument statement with a file+line prefix, at the
 * pantheios::NOTICE severity level
 *
 * \ingroup group__tracing
 */

/** \def PANTHEIOS_TRACE_INFORMATIONAL
 *
 * Logs a multi-argument statement with a file+line prefix, at the
 * pantheios::INFORMATIONAL severity level
 *
 * \ingroup group__tracing
 */

/** \def PANTHEIOS_TRACE_DEBUG
 *
 * Logs a multi-argument statement with a file+line prefix, at the
 * pantheios::DEBUG severity level
 *
 * \ingroup group__tracing
 */

#  define PANTHEIOS_TRACE(sev, ...)             ::pantheios::log((sev), PANTHEIOS_TRACE_LOG_PREFIX, __VA_ARGS__)
#  if !defined(PANTHEIOS_NO_STOCK_LEVELS)
#   define PANTHEIOS_TRACE_EMERGENCY(...)       ::pantheios::log(PANTHEIOS_SEV_EMERGENCY, PANTHEIOS_TRACE_LOG_PREFIX, __VA_ARGS__)
#   define PANTHEIOS_TRACE_ALERT(...)           ::pantheios::log(PANTHEIOS_SEV_ALERT, PANTHEIOS_TRACE_LOG_PREFIX, __VA_ARGS__)
#   define PANTHEIOS_TRACE_CRITICAL(...)        ::pantheios::log(PANTHEIOS_SEV_CRITICAL, PANTHEIOS_TRACE_LOG_PREFIX, __VA_ARGS__)
#   define PANTHEIOS_TRACE_ERROR(...)           ::pantheios::log(PANTHEIOS_SEV_ERROR, PANTHEIOS_TRACE_LOG_PREFIX, __VA_ARGS__)
#   define PANTHEIOS_TRACE_WARNING(...)         ::pantheios::log(PANTHEIOS_SEV_WARNING, PANTHEIOS_TRACE_LOG_PREFIX, __VA_ARGS__)
#   define PANTHEIOS_TRACE_NOTICE(...)          ::pantheios::log(PANTHEIOS_SEV_NOTICE, PANTHEIOS_TRACE_LOG_PREFIX, __VA_ARGS__)
#   define PANTHEIOS_TRACE_INFORMATIONAL(...)   ::pantheios::log(PANTHEIOS_SEV_INFORMATIONAL, PANTHEIOS_TRACE_LOG_PREFIX, __VA_ARGS__)
#   define PANTHEIOS_TRACE_DEBUG(...)           ::pantheios::log(PANTHEIOS_SEV_DEBUG, PANTHEIOS_TRACE_LOG_PREFIX, __VA_ARGS__)
#  endif /* !PANTHEIOS_NO_STOCK_LEVELS */
# endif /* __cplusplus */

#endif /* PANTHEIOS_USES_VARIADIC_MACROS */

/* /////////////////////////////////////////////////////////////////////////
 * Inclusion
 */

#ifdef STLSOFT_PPF_pragma_once_SUPPORT
# pragma once
#endif /* STLSOFT_PPF_pragma_once_SUPPORT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_TRACE */

/* ///////////////////////////// end of file //////////////////////////// */
