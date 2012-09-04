/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/internal/lean.h
 *
 * Purpose:     Suppresses as much as possible of operating-system headers,
 *              to decrease compilation times.
 *
 * Created:     22nd April 2008
 * Updated:     10th August 2009
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2008-2009, Matthew Wilson and Synesis Software
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
 * - Neither the names of Matthew Wilson and Synesis Software nor the names
 *   of any contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
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


/** \file pantheios/internal/lean.h
 *
 * [C, C++] INTERNAL FILE: Suppresses as much as possible of
 *  operating-system headers, to decrease compilation times.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INTERNAL_H_LEAN
#define PANTHEIOS_INCL_PANTHEIOS_INTERNAL_H_LEAN

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#if defined(__FreeBSD__) || \
    defined(__NetBSD__) || \
    defined(__OpenBSD__) || \
    defined(__bsdi__) || \
    defined(_SYSTYPE_BSD)
/* # include <pantheios/internal/bsdlean.h> */
#endif /* BSD */

#if defined(__sysv__) || \
    defined(__SVR4) || \
    defined(__svr4__) || \
    defined(_SYSTYPE_SVR4)
/* # include <pantheios/internal/svr4lean.h> */
#endif /* SVR4 */

#if defined(__APPLE__) || \
    defined(__MACOSX__)
/* # include <pantheios/internal/maclean.h> */
#endif /* Mac */

#if defined(hpux) || \
    defined(_hpux)
/* # include <pantheios/internal/hpuxlean.h> */
#endif /* HPUX */

#if defined(__linux__) || \
    defined(__linux) || \
    defined(linux)
/* # include <pantheios/internal/linuxlean.h> */
#endif /* Linux */

#if defined(sun) || \
    defined(__sun)
/* # include <pantheios/internal/sunlean.h> */
#endif /* Sun */

#if defined(unix) || \
    defined(UNIX) || \
    defined(__unix__) || \
    defined(__unix)
/* # include <pantheios/internal/unixlean.h> */
#endif /* UNIX */

#if defined(_WIN32) || \
    defined(_WIN64)
# include <pantheios/internal/winlean.h>
#endif /* _WIN32 && _WIN64 */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_INTERNAL_H_LEAN */

/* ///////////////////////////// end of file //////////////////////////// */
