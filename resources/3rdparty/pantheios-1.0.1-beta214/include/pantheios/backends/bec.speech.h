/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/backends/bec.speech.h
 *
 * Purpose:     Declaration of the Pantheios speech Stock Back-end API.
 *
 * Created:     10th July 2006
 * Updated:     30th April 2010
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


/** \file pantheios/backends/bec.speech.h
 *
 * [C, C++] Pantheios speech Back-end Common API
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BEC_SPEECH
#define PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BEC_SPEECH

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_SPEECH_MAJOR    3
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_SPEECH_MINOR    1
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_SPEECH_REVISION 1
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_SPEECH_EDIT     19
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

/** \defgroup group__backend__stock_backends__speech Pantheios speech Stock Back-end
 * \ingroup group__backend__stock_backends
 *  Back-end library that outputs as speech.
 */

/* /////////////////////////////////////////////////////////////////////////
 * Constants
 */

/** \defgroup group__backend__stock_backends__speech__flags Pantheios speech Stock Back-end Flags
 * \ingroup group__backend__stock_backends__speech
 *  Flags for the \ref group__backend__stock_backends__speech
 */

/** \def PANTHEIOS_BE_SPEECH_F_SYNCHRONOUS
 *  Causes the \ref group__backend__stock_backends__speech to
 *   log speech synchronously. By default, speech output is
 *   asynchronous.
 * \ingroup group__backend__stock_backends__speech__flags
 */

/** \def PANTHEIOS_BE_SPEECH_F_PURGE_BEFORE_SPEAK
 *  Causes any log statements cached in the
 *   \ref group__backend__stock_backends__speech prior to purge any
 *   outstanding speach requests before attempting to emit the current
 *   statement.
 * \ingroup group__backend__stock_backends__speech__flags
 */

/** \def PANTHEIOS_BE_SPEECH_F_SPEAK_PUNCTUATION
 *  Causes the \ref group__backend__stock_backends__speech to
 *   speak punctuation.
 * \ingroup group__backend__stock_backends__speech__flags
 */

/** \def PANTHEIOS_BE_SPEECH_F_SYNCHRONOUS_ON_CRITICAL
 *  Causes the \ref group__backend__stock_backends__speech to
 *   output speech synchronously and to purge any previous outstanding
 *   speach requests if the severity level is pantheios::SEV_CRITICAL,
 *   pantheios::SEV_ALERT, or pantheios::SEV_EMERGENCY.
 * \ingroup group__backend__stock_backends__speech__flags
 */

/** \def PANTHEIOS_BE_SPEECH_F_UNINIT_DISCARD_WORKAROUND
 *  Causes the \ref group__backend__stock_backends__speech to
 *   discard the speech object reference, without waiting until done,
 *   at uninitialisation time. This is to workaround what appears to
 *   be a defect with SAPI, whereby the SAPI DLL is unloaded during
 *   application shutdown before all speech object references are
 *   released.
 * \ingroup group__backend__stock_backends__speech__flags
 */

#define PANTHEIOS_BE_SPEECH_F_SYNCHRONOUS               (0x00100000)
#define PANTHEIOS_BE_SPEECH_F_PURGE_BEFORE_SPEAK        (0x00200000)
#define PANTHEIOS_BE_SPEECH_F_SPEAK_PUNCTUATION         (0x00400000)
#define PANTHEIOS_BE_SPEECH_F_SYNCHRONOUS_ON_CRITICAL   (0x00800000)
#define PANTHEIOS_BE_SPEECH_F_UNINIT_DISCARD_WORKAROUND (0x01000000)

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

/** Structure used for specifying initialisation information to the
 *    be.speech library.
 * \ingroup group__backend__stock_backends__speech
 */
struct pan_be_speech_init_t
{
#if !defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION) && \
    !defined(PANTHEIOS_NO_NAMESPACE)
    typedef pantheios::pan_uint16_t pan_uint16_t;
    typedef pantheios::pan_uint32_t pan_uint32_t;
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION && !PANTHEIOS_NO_NAMESPACE */

    pan_uint32_t    version;    /*!< Must be initialised to the value of PANTHEIOS_VER */
    pan_uint32_t    flags;      /*!<  \ref group__backend__stock_backends__speech__flags "Flags" that control the information displayed. */

#ifdef __cplusplus
public: /* Construction */
# ifndef PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
    pan_be_speech_init_t();
# endif /* !PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT */
#endif /* __cplusplus */
};
#ifndef __cplusplus
typedef struct pan_be_speech_init_t   pan_be_speech_init_t;
#endif /* !__cplusplus */

/* /////////////////////////////////////////////////////////////////////////
 * Application-defined functions
 */

/** Fills out a copy of the initialisation structure with default
 *    values (representative of the default behaviour of the library),
 *    ready to be customised and passed to the API initialiser function
 *    pantheios_be_speech_init().
 */
PANTHEIOS_CALL(void) pantheios_be_speech_getDefaultAppInit(
    pan_be_speech_init_t* init
) /* throw() */;

/** \ref page__backend__callbacks "Callback" function defined by
 *    the application, invoked when the
 *    API is initialised with a NULL <code>init</code> parameter.
 * \ingroup group__backend__stock_backends__speech
 *
 * \param backEndId The back-end identifier passed to the back-end
 *   during its initialisation.
 * \param init A pointer to an already-initialised instance of
 *   pan_be_speech_init_t.
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
PANTHEIOS_CALL(void) pantheios_be_speech_getAppInit(
    int                     backEndId
,   pan_be_speech_init_t*   init
) /* throw() */;

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/** Implements the functionality for pantheios_be_init() over the speech API.
 * \ingroup group__backend__stock_backends__speech
 */
PANTHEIOS_CALL(int) pantheios_be_speech_init(
    PAN_CHAR_T const*           processIdentity
,   int                         id
,   pan_be_speech_init_t const* init
,   void*                       reserved
,   void**                      ptoken
);

/** Implements the functionality for pantheios_be_uninit() over the speech API.
 * \ingroup group__backend__stock_backends__speech
 */
PANTHEIOS_CALL(void) pantheios_be_speech_uninit(
    void* token
);

/** Implements the functionality for pantheios_be_logEntry() over the speech API.
 * \ingroup group__backend__stock_backends__speech
 */
PANTHEIOS_CALL(int) pantheios_be_speech_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
);

/** Parses the be.speech back-end flags
 *
 * \ingroup group__backend
 *
 *
 */
PANTHEIOS_CALL(int) pantheios_be_speech_parseArgs(
    size_t                          numArgs
#ifdef PANTHEIOS_NO_NAMESPACE
,   struct pan_slice_t* const       args
#else /* ? PANTHEIOS_NO_NAMESPACE */
,   pantheios::pan_slice_t* const   args
#endif /* PANTHEIOS_NO_NAMESPACE */
,   pan_be_speech_init_t*           init
);


/* ////////////////////////////////////////////////////////////////////// */

#ifdef __cplusplus
# ifndef PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
inline pan_be_speech_init_t::pan_be_speech_init_t()
{
    pantheios_be_speech_getDefaultAppInit(this);
}
# endif /* !PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT */
#endif /* __cplusplus */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BEC_SPEECH */

/* ///////////////////////////// end of file //////////////////////////// */
