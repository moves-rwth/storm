/* /////////////////////////////////////////////////////////////////////////
 * File:    src/shwild_vector.hpp
 *
 * Purpose: vector class for shwild implementation
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


#ifndef SHWILD_INCL_HPP_SHWILD_VECTORVECTOR
#define SHWILD_INCL_HPP_SHWILD_VECTORVECTOR

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#include <shwild/shwild.h>
#include "shwild_stlsoft.h"

#if defined(STLSOFT_COMPILER_IS_WATCOM)

# include <wcvector.h>

#else /* ? compiler */

# include <vector>

#endif /* compiler */

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

/** \brief Generator class for selecting a vector implementation; INTERNAL CLASS.
 *
 * This class abstracts away the choice between std::vector and other classes
 * that may also be used for shwild for translators that do not support the
 * standard library.
 */
template <class T>
struct vector_maker
{
#if defined(STLSOFT_COMPILER_IS_WATCOM)
    class type
        : public WCValVector<T>
    {
        typedef WCValVector<T>  parent_class_type;
    public:
        type()
        {}
        type(size_t length)
            : parent_class_type(length)
        {}

    public:
        void push_back(T const &t)
        {
            static_cast<parent_class_type*>(this)->operator [](size()) = t;
        }

    public:
        size_t size() const
        {
            return parent_class_type::length();
        }
    };
#else /* ? compiler */
    typedef std::vector<T>  type;
#endif /* compiler */
};

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(SHWILD_NO_NAMESPACE)
} // namespace impl

} // namespace shwild

#endif /* !SHWILD_NO_NAMESPACE */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !SHWILD_INCL_HPP_SHWILD_VECTORVECTOR */

/* ///////////////////////////// end of file //////////////////////////// */
