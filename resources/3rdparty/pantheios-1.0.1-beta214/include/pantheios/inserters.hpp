/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters.hpp
 *
 * Purpose:     Main include file for all Pantheios standard inserters.
 *
 * Created:     21st June 2005
 * Updated:     12th December 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2010, Matthew Wilson and Synesis Software
 * Copyright (c) 1999-2005, Synesis Software and Matthew Wilson
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


/** \file pantheios/inserters.hpp
 *
 * [C++ only] Main include file for all Pantheios standard inserters.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_HPP_INSERTERS
#define PANTHEIOS_INCL_PANTHEIOS_HPP_INSERTERS

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_HPP_INSERTERS_MAJOR    2
# define PANTHEIOS_VER_PANTHEIOS_HPP_INSERTERS_MINOR    6
# define PANTHEIOS_VER_PANTHEIOS_HPP_INSERTERS_REVISION 1
# define PANTHEIOS_VER_PANTHEIOS_HPP_INSERTERS_EDIT     21
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_HPP_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_HPP_PANTHEIOS */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_ARGS
# include <pantheios/inserters/args.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_ARGS */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_B
# include <pantheios/inserters/b.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_B */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_B64
# include <pantheios/inserters/b64.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_B64 */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_BLOB
# include <pantheios/inserters/blob.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_BLOB */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_BOOLEAN
# include <pantheios/inserters/boolean.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_BOOLEAN */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_CH
# include <pantheios/inserters/ch.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_CH */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_CHARACTER
# include <pantheios/inserters/character.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_CHARACTER */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_HEX_PTR
# include <pantheios/inserters/hex_ptr.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_HEX_PTR */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_HOSTID
# include <pantheios/inserters/hostid.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_HOSTID */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_I
# include <pantheios/inserters/i.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_I */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_INTEGER
# include <pantheios/inserters/integer.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_INTEGER */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_M2W
# include <pantheios/inserters/m2w.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_M2W */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_P
# include <pantheios/inserters/p.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_P */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_PAD
# include <pantheios/inserters/pad.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_PAD */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_POINTER
# include <pantheios/inserters/pointer.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_POINTER */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_PROCESSID
# include <pantheios/inserters/processid.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_PROCESSID */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_REAL
# include <pantheios/inserters/real.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_REAL */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_SLICE
# include <pantheios/inserters/slice.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_SLICE */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_THREADID
# include <pantheios/inserters/threadid.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_THREADID */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_W2M
# include <pantheios/inserters/w2m.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_W2M */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_XI
# include <pantheios/inserters/xi.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_XI */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_XP
# include <pantheios/inserters/xp.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_XP */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_HPP_INSERTERS */

/* ///////////////////////////// end of file //////////////////////////// */
