/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/backends/bec.WindowsMessageBox.h
 *
 * Purpose:     Declaration of the Pantheios WindowsMessageBox Stock Back-end API.
 *
 * Created:     8th March 2008
 * Updated:     11th September 2009
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2008-2009, Matthew Wilson and Synesis Software
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


/** \file pantheios/backends/bec.WindowsMessageBox.h
 *
 * [C, C++] Pantheios Windows Message Box Back-end Common API
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_WINDOWSMESSAGEBOX
#define PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_WINDOWSMESSAGEBOX

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

/** \defgroup group__backend__stock_backends__WindowsMessageBox Pantheios Windows Message Box Stock Back-end
 * \ingroup group__backend__stock_backends
 *  Back-end library that writes to the Windows event log.
 */

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/** Implements the functionality for pantheios_be_init() over the Windows Message Box API.
 *
 * \ingroup group__backend__stock_backends__WindowsMessageBox
 */
PANTHEIOS_CALL(int) pantheios_be_WindowsMessageBox_init(
    PAN_CHAR_T const*   processIdentity
,   int                 id
,   void*               unused
,   void*               reserved
,   void**              ptoken
);

/** Implements the functionality for pantheios_be_uninit() over the Windows Message Box API.
 * \ingroup group__backend__stock_backends__WindowsMessageBox
 */
PANTHEIOS_CALL(void) pantheios_be_WindowsMessageBox_uninit(
    void* token
);

/** Implements the functionality for pantheios_be_logEntry() over the Windows Message Box API.
 * \ingroup group__backend__stock_backends__WindowsMessageBox
 */
PANTHEIOS_CALL(int) pantheios_be_WindowsMessageBox_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
);

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_WINDOWSMESSAGEBOX */

/* ///////////////////////////// end of file //////////////////////////// */
