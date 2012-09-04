/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/backends/be.lrsplit.h
 *
 * Purpose:     Pantheios Local/Remote Split Back-end library API
 *
 * Created:     26th June 2005
 * Updated:     11th September 2009
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


/** \file pantheios/backends/be.lrsplit.h
 *
 * [C, C++] Pantheios Local/Remote Split Back-end library API
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BE_LRSPLIT
#define PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BE_LRSPLIT

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BE_LRSPLIT_MAJOR    2
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BE_LRSPLIT_MINOR    1
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BE_LRSPLIT_REVISION 1
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BE_LRSPLIT_EDIT     11
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

/** \defgroup group__backend__stock_backends__lrsplit Pantheios Local/Remote Split Back-end
 * \ingroup group__backend__stock_backends
 *  Back-end library that splits the emission of log statements,
 *   writing to <b>local</b> and/or <b>remote</b> back-ends that are
 *   independently filtered.
 */

/* /////////////////////////////////////////////////////////////////////////
 * Pantheios Local/Remote Split backend library API
 */

/** Initialises the local back-end API.
 *
 * \ingroup group__backend__stock_backends__lrsplit
 *
 * \note The semantics of this function, and the constraints on its behaviour,
 *  are identical to those of the Pantheios backend library function
 *  pantheios_be_init(), except it applies to the local backend only
 */
PANTHEIOS_CALL(int) pantheios_be_local_init(
    PAN_CHAR_T const*   processIdentity
,   void*               reserved
,   void**              ptoken
);

/** Initialises the remote back-end API.
 *
 * \ingroup group__backend__stock_backends__lrsplit
 *
 * \note The semantics of this function, and the constraints on its behaviour,
 *  are identical to those of the Pantheios backend library function
 *  pantheios_be_init(), except it applies to the remote backend only
 */
PANTHEIOS_CALL(int) pantheios_be_remote_init(
    PAN_CHAR_T const*   processIdentity
,   void*               reserved
,   void**              ptoken
);

/** Uninitialises the local back-end API.
 *
 * \ingroup group__backend__stock_backends__lrsplit
 *
 * \note The semantics of this function, and the constraints on its behaviour,
 *  are identical to those of the Pantheios backend library function
 *  pantheios_be_uninit(), except it applies to the local backend only
 */
PANTHEIOS_CALL(void) pantheios_be_local_uninit(
    void* token
);
/** Uninitialises the remote back-end API.
 *
 * \ingroup group__backend__stock_backends__lrsplit
 *
 * \note The semantics of this function, and the constraints on its behaviour,
 *  are identical to those of the Pantheios backend library function
 *  pantheios_be_uninit(), except it applies to the remote backend only
 */
PANTHEIOS_CALL(void) pantheios_be_remote_uninit(
    void* token
);

/** Passes a log-entry to the local back-end API.
 *
 * \ingroup group__backend__stock_backends__lrsplit
 *
 * \note The semantics of this function, and the constraints on its behaviour,
 *  are identical to those of the Pantheios backend library function
 *  pantheios_be_logEntry(), except it applies to the local backend only
 */
PANTHEIOS_CALL(int) pantheios_be_local_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
);
/** Passes a log-entry to the remote back-end API.
 *
 * \ingroup group__backend__stock_backends__lrsplit
 *
 * \note The semantics of this function, and the constraints on its behaviour,
 *  are identical to those of the Pantheios backend library function
 *  pantheios_be_logEntry(), except it applies to the remote backend only
 */
PANTHEIOS_CALL(int) pantheios_be_remote_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
);

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BE_LRSPLIT */

/* ///////////////////////////// end of file //////////////////////////// */
