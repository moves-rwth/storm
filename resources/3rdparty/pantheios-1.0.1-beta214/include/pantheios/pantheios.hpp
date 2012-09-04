/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/pantheios.hpp
 *
 * Purpose:     Primary header file for Pantheios
 *
 * Created:     21st June 2005
 * Updated:     9th January 2011
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2011, Matthew Wilson and Synesis Software
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


/** \file pantheios/pantheios.hpp
 *
 * [C++ only] Primary header file for the
 *    \ref group__application_layer_interface, and included in all
 *    C++ compilation units.
 *
 * Users should include this file to ensure that the Pantheios library is
 * automatically initialised, and to access
 * the \ref group__application_layer_interface.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_HPP_PANTHEIOS
#define PANTHEIOS_INCL_PANTHEIOS_HPP_PANTHEIOS

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_HPP_PANTHEIOS_MAJOR      3
# define PANTHEIOS_VER_PANTHEIOS_HPP_PANTHEIOS_MINOR      9
# define PANTHEIOS_VER_PANTHEIOS_HPP_PANTHEIOS_REVISION   2
# define PANTHEIOS_VER_PANTHEIOS_HPP_PANTHEIOS_EDIT       102
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

/* Main Pantheios Header */
#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

#ifndef __cplusplus
# error pantheios/pantheios.hpp is only valid in C++ compilation units
#endif /* !__cplusplus */

/* Required to validate the parameters are not integral types */
#ifndef STLSOFT_INCL_STLSOFT_META_HPP_IS_FUNDAMENTAL_TYPE
# include <stlsoft/meta/is_fundamental_type.hpp>
#endif /* !STLSOFT_INCL_STLSOFT_META_HPP_IS_FUNDAMENTAL_TYPE */

/* Now select various headers to bring in string access shims */
#ifndef STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_H_FWD
# include <stlsoft/shims/access/string/fwd.h>
#endif /* !STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_H_FWD */

/* Backwards-compatibility */
#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# if defined(PANTHEIOS_NO_INCLUDE_STLSOFT_SHIM_ACCESS_SHIMS) && \
     !defined(PANTHEIOS_NO_INCLUDE_STLSOFT_STRING_ACCESS)
#  ifdef STLSOFT_CF_PRAGMA_MESSAGE_SUPPORT
#   pragma messsage("The symbol PANTHEIOS_NO_INCLUDE_STLSOFT_SHIM_ACCESS_SHIMS is deprecated, and support for it will be removed before 1.0.1. Please use PANTHEIOS_NO_INCLUDE_STLSOFT_STRING_ACCESS instead.")
#  endif /* STLSOFT_CF_PRAGMA_MESSAGE_SUPPORT */
#  define PANTHEIOS_NO_INCLUDE_STLSOFT_STRING_ACCESS
# endif /* PANTHEIOS_NO_INCLUDE_STLSOFT_SHIM_ACCESS_SHIMS && !PANTHEIOS_NO_INCLUDE_STLSOFT_STRING_ACCESS */
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */



#ifdef PANTHEIOS_CUSTOM_SHIM_INCLUDE
# include PANTHEIOS_CUSTOM_SHIM_INCLUDE
#else /* ? PANTHEIOS_CUSTOM_SHIM_INCLUDE */
# if !defined(PANTHEIOS_NO_INCLUDE_STLSOFT_STRING_ACCESS)
#  ifndef STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_HPP_STRING
#   include <stlsoft/shims/access/string.hpp>
#  endif /* !STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_HPP_STRING */
#  ifndef STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_STD_HPP_TIME
#   include <stlsoft/shims/access/string/std/time.hpp>
#  endif /* !STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_STRING_STD_HPP_TIME */


#  ifdef PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS
#   define PANTHEIOS_NO_INCLUDE_ACESTL_STRING_ACCESS
#   define PANTHEIOS_NO_INCLUDE_ATLSTL_STRING_ACCESS
#   define PANTHEIOS_NO_INCLUDE_COMSTL_STRING_ACCESS
#   define PANTHEIOS_NO_INCLUDE_MFCSTL_STRING_ACCESS
#   define PANTHEIOS_NO_INCLUDE_PLATFORMSTL_STRING_ACCESS
#   define PANTHEIOS_NO_INCLUDE_UNIXSTL_STRING_ACCESS
#   define PANTHEIOS_NO_INCLUDE_WINSTL_STRING_ACCESS
#   define PANTHEIOS_NO_INCLUDE_WTLSTL_STRING_ACCESS
#  endif /* PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS */

 /* Include shims for MFC, if
  *
  *  - it's not been proscribed, by PANTHEIOS_NO_INCLUDE_MFCSTL_STRING_ACCESS, and either
  *      - it's been explicitly requested, by PANTHEIOS_FORCE_INCLUDE_MFCSTL_STRING_ACCESS, or
  *      - _AFX and _MFC_VER are defined
  */
