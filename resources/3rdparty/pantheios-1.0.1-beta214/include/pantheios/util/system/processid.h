/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/util/system/processid.h
 *
 * Purpose:     Functions for eliciting process identifier
 *
 * Created:     25th July 2008
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


/** \file pantheios/util/system/processid.h
 *
 * [C, C++] Functions for eliciting process identifier
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_SYSTEM_H_PROCESSID
#define PANTHEIOS_INCL_PANTHEIOS_SYSTEM_H_PROCESSID

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_SYSTEM_H_PROCESSID_MAJOR       1
# define PANTHEIOS_VER_PANTHEIOS_SYSTEM_H_PROCESSID_MINOR       0
# define PANTHEIOS_VER_PANTHEIOS_SYSTEM_H_PROCESSID_REVISION    1
# define PANTHEIOS_VER_PANTHEIOS_SYSTEM_H_PROCESSID_EDIT        4
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

/** Returns an identifier for the process.
 *
 * \ingroup group__utility
 */
PANTHEIOS_CALL(stlsoft_ns_qual(ss_sint64_t)) pantheios_getCurrentProcessId(void);


#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace util
{

/** Equivalent to \ref pantheios::pantheios_getCurrentProcessId "pantheios_getCurrentProcessId()".
 *
 * \ingroup group__utility
 */
inline stlsoft_ns_qual(ss_sint64_t) getCurrentProcessId()
{
    return pantheios_getCurrentProcessId();
}

} /* namespace util */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_SYSTEM_H_PROCESSID */

/* ///////////////////////////// end of file //////////////////////////// */
