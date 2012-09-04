/* /////////////////////////////////////////////////////////////////////////
 * File:        xtests/internal/safestr.h
 *
 * Purpose:     Handles detection of safe string library.
 *
 * Created:     9th February 2008
 * Updated:     8th March 2008
 *
 * Author:      Matthew Wilson
 *
 * Home:        http://www.xtests.org/
 *
 * Copyright:   Matthew Wilson and Synesis Software Pty Ltd, 2008.
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


/** \file xtests/internal/safestr.h
 *
 * \brief [C, C++] Handles detection of safe string library.
 */

#ifndef XTESTS_INCL_XTESTS_INTERNAL_H_SAFESTR
#define XTESTS_INCL_XTESTS_INTERNAL_H_SAFESTR

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef XTESTS_INCL_XTESTS_H_XTESTS
# include <xtests/xtests.h>
#endif /* !XTESTS_INCL_XTESTS_H_XTESTS */

#ifdef XTESTS_SAFE_STR_USE_crtdefs_h_
# undef XTESTS_SAFE_STR_USE_crtdefs_h_
#endif /* XTESTS_SAFE_STR_USE_crtdefs_h_ */

#if defined(__BORLANDC__)
#elif defined(__DMC__)
#elif defined(__GNUC__)
#elif defined(__INTEL_COMPILER)
# if defined(_MSC_VER) && \
     _MSC_VER >= 1400
#  define XTESTS_SAFE_STR_USE_crtdefs_h_
# endif /* _MSC_VER >= 1400 */
#elif defined(__MWERKS__)
#elif defined(__WATCOMC__)
#elif defined(_MSC_VER)
# if _MSC_VER >= 1400
#  define XTESTS_SAFE_STR_USE_crtdefs_h_
# endif /* _MSC_VER >= 1400 */
#elif defined(__COMO__)
#else
#endif /* compiler */

#ifdef XTESTS_SAFE_STR_USE_crtdefs_h_
# include <crtdefs.h>
#endif /* XTESTS_SAFE_STR_USE_crtdefs_h_ */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifdef __STDC_SECURE_LIB__
# if defined(__STDC_WANT_SECURE_LIB__) && \
     __STDC_WANT_SECURE_LIB__ == 1
#  define XTESTS_USING_SAFE_STR_FUNCTIONS
# endif /* __STDC_WANT_SECURE_LIB__ */
#endif /* __STDC_SECURE_LIB__ */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* XTESTS_INCL_XTESTS_INTERNAL_H_SAFESTR */

/* ////////////////////////////////////////////////////////////////////// */
