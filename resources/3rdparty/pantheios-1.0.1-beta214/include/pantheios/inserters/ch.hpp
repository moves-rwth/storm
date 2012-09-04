/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters/ch.hpp
 *
 * Purpose:     Shorthand inserter ch for pantheios::character.
 *
 * Created:     21st June 2005
 * Updated:     16th October 2010
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


/** \file pantheios/inserters/ch.hpp
 *
 * [C++ only] Definition of the pantheios::character string inserter
 *   for built-in character type(s).
 *
 * This file is not included by default.
 *
<pre>
 \#include &lt;pantheios/inserters/character.hpp>
 \#include &lt;pantheios/pantheios.hpp>

 . . .

 char c = '~';
 pantheios::log_DEBUG("c=", pantheios::character(c));
</pre>
 *
 * The short alias \c ch is available, via inclusion of 
 * pantheios/inserters/ch.hpp
 *
<pre>
 \#include &lt;pantheios/inserters/ch.hpp>

 . . .

 char c = '~';
 pantheios::log_DEBUG("c=", pantheios::ch(c));
</pre>
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_CH
#define PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_CH

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_CH_MAJOR     1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_CH_MINOR     0
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_CH_REVISION  1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_CH_EDIT      1
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */
#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_CHARACTER
# include <pantheios/inserters/character.hpp>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_CHARACTER */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{

#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Inserter classes
 */

/** Class for inserting characters into Pantheios diagnostic logging
 *   statements.
 *
 * \ingroup group__application_layer_interface__inserters
 *
 * This class converts a character variable into a string, thereby enabling
 * it to be inserted into a logging statement. Consider the following
 * statement:
 *
 * \code
  char        c   = '#';
  char        s[] = "abc";
  std::string str("def");

  pantheios::log(pantheios::notice, "s=", s, ", c=", pantheios::character(c), ", str=", str);
 * \endcode
 *
 * This will produce the output:
 *
 * &nbsp;&nbsp;&nbsp;&nbsp;<b>s=abc, c=#, str=def</b>
 *
 * \note Currently, Pantheios does not support the insertion of character types
 * in diagnostic logging statements, due to the various ambiguities inherent
 * in the C++ language. (See chapters 14, 15, 19, 24 of
 * <a href = "http://imperfectcplusplus.com" target="_blank">Imperfect C++</a>
 * for discussions of these issues.) It is possible that a future version of
 * the library will be able to incorporate them directly, so long as that does
 * not sacrifice Pantheios's central claim of not paying for what you don't use.
 */
typedef character ch;

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

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_CH */

/* ///////////////////////////// end of file //////////////////////////// */
