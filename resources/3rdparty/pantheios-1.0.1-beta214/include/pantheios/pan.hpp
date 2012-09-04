/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/pan.hpp
 *
 * Purpose:     Equivalent to the Pantheios C++ primary include
 *              pantheios/pantheios.hpp, but aliases the pantheios
 *              namespace to pan.
 *
 * Created:     13th January 2008
 * Updated:     24th June 2011
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2008-2011, Matthew Wilson and Synesis Software
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


/** \file pantheios/pan.hpp
 *
 * \brief [C++ only] Equivalent to the Pantheios C++ primary include
 *   pantheios/pantheios.hpp, but aliases the pantheios namespace to pan.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_HPP_PAN
#define PANTHEIOS_INCL_PANTHEIOS_HPP_PAN

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_HPP_PAN_MAJOR      1
# define PANTHEIOS_VER_PANTHEIOS_HPP_PAN_MINOR      0
# define PANTHEIOS_VER_PANTHEIOS_HPP_PAN_REVISION   3
# define PANTHEIOS_VER_PANTHEIOS_HPP_PAN_EDIT       5
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Language
 */

#ifndef __cplusplus
# error This file can only be included in C++ compilation units
#endif /* !__cplusplus */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_HPP_PANTHEIOS
# include <pantheios/pantheios.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_HPP_PANTHEIOS */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pan = ::pantheios;
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Inclusion
 */

#ifdef STLSOFT_PPF_pragma_once_SUPPORT
# pragma once
#endif /* STLSOFT_PPF_pragma_once_SUPPORT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_HPP_PAN */

/* ///////////////////////////// end of file //////////////////////////// */
