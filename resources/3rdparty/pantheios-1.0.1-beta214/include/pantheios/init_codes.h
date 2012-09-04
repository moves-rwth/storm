/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/init_codes.h
 *
 * Purpose:     Back-/Front-end initialisation codes.
 *
 * Created:     27th September 2007
 * Updated:     27th December 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2007-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/init_codes.h
 *
 * [C, C++] Back-/Front-end initialisation codes.
 *
 * This file defines advisory status codes for back-end & front-end
 * initialisation.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_INIT_CODES
#define PANTHEIOS_INCL_PANTHEIOS_H_INIT_CODES

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_H_INIT_CODES_MAJOR     2
# define PANTHEIOS_VER_PANTHEIOS_H_INIT_CODES_MINOR     5
# define PANTHEIOS_VER_PANTHEIOS_H_INIT_CODES_REVISION  1
# define PANTHEIOS_VER_PANTHEIOS_H_INIT_CODES_EDIT      17
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

/* /////////////////////////////////////////////////////////////////////////
 * Documentation
 */

/** \defgroup group__init_codes Pantheios Low-level Initialisation Codes
 *
 * Status codes for use by the implementors of the various sub-systems,
 * including \ref group__core_library "core",
 * \ref group__backend "back-ends",
 * \ref group__frontend "front-ends", and so on
 */

/** \defgroup group__init_codes__core Pantheios Core Low-level Initialisation Codes
 *
 * \ingroup group__init_codes
 *
 * Status codes used in the implementation of the \ref group__core_library "core".
 */

/** \defgroup group__init_codes__backend Pantheios Back-End Low-level Initialisation Codes
 *
 * \ingroup group__init_codes
 *
 * Status codes for use by the implementors of \ref group__backend "back-ends".
 */

/** \defgroup group__init_codes__frontend Pantheios Front-End Low-level Initialisation Codes
 *
 * \ingroup group__init_codes
 *
 * Status codes for use by the implementors of \ref group__frontend "front-ends".
 */

/* /////////////////////////////////////////////////////////////////////////
 * Status codes
 */

/** \def PANTHEIOS_INIT_RC_SUCCESS
 *
 * Specifies that the operation completed successfully
 *
 * \ingroup group__init_codes
 */
#define PANTHEIOS_INIT_RC_SUCCESS                       (0)

/** \def PANTHEIOS_INIT_RC_OUT_OF_MEMORY
 *
 * Specifies that the operation failed due to memory exhaustion
 *
 * \ingroup group__init_codes
 */
#define PANTHEIOS_INIT_RC_OUT_OF_MEMORY                 (-1)

/** \def PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION
 *
 * Specifies that the operation failed because a general, standard-derived, exception was thrown
 *
 * \ingroup group__init_codes
 */
#define PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION         (-2)

/** \def PANTHEIOS_INIT_RC_UNKNOWN_FAILURE
 *
 * Specifies that the operation failed because a bespoke, non-standard-derived, exception was thrown; this may indicate a design failure.
 *
 * \ingroup group__init_codes
 */
#define PANTHEIOS_INIT_RC_UNKNOWN_FAILURE               (-3)

/** \def PANTHEIOS_INIT_RC_UNSPECIFIED_FAILURE
 *
 * Specifies that the operation failed in an unspecified manner. Callers may be able to retrieve more specific information from the platform-specific error mechanism (e.g. errno / GetLastError()).
 *
 * \ingroup group__init_codes
 */
#define PANTHEIOS_INIT_RC_UNSPECIFIED_FAILURE           (-4)

/** \def PANTHEIOS_INIT_RC_NOT_IMPLEMENTED
 *
 * Specifies that the operation failed because it is not implemented
 *
 * \ingroup group__init_codes
 */
#define PANTHEIOS_INIT_RC_NOT_IMPLEMENTED               (-5)

/** \def PANTHEIOS_INIT_RC_CANNOT_CREATE_TSS_INDEX
 *
 * Specifies that the operation failed because a Thread-Specific Storage key
 * could not be created.
 *
 * \ingroup group__init_codes
 */
#define PANTHEIOS_INIT_RC_CANNOT_CREATE_TSS_INDEX       (-6)

/** \def PANTHEIOS_INIT_RC_CANNOT_CREATE_THREAD
 *
 * Specifies that the operation failed because a thread could not be
 * created.
 *
 * \ingroup group__init_codes
 */
#define PANTHEIOS_INIT_RC_CANNOT_CREATE_THREAD          (-7)



/** \def PANTHEIOS_BE_INIT_RC_NO_BACKENDS_SPECIFIED
 *
 * Specifies that the operation failed because no backends were specified
 *
 * \ingroup group__init_codes__backend
 */
#define PANTHEIOS_BE_INIT_RC_NO_BACKENDS_SPECIFIED      (-10001)

/** \def PANTHEIOS_BE_INIT_RC_ALL_BACKEND_INITS_FAILED
 *
 * Specifies that the operation failed because all backends failed to initialise
 *
 * \ingroup group__init_codes__backend
 */
#define PANTHEIOS_BE_INIT_RC_ALL_BACKEND_INITS_FAILED   (-10002)

/** \def PANTHEIOS_BE_INIT_RC_INVALID_PROCESSID
 *
 * Specifies that the operation failed due to specification of an invalid process identifier
 *
 * \ingroup group__init_codes__backend
 */
