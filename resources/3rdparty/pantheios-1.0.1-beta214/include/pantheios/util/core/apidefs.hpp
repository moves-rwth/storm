/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/util/core/apidefs.hpp
 *
 * Purpose:     API definition helpers for use in Pantheios back- and front-
 *              ends.
 *
 * Created:     3rd November 2007
 * Updated:     10th August 2009
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2007-2009, Matthew Wilson and Synesis Software
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


/** \file pantheios/util/core/apidefs.hpp
 *
 * [C++ only] API definition helpers for use in Pantheios back- and front-
 *   ends.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_UTIL_CORE_HPP_APIDEFS
#define PANTHEIOS_INCL_PANTHEIOS_UTIL_CORE_HPP_APIDEFS

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_UTIL_CORE_HPP_APIDEFS_MAJOR    2
# define PANTHEIOS_VER_PANTHEIOS_UTIL_CORE_HPP_APIDEFS_MINOR    1
# define PANTHEIOS_VER_PANTHEIOS_UTIL_CORE_HPP_APIDEFS_REVISION 1
# define PANTHEIOS_VER_PANTHEIOS_UTIL_CORE_HPP_APIDEFS_EDIT     15
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */
#ifndef PANTHEIOS_INCL_PANTHEIOS_H_BACKEND
# include <pantheios/backend.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_BACKEND */
#ifndef PANTHEIOS_INCL_PANTHEIOS_H_FRONTEND
# include <pantheios/frontend.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_FRONTEND */
#ifndef PANTHEIOS_INCL_PANTHEIOS_H_INIT_CODES
# include <pantheios/init_codes.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_INIT_CODES */

#ifndef STLSOFT_INCL_STLSOFT_H_STLSOFT
# include <stlsoft/stlsoft.h>
#endif /* !STLSOFT_INCL_STLSOFT_H_STLSOFT */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

/** Generic function pointer typedef for front-end initialisation */
typedef int (PANTHEIOS_CALLCONV* pantheios_fe_X_init_pfn_t)(
    void*   reserved
,   void**  ptoken
);

/** Function pointer typedef for front-end uninitialisation */
typedef int (PANTHEIOS_CALLCONV* pantheios_fe_X_uninit_pfn_t)(
    void*   token
);

/** Function pointer typedef for front-end process-identity */
typedef PAN_CHAR_T const* (PANTHEIOS_CALLCONV* pantheios_fe_X_getProcessIdentity_pfn_t)(void* token);

/** Function pointer typedef for front-end severity arbitration */
typedef int (PANTHEIOS_CALLCONV* pantheios_fe_X_isSeverityLogged_pfn_t)(
    void*   token
,   int     severity
,   int     backEndId
);


/** Generic function pointer typedef for back-end initialisation */
typedef int (PANTHEIOS_CALLCONV* pantheios_be_X_init_pfn_t)(
    PAN_CHAR_T const*   processIdentity
,   int                 backEndId
,   void const*         init
,   void*               reserved
,   void**              ptoken
);

/** Function pointer typedef for back-end uninitialisation */
typedef int (PANTHEIOS_CALLCONV* pantheios_be_X_uninit_pfn_t)(
    void*       token
);

/** Function pointer typedef for back-end entry logging */
typedef int (PANTHEIOS_CALLCONV* pantheios_be_X_logEntry_pfn_t)(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
);

/* /////////////////////////////////////////////////////////////////////////
 * Front-end Functions
 */

#if defined(STLSOFT_CF_EXCEPTION_SUPPORT) || \
    defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION)
/** Invokes a front-end initialisation function, implementating all the
 * exception-handling boilerplate for the function.
 *
 * \param pfn The front-end initialisation function
 * \param reserved The <code>reserved</code> parameter to be passed to <code>pfn</code>
 * \param ptoken The <code>ptoken</code> parameter to be passed to <code>pfn</code>
 */
PANTHEIOS_CALL(int) pantheios_call_fe_init(
    pantheios_fe_X_init_pfn_t   pfn
,   void*                       reserved
,   void**                      ptoken
);
#else /* ? STLSOFT_CF_EXCEPTION_SUPPORT */
inline int pantheios_call_fe_uninit(pantheios_fe_X_init_pfn_t pfn, void* reserved, void** ptoken)
{
    return pfn(reserved, ptoken);
}
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */


