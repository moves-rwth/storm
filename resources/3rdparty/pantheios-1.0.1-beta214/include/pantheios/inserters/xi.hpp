/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters/xi.hpp
 *
 * Purpose:     Defines shorthand name for pantheios::integer inserter
 *              class for inserting integers in hexadecimal format.
 *
 * Created:     12th March 2010
 * Updated:     26th November 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/inserters/xi.hpp
 *
 * [C++ only] Defines shorthand name for pantheios::integer inserter class
 *  for inserting integers in hexadecimal format.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_XI
#define PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_XI

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_XI_MAJOR     1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_XI_MINOR     0
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_XI_REVISION  4
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_XI_EDIT      6
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_INTEGER
# include <pantheios/inserters/integer.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_INTEGER */

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

/** Shortcut inserter class for inserting integers in a hexadecimal format.
 */
class xi
  : public integer
{
public:
    /// The parent class type
    typedef integer     parent_class_type;
    /// This type
    typedef xi          class_type;

private:
    // These helpers have to be here, otherwise CodeWarrior doesn't see them
    static int get_default_width_(size_t numBytes)
    {
        return 2 + int(2 * numBytes);
    }
    static int validate_width_(int width)
    {
        return width;
    }

public:
#ifdef STLSOFT_CF_8BIT_INT_EXTENDED_TYPE_IS_DISTINCT
    ///  Construct from an 8-bit signed integer
    explicit xi(::stlsoft::sint8_t i, int width = get_default_width_(1))
        : parent_class_type(i, validate_width_(width), int(fmt::fullHex))
    {}
    ///  Construct from an 8-bit unsigned integer
    explicit xi(::stlsoft::uint8_t i, int width = get_default_width_(1))
          : parent_class_type(i, validate_width_(width), int(fmt::fullHex))
    {}
#endif // STLSOFT_CF_8BIT_INT_EXTENDED_TYPE_IS_DISTINCT
#ifdef STLSOFT_CF_16BIT_INT_EXTENDED_TYPE_IS_DISTINCT
    ///  Construct from a 16-bit signed integer
    explicit xi(::stlsoft::sint16_t i, int width = get_default_width_(2))
          : parent_class_type(i, validate_width_(width), int(fmt::fullHex))
    {}
    ///  Construct from a 16-bit unsigned integer
    explicit xi(::stlsoft::uint16_t i, int width = get_default_width_(2))
          : parent_class_type(i, validate_width_(width), int(fmt::fullHex))
    {}
#endif // STLSOFT_CF_16BIT_INT_EXTENDED_TYPE_IS_DISTINCT
#if defined(STLSOFT_CF_32BIT_INT_EXTENDED_TYPE_IS_DISTINCT) || \
( defined(STLSOFT_COMPILER_IS_MSVC) && \
  _MSC_VER == 1200) /* NOTE: This is a hack, and should be updated when STLSoft 1.10 is released */
    ///  Construct from a 32-bit signed integer
    explicit xi(::stlsoft::sint32_t i, int width = get_default_width_(4))
          : parent_class_type(i, validate_width_(width), int(fmt::fullHex))
    {}
    ///  Construct from a 32-bit unsigned integer
    explicit xi(::stlsoft::uint32_t i, int width = get_default_width_(4))
          : parent_class_type(i, validate_width_(width), int(fmt::fullHex))
    {}
#endif // STLSOFT_CF_32BIT_INT_EXTENDED_TYPE_IS_DISTINCT
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
    ///  Construct from a 64-bit signed integer
    explicit xi(::stlsoft::sint64_t i, int width = get_default_width_(8))
          : parent_class_type(i, validate_width_(width), int(fmt::fullHex))
    {}
    ///  Construct from a 64-bit unsigned integer
    explicit xi(::stlsoft::uint64_t i, int width = get_default_width_(8))
          : parent_class_type(i, validate_width_(width), int(fmt::fullHex))
    {}
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
    ///  Construct from an int
    explicit xi(int i, int width = get_default_width_(sizeof(int)))
          : parent_class_type(i, validate_width_(width), int(fmt::fullHex))
    {}
    ///  Construct from an unsigned int
    explicit xi(unsigned int i, int width = get_default_width_(sizeof(unsigned int)))
          : parent_class_type(i, validate_width_(width), int(fmt::fullHex))
    {}
    ///  Construct from a long
    explicit xi(long i, int width = get_default_width_(sizeof(long)))
          : parent_class_type(i, validate_width_(width), int(fmt::fullHex))
    {}
    ///  Construct from an unsigned long
    explicit xi(unsigned long i, int width = get_default_width_(sizeof(unsigned long)))
          : parent_class_type(i, validate_width_(width), int(fmt::fullHex))
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

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_XI */

/* ///////////////////////////// end of file //////////////////////////// */
