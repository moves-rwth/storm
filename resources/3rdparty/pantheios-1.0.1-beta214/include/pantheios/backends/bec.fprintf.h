/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/backends/bec.fprintf.h
 *
 * Purpose:     Declaration of the Pantheios fprintf Stock Back-end API.
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


/** \file pantheios/backends/bec.fprintf.h
 *
 * [C, C++] Pantheios fprintf() Back-end Common API
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_FPRINTF
#define PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_FPRINTF

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_FPRINTF_MAJOR       2
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_FPRINTF_MINOR       1
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_FPRINTF_REVISION    1
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_FPRINTF_EDIT        21
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
#ifndef PANTHEIOS_INCL_PANTHEIOS_UTIL_BACKENDS_H_ARGUMENTS
# include <pantheios/util/backends/arguments.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_UTIL_BACKENDS_H_ARGUMENTS */

/* /////////////////////////////////////////////////////////////////////////
 * Documentation
 */

/** \defgroup group__backend__stock_backends__fprintf Pantheios fprintf() Stock Back-end
 * \ingroup group__backend__stock_backends
 *  Back-end library that writes to the standard output or error stream
 *   via <code>fprintf()</code>.
 */

/* /////////////////////////////////////////////////////////////////////////
 * Constants
 */

/** \defgroup group__backend__stock_backends__fprintf__flags Pantheios fprintf() Stock Back-end Flags
 *
 * \ingroup group__backend__stock_backends__fprintf
 *  Flags for the \ref group__backend__stock_backends__fprintf
 */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

/** Structure used for specifying initialisation information to the
 *    be.fprintf library.
 * \ingroup group__backend__stock_backends__fprintf
 */
struct pan_be_fprintf_init_t
{
#if !defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION) && \
    !defined(PANTHEIOS_NO_NAMESPACE)
    typedef pantheios::pan_uint16_t pan_uint16_t;
    typedef pantheios::pan_uint32_t pan_uint32_t;
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION && !PANTHEIOS_NO_NAMESPACE */

    pan_uint32_t    version;    /*!< Must be initialised to the value of PANTHEIOS_VER */
    pan_uint32_t    flags;      /*!<  \ref group__backend__stock_backends__fprintf__flags "Flags" that control the information displayed. */

#ifdef __cplusplus
public: /* Construction */
# ifndef PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
    pan_be_fprintf_init_t();
# endif /* !PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT */
#endif /* __cplusplus */
};
#ifndef __cplusplus
typedef struct pan_be_fprintf_init_t    pan_be_fprintf_init_t;
#endif /* !__cplusplus */

/* /////////////////////////////////////////////////////////////////////////
 * Application-defined functions
 */

/** \ref page__backend__callbacks "Callback" function defined by
 *    the application, invoked when the
 *    API is initialised with a NULL <code>init</code> parameter.
 * \ingroup group__backend__stock_backends__fprintf
 *
 * \param backEndId The back-end identifier passed to the back-end
 *   during its initialisation.
 * \param init A pointer to an already-initialised instance of
 *   pan_be_fprintf_init_t.
 *
 * If any application-specific changes are required they can be made to
 * the structure to which <code>init</code> points, which will already
 * have been initialised. These changes will then be incorporated into
 * the back-end state, and reflected in its behaviour.
 *
 * If no changes are required, then the function can be a simple stub,
 * containing no instructions.
 *
 * \note This function is only required when the
 *   \ref page__backend__callbacks "callback" version of the library is
 *   used.
 *
 * \exception "throw()" This function must <b>not</b> throw any exceptions!
 *
 * \warning This function will be called during the initialisation of
 *   Pantheios, and so <b>must not</b> make any calls into Pantheios, either
 *   directly or indirectly!
 */
PANTHEIOS_CALL(void) pantheios_be_fprintf_getAppInit(
    int                     backEndId
,   pan_be_fprintf_init_t*  init
) /* throw() */;

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/** Fills out a copy of the initialisation structure with default
 *    values (representative of the default behaviour of the library),
 *    ready to be customised and passed to the API initialiser function
 *    pantheios_be_fprintf_init().
 * \ingroup group__backend__stock_backends__fprintf
 *
 * \note This function should <b>not</b> be called on an
 *   already-initialised instance, as is the case in the implementation
 *   of the pantheios_be_fprintf_getAppInit() function, as it will
 *   already have been called by pantheios_be_fprintf_init() prior
 *   to the callback.
 */
PANTHEIOS_CALL(void) pantheios_be_fprintf_getDefaultAppInit(pan_be_fprintf_init_t* init) /* throw() */;

/** Implements the functionality for pantheios_be_init() over the fprintf() function.
 * \ingroup group__backend__stock_backends__fprintf
 */
PANTHEIOS_CALL(int) pantheios_be_fprintf_init(
    PAN_CHAR_T const*               processIdentity
,   int                             id
,   pan_be_fprintf_init_t const*    unused
,   void*                           reserved
,   void**                          ptoken
);

/** Implements the functionality for pantheios_be_uninit() over the fprintf() function.
 * \ingroup group__backend__stock_backends__fprintf
 */
PANTHEIOS_CALL(void) pantheios_be_fprintf_uninit(void* token);

/** Implements the functionality for pantheios_be_logEntry() over the fprintf() function.
 * \ingroup group__backend__stock_backends__fprintf
 */
PANTHEIOS_CALL(int) pantheios_be_fprintf_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
);

/** \def pantheios_be_fprintf_parseArgs
 *
 * Parses the be.fprintf back-end flags
 *
 * \ingroup group__backend
 *
 * Processes an argument list in the same way as
 * pantheios_be_parseStockArgs(), filling out the
 * pan_be_fprintf_init_t in accordance with the arguments
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
 * There are currently no back-end specific arguments, hence
 * pantheios_be_fprintf_parseArgs is actually a \#define for
 * pantheios_be_parseStockArgs. At such time as back-end specific arguments
 * are required, it will become a first-class function.
 */
#define pantheios_be_fprintf_parseArgs(num, args, init)     pantheios_be_parseStockArgs((num), (args), (NULL == (init)) ? NULL : (&(init)->flags))


/* ////////////////////////////////////////////////////////////////////// */

#ifdef __cplusplus
# ifndef PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
inline pan_be_fprintf_init_t::pan_be_fprintf_init_t()
{
    pantheios_be_fprintf_getDefaultAppInit(this);
}
# endif /* !PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT */
#endif /* __cplusplus */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_FPRINTF */

/* ///////////////////////////// end of file //////////////////////////// */
