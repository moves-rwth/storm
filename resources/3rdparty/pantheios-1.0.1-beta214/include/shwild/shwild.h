/* /////////////////////////////////////////////////////////////////////////
 * File:    shwild/shwild.h
 *
 * Purpose: Root header file for the shwild library
 *
 * Created: 17th June 2005
 * Updated: 19th December 2011
 *
 * Home:    http://shwild.org/
 *
 * Copyright (c) 2005-2011, Matthew Wilson and Sean Kelly
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
 * - Neither the names of Matthew Wilson and Sean Kelly nor the names of
 *   any contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
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


/** \file shwild/shwild.h [C/C++] This is the root file of the shwild C-API
 */

#ifndef SHWILD_INCL_SHWILD_H_SHWILD
#define SHWILD_INCL_SHWILD_H_SHWILD

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef SHWILD_DOCUMENTATION_SKIP_SECTION
# define SHWILD_VER_SHWILD_H_SHWILD_MAJOR       1
# define SHWILD_VER_SHWILD_H_SHWILD_MINOR       2
# define SHWILD_VER_SHWILD_H_SHWILD_REVISION    20
# define SHWILD_VER_SHWILD_H_SHWILD_EDIT        30
#endif /* !SHWILD_DOCUMENTATION_SKIP_SECTION */

/** \def SHWILD_VER_MAJOR
 * The major version number of shwild
 */

/** \def SHWILD_VER_MINOR
 * The minor version number of shwild
 */

/** \def SHWILD_VER_REVISION
 * The revision version number of shwild
 */

/** \def SHWILD_VER
 * The current composite version number of shwild
 */

#ifndef SHWILD_DOCUMENTATION_SKIP_SECTION
# define SHWILD_VER_0_9_1       0x00090100
# define SHWILD_VER_0_9_2       0x00090200
# define SHWILD_VER_0_9_3       0x00090300
# define SHWILD_VER_0_9_4       0x00090400
# define SHWILD_VER_0_9_5       0x00090500
# define SHWILD_VER_0_9_6       0x00090600
# define SHWILD_VER_0_9_7       0x00090700
# define SHWILD_VER_0_9_8       0x00090800
# define SHWILD_VER_0_9_9       0x00090900
# define SHWILD_VER_0_9_10      0x00090a00
# define SHWILD_VER_0_9_11      0x00090b00
# define SHWILD_VER_0_9_12      0x00090cff
# define SHWILD_VER_0_9_13      0x00090dff
# define SHWILD_VER_0_9_14      0x00090eff
# define SHWILD_VER_0_9_15      0x00090fff
# define SHWILD_VER_0_9_16      0x000910ff
# define SHWILD_VER_0_9_17      0x000911ff
# define SHWILD_VER_0_9_18      0x000912ff
# define SHWILD_VER_0_9_19      0x000913ff
# define SHWILD_VER_0_9_20      0x000914ff
#endif /* !SHWILD_DOCUMENTATION_SKIP_SECTION */

#define SHWILD_VER_MAJOR        0
#define SHWILD_VER_MINOR        9
#define SHWILD_VER_REVISION     20

#define SHWILD_VER              SHWILD_VER_0_9_20

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#include <stddef.h>     /* for size_t */

/* /////////////////////////////////////////////////////////////////////////
 * Documentation
 */

/** \defgroup group__shwild_api shwild API
 * The shwild Public API
 *
 * The types, constants and functions that constitute the shwild API. Users of
 * the shwild library will use only these functions to effect pattern creation
 * and matching.
 */

/** \defgroup group__shwild_api__c_api C API
 * \ingroup group__shwild_api
 * These types, constants and functions form the core of the shwild API.
 */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

/** Handle type for use in declaring opaque handles to compiled patterns
 * \ingroup group__shwild_api__c_api
 */
struct shwild_handle_t_;

/** Handle to compiled pattern.
 * \ingroup group__shwild_api__c_api
 *
 * Used by shwild_compile_pattern(), shwild_match_pattern() and shwild_destroy_pattern()
 */
