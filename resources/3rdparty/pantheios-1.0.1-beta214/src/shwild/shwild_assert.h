/* /////////////////////////////////////////////////////////////////////////
 * File:        src/shwild_assert.h
 *
 * Purpose:     Contract Programming enforcement definitions.
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


#ifndef SHWILD_INCL_H_SHWILD_ASSERT
#define SHWILD_INCL_H_SHWILD_ASSERT

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#include <shwild/shwild.h>
#include "shwild_stlsoft.h"

/* /////////////////////////////////////////////////////////////////////////
 * Macros
 */

#define SHWILD_ASSERT(expr)                 STLSOFT_ASSERT(expr)

#define SHWILD_MESSAGE_ASSERT(msg, expr)    STLSOFT_MESSAGE_ASSERT(msg, expr)

/* ////////////////////////////////////////////////////////////////////// */

#endif /* SHWILD_INCL_H_SHWILD_ASSERT */

/* ////////////////////////////////////////////////////////////////////// */
