/* /////////////////////////////////////////////////////////////////////////
 * File:        src/backends/bec.WindowsSyslog.cpp
 *
 * Purpose:     Implementation of the Pantheios Windows-SysLog Stock Back-end API.
 *
 * Created:     23rd September 2005
 * Updated:     5th August 2012
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2012, Matthew Wilson and Synesis Software
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
#include <pantheios/internal/winlean.h>
#define PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
#include <pantheios/backends/bec.WindowsSyslog.h>
#include <pantheios/internal/safestr.h>

#include <pantheios/init_codes.h>
#include <pantheios/quality/contract.h>
#include <pantheios/util/core/apidefs.hpp>
#include <pantheios/util/backends/arguments.h>
#include <pantheios/util/string/strdup.h>
#include <pantheios/util/system/hostname.h>

/* STLSoft Header files */
#include <stlsoft/stlsoft.h>
#include <pantheios/util/memory/auto_buffer_selector.hpp>
#include <stlsoft/conversion/char_conversions.hpp>
#include <stlsoft/conversion/integer_to_string.hpp>
#include <stlsoft/util/minmax.hpp>

/* Standard C Header files */
#include <stdio.h>
#include <string.h>
#include <time.h>

/* Windows Header Files */
#include <winsock2.h>

/* /////////////////////////////////////////////////////////////////////////
 * Compiler compatibility
 */

#if (   defined(STLSOFT_COMPILER_IS_MSVC) && \
        _MSC_VER < 1200)
# define _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

namespace
{

#if !defined(PANTHEIOS_NO_NAMESPACE)

    using ::pantheios::pan_char_t;
    using ::pantheios::pan_uint8_t;
    using ::pantheios::pan_uint16_t;
    using ::pantheios::pan_uint32_t;
    using ::pantheios::pan_slice_t;
    using ::pantheios::pan_sev_t;
    using ::pantheios::pantheios_getHostName;
    using ::pantheios::util::pantheios_onBailOut3;
    using ::pantheios::pantheios_util_strdup_nothrow_m;
    using ::pantheios::pantheios_util_strfree_m;

#endif /* !PANTHEIOS_NO_NAMESPACE */

    template <ss_typename_param_k T>
    struct buffer_selector_
    {
        typedef ss_typename_type_k
#if !defined(PANTHEIOS_NO_NAMESPACE)
            pantheios::util::auto_buffer_selector<
#else /* ? !PANTHEIOS_NO_NAMESPACE */
            auto_buffer_selector<
#endif /* !PANTHEIOS_NO_NAMESPACE */
            T
        ,   1024
        >::type                                 type;
    };

    typedef buffer_selector_<char>::type        buffer_a_t;
    typedef buffer_selector_<wchar_t>::type     buffer_w_t;
    typedef buffer_selector_<pan_char_t>::type  buffer_t;

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

namespace
{

    struct WindowsSysLog_Context
    {
        int             id;
        unsigned        flags;
        SOCKET          sk;
        char*           processIdentity;
        size_t          cchProcessIdentity;
        char*           hostIdentity;
        size_t          cchHostIdentity;
        unsigned char   facility;
    };
    typedef struct WindowsSysLog_Context   WindowsSysLog_Context;

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * Helper functions
 */

static char* pan_make_hostIdentity_(void);
static char* pri_print_(char* s, size_t cch, int i, size_t& cchWritten);
static int pantheios_be_WindowsSyslog_logEntry_a_(
    void*       feToken
,   void*       beToken
,   int         severity
,   char const* entry
,   size_t      cchEntry
);
static int pantheios_be_WindowsSyslog_init_a_(
    char const*                         processIdentity
,   int                                 id
,   pan_be_WindowsSyslog_init_t const*  init
,   void*                               reserved
,   void**                              ptoken
);

/* /////////////////////////////////////////////////////////////////////////
 * API
 *
 * Some man pages advise that:
 *
 * - the processIdentity is
 */


PANTHEIOS_CALL(void) pantheios_be_WindowsSyslog_getDefaultAppInit(pan_be_WindowsSyslog_init_t* init) /* throw() */
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != init, "initialisation structure pointer may not be null");

    init->version       =   PANTHEIOS_VER;
    init->flags         =   0;
    init->addrSize      =   4;
    init->bytes[0]      =   255;
    init->bytes[1]      =   255;
    init->bytes[2]      =   255;
    init->bytes[3]      =   255;
    init->hostName      =   NULL;
    init->port          =   514;
    init->facility      =   PANTHEIOS_SYSLOG_FAC_USER;
}