typedef struct shwild_handle_t_* shwild_handle_t;

/** Length-aware string.
 * \ingroup group__shwild_api__c_api
 *
 * This structure is used within the library to provide higher efficiency. In order
 * to maximise efficiency, there are equivalent versions of all API functions that
 * allow application code to specify string arguments as string slices, including
 * shwild_match_s(), shwild_compile_pattern_s() and shwild_match_pattern_s().
 */
struct shwild_slice_t
{
    size_t      len;    /*!< Number of characters in the slice. */
    char const* ptr;    /*!< Pointer to the first character in the slice. May be NULL if len == 0. */

#ifdef __cplusplus
public:
    /** Initialises members to default value */
    shwild_slice_t();
    /** Copies members from another slice instance */
    shwild_slice_t(shwild_slice_t const& rhs);
    /** Initialises members from the given parameters
     * 
     * \param n The number of characters in the string to be sliced
     * \param s Pointer to the first character in the string to be sliced. May be NULL only if n == 0.
     */
    shwild_slice_t(size_t n, char const* s);
#endif /* __cplusplus */
};
#ifndef SHWILD_DOCUMENTATION_SKIP_SECTION
typedef struct shwild_slice_t   shwild_slice_t;
#endif /* !SHWILD_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Constants and definitions
 */

/** \defgroup group__shwild_api__flags Pattern Control Flags
 * \ingroup group__shwild_api
 * These flags control the pattern matching behaviour:
 *
 * 1. Escape character. This is to recognise \ as an escape character, escaping
 *     any following character.
 *
 *     This is on by default, but may be suppressed by 
 *     SHWILD_F_SUPPRESS_BACKSLASH_ESCAPE.
 *
 * 2. Range support. This recognises [] as delimiting a range of possible 
 *      characters that may sustitute for a single character.
 *
 *     This is on by default, but may be suppressed by 
 *     SHWILD_F_SUPPRESS_RANGE_SUPPORT.
 *
 * 3. Range continuum support. This recognises a continuum of characters or
 *     numbers, e.g. [3-5m-q] === [345mnopq]
 *
 *     This is on by default, but may be suppressed by 
 *     SHWILD_F_SUPPRESS_RANGE_CONTINUUM_SUPPORT.
 * @{
 */

#define SHWILD_F_SUPPRESS_RANGE_SUPPORT                             (0x0001)    /*!< Suppresses the recognition of ranges. [ and ] are treated as literal characters (and need no escaping). */
#define SHWILD_F_SUPPRESS_BACKSLASH_ESCAPE                          (0x0002)    /*!< Suppresses the use of backslash interpretation as escape. \ is treated as a literal character. */
#define SHWILD_F_SUPPRESS_RANGE_CONTINUUM_SUPPORT                   (0x0004)    /*!< Suppresses the recognition of range continuums, i.e. [0-9]. */
#define SHWILD_F_SUPPRESS_RANGE_CONTINUUM_HIGHLOW_SUPPORT           (0x0008)    /*!< Suppresses the recognition of reverse range continuums, i.e. [9-0], [M-D]. */
#define SHWILD_F_SUPPRESS_RANGE_CONTINUUM_CROSSCASE_SUPPORT         (0x0010)    /*!< Suppresses the recognition of cross-case range continuums, i.e. [h-J] === [hijHIJ]. */
#define SHWILD_F_SUPPRESS_RANGE_LITERAL_WILDCARD_SUPPORT            (0x0020)    /*!< Suppresses the recognition of ? and * as literal inside range. */
#define SHWILD_F_SUPPRESS_RANGE_LEADTRAIL_LITERAL_HYPHEN_SUPPORT    (0x0040)    /*!< Suppresses the recognition of leading/trailing hyphens as literal inside range. */
#define SHWILD_F_SUPPRESS_RANGE_NOT_SUPPORT                         (0x0400)    /*!< Suppresses the use of a leading ^ to mean not any of the following, i.e. [^0-9] means do not match a digit. */
#define SHWILD_F_IGNORE_CASE                                        (0x0200)    /*!< Comparison is case-insensitive. */

