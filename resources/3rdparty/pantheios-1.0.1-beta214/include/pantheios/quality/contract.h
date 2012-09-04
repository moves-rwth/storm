/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/quality/contract.h (formerly pantheios/contract/assert.h)
 *
 * Purpose:     Defines the contract enforcement constructs used in the
 *              Pantheios core and APIs.
 *
 * Created:     26th June 2005
 * Updated:     2nd August 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/quality/contract.h
 *
 * [C, C++] Defines the contract enforcement constructs used in the
 *  Pantheios core and APIs.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_QUALITY_H_CONTRACT
#define PANTHEIOS_INCL_PANTHEIOS_QUALITY_H_CONTRACT

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_QUALITY_H_CONTRACT_MAJOR       3
# define PANTHEIOS_VER_PANTHEIOS_QUALITY_H_CONTRACT_MINOR       2
# define PANTHEIOS_VER_PANTHEIOS_QUALITY_H_CONTRACT_REVISION    1
# define PANTHEIOS_VER_PANTHEIOS_QUALITY_H_CONTRACT_EDIT        16
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

#if defined(PANTHEIOS_QA_USE_CUSTOM_CONTRACT)
# include PANTHEIOS_QA_CUSTOM_CONTRACT_INCLUDE
#elif defined(PANTHEIOS_QA_USE_XCONTRACT)
# include <xcontract/xcontract.h>
#else /* ? contract */
# include <stlsoft/stlsoft.h>
#endif /* contract */

/* /////////////////////////////////////////////////////////////////////////
 * Helper macros
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

# ifndef PANTHEIOS_QA_DO_MSG_TYPECHECK
   /* Since this typecheck is only needed to ensure a coding error has not
    * been made, it is not actually necessary to check it on every compiler,
    * only on at least one compiler used to verify the distribution before
    * release. Therefore, we only do the check on compilers we already know
    * to work; for any others we avoid the possible ambiguities (such as
    * those we see with GCC in C) between literals and char const*, and so
    * on.
    */

#  ifdef __cplusplus
    /* C++ */
#   if defined(STLSOFT_COMPILER_IS_BORLAND) || \
       defined(STLSOFT_COMPILER_IS_MWERKS) || \
       defined(STLSOFT_COMPILER_IS_DMC) || \
       defined(STLSOFT_COMPILER_IS_GCC) || \
       defined(STLSOFT_COMPILER_IS_MSVC)
#    define PANTHEIOS_QA_DO_MSG_TYPECHECK
#   endif /* compiler */
#  else /* ? __cplusplus */
    /* C */
#   if defined(STLSOFT_CF_C99_INLINE) || \
       defined(STLSOFT_CUSTOM_C_INLINE)
#   if defined(STLSOFT_COMPILER_IS_BORLAND) || \
       defined(STLSOFT_COMPILER_IS_MWERKS) || \
       defined(STLSOFT_COMPILER_IS_DMC) || \
       defined(STLSOFT_COMPILER_IS_MSVC)
#    define PANTHEIOS_QA_DO_MSG_TYPECHECK
#   endif /* compiler */
#  endif /* C99 or custom inline */
#  endif /* __cplusplus */
# endif /* !PANTHEIOS_QA_DO_MSG_TYPECHECK */

# if defined(PANTHEIOS_QA_DO_MSG_TYPECHECK)
STLSOFT_INLINE char const* PANTHEIOS_TYPECHECK_MSG_(char const* msg)
{
    return msg;
}

#  if defined(__cplusplus)
#   ifdef STLSOFT_CF_STATIC_ARRAY_SIZE_DETERMINATION_SUPPORT
template <size_t N>
inline char const* PANTHEIOS_TYPECHECK_MSG_(char (&ar)[N])
{
    return &ar[0];
}
#   endif /* STLSOFT_CF_STATIC_ARRAY_SIZE_DETERMINATION_SUPPORT */
inline void PANTHEIOS_TYPECHECK_MSG_(...)
{}
#  endif /* __cplusplus */

# else /* ? PANTHEIOS_QA_DO_MSG_TYPECHECK */
#  define PANTHEIOS_TYPECHECK_MSG_(msg)             (msg)
# endif /* PANTHEIOS_QA_DO_MSG_TYPECHECK */

#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Enforcements
 */

#if defined(PANTHEIOS_QA_USE_CUSTOM_CONTRACT)

 /* If the user is supplying a custom contract library, then they must
  * define all the requisite symbols, so we just test their existence.
  */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_INTERNAL