#if defined(STLSOFT_CF_EXCEPTION_SUPPORT) || \
    defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION)
/** Invokes a front-end uninitialisation function, implementating all the
 * exception-handling boilerplate for the function.
 *
 * \param pfn The front-end uninitialisation function
 * \param token The <code>token</code> parameter to be passed to <code>pfn</code>
 */
PANTHEIOS_CALL(int) pantheios_call_fe_uninit(
    pantheios_fe_X_uninit_pfn_t pfn
,   void*                       token
);
#else /* ? STLSOFT_CF_EXCEPTION_SUPPORT */
inline int pantheios_call_fe_uninit(pantheios_fe_X_uninit_pfn_t pfn, void* token)
{
    return pfn(token);
}
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */


#if defined(STLSOFT_CF_EXCEPTION_SUPPORT) || \
    defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION)
/** Invokes a front-end process-identity function, implementating all the
 * exception-handling boilerplate for the function.
 *
 * \param pfn The front-end process-identity function
 * \param token The <code>token</code> parameter to be passed to <code>pfn</code>
 */
PANTHEIOS_CALL(PAN_CHAR_T const*) pantheios_call_fe_getProcessIdentity(
    pantheios_fe_X_getProcessIdentity_pfn_t pfn
,   void*                                   token
);
#else /* ? STLSOFT_CF_EXCEPTION_SUPPORT */
inline PAN_CHAR_T const* pantheios_call_fe_getProcessIdentity(pantheios_fe_X_getProcessIdentity_pfn_t pfn, void* token)
{
    return pfn(token);
}
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */


#if defined(STLSOFT_CF_EXCEPTION_SUPPORT) || \
    defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION)
/** Invokes a front-end severity arbitration function, implementating all the
 * exception-handling boilerplate for the function.
 *
 * \param pfn The front-end severity arbitration function
 * \param token The <code>token</code> parameter to be passed to <code>pfn</code>
 * \param severity The severity level to test. Usually one of
 *   the \link pantheios::pan_severity_t PANTHEIOS_SEV_*\endlink enumerators.
 * \param backEndId A front-end+back-end specific parameter. This is used to
 *   enable a back-end, such as the be.lrsplit library, to simultaneously
 *   provide multiple actual back-ends and query the front-end for their
 *   respective. The value 0 is used within the Pantheios library and
 *   stands for every back-end. All other values indicate specific
 *   back-end splits, although by convention 1 indicates local logging
 *   and 2 indicates remote logging.
 */
PANTHEIOS_CALL(int) pantheios_call_fe_isSeverityLogged(
    pantheios_fe_X_isSeverityLogged_pfn_t   pfn
,   void*                                   token
,   int                                     severity
,   int                                     backEndId
);
#else /* ? STLSOFT_CF_EXCEPTION_SUPPORT */
inline int pantheios_call_fe_isSeverityLogged(pantheios_fe_X_isSeverityLogged_pfn_t pfn, void* token, int severity, int backEndId)
{
    return pfn(token, severity, backEndId);
}
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

/* /////////////////////////////////////////////////////////////////////////
 * Back-end Functions
 */

#if defined(STLSOFT_CF_EXCEPTION_SUPPORT) || \
    defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION)
/** Invokes a back-end initialisation function, implementating all the
 * exception-handling boilerplate for the function.
 *
 * \param pfn The back-end initialisation function
 * \param processIdentity The <code>processIdentity</code> parameter to be passed to <code>pfn</code>
 * \param backEndId The <code>backEndId</code> parameter to be passed to <code>pfn</code>
 * \param init The <code>init</code> parameter to be passed to <code>pfn</code>
 * \param reserved The <code>reserved</code> parameter to be passed to <code>pfn</code>
 * \param ptoken The <code>ptoken</code> parameter to be passed to <code>pfn</code>
 */
