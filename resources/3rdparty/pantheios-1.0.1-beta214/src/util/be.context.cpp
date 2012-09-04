/* /////////////////////////////////////////////////////////////////////////
 * File:        src/util/be.context.cpp
 *
 * Purpose:     Implementation of pantheios::util::backends::Context.
 *
 * Created:     18th December 2006
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


/* Pantheios Header files */
#include <pantheios/pantheios.h>
#include <pantheios/internal/nox.h>

#include <pantheios/util/backends/context.hpp>

#include <pantheios/internal/safestr.h>

#if defined(STLSOFT_COMPILER_IS_MSVC) && \
    defined(PANTHEIOS_USING_SAFE_STR_FUNCTIONS)
# pragma warning(disable : 4996)
#endif /* compiler && Safe CRT */

#include <pantheios/backend.h>
#include <pantheios/quality/contract.h>
#include <pantheios/util/string/strdup.h>
#include <pantheios/util/system/threadid.h>
#include <pantheios/util/time/currenttime.h>

/* STLSoft Header files */
#include <stlsoft/conversion/integer_to_string.hpp>
#include <stlsoft/iterators/member_selector_iterator.hpp>
#include <stlsoft/iterators/cstring_concatenator_iterator.hpp>

/* Standard C++ Header files */
#include <algorithm>
#include <numeric>

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
namespace util
{
namespace backends
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Context
 */

inline pan_char_t* make_process_identity_(pan_char_t const* processIdentity)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != processIdentity, "process identity may not be the null string");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API('\0' != 0[processIdentity], "process identity may not be the empty string");

#if !defined(PANTHEIOS_NO_NAMESPACE)
# ifdef STLSOFT_CF_THROW_BAD_ALLOC
    using pantheios::pantheios_util_strdup_throw;
# else /* ? STLSOFT_CF_THROW_BAD_ALLOC */
    using pantheios::pantheios_util_strdup_nothrow;
# endif /* STLSOFT_CF_THROW_BAD_ALLOC */
#endif /* !PANTHEIOS_NO_NAMESPACE */

#ifdef STLSOFT_CF_THROW_BAD_ALLOC
    return pantheios_util_strdup_throw(processIdentity);
#else /* ? STLSOFT_CF_THROW_BAD_ALLOC */
    return pantheios_util_strdup_nothrow(processIdentity);
#endif /* STLSOFT_CF_THROW_BAD_ALLOC */
}

inline int check_severity_mask_(int severityMask)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(0 == (severityMask & ~int(0x0f)), "severity mask must be in the range [0, 16)");

    return severityMask;
}

/* explicit */ Context::Context(pan_char_t const* processIdentity, int id, pan_uint32_t flags, int severityMask)
    : m_processIdentity(make_process_identity_(processIdentity))
    , m_id(id)
    , m_flags(flags)
    , m_severityMask(check_severity_mask_(severityMask))
{
//    try
//    {
        // 0: "["
        // 1: process Id
        // 2: "; "
        // 3: time
        // 4: ";  "
        // 5: severity
        // 6: "]: "
        // 7: entry

        if( (m_flags & PANTHEIOS_BE_INIT_F_NO_PROCESS_ID) &&
            (m_flags & PANTHEIOS_BE_INIT_F_NO_THREAD_ID) &&
            (m_flags & PANTHEIOS_BE_INIT_F_NO_DATETIME) &&
            (m_flags & PANTHEIOS_BE_INIT_F_NO_SEVERITY))
        {
            // Nothing to do
        }
        else
        {
            if(m_flags & PANTHEIOS_BE_INIT_F_DETAILS_AT_START)
            {
                m_slice0 = pan_slice_t(-1, PANTHEIOS_LITERAL_STRING(" ["));
            }
            else
            {
                m_slice0 = pan_slice_t(-1, PANTHEIOS_LITERAL_STRING(" [") + 1);
            }

            if(0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_PROCESS_ID))
            {
                m_slice1 = pan_slice_t(-1, m_processIdentity);
            }

#if 0
            // If we're not suspending processId and any following field
            // then we need to separate them
            if( 0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_PROCESS_ID) &&
                (   0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_THREAD_ID) ||
                    0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_DATETIME) ||
                    0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_SEVERITY)))
            {
                m_slice2 = pan_slice_t(-1, PANTHEIOS_LITERAL_STRING(", "));
            }

            // If we're not suspending threadId and any following field
            // we need to separate them
            if( 0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_THREAD_ID) &&
                (   0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_DATETIME) ||
                    0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_SEVERITY)))
            {
                m_slice4 = pan_slice_t(-1, PANTHEIOS_LITERAL_STRING(", "));
            }

            // If we're not suspending date/time and any following field
            // we need to separate them
            if( 0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_DATETIME) &&
                (   0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_SEVERITY)))
            {
                m_slice6 = pan_slice_t(-1, ", ");
            }
