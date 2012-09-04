/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/internal/string_encoding.h
 *
 * Purpose:     Utilities to assist with implementation as multibyte or
 *              wide string
 *
 * Created:     14th November 2008
 * Updated:     22nd March 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2008-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/internal/string_encoding.h
 *
 * [C, C++] Utilities to assist with implementation as multibyte or wide
 *   string
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INTERNAL_H_STRING_ENCODING
#define PANTHEIOS_INCL_PANTHEIOS_INTERNAL_H_STRING_ENCODING

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#include <pantheios/pantheios.h>

/* /////////////////////////////////////////////////////////////////////////
 * Macros
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_LITERAL_w_(x)            L ## x
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/** \def PANTHEIOS_LITERAL_STRING(s)
 *
 * Defines the literal string as a multibyte or wide string, depending on
 * the absence or presence, respectively, of the symbol PANTHEIOS_USE_WIDE_STRINGS
 */

/** \def PANTHEIOS_LITERAL_CHAR(s)
 *
 * Defines the literal string as a multibyte or wide character, depending on
 * the absence or presence, respectively, of the symbol PANTHEIOS_USE_WIDE_STRINGS
 */

#ifdef PANTHEIOS_USE_WIDE_STRINGS
# define PANTHEIOS_LITERAL_STRING(x)        PANTHEIOS_LITERAL_w_(x)
# define PANTHEIOS_LITERAL_CHAR(x)          PANTHEIOS_LITERAL_w_(x)
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
# define PANTHEIOS_LITERAL_STRING(x)        x
# define PANTHEIOS_LITERAL_CHAR(x)          x
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_INTERNAL_H_STRING_ENCODING */

/* ///////////////////////////// end of file //////////////////////////// */
