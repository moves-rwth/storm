/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters/variant_bool.hpp
 *
 * Purpose:     String inserter for booleans.
 *
 * Created:     18th June 2012
 * Updated:     18th June 2012
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2012, Matthew Wilson and Synesis Software
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


/** \file pantheios/inserters/variant_bool.hpp
 *
 * [C++ only] String inserter for booleans.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_VARIANT_BOOL
#define PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_VARIANT_BOOL

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_VARIANT_BOOL_MAJOR       1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_VARIANT_BOOL_MINOR       0
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_VARIANT_BOOL_REVISION    1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_VARIANT_BOOL_EDIT        1
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_BOOLEAN
# include <pantheios/inserters/boolean.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_BOOLEAN */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#else /* ? !PANTHEIOS_NO_NAMESPACE */
# if defined(__RPCNDR_H__)
#  error pantheios::variant_bool may not be used with Windows when namespace is suspended. If you are including pantheios/inserters.hpp, try including only those specific inserter files that you need
# endif /* rpcndr.h */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

/** Class for inserting VARIANT_BOOL variables into Pantheios diagnostic
 *   logging statements.
 *
 * \ingroup group__application_layer_interface__inserters
 *
 * Consider the following statement:
 *
 * \code
  char          s[] = "abc";
  std::string   str("def");
  VARIANT_BOOL  b   = VARIANT_FALSE;

  pantheios::log(pantheios::notice, "s=", s, ", b=", pantheios::variant_bool(b), ", str=", str);
 * \endcode
 *
 * This will produce the output:
 *
 * &nbsp;&nbsp;&nbsp;&nbsp;<b>s=abc, b=false, str=def</b>
 *
 * \note The class provides the static method set_value_strings() for
 *   assigning user-defined strings to represent true and false values.
 */
class variant_bool
    : public boolean
{
/// \name Member Types
/// @{
public:
    typedef boolean         parent_class_type;
    typedef variant_bool    class_type;
/// @}

/// \name Construction
/// @{
public:
    /// Construct from a variant_bool value
    ///
    /// \param value The variant_bool whose value will be represented as a string
    explicit variant_bool(VARIANT_BOOL value)
        : parent_class_type(VARIANT_FALSE != value)
    {}
private:
    class_type& operator =(class_type const&);
/// @}

/// \name Implementation
/// @{
public:
    virtual
    pan_slice_t const* get_slice_(bool value) const
    {
        return boolean::get_slice_(1, value);
    }
/// @}
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

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_VARIANT_BOOL */

/* ///////////////////////////// end of file //////////////////////////// */