PANTHEIOS_CALL(int) pantheios_be_WindowsSyslog_init(
    pan_char_t const*                   processIdentity
,   int                                 id
,   pan_be_WindowsSyslog_init_t const*  init
,   void*                               reserved
,   void**                              ptoken
)
{
#ifdef PANTHEIOS_USE_WIDE_STRINGS
    return pantheios_be_WindowsSyslog_init_a_(stlsoft::w2m(processIdentity), id, init, reserved, ptoken);
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    return pantheios_be_WindowsSyslog_init_a_(processIdentity, id, init, reserved, ptoken);
#endif /* PANTHEIOS_USE_WIDE_STRINGS */
}

static int pantheios_be_WindowsSyslog_init_a_(
    char const*                         processIdentity
,   int                                 id
,   pan_be_WindowsSyslog_init_t const*  init
,   void*                               reserved
,   void**                              ptoken
)
{
    WindowsSysLog_Context* ctxt = static_cast<WindowsSysLog_Context*>(::calloc(1, sizeof(WindowsSysLog_Context)));

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != processIdentity, "process identity may not be the null string");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API('\0' != 0[processIdentity], "process identity may not be the empty string");
    STLSOFT_SUPPRESS_UNUSED(reserved);
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != ptoken, "token pointer may not be null");

    *ptoken = NULL;

    if(NULL == ctxt)
    {
        return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
    }
    else if(NULL != ::strpbrk(processIdentity, " \t\r\n\b\v"))
    {
        return PANTHEIOS_BE_INIT_RC_INVALID_PROCESSID;
    }
    else
    {
        WSADATA                     wsadata;
        pan_be_WindowsSyslog_init_t init_;
        struct sockaddr_in          addr_in;
        const unsigned long         ADDR_BROADCAST  =   INADDR_BROADCAST;
        BOOL                        bBroadcast      =   TRUE;

        if(0 != ::WSAStartup(0x0202, &wsadata))
        {
            goto error_startup;
        }

        /* (i) apply Null Object (Variable) pattern */

        if(NULL == init)
        {
            pantheios_be_WindowsSyslog_getDefaultAppInit(&init_);

#ifdef PANTHEIOS_BE_USE_CALLBACK
            pantheios_be_WindowsSyslog_getAppInit(id, &init_);
#endif /* PANTHEIOS_BE_USE_CALLBACK */

            init = &init_;
        }

        ctxt->id    =   id;
        ctxt->flags =   init->flags;

        memset(&addr_in, 0, sizeof(addr_in));
        if( 0 == init->addrSize &&
            NULL != init->hostName)
        {
            unsigned long   addr = ::inet_addr(init->hostName);
            struct hostent* he;

            if( INADDR_BROADCAST == addr &&
                0 != ::strcmp(init->hostName, "255.255.255.255") &&
                NULL != (he = ::gethostbyname(init->hostName)))
            {
                memcpy(&addr_in.sin_addr, he->h_addr, he->h_length);
            }
            else
            {
                memcpy(&addr_in.sin_addr, &addr, sizeof(addr));
            }
        }
        else
        {
            switch(init->addrSize)
            {
                case    16:
                default:
                    OutputDebugStringA("Invalid/unsupported address size: currently only 4 (IPv4) is supported\n");
                    memcpy(&addr_in.sin_addr, &ADDR_BROADCAST, sizeof(ADDR_BROADCAST));
                    break;
                case    4:
                    addr_in.sin_addr.S_un.S_un_b.s_b1   =   init->bytes[0];
                    addr_in.sin_addr.S_un.S_un_b.s_b2   =   init->bytes[1];
                    addr_in.sin_addr.S_un.S_un_b.s_b3   =   init->bytes[2];
                    addr_in.sin_addr.S_un.S_un_b.s_b4   =   init->bytes[3];

                    if(addr_in.sin_addr.s_addr != 0xffffffff)
                    {
                        bBroadcast = FALSE;
                    }
                    break;
            }
        }
        addr_in.sin_family  =   AF_INET;
        addr_in.sin_port    =   ::htons(init->port);

        ctxt->sk = ::socket(AF_INET, SOCK_DGRAM, 0);
        if(stlsoft_static_cast(SOCKET, SOCKET_ERROR) == ctxt->sk)
        {
            goto error_sock;
        }

        ctxt->processIdentity       =   pantheios_util_strdup_nothrow_m(processIdentity);
        if(NULL == ctxt->processIdentity)
        {
            goto error_procId;
        }
        ctxt->cchProcessIdentity    =   ::strlen(ctxt->processIdentity);

        ctxt->hostIdentity          =   pan_make_hostIdentity_();
        if(NULL == ctxt->hostIdentity)
        {
            goto error_hostId;
        }
        ctxt->cchHostIdentity       =   ::strlen(ctxt->hostIdentity);

        if(bBroadcast)
        {
            ::setsockopt(ctxt->sk, SOL_SOCKET, SO_BROADCAST, (char const*)&bBroadcast, sizeof(bBroadcast));
        }

        if(SOCKET_ERROR == ::connect(ctxt->sk, (struct sockaddr const*)&addr_in, sizeof(addr_in)))
        {
            goto error_connect;
        }

        ctxt->facility              =   static_cast<unsigned char>(init->facility % 124);   // Any more than 124 will overflow priority of <999>

        *ptoken = ctxt;

        return PANTHEIOS_INIT_RC_SUCCESS;

// NOTE: This goto lark is a vestige of the original C implementation.

error_connect:
        pantheios_util_strfree_m(ctxt->hostIdentity);
error_hostId:
        pantheios_util_strfree_m(ctxt->processIdentity);
error_procId:
        ::closesocket(ctxt->sk);
error_sock:
        ::WSACleanup();
error_startup:
        ::free(ctxt);
        return PANTHEIOS_INIT_RC_UNSPECIFIED_FAILURE;
    }
}