#  define PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_INTERNAL is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_INTERNAL */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_API
#  define PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_API is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_API */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_APPL_LAYER
#  define PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_APPL_LAYER is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_APPL_LAYER */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_INTERNAL
#  define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_INTERNAL is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_INTERNAL */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API
#  define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_APPL_LAYER
#  define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_APPL_LAYER is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_APPL_LAYER */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_INTERNAL
#  define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_INTERNAL is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_INTERNAL */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API
#  define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_APPL_LAYER
#  define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_APPL_LAYER is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_APPL_LAYER */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_INTERNAL
#  define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_INTERNAL is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_INTERNAL */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_API
#  define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_API is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_API */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_APPL_LAYER
#  define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_APPL_LAYER is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_APPL_LAYER */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_INTERNAL
#  define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_INTERNAL is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_INTERNAL */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API
#  define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_APPL_LAYER
#  define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_APPL_LAYER is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_APPL_LAYER */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_INTERNAL
#  define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_INTERNAL is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_INTERNAL */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_API
#  define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_API is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_API */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_APPL_LAYER
#  define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_APPL_LAYER is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_APPL_LAYER */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_INTERNAL
#  define PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_INTERNAL is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_INTERNAL */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_API
#  define PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_API is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_API */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_APPL_LAYER
#  define PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_APPL_LAYER is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_APPL_LAYER */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_INTERNAL
#  define PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_INTERNAL is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_INTERNAL */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_API
#  define PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_API is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_API */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_APPL_LAYER
#  define PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_APPL_LAYER is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_APPL_LAYER */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_INTERNAL
#  define PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_INTERNAL is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_INTERNAL */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_API
#  define PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_API is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_API */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_APPL_LAYER
#  define PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_APPL_LAYER is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_APPL_LAYER */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_APPL_DEF
#  define PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_APPL_DEF is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_APPL_DEF */

# ifndef PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION
#  define PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION is not defined by the custom contract library
# endif /* !PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION */

#elif defined(PANTHEIOS_QA_USE_XCONTRACT)

# ifndef PANTHEIOS_CONTRACT_LEVEL_INTERNAL
#  define PANTHEIOS_CONTRACT_LEVEL_INTERNAL        (41)
# endif /* !PANTHEIOS_CONTRACT_LEVEL_INTERNAL */

# ifndef PANTHEIOS_CONTRACT_LEVEL_API
#  define PANTHEIOS_CONTRACT_LEVEL_API             (43)
# endif /* !PANTHEIOS_CONTRACT_LEVEL_API */

# ifndef PANTHEIOS_CONTRACT_LEVEL_APPL_LAYER
#  define PANTHEIOS_CONTRACT_LEVEL_APPL_LAYER      (45)
# endif /* !PANTHEIOS_CONTRACT_LEVEL_APPL_LAYER */

# ifndef PANTHEIOS_CONTRACT_LEVEL_APPL_DEF
#  define PANTHEIOS_CONTRACT_LEVEL_APPL_DEF        (47)
# endif /* !PANTHEIOS_CONTRACT_LEVEL_APPL_DEF */

# define PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_INTERNAL(msg)              XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_unexpectedCondition), PANTHEIOS_CONTRACT_LEVEL_INTERNAL, NULL, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_API(msg)                   XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_unexpectedCondition), PANTHEIOS_CONTRACT_LEVEL_API, NULL, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_APPL_LAYER(msg)            XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_unexpectedCondition), PANTHEIOS_CONTRACT_LEVEL_APPL_LAYER, NULL, PANTHEIOS_TYPECHECK_MSG_(msg))

# define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_INTERNAL(expr, msg)          XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_precondition_logic), PANTHEIOS_CONTRACT_LEVEL_INTERNAL, expr, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(expr, msg)               XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_precondition_logic), PANTHEIOS_CONTRACT_LEVEL_API, expr, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_APPL_LAYER(expr, msg)        XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_precondition_logic), PANTHEIOS_CONTRACT_LEVEL_APPL_LAYER, expr, PANTHEIOS_TYPECHECK_MSG_(msg))

# define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_INTERNAL(expr, msg)         XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_precondition_parameters), PANTHEIOS_CONTRACT_LEVEL_INTERNAL, expr, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(expr, msg)              XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_precondition_parameters), PANTHEIOS_CONTRACT_LEVEL_API, expr, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_APPL_LAYER(expr, msg)       XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_precondition_parameters), PANTHEIOS_CONTRACT_LEVEL_APPL_LAYER, expr, PANTHEIOS_TYPECHECK_MSG_(msg))

# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_INTERNAL(expr, msg)        XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_postcondition_returnValue), PANTHEIOS_CONTRACT_LEVEL_INTERNAL, expr, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_API(expr, msg)             XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_postcondition_returnValue), PANTHEIOS_CONTRACT_LEVEL_API, expr, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_APPL_LAYER(expr, msg)      XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_postcondition_returnValue), PANTHEIOS_CONTRACT_LEVEL_APPL_LAYER, expr, PANTHEIOS_TYPECHECK_MSG_(msg))

# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_INTERNAL(expr, msg)         XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_postcondition_logic), PANTHEIOS_CONTRACT_LEVEL_INTERNAL, expr, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API(expr, msg)              XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_postcondition_logic), PANTHEIOS_CONTRACT_LEVEL_API, expr, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_APPL_LAYER(expr, msg)       XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_postcondition_logic), PANTHEIOS_CONTRACT_LEVEL_APPL_LAYER, expr, PANTHEIOS_TYPECHECK_MSG_(msg))

# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_INTERNAL(expr, msg)        XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_postcondition_parameters), PANTHEIOS_CONTRACT_LEVEL_INTERNAL, expr, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_API(expr, msg)             XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_postcondition_parameters), PANTHEIOS_CONTRACT_LEVEL_API, expr, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_APPL_LAYER(expr, msg)      XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_postcondition_parameters), PANTHEIOS_CONTRACT_LEVEL_APPL_LAYER, expr, PANTHEIOS_TYPECHECK_MSG_(msg))

# define PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_INTERNAL(expr, msg)             XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_invariant_class), PANTHEIOS_CONTRACT_LEVEL_INTERNAL, expr, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_API(expr, msg)                  XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_invariant_class), PANTHEIOS_CONTRACT_LEVEL_API, expr, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_APPL_LAYER(expr, msg)           XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_invariant_class), PANTHEIOS_CONTRACT_LEVEL_APPL_LAYER, expr, PANTHEIOS_TYPECHECK_MSG_(msg))

# define PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_INTERNAL(expr, msg)            XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_invariant_global), PANTHEIOS_CONTRACT_LEVEL_INTERNAL, expr, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_API(expr, msg)                 XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_invariant_global), PANTHEIOS_CONTRACT_LEVEL_API, expr, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_APPL_LAYER(expr, msg)          XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_invariant_global), PANTHEIOS_CONTRACT_LEVEL_APPL_LAYER, expr, PANTHEIOS_TYPECHECK_MSG_(msg))

# define PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_INTERNAL(expr, msg)                 XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_staticData), PANTHEIOS_CONTRACT_LEVEL_INTERNAL, expr, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_API(expr, msg)                      XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_staticData), PANTHEIOS_CONTRACT_LEVEL_API, expr, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_APPL_LAYER(expr, msg)               XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_staticData), PANTHEIOS_CONTRACT_LEVEL_APPL_LAYER, expr, PANTHEIOS_TYPECHECK_MSG_(msg))
# define PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_APPL_DEF(expr, msg)                 XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_staticData), PANTHEIOS_CONTRACT_LEVEL_APPL_DEF, expr, PANTHEIOS_TYPECHECK_MSG_(msg))

# define PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(expr)                                XCONTRACT_ENFORCE_CONDITION_TYPE_LEVEL_(XCONTRACT_NS_QUAL(xContract_intermediateAssumption), PANTHEIOS_CONTRACT_LEVEL_INTERNAL, expr, PANTHEIOS_TYPECHECK_MSG_(msg))

#else /* ? PANTHEIOS_QA_USE_XCONTRACT */

