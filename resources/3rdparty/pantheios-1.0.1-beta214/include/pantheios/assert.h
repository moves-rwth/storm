/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/assert.h
 *
 * Purpose:     Pantheios Assertion API.
 *
 * Created:     8th May 2009
 * Updated:     6th November 2010
 *
 * Thanks to:   markitus82 for requesting this functionality
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2009-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/assert.h
 *
 * [C, C++] Include file for the \ref group__assertion
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_ASSERT
#define PANTHEIOS_INCL_PANTHEIOS_H_ASSERT

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_H_ASSERT_MAJOR     1
# define PANTHEIOS_VER_PANTHEIOS_H_ASSERT_MINOR     1
# define PANTHEIOS_VER_PANTHEIOS_H_ASSERT_REVISION  2
# define PANTHEIOS_VER_PANTHEIOS_H_ASSERT_EDIT      8
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

#include <stlsoft/stlsoft.h>

/* /////////////////////////////////////////////////////////////////////////
 * Assertion features
 */

/** \defgroup group__assertion Pantheios Assertion API
 *
 * Pantheios Assertion API
 *
 * This API allows you to combine traditional assert()-based programming
 * with diagnostic logging, using the PANTHEIOS_ASSERT() macro.
 *
<pre>
  int main()
  {
    PANTHEIOS_ASSERT(false); // Will assert, and will log

    return 0;
  }
</pre>
 */



/** \def PANTHEIOS_ASSERT_SEVERITY_LEVEL
 *
 * The level at which log statements will be submitted by
 * \c PANTHEIOS_ASSERT and \c PANTHEIOS_MESSAGE_ASSERT.
 *
 * Defaults to \c PANTHEIOS_SEV_EMERGENCY
 *
 * \ingroup group__assertion
 */

#ifndef PANTHEIOS_ASSERT_SEVERITY_LEVEL
# define PANTHEIOS_ASSERT_SEVERITY_LEVEL    PANTHEIOS_SEV_EMERGENCY
#endif /* !PANTHEIOS_ASSERT_SEVERITY_LEVEL */


/** \def PANTHEIOS_ASSERT(expr)
 *
 * Defines an assertion construct for runtime verification.
 *
 * \param expr Must be non-zero, or an assertion will be fired
 *
 * If the expression evaluates to false a log statement will be
 * submitted at the PANTHEIOS_ASSERT_SEVERITY_LEVEL severity level, and
 * emitted if that level is currently switched on. The expression will also
 * be subjected to an assert, via \c STLSOFT_ASSERT()
 *
 * \ingroup group__assertion
 */

#define PANTHEIOS_ASSERT(expr)              \
                                            \
    do                                      \
    {                                       \
        if(!(expr))                         \
        {                                   \
            PANTHEIOS_NS_QUAL(pantheios_logassertfail)(PANTHEIOS_ASSERT_SEVERITY_LEVEL, PANTHEIOS_FILELINE_A, "assertion failed: " #expr); \
        }                                   \
                                            \
        STLSOFT_ASSERT(expr);               \
    } while(0)


/** \def PANTHEIOS_MESSAGE_ASSERT(expr, msg)
 *
 * Defines an assertion construct for runtime verification.
 *
 * \param expr Must be non-zero, or an assertion will be fired
 * \param msg The message that will be associated with the output if the assertion fires
 *
 * If the expression evaluates to false a log statement will be
 * submitted at the PANTHEIOS_ASSERT_SEVERITY_LEVEL severity level, and
 * emitted if that level is currently switched on. The expression will also
 * be subjected to an assert, via \c STLSOFT_MESSAGE_ASSERT()
 *
 * \ingroup group__assertion
 */

#define PANTHEIOS_MESSAGE_ASSERT(expr, msg) \
                                            \
    do                                      \
    {                                       \
        if(!(expr))                         \
        {                                   \
            PANTHEIOS_NS_QUAL(pantheios_logassertfail)(PANTHEIOS_ASSERT_SEVERITY_LEVEL, PANTHEIOS_FILELINE_A, "assertion failed: " #expr "; message: " msg); \
        }                                   \
                                            \
        STLSOFT_MESSAGE_ASSERT(msg, expr);  \
    } while(0)


/* /////////////////////////////////////////////////////////////////////////
 * Inclusion
 */

#ifdef STLSOFT_PPF_pragma_once_SUPPORT
# pragma once
#endif /* STLSOFT_PPF_pragma_once_SUPPORT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_ASSERT */

/* ///////////////////////////// end of file //////////////////////////// */
