/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/backends/bec.COMErrorObject.h
 *
 * Purpose:     Declaration of the Pantheios COMErrorObject Stock Back-end API.
 *
 * Created:     21st June 2005
 * Updated:     30th April 2010
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


/** \file pantheios/backends/bec.COMErrorObject.h
 *
 * [C, C++] Pantheios COM Error Object Back-end Common API
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_COMERROROBJECT
#define PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_COMERROROBJECT

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_COMERROROBJECT_MAJOR    3
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_COMERROROBJECT_MINOR    1
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_COMERROROBJECT_REVISION 1
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_COMERROROBJECT_EDIT     22
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

/* /////////////////////////////////////////////////////////////////////////
 * Documentation
 */

/** \defgroup group__backend__stock_backends__COMErrorObject Pantheios COM Error Object Stock Back-end
 * \ingroup group__backend__stock_backends
 *  Back-end library that sets the calling threads's COM error object.
 */

/* /////////////////////////////////////////////////////////////////////////
 * Constants
 */

/** \defgroup group__backend__stock_backends__COMErrorObject__flags Pantheios COM Error Object Stock Back-end Flags
 * \ingroup group__backend__stock_backends__COMErrorObject
 *  Flags for the \ref group__backend__stock_backends__COMErrorObject
 */

/** \def PANTHEIOS_BE_COMERROROBJECT_F_DONT_OVERWRITE_EXISTING
 *  Prevents the \ref group__backend__stock_backends__COMErrorObject
 *   from writing to the COM Error Object if there is an existing error
 *   object set (for the calling thread). This is helpful when several
 *   levels of code are handling and reporting errors, so as not to lose.
 *   the innermost (and most exact) error report.
 * \ingroup group__backend__stock_backends__COMErrorObject__flags
 */

#define PANTHEIOS_BE_COMERROROBJECT_F_DONT_OVERWRITE_EXISTING       (0x00100000)

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

/** Structure used for specifying initialisation information to the
 *    be.COMErrorObject library.
 * \ingroup group__backend__stock_backends__COMErrorObject
 */
struct pan_be_COMErrorObject_init_t
{
#if !defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION) && \
    !defined(PANTHEIOS_NO_NAMESPACE)
    typedef pantheios::pan_uint16_t pan_uint16_t;
    typedef pantheios::pan_uint32_t pan_uint32_t;
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION && !PANTHEIOS_NO_NAMESPACE */

    pan_uint32_t    version;    /*!< Must be initialised to the value of PANTHEIOS_VER */
    pan_uint32_t    flags;      /*!< \ref group__backend__stock_backends__COMErrorObject__flags "Flags" that control the information displayed. */

#ifdef __cplusplus
public: /* Construction */
# ifndef PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
    pan_be_COMErrorObject_init_t();
# endif /* !PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT */
#endif /* __cplusplus */
};
#ifndef __cplusplus
typedef struct pan_be_COMErrorObject_init_t pan_be_COMErrorObject_init_t;
#endif /* !__cplusplus */

/* /////////////////////////////////////////////////////////////////////////
 * Application-defined functions
 */

/** Callback function defined by the application, invoked when the
 *    API is initialised with a NULL <code>init</code> parameter.
 *
 * \note When using explicit initialisation, this function must be defined,
 *   but will not be invoked.
 */
PANTHEIOS_CALL(void) pantheios_be_COMErrorObject_getAppInit(
    int                             backEndId
,   pan_be_COMErrorObject_init_t*   init
) /* throw() */;

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/** Fills out a copy of the initialisation structure with default
 *    values (representative of the default behaviour of the library),
 *    ready to be customised and passed to the API initialiser function
 *    pantheios_be_COMErrorObject_init().
 */
PANTHEIOS_CALL(void) pantheios_be_COMErrorObject_getDefaultAppInit(
    pan_be_COMErrorObject_init_t* init
) /* throw() */;

/** Implements the functionality for pantheios_be_init() over the COM Error Object API.
 * \ingroup group__backend__stock_backends__COMErrorObject
 */
PANTHEIOS_CALL(int) pantheios_be_COMErrorObject_init(
    PAN_CHAR_T const*                   processIdentity
,   int                                 id
,   pan_be_COMErrorObject_init_t const* init
,   void*                               reserved
,   void**                              ptoken
);

/** Implements the functionality for pantheios_be_uninit() over the COM Error Object API.
 * \ingroup group__backend__stock_backends__COMErrorObject
 */
PANTHEIOS_CALL(void) pantheios_be_COMErrorObject_uninit(
    void* token
);

/** Implements the functionality for pantheios_be_logEntry() over the COM Error Object API.
 * \ingroup group__backend__stock_backends__COMErrorObject
 */
PANTHEIOS_CALL(int) pantheios_be_COMErrorObject_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
);

/** Parses the be.COMErrorObject back-end flags
 *
 * \ingroup group__backend
 *
 * Processes an argument list in the same way as
 * pantheios_be_parseStockArgs(), filling out the
 * pan_be_COMErrorObject_init_t in accordance with the arguments
 * found.
 *
 * Recognises the following standard argument names:
 * - "showProcessId"            (Boolean)
 * - "showTime"                 (Boolean)
 * - "showSeverity"             (Boolean)
 * - "useSystemTime"            (Boolean)
 * - "showDetailsAtStart"       (Boolean)
 * - "useUnixFormat"            (Boolean)
 * - "showDate"                 (Boolean)
 * - "showTime"                 (Boolean)
 * - "highResolution"           (Boolean)
 * - "lowResolution"            (Boolean)
 *
 * Recognises the following back-end specific argument names:
 * - "overwriteExisting"        (Boolean)
 */

PANTHEIOS_CALL(int) pantheios_be_COMErrorObject_parseArgs(
    size_t                          numArgs
#ifdef PANTHEIOS_NO_NAMESPACE
,   struct pan_slice_t* const       args
#else /* ? PANTHEIOS_NO_NAMESPACE */
,   pantheios::pan_slice_t* const   args
#endif /* PANTHEIOS_NO_NAMESPACE */
,   pan_be_COMErrorObject_init_t*   init
);


/* ////////////////////////////////////////////////////////////////////// */

#ifdef __cplusplus
# ifndef PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
inline pan_be_COMErrorObject_init_t::pan_be_COMErrorObject_init_t()
{
    pantheios_be_COMErrorObject_getDefaultAppInit(this);
}
# endif /* !PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT */
#endif /* __cplusplus */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_COMERROROBJECT */

/* ///////////////////////////// end of file //////////////////////////// */