/** \def PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_INTERNAL(msg)
 *
 * \ingroup group__quality
 *
 * Unexpected condition contract enforcement (internal)
 *
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_INTERNAL(msg)              STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), 0)

/** \def PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_API(msg)
 *
 * \ingroup group__quality
 *
 * Unexpected condition contract enforcement (API)
 *
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_API(msg)                   STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), 0)

/** \def PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_APPL_LAYER(msg)
 *
 * \ingroup group__quality
 *
 * Unexpected condition contract enforcement (application layer)
 *
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_APPL_LAYER(msg)            STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), 0)


/** \def PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_INTERNAL(expr, msg)
 *
 * \ingroup group__quality
 *
 * Precondition (state) contract enforcement (internal)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_INTERNAL(expr, msg)          STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)

/** \def PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(expr, msg)
 *
 * \ingroup group__quality
 *
 * Precondition (state) contract enforcement (API)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(expr, msg)               STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)

/** \def PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_APPL_LAYER(expr, msg)
 *
 * \ingroup group__quality
 *
 * Precondition (state) contract enforcement (application layer)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_APPL_LAYER(expr, msg)        STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)


/** \def PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_INTERNAL(expr, msg)
 *
 * \ingroup group__quality
 *
 * Precondition (parameters) contract enforcement (internal)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_INTERNAL(expr, msg)         STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)

/** \def PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(expr, msg)
 *
 * \ingroup group__quality
 *
 * Precondition (parameters) contract enforcement (API)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(expr, msg)              STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)

/** \def PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_APPL_LAYER(expr, msg)
 *
 * \ingroup group__quality
 *
 * Precondition (parameters) contract enforcement (application layer)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_APPL_LAYER(expr, msg)       STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)


/** \def PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_INTERNAL(expr, msg)
 *
 * \ingroup group__quality
 *
 * Postcondition (return-value) contract enforcement (internal)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_INTERNAL(expr, msg)        STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)

/** \def PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_API(expr, msg)
 *
 * \ingroup group__quality
 *
 * Postcondition (return-value) contract enforcement (API)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_API(expr, msg)             STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)

/** \def PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_APPL_LAYER(expr, msg)
 *
 * \ingroup group__quality
 *
 * Postcondition (return-value) contract enforcement (application layer)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_APPL_LAYER(expr, msg)      STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)


/** \def PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_INTERNAL(expr, msg)
 *
 * \ingroup group__quality
 *
 * Postcondition (state) contract enforcement (internal)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_INTERNAL(expr, msg)         STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)

/** \def PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API(expr, msg)
 *
 * \ingroup group__quality
 *
 * Postcondition (state) contract enforcement (API)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API(expr, msg)              STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)

/** \def PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_APPL_LAYER(expr, msg)
 *
 * \ingroup group__quality
 *
 * Postcondition (state) contract enforcement (application layer)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_APPL_LAYER(expr, msg)       STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)


/** \def PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_INTERNAL(expr, msg)
 *
 * \ingroup group__quality
 *
 * Postcondition (parameters) contract enforcement (internal)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_INTERNAL(expr, msg)        STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)

/** \def PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_API(expr, msg)
 *
 * \ingroup group__quality
 *
 * Postcondition (parameters) contract enforcement (API)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_API(expr, msg)             STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)

/** \def PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_APPL_LAYER(expr, msg)
 *
 * \ingroup group__quality
 *
 * Postcondition (parameters) contract enforcement (application layer)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_PARAMS_APPL_LAYER(expr, msg)      STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)


/** \def PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_INTERNAL(expr, msg)
 *
 * \ingroup group__quality
 *
 * Invariant (class) contract enforcement (internal)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_INTERNAL(expr, msg)             STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)

/** \def PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_API(expr, msg)
 *
 * \ingroup group__quality
 *
 * Invariant (class) contract enforcement (API)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_API(expr, msg)                  STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)

/** \def PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_APPL_LAYER(expr, msg)
 *
 * \ingroup group__quality
 *
 * Invariant (class) contract enforcement (application layer)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_CLASS_INVARIANT_APPL_LAYER(expr, msg)           STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)


/** \def PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_INTERNAL(expr, msg)
 *
 * \ingroup group__quality
 *
 * Invariant (global) contract enforcement (internal)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_INTERNAL(expr, msg)            STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)

/** \def PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_API(expr, msg)
 *
 * \ingroup group__quality
 *
 * Invariant (global) contract enforcement (API)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_API(expr, msg)                 STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)

/** \def PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_APPL_LAYER(expr, msg)
 *
 * \ingroup group__quality
 *
 * Invariant (global) contract enforcement (application layer)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_GLOBAL_INVARIANT_APPL_LAYER(expr, msg)          STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)


/** \def PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_INTERNAL(expr, msg)
 *
 * \ingroup group__quality
 *
 * Static data state contract enforcement (internal)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_INTERNAL(expr, msg)                 STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)

/** \def PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_API(expr, msg)
 *
 * \ingroup group__quality
 *
 * Static data state contract enforcement (API)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_API(expr, msg)                      STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)

/** \def PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_APPL_LAYER(expr, msg)
 *
 * \ingroup group__quality
 *
 * Static data state contract enforcement (application layer)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_APPL_LAYER(expr, msg)               STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)

/** \def PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_APPL_DEF(expr, msg)
 *
 * \ingroup group__quality
 *
 * Static data state contract enforcement (application defined)
 *
 * \param expr The expression that must evaluate to \c true
 * \param msg The literal string describing the failed condition
 */
# define PANTHEIOS_CONTRACT_ENFORCE_STATIC_DATA_APPL_DEF(expr, msg)                 STLSOFT_MESSAGE_ASSERT(PANTHEIOS_TYPECHECK_MSG_(msg), expr)


/** \def PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(expr)
 *
 * \ingroup group__quality
 *
 * Intermediate assumption contract enforcement
 *
 * \param expr The expression that must evaluate to \c true
 */
# define PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(expr)                                STLSOFT_ASSERT(expr)


#endif /* PANTHEIOS_QA_USE_XCONTRACT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_QUALITY_H_CONTRACT */

/* ///////////////////////////// end of file //////////////////////////// */