#define PANTHEIOS_BE_INIT_RC_INVALID_PROCESSID          (-10003)

/** \def PANTHEIOS_BE_INIT_RC_API_MUTEX_INIT_FAILED
 *
 * Specifies that the operation failed because mutex initialisation failed
 *
 * \ingroup group__init_codes__backend
 */
#define PANTHEIOS_BE_INIT_RC_API_MUTEX_INIT_FAILED      (-10004)

/** \def PANTHEIOS_BE_INIT_RC_INTENDED_FAILURE
 *
 * Specifies that the operation failed deliberately, according to its design
 *
 * \ingroup group__init_codes__backend
 */
#define PANTHEIOS_BE_INIT_RC_INTENDED_FAILURE           (-10005)

/** \def PANTHEIOS_BE_INIT_RC_INIT_PARAM_REQUIRED
 *
 * Specifies that the operation failed because a required parameter was not supplied
 *
 * \ingroup group__init_codes__backend
 */
#define PANTHEIOS_BE_INIT_RC_INIT_PARAM_REQUIRED        (-10006)

/** \def PANTHEIOS_BE_INIT_RC_INVALID_ARGUMENT
 *
 * Specifies that the operation failed due to an invalid argument being specified
 *
 * \ingroup group__init_codes__backend
 */
#define PANTHEIOS_BE_INIT_RC_INVALID_ARGUMENT           (-10007)

/** \def PANTHEIOS_BE_INIT_RC_ARGUMENT_TOO_LONG
 *
 * Specifies that the operation failed due to an argument being too long
 *
 * \ingroup group__init_codes__backend
 */
#define PANTHEIOS_BE_INIT_RC_ARGUMENT_TOO_LONG          (-10008)

/** \def PANTHEIOS_BE_INIT_RC_ARGUMENT_OUT_OF_RANGE
 *
 * Specifies that the operation failed due to an argument being out of range
 *
 * \ingroup group__init_codes__backend
 */
#define PANTHEIOS_BE_INIT_RC_ARGUMENT_OUT_OF_RANGE      (-10009)

/** \def PANTHEIOS_BE_INIT_RC_INIT_CONFIG_REQUIRED
 *
 * Specifies that the operation failed because required initialisation configuration information was missing or not supplied
 *
 * \ingroup group__init_codes__backend
 */
#define PANTHEIOS_BE_INIT_RC_INIT_CONFIG_REQUIRED       (-10010)

/** \def PANTHEIOS_BE_INIT_RC_PERMISSION_DENIED
 *
 * Specifies that the operation failed because permission to access a required resource (such as a file) was denied
 *
 * \ingroup group__init_codes__backend
 */
#define PANTHEIOS_BE_INIT_RC_PERMISSION_DENIED          (-10011)

/** \def PANTHEIOS_BE_INIT_RC_RESOURCE_BUSY
 *
 * Specifies that the operation failed because a required resource (such as a file) was already in use
 *
 * \ingroup group__init_codes__backend
 */
#define PANTHEIOS_BE_INIT_RC_RESOURCE_BUSY              (-10012)

/** \def PANTHEIOS_BE_INIT_RC_FUTURE_VERSION_REQUESTED
 *
 * Specifies that the operation failed because a version was requested of a later version of Pantheios than was used to create the back-end
 *
 * \ingroup group__init_codes__backend
 */
#define PANTHEIOS_BE_INIT_RC_FUTURE_VERSION_REQUESTED   (-10013)

/** \def PANTHEIOS_BE_INIT_RC_OLD_VERSION_NOT_SUPPORTED
 *
 * Specifies that the operation failed because a version was requested of a previous version of Pantheios that is no longer supported by the back-end
 *
 * \ingroup group__init_codes__backend
 */
#define PANTHEIOS_BE_INIT_RC_OLD_VERSION_NOT_SUPPORTED  (-10014)

/** \def PANTHEIOS_BE_LOGENTRY_FAILED
 *
 * Specifies that the underlying call to <code>pantheios_be_logEntry()</code> failed; the back-end may have used bail-out logging to record the problem.
 *
 * \ingroup group__init_codes__backend
 */
#define PANTHEIOS_BE_LOGENTRY_FAILED                    (-10015)


/** \def PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE
 *
 * Specifies that the operation failed deliberately, according to its design
 *
 * \ingroup group__init_codes__frontend
 */
#define PANTHEIOS_FE_INIT_RC_INTENDED_FAILURE           (-20001)

/** \def PANTHEIOS_FE_INIT_RC_SYSTEM_NOT_CONFIGURED
 *
 * Specifies that the operation failed because the system is not correctly configured
 *
 * \ingroup group__init_codes__frontend
 */
#define PANTHEIOS_FE_INIT_RC_SYSTEM_NOT_CONFIGURED      (-20002)

/** \def PANTHEIOS_FE_INIT_RC_INIT_CONFIG_REQUIRED
 *
 * Specifies that the operation failed because required initialisation configuration information was missing or not supplied
 *
 * \ingroup group__init_codes__frontend
 */
#define PANTHEIOS_FE_INIT_RC_INIT_CONFIG_REQUIRED       (-20003)

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_INIT_CODES */

/* ///////////////////////////// end of file //////////////////////////// */
