/* /////////////////////////////////////////////////////////////////////////
 * File:    src/shwild_string.hpp
 *
 * Purpose: String class for shwild implementation
 *
 * Created: 17th June 2005
 * Updated: 20th December 2011
 *
 * Home:    http://shwild.org/
 *
 * Copyright (c) 2005-2011, Matthew Wilson and Sean Kelly
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
 * - Neither the names of Matthew Wilson and Sean Kelly nor the names of
 *   any contributors may be used to endorse or promote products derived
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


#ifndef SHWILD_INCL_HPP_SHWILD_STRING
#define SHWILD_INCL_HPP_SHWILD_STRING

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#include <shwild/shwild.h>

#include "shwild_stlsoft.h"

#if defined(STLSOFT_CF_std_NAMESPACE) || \
    defined(STLSOFT_CF_std_NAMESPACE)

# if !defined(SHWILD_CUSTOM_STRING_CLASS)
#  include <string>
# else /* ? SHWILD_CUSTOM_STRING_CLASS */
#  include SHWILD_CUSTOM_STRING_CLASS_INCLUDE
# endif /* SHWILD_CUSTOM_STRING_CLASS */

#else /* ? STLSOFT_CF_std_NAMESPACE */

# if defined(STLSOFT_COMPILER_IS_WATCOM)
#  include <string.hpp>
#  define   SHWILD_CUSTOM_STRING_CLASS  WatcomString
# else /* ? compiler */
#  error No other non-std compiler is known
# endif /* ? compiler */

#endif /* STLSOFT_CF_std_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(SHWILD_NO_NAMESPACE)
namespace shwild
{
namespace impl
{
#endif /* !SHWILD_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

#ifndef STLSOFT_CF_std_NAMESPACE
# if defined(STLSOFT_COMPILER_IS_WATCOM)
class WatcomString
    : public String
{
public:
    WatcomString(char const *s)
        : String(s)
    {}
    WatcomString(char const *s, size_t len)
        : String(s, len)
    {}
public:
    char const  *data() const
    {
        return *this;
    }
};
# endif /* ? compiler */
#endif /* !STLSOFT_CF_std_NAMESPACE */

#if !defined(SHWILD_CUSTOM_STRING_CLASS)
typedef std::string                 shwild_string_t;
#else /* ? SHWILD_CUSTOM_STRING_CLASS */
typedef SHWILD_CUSTOM_STRING_CLASS  shwild_string_t;
#endif /* SHWILD_CUSTOM_STRING_CLASS */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(SHWILD_NO_NAMESPACE)
} // namespace impl

} // namespace shwild
#endif /* !SHWILD_NO_NAMESPACE */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !SHWILD_INCL_HPP_SHWILD_STRING */

/* ///////////////////////////// end of file //////////////////////////// */