PANTHEIOS_CALL(void) pantheios_be_WindowsSyslog_uninit(void* token)
{
    WindowsSysLog_Context* ctxt = static_cast<WindowsSysLog_Context*>(token);

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != token, "token must be non-null");

    pantheios_util_strfree_m(ctxt->hostIdentity);
    pantheios_util_strfree_m(ctxt->processIdentity);
    ::closesocket(ctxt->sk);
    ::WSACleanup();
    ::free(ctxt);
}

static int pantheios_be_WindowsSyslog_logEntry_(
    void*               feToken
,   void*               beToken
,   int                 severity
,   pan_char_t const*   entry
,   size_t              cchEntry
)
{
#ifdef PANTHEIOS_USE_WIDE_STRINGS
    stlsoft::w2m e(entry, cchEntry);

    STLSOFT_ASSERT(e.size() <= cchEntry);

    return pantheios_be_WindowsSyslog_logEntry_a_(feToken, beToken, severity, e.data(), e.size());
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    return pantheios_be_WindowsSyslog_logEntry_a_(feToken, beToken, severity, entry, cchEntry);
#endif /* PANTHEIOS_USE_WIDE_STRINGS */
}

static int pantheios_be_WindowsSyslog_logEntry_a_(
    void*       feToken
,   void*       beToken
,   int         severity
,   char const* entry
,   size_t      cchEntry
)
{
    STLSOFT_SUPPRESS_UNUSED(feToken);

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != beToken, "back-end token must be non-null");

    static const char   tm_fmt[]                                =   "%b %d %I:%M:%S";
# ifdef PANTHEIOS_USING_SAFE_STR_FUNCTIONS
    static errno_t      (*fns[2])(struct tm*, const time_t*)    =   { ::localtime_s, ::gmtime_s };
# else /* ? PANTHEIOS_USING_SAFE_STR_FUNCTIONS */
    static struct tm*   (*fns[2])(const time_t*)                =   { ::localtime, ::gmtime };
# endif /* PANTHEIOS_USING_SAFE_STR_FUNCTIONS */

    WindowsSysLog_Context* ctxt       =   static_cast<WindowsSysLog_Context*>(beToken);
    char                szPri_[6];
    char*               szPri;
    char                szTime[31];
    const int           priority    =   (ctxt->facility * 8) + (severity % 8);  /* This stock back-end ignores any custom severity information. */
    time_t              t           =   time(NULL);
# ifdef PANTHEIOS_USING_SAFE_STR_FUNCTIONS
    struct tm           tm_;
    struct tm*          tm          =   &tm_;
    errno_t             err         =   fns[0 != (ctxt->flags & PANTHEIOS_BE_WINDOWSSYSLOG_F_USE_SYSTEM_TIME)](tm, &t);

    if(0 != err)
    {
        char    msg[1001];

        if(0 != ::strerror_s(&msg[0], STLSOFT_NUM_ELEMENTS(msg), err))
        {
            msg[0] = '\0';
        }

        pantheios_onBailOut3(pantheios::critical, "failed to elicit time", msg);

        return 0;
    }
