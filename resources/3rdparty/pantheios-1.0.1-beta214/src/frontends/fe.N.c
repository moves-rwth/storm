/* /////////////////////////////////////////////////////////////////////////
 * File:        src/frontends/fe.N.c
 *
 * Purpose:     Implementation of the Pantheios fe.N Stock Front-end API.
 *
 * Created:     18th October 2006
 * Updated:     23rd May 2011
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2006-2011, Matthew Wilson and Synesis Software
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
#include <pantheios/init_codes.h>
#include <pantheios/frontends/fe.N.h>
#include <pantheios/quality/contract.h>

#include <stdlib.h>

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

struct pantheios_fe_N_init_t
{
    size_t  numBackEnds;
    int     netLevel;
};
typedef struct pantheios_fe_N_init_t pantheios_fe_N_init_t;

/* /////////////////////////////////////////////////////////////////////////
 * Helper functions
 */

static size_t pantheios_fe_N_countBackEnds_(void)
{
    size_t              n = 0;
    pan_fe_N_t const*   frontEnd;

    for(frontEnd = &PAN_FE_N_SEVERITY_CEILINGS[0]; 0 != frontEnd->backEndId; ++n, ++frontEnd)
    {}

    return n;
}

static int pantheios_fe_N_calc0Level_(size_t numBackEnds)
{
    size_t  n;
    int     severityCeiling = PAN_FE_N_SEVERITY_CEILINGS[numBackEnds].severityCeiling;

    for(n = 0; n != numBackEnds; ++n)
    {
        pan_fe_N_t const* const frontEnd = &PAN_FE_N_SEVERITY_CEILINGS[n];

        if(frontEnd->severityCeiling > severityCeiling)
        {
            severityCeiling = frontEnd->severityCeiling;
        }
    }

    return severityCeiling;
}

/* /////////////////////////////////////////////////////////////////////////
 * API
 *
 * NOTE: We use *token for storing the (pre-calculated) 0 level
 */

PANTHEIOS_CALL(int) pantheios_fe_init(
    void*   reserved
,   void**  ptoken
)
{
    pantheios_fe_N_init_t* init;

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != ptoken, "token pointer may not be null");
    /* PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(pantheios_isInitialising(), "This can only be called when Pantheios is initialising"); */

    STLSOFT_SUPPRESS_UNUSED(reserved);

    init = (pantheios_fe_N_init_t*)malloc(sizeof(pantheios_fe_N_init_t));

    if(NULL == init)
    {
        return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
    }
    else
    {
        init->numBackEnds   =   pantheios_fe_N_countBackEnds_();
        init->netLevel      =   pantheios_fe_N_calc0Level_(init->numBackEnds);

        *ptoken = init;

        return PANTHEIOS_INIT_RC_SUCCESS;
    }
}

PANTHEIOS_CALL(void) pantheios_fe_uninit(
    void* token
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != token, "token must not be null");

    free(token);
}

PANTHEIOS_CALL(PAN_CHAR_T const*) pantheios_fe_getProcessIdentity(
    void* token
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != token, "token must not be null");
    /* PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(pantheios_isInitialising(), "This can only be called when Pantheios is initialising"); */

    STLSOFT_SUPPRESS_UNUSED(token);

#ifdef PANTHEIOS_BE_USE_CALLBACK
    return pantheios_fe_getAppProcessIdentity();
#else /* ? PANTHEIOS_BE_USE_CALLBACK */
    return PANTHEIOS_FE_PROCESS_IDENTITY;
#endif /* PANTHEIOS_BE_USE_CALLBACK */
}

PANTHEIOS_CALL(int) pantheios_fe_isSeverityLogged(
    void*   token
,   int     severity
,   int     backEndId
)
{
    pantheios_fe_N_init_t*  init;
    int                     severityCeiling;

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != token, "token must not be null");

    init = (pantheios_fe_N_init_t*)token;

    if(0 == backEndId)
    {
        severityCeiling = init->netLevel;
    }
    else
    {
        size_t const numBackEnds = init->numBackEnds;

        /* Optimise the search, if its index matches the id. */
        if( backEndId >= 1 &&
            (size_t)backEndId <= numBackEnds &&
            PAN_FE_N_SEVERITY_CEILINGS[backEndId - 1].backEndId == backEndId)
        {
            severityCeiling = PAN_FE_N_SEVERITY_CEILINGS[backEndId - 1].severityCeiling;
        }
        else
        {
            /* Search through the array. If we don't find the backEndId matching, then use
             * the one in .
             */
            size_t n;

            severityCeiling = PAN_FE_N_SEVERITY_CEILINGS[numBackEnds].severityCeiling;

            for(n = 0; n != numBackEnds; ++n)
            {
                pan_fe_N_t const* const frontEnd = &PAN_FE_N_SEVERITY_CEILINGS[n];

                if(frontEnd->backEndId == backEndId)
                {
                    severityCeiling = frontEnd->severityCeiling;
                    break;
                }
            }
        }
    }

    return (severity & 0x0f) <= severityCeiling;
}

/* ///////////////////////////// end of file //////////////////////////// */