#else /* ? 0 */
            // If we're not suspending processId and any following field
            // then we need to separate them
            if( 0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_THREAD_ID) &&
                (   0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_PROCESS_ID)))
            {
                m_slice2 = pan_slice_t(-1, PANTHEIOS_LITERAL_STRING("."));
            }

            // If we're not suspending date/time and any preceeding field
            // we need to separate them
            if( 0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_DATETIME) &&
                (   0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_PROCESS_ID) ||
                    0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_THREAD_ID)))
            {
                m_slice4 = pan_slice_t(-1, PANTHEIOS_LITERAL_STRING(", "));
            }

            // If we're not suspending severity and any preceeding field
            // we need to separate them
            if( 0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_SEVERITY) &&
                (   0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_PROCESS_ID) ||
                    0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_THREAD_ID) ||
                    0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_DATETIME)))
            {
                m_slice6 = pan_slice_t(-1, PANTHEIOS_LITERAL_STRING("; "));
            }
#endif /* 0 */

            if(m_flags & PANTHEIOS_BE_INIT_F_DETAILS_AT_START)
            {
                m_slice8 = pan_slice_t(-1, PANTHEIOS_LITERAL_STRING("]"));
            }
            else
            {
                m_slice8 = pan_slice_t(-1, PANTHEIOS_LITERAL_STRING("]: "));
            }
        }
//    }
//    catch(...)
//    {
//        pantheios::util::strfree(m_processIdentity);
//
//        throw;
//    }
}

/* virtual */ Context::~Context() throw()
{
#if !defined(PANTHEIOS_NO_NAMESPACE)
    using pantheios::pantheios_util_strfree;
#endif /* !PANTHEIOS_NO_NAMESPACE */

    pantheios_util_strfree(m_processIdentity);
}

int Context::logEntry(int severity, pan_char_t const* entry, size_t cchEntry)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_INTERNAL((m_severityMask >= 0 && m_severityMask < 16), "invalid severity mask");

    int severity4   =   severity & m_severityMask;
    int severityX   =   (severity & ~m_severityMask) >> 4;

    if( PANTHEIOS_BE_INIT_F_NO_PROCESS_ID == (m_flags & PANTHEIOS_BE_INIT_F_NO_PROCESS_ID) &&
        PANTHEIOS_BE_INIT_F_NO_THREAD_ID == (m_flags & PANTHEIOS_BE_INIT_F_NO_THREAD_ID) &&
        PANTHEIOS_BE_INIT_F_NO_DATETIME == (m_flags & PANTHEIOS_BE_INIT_F_NO_DATETIME) &&
        PANTHEIOS_BE_INIT_F_NO_SEVERITY == (m_flags & PANTHEIOS_BE_INIT_F_NO_SEVERITY))
    {
        return this->rawLogEntry(severity4, severityX, entry, cchEntry);
    }

    pan_slice_t         slices[rawLogArrayDimension];
    pan_slice_t*        slice = &slices[0];
    pan_char_t          szTime[101];
    pan_beutil_time_t   tm(STLSOFT_NUM_ELEMENTS(szTime), szTime);
    pan_char_t          num[21];
    pan_slice_t         threadId;

    // 0: "["
    // 1: process Id
    // 2: "; "
    // 3: thread Id
    // 4: "; "
    // 5: time
    // 6: ";  "
    // 5: severity
    // 8: "]: "
    // 9: entry

    if(m_flags & PANTHEIOS_BE_INIT_F_DETAILS_AT_START)
    {
        ++slice;
    }

    *slice++ = m_slice0;

    // ProcessId and its separator already sorted in the ctor

    *slice++ = m_slice1;

    *slice++ = m_slice2;

    // ThreadId

    if(0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_THREAD_ID))
    {
        threadId.ptr = stlsoft::integer_to_string(&num[0], STLSOFT_NUM_ELEMENTS(num), pantheios_getCurrentThreadId(), &threadId.len);
    }

    *slice++ = threadId;

    *slice++ = m_slice4;

    // Date time

    if(0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_DATETIME))
    {
        int timeFlags = 0;

        if(PANTHEIOS_BE_INIT_F_USE_SYSTEM_TIME & m_flags)
        {
            timeFlags |= PANTHEIOS_GETCURRENTTIME_F_USE_SYSTEM_TIME;
        }

        if(PANTHEIOS_BE_INIT_F_USE_UNIX_FORMAT & m_flags)
        {
            timeFlags |= PANTHEIOS_GETCURRENTTIME_F_USE_UNIX_FORMAT;
        }

        if(PANTHEIOS_BE_INIT_F_HIDE_DATE & m_flags)
        {
            timeFlags |= PANTHEIOS_GETCURRENTTIME_F_HIDE_DATE;
        }

        if(PANTHEIOS_BE_INIT_F_HIDE_TIME & m_flags)
        {
            timeFlags |= PANTHEIOS_GETCURRENTTIME_F_HIDE_TIME;
        }

        if(PANTHEIOS_BE_INIT_F_HIGH_RESOLUTION & m_flags)
        {
            timeFlags |= PANTHEIOS_GETCURRENTTIME_F_TIME_RES_MICROSECS;
        }
        else if(PANTHEIOS_BE_INIT_F_LOW_RESOLUTION & m_flags)
        {
            // CodeWarrior has a cow here, indicating that the |= expression
            // has no effect. So, it is commented out to avoid the compiler
            // warning, and we protect against maintenance changes by using
            // a static assert.

            STLSOFT_STATIC_ASSERT(0 == PANTHEIOS_GETCURRENTTIME_F_TIME_RES_SECONDS);

            /* timeFlags |= PANTHEIOS_GETCURRENTTIME_F_TIME_RES_SECONDS; */
        }
        else
        {
            timeFlags |= PANTHEIOS_GETCURRENTTIME_F_TIME_RES_MILLISECS;
        }

        *slice++ = pan_slice_t(szTime, pantheios_util_getCurrentTime(&tm, timeFlags));
    }
    else
    {
        slice++;
    }

    *slice++ = m_slice6;

    // Severity

    if(0 == (m_flags & PANTHEIOS_BE_INIT_F_NO_SEVERITY))
    {
        *slice++ = pan_slice_t( pantheios_getSeverityString(pan_sev_t(severity4))
                            ,   pantheios_getSeverityStringLength(pan_sev_t(severity4)));
    }
    else
    {
        ++slice;
    }

    *slice++ = m_slice8;

    if(m_flags & PANTHEIOS_BE_INIT_F_DETAILS_AT_START)
    {
        slices[0] = pan_slice_t(entry, cchEntry);
    }
    else
    {
        *slice++ = pan_slice_t(entry, cchEntry);

        STLSOFT_SUPPRESS_UNUSED(slice);
    }

    PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(static_cast<size_t>(slice - &slices[0]) == STLSOFT_NUM_ELEMENTS(slices));

