/* /////////////////////////////////////////////////////////////////////////
 * File:        inserters/processid.cpp
 *
 * Purpose:     Implementation of the inserter classes.
 *
 * Created:     16th October 2006
 * Updated:     5th August 2012
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


#define PANTHEIOS_NO_INCLUDE_STLSOFT_STRING_ACCESS

/* Pantheios Header Files */
#include <pantheios/pantheios.h>
#include <pantheios/internal/lean.h>

#include <pantheios/inserters/processid.hpp>
#include <pantheios/util/system/processid.h>

#include <pantheios/quality/contract.h>

#include <stlsoft/conversion/integer_to_string.hpp>
#include <stlsoft/shims/access/string/std/c_string.h>

/* /////////////////////////////////////////////////////////////////////////
 * Warning suppression
 */

#if defined(STLSOFT_COMPILER_IS_BORLAND)
# pragma warn -8008
# pragma warn -8066
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{

#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

struct processId_t const* processId =   0;

/* /////////////////////////////////////////////////////////////////////////
 * Helper functions
 */

namespace
{

    void pantheios_getProcessId_(pan_char_t const** processId, size_t* processIdLength)
    {
        // NOTE: This function is _not_ threadsafe. However, this does not matter
        // because any overwrite, should it occur, will be entirely benign

        static pan_char_t           s_processIdBuff[21];    // Large enough for 64-bit signed/unsigned integer
        static size_t               s_processIdLength;
        static pan_char_t const*    s_processId = ::stlsoft::integer_to_string(&s_processIdBuff[0], STLSOFT_NUM_ELEMENTS(s_processIdBuff), pantheios_getCurrentProcessId(), &s_processIdLength);

        if(NULL != processId)
        {
            *processId = s_processId;
        }

        if(NULL != processIdLength)
        {
            *processIdLength = s_processIdLength;
        }
    }

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * API functions
 */

PANTHEIOS_CALL(pan_char_t const*) pantheios_processId(void)
{
    return pantheios_getCurrentProcessIdString();
}

PANTHEIOS_CALL(pan_char_t const*) pantheios_getCurrentProcessIdString(void)
{
    pan_char_t const* processId;

    pantheios_getProcessId_(&processId, NULL);

    return processId;
}

PANTHEIOS_CALL(size_t) pantheios_processIdLength(void)
{
    return pantheios_getCurrentProcessIdStringLength();
}

PANTHEIOS_CALL(size_t) pantheios_getCurrentProcessIdStringLength(void)
{
    size_t  processIdLen;

    pantheios_getProcessId_(NULL, &processIdLen);

    return processIdLen;
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
