/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters/fmt.hpp
 *
 * Purpose:     Format constants for Pantheios inserter classes.
 *
 * Created:     21st June 2005
 * Updated:     6th August 2012
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2012, Matthew Wilson and Synesis Software
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


/** \file pantheios/inserters/fmt.hpp
 *
 * [C++ only] Format constants for Pantheios inserter classes.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_FMT
#define PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_FMT

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_FMT_MAJOR    2
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_FMT_MINOR    0
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_FMT_REVISION 12
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_FMT_EDIT     23
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

#ifndef STLSOFT_INCL_STLSOFT_H_STLSOFT
# include <stlsoft/stlsoft.h>
#endif /* !STLSOFT_INCL_STLSOFT_H_STLSOFT */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{

#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Enumerations
 */

/** Format constants used by Pantheios inserter classes.
 *
 * \ingroup group__application_layer_interface__inserters
 *
 * These constants control the formatting of some of the
 * \ref group__application_layer_interface__inserters "inserter classes",
 * for example:
 *
 * \code
 pantheios::log_NOTICE("Integer: ", pantheios::integer(10, pantheios::fmt::fullHex);

 pantheios::log_NOTICE("Pointer: ", pantheios::pointer(NULL, 8 | pantheios::fmt::hex | pantheios::fmt::zeroPad);
 * \endcode
 *
 */
struct fmt
{
    enum
    {
            zeroXPrefix =   0x0100  /*!<  Applies a \c 0x prefix to the output. \note This is ignored unless \link pantheios::fmt::hex hex\endlink is also specified. */
        ,   zeroPad     =   0x0200  /*!<  Zero-pads the output. Ignored when explicitly left-aligning. */
#if defined(PANTHEIOS_OBSOLETE) || \
    defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION)
        ,   zeroPadded  =   zeroPad /*!< \deprecated This flag is deprecated, and may be removed in a future version; instead use the \link pantheios::fmt::zeroPad zeroPad\endlink flag. */
#endif /* PANTHEIOS_OBSOLETE */
        ,   hex         =   0x0400  /*!<  Represents the output in hexadecimal. */
        ,   fullHex     =   zeroXPrefix | zeroPad | hex  /*!<  A combination of the \link pantheios::fmt::zeroXPrefix zeroXPrefix\endlink, \link pantheios::fmt::zeroPad zeroPad\endlink and \link pantheios::fmt::hex hex\endlink flags */
#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
        ,   showPlus    =   0x0800  /*!< [PROVISIONAL] Shows a leading plus '+' for positively valued numeric arguments. */
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
    };
}; // namespace fmt

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Inclusion
 */

#ifdef STLSOFT_PPF_pragma_once_SUPPORT
# pragma once
#endif /* STLSOFT_PPF_pragma_once_SUPPORT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_FMT */

/* ///////////////////////////// end of file //////////////////////////// */
