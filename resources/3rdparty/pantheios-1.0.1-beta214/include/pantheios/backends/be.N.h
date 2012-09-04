/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/backends/be.N.h
 *
 * Purpose:     Declaration of the Pantheios be.N Stock Back-end API.
 *
 * Created:     18th October 2006
 * Updated:     26th November 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2006-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/backends/be.N.h
 *
 * [C,C++] Pantheios be.N Stock Back-end API
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_FRONTENDS_H_BE_N
#define PANTHEIOS_INCL_PANTHEIOS_FRONTENDS_H_BE_N

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BE_N_MAJOR      1
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BE_N_MINOR      6
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BE_N_REVISION   2
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BE_N_EDIT       21
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

#include <limits.h>

/* /////////////////////////////////////////////////////////////////////////
 * Documentation
 */

/** \defgroup group__backend__stock_backends__N Pantheios N Stock Back-end
 * \ingroup group__backend__stock_backends
 *  Back-end library that splits output to N back-ends.
 */

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/* /////////////////////////////////////////////////////////////////////////
 * Constants
 */

/** \defgroup group__backend__stock_backends__N__flags Pantheios be.N Stock Back-end Flags
 * \ingroup group__backend__stock_backends__N
 *  Flags for the \ref group__backend__stock_backends__N
 */

/** \def PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE
 *  Cause the \ref group__backend__stock_backends__N to
 *   ignore initialisation failure of the given back-end.
 * \ingroup group__backend__stock_backends__N__flags
 */

#define PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE            (0x00100000)

/** \def PANTHEIOS_BE_N_F_ID_MUST_MATCH_CUSTOM28
` *  Cause the \ref group__backend__stock_backends__N to
 *   only output to a given back-end if the custom-28 value
 *   matches the back-end's Id.
 * \ingroup group__backend__stock_backends__N__flags
 */

#define PANTHEIOS_BE_N_F_ID_MUST_MATCH_CUSTOM28         (0x00200000)

/** \def PANTHEIOS_BE_N_F_IGNORE_NONMATCHED_CUSTOM28_ID
 *  Cause the \ref group__backend__stock_backends__N to
 *   only output to a given back-end if the custom-28 value
 *   does not match the back-end's Id.
 * \ingroup group__backend__stock_backends__N__flags
 */

#define PANTHEIOS_BE_N_F_IGNORE_NONMATCHED_CUSTOM28_ID  (0x00400000)

/** \def PANTHEIOS_BE_N_F_INIT_ONLY_IF_PREVIOUS_FAILED
 *  Cause the \ref group__backend__stock_backends__N to
 *   initialise the given back-end only if all others have failed.
 * \ingroup group__backend__stock_backends__N__flags
 */

#define PANTHEIOS_BE_N_F_INIT_ONLY_IF_PREVIOUS_FAILED   (0x00800000)

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

/** Structure that describes a back-end to be used by the \ref group__backend__stock_backends__N.
 *
 * \ingroup group__backend__stock_backends__N
 *
 * \see PANTHEIOS_BE_N_STDFORM_ENTRY
 */
struct pan_be_N_t
{
    /** A combination of the
     *   \ref group__backend__stock_backends__N__flags "be.N flags" that control
     *   the behaviour of the given back-end
     */
    int     flags;
    /** The back-end Id
     *
     * Must be >0.
     */
    int     backEndId;
    /** Pointer to the back-end's initialisation function */
    int     (PANTHEIOS_CALLCONV *pfnInit)(
                PAN_CHAR_T const*
            ,   int
            ,   void const*
            ,   void*
            ,   void**
            );
    /** Pointer to the back-end's uninitialisation function */
    void    (PANTHEIOS_CALLCONV *pfnUninit)(    void*);
    /** Pointer to the back-end's log-entry function */
    int     (PANTHEIOS_CALLCONV *pfnLogEntry)(
                void*
            ,   void*
            ,   int
            ,   PAN_CHAR_T const*
            ,   size_t
            );
    /** A per-back-end static severity ceiling
     */
    int     severityCeiling;
#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
    /* INTERNAL USE ONLY */
    void*   token;
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
};
#ifndef __cplusplus
typedef struct pan_be_N_t   pan_be_N_t;
#endif /* __cplusplus */


/* PANTHEIOS_BE_N_ENTRY has been removed */


/** \def PANTHEIOS_BE_N_STDFORM_ENTRY
 *
 * Defines an entry in an array of pan_be_N_t.
 *
 * \param backEndId The back-end identifier. Must be >0
 * \param be_prefix The prefix of all the back-end functions, e.g. <b>pantheios_be_speech</b>
 * \param flags A combination of the
 *   \ref group__backend__stock_backends__N__flags "be.N flags" that control
 *   the behaviour of the given back-end
 *
 * This is used in combination with PANTHEIOS_BE_N_TERMINATOR_ENTRY to
 * define the set of concrete back-ends are to be attached to the program:
<pre>
pan_be_N_t  PAN_BE_N_BACKEND_LIST[] =
{
    PANTHEIOS_BE_N_STDFORM_ENTRY(1, pantheios_be_file, 0)
  , PANTHEIOS_BE_N_STDFORM_ENTRY(2, pantheios_be_fprintf, 0)
  , PANTHEIOS_BE_N_STDFORM_ENTRY(3, pantheios_be_null, 0)
\#if defined(PLATFORMSTL_OS_IS_UNIX)
  , PANTHEIOS_BE_N_STDFORM_ENTRY(4, pantheios_be_syslog, 0)
\#elif defined(PLATFORMSTL_OS_IS_WIN32) || \
      defined(PLATFORMSTL_OS_IS_WIN64)
  , PANTHEIOS_BE_N_STDFORM_ENTRY(4, pantheios_be_WindowsSyslog, 0)
\#endif
  , PANTHEIOS_BE_N_STDFORM_ENTRY(5, pantheios_be_file, 0)
  , PANTHEIOS_BE_N_TERMINATOR_ENTRY
};
</pre>
 *
 * \ingroup group__backend__stock_backends__N
 *
 * \note This is to be used insead of PANTHEIOS_BE_N_ENTRY
 */
