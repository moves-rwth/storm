/* /////////////////////////////////////////////////////////////////////////
 * File:        src/backends/be.lrsplit.c
 *
 * Purpose:     Implementation
 *
 * Created:     26th June 2005
 * Updated:     22nd March 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2010, Matthew Wilson and Synesis Software
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
#include <pantheios/backend.h>
#include <pantheios/frontend.h>
#include <pantheios/backends/be.lrsplit.h>

#include <pantheios/init_codes.h>
#include <pantheios/quality/contract.h>

#include <stdio.h>
#include <string.h>

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

#define pan_lr_tokens_t         pantheios_src_util_pan_lr_tokens_t

struct pan_lr_tokens_t
{
    void*   localToken;
    void*   remoteToken;
};
typedef struct pan_lr_tokens_t  pan_lr_tokens_t;

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

PANTHEIOS_CALL(int) pantheios_be_init(
    PAN_CHAR_T const*   processIdentity
,   void*               reserved
,   void**              ptoken
)
{
    pan_lr_tokens_t tokens  =   { NULL, NULL };
    int             res;

    /* Initialise the local first */
    res = pantheios_be_local_init(processIdentity, reserved, &tokens.localToken);

    if(0 != res)
    {
        pantheios_onBailOut3(PANTHEIOS_SEV_ALERT, "local back-end did not initialise", NULL);

        return res;
    }
    else
    {
        res = pantheios_be_remote_init(processIdentity, reserved, &tokens.remoteToken);

        if(0 != res)
        {
            pantheios_onBailOut3(PANTHEIOS_SEV_ALERT, "remote back-end did not initialise", NULL);

            pantheios_be_local_uninit(tokens.localToken);

            return res;
        }
        else
        {
            pan_lr_tokens_t* ptokens = (pan_lr_tokens_t*)malloc(sizeof(pan_lr_tokens_t));

            if(NULL == ptokens)
            {
                pantheios_be_remote_uninit(tokens.remoteToken);
                pantheios_be_local_uninit(tokens.localToken);

                return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
            }
            else
            {
                *ptokens    =   tokens;
                *ptoken     =   ptokens;

                return 0;
            }
        }
    }
}

PANTHEIOS_CALL(void) pantheios_be_uninit(void* token)
{
    pan_lr_tokens_t* tokens = (pan_lr_tokens_t*)token;

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != token, "token may not be null");

    pantheios_be_local_uninit(tokens->localToken);
    pantheios_be_remote_uninit(tokens->remoteToken);
    free(tokens);
}

PANTHEIOS_CALL(int) pantheios_be_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
)
{
    pan_lr_tokens_t* tokens = (pan_lr_tokens_t*)beToken;
    int             r1;
    int             r2;

    /* Pre-condition testing. */
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != tokens, "back-end token pointer may not be null");

    r1 = pantheios_fe_isSeverityLogged(feToken, severity, PANTHEIOS_BEID_LOCAL)
            ? pantheios_be_local_logEntry(feToken, tokens->localToken, severity, entry, cchEntry)
            : 0;
    r2 = pantheios_fe_isSeverityLogged(feToken, severity, PANTHEIOS_BEID_REMOTE)
            ? pantheios_be_remote_logEntry(feToken, tokens->remoteToken, severity, entry, cchEntry)
            : 0;

    /* Remote is given priority in error stakes. It's probably of little significance,
     * but a choice was demanded.
     */
    if(r2 < 0)
    {
        return r2;
    }
    else if(r1 < 0)
    {
        return r1;
    }
    else
    {
        return r2;
    }
}

/* ///////////////////////////// end of file //////////////////////////// */