#  if !defined(PANTHEIOS_NO_INCLUDE_MFCSTL_STRING_ACCESS) && \
      (   defined(PANTHEIOS_FORCE_INCLUDE_MFCSTL_STRING_ACCESS) || \
          defined(_AFXDLL) || \
          (   defined(_AFX) && \
              defined(_MFC_VER)))
#   ifndef MFCSTL_INCL_MFCSTL_SHIMS_ACCESS_HPP_STRING
#    include <mfcstl/shims/access/string.hpp>
#   endif /* !MFCSTL_INCL_MFCSTL_SHIMS_ACCESS_HPP_STRING */
#  endif /* MFC */

 /* Include shims for ATL, if
  *
  *  - it's not been proscribed, by PANTHEIOS_NO_INCLUDE_ATLSTL_STRING_ACCESS, and either
  *      - it's been explicitly requested, by PANTHEIOS_FORCE_INCLUDE_ATLSTL_STRING_ACCESS, or
  *      - _ATL and _ATL_VER are defined
  */
#  if !defined(PANTHEIOS_NO_INCLUDE_ATLSTL_STRING_ACCESS) && \
      (   defined(PANTHEIOS_FORCE_INCLUDE_ATLSTL_STRING_ACCESS) || \
          (   defined(_ATL) && \
              defined(_ATL_VER)))
#   ifndef ATLSTL_INCL_ATLSTL_SHIMS_ACCESS_HPP_STRING
#    include <atlstl/shims/access/string.hpp>
#   endif /* !ATLSTL_INCL_ATLSTL_SHIMS_ACCESS_HPP_STRING */
#  endif /* ATL */

 /* Include shims for COM, if
  *
  *  - it's not been proscribed, by PANTHEIOS_NO_INCLUDE_COMSTL_STRING_ACCESS, and either
  *      - it's been explicitly requested, by PANTHEIOS_FORCE_INCLUDE_COMSTL_STRING_ACCESS, or
  *      - WIN32/WIN64 is defined
  */
#  if !defined(PANTHEIOS_NO_INCLUDE_COMSTL_STRING_ACCESS) && \
      (   defined(PANTHEIOS_FORCE_INCLUDE_COMSTL_STRING_ACCESS) || \
          defined(WIN32) || \
          defined(WIN64))
#   ifndef COMSTL_INCL_COMSTL_SHIMS_ACCESS_HPP_STRING
#    include <comstl/shims/access/string.hpp>
#   endif /* !COMSTL_INCL_COMSTL_SHIMS_ACCESS_HPP_STRING */
#  endif /* COM */

 /* Include shims for UNIX, if
  *
  *  - it's not been proscribed, by PANTHEIOS_NO_INCLUDE_UNIXSTL_STRING_ACCESS, and either
  *      - it's been explicitly requested, by PANTHEIOS_FORCE_INCLUDE_UNIXSTL_STRING_ACCESS, or
  *      - unix, __unix, __unix__ or UNIX is defined
  */
#  if !defined(PANTHEIOS_NO_INCLUDE_UNIXSTL_STRING_ACCESS) && \
      (   defined(PANTHEIOS_FORCE_INCLUDE_UNIXSTL_STRING_ACCESS) || \
          (  defined(unix) || \
             defined(__unix) || \
             defined(__unix__) || \
             defined(UNIX)))
#   ifndef UNIXSTL_INCL_UNIXSTL_SHIMS_ACCESS_HPP_STRING
#    include <unixstl/shims/access/string.hpp>
#   endif /* !UNIXSTL_INCL_UNIXSTL_SHIMS_ACCESS_HPP_STRING */
#  endif /* UNIX */

 /* Include shims for Windows, if
  *
  *  - it's not been proscribed, by PANTHEIOS_NO_INCLUDE_WINSTL_STRING_ACCESS, and either
  *      - it's been explicitly requested, by PANTHEIOS_FORCE_INCLUDE_WINSTL_STRING_ACCESS, or
  *      - WIN32/WIN64 is defined
  */
#  if !defined(PANTHEIOS_NO_INCLUDE_WINSTL_STRING_ACCESS) && \
      (   defined(PANTHEIOS_FORCE_INCLUDE_WINSTL_STRING_ACCESS) || \
          defined(WIN32) || \
          defined(WIN64))
