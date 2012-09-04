/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/inserters/adaptor.hpp
 *
 * Purpose:     Inserter adaptors for the Pantheios application layer.
 *
 * Created:     29th July 2006
 * Updated:     18th June 2012
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


/** \file pantheios/inserters/adaptor.hpp
 *
 * [C++ only] Inserter adaptors for the Pantheios application layer.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_INSERTER
#define PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_INSERTER

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_INSERTER_MAJOR       1
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_INSERTER_MINOR       2
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_INSERTER_REVISION    4
# define PANTHEIOS_VER_PANTHEIOS_INSERTERS_HPP_INSERTER_EDIT        17
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

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

/**
 *
 * \ingroup group__application_layer_interface__inserters
 *
 */
template<   ss_typename_param_k T
        ,   ss_typename_param_k S
        >
class inserter
{
public:
    typedef T                   value_type;
    typedef S                   string_type;
    typedef inserter<T, S>      class_type;

public:
    inserter(T value, S (*pfn)(T))
        : m_sourceValue(value)
        , m_convertedValue()
        , m_pfn(pfn)
        , m_bInitialised(false)
    {}

public:
    pan_char_t const* data() const
    {
        if(!m_bInitialised)
        {
            convert_();
        }

        return m_convertedValue.data();
    }
    size_t      length() const
    {
        if(!m_bInitialised)
        {
            convert_();
        }

        return m_convertedValue.length();
    }

private:
    void convert_() const
    {
        const_cast<class_type*>(this)->convert_();
    }
    void convert_()
    {
        m_convertedValue    =   m_pfn(m_sourceValue);
        m_bInitialised      =   true;
    }

private:
    T       m_sourceValue;
    S       m_convertedValue;
    S       (*m_pfn)(T);
    bool    m_bInitialised;

private:
    class_type& operator =(class_type const&);
};

/* /////////////////////////////////////////////////////////////////////////
 * Inserter functions
 */

template<   ss_typename_param_k T
        ,   ss_typename_param_k S
        >
inline inserter<T, S> insert(T &value, S (*pfn)(T))
{
    return inserter<T, S>(value, pfn);
}

#if 0
template<   ss_typename_param_k T
        ,   ss_typename_param_k S
        >
inline inserter<T const&, S> insert(T const& value, S (*pfn)(T const&))
{
    return inserter<T const&, S>(value, pfn);
}

template<   ss_typename_param_k T
        ,   ss_typename_param_k S
        >
inline inserter<T const*, S> insert(T const* value, S (*pfn)(T const*))
{
    return inserter<T const*, S>(value, pfn);
}

template<   ss_typename_param_k T
        ,   ss_typename_param_k S
        >
inline inserter<T*, S> insert(T *value, S (*pfn)(T*))
{
    return inserter<T*, S>(value, pfn);
}
#endif /* 0 */

/* /////////////////////////////////////////////////////////////////////////
 * Shim functions
 */

# if !defined(PANTHEIOS_NO_NAMESPACE)
namespace shims
{
# endif /* !PANTHEIOS_NO_NAMESPACE */

template<   ss_typename_param_k T
        ,   ss_typename_param_k S
        >
inline
# ifdef PANTHEIOS_USE_WIDE_STRINGS
wchar_t const*
c_str_data_w(inserter<T, S> const& i)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
char const*
c_str_data_a(inserter<T, S> const& i)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return i.data();
}

template<   ss_typename_param_k T
        ,   ss_typename_param_k S
        >
inline
size_t
# ifdef PANTHEIOS_USE_WIDE_STRINGS
c_str_len_w(inserter<T, S> const& i)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
c_str_len_a(inserter<T, S> const& i)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
{
    return i.length();
}

# if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace shims */

#  if defined(STLSOFT_COMPILER_IS_GCC)
    /* GCC does not seem to correctly handle the phases of
     * processing of C++ templates, so we need to 'use' the
     * shims into the same namespace as the inserter class
     * in order that ADL can suffice instead.
     */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
    using ::pantheios::shims::c_str_data_w;
    using ::pantheios::shims::c_str_len_w;
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    using ::pantheios::shims::c_str_data_a;
    using ::pantheios::shims::c_str_len_a;
# endif /* PANTHEIOS_USE_WIDE_STRINGS */

#  endif /* compiler */

# endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */

namespace stlsoft
{
    // 'Export' the string access shims into the STLSoft namespace
    //
    // c_str_ptr(_a) is not necessary for version 1.0 of Pantheios, but it's
    // defined and exported in order to allow for the case where someone
    // may find a legitimate use for the conversion classes additional to
    // the type-tunneling of the Pantheios API.

# ifdef PANTHEIOS_USE_WIDE_STRINGS
    using ::pantheios::shims::c_str_data_w;
    using ::pantheios::shims::c_str_len_w;
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    using ::pantheios::shims::c_str_data_a;
    using ::pantheios::shims::c_str_len_a;
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
}

#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Inclusion
 */

#ifdef STLSOFT_PPF_pragma_once_SUPPORT
# pragma once
#endif /* STLSOFT_PPF_pragma_once_SUPPORT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_INSERTERS_HPP_INSERTER */

/* ///////////////////////////// end of file //////////////////////////// */
