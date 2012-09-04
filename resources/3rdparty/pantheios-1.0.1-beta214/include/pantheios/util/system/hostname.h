/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/util/system/hostname.h
 *
 * Purpose:     Functions for eliciting host name
 *
 * Created:     14th April 2008
 * Updated:     10th August 2009
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2008-2009, Matthew Wilson and Synesis Software
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


/** \file pantheios/util/system/hostname.h
 *
 * [C, C++] Functions for eliciting host name
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_SYSTEM_H_HOSTNAME
#define PANTHEIOS_INCL_PANTHEIOS_SYSTEM_H_HOSTNAME

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_SYSTEM_H_HOSTNAME_MAJOR    1
# define PANTHEIOS_VER_PANTHEIOS_SYSTEM_H_HOSTNAME_MINOR    2
# define PANTHEIOS_VER_PANTHEIOS_SYSTEM_H_HOSTNAME_REVISION 1
# define PANTHEIOS_VER_PANTHEIOS_SYSTEM_H_HOSTNAME_EDIT     12
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */
#ifndef PANTHEIOS_INCL_PANTHEIOS_QUALITY_H_CONTRACT
# include <pantheios/quality/contract.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_QUALITY_H_CONTRACT */
#ifndef STLSOFT_INCL_STLSOFT_H_STLSOFT
# include <stlsoft/stlsoft.h>
#endif /* !STLSOFT_INCL_STLSOFT_H_STLSOFT */
#ifdef __cplusplus
# if defined(STLSOFT_COMPILER_IS_MSVC) && \
     _MSC_VER < 1300
#  include <pantheios/util/memory/auto_buffer_selector.hpp>
# endif /* VC6 */
# ifndef STLSOFT_INCL_STLSOFT_MEMORY_HPP_AUTO_BUFFER
#  include <stlsoft/memory/auto_buffer.hpp>
# endif /* !STLSOFT_INCL_STLSOFT_MEMORY_HPP_AUTO_BUFFER */
#endif /* __cplusplus */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/** Retrieves the host system identifier
 *
 * \ingroup group__utility
 *
 * \param buffer Pointer to a character buffer to receive a copy of the host
 *   name. May not be <code>NULL</code> unless <code>cchBuffer</code> is 0
 * \param cchBuffer Number of characters available in <code>buffer</code>.
 *   Must include the space for a terminating nul.
 *
 * \return The number of characters copied into the destination buffer, or
 *   an error indication code.
 * \retval 0 The host name was unavailable. (For those builds for which
 *   exceptions are not supported, this will include out-of-memory
 *   conditions.)
 * \retval cchBuffer There was insufficient space to copy the host name into
 *   the buffer. The host name is <b>not</b> copied into the buffer.
 *
 * \remarks This function abstracts the underlying system call(s) of the
 * supported operating systems. The somewhat awkward semantics - indicating
 * exhaustion by returning the requested size rather than the required
 * size - is a result of the limitation of the UNIX
 * <code>gethostname()</code> function. Since the function is intended to be
 * mostly used, via the C++ wrapper, from the pantheios::hostId inserter
 * class, the inconvenience is deemed acceptable.
 */
PANTHEIOS_CALL(size_t) pantheios_getHostName(pan_char_t* buffer, size_t cchBuffer);

#if !defined(PANTHEIOS_NO_NAMESPACE)
/** Equivalent to \ref pantheios::pantheios_getHostName "pantheios_getHostName()"
 *
 * \ingroup group__utility
 */
inline size_t getHostName(pan_char_t* buffer, size_t cchBuffer)
{
    return pantheios_getHostName(buffer, cchBuffer);
}
#endif /* !PANTHEIOS_NO_NAMESPACE */

#ifdef __cplusplus

/** Retrieves the host system identifier
 *
 * \ingroup group__utility
 *
 * \param buffer A mutable (non-const) reference to an instance of a
 *   specialisation of <code>stlsoft::auto_buffer</code> in which the
 *   elicited host-name will be written, including terminating
 *   nul-character
 */
#if defined(STLSOFT_COMPILER_IS_MSVC) && \
    _MSC_VER < 1300
template <typename B>
inline size_t getHostName(B& buffer)
#else /* ? compiler */
template <size_t N, typename A>
#  ifdef STLSOFT_AUTO_BUFFER_NEW_FORM
inline size_t getHostName(stlsoft::auto_buffer<pan_char_t, N, A>& buffer)
#  else /* ? STLSOFT_AUTO_BUFFER_NEW_FORM */
inline size_t getHostName(stlsoft::auto_buffer<pan_char_t, A, N>& buffer)
#  endif /* STLSOFT_AUTO_BUFFER_NEW_FORM */
#endif /* compiler */
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(0 != buffer.size(), "buffer must not be empty");

    for(;;)
    {
        size_t cch = pantheios_getHostName(&buffer[0], buffer.size());

        if(buffer.size() == cch)
        {
            if(!buffer.resize(2 * buffer.size()))
            {
                return 0;
            }
        }
        else
        {
            buffer.resize(cch);

            break;
        }
    }

    return buffer.size();
}

#endif /* __cplusplus */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_SYSTEM_H_HOSTNAME */

/* ///////////////////////////// end of file //////////////////////////// */
