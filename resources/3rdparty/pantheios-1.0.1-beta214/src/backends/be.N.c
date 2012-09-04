/* /////////////////////////////////////////////////////////////////////////
 * File:        src/backends/be.N.c
 *
 * Purpose:     Implementation of the Pantheios be.N Stock Back-end API.
 *
 * Created:     18th October 2006
 * Updated:     27th December 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2006-2010, Matthew Wilson and Synesis Software
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
#include <pantheios/backends/be.N.h>

#include <pantheios/init_codes.h>
#include <pantheios/quality/contract.h>

/* /////////////////////////////////////////////////////////////////////////
 * Helper functions
 */

static size_t pantheios_be_N_countBackEnds_everytime_(void);
static size_t pantheios_be_N_countBackEnds_onetime_(void);


static size_t pantheios_be_N_countBackEnds_(void)
{
    return pantheios_be_N_countBackEnds_onetime_();
}

static size_t pantheios_be_N_countBackEnds_onetime_(void)
{
    size_t const numBackEnds = pantheios_be_N_countBackEnds_everytime_();

    return numBackEnds;
}

static size_t pantheios_be_N_countBackEnds_everytime_(void)
{
    size_t      n   =   0;
    pan_be_N_t* backEnd;

    for(backEnd = &PAN_BE_N_BACKEND_LIST[0]; NULL != backEnd->pfnInit; ++n, ++backEnd)
    {
        PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_APPL_DEF(((NULL == backEnd->pfnInit) == (NULL == backEnd->pfnUninit)), "back-end descriptor must specify all functions or none");
        PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_APPL_DEF(((NULL == backEnd->pfnInit) == (NULL == backEnd->pfnLogEntry)), "back-end descriptor must specify all functions or none");
    }

    return n;
}

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

PANTHEIOS_CALL(int) pantheios_be_init(
    PAN_CHAR_T const*   processIdentity
,   void*               reserved
,   void**              ptoken
)
{
    size_t const    numBackEnds     =   pantheios_be_N_countBackEnds_();
    size_t          n;
    size_t          numSucceeded    =   0;
    int             res;
    pan_be_N_t*     terminalBackEnd =   &PAN_BE_N_BACKEND_LIST[numBackEnds];

    STLSOFT_SUPPRESS_UNUSED(reserved);

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != ptoken, "token pointer may not be null");

    *ptoken = NULL;

    if(0 == numBackEnds)
    {
        res = PANTHEIOS_BE_INIT_RC_NO_BACKENDS_SPECIFIED;
    }
    else
    {
        res = 0;

        for(n = 0; n < numBackEnds; ++n)
        {
            pan_be_N_t* backEnd = &PAN_BE_N_BACKEND_LIST[n];

            PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_APPL_DEF(0 != backEnd->backEndId, "be.N requires non-0 backEndId for all back-ends");

            if(-1 == backEnd->backEndId)
            {
                backEnd->backEndId = pantheios_getNextBackEndId();
            }

            if( PANTHEIOS_BE_N_F_INIT_ONLY_IF_PREVIOUS_FAILED == (PANTHEIOS_BE_N_F_INIT_ONLY_IF_PREVIOUS_FAILED & backEnd->flags) &&
                0 != numSucceeded)
            {
                backEnd->flags |= PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE;

                continue;
            }
            else
            {
                res = (*backEnd->pfnInit)(processIdentity, backEnd->backEndId, NULL, NULL, &backEnd->token);
            }

            if(0 == res)
            {
                /* Initialisation of the given back-end has succeeded, so
                 * remove PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE, if present.
                 */
                backEnd->flags &= ~(PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE);

                ++numSucceeded;

                if(terminalBackEnd->severityCeiling < backEnd->severityCeiling)
                {
                    terminalBackEnd->severityCeiling = backEnd->severityCeiling;
                }
            }
            else
            {
                if(PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE & backEnd->flags)
                {
                    /* Ignore failure. */
                    res = 0;
                }
                else
                {
                    break;
                }
            }
        }

        if(0 != res)
        {
            for(; 0 != n; --n)
            {
                pan_be_N_t* backEnd = &PAN_BE_N_BACKEND_LIST[n - 1];

                if(PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE & backEnd->flags)
                {
                    /* Was not initialised. */
                }
                else
                {
                    (*backEnd->pfnUninit)(backEnd->token);
                }
            }
        }
        else if(0 == numSucceeded)
        {
            res = PANTHEIOS_BE_INIT_RC_ALL_BACKEND_INITS_FAILED;
        }
    }

    return res;
}

PANTHEIOS_CALL(void) pantheios_be_uninit(void* token)
{
    size_t const    numBackEnds =   pantheios_be_N_countBackEnds_();
    size_t          n;

    STLSOFT_SUPPRESS_UNUSED(token);

    for(n = 0; n < numBackEnds; ++n)
    {
        pan_be_N_t* backEnd = &PAN_BE_N_BACKEND_LIST[n];

        if(PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE & backEnd->flags)
        {
            /* Was not initialised. */
        }
        else
        {
            (*backEnd->pfnUninit)(backEnd->token);
        }
    }
}

PANTHEIOS_CALL(int) pantheios_be_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
)
{
    int         res         =   0;
    pan_be_N_t* backEnd;
    int const   severity4   =   severity & 0x0f;
    int const   custom28    =   (severity >> 4) & 0x0fffffff;

    STLSOFT_SUPPRESS_UNUSED(beToken);

    for(backEnd = &PAN_BE_N_BACKEND_LIST[0]; NULL != backEnd->pfnInit; ++backEnd)
    {
        if(PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE & backEnd->flags)
        {
            /* Was not initialised. */
        }
        else if(severity4 > backEnd->severityCeiling)
        {
            /* Statically filtered out */
        }
        else
        {
/*
            custom28        must-match      ignore non-match

              0               skip              ok
              == id           ok                ok
              != id           skip              skip
*/

            if(custom28 != backEnd->backEndId)
            {
                /* If the custom28 does not match the back-end's id, we need to
                 * see if the back-end has special instructions.
                 */

                /* Test for PANTHEIOS_BE_N_F_ID_MUST_MATCH_CUSTOM28: skip mismatch'd output if specified. */
                if(PANTHEIOS_BE_N_F_ID_MUST_MATCH_CUSTOM28 & backEnd->flags)
                {
                    continue;
                }

                /* Test for PANTHEIOS_BE_N_F_IGNORE_NONMATCHED_CUSTOM28_ID: skip if mismatched, and custom != 0 */
                if( PANTHEIOS_BE_N_F_IGNORE_NONMATCHED_CUSTOM28_ID == (PANTHEIOS_BE_N_F_IGNORE_NONMATCHED_CUSTOM28_ID & backEnd->flags) &&
                    0 != custom28)
                {
                    continue;
                }
            }

            if(pantheios_fe_isSeverityLogged(feToken, severity, backEnd->backEndId))
            {
                int r2 = (*backEnd->pfnLogEntry)(feToken, backEnd->token, severity, entry, cchEntry);

                if(0 != r2)
                {
                    res = r2;
                }
            }
        }
    }

    return res;
}

/* ///////////////////////////// end of file //////////////////////////// */