# else /* ? PANTHEIOS_USING_SAFE_STR_FUNCTIONS */
    struct tm*          tm          =   fns[0 != (ctxt->flags & PANTHEIOS_BE_WINDOWSSYSLOG_F_USE_SYSTEM_TIME)](&t);
# endif /* PANTHEIOS_USING_SAFE_STR_FUNCTIONS */
    size_t              cchPriority;
    size_t              cchTime;
    size_t              cchTotal;

    PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(priority < 1000);

    szPri       =   pri_print_(&szPri_[0], STLSOFT_NUM_ELEMENTS(szPri_), priority, cchPriority);
    cchTime     =   ::strftime(&szTime[0], STLSOFT_NUM_ELEMENTS(szTime), tm_fmt, tm);

    cchTotal = cchPriority + cchTime + 1 + ctxt->cchHostIdentity + 1 + ctxt->cchProcessIdentity + 1 + cchEntry;

    buffer_a_t buffer(1 + cchTotal);

#ifndef STLSOFT_CF_THROW_BAD_ALLOC
    if(0 == buffer.size())
    {
        return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
    }
    else
#endif /* !STLSOFT_CF_THROW_BAD_ALLOC */
    {
        // === "%s%s %s %s %s", szPri, szTime, ctxt->hostIdentity, ctxt->processIdentity, entry

        size_t  cchWritten  =   0;
        char*   p           =   &buffer[0];

        ::memcpy(p, szPri, cchPriority * sizeof(char));
        cchWritten += cchPriority;
        p += cchPriority;

        ::memcpy(p, szTime, cchTime * sizeof(char));
        cchWritten += cchTime;
        p += cchTime;

        *p++ = ' ';
        ++cchWritten;

        ::memcpy(p, ctxt->hostIdentity, ctxt->cchHostIdentity * sizeof(char));
        cchWritten += ctxt->cchHostIdentity;
        p += ctxt->cchHostIdentity;

        *p++ = ' ';
        ++cchWritten;

        ::memcpy(p, ctxt->processIdentity, ctxt->cchProcessIdentity * sizeof(char));
        cchWritten += ctxt->cchProcessIdentity;
        p += ctxt->cchProcessIdentity;

        *p++ = ' ';
        ++cchWritten;

        ::memcpy(p, entry, cchEntry * sizeof(char));
        cchWritten += cchEntry;
        p += cchEntry;

        PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(cchWritten == size_t(p - buffer.data()));
        PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(cchWritten == cchTotal);

        STLSOFT_SUPPRESS_UNUSED(p);

        return ::send(ctxt->sk, buffer.data(), (int)cchWritten, 0);
    }
}

PANTHEIOS_CALL(int) pantheios_be_WindowsSyslog_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   pan_char_t const*   entry
,   size_t              cchEntry
)
{
    return pantheios_call_be_logEntry(pantheios_be_WindowsSyslog_logEntry_, feToken,beToken, severity, entry, cchEntry);
}

