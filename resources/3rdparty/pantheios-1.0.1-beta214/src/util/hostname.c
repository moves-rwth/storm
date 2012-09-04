/* /////////////////////////////////////////////////////////////////////////
 * File:        src/util/hostname.c
 *
 * Purpose:     Time functions for use in Pantheios back-ends.
 *
 * Created:     22nd August 2006
 * Updated:     10th August 2009
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2006-2009, Matthew Wilson and Synesis Software
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


#include <pantheios/pantheios.h>
#include <pantheios/internal/lean.h>
#include <pantheios/quality/contract.h>
#include <pantheios/internal/safestr.h>

#include <pantheios/util/system/hostname.h>

#include <platformstl/platformstl.h>

#include <string.h>
#if defined(PLATFORMSTL_OS_IS_UNIX)
# include <errno.h>
# include <unistd.h>
#elif defined(PLATFORMSTL_OS_IS_WINDOWS)
# include <windows.h>
#else /* ? PLATFORMSTL_OS_IS_???? */
# error Platform not discriminated
#endif /* PLATFORMSTL_OS_IS_???? */

/* /////////////////////////////////////////////////////////////////////////
 * Character string encoding support
 */

/* We _could_ just use GetComputerName() here, and rely on a correlation between
 * the presence/absence of UNICODE/_UNICODE and PANTHEIOS_USE_WIDE_STRINGS, but
 * if the user has specified PANTHEIOS_USE_WIDE_STRINGS or
 * PANTHEIOS_NO_USE_WIDE_STRINGS the correlation would be broken (along with the
 * build
 */

#ifdef PANTHEIOS_USE_WIDE_STRINGS

# define pan_GetComputerName_       GetComputerNameW
# define pan_strlen_                wcslen

#else /* ? PANTHEIOS_USE_WIDE_STRINGS */

# define pan_GetComputerName_       GetComputerNameA
# define pan_strlen_                strlen

#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* /////////////////////////////////////////////////////////////////////////
 * Helper Functions
 */

# if defined(UNIXSTL_OS_IS_MACOSX)
/* On Mac OS-X (10.5, at least) errno is not set to ENAMETOOLONG in
 * the case where the buffer is smaller than the hostname length.
 *
 * So, we must establish this for ourselves, as follows:
 */
int pantheios_gethostname_with_errno_fix_(pan_char_t* buffer, size_t cchBuffer)
{
    int res = gethostname(&buffer[0], cchBuffer);

    if(0 != cchBuffer)
    {
        size_t n2 = pan_strlen_(buffer);

        if(n2 + 1 < cchBuffer)
        {
            errno = 0;
        }
        else
        {
            errno = ENAMETOOLONG;
            res = -1;
        }
    }

    return res;
}
#  define gethostname   pantheios_gethostname_with_errno_fix_
# endif /* Mac OS-X */

static size_t pantheios_getHostName_body_(pan_char_t* buffer, size_t cchBuffer)
{
#if defined(PLATFORMSTL_OS_IS_UNIX)

    if(0 == cchBuffer)
    {
        errno = ENAMETOOLONG;

        return 0;
    }
    else
    {
        int res;

        PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(NULL != buffer);

        /* First, we mark the last available character in the buffer with
         * the nul terminator, to test later
         */
        buffer[cchBuffer - 1] = '\0';

        errno = 0;

        res = gethostname(&buffer[0], cchBuffer);

        /* Test for ENAMETOOLONG, to avoid any implementation-dependent
         * behaviour wrt whether this it is set on a 0 or a -1 return
         */
        if(ENAMETOOLONG == errno)
        {
            /* To homogenise platform behaviour, we ensure that no fragment is filled out */
            buffer[0] = '\0';

            return cchBuffer;
        }
        else
        {
            if(0 != res)
            {
                /* Was an error, so return 0 */
                return 0;
            }
            else if('\0' != buffer[cchBuffer - 1])
            {
                /* Was insufficient buffer, so we return the given size (which is the
                 * error indicator for that condition
                 *
                 * Also, to homogenise platform behaviour, we ensure that no fragment
                 * of the buffer is returned as filled out.
                 */
                buffer[0] = '\0';

                return cchBuffer;
            }
            else
            {
                /* Buffer was sufficient, and the value has been written. The only
                 * way to return the length is to do strlen on it
                 */

                return pan_strlen_(buffer);
            }
        }
    }

#elif defined(PLATFORMSTL_OS_IS_WINDOWS)

    DWORD cchSize = stlsoft_static_cast(DWORD, cchBuffer);

    if(!pan_GetComputerName_(&buffer[0], &cchSize))
    {
        DWORD   err = GetLastError();

        if(ERROR_BUFFER_OVERFLOW == err)
        {
            return cchBuffer;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return cchSize;
    }

#else /* ? PLATFORMSTL_OS_IS_???? */
# error Platform not discriminated
#endif /* PLATFORMSTL_OS_IS_???? */
}

/* /////////////////////////////////////////////////////////////////////////
 * API Functions
 */

size_t pantheios_getHostName(pan_char_t* buffer, size_t cchBuffer)
{
    size_t  res;

    /* Preconditions */
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API((0 == cchBuffer || NULL != buffer), "character buffer pointer may only be null if the indicated size is 0");

    /* Body */
    res = pantheios_getHostName_body_(buffer, cchBuffer);

    /* Postconditions */
    PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API(res <= cchBuffer, "the result must not exceed the buffer length");
    PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API((0 == res || cchBuffer == res || pan_strlen_(buffer) < cchBuffer), "the result must be 0, or the buffer must be zero, or the length of the written string is less than the buffer length");

    return res;
}

/* ///////////////////////////// end of file //////////////////////////// */
