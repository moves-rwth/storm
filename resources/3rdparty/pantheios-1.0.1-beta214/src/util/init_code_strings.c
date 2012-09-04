/* /////////////////////////////////////////////////////////////////////////
 * File:        src/util/init_code_strings.c
 *
 * Purpose:     Initialisation code strings for Pantheios API
 *
 * Created:     27th September 2007
 * Updated:     1st February 2011
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2007-2011, Matthew Wilson and Synesis Software
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
#include <pantheios/init_codes.h>
#include <pantheios/quality/contract.h>

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Macros
 */

#define pan_init_char_t_                    char
#define PANTHEIOS_INIT_CODE_STRING(x)       x

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

#ifdef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
struct InitCodeString
#else /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
# define InitCodeString        pantheios_src_util_InitCodeString
typedef struct InitCodeString  InitCodeString;
struct InitCodeString
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
{
    int                     code;   /*!< The status code.   */
    pan_init_char_t_ const* str;    /*!< The string.        */
    size_t                  len;    /*!< The string length. */
};



#define INIT_ERR_STR_DECL(rc, desc)                                                     \
                                                                                        \
    static const pan_init_char_t_   s_str##rc[] =   desc;                                   \
    static const InitCodeString     s_rct##rc = { rc, s_str##rc, STLSOFT_NUM_ELEMENTS(s_str##rc) - 1 }


#define INIT_ERR_STR_ENTRY(rc)                                                          \
                                                                                        \
    &s_rct##rc


static pan_init_char_t_ const* pantheios_LookupCodeA_(int code, InitCodeString const** mappings, size_t cMappings, size_t* len)
{
    /* Use Null Object (Variable) here for len, so do not need to check
     * elsewhere.
     */
    size_t  len_;

    if(NULL == len)
    {
        len = &len_;
    }

    /* Linear search. */
    { size_t i; for(i = 0; i < cMappings; ++i)
    {
        if(code == mappings[i]->code)
        {
            return (*len = mappings[i]->len, mappings[i]->str);
        }
    }}

    return (*len = 0, PANTHEIOS_INIT_CODE_STRING("unrecognised status code"));
}