PANTHEIOS_CALL(int) pantheios_call_be_void_init(
    pantheios_be_X_init_pfn_t   pfn
,   PAN_CHAR_T const*           processIdentity
,   int                         backEndId
,   void const*                 init
,   void*                       reserved
,   void**                      ptoken
);
#else /* ? STLSOFT_CF_EXCEPTION_SUPPORT */
inline int pantheios_call_be_void_init(
    pantheios_be_X_init_pfn_t   pfn
,   PAN_CHAR_T const*           processIdentity
,   int                         backEndId
,   void const*                 init
,   void*                       reserved
,   void**                      ptoken
)
{
    return pfn(processIdentity, backEndId, init, reserved, ptoken);
}
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */


/** Invokes a back-end initialisation function, implementating all the
 * exception-handling boilerplate for the function.
 *
 * \param pfn The back-end initialisation function
 * \param processIdentity The <code>processIdentity</code> parameter to be passed to <code>pfn</code>
 * \param backEndId The <code>backEndId</code> parameter to be passed to <code>pfn</code>
 * \param init The <code>init</code> parameter to be passed to <code>pfn</code>
 * \param reserved The <code>reserved</code> parameter to be passed to <code>pfn</code>
 * \param ptoken The <code>ptoken</code> parameter to be passed to <code>pfn</code>
 */
template <typename T>
inline int pantheios_call_be_X_init(
#ifdef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
    int (*                      pfn)(PAN_CHAR_T const*, int, T const*, void*, void**)
#else /* ? PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
    int (PANTHEIOS_CALLCONV*    pfn)(PAN_CHAR_T const*, int, T const*, void*, void**)
#endif /* PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
,   PAN_CHAR_T const*           processIdentity
,   int                         backEndId
,   T const*                    init
,   void*                       reserved
,   void**                      ptoken
)
{
    return pantheios_call_be_void_init(reinterpret_cast<pantheios_be_X_init_pfn_t>(pfn), processIdentity, backEndId, init, reserved, ptoken);
}


#if defined(STLSOFT_CF_EXCEPTION_SUPPORT) || \
    defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION)
/** Invokes a back-end uninitialisation function, implementating all the
 * exception-handling boilerplate for the function.
 *
 * \param pfn The back-end uninitialisation function
 * \param token The <code>token</code> parameter to be passed to <code>pfn</code>
 */
PANTHEIOS_CALL(int) pantheios_call_be_uninit(
    pantheios_be_X_uninit_pfn_t pfn
,   void*                       token
);
#else /* ? STLSOFT_CF_EXCEPTION_SUPPORT */
inline int pantheios_call_be_uninit(pantheios_be_X_uninit_pfn_t pfn, void* token)
{
    return pfn(token);
}
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */


#if defined(STLSOFT_CF_EXCEPTION_SUPPORT) || \
    defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION)
/** Invokes a back-end log function, implementating all the
 * exception-handling boilerplate for the function.
 *
 * \param pfn The back-end log function
 * \param feToken The <code>feToken</code> parameter to be passed to <code>pfn</code>
 * \param beToken The <code>beToken</code> parameter to be passed to <code>pfn</code>
 * \param severity The <code>severity</code> parameter to be passed to <code>pfn</code>
 * \param entry The <code>entry</code> parameter to be passed to <code>pfn</code>
 * \param cchEntry The <code>cchEntry</code> parameter to be passed to <code>pfn</code>
 */
PANTHEIOS_CALL(int) pantheios_call_be_logEntry(
    pantheios_be_X_logEntry_pfn_t   pfn
,   void*                           feToken
,   void*                           beToken
,   int                             severity
,   PAN_CHAR_T const*               entry
,   size_t                          cchEntry
);
#else /* ? STLSOFT_CF_EXCEPTION_SUPPORT */
inline int pantheios_call_be_logEntry(
    pantheios_be_X_logEntry_pfn_t   pfn
,   void*                           feToken
,   void*                           beToken
,   int                             severity
,   PAN_CHAR_T const*               entry
,   size_t                          cchEntry
)
{
    return pfn(feToken, beToken, severity, entry, cchEntry);
}
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_UTIL_CORE_HPP_APIDEFS */

/* ///////////////////////////// end of file //////////////////////////// */