#   ifndef WINSTL_INCL_WINSTL_SHIMS_ACCESS_HPP_STRING
#    include <winstl/shims/access/string.hpp>
#   endif /* !WINSTL_INCL_WINSTL_SHIMS_ACCESS_HPP_STRING */
#   ifndef WINSTL_INCL_WINSTL_SHIMS_ACCESS_STRING_HPP_TIME
#    include <winstl/shims/access/string/time.hpp>
#   endif /* !WINSTL_INCL_WINSTL_SHIMS_ACCESS_STRING_HPP_TIME */
#  endif /* Win */

# endif /* !PANTHEIOS_NO_INCLUDE_STLSOFT_STRING_ACCESS */
#endif /* PANTHEIOS_CUSTOM_SHIM_INCLUDE */

/* /////////////////////////////////////////////////////////////////////////
 * GCC bug
 */

/* NOTE: We must include <pantheios/inserters.hpp> here, as GCC is a bit
 * thick about its two-phase name lookup when resolving shim functions for
 * the Pantheios inserter types integer, pointer and real.
 */

#if defined(STLSOFT_COMPILER_IS_GCC) && \
    __GNUC__ < 4
//# include <pantheios/inserters.hpp>
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * C++ API components
 *
 * To suppress all generated functions, define the preprocessor symbol
 *  PANTHEIOS_NO_GENERATED_FUNCTIONS
 *
 * To suppress all log() generated functions, define the preprocessor
 * symbol
 *  PANTHEIOS_NO_LOG_FUNCTIONS
 *
 * To suppress all log-specific generated functions (i.e. log_DEBUG(),
 * log_CRITICAL(), etc.), define the preprocessor symbol
 * <code>PANTHEIOS_NO_LOG_SEV_FUNCTIONS</code>. Note: if the symbol
 * <code>PANTHEIOS_NO_STOCK_LEVELS</code> is defined, then
 * <code>PANTHEIOS_NO_LOG_SEV_FUNCTIONS</code> will be defined
 * automatically.
 */

#ifdef PANTHEIOS_NO_STOCK_LEVELS
# ifndef PANTHEIOS_NO_LOG_SEV_FUNCTIONS
#  define PANTHEIOS_NO_LOG_SEV_FUNCTIONS
# endif /* !PANTHEIOS_NO_LOG_SEV_FUNCTIONS */
#endif /* PANTHEIOS_NO_STOCK_LEVELS */

#ifdef PANTHEIOS_INVOKE_SHIM_PAIR_
# undef PANTHEIOS_INVOKE_SHIM_PAIR_
#endif /* PANTHEIOS_INVOKE_SHIM_PAIR_ */

#ifdef PANTHEIOS_VALIDATE_TYPE_NOT_FUNDAMENTAL_
# undef PANTHEIOS_VALIDATE_TYPE_NOT_FUNDAMENTAL_
#endif /* PANTHEIOS_VALIDATE_TYPE_NOT_FUNDAMENTAL_ */

/* /////////////////////////////////////////////////////////////////////////
 * Compiler compatibility
 */

/* The Digital Mars and GCC compilers both exhibit strange behaviour (bugs?)
 * with respect to the invocation of shims.
 *
 * - DMC++ requires explicit qualification, even in the presence of a
 *    (local) using declaration
 * - GCC requires a local using declaration and *must not* have explicit
 *    qualification.
 *
 * - all other compilers work fine without any using declaration and with
 *    explicit qualification.
 *
 * As currently defined, we follow the GCC way (which works for all other
 * compilers, except DMC++), and explicitly qualify for DMC++.
 *
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

# ifdef PANTHEIOS_INVOKE_c_str_data_
#  undef PANTHEIOS_INVOKE_c_str_data_
# endif /* PANTHEIOS_INVOKE_c_str_data_ */
# ifdef PANTHEIOS_INVOKE_c_str_len_
#  undef PANTHEIOS_INVOKE_c_str_len_
# endif /* PANTHEIOS_INVOKE_c_str_len_ */
# ifdef PANTHEIOS_INVOKE_c_str_ptr_
#  undef PANTHEIOS_INVOKE_c_str_ptr_
# endif /* PANTHEIOS_INVOKE_c_str_ptr_ */
# ifdef PANTHEIOS_INVOKE_SHIM_PAIR_
#  undef PANTHEIOS_INVOKE_SHIM_PAIR_
# endif /* PANTHEIOS_INVOKE_SHIM_PAIR_ */
# ifdef PANTHEIOS_c_str_data_name_
#  undef PANTHEIOS_c_str_data_name_
# endif /* PANTHEIOS_c_str_data_name_ */

