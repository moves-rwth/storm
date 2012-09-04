/* /////////////////////////////////////////////////////////////////////////
 * File:        src/util/core.apidefs.cpp
 *
 * Purpose:     Utility functions for use in Pantheios back- and front-
 *              ends.
 *
 * Created:     19th August 2007
 * Updated:     27th December 2010
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
#include <pantheios/util/core/apidefs.hpp>

#include <pantheios/backend.h>
#include <pantheios/quality/contract.h>

#include <exception>
#include <new>

/* /////////////////////////////////////////////////////////////////////////
 * API functions
 */

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT

PANTHEIOS_CALL(int) pantheios_call_fe_init(
    pantheios_fe_X_init_pfn_t   pfn
,   void*                       reserved
,   void**                      ptoken
)
{
    try
    {
        return pfn(reserved, ptoken);
    }
    catch(std::bad_alloc&)
    {
        return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
    }
    catch(std::exception&)
    {
        return PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
    }
    catch(...)
    {
        return PANTHEIOS_INIT_RC_UNKNOWN_FAILURE;
    }
}

PANTHEIOS_CALL(int) pantheios_call_fe_uninit(
    pantheios_fe_X_uninit_pfn_t pfn
,   void*                       token
)
{
    try
    {
        return pfn(token);
    }
    catch(std::bad_alloc&)
    {
        return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
    }
    catch(std::exception&)
    {
        return PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
    }
    catch(...)
    {
        return PANTHEIOS_INIT_RC_UNKNOWN_FAILURE;
    }
}

PANTHEIOS_CALL(PAN_CHAR_T const*) pantheios_call_fe_getProcessIdentity(
    pantheios_fe_X_getProcessIdentity_pfn_t pfn
,   void*                                   token
)
{
    try
    {
        return pfn(token);
    }
    catch(...)
    {
        return PANTHEIOS_LITERAL_STRING("");
    }
}

PANTHEIOS_CALL(int) pantheios_call_fe_isSeverityLogged(
    pantheios_fe_X_isSeverityLogged_pfn_t   pfn
,   void*                                   token
,   int                                     severity
,   int                                     backEndId
)
{
    try
    {
        return pfn(token, severity, backEndId);
    }
    catch(std::bad_alloc&)
    {
        return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
    }
    catch(std::exception&)
    {
        return PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
    }
    catch(...)
    {
        return PANTHEIOS_INIT_RC_UNKNOWN_FAILURE;
    }
}




PANTHEIOS_CALL(int) pantheios_call_be_void_init(
    pantheios_be_X_init_pfn_t   pfn
,   PAN_CHAR_T const*           processIdentity
,   int                         backEndId
,   void const*                 init
,   void*                       reserved
,   void**                      ptoken
)
{
    try
    {
        return pfn(processIdentity, backEndId, init, reserved, ptoken);
    }
    catch(std::bad_alloc&)
    {
        return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
    }
    catch(std::exception&)
    {
        return PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
    }
    catch(...)
    {
        return PANTHEIOS_INIT_RC_UNKNOWN_FAILURE;
    }
}


PANTHEIOS_CALL(int) pantheios_call_be_uninit(
    pantheios_be_X_uninit_pfn_t pfn
,   void*                       token
)
{
    try
    {
        return pfn(token);
    }
    catch(std::bad_alloc&)
    {
        return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
    }
    catch(std::exception&)
    {
        return PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
    }
    catch(...)
    {
        return PANTHEIOS_INIT_RC_UNKNOWN_FAILURE;
    }
}


PANTHEIOS_CALL(int) pantheios_call_be_logEntry(
    pantheios_be_X_logEntry_pfn_t   pfn
,   void*                           feToken
,   void*                           beToken
,   int                             severity
,   PAN_CHAR_T const*               entry
,   size_t                          cchEntry
)
{
    try
    {
        return pfn(feToken, beToken, severity, entry, cchEntry);
    }
    catch(std::bad_alloc&)
    {
        return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
    }
    catch(std::exception&)
    {
        return PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
    }
    catch(...)
    {
        return PANTHEIOS_INIT_RC_UNKNOWN_FAILURE;
    }
}

#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

/* ///////////////////////////// end of file //////////////////////////// */
