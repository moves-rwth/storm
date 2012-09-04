/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/util/memory/auto_buffer_selector.hpp
 *
 * Purpose:     Type generator template for defining stlsoft::auto_buffer
 *              specialisations.
 *
 * Created:     9th November 2007
 * Updated:     14th February 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2007-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/util/memory/auto_buffer_selector.hpp
 *
 * [C++ only] Type generator template (see 12.2 of Extended STL,
 *   volume 1; http://extendedstl.com/) for defining stlsoft::auto_buffer
 *   specialisations.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_UTIL_MEMORY_HPP_AUTO_BUFFER_SELECTOR
#define PANTHEIOS_INCL_PANTHEIOS_UTIL_MEMORY_HPP_AUTO_BUFFER_SELECTOR

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_UTIL_MEMORY_HPP_AUTO_BUFFER_SELECTOR_MAJOR     1
# define PANTHEIOS_VER_PANTHEIOS_UTIL_MEMORY_HPP_AUTO_BUFFER_SELECTOR_MINOR     2
# define PANTHEIOS_VER_PANTHEIOS_UTIL_MEMORY_HPP_AUTO_BUFFER_SELECTOR_REVISION  2
# define PANTHEIOS_VER_PANTHEIOS_UTIL_MEMORY_HPP_AUTO_BUFFER_SELECTOR_EDIT      10
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

#if defined(STLSOFT_COMPILER_IS_MSVC) && \
    _MSC_VER >= 1400
# define PANTHEIOS_COMPILER_CF_STD_ALLOCATOR_SYMBOLS_MULTIPLY_DEFINED
#endif /* compiler */

#ifndef PANTHEIOS_COMPILER_CF_STD_ALLOCATOR_SYMBOLS_MULTIPLY_DEFINED
# ifndef STLSOFT_INCL_STLSOFT_MEMORY_HPP_ALLOCATOR_SELECTOR
#  include <stlsoft/memory/allocator_selector.hpp>
# endif /* !STLSOFT_INCL_STLSOFT_MEMORY_HPP_ALLOCATOR_SELECTOR */
#endif /* !PANTHEIOS_COMPILER_CF_STD_ALLOCATOR_SYMBOLS_MULTIPLY_DEFINED */
#ifndef STLSOFT_INCL_STLSOFT_MEMORY_HPP_AUTO_BUFFER
# include <stlsoft/memory/auto_buffer.hpp>
#endif /* !STLSOFT_INCL_STLSOFT_MEMORY_HPP_AUTO_BUFFER */
#ifdef PANTHEIOS_COMPILER_CF_STD_ALLOCATOR_SYMBOLS_MULTIPLY_DEFINED
# ifndef STLSOFT_INCL_STLSOFT_MEMORY_HPP_MALLOC_ALLOCATOR
#  include <stlsoft/memory/malloc_allocator.hpp>
# endif /* !STLSOFT_INCL_STLSOFT_MEMORY_HPP_MALLOC_ALLOCATOR */
#endif /* PANTHEIOS_COMPILER_CF_STD_ALLOCATOR_SYMBOLS_MULTIPLY_DEFINED */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
namespace util
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

/** <a href = "http://extendedstl.com/glossary.html#type_generator">Type-generator template</a>
 *   that deduces the appropriate specialisation of
 *   <code>stlsoft::auto_buffer</code>.
 *
 * There are two reasons for this:
 * - Some (older) compilers have to use the old (pre-1.9) form of
 *   <code>stlsoft::auto_buffer</code>. Using
 *   pantheios::auto_buffer_selector makes Pantheios code independent of
 *   such compilers' behavioural odities.
 * - Visual C++ 8 & 9 both have  bug that causes them to exhibit a linker
 *   error (LNK2005) whereby std::allocator<>::allocate() and
 *   std::allocator<>::deallocate() are multiply defined. In that case, we
 *   always select stlsoft::malloc_allocator as the allocator, as this will
 *   still hook into the VC++ memory tracing debug API.
 */
template<   typename    T
        ,   size_t      N
#ifdef PANTHEIOS_COMPILER_CF_STD_ALLOCATOR_SYMBOLS_MULTIPLY_DEFINED
        ,   typename    A = stlsoft::malloc_allocator<T>
#else /* ? PANTHEIOS_COMPILER_CF_STD_ALLOCATOR_SYMBOLS_MULTIPLY_DEFINED */
        ,   typename    A = ss_typename_type_def_k stlsoft::allocator_selector<T>::allocator_type
#endif /* PANTHEIOS_COMPILER_CF_STD_ALLOCATOR_SYMBOLS_MULTIPLY_DEFINED */
        >
struct auto_buffer_selector
{
    /** The generated type */
#ifdef STLSOFT_AUTO_BUFFER_NEW_FORM
    typedef stlsoft::auto_buffer<T, N, A>           type;
#else /* ? STLSOFT_AUTO_BUFFER_NEW_FORM */
    typedef stlsoft::auto_buffer<T, A, N>           type;
#endif /* STLSOFT_AUTO_BUFFER_NEW_FORM */
};

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace util */
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Inclusion
 */

#ifdef STLSOFT_PPF_pragma_once_SUPPORT
# pragma once
#endif /* STLSOFT_PPF_pragma_once_SUPPORT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_UTIL_MEMORY_HPP_AUTO_BUFFER_SELECTOR */

/* ///////////////////////////// end of file //////////////////////////// */
