/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/internal/threading.h
 *
 * Purpose:     Detects whether the library is being built single- or
 *              multi-threaded.
 *
 * Created:     20th November 2007
 * Updated:     18th March 2012
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2007-2012, Matthew Wilson and Synesis Software
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


/** \file pantheios/internal/threading.h
 *
 * \brief [C, C++] INTERNAL FILE: Detects whether the library is being built
 *   single- or multi-threaded.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INTERNAL_H_THREADING
#define PANTHEIOS_INCL_PANTHEIOS_INTERNAL_H_THREADING

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

#ifndef PLATFORMSTL_INCL_PLATFORMSTL_H_PLATFORMSTL
# include <platformstl/platformstl.h>
#endif /* !PLATFORMSTL_INCL_PLATFORMSTL_H_PLATFORMSTL */
#ifndef PLATFORMSTL_INCL_PLATFORMSTL_SYNCH_UTIL_H_FEATURES
# include <platformstl/synch/util/features.h>
#endif /* !PLATFORMSTL_INCL_PLATFORMSTL_SYNCH_UTIL_H_FEATURES */

/* /////////////////////////////////////////////////////////////////////////
 * Feature discrimination
 */

#ifdef PANTHEIOS_MT
# undef PANTHEIOS_MT
#endif /* PANTHEIOS_MT */
#ifdef PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS
# undef PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS
#endif /* PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS */

/* Detect whether we're multithreaded */

#if defined(PANTHEIOS_FORCE_MT)
# define PANTHEIOS_MT
#elif !defined(PANTHEIOS_NO_MT)
# if defined(PLATFORMSTL_OS_IS_UNIX)
#  if defined(UNIXSTL_USING_PTHREADS)
#   define PANTHEIOS_MT
#  endif /* UNIXSTL_USING_PTHREADS */
# elif defined(PLATFORMSTL_OS_IS_WINDOWS)
#  if defined(__MT__) || \
      defined(_REENTRANT) || \
      defined(_MT)
#   define PANTHEIOS_MT
#  endif /* ?? mt ?? */
# else /* ? OS */
#  error Platform is not yet discriminated
# endif /* OS */
#endif /* PANTHEIOS_FORCE_MT || PANTHEIOS_NO_MT */

/* Detect whether, if we're multithreaded, we're using atomic integer ops */

#ifdef PANTHEIOS_MT
# if defined(PLATFORMSTL_HAS_ATOMIC_INTEGER_OPERATIONS) && \
     !defined(PANTHEIOS_FORCE_NO_ATOMIC_INTEGER_OPERATIONS)
#  define PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS
# endif
#endif /* PANTHEIOS_MT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_INTERNAL_H_THREADING */

/* ///////////////////////////// end of file //////////////////////////// */
