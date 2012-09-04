/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/util/backends/arguments.h
 *
 * Purpose:     Pantheios back end API
 *
 * Created:     21st June 2005
 * Updated:     10th August 2009
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2009, Matthew Wilson and Synesis Software
 * Copyright (c) 1999-2005, Synesis Software and Matthew Wilson
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


/** \file pantheios/util/backends/arguments.h
 *
 * [C, C++] Definition of the back-end parsing functions.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_UTIL_BACKENDS_H_ARGUMENTS
#define PANTHEIOS_INCL_PANTHEIOS_UTIL_BACKENDS_H_ARGUMENTS

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_UTIL_BACKENDS_H_ARGUMENTS_MAJOR    2
# define PANTHEIOS_VER_PANTHEIOS_UTIL_BACKENDS_H_ARGUMENTS_MINOR    1
# define PANTHEIOS_VER_PANTHEIOS_UTIL_BACKENDS_H_ARGUMENTS_REVISION 1
# define PANTHEIOS_VER_PANTHEIOS_UTIL_BACKENDS_H_ARGUMENTS_EDIT     22
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

/* /////////////////////////////////////////////////////////////////////////
 * Parsing functions
 */

/** Parses a single boolean back-end argument
 *
 * \ingroup group__backend
 *
 * Searches the argument slices list for an argument of the given name. If
 * it has one of the values ['yes', 'true', 'no', 'false'] the flags
 * parameter is adjusted to either set or remove the flagValue value, as
 * directed by the flagSuppressesAction parameter, and the argument is
 * marked as having been processed by setting its length to 0.
 *
 * \param numArgs Number of elements in the argument array. May be 0
 * \param args Pointer to the base of the argument array. May only be NULL
 *   if numArgs is 0
 * \param argName The name of the value to look for. May not be NULL
 * \param flagSuppressesAction Indicates whether the flagValue is a
 *   suppression flag, such as PANTHEIOS_BE_INIT_F_NO_SEVERITY, rather than
 *   an activation flag, such as PANTHEIOS_BE_INIT_F_USE_SYSTEM_TIME.
 * \param flagValue A flag value, such as PANTHEIOS_BE_INIT_F_NO_SEVERITY
 * \param flags A pointer to a flags variable whose value may be modified
 *   upon a successful match. May not be NULL.
 *
 * \return An indication of whether a valid argument was found
 * \retval 0 No matching arguments were found, or the argument was found but
 *   did not have boolean values
 * \retval 1 An argument with the given name, and with values interpretable as
 *   boolean, was found.
 *
 * \pre NULL != args || 0 == numArgs
 * \pre NULL != argName
 * \pre NULL != flags
 */
PANTHEIOS_CALL(int) pantheios_be_parseBooleanArg(size_t                         numArgs
#ifdef PANTHEIOS_NO_NAMESPACE
                                            ,   pan_slice_t* const              args
                                            ,   pan_char_t const*               argName
                                            ,   int                             flagSuppressesAction
                                            ,   pan_uint32_t                    flagValue
                                            ,   pan_uint32_t*                   flags);
#else /* ? PANTHEIOS_NO_NAMESPACE */
                                            ,   pantheios::pan_slice_t* const   args
                                            ,   pantheios::pan_char_t const*    argName
                                            ,   int                             flagSuppressesAction
                                            ,   pantheios::pan_uint32_t         flagValue
                                            ,   pantheios::uint32_t*            flags);
#endif /* PANTHEIOS_NO_NAMESPACE */


/** Parses a single string back-end argument
 *
 * \ingroup group__backend
 *
 * Searches the argument slices list for an argument of the given name. If
 * found, it transfers the slice (i.e. copies the pointers) to the given
 * argument value parameter.
 *
 * \param numArgs Number of elements in the argument array. May be 0
 * \param args Pointer to the base of the argument array. May only be NULL
 *   if numArgs is 0
 * \param argName The name of the value to look for. May not be NULL
 * \param argValue A pointer to a slice that will receive the value part of
 *   an argument if successfully matched. May not be NULL.
 *
 * \pre NULL != args || 0 == numArgs
 * \pre NULL != argName
 * \pre NULL != argValue
 */
PANTHEIOS_CALL(int) pantheios_be_parseStringArg(size_t                          numArgs
#ifdef PANTHEIOS_NO_NAMESPACE
                                            ,   pan_slice_t* const              args
                                            ,   pan_char_t const*               argName
                                            ,   pan_slice_t*                    argValue);
#else /* ? PANTHEIOS_NO_NAMESPACE */
                                            ,   pantheios::pan_slice_t* const   args
                                            ,   pantheios::pan_char_t const*    argName
                                            ,   pantheios::pan_slice_t*         argValue);
#endif /* PANTHEIOS_NO_NAMESPACE */


/** Parses the stock back-end arguments
 *
 * \ingroup group__backend
 *
 * Searches the argument slices list for the arguments associated with all
 * stock \ref group__backend__init__flags. For each argument found (and
 * having an appropriate value; see pantheios_be_parseBooleanArg()) its
 * value is processed, the flags parameter's value is adjusted
 * accordingly, and the argument slice's length is set to 0 to indicate
 * that it has been successfully processed. Returns the number of arguments
 * successfully matched.
 *
 * Recognises the following argument names:
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
 * \param numArgs Number of elements in the argument array. May be 0
 * \param args Pointer to the base of the argument array. May only be NULL
 *   if numArgs is 0
 * \param flags A pointer to a flags variable whose value may be modified
 *   upon a successful matches. May not be NULL.
 *
 * \return The number of arguments successfully matched
 *
 * \pre NULL != args || 0 == numArgs
 * \pre NULL != flags
 */
PANTHEIOS_CALL(int) pantheios_be_parseStockArgs(size_t                          numArgs
#ifdef PANTHEIOS_NO_NAMESPACE
                                            ,   pan_slice_t* const              args
                                            ,   pan_uint32_t*                   flags);
#else /* ? PANTHEIOS_NO_NAMESPACE */
                                            ,   pantheios::pan_slice_t* const   args
                                            ,   pantheios::uint32_t*            flags);
#endif /* PANTHEIOS_NO_NAMESPACE */


/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_UTIL_BACKENDS_H_ARGUMENTS */

/* ///////////////////////////// end of file //////////////////////////// */