# ifdef PANTHEIOS_USE_WIDE_STRINGS
#  define PANTHEIOS_c_str_data_name_                    c_str_data_w
#  define PANTHEIOS_c_str_len_name_                     c_str_len_w
#  define PANTHEIOS_c_str_ptr_name_                     c_str_ptr_w
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */               
#  define PANTHEIOS_c_str_data_name_                    c_str_data_a
#  define PANTHEIOS_c_str_len_name_                     c_str_len_a
#  define PANTHEIOS_c_str_ptr_name_                     c_str_ptr_a
# endif /* PANTHEIOS_USE_WIDE_STRINGS */                
                                                        
# if defined(STLSOFT_COMPILER_IS_DMC)                   
#  define PANTHEIOS_DECLARE_SHIM_PAIR_()                ((void)0)
#  define PANTHEIOS_INVOKE_SHIM_PAIR_(x)                stlsoft::PANTHEIOS_c_str_len_name_(x), stlsoft::PANTHEIOS_c_str_data_name_(x)
# elif defined(STLSOFT_COMPILER_IS_GCC)                 
#  define PANTHEIOS_DECLARE_SHIM_PAIR_()                using ::stlsoft::PANTHEIOS_c_str_data_name_; using ::stlsoft::PANTHEIOS_c_str_len_name_
#  define PANTHEIOS_INVOKE_SHIM_PAIR_(x)                PANTHEIOS_c_str_len_name_(x), PANTHEIOS_c_str_data_name_(x)
# else /* ? compiler */                                 
#  define PANTHEIOS_DECLARE_SHIM_PAIR_()                using ::stlsoft::PANTHEIOS_c_str_data_name_; using ::stlsoft::PANTHEIOS_c_str_len_name_
#  define PANTHEIOS_INVOKE_SHIM_PAIR_(x)                PANTHEIOS_c_str_len_name_(x), PANTHEIOS_c_str_data_name_(x)
# endif /* compiler */

# define PANTHEIOS_VALIDATE_TYPE_NOT_FUNDAMENTAL_(t)    STLSOFT_STATIC_ASSERT(0 == stlsoft::is_fundamental_type<t>::value)

#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

#ifndef PANTHEIOS_NO_GENERATED_FUNCTIONS
# ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
namespace internal
{
#  include "./internal/generated/log_dispatch_functions.h"
#  include "./internal/generated/log_dispatch_functions.hpp"
} /* namespace internal */
# endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
# if !defined(PANTHEIOS_NO_LOG_FUNCTIONS)
#  include "./internal/generated/log_functions.hpp"     // log(s), log(s, s) etc etc
# endif /* !PANTHEIOS_NO_LOG_FUNCTIONS */
# if !defined(PANTHEIOS_NO_LOG_SEV_FUNCTIONS)
#  include "./internal/generated/log_sev_functions.hpp" // log_ALERT() overloads, log_ERROR() overloads, etc.
# endif /* !PANTHEIOS_NO_LOG_SEV_FUNCTIONS */
#endif /* !PANTHEIOS_NO_GENERATED_FUNCTIONS */

#undef PANTHEIOS_INVOKE_SHIM_PAIR_

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Auto-initialisation
 *
 * Except when making a DLL, this is automatic, unless PANTHEIOS_NO_AUTO_INIT
 * is defined.
 *
 * To force it for a DLL, define PANTHEIOS_FORCE_AUTO_INIT
 */

#if defined(__DLL__) /* Borland, with -WD */ || \
    defined(_WINDLL) /* VC++, defined by user/wizard */ || \
    defined(_USRDLL) /* VC++, defined by user/wizard */
# define PANTHEIOS_NO_AUTO_INIT
#endif /* dynamic library */

#if defined(PANTHEIOS_FORCE_AUTO_INIT) || \
    !defined(PANTHEIOS_NO_AUTO_INIT)
# include "./internal/initialiser.hpp"    // Schwarz counter initialisation
#endif /* PANTHEIOS_FORCE_AUTO_INIT || !PANTHEIOS_NO_AUTO_INIT */

/* /////////////////////////////////////////////////////////////////////////
 * Inclusion
 */

#ifdef STLSOFT_PPF_pragma_once_SUPPORT
# pragma once
#endif /* STLSOFT_PPF_pragma_once_SUPPORT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_HPP_PANTHEIOS */

/* ///////////////////////////// end of file //////////////////////////// */
