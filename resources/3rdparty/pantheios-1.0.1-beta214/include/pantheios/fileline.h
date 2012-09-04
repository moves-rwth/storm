/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/fileline.h
 *
 * Purpose:     Customisable definition of file+line, for the Pantheios
 *              Assertion and Tracing APIs.
 *
 * Created:     11th November 2007
 * Updated:     7th December 2010
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


/** \file pantheios/fileline.h
 *
 * [C, C++] Customisable definition of file+line, for
 *  the \ref group__assertion
 *  and the \ref group__tracing.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_FILELINE
#define PANTHEIOS_INCL_PANTHEIOS_H_FILELINE

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_H_FILELINE_MAJOR       2
# define PANTHEIOS_VER_PANTHEIOS_H_FILELINE_MINOR       0
# define PANTHEIOS_VER_PANTHEIOS_H_FILELINE_REVISION    1
# define PANTHEIOS_VER_PANTHEIOS_H_FILELINE_EDIT        15
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

#include <stlsoft/stlsoft.h>

/* /////////////////////////////////////////////////////////////////////////
 * Tracing features
 */

/** \def PANTHEIOS_FILELINE_A
 *
 * Default file+line format for \ref group__tracing
 *
 * \ingroup group__tracing
 */

/** \def PANTHEIOS_FILELINE
 *
 * Default file+line format for \ref group__tracing
 *
 * \ingroup group__tracing
 *
 * \remarks Because Pantheios is currently multibyte-only, this resolves to
 *   PANTHEIOS_FILELINE_A
 */

 /* Define the defaults here, in case we use them below. */
# define PANTHEIOS_FILELINE_A                   __FILE__ "(" PANTHEIOS_STRINGIZE(__LINE__) "): "
# define PANTHEIOS_FILELINE                     PANTHEIOS_FILELINE_A

/* /////////////////////////////////////////////////////////////////////////
 * Inclusion
 */

#ifdef STLSOFT_PPF_pragma_once_SUPPORT
# pragma once
#endif /* STLSOFT_PPF_pragma_once_SUPPORT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_FILELINE */

/* ///////////////////////////// end of file //////////////////////////// */