PANTHEIOS_CALL(int) pantheios_be_WindowsSyslog_parseArgs(
    size_t                          numArgs
,   pan_slice_t* const              args
,   pan_be_WindowsSyslog_init_t*    init
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API((NULL != args || 0 == numArgs), "argument pointer must be non-null, or number of arguments must be 0");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != init, "initialisation structure pointer may not be null");

    pantheios_be_WindowsSyslog_getDefaultAppInit(init);

    // 1. Parse the stock arguments
    int res = pantheios_be_parseStockArgs(numArgs, args, &init->flags);

    if(res >= 0)
    {
        pan_slice_t address;
        pan_slice_t port;
        pan_slice_t facility;

        // 2.a Parse the custom argument: "address"
        res = pantheios_be_parseStringArg(numArgs, args, PANTHEIOS_LITERAL_STRING("address"), &address);

        if(res > 0)
        {
            if(address.len > sizeof(init->hostNameBuff) - 1)
            {
                res = PANTHEIOS_BE_INIT_RC_ARGUMENT_TOO_LONG;
            }
            else
            {
                ::memcpy(&init->hostNameBuff[0], address.ptr, sizeof(pan_char_t) * STLSOFT_NUM_ELEMENTS(init->hostNameBuff));
                init->hostNameBuff[address.len] = '\0';
                init->hostName = &init->hostNameBuff[0];
                init->addrSize = 0;
            }
        }

        if(res >= 0)
        {
            // 2.b Parse the custom argument: "port"
            res = pantheios_be_parseStringArg(numArgs, args, PANTHEIOS_LITERAL_STRING("port"), &port);

            if(res > 0)
            {
                char    sz[21];
                int     portNum;

                ::memcpy(&sz[0], port.ptr, stlsoft::minimum(port.len, sizeof(pan_char_t) * STLSOFT_NUM_ELEMENTS(sz) - 1));
                sz[stlsoft::minimum(port.len, STLSOFT_NUM_ELEMENTS(sz) - 1)] = '\0';

                portNum = ::atoi(sz);

                if( portNum > 0 &&
                    portNum < 65536)
                {
                    init->port = static_cast<pan_uint16_t>(portNum);
                }
                else
                {
                    res = PANTHEIOS_BE_INIT_RC_ARGUMENT_OUT_OF_RANGE;
                }
            }
        }

        if(res >= 0)
        {
            // 2.b Parse the custom argument: "facility"
            res = pantheios_be_parseStringArg(numArgs, args, PANTHEIOS_LITERAL_STRING("facility"), &facility);

            if(res > 0)
            {
                char    sz[21];
                int     facilityNum;

                ::memcpy(&sz[0], facility.ptr, stlsoft::minimum(facility.len, sizeof(pan_char_t) * STLSOFT_NUM_ELEMENTS(sz) - 1));
                sz[stlsoft::minimum(facility.len, STLSOFT_NUM_ELEMENTS(sz) - 1)] = '\0';

                facilityNum = ::atoi(sz);

                if( facilityNum >= 0 &&
                    facilityNum < 24)
                {
                    init->facility = static_cast<pan_uint8_t>(facilityNum);
                }
                else
                {
                    res = PANTHEIOS_BE_INIT_RC_ARGUMENT_OUT_OF_RANGE;
                }
            }
        }
    }

    if(res >= 0)
    {
        // 2.d Parse the custom argument: "useStderr"
        res = pantheios_be_parseBooleanArg(numArgs, args, PANTHEIOS_LITERAL_STRING("useStderr"), false, PANTHEIOS_BE_WINDOWSSYSLOG_F_PERROR, &init->flags);
    }

    if(res >= 0)
    {
        // 2.e Parse the custom argument: "useConsole"
        res = pantheios_be_parseBooleanArg(numArgs, args, PANTHEIOS_LITERAL_STRING("useConsole"), false, PANTHEIOS_BE_WINDOWSSYSLOG_F_CONS, &init->flags);
    }

    if(res >= 0)
    {
        // 2.f Parse the custom argument: "showPid"
        res = pantheios_be_parseBooleanArg(numArgs, args, PANTHEIOS_LITERAL_STRING("showPid"), false, PANTHEIOS_BE_WINDOWSSYSLOG_F_PID, &init->flags);
    }

    if(res >= 0)
    {
        // 2.g Parse the custom argument: "connectImmediately"
        res = pantheios_be_parseBooleanArg(numArgs, args, PANTHEIOS_LITERAL_STRING("connectImmediately"), false, PANTHEIOS_BE_WINDOWSSYSLOG_F_NDELAY, &init->flags);
    }

    return res;
}

/* ////////////////////////////////////////////////////////////////////// */

// TODO: This should go in a src/util/x/y.cpp file
static char* pan_make_hostIdentity_(void)
{
    pan_char_t  szHostName[1 + MAX_COMPUTERNAME_LENGTH];
    size_t      cch = pantheios_getHostName(&szHostName[0], STLSOFT_NUM_ELEMENTS(szHostName));

    switch(cch)
    {
        case    0:
        case    STLSOFT_NUM_ELEMENTS(szHostName):
            return pantheios_util_strdup_nothrow_m("<localhost>");
        default:
#ifdef PANTHEIOS_USE_WIDE_STRINGS
            return pantheios_util_strdup_nothrow_m(stlsoft::w2m(szHostName));
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
            return pantheios_util_strdup_nothrow_m(szHostName);
#endif /* PANTHEIOS_USE_WIDE_STRINGS */
    }
}

char* pri_print_(char* s, size_t cch, int i, size_t& cchWritten)
{
    STLSOFT_ASSERT(NULL != s);
    STLSOFT_ASSERT(i >= 0);
    STLSOFT_ASSERT(i < 256);
    STLSOFT_ASSERT(cch >= 6);

    s[0]        = '\0';
    s[cch - 1]  = '\0';

    char* r = const_cast<char*>(stlsoft::integer_to_string(s, cch - 1, i, &cchWritten));

    s[cch - 2]  = '>';  ++cchWritten;
    *--r        = '<';  ++cchWritten;

    return r;
}

/* ///////////////////////////// end of file //////////////////////////// */
