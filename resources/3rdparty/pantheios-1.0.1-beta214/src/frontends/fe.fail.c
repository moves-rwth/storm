/* /////////////////////////////////////////////////////////////////////////
 * File:        src/frontends/fe.fail.c
 *
 * Purpose:     Implementation of the fe.fail front-end.
 *
 * Created:     8th March 2008
 * Updated:     22nd March 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2008-2010, Matthew Wilson and Synesis Software
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
#include <pantheios/frontend.h>
#include <pantheios/frontends/fe.fail.h>
#include <pantheios/quality/contract.h>
#include <pantheios/init_codes.h>

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

PANTHEIOS_CALL(int) pantheios_fe_init(
    void*   reserved
,   void**  ptoken
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != ptoken, "token pointer may not be null");
    /* PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(pantheios_isInitialising(), "This can only be called when Pantheios is initialising"); */

    STLSOFT_SUPPRESS_UNUSED(reserved);

    *ptoken = NULL;

    return PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE;
}

PANTHEIOS_CALL(void) pantheios_fe_uninit(
    void* token
)
{
    PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_API("This should never be called!");

    STLSOFT_SUPPRESS_UNUSED(token);
}

PANTHEIOS_CALL(PAN_CHAR_T const*) pantheios_fe_getProcessIdentity(
    void* token
)
{
    PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_API("This should never be called!");

    STLSOFT_SUPPRESS_UNUSED(token);

    return NULL;
}

PANTHEIOS_CALL(int) pantheios_fe_isSeverityLogged(
    void*   token
,   int     severity
,   int     backEndId
)
{
    PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_API("This should never be called!");

    STLSOFT_SUPPRESS_UNUSED(token);
    STLSOFT_SUPPRESS_UNUSED(severity);
    STLSOFT_SUPPRESS_UNUSED(backEndId);

    return 0;   /* Causes all diagnostic logging statements to be suppressed */
}

/* ///////////////////////////// end of file //////////////////////////// */