#define PANTHEIOS_BE_N_STDFORM_ENTRY(backEndId, be_prefix, flags)   \
    {                                                               \
            flags                                                   \
        ,   backEndId                                               \
        ,   (int(PANTHEIOS_CALLCONV *)(PAN_CHAR_T const*,int,void const*,void*,void**))be_prefix ## _init \
        ,   be_prefix ## _uninit                                    \
        ,   be_prefix ## _logEntry                                  \
        ,   INT_MAX                                                 \
        ,   NULL                                                    \
    }

/** \def PANTHEIOS_BE_N_FILTERED_ENTRY
 *
 * Defines an entry in an array of pan_be_N_t.
 *
 * \param backEndId The back-end identifier. Must be >0.
 * \param be_prefix The prefix of all the back-end functions, e.g. <b>pantheios_be_speech</b>.
 * \param severityCeiling A static severity level, used to filter out
 *   messages of a given severity level prior to passing to the given
 *   specific back-end.
 * \param flags A combination of the
 *   \ref group__backend__stock_backends__N__flags "be.N flags" that control
 *   the behaviour of the given back-end.
 *
 * This is used in combination with PANTHEIOS_BE_N_TERMINATOR_ENTRY to
 * define the set of concrete back-ends are to be attached to the program:
<pre>
pan_be_N_t  PAN_BE_N_BACKEND_LIST[] =
{
    PANTHEIOS_BE_N_STDFORM_ENTRY(1, pantheios_be_file, 0)
  , PANTHEIOS_BE_N_STDFORM_ENTRY(2, pantheios_be_fprintf, 0)
  , PANTHEIOS_BE_N_STDFORM_ENTRY(3, pantheios_be_null, 0)
\#if defined(PLATFORMSTL_OS_IS_UNIX)
  , PANTHEIOS_BE_N_FILTERED_ENTRY(4, pantheios_be_syslog, PANTHEIOS_SEV_WARNING, 0)
\#elif defined(PLATFORMSTL_OS_IS_WIN32) || \
      defined(PLATFORMSTL_OS_IS_WIN64)
  , PANTHEIOS_BE_N_FILTERED_ENTRY(4, pantheios_be_WindowsSyslog, PANTHEIOS_SEV_WARNING, 0)
\#endif
  , PANTHEIOS_BE_N_STDFORM_ENTRY(5, pantheios_be_file, 0)
  , PANTHEIOS_BE_N_TERMINATOR_ENTRY
};
</pre>
 *
 * The output to back-end 4 is statically filtered, so that any statement
 * with severity less than or equal to the specified level will be output,
 * subject to dynamic filtering via pantheios_fe_isSeverityLogged().
 *
 * \ingroup group__backend__stock_backends__N
 */
#define PANTHEIOS_BE_N_FILTERED_ENTRY(backEndId, be_prefix, severityCeiling, flags)                         \
    {                                                                                                       \
            flags                                                                                           \
        ,   backEndId                                                                                       \
        ,   (int(PANTHEIOS_CALLCONV*)(PAN_CHAR_T const*,int,void const*,void*,void**))be_prefix ## _init    \
        ,   be_prefix ## _uninit                                                                            \
        ,   be_prefix ## _logEntry                                                                          \
        ,   severityCeiling                                                                                 \
        ,   NULL                                                                                            \
    }

/** \def PANTHEIOS_BE_N_TERMINATOR_ENTRY
 *
 * Defines a terminating entry in an array of pan_be_N_t.
 *
 * \ingroup group__backend__stock_backends__N
 *
 * \see PANTHEIOS_BE_N_STDFORM_ENTRY
 */
#define PANTHEIOS_BE_N_TERMINATOR_ENTRY                 { 0, 0, NULL, NULL, NULL, -1, NULL }

/* /////////////////////////////////////////////////////////////////////////
 * External Declarations
 */

/** The application-defined array of back-end descriptors that specify what
 *   concrete back-ends are to be attached to the program.
 *
 * \note The array must be defined with a last terminator element whose
 *   <code>pfnInit</code> member is <code>NULL</code>.
 *
 * \ingroup group__backend__stock_backends__N
 */
PANTHEIOS_EXTERN_C pan_be_N_t       PAN_BE_N_BACKEND_LIST[];

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_FRONTENDS_H_BE_N */

/* ///////////////////////////// end of file //////////////////////////// */