static pan_init_char_t_ const* pantheios_LookupInitCodeStringA_(int code, size_t* len)
{
    INIT_ERR_STR_DECL(PANTHEIOS_INIT_RC_SUCCESS                             ,   PANTHEIOS_INIT_CODE_STRING("operation completed successfully")                              );
    INIT_ERR_STR_DECL(PANTHEIOS_INIT_RC_OUT_OF_MEMORY                       ,   PANTHEIOS_INIT_CODE_STRING("out of memory")                                                 );
    INIT_ERR_STR_DECL(PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION               ,   PANTHEIOS_INIT_CODE_STRING("general exception")                                             );
    INIT_ERR_STR_DECL(PANTHEIOS_INIT_RC_UNKNOWN_FAILURE                     ,   PANTHEIOS_INIT_CODE_STRING("unknown failure")                                               );
    INIT_ERR_STR_DECL(PANTHEIOS_INIT_RC_UNSPECIFIED_FAILURE                 ,   PANTHEIOS_INIT_CODE_STRING("unspecified failure")                                           );
    INIT_ERR_STR_DECL(PANTHEIOS_INIT_RC_NOT_IMPLEMENTED                     ,   PANTHEIOS_INIT_CODE_STRING("feature not implemented, or not supported by host environment") );
    INIT_ERR_STR_DECL(PANTHEIOS_INIT_RC_CANNOT_CREATE_TSS_INDEX             ,   PANTHEIOS_INIT_CODE_STRING("cannot create TSS index")                                       );
    INIT_ERR_STR_DECL(PANTHEIOS_INIT_RC_CANNOT_CREATE_THREAD                ,   PANTHEIOS_INIT_CODE_STRING("cannot create thread")                                          );

    INIT_ERR_STR_DECL(PANTHEIOS_BE_INIT_RC_NO_BACKENDS_SPECIFIED            ,   PANTHEIOS_INIT_CODE_STRING("no backends specified")                                         );
    INIT_ERR_STR_DECL(PANTHEIOS_BE_INIT_RC_ALL_BACKEND_INITS_FAILED         ,   PANTHEIOS_INIT_CODE_STRING("all backends failed")                                           );
    INIT_ERR_STR_DECL(PANTHEIOS_BE_INIT_RC_INVALID_PROCESSID                ,   PANTHEIOS_INIT_CODE_STRING("invalid process identifier")                                    );
    INIT_ERR_STR_DECL(PANTHEIOS_BE_INIT_RC_API_MUTEX_INIT_FAILED            ,   PANTHEIOS_INIT_CODE_STRING("API mutex initialisation failed")                               );
    INIT_ERR_STR_DECL(PANTHEIOS_BE_INIT_RC_INTENDED_FAILURE                 ,   PANTHEIOS_INIT_CODE_STRING("feature intentionally failed")                                  );
    INIT_ERR_STR_DECL(PANTHEIOS_BE_INIT_RC_INIT_PARAM_REQUIRED              ,   PANTHEIOS_INIT_CODE_STRING("backend requires initialisation parameter")                     );
    INIT_ERR_STR_DECL(PANTHEIOS_BE_INIT_RC_INIT_CONFIG_REQUIRED             ,   PANTHEIOS_INIT_CODE_STRING("backend requires initialisation configuration information")     );
    INIT_ERR_STR_DECL(PANTHEIOS_BE_INIT_RC_INVALID_ARGUMENT                 ,   PANTHEIOS_INIT_CODE_STRING("an invalid argument was passed to the backend")                 );
    INIT_ERR_STR_DECL(PANTHEIOS_BE_INIT_RC_ARGUMENT_TOO_LONG                ,   PANTHEIOS_INIT_CODE_STRING("an argument was too long")                                      );
    INIT_ERR_STR_DECL(PANTHEIOS_BE_INIT_RC_ARGUMENT_OUT_OF_RANGE            ,   PANTHEIOS_INIT_CODE_STRING("an argument was out of range")                                  );
    INIT_ERR_STR_DECL(PANTHEIOS_BE_INIT_RC_PERMISSION_DENIED                ,   PANTHEIOS_INIT_CODE_STRING("permission to access a required resource was denied")           );
    INIT_ERR_STR_DECL(PANTHEIOS_BE_INIT_RC_RESOURCE_BUSY                    ,   PANTHEIOS_INIT_CODE_STRING("required resource is already is use")                           );

    INIT_ERR_STR_DECL(PANTHEIOS_BE_INIT_RC_FUTURE_VERSION_REQUESTED         ,   PANTHEIOS_INIT_CODE_STRING("a version was requested of a later version of Pantheios than was used to create the back-end")    );
    INIT_ERR_STR_DECL(PANTHEIOS_BE_INIT_RC_OLD_VERSION_NOT_SUPPORTED        ,   PANTHEIOS_INIT_CODE_STRING("a version was requested of a previous version of Pantheios that is no longer supported by the back-end")  );

    INIT_ERR_STR_DECL(PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE                 ,   PANTHEIOS_INIT_CODE_STRING("feature intentionally failed")                                  );
    INIT_ERR_STR_DECL(PANTHEIOS_FE_INIT_RC_SYSTEM_NOT_CONFIGURED            ,   PANTHEIOS_INIT_CODE_STRING("system not configured for frontend")                            );
    INIT_ERR_STR_DECL(PANTHEIOS_FE_INIT_RC_INIT_CONFIG_REQUIRED             ,   PANTHEIOS_INIT_CODE_STRING("frontend requires initialisation configuration information")    );

    static const InitCodeString* s_strings[] =
    {
        INIT_ERR_STR_ENTRY(PANTHEIOS_INIT_RC_SUCCESS                    ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_INIT_RC_OUT_OF_MEMORY              ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION      ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_INIT_RC_UNKNOWN_FAILURE            ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_INIT_RC_UNSPECIFIED_FAILURE        ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_INIT_RC_NOT_IMPLEMENTED            ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_INIT_RC_CANNOT_CREATE_TSS_INDEX    ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_INIT_RC_CANNOT_CREATE_THREAD       ),

        INIT_ERR_STR_ENTRY(PANTHEIOS_BE_INIT_RC_NO_BACKENDS_SPECIFIED   ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_BE_INIT_RC_ALL_BACKEND_INITS_FAILED),
        INIT_ERR_STR_ENTRY(PANTHEIOS_BE_INIT_RC_INVALID_PROCESSID       ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_BE_INIT_RC_API_MUTEX_INIT_FAILED   ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_BE_INIT_RC_INTENDED_FAILURE        ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_BE_INIT_RC_INIT_PARAM_REQUIRED     ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_BE_INIT_RC_INIT_CONFIG_REQUIRED    ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_BE_INIT_RC_INVALID_ARGUMENT        ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_BE_INIT_RC_ARGUMENT_TOO_LONG       ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_BE_INIT_RC_ARGUMENT_OUT_OF_RANGE   ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_BE_INIT_RC_PERMISSION_DENIED       ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_BE_INIT_RC_RESOURCE_BUSY           ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_BE_INIT_RC_FUTURE_VERSION_REQUESTED),
        INIT_ERR_STR_ENTRY(PANTHEIOS_BE_INIT_RC_OLD_VERSION_NOT_SUPPORTED),


        INIT_ERR_STR_ENTRY(PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE        ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_FE_INIT_RC_SYSTEM_NOT_CONFIGURED   ),
        INIT_ERR_STR_ENTRY(PANTHEIOS_FE_INIT_RC_INIT_CONFIG_REQUIRED    ),
    };

    return pantheios_LookupCodeA_(code, s_strings, STLSOFT_NUM_ELEMENTS(s_strings), len);
}

PANTHEIOS_CALL(pan_init_char_t_ const*) pantheios_getInitCodeString(int code)
{
    return pantheios_LookupInitCodeStringA_((int)code, NULL);
}

PANTHEIOS_CALL(size_t) pantheios_getInitCodeStringLength(int code)
{
    size_t len;

    return (pantheios_LookupInitCodeStringA_((int)code, &len), len);
}

/* deprecated */
PANTHEIOS_CALL(pan_init_char_t_ const*) pantheios_getInitErrorString(int code)
{
    return pantheios_LookupInitCodeStringA_((int)code, NULL);
}

/* deprecated */
PANTHEIOS_CALL(size_t) pantheios_getInitErrorStringLength(int code)
{
    size_t  len;

    return (pantheios_LookupInitCodeStringA_((int)code, &len), len);
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
