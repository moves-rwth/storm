/* /////////////////////////////////////////////////////////////////////////
 * File:        src/util/severity_strings.c
 *
 * Purpose:     Severity strings for Pantheios API
 *
 * Created:     26th July 2005
 * Updated:     10th August 2009
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2009, Matthew Wilson and Synesis Software
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
#include <pantheios/quality/contract.h>

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

#ifdef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
struct SeverityString
#else /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
# define SeverityString         pantheios_src_util_SeverityString
typedef struct SeverityString  SeverityString;
struct SeverityString
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
{
    int                 severity;   /*!< The severity code. */
    pan_char_t const*   str;        /*!< The string.        */
    size_t              len;        /*!< The string length. */
};



#define SEVERITY_STR_DECL(rc, desc)                                                     \
                                                                                        \
    static const pan_char_t     s_str##rc[] =   desc;                                   \
    static const SeverityString s_rct##rc = { rc, s_str##rc, STLSOFT_NUM_ELEMENTS(s_str##rc) - 1 }


#define SEVERITY_STR_ENTRY(rc)                                                          \
                                                                                        \
    &s_rct##rc


static pan_char_t const* pantheios_LookupCodeA_(int severity, SeverityString const** mappings, size_t cMappings, size_t* len)
{
    /* Use Null Object (Variable) here for len, so do not need to check
     * elsewhere.
     */
    size_t  len_;

    if(NULL == len)
    {
        len = &len_;
    }

    /* Checked, indexed search. */
    if( severity >= 0 &&
        severity <= PANTHEIOS_SEV_DEBUG)
    {
        if(severity == mappings[severity]->severity)
        {
            return (*len = mappings[severity]->len, mappings[severity]->str);
        }
    }

    /* Linear search. Should only be needed if order in
     * pantheios_LookupSeverityStringA_() messed up.
     */
    { size_t i; for(i = 0; i < cMappings; ++i)
    {
        if(severity == mappings[i]->severity)
        {
            return (*len = mappings[i]->len, mappings[i]->str);
        }
    }}

    return (*len = 0, PANTHEIOS_LITERAL_STRING(""));
}

static pan_char_t const* pantheios_LookupSeverityStringA_(int error, size_t* len)
{
    SEVERITY_STR_DECL(PANTHEIOS_SEV_EMERGENCY        ,   PANTHEIOS_LITERAL_STRING("Emergency")      );
    SEVERITY_STR_DECL(PANTHEIOS_SEV_ALERT            ,   PANTHEIOS_LITERAL_STRING("Alert")          );
    SEVERITY_STR_DECL(PANTHEIOS_SEV_CRITICAL         ,   PANTHEIOS_LITERAL_STRING("Critical")       );
    SEVERITY_STR_DECL(PANTHEIOS_SEV_ERROR            ,   PANTHEIOS_LITERAL_STRING("Error")          );
    SEVERITY_STR_DECL(PANTHEIOS_SEV_WARNING          ,   PANTHEIOS_LITERAL_STRING("Warning")        );
    SEVERITY_STR_DECL(PANTHEIOS_SEV_NOTICE           ,   PANTHEIOS_LITERAL_STRING("Notice")         );
    SEVERITY_STR_DECL(PANTHEIOS_SEV_INFORMATIONAL    ,   PANTHEIOS_LITERAL_STRING("Informational")  );
    SEVERITY_STR_DECL(PANTHEIOS_SEV_DEBUG            ,   PANTHEIOS_LITERAL_STRING("Debug")          );

    static const SeverityString* s_strings[] =
    {
        SEVERITY_STR_ENTRY(PANTHEIOS_SEV_EMERGENCY),
        SEVERITY_STR_ENTRY(PANTHEIOS_SEV_ALERT),
        SEVERITY_STR_ENTRY(PANTHEIOS_SEV_CRITICAL),
        SEVERITY_STR_ENTRY(PANTHEIOS_SEV_ERROR),
        SEVERITY_STR_ENTRY(PANTHEIOS_SEV_WARNING),
        SEVERITY_STR_ENTRY(PANTHEIOS_SEV_NOTICE),
        SEVERITY_STR_ENTRY(PANTHEIOS_SEV_INFORMATIONAL),
        SEVERITY_STR_ENTRY(PANTHEIOS_SEV_DEBUG),
    };

    return pantheios_LookupCodeA_(error, s_strings, STLSOFT_NUM_ELEMENTS(s_strings), len);
}

/* deprecated */
PANTHEIOS_CALL(pan_char_t const*) pantheios_getSeverityString(pan_sev_t severity)
{
    return pantheios_getStockSeverityString(severity);
}

PANTHEIOS_CALL(pan_char_t const*) pantheios_getStockSeverityString(pan_sev_t severity)
{
    return pantheios_LookupSeverityStringA_((int)severity, NULL);
}

/* deprecated */
PANTHEIOS_CALL(size_t) pantheios_getSeverityStringLength(pan_sev_t severity)
{
    return pantheios_getStockSeverityStringLength(severity);
}

PANTHEIOS_CALL(size_t) pantheios_getStockSeverityStringLength(pan_sev_t severity)
{
    size_t  len;

    return (pantheios_LookupSeverityStringA_((int)severity, &len), len);
}

#if 0
PANTHEIOS_CALL(pan_char_t const*) pantheios_getInserterFormat(pan_char_t buff[200], int widthAndFormat, int bUnsigned, int intSize)
{
    /* Possible formats:
     *
     *
     */

    /* We take the width + format, the signedness, and the size of the integer
     *
     * e.g.:
     *
     *              0       -        unsigned             32                    =>      %u / %lu
     *              8       -        signed               32                    =>      %8d / %8ld
     *              -8      -        signed               32                    =>      %-8d / %-8ld
     *              4       hex      unsigned             64                    =>      %4x
     *
     */


    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API((intSize == 8 || intSize == 16 || intSize == 32 || intSize == 64), "size must be 8|16|32|64");

    widthAndFormat &= ~(0xff);  /* We don't care about the width here */

#if 0
    switch(
            zeroXPrefix =   0x0100  /*!< Applies a \c 0x prefix to the output. */
        ,   zeroPadded  =   0x0200  /*!< Zero-pads the output. */
        ,   hex         =   0x0400  /*!< Represents the output in hexadecimal. */
#endif /* 0 */



    return buff;
}
#endif /* 0 */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