#if 0
/* Not currently supported. */
#define SHWILD_F_ALLOW_RANGE_LITERAL_BRACKET_SUPPORT                (0x0080)    /*!< Treats [ and ] as literal inside range. ] only literal if immediately preceeds closing ]. NOT CURRENTLY SUPPORTED. */
#define SHWILD_F_ALLOW_RANGE_QUANTIFICATION_SUPPORT                 (0x0100)    /*!< Allows quantification of the wildcards, with trailing escaped numbers, as in [a-Z]\2-10. All chars in 0-9- become range specifiers. These are separated from actual pattern digits by []. NOT CURRENTLY SUPPORTED. */
#define SHWILD_F_FNM_PATHNAME_SEMANTICS                             (0x0800)    /*!< Will only match / (and \ on Win32) characters with literals, not via any wildcard substitutions. */
#endif /* 0 */

/** @} */

/** \defgroup group__shwild_api__result_codes shwild Results Codes
 * \ingroup group__shwild_api
 * These codes represent the 
 *
 * @{
 */

#define SHWILD_RC_SUCCESS               (0)                             /*!< Operating completed successfully */
#define SHWILD_RC_ERROR_BASE_           (-2000)
#define SHWILD_RC_ALLOC_ERROR           (SHWILD_RC_ERROR_BASE_ - 1)     /*!< Memory exhaustion. */
#define SHWILD_RC_PARSE_ERROR           (SHWILD_RC_ERROR_BASE_ - 2)     /*!< Parsing error. */
#define SHWILD_RC_UNSPECIFIED           (SHWILD_RC_ERROR_BASE_ - 3)     /*!< Unspecified exception. */

/** @} */

/* /////////////////////////////////////////////////////////////////////////
 * C API
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** Initialises the shwild API
 *
 * \return Status indicating whether initialisation was successful
 * \retval <0 An error
 * \retval >=0 Initialisation was successful
 */
int shwild_init(void);

/** Uninitialises the shwild API
 *
 * \remarks Must be called once for each successful call to shwild_init().
 */
void shwild_uninit(void);

/** Matches a string against an shwild pattern
 * \ingroup group__shwild_api__c_api
 *
 * \param pattern The shwild pattern against which matching will be performed
 * \param string The string to match against the pattern
 * \param flags Flags which moderate the search.
 *
 * \return Status indicating whether the string matched against the given pattern
 * \retval <0 An error code (one of SHWILD_RC_*)
 * \retval 0 The string matched the pattern
 * \retval non-0 The string did not match the pattern
 */
int shwild_match(char const* pattern, char const* string, unsigned flags);

/** Synonym for shwild_match() using length-aware string arguments
 * \ingroup group__shwild_api__c_api
 */
int shwild_match_s(shwild_slice_t const* pattern, shwild_slice_t const* string, unsigned flags);

/** Compiles a pattern into an efficient form for use in multiple match operations
 * \ingroup group__shwild_api__c_api
 *
 * \param pattern The shwild pattern against which matching will be performed
 * \param flags Flags which moderate the search.
 * \param phCompiledPattern Pointer to a variable to hold the compiled pattern
 *
 * \return Status indicating whether the operating completed successfully
 * \retval <0 The operation failed
 * \retval >=0 The operation succeeded. The value indicates the number of match 
 *   sub-components created to represent the pattern. The compiled pattern must
 *   be destroyed when it is no longer needed, by shwild_destroy_pattern(), to 
 *   avoid memory leaks.
 */
int shwild_compile_pattern(char const* pattern, unsigned flags, shwild_handle_t* phCompiledPattern);

/** Synonym for shwild_compile_pattern() using length-aware string arguments
 * \ingroup group__shwild_api__c_api
 */
int shwild_compile_pattern_s(shwild_slice_t const* pattern, unsigned flags, shwild_handle_t* phCompiledPattern);

/** Matches a string against against a pre-compiled shwild pattern
 * \ingroup group__shwild_api__c_api
 *
 * \param hCompiledPattern The precompiled shwild pattern against which matching will be performed
 * \param string The string to match against the pattern
 *
 * \return Status indicating whether the string matched against the given pattern
 * \retval 0 The string did not match the pattern
 * \retval non-0 The string matched against the pattern
 */
