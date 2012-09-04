/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/util/memory/inserters.hpp
 *
 * Purpose:     Memory (de-)allocation for the inserters.
 *
 * Created:     8th January 2008
 * Updated:     14th October 2008
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2008, Matthew Wilson and Synesis Software
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


/** \file pantheios/util/memory/inserters.hpp
 *
 * [C++ only] Memory (de-)allocation for the inserters.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_UTIL_INSERTERS_HPP_MEMORY
#define PANTHEIOS_INCL_PANTHEIOS_UTIL_INSERTERS_HPP_MEMORY

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_UTIL_INSERTERS_HPP_MEMORY_MAJOR    2
# define PANTHEIOS_VER_PANTHEIOS_UTIL_INSERTERS_HPP_MEMORY_MINOR    0
# define PANTHEIOS_VER_PANTHEIOS_UTIL_INSERTERS_HPP_MEMORY_REVISION 1
# define PANTHEIOS_VER_PANTHEIOS_UTIL_INSERTERS_HPP_MEMORY_EDIT     7
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

#error This file is now deprecated, as of 1.0.1 beta 166, and will be removed in a future release. Please instead implement inserter memory operations in terms of the new pantheios_inserterAllocate() and pantheios_inserterDeallocate() functions

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_UTIL_INSERTERS_HPP_MEMORY */

/* ///////////////////////////// end of file //////////////////////////// */
