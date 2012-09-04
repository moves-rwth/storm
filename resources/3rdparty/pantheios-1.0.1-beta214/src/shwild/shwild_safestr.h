/* /////////////////////////////////////////////////////////////////////////
 * File:        src/shwild_safestr.h
 *
 * Purpose:     Handles detection of safe string library.
 *
 * Created:     8th February 2008
 * Updated:     20th December 2011
 *
 * Author:      Matthew Wilson
 *
 * Home:        http://www.shwild.org/
 *
 * Copyright (c) 2008-2011, Matthew Wilson
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


#ifndef SHWILD_INCL_H_SHWILD_SAFESTR
#define SHWILD_INCL_H_SHWILD_SAFESTR

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#include <shwild/shwild.h>

#include "shwild_stlsoft.h"

#ifdef SHWILD_SAFE_STR_USE_crtdefs_h_
# undef SHWILD_SAFE_STR_USE_crtdefs_h_
#endif /* SHWILD_SAFE_STR_USE_crtdefs_h_ */

#if defined(__BORLANDC__)
#elif defined(__DMC__)
#elif defined(__GNUC__)
#elif defined(__INTEL_COMPILER)
# if defined(_MSC_VER) && \
     _MSC_VER >= 1400
#  define SHWILD_SAFE_STR_USE_crtdefs_h_
# endif /* _MSC_VER >= 1400 */
#elif defined(__MWERKS__)
#elif defined(__WATCOMC__)
#elif defined(_MSC_VER)
# if _MSC_VER >= 1400
#  define SHWILD_SAFE_STR_USE_crtdefs_h_
# endif /* _MSC_VER >= 1400 */
#elif defined(__COMO__)
#else
#endif /* compiler */

#ifdef SHWILD_SAFE_STR_USE_crtdefs_h_
# include <crtdefs.h>
#else /* ? SHWILD_SAFE_STR_USE_crtdefs_h_ */
# include <string.h>
#endif /* SHWILD_SAFE_STR_USE_crtdefs_h_ */

/* /////////////////////////////////////////////////////////////////////////
 * Feature detection
 */

#ifdef __STDC_SECURE_LIB__
# if defined(__STDC_WANT_SECURE_LIB__) && \
     __STDC_WANT_SECURE_LIB__ == 1
#  define SHWILD_USING_SAFE_STR_FUNCTIONS
# endif /* __STDC_WANT_SECURE_LIB__ */
#endif /* __STDC_SECURE_LIB__ */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* SHWILD_INCL_H_SHWILD_SAFESTR */

/* ////////////////////////////////////////////////////////////////////// */