int shwild_match_pattern(shwild_handle_t hCompiledPattern, char const* string);

/** Synonym for shwild_match_pattern() using length-aware string arguments
 * \ingroup group__shwild_api__c_api
 */
int shwild_match_pattern_s(shwild_handle_t hCompiledPattern, shwild_slice_t const* string);

/** Releases all resources associated with a pre-compiled shwild pattern
 * \ingroup group__shwild_api__c_api
 *
 * \param hCompiledPattern The pattern to be destroyed.
 */
void shwild_destroy_pattern(shwild_handle_t hCompiledPattern);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(__cplusplus) && \
    !defined(SHWILD_DOCUMENTATION_SKIP_SECTION) && \
    !defined(SHWILD_NO_NAMESPACE)
# define SHWILD_NO_NAMESPACE
#endif /* __cplusplus, etc. */

#if !defined(SHWILD_NO_NAMESPACE)
/** The shwild/C++ namespace - \c shwild - that contains wrappers for the 
 * \link group__shwild_api__c_api C API\endlink. */
namespace shwild
{
#endif /* !SHWILD_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * C++ functions
 */

#ifdef __cplusplus

/** \defgroup group__shwild_api__cpp_api C++ API
 * \ingroup group__shwild_api
 * The C++ API provides convenient overloads of the 
 *   \ref group__shwild_api__c_api functions for use in C++, along with the
 *   Pattern class that provides a convenient interface to compiled patterns.
 * @{
 */

/** C++ synonym for shwild_slice_t
 * \ingroup group__shwild_api__cpp_api
 */
typedef shwild_slice_t  slice_t;

/** C++ overload synonym for shwild_match()
 * \ingroup group__shwild_api__cpp_api
 */
inline int match(char const* pattern, char const* string, unsigned flags = 0)
{
    return shwild_match(pattern, string, flags);
}
/** C++ overload synonym for shwild_compile_pattern()
 * \ingroup group__shwild_api__cpp_api
 */
inline int compile_pattern(char const* pattern, unsigned flags, shwild_handle_t* phCompiledPattern)
{
    return shwild_compile_pattern(pattern, flags, phCompiledPattern);
}
/** C++ overload synonym for shwild_match_pattern()
 * \ingroup group__shwild_api__cpp_api
 */
inline int match_pattern(shwild_handle_t hCompiledPattern, char const* string)
{
    return shwild_match_pattern(hCompiledPattern, string);
}
/** C++ overload synonym for shwild_destroy_pattern()
 * \ingroup group__shwild_api__cpp_api
 */
inline void destroy_pattern(shwild_handle_t hCompiledPattern)
{
    shwild_destroy_pattern(hCompiledPattern);
}

/** @} */

#endif /* __cplusplus */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(SHWILD_NO_NAMESPACE)
} /* namespace shwild */
#endif /* !SHWILD_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Implementation
 */

#ifndef SHWILD_DOCUMENTATION_SKIP_SECTION

# ifdef __cplusplus

#  if defined(__BORLANDC__) && \
      __BORLANDC__ >= 0x0582
#   pragma warn -8026
#  endif /* compiler */


inline shwild_slice_t::shwild_slice_t()
    : len(0)
    , ptr(NULL)
{}
inline shwild_slice_t::shwild_slice_t(shwild_slice_t const& rhs)
    : len(rhs.len)
    , ptr(rhs.ptr)
{}
inline shwild_slice_t::shwild_slice_t(size_t n, char const* s)
    : len(n)
    , ptr(s)
{}

#  if defined(__BORLANDC__) && \
      __BORLANDC__ >= 0x0582
#   pragma warn .8026
#  endif /* compiler */

# endif /* __cplusplus */

#endif /* !SHWILD_DOCUMENTATION_SKIP_SECTION */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !SHWILD_INCL_SHWILD_H_SHWILD */

/* ///////////////////////////// end of file //////////////////////////// */