#if !defined(STLSOFT_COMPILER_IS_BORLAND)

    // The sophisticated way
    const size_t cchTotal = std::accumulate(stlsoft::member_selector(&slices[0], &pan_slice_t::len)
                                        ,   stlsoft::member_selector(&slices[0] + rawLogArrayDimension, &pan_slice_t::len)
                                        ,   size_t(0));

#else /* ? compiler */

    // The crappy way, for less-than compilers
    size_t cchTotal = 0;

    { for(size_t i = 0; i != rawLogArrayDimension; ++i)
    {
        cchTotal += slices[i].len;
    }}

#endif /* compiler */


#if defined(STLSOFT_COMPILER_IS_DMC)
    return this->rawLogEntry(severity4, severityX, static_cast<const pan_slice_t (&)[rawLogArrayDimension]>(slices), cchTotal);
#else /* ? compiler */
    return this->rawLogEntry(severity4, severityX, slices, cchTotal);
#endif /* compiler */
}

/* virtual */ int Context::rawLogEntry(int severity4, int severityX, pan_char_t const* entry, size_t cchEntry)
{
    // Since all stock back-ends to this point have overload only the old,
    // two-parameter, overload, we provide a default for this method
    // implemented in terms of the overload they will have provided. New, or
    // updated, back-ends can provided this overload to act more efficiently
    // if required.

    pan_slice_t slices[rawLogArrayDimension];

    slices[0] = pan_slice_t(entry, cchEntry);

#if defined(STLSOFT_COMPILER_IS_DMC)
    return this->rawLogEntry(severity4, severityX, static_cast<const pan_slice_t (&)[rawLogArrayDimension]>(slices), cchEntry);
#else /* ? compiler */
    return this->rawLogEntry(severity4, severityX, slices, cchEntry);
#endif /* compiler */
}

/* static */ size_t Context::concatenateSlices(pan_char_t* dest, size_t cchDest, size_t numSlices, pan_slice_t const* slices)
{
    size_t numWritten = 0;

    std::copy(  slices
            ,   slices + numSlices
            ,   stlsoft::cstring_concatenator(&dest[0], &numWritten));

    PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_INTERNAL(numWritten <= cchDest, "the wrong number of characters were written");
    STLSOFT_SUPPRESS_UNUSED(cchDest);

    return numWritten;
}


pan_char_t const* Context::getProcessIdentity() const
{
    return m_processIdentity;
}

int Context::getBackEndId() const
{
    return m_id;
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} // namespace backends
} // namespace util
} // namespace pantheios
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
