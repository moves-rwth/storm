/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters/xp.hpp
 *
 * Purpose:     Defines shorthand name for pantheios::pointer inserter
 *              class for inserting pointers in hexadecimal format.
 *
 * Created:     23rd March 2010
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


/** \file pantheios/inserters/xp.hpp
 *
 * [C++ only] Defines shorthand name for pantheios::pointer inserter class
 *  for inserting pointers in hexadecimal format.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_XP
#define PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_XP

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_XP_MAJOR     1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_XP_MINOR     1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_XP_REVISION  1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_XP_EDIT      4
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_POINTER
# include <pantheios/inserters/pointer.hpp>
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

/** Shortcut inserter class for inserting pointers in a hexadecimal format.
 */
class xp
  : public pointer
{
public:
    /// The parent class type
    typedef pointer     parent_class_type;
    /// This type
    typedef xp          class_type;

private:
    // These helpers have to be here, otherwise CodeWarrior doesn't see them
    static int get_default_width_(size_t numBytes)
    {
        // TODO: Add the 2 back in when pointer writing adjusted as integer was done in 1.0.1b197
        return /* 2 + */ int(2 * numBytes);
    }
    static int validate_width_(int width)
    {
        return width;
    }

public:
    ///  Construct from an unsigned long
    explicit xp(void const volatile* pv, int width = get_default_width_(sizeof(void*)))
          : parent_class_type(pv, validate_width_(width), int(fmt::fullHex))
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

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_XP */

/* ///////////////////////////// end of file //////////////////////////// */
