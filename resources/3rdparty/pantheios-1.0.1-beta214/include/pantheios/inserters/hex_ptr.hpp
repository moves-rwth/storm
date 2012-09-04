/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters/hex_ptr.hpp
 *
 * Purpose:     String inserters for pointers in Hexadecimal notation.
 *
 * Created:     11th October 2006
 * Updated:     20th December 2012
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2006-2012, Matthew Wilson and Synesis Software
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


/** \file pantheios/inserters/hex_ptr.hpp
 *
 * [C++ only] String inserters for pointers in Hexadecimal notation.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_HEX_PTR
#define PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_HEX_PTR

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_HEX_PTR_MAJOR    1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_HEX_PTR_MINOR    3
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_HEX_PTR_REVISION 1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_HEX_PTR_EDIT     17
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_POINTER
# include <pantheios/inserters/pointer.hpp>  // for pantheios::pointer
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_POINTER */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{

#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

/** [DEPRECATED] Class for inserting pointers into Pantheios diagnostic
 *   logging statements.
 *
 * \ingroup group__application_layer_interface__inserters
 *
 * This class acts as a syntactic shorthand for the pantheios::pointer
 * inserter class, by supplying the common width/format values in a
 * defaulted constructor argument. Just as does pantheios::pointer, it
 * converts a pointer variable into a string, thereby enabling it to be
 * inserted into a logging statement.
 *
 * Consider the following statement:
 *
 * \code
  void*       p   = reinterpret_cast<void*>(0x01234567);
  char        s[] = "abc";
  std::string str("def");

  pantheios::log(pantheios::notice, "s=", s, ", p=", pantheios::hex_ptr(p), ", str=", str);
 * \endcode
 *
 * On a 32-bit system, this will produce the output:
 *
 * &nbsp;&nbsp;&nbsp;&nbsp;<b>s=abc, p=0x01234567, str=def</b>
 *
 * On a 64-bit system, this will produce the output:
 *
 * &nbsp;&nbsp;&nbsp;&nbsp;<b>s=abc, p=0x0000000001234567, str=def</b>
 *
 * \deprecated This class is now obsolete, and will be removed from a future
 *   version of Pantheios; instead use the pantheios::xp inserter class.
 */
class hex_ptr
    : public pointer
{
public: // Member Constants
    enum
    {
            pointerHexWidth = sizeof(void*) * 2 /*!< The maximum number of characters required to express any pointer value as a hex string */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
        ,   bitSize = pointerHexWidth
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
    };

public: // Construction
#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
    hex_ptr(void const volatile *p, int flags)
        : pointer(p, flags)
    {}
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

    /// Construct from a pointer, using the ambient pointer width and
    /// pantheios::fmt::fullHex format
    ///
    /// \param p The pointer whose value will be represented as a string
    explicit hex_ptr(void const volatile *p)
        : pointer(p,  pointerHexWidth, fmt::fullHex)
    {}

    /// Construct from a pointer, with width/format specifier
    ///
    /// \param p The pointer whose value will be represented as a string
    /// \param minWidth The minimum width of the field. Must be in the range
    ///   [-20, 20]
    /// \param format Combination of \link pantheios::fmt format\endlink flags
    hex_ptr(void const volatile *p, int minWidth, int format)
        : pointer(p, minWidth, format)
    {}
};

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

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_HEX_PTR */

/* ///////////////////////////// end of file //////////////////////////// */
