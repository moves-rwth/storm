/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/pantheios.h
 *
 * Purpose:     Pantheios Core and Util APIs.
 *
 * Created:     21st June 2005
 * Updated:     6th August 2012
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2012, Matthew Wilson and Synesis Software
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


/** \file pantheios/pantheios.h
 *
 * [C, C++] Primary include file for the \ref group__core_library and
 *  \ref group__util_library.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
#define PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_H_PANTHEIOS_MAJOR      3
# define PANTHEIOS_VER_PANTHEIOS_H_PANTHEIOS_MINOR      46
# define PANTHEIOS_VER_PANTHEIOS_H_PANTHEIOS_REVISION   1
# define PANTHEIOS_VER_PANTHEIOS_H_PANTHEIOS_EDIT       350
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/** \def PANTHEIOS_VER_MAJOR
 * The major version number of Pantheios
 */

/** \def PANTHEIOS_VER_MINOR
 * The minor version number of Pantheios
 */

/** \def PANTHEIOS_VER_REVISION
 * The revision version number of Pantheios
 */

/** \def PANTHEIOS_VER
 * The current composite version number of Pantheios
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_1_0_1_B2                 0x01000102
# define PANTHEIOS_VER_1_0_1_B193               0x010001c1
# define PANTHEIOS_VER_1_0_1_B194               0x010001c2
# define PANTHEIOS_VER_1_0_1_B195               0x010001c3
# define PANTHEIOS_VER_1_0_1_B196               0x010001c4
# define PANTHEIOS_VER_1_0_1_B197               0x010001c5
# define PANTHEIOS_VER_1_0_1_B198               0x010001c6
# define PANTHEIOS_VER_1_0_1_B199               0x010001c7
# define PANTHEIOS_VER_1_0_1_B200               0x010001c8
# define PANTHEIOS_VER_1_0_1_B201               0x010001c9
# define PANTHEIOS_VER_1_0_1_B202               0x010001ca
# define PANTHEIOS_VER_1_0_1_B203               0x010001cb
# define PANTHEIOS_VER_1_0_1_B204               0x010001cc
# define PANTHEIOS_VER_1_0_1_B205               0x010001cd
# define PANTHEIOS_VER_1_0_1_B206               0x010001ce
# define PANTHEIOS_VER_1_0_1_B207               0x010001cf
# define PANTHEIOS_VER_1_0_1_B208               0x010001d0
# define PANTHEIOS_VER_1_0_1_B209               0x010001d1
# define PANTHEIOS_VER_1_0_1_B210               0x010001d2
# define PANTHEIOS_VER_1_0_1_B211               0x010001d3
# define PANTHEIOS_VER_1_0_1_B212               0x010001d4
# define PANTHEIOS_VER_1_0_1_B213               0x010001d5
# define PANTHEIOS_VER_1_0_1_B214               0x010001d6
# define PANTHEIOS_VER_1_0_1                    0x010001ff
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

#define PANTHEIOS_VER_MAJOR                     1
#define PANTHEIOS_VER_MINOR                     0
#define PANTHEIOS_VER_REVISION                  1

#define PANTHEIOS_VER                           PANTHEIOS_VER_1_0_1_B214

/* /////////////////////////////////////////////////////////////////////////
 * Includes - 1
 */

/* Pantheios core API relies on STLSoft for compiler/library discrimination,
 * and various fundamental types.
 */
#ifndef STLSOFT_INCL_STLSOFT_H_STLSOFT
# include <stlsoft/stlsoft.h>
#endif /* !STLSOFT_INCL_STLSOFT_H_STLSOFT */

#if !defined(_STLSOFT_VER) || \
    _STLSOFT_VER < 0x010973ff
# error This version Pantheios requires STLSoft 1.9.115, or later. (www.stlsoft.org)
#endif /* STLSoft version */

#ifndef PANTHEIOS_INCL_H_STDARG
# define PANTHEIOS_INCL_H_STDARG
# include <stdarg.h>     /* for va_list */
#endif /* !PANTHEIOS_INCL_H_STDARG */
#ifndef PANTHEIOS_INCL_H_STDDEF
# define PANTHEIOS_INCL_H_STDDEF
# include <stddef.h>     /* for size_t */
#endif /* !PANTHEIOS_INCL_H_STDDEF */

/* /////////////////////////////////////////////////////////////////////////
 * Feature detection
 */

/* By default, Pantheios is a multi-byte string library
 *
 * If you define \c PANTHEIOS_USE_WIDE_STRINGS, then it will be built in
 * wide string form.
 *
 * Also, if you are compiling for Windows (32-bit or 64-bit) and define
 * either \c _UNICODE or \c UNICODE, then \c PANTHEIOS_USE_WIDE_STRINGS will
 * be defined automatically.
 *
 * In either case, if you define \c PANTHEIOS_NO_USE_WIDE_STRINGS, then the
 * wide string support will be suppressed.
 */

#ifndef PANTHEIOS_USE_WIDE_STRINGS
# if defined(_WIN32) && \
     defined(UNICODE) && \
     defined(_UNICODE)
#  define PANTHEIOS_USE_WIDE_STRINGS
# endif /* _WIN32 && UNICODE && _UNICODE */
#endif /* !PANTHEIOS_USE_WIDE_STRINGS */

#ifdef PANTHEIOS_NO_USE_WIDE_STRINGS
# ifdef PANTHEIOS_USE_WIDE_STRINGS
#  undef PANTHEIOS_USE_WIDE_STRINGS
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
#endif /* PANTHEIOS_NO_USE_WIDE_STRINGS */

/* /////////////////////////////////////////////////////////////////////////
 * Includes - 2
 */

#ifdef PANTHEIOS_USE_WIDE_STRINGS
# ifndef _WCHAR_T_DEFINED
#  include <wchar.h>    /* for wchar_t (if not built-in) */
# endif /* !_WCHAR_T_DEFINED */
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* TODO: make this include relative */
#include <pantheios/internal/string_encoding.h>

/* /////////////////////////////////////////////////////////////////////////
 * Macros and definitions
 */

/** \def PANTHEIOS_STRINGIZE(x)
 *
 * Macro for stringizing symbols during preprocessing phase
 *
 * \ingroup group__utility
 *
 * \param x The preprocessor symbol whose value will be turned into a string
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_STRINGIZE_(x)    #x
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
#define PANTHEIOS_STRINGIZE(x)      PANTHEIOS_STRINGIZE_(x)

/** \def PANTHEIOS_MAKE_EXTENDED_SEVERITY(sev, xi)
 *
 * Defines a composite severity level comprising
 * a \link pantheios::pan_severity_t stock severity level\endlink and
 * 28-bits of extended information
 *
 * \param sev A \link pantheios::pan_severity_t stock severity level\endlink
 * \param xi28 The extended severity information. Must be no larger than
 *   28-bits
 *
 * \pre 0 == (xi28 & ~0xf0000000)
 */
#define PANTHEIOS_MAKE_EXTENDED_SEVERITY(sev, xi28)     \
                                                        \
    (((sev) & 0x0f) | (((xi28) << 4) & ~0x0f))

/** \def PANTHEIOS_CARG_STR(s)
 *
 * \param s A (nul-terminated) C-style string pointer/array
 *
 * \note The length of this macro is necessary to disambiguate with respect to
 *   arbitrary codebases. For convenience, it is recommended that you 
 *   <code>#define</code> a shorter macro for use in your code, as in:
\htmlonly 
<pre>
\#define PANARG_S(s)        PANTHEIOS_CARG_STR(s)
</pre>
\endhtmlonly
 */

#define PANTHEIOS_CARG_STR(s)                               (s), (-1)

/** \def PANTHEIOS_CARG_STR_LEN(s, n)
 *
 * \param s A C-style string pointer/array, whose length is \c n
 * \param n The length of the string pointed to by the \c s parameter
 *
 * \note The length of this macro is necessary to disambiguate with respect to
 *   arbitrary codebases. For convenience, it is recommended that you 
 *   <code>#define</code> a shorter macro for use in your code, as in:
\htmlonly 
<pre>
\#define PANARG_SN(s, n)    PANTHEIOS_CARG_STR_LEN(s, n)
</pre>
\endhtmlonly
 */

#define PANTHEIOS_CARG_STR_LEN(s, n)                        (s), (n)

/** \def PANTHEIOS_DECLSPEC
 *
 * Storage class specifier to be applied to all Pantheios API functions
 *
 * \ingroup group__utility
 *
 * Defaults to no symbol. Can be defined to a symbol appropriate to the
 * operating system and linkage model. e.g. on Windows you might choose to
 * define it to <code>__declspec(dllexport)</code> when building Pantheios
 * and to <code>__declspec(dllimport)</code> in client code using Pantheios.
 *
 * Used in the PANTHEIOS_CALL() macro.
 */

/** \def PANTHEIOS_EXTERN_C
 *
 * Resolves to <code>extern</code> when compiling in C, and
 * <code>extern "C"</code> when compiling in C++.
 *
 * \ingroup group__utility
 *
 * Used in the PANTHEIOS_CALL() macro.
 */

/** \def PANTHEIOS_CALLCONV
 *
 * Calling convention specifier to be applied to all Pantheios API functions
 *
 * Used in the PANTHEIOS_CALL() macro.
 *
 * \ingroup group__utility
 */

/** \def PANTHEIOS_CALL(rt)
 *
 * Used to declare and define Pantheios API functions.
 *
 * \ingroup group__utility
 */

/** \def PANTHEIOS_DECLARE_DEPRECATION(symtype, oldfn, newfn)
 *
 * Used to mark an inline (C++) function as deprecated
 *
 * \ingroup group__utility
 */

/** \def PANTHEIOS_CALL_DEPRECATED(rt, oldfn, newfn)
 *
 * Used to declare an API function as deprecated
 *
 * \see PANTHEIOS_CALL
 * \see PANTHEIOS_DECLARE_DEPRECATION
 *
 * \ingroup group__utility
 */

#if !defined(PANTHEIOS_DECLSPEC)
# define PANTHEIOS_DECLSPEC
#endif /* !PANTHEIOS_DECLSPEC */

#if defined(__cplusplus) || \
    defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION)
# define PANTHEIOS_EXTERN_C            extern "C"
#else /* ? __cplusplus */
# define PANTHEIOS_EXTERN_C            extern
#endif /* !__cplusplus */

#if !defined(PANTHEIOS_CALLCONV)
# define PANTHEIOS_CALLCONV
#endif /* !PANTHEIOS_CALLCONV */

#define PANTHEIOS_CALL(rt)             PANTHEIOS_DECLSPEC PANTHEIOS_EXTERN_C rt PANTHEIOS_CALLCONV

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_CPP_CALL(rt)        PANTHEIOS_DECLSPEC extern "C++" rt PANTHEIOS_CALLCONV
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

#ifdef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_DECLARE_DEPRECATION(symtype, oldfn, newfn)
# define PANTHEIOS_CALL_DEPRECATED(rt, oldfn, newfn)            PANTHEIOS_CALL(rt)
#else /* ? PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
# if defined(STLSOFT_COMPILER_IS_MSVC) && \
    (   _MSC_VER >= 1500 || \
        (   _MSC_VER >= 1400 && \
            defined(_MSC_FULL_VER) && \
            _MSC_FULL_VER >= 140050320))
#  define PANTHEIOS_DECLARE_DEPRECATION(symtype, oldfn, newfn)  __declspec(deprecated("The " symtype " " PANTHEIOS_STRINGIZE(oldfn) " is deprecated and will be removed from a future version of Pantheios; use " PANTHEIOS_STRINGIZE(newfn) " instead"))
#  define PANTHEIOS_CALL_DEPRECATED(rt, oldfn, newfn)           PANTHEIOS_DECLARE_DEPRECATION("function", oldfn, newfn) PANTHEIOS_DECLSPEC PANTHEIOS_EXTERN_C rt PANTHEIOS_CALLCONV
# else /* ? compiler */
#  define PANTHEIOS_DECLARE_DEPRECATION(symtype, oldfn, newfn)
#  define PANTHEIOS_CALL_DEPRECATED(rt, oldfn, newfn)           PANTHEIOS_CALL(rt)
# endif /* compiler */
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if defined(STLSOFT_CF_NAMESPACE_SUPPORT)
# define PANTHEIOS_CF_NAMESPACE_SUPPORT
#else /* STLSOFT_CF_NAMESPACE_SUPPORT */
# ifdef PANTHEIOS_CF_NAMESPACE_SUPPORT
#  undef PANTHEIOS_CF_NAMESPACE_SUPPORT
# endif /* PANTHEIOS_CF_NAMESPACE_SUPPORT */
#endif /* STLSOFT_CF_NAMESPACE_SUPPORT */

#if !defined(PANTHEIOS_CF_NAMESPACE_SUPPORT) && \
    !defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION) && \
    !defined(PANTHEIOS_NO_NAMESPACE)
# define PANTHEIOS_NO_NAMESPACE
#endif /* __cplusplus, etc. */

#if !defined(PANTHEIOS_NO_NAMESPACE)
/** The Pantheios root namespace - \c pantheios.
 *
 * This the the root namespace for Pantheios, and contains the C-API functions, along
 * with appropriate type-defs
 */
namespace pantheios
{

/** The namespace within which utility functions are located
 *
 * These functions are not ordinarily called directly by Pantheios users
 */
namespace util
{
} /* namespace util */

/** The namespace within which functions related to the core are located
 *
 * These functions are never called directly by Pantheios users
 */
namespace core
{
} /* namespace core */

# define PANTHEIOS_NS_QUAL(x)           ::pantheios::x
# define PANTHEIOS_SUBNS_QUAL(sns, x)   ::pantheios::sns::x

#else /* ? !PANTHEIOS_NO_NAMESPACE */

# define PANTHEIOS_NS_QUAL(x)           x
# define PANTHEIOS_SUBNS_QUAL(sns, x)   x

#endif /* !PANTHEIOS_NO_NAMESPACE */

/** \def PANTHEIOS_NS_QUAL(x)
 *
 * Puts \c x in the \c pantheios namespace if namespace is supported (and
 * not suppressed), otherwise \c x is unqualified
 */

/** \def PANTHEIOS_SUBNS_QUAL(sns, x)
 *
 * Puts \c x in the \c pantheios::sns namespace if namespace is supported
 * (and not suppressed), otherwise \c x is unqualified
 */

/* /////////////////////////////////////////////////////////////////////////
 * Types
 */

/* Basic types */

/** 8-bit unsigned integer type. */
typedef stlsoft_ns_qual(ss_uint8_t)     pan_uint8_t;
/** 16-bit unsigned integer type. */
typedef stlsoft_ns_qual(ss_uint16_t)    pan_uint16_t;
/** 32-bit unsigned integer type. */
typedef stlsoft_ns_qual(ss_uint32_t)    pan_uint32_t;
/** 64-bit unsigned integer type. */
typedef stlsoft_ns_qual(ss_uint64_t)    pan_uint64_t;

/** 8-bit signed integer type. */
typedef stlsoft_ns_qual(ss_sint8_t)     pan_sint8_t;
/** 16-bit signed integer type. */
typedef stlsoft_ns_qual(ss_sint16_t)    pan_sint16_t;
/** 32-bit signed integer type. */
typedef stlsoft_ns_qual(ss_sint32_t)    pan_sint32_t;
/** 64-bit signed integer type. */
typedef stlsoft_ns_qual(ss_sint64_t)    pan_sint64_t;


/** Character type used by the library
 *
 * This will be <code>wchar_t</code> if
 * <code>PANTHEIOS_USE_WIDE_STRINGS</code> is defined, otherwise
 * <code>char</code>.
 */
#ifdef PANTHEIOS_USE_WIDE_STRINGS
typedef wchar_t     pan_char_t;
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
typedef char        pan_char_t;
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/** \def PAN_CHAR_T
 *
 * The type \c pan_char_t, suitably namespace qualified in the contexts in
 * which the \c pantheios namespace is operative. Acts as a convenient
 * shorthand for <code>::pantheios::pan_char_t</code> when in C++ (and
 * namespace not suppressed) or <code>pan_char_t</code> otherwise.
 */
#define PAN_CHAR_T  PANTHEIOS_NS_QUAL(pan_char_t)

/** String slice used by the \ref group__application_layer_interface
 *    to communicate with the \ref group__core_library.
 *
 * \ingroup group__core_library
 *
 * This structure represents a character string as a slice, in the form of a
 * pointer to the first character, and a count of the number of characters in
 * the slice.
 *
 * \note The slice need not be nul-terminated. Hence, slices may overlap.
 */
struct pan_slice_t
{
    size_t              len;    /*!<  Number of characters in the c-style string represented by the slice. */
    pan_char_t const*   ptr;    /*!<  Pointer to the first character in the c-style string represented by the slice. */

#ifdef __cplusplus
    /** Constructs a null/empty slice. */
    pan_slice_t();
    /** Constructs a slice from the given pointer and length. */
    pan_slice_t(pan_char_t const* p, size_t l);
    /** Copy constructor. */
    pan_slice_t(pan_slice_t const& rhs);
    /** Copy assignment operator. */
    pan_slice_t& operator =(pan_slice_t const& rhs);

# ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
    /** "private" constructor for supporting C-compatible log functions. */
    pan_slice_t(int len, pan_char_t const* p);
# endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
    /**
     *
     * \pre fromLen <= 32767
     * \pre toLen <= 32767
     */
    static size_t   get_lazy_length(size_t fromLen, size_t toLen);
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

private:
    static size_t   calc_length_n_(pan_char_t const* s, size_t len);

#endif /* __cplusplus */
};
#if !defined(__cplusplus)
typedef struct pan_slice_t  pan_slice_t;
#endif /* !__cplusplus */


#ifndef PANTHEIOS_NO_STOCK_LEVELS
/** API Severity level
 *
 * \ingroup group__core_library
 *
 * \remarks the enumerator values correspond to those of the SysLog protocol
 *
 * \note This type is not used throughout the Pantheios architecture, serving merely
 *   to define the eight stock severity levels. Instead, the
 *   \link pantheios::pan_sev_t pan_sev_t\endlink type (a 32-bit signed integer)
 *   is used instead. This is because Pantheios supports any severity level
 *   (that can be expressed in 32-bits). The definition of these SysLog-inspired
 *   values is simply what the
 *   \link group__backend__stock_backends stock back ends\endlink
 *   are implemented to recognise.
 */
enum pan_severity_t
{
        SEV_EMERGENCY       =   0   /*!< system is unusable */
    ,   SEV_ALERT           =   1   /*!< action must be taken immediately */
    ,   SEV_CRITICAL        =   2   /*!< critical conditions */
    ,   SEV_ERROR           =   3   /*!< error conditions */
    ,   SEV_WARNING         =   4   /*!< warning conditions */
    ,   SEV_NOTICE          =   5   /*!< normal but significant condition */
    ,   SEV_INFORMATIONAL   =   6   /*!< informational */
    ,   SEV_DEBUG           =   7   /*!< debug-level messages */

# if defined(PANTHEIOS_NO_NAMESPACE)
    ,   PANTHEIOS_SEV_EMERGENCY         =   SEV_EMERGENCY
    ,   PANTHEIOS_SEV_ALERT             =   SEV_ALERT
    ,   PANTHEIOS_SEV_CRITICAL          =   SEV_CRITICAL
    ,   PANTHEIOS_SEV_ERROR             =   SEV_ERROR
    ,   PANTHEIOS_SEV_WARNING           =   SEV_WARNING
    ,   PANTHEIOS_SEV_NOTICE            =   SEV_NOTICE
    ,   PANTHEIOS_SEV_INFORMATIONAL     =   SEV_INFORMATIONAL
    ,   PANTHEIOS_SEV_DEBUG             =   SEV_DEBUG
# endif /* PANTHEIOS_NO_NAMESPACE */

};
# if !defined(__cplusplus)
typedef enum pan_severity_t pan_severity_t;
# endif /* !__cplusplus */
#endif /* !PANTHEIOS_NO_STOCK_LEVELS */

/** Severity level type
 *
 * \remarks This is not of type \link pantheios::pan_severity_t pan_severity_t\endlink,
 *   because the Pantheios API can accept any severity level that can be
 *   expressed within 32 bits.
 */
typedef stlsoft_ns_qual(ss_sint32_t)    pan_sev_t;

#if defined(__cplusplus) && \
    !defined(PANTHEIOS_NO_STOCK_LEVELS) && \
    !defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION)
# define PANTHEIOS_INCLUDING_STOCKLEVELS
# include "./internal/stock_levels.hpp"    /* Stock/custom level type */
# undef PANTHEIOS_INCLUDING_STOCKLEVELS

#endif /* __cplusplus */

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/** Returns information about the version of Pantheios
 *
 * \ingroup group__core_library
 *
 * The value returned is split into 4 x 8 bits, as follows:
 *  - bits 24-31: the major version (PANTHEIOS_VER_MAJOR)
 *  - bits 16-23: the minor version (PANTHEIOS_VER_MINOR)
 *  - bits 8-15: the revision number (PANTHEIOS_VER_REVISION)
 *  - bits 0-7: the alpha/beta designatorm, or 0xFF for a non-beta release
 *
 * \return The value of PANTHEIOS_VER at the time that Pantheios was
 *   built
 *
 * \remarks This function may be called without having initialised the
 *   Pantheios library
 */
PANTHEIOS_CALL(pan_uint32_t) pantheios_getVersion(void);

/** Initialises the Pantheios library
 *
 * \ingroup group__core_library
 *
 * \return Indicates
 * \retval <0 initialisation has failed, and the library cannot be used
 * \retval >=0 initialisation has succeeded, and the library can be used.
 *   pantheios_uninit() should be called when the library is no longer needed
 *
 * \note C++ compilation units that include pantheios/pantheios.hpp do not
 *   need to explicitly call pantheios_init() / pantheios_uninit(), since
 *   they will be automatically called by the pantheios_initialiser Schwarz
 *   counter class defined by the C++ inclusion. Further note that this is
 *   disabled by the definition of the <code>PANTHEIOS_NO_AUTO_INIT</code>,
 *   which is  automatically defined by a Windows DLL build (as detected by
 *   the presence of any of the <code>__DLL__</code>, <code>_WINDLL</code>
 *   or <code>_USRDLL</code> symbols).
 *   Auto-initialisation can be forced regardless of the definition of
 *   <code>PANTHEIOS_NO_AUTO_INIT</code> by the definition of the symbol
 *   <code>PANTHEIOS_FORCE_AUTO_INIT</code>.
 */
PANTHEIOS_CALL(int) pantheios_init(void);

/** Uninitialises the Pantheios library
 *
 * \ingroup group__core_library
 *
 * Should be called for every call to pantheios_init() that returns a
 * non-negative code
 *
 * \note C++ compilation units that include pantheios/pantheios.hpp do not
 *   need to explicitly call pantheios_init() / pantheios_uninit(), since
 *   they will be automatically called by the pantheios_initialiser Schwarz
 *   counter class defined by the C++ inclusion. Further note that this is
 *   disabled by the definition of the <code>PANTHEIOS_NO_AUTO_INIT</code>,
 *   which is  automatically defined by a Windows DLL build (as detected by
 *   the presence of any of the <code>__DLL__</code>, <code>_WINDLL</code>
 *   or <code>_USRDLL</code> symbols).
 *   Auto-initialisation can be forced regardless of the definition of
 *   <code>PANTHEIOS_NO_AUTO_INIT</code> by the definition of the symbol
 *   <code>PANTHEIOS_FORCE_AUTO_INIT</code>.
 */
PANTHEIOS_CALL(void) pantheios_uninit(void);

/** Indicates whether a given severity is currently being logged by the
 *    process.
 *
 * \ingroup group__core_library
 *
 * \param severity The severity level to test. Usually one of the \link pantheios::pan_severity_t PANTHEIOS_SEV_*\endlink enumerators.
 *
 * \retval 0 The given severity level is not being logged.
 * \retval 1 The given severity level is being logged.
 *
 * This function is used by \ref group__application_layer_interface "Application Layer API"
 * for filtering statements prior to formatting their elements for output. It
 * is also useful in those rare circumstances where you may need to undertake
 * a significant amount of client-side preparation of arguments to be passed
 * to the diagnostic logging statements.
 */
PANTHEIOS_CALL(int) pantheios_isSeverityLogged(pan_sev_t severity);

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
/** [Currently undocumented] Gets the process identity.
 *
 * \note THIS FUNCTION IS NOT PART OF THE PUBLICLY DOCUMENTED API OF
 *   PANTHEIOS, AND IS SUBJECT TO REMOVAL/CHANGE IN A FUTURE RELEASE.
 *
 * \warning Calling this function before the pantheios_init() (or after
 *   pantheios_uninit()) results in undefined behaviour. Don't do it! If you
 *   need the process identity at a time that is potentially outside the
 *   lifetime of the Pantheios core, then use a front-end that itself uses
 *   PANTHEIOS_FE_PROCESS_IDENTITY, and use that.
 */
PANTHEIOS_CALL(pan_char_t const*) pantheios_getProcessIdentity(void);
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/** Returns a constant pointer to a non-NULL non-modifiable nul-terminated string
 * representing the severity level.
 *
 * \ingroup group__util_library
 *
 * \param severity The severity level whose string equivalent is to be
 *   returned. Must be one of the
 *   \link pantheios::pan_severity_t PANTHEIOS_SEV_*\endlink enumerators,
 *   otherwise, the empty string (<code>""</code>) will be returned.
 */
PANTHEIOS_CALL(pan_char_t const*) pantheios_getStockSeverityString(pan_sev_t severity);
#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
PANTHEIOS_CALL_DEPRECATED(pan_char_t const*, pantheios_getSeverityString, pantheios_getStockSeverityString) pantheios_getSeverityString(pan_sev_t severity);
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/** Returns the length of the string returned by pantheios_getSeverityString().
 *
 * \ingroup group__util_library
 *
 * \param severity The severity level whose string equivalent is to be
 *   returned. Must be one of the
 *   \link pantheios::pan_severity_t PANTHEIOS_SEV_*\endlink enumerators,
 *   otherwise, 0 will be returned.
 */
PANTHEIOS_CALL(size_t) pantheios_getStockSeverityStringLength(pan_sev_t severity);
#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
PANTHEIOS_CALL_DEPRECATED(size_t, pantheios_getSeverityStringLength, pantheios_getStockSeverityStringLength) pantheios_getSeverityStringLength(pan_sev_t severity);
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/** Returns a constant pointer to a non-NULL non-modifiable nul-terminated
 * string representing the initialisation code.
 *
 * \ingroup group__util_library
 *
 * \param code The initialisation code whose string explanation is to be
 *   returned. If the error code is not recognised,
 *   the empty string (<code>""</code>) will be returned.
 */
PANTHEIOS_CALL(char const*) pantheios_getInitCodeString(int code);
#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
PANTHEIOS_CALL_DEPRECATED(char const*, pantheios_getInitErrorString, pantheios_getInitCodeString) pantheios_getInitErrorString(int code);
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/** Returns the length of the string returned by pantheios_getInitCodeString().
 *
 * \ingroup group__util_library
 *
 * \param code The initialisation code whose string equivalent is to be
 *   returned. If the error code is not recognised, 0 will be returned.
 */
PANTHEIOS_CALL(size_t) pantheios_getInitCodeStringLength(int code);
#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
PANTHEIOS_CALL_DEPRECATED(size_t, pantheios_getInitErrorStringLength, pantheios_getInitCodeStringLength) pantheios_getInitErrorStringLength(int code);
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace core
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/** Core logging function, which receives a severity and an array of string
 * slices, and outputs them to the back-end
 *
 * \ingroup group__core_library
 *
 * \param severity The severity of the log entry
 * \param slices Pointer to the array of slices
 * \param numSlices The number of slices
 *
 * \return An indicator of success
 * \retval <0 An error occurred
 * \retval >=0 The log entry was successfully passed to the back-end
 */
PANTHEIOS_CALL(int) pantheios_log_n(
    pan_sev_t                       severity
,   size_t                          numSlices
#ifdef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
,   pantheios::pan_slice_t const*   slices
#else /* ? PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
,   pan_slice_t const*              slices
#endif /* PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
);

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_INCLUDING_C_API_FUNCTIONS
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
#include "./internal/generated/log_functions.h"    /* pantheios_log_1() (pantheios::log_1()), etc.  */
#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# undef PANTHEIOS_INCLUDING_C_API_FUNCTIONS
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace core */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/** printf()-form of logging function, passing the formatted result to
 * the back-end
 *
 * \ingroup group__core_library
 *
 * \param severity The severity of the log entry
 * \param format The format string
 *
 * \warning This function is not type-safe. C++ application code should
 *   prefer the \ref group__application_layer_interface "application layer"
 *   functions.
 */
PANTHEIOS_CALL(int) pantheios_logprintf(
    pan_sev_t           severity
,   pan_char_t const*   format
,   ...
)
#if defined(STLSOFT_COMPILER_IS_GCC) /* TODO: Change this to proper discriminated feature */
__attribute__((format(printf,2,3)))
#endif /* compiler */
;

/** vprintf()-form of logging function, passing the formatted result to
 * the back-end
 *
 * \ingroup group__core_library
 *
 * \param severity The severity of the log entry
 * \param format The format string
 * \param args <code>va_list</code> form of the arguments
 *
 * \note The behaviour is undefined if the total size of the formatted output
 *   exceeds 4095 characters.
 *
 * \warning This function is not type-safe. C++ application code should
 *   prefer the \ref group__application_layer_interface "application layer"
 *   functions.
 */
PANTHEIOS_CALL(int) pantheios_logvprintf(
    pan_sev_t           severity
,   pan_char_t const*   format
,   va_list             args
);

/* /////////////////////////////////////////////////////////////////////////
 * Utility functions
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace util
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/** Prints a bail-out message to operating system-dependent
 *    facilities (e.g. console, file, debugger, event log, etc.) in
 *    the event that initialisation (of front-end and/or back-end(s))
 *    does not succeed.
 *
 * This is invoked within the core, and certain stock front-/back-ends,
 * if initialisation fails. Implementors of custom front-/back-ends may also
 * use it if they fail to initialise, in order to provide information that
 * can be tracked by a user / system administrator.
 *
 * \param severity The severity level at which the event will be represented
 *   to the operating-system dependent facilities
 * \param message The message to be emitted
 * \param processId An optional identifier of the process within which the
 *   subsystem has failed. May be NULL
 * \param qualifier An optional additional qualifier (e.g. error string) that
 *   may be used to pass additional information about the reason why the
 *   subsystem has failed. May be NULL
 *
 * \remarks On UNIX systems, this function will output to the console, the
 *   file "logging-bailout.txt" in the current directory, and to Syslog. On
 *   Windows systems, this function will output to the console, the file
 *   "logging-bailout.txt" in the current directory, the Windows debugger, and
 *   to the Windows Event Log.
 *
 * \see pantheios_onBailOut3
 */
PANTHEIOS_CALL(void) pantheios_onBailOut4(
    int           severity
,   char const*   message
,   char const*   processId
,   char const*   qualifier
);

/** Prints a bail-out message to operating system-dependent
 *    facilities (e.g. console, file, debugger, event log, etc.) in
 *    the event that initialisation (of front-end and/or back-end(s))
 *    does not succeed.
 *
 * This is invoked within the core, and certain stock front-/back-ends,
 * if initialisation fails. Implementors of custom front-/back-ends may also
 * use it if they fail to initialise, in order to provide information that
 * can be tracked by a user / system administrator.
 *
 * \param severity The severity level at which the event will be represented
 *   to the operating-system dependent facilities
 * \param message The message to be emitted
 * \param processId An optional identifier of the process within which the
 *   subsystem has failed. May be NULL
 *
 * \remarks On UNIX systems, this function will output to the console, the
 *   file "logging-bailout.txt" in the current directory, and to Syslog. On
 *   Windows systems, this function will output to the console, the file
 *   "logging-bailout.txt" in the current directory, the Windows debugger, and
 *   to the Windows Event Log.
 *
 * \see pantheios_onBailOut4
 *
 * \note This is equivalent to calling pantheios_onBailOut4() specifying NULL
 *   for the <code>qualifier</code> parameter
 */
PANTHEIOS_CALL(void) pantheios_onBailOut3(
    int           severity
,   char const*   message
,   char const*   processId
);

# ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

/** [Currently undocumented] Calculates the length of a string, in light of
 * special conditions indicated by special values of the len parameter.
 *
 * \note THIS FUNCTION IS NOT PART OF THE PUBLICLY DOCUMENTED API OF
 *   PANTHEIOS, AND IS SUBJECT TO REMOVAL/CHANGE IN A FUTURE RELEASE.
 */
PANTHEIOS_CALL(size_t) pantheios_util_strnlen(pan_char_t const* s, size_t len);
PANTHEIOS_CALL_DEPRECATED(size_t, pantheios_strnlen, pantheios_util_strnlen) pantheios_strnlen(pan_char_t const* s, size_t len);

# endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace util */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Core functions
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace core
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

# ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
/* /////////////////////////////////////
 * Undocumented API functions
 *
 * ALL OF THE FUNCTIONS IN THIS SECTION ARE NOT PART OF THE PUBLICLY
 * DOCUMENTED API OF PANTHEIOS, AND ARE SUBJECT TO REMOVAL/CHANGE IN A
 * FUTURE RELEASE.
 */

/** [Currently undocumented] Indicates whether the Pantheios libraries are
 * undergoing initialisation.
 *
 * \note THIS FUNCTION IS NOT PART OF THE PUBLICLY DOCUMENTED API OF
 *   PANTHEIOS, AND IS SUBJECT TO REMOVAL/CHANGE IN A FUTURE RELEASE.
 */
PANTHEIOS_CALL(int)     pantheios_isInitialising(void);
/** [Currently undocumented] Indicates whether the Pantheios libraries have
 * been successfully initialised.
 *
 * \note THIS FUNCTION IS NOT PART OF THE PUBLICLY DOCUMENTED API OF
 *   PANTHEIOS, AND IS SUBJECT TO REMOVAL/CHANGE IN A FUTURE RELEASE.
 */
PANTHEIOS_CALL(int)     pantheios_isInitialised(void);
/** [Currently undocumented] Exits the process with all haste, .
 *
 * \warning Calling this function causes the process to exit immediately,
 *   potentially leaking resources. Do not call this unless in extremis AND
 *   you know what you're doing!
 *
 * \note THIS FUNCTION IS NOT PART OF THE PUBLICLY DOCUMENTED API OF
 *   PANTHEIOS, AND IS SUBJECT TO REMOVAL/CHANGE IN A FUTURE RELEASE.
 */
#  if defined(STLSOFT_COMPILER_IS_MSVC)
__declspec(noreturn)
#  endif /* compiler */
PANTHEIOS_CALL(void)    pantheios_exitProcess(int code)
#  if defined(STLSOFT_COMPILER_IS_GCC)
__attribute__((noreturn))
#  endif /* compiler */
;
/** [Currently undocumented] Equivalent to pantheios_exitProcess().
 *
 * \note THIS FUNCTION IS NOT PART OF THE PUBLICLY DOCUMENTED API OF
 *   PANTHEIOS, AND IS SUBJECT TO REMOVAL/CHANGE IN A FUTURE RELEASE.
 */
PANTHEIOS_CALL_DEPRECATED(void, pantheios_exit_process, pantheios_exitProcess) pantheios_exit_process(int code);

/** [Currently undocumented] Allocates a permanent block of memory.
 *
 * \note THIS FUNCTION IS NOT PART OF THE PUBLICLY DOCUMENTED API OF
 *   PANTHEIOS, AND IS SUBJECT TO REMOVAL/CHANGE IN A FUTURE RELEASE.
 * 
 * \remarks Memory block is not release until Pantheios is uninitialised.
 *
 * \param cb The number of bytes to allocate
 *
 * \return the NULL pointer if allocation failed, otherwise a pointer to a
 *   correctly aligned block of bytes, all of whose values are 0x00.
 */
PANTHEIOS_CALL(void*)   pantheios_malloc(size_t cb);

/* Memory allocation/deallocation for inserter memory blocks */

/** [Currently undocumented] Allocates inserter memory.
 *
 * \warning Future versions of Pantheios might release asynchronously the
 *   memory allocated by this function, thereby breaking any code that calls
 *   it and uses it for purposes other than in the implementation of
 *   TEMPORARY inserter instances. Do you call this unless you know what
 *   you're doing!
 *
 * \note THIS FUNCTION IS NOT PART OF THE PUBLICLY DOCUMENTED API OF
 *   PANTHEIOS, AND IS SUBJECT TO REMOVAL/CHANGE IN A FUTURE RELEASE.
 */
PANTHEIOS_CALL(void*)   pantheios_inserterAllocate(size_t cb);
/** [Currently undocumented] Deallocates memory allocated with pantheios_inserterAllocate().
 *
 * \note THIS FUNCTION IS NOT PART OF THE PUBLICLY DOCUMENTED API OF
 *   PANTHEIOS, AND IS SUBJECT TO REMOVAL/CHANGE IN A FUTURE RELEASE.
 */
PANTHEIOS_CALL(void)    pantheios_inserterDeallocate(void* pv);

/** [Currently undocumented] Returns a pointer to a pad string of (up to)
 * the requested length.
 *
 * \note THIS FUNCTION IS NOT PART OF THE PUBLICLY DOCUMENTED API OF
 *   PANTHEIOS, AND IS SUBJECT TO REMOVAL/CHANGE IN A FUTURE RELEASE.
 *
 * \param minimumWidth The requested length of the pad string
 * \param actualWidth Pointer to a variable to receive the actual number of
 *   pad characters available. Result may be less than \c minimumWidth but
 *   will never be so for requests less than 1000. Must not be NULL.
 *
 * \remarks Guaranteed to support up to 1000 characters.
 *
 * \note The string is NOT guaranteed to be nul-terminated!
 *
 * \pre (NULL != actualWidth)
 */
PANTHEIOS_CALL(pan_char_t const*) pantheios_getPad(size_t minimumWidth, size_t* actualWidth);

# endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace core */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Core functions
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace core
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/** Returns a (thread-safe) unique back-end identifier
 *
 * \ingroup group__core_library
 *
 * This function provides back-end identiers, useful with custom use of
 * Pantheios \ref group__backend "back-ends", that are guaranteed unique
 * throughout a given process, even in the case where it is invoked by
 * multiple threads concurrently.
 *
 * \note The returned values from this function are always >1000, so as not
 *   to clash with any of the
 *   \ref group__backend__stock_ids "pre-defined values" used by the
 *   Pantheios infrastructure.
 *
 * \pre The behaviour of this function is undefined if it is called more
 *   than INT_MAX - 1000 times in the lifetime of a process
 */
PANTHEIOS_CALL(int) pantheios_getNextBackEndId(void);

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace core */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/** A functional equivalent to\htmlonly <code>::puts()</code>\endhtmlonly, incorporating a
 *   severity level.
 *
 * \ingroup group__core_library
 *
 * Using this function skips the application layer entirely and, if the
 * given severity level is enabled, goes straight to the back-end. It is
 * therefore more suitable for use in emergency bail-out situations, such as
 * in an application-level <code>catch(...)</code> clause.
 *
 * \param severity The severity of the message to be output.
 * \param message The message to be output.
 */
PANTHEIOS_CALL(void) pantheios_logputs(
    pan_sev_t           severity
,   pan_char_t const*   message
);

/** [DEPRECATED] Equivalent to \link pantheios::pantheios_logputs() pantheios_logputs()\endlink.
 *
 * \ingroup group__core_library
 *
 * \deprecated This function is now obsolete, and will be removed from a future
 *   version of Pantheios; instead
 *   use \link pantheios::pantheios_logputs pantheios_logputs()\endlink.
 */
PANTHEIOS_CALL_DEPRECATED(void, pantheios_puts, pantheios_logputs) pantheios_puts(pan_sev_t severity, pan_char_t const* message);

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
PANTHEIOS_CALL(void) pantheios_logassertfail(
    pan_sev_t   severity
,   char const* fileLine
,   char const* message
);
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Back-end map functions
 */

#if 0 /* Not yet released */

/** Indicates whether the back-end map contains the given back-end
 *    Id and, if it does, optionally returns the associated token into a
 *    caller-supplied pointer.
 *
 * \ingroup group__core_library
 *
 * \param backEndId The identifier of the back-end
 * \param ptoken Pointer to a <code>void*</code> variable to receive the
 *    pointer to the token. May be NULL, if the caller does not want to
 *    retrieve the token.
 *
 * \return A Boolean value indicating whether the back-end is registered in
 *    the map.
 * \retval 0 The value of backEndId is not in the map
 * \retval 1 The value of backEndId is in the map
 */
PANTHEIOS_CALL(int) pantheios_backEndMap_lookup(int backEndId, void** ptoken);

/** Adds an entry into the back-end map.
 *
 * \ingroup group__core_library
 *
 * \param backEndId The identifier of the back-end
 * \param token The token value to add
 *
 * \return A value indicating whether the addition was successful
 * \retval 0 The entry is added, and backEndId is linked to token
 * \retval 1 An entry already exists for backEndId. It is NOT updated.
 * \retval <0 The operation failed
 */
PANTHEIOS_CALL(int) pantheios_backEndMap_add(int backEndId, void* token);

/** Removes an entry from the back-end map.
 *
 * \ingroup group__core_library
 *
 * \param backEndId The identifier of the back-end to be removed
 *
 * \return A value indicating whether the removal was successful
 * \retval 0 No entry for the given backEndId exists
 * \retval 1 The entry is removed
 * \retval <0 The operation failed
 */
PANTHEIOS_CALL(int) pantheios_backEndMap_remove(int backEndId);
#endif /* 0 */

/* /////////////////////////////////////////////////////////////////////////
 * Generated function control
 */

#if defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION) || \
    !defined(PANTHEIOS_APPL_PARAMS_LIMIT)
 /** \def PANTHEIOS_APPL_PARAMS_LIMIT
  * The number of parameters available in
  * the \ref group__application_layer_interface "Application Layer API",
  * which can be overridden by defining it to a number between 1 and 32 to
  * reduce compilation times where larger parameter lists are not required.
  */
# define PANTHEIOS_APPL_PARAMS_LIMIT    PANTHEIOS_APPL_PARAMS_LIMIT_MAX_GENERATED
#endif /* PANTHEIOS_DOCUMENTATION_SKIP_SECTION || !PANTHEIOS_APPL_PARAMS_LIMIT */

/* Sanity check on PANTHEIOS_APPL_PARAMS_LIMIT */
#if PANTHEIOS_APPL_PARAMS_LIMIT < 0 || \
    PANTHEIOS_APPL_PARAMS_LIMIT > PANTHEIOS_APPL_PARAMS_LIMIT_MAX_GENERATED
# error PANTHEIOS_APPL_PARAMS_LIMIT Must be defined to be a number between 1 and PANTHEIOS_APPL_PARAMS_LIMIT_MAX_GENERATED
#endif /* PANTHEIOS_APPL_PARAMS_LIMIT */

/* /////////////////////////////////////////////////////////////////////////
 * API functions for C++
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)

/** 8-bit unsigned integer type. */
typedef pan_uint8_t     uint8_t;
/** 16-bit unsigned integer type. */
typedef pan_uint16_t    uint16_t;
/** 32-bit unsigned integer type. */
typedef pan_uint32_t    uint32_t;
/** 64-bit unsigned integer type. */
typedef pan_uint64_t    uint64_t;

/** 8-bit signed integer type. */
typedef pan_sint8_t     sint8_t;
/** 16-bit signed integer type. */
typedef pan_sint16_t    sint16_t;
/** 32-bit signed integer type. */
typedef pan_sint32_t    sint32_t;
/** 64-bit signed integer type. */
typedef pan_sint64_t    sint64_t;

/* /////////////////////////////////////////////////////////////////////////
 * Utility Functions
 */

namespace util
{

# ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

inline void onBailOut(int severity, char const* message, char const* processId)
{
    pantheios_onBailOut3(severity, message, processId);
}

inline void onBailOut(int severity, char const* message, char const* processId, char const* qualifier)
{
    pantheios_onBailOut4(severity, message, processId, qualifier);
}

inline size_t strnlen(pan_char_t const* s, size_t len)
{
    return pantheios_util_strnlen(s, len);
}

# endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

} /* namespace util */

/* /////////////////////////////////////////////////////////////////////////
 * Core Functions
 */

/** Equivalent to \ref pantheios::pantheios_init "pantheios_init()".
 *
 * \ingroup group__core_library
 */
inline int init()
{
    return pantheios_init();
}

/** Equivalent to \ref pantheios::pantheios_uninit "pantheios_uninit()".
 *
 * \ingroup group__core_library
 */
inline void uninit()
{
    pantheios_uninit();
}

namespace core
{

# ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

inline int isInitialising()
{
    return pantheios_isInitialising();
}

inline int isInitialised()
{
    return pantheios_isInitialised();
}

#  if defined(STLSOFT_COMPILER_IS_GCC)
void exitProcess(int code)
__attribute__((noreturn));
#  endif /* compiler */

#  if defined(STLSOFT_COMPILER_IS_MSVC)
__declspec(noreturn)
#  endif /* compiler */
inline void exitProcess(int code)
{
    pantheios_exitProcess(code);
}

inline PANTHEIOS_DECLARE_DEPRECATION("function", exit_process, exitProcess) void exit_process(int code)
{
    exitProcess(code);
}

#ifndef malloc
inline void* malloc(size_t cb)
{
    return pantheios_malloc(cb);
}
#endif /* !malloc */

inline void* inserterAllocate(size_t cb)
{
    return pantheios_inserterAllocate(cb);
}

inline void inserterDeallocate(void* pv)
{
    pantheios_inserterDeallocate(pv);
}

inline pan_char_t const* getPad(size_t minimumWidth, size_t* actualWidth)
{
    return pantheios_getPad(minimumWidth, actualWidth);
}

# endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

} /* namespace core */

/** Equivalent to \ref pantheios::pantheios_isSeverityLogged "pantheios_isSeverityLogged()".
 *
 * \ingroup group__core_library
 */
inline int isSeverityLogged(pan_sev_t severity)
{
    return pantheios_isSeverityLogged(severity);
}

# ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
/** [Currently undocumented] Equivalent to \ref pantheios::pantheios_getProcessIdentity "pantheios_getProcessIdentity()"
 *
 * \note THIS FUNCTION IS NOT PART OF THE PUBLICLY DOCUMENTED API OF
 *   PANTHEIOS, AND IS SUBJECT TO REMOVAL/CHANGE IN A FUTURE RELEASE.
 */
inline pan_char_t const* getProcessIdentity()
{
    return pantheios_getProcessIdentity();
}
# endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/** Equivalent to \ref pantheios::pantheios_getStockSeverityString "pantheios_getStockSeverityString()".
 *
 * \ingroup group__core_library
 */
inline pan_char_t const* getStockSeverityString(pan_sev_t severity)
{
    return pantheios_getStockSeverityString(severity);
}

# ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

inline PANTHEIOS_DECLARE_DEPRECATION("function", getSeverityString, getStockSeverityString) pan_char_t const* getSeverityString(pan_sev_t severity)
{
    return getStockSeverityString(severity);
}

# endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/** Equivalent to \ref pantheios::pantheios_getStockSeverityStringLength "pantheios_getStockSeverityStringLength()".
 *
 * \ingroup group__core_library
 */
inline size_t getStockSeverityStringLength(pan_sev_t severity)
{
    return pantheios_getStockSeverityStringLength(severity);
}

# ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

inline PANTHEIOS_DECLARE_DEPRECATION("function", getSeverityStringLength, getStockSeverityStringLength) size_t getSeverityStringLength(pan_sev_t severity)
{
    return getStockSeverityStringLength(severity);
}

# endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/** Equivalent to \ref pantheios::pantheios_getInitCodeString "pantheios_getInitCodeString()".
 *
 * \ingroup group__core_library
 */
inline char const* getInitCodeString(int code)
{
    return pantheios_getInitCodeString(code);
}

# ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

inline PANTHEIOS_DECLARE_DEPRECATION("function", getInitErrorString, getInitCodeString) char const* getInitErrorString(int code)
{
    return getInitCodeString(code);
}

# endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/** Equivalent to \ref pantheios::pantheios_getInitCodeStringLength "pantheios_getInitCodeStringLength()".
 *
 * \ingroup group__core_library
 */
inline size_t getInitCodeStringLength(int code)
{
    return pantheios_getInitCodeStringLength(code);
}

# ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

inline PANTHEIOS_DECLARE_DEPRECATION("function", getInitErrorStringLength, getInitCodeStringLength) size_t getInitErrorStringLength(int code)
{
    return getStockSeverityStringLength(code);
}

# endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

namespace core
{

/** Equivalent to \ref pantheios::core::pantheios_getNextBackEndId "pantheios_getNextBackEndId()".
 *
 * \pre The behaviour of this function is undefined if it is called more
 *   than INT_MAX - 1000 times in the lifetime of a process
 */
inline int getNextBackEndId()
{
    return pantheios_getNextBackEndId();
}

} /* namespace core */

/** Equivalent to \ref pantheios::pantheios_logputs "pantheios_logputs()".
 *
 * \ingroup group__core_library
 */
inline void logputs(
    pan_sev_t           severity
,   pan_char_t const*   message
)
{
    pantheios_logputs(severity, message);
}

/** [DEPRECATED] Equivalent to \ref pantheios::pantheios_logputs "pantheios_logputs()".
 *
 * \ingroup group__core_library
 *
 * \deprecated This function is now obsolete, and will be removed from a future
 *   version of Pantheios; instead
 *   use \link pantheios::logputs logputs()\endlink.
 */
inline PANTHEIOS_DECLARE_DEPRECATION("function", puts, logputs) void puts(pan_sev_t severity, pan_char_t const* message)
{
    pantheios_logputs(severity, message);
}

/** Equivalent to \ref pantheios::pantheios_logprintf "pantheios_logprintf()".
 *
 * \ingroup group__core_library
 */

#if !defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION) && \
    defined(STLSOFT_COMPILER_IS_GCC) /* TODO: Change this to proper discriminated feature */
int logprintf(
    pan_sev_t           severity
,   pan_char_t const*   format
,   ...
)
__attribute__((format(printf,2,3)))
;
#endif /* compiler */

inline int logprintf(
    pan_sev_t           severity
,   pan_char_t const*   format
,   ...
)
{
    va_list args;
    int     ret;

    va_start(args, format);

    ret = pantheios_logvprintf(severity, format, args);

    va_end(args);

    return ret;
}


/** Equivalent to \ref pantheios::pantheios_logvprintf "pantheios_logvprintf()".
 *
 * \ingroup group__core_library
 */
inline int logvprintf(
    pan_sev_t           severity
,   pan_char_t const*   format
,   va_list             args
)
{
    return pantheios_logvprintf(severity, format, args);
}

#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * String access shims
 */

/* TODO: move all these shims into a separate file */

#ifdef __cplusplus

# if !defined(PANTHEIOS_NO_NAMESPACE)
namespace shims
{
# endif /* !PANTHEIOS_NO_NAMESPACE */

/** Returns a nul-terminated non-NULL C-style string representing the slice */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_ptr_w(pan_slice_t const& s)
{
    return (0 == s.len) ? L"" : s.ptr;
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_ptr_a(pan_slice_t const& s)
{
    return (0 == s.len) ? "" : s.ptr;
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** Returns a nul-terminated non-NULL C-style string representing the slice */
inline pan_char_t const* c_str_ptr(pan_slice_t const& s)
{
# ifdef PANTHEIOS_USE_WIDE_STRINGS
    return c_str_ptr_w(s);
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    return c_str_ptr_a(s);
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
}

/** Returns a nul-terminated potentially null C-style string representing the slice */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_ptr_null_w(pan_slice_t const& s)
{
    return (0 != s.len) ? s.ptr : NULL;
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_ptr_null_a(pan_slice_t const& s)
{
    return (0 != s.len) ? s.ptr : NULL;
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** Returns a nul-terminated potentially null C-style string representing the slice */
inline pan_char_t const* c_str_ptr_null(pan_slice_t const& s)
{
# ifdef PANTHEIOS_USE_WIDE_STRINGS
    return c_str_ptr_null_w(s);
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    return c_str_ptr_null_a(s);
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
}

/** Returns a possibly non-nul-terminated non-NULL C-style string representing the slice */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_data_w(pan_slice_t const& s)
{
    return c_str_ptr(s);
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_data_a(pan_slice_t const& s)
{
    return c_str_ptr(s);
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** Returns a possibly non-nul-terminated non-NULL C-style string representing the slice */
inline pan_char_t const* c_str_data(pan_slice_t const& s)
{
# ifdef PANTHEIOS_USE_WIDE_STRINGS
    return c_str_data_w(s);
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    return c_str_data_a(s);
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
}

/** Returns the number of characters in the length of the C-style string representing the slice */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline size_t c_str_len_w(pan_slice_t const& s)
{
    return s.len;
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline size_t c_str_len_a(pan_slice_t const& s)
{
    return s.len;
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** Returns the number of characters in the length of the C-style string representing the slice */
inline size_t c_str_len(pan_slice_t const& s)
{
# ifdef PANTHEIOS_USE_WIDE_STRINGS
    return c_str_len_w(s);
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    return c_str_len_a(s);
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
}



/** Returns a nul-terminated non-NULL C-style string representing the slice */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_ptr_w(pan_slice_t const* s)
{
    return (NULL != s) ? c_str_ptr_w(*s) : L"";
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_ptr_a(pan_slice_t const* s)
{
    return (NULL != s) ? c_str_ptr_a(*s) : "";
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** Returns a nul-terminated non-NULL C-style string representing the slice */
inline pan_char_t const* c_str_ptr(pan_slice_t const* s)
{
# ifdef PANTHEIOS_USE_WIDE_STRINGS
    return c_str_ptr_w(s);
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    return c_str_ptr_a(s);
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
}

/** Returns a nul-terminated potentially null C-style string representing the slice */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_ptr_null_w(pan_slice_t const* s)
{
    return (NULL != s && (0 != s->len)) ? s->ptr : NULL;
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_ptr_null_a(pan_slice_t const* s)
{
    return (NULL != s && (0 != s->len)) ? s->ptr : NULL;
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** Returns a nul-terminated potentially null C-style string representing the slice */
inline pan_char_t const* c_str_ptr_null(pan_slice_t const* s)
{
# ifdef PANTHEIOS_USE_WIDE_STRINGS
    return c_str_ptr_null_w(s);
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    return c_str_ptr_null_a(s);
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
}

/** Returns a possibly non-nul-terminated non-NULL C-style string representing the slice */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_data_w(pan_slice_t const* s)
{
    return c_str_ptr(s);
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_data_a(pan_slice_t const* s)
{
    return c_str_ptr(s);
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** Returns a possibly non-nul-terminated non-NULL C-style string representing the slice */
inline pan_char_t const* c_str_data(pan_slice_t const* s)
{
# ifdef PANTHEIOS_USE_WIDE_STRINGS
    return c_str_data_w(s);
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    return c_str_data_a(s);
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
}

/** Returns the number of characters in the length of the C-style string representing the slice */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline size_t c_str_len_w(pan_slice_t const* s)
{
    return (NULL != s) ? s->len : 0;
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline size_t c_str_len_a(pan_slice_t const* s)
{
    return (NULL != s) ? s->len : 0;
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** Returns the number of characters in the length of the C-style string representing the slice */
inline size_t c_str_len(pan_slice_t const* s)
{
# ifdef PANTHEIOS_USE_WIDE_STRINGS
    return c_str_len_w(s);
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    return c_str_len_a(s);
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
}



# ifndef PANTHEIOS_NO_STOCK_LEVELS
/** Returns a possibly non-nul-terminated non-NULL C-style string representing the severity */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_data_w(pan_severity_t severity)
{
    return pantheios_getStockSeverityString(severity);
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_data_a(pan_severity_t severity)
{
    return pantheios_getStockSeverityString(severity);
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** Returns a possibly non-nul-terminated non-NULL C-style string representing the severity */
inline pan_char_t const* c_str_data(pan_severity_t severity)
{
    return pantheios_getStockSeverityString(severity);
}

/** Returns the number of characters in the length of the C-style string representing the severity */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline size_t c_str_len_w(pan_severity_t severity)
{
    return pantheios_getStockSeverityStringLength(severity);
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline size_t c_str_len_a(pan_severity_t severity)
{
    return pantheios_getStockSeverityStringLength(severity);
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** Returns the number of characters in the length of the C-style string representing the severity */
inline size_t c_str_len(pan_severity_t severity)
{
    return pantheios_getStockSeverityStringLength(severity);
}

/** Returns a nul-terminated non-NULL C-style string representing the severity */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_ptr_w(pan_severity_t severity)
{
    return pantheios_getStockSeverityString(severity);
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_ptr_a(pan_severity_t severity)
{
    return pantheios_getStockSeverityString(severity);
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** Returns a nul-terminated non-NULL C-style string representing the severity */
inline pan_char_t const* c_str_ptr(pan_severity_t severity)
{
    return pantheios_getStockSeverityString(severity);
}

/** Returns a nul-terminated potentially null C-style string representing the severity */
# ifdef PANTHEIOS_USE_WIDE_STRINGS
inline wchar_t const* c_str_ptr_null_w(pan_severity_t severity)
{
    wchar_t const* s = pantheios_getStockSeverityString(severity);

    return ('\0' != *s) ? s : NULL;
}
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
inline char const* c_str_ptr_null_a(pan_severity_t severity)
{
    char const* s = pantheios_getStockSeverityString(severity);

    return ('\0' != *s) ? s : NULL;
}
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
/** Returns a nul-terminated potentially null C-style string representing the severity */
inline pan_char_t const* c_str_ptr_null(pan_severity_t severity)
{
    pan_char_t const* s = pantheios_getStockSeverityString(severity);

    return ('\0' != *s) ? s : NULL;
}
# endif /* !PANTHEIOS_NO_STOCK_LEVELS */

# if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace shims */
# endif /* !PANTHEIOS_NO_NAMESPACE */

#endif /* __cplusplus */

/* /////////////////////////////////////////////////////////////////////////
 * pan_slice_t
 */

#ifdef __cplusplus
# ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION

inline /* static */ size_t pan_slice_t::calc_length_n_(pan_char_t const* s, size_t len)
{
#if !defined(PANTHEIOS_NO_NAMESPACE)
    return pantheios::util::strnlen(s, len);
#else /* ? !PANTHEIOS_NO_NAMESPACE */
    return pantheios_util_strnlen(s, len);
#endif /* !PANTHEIOS_NO_NAMESPACE */
}

inline pan_slice_t::pan_slice_t()
    : len(0)
    , ptr(NULL)
{}

inline pan_slice_t::pan_slice_t(pan_char_t const* p, size_t l)
    : len(l)
    , ptr(p)
{
    static size_t const topBit = size_t(0x01) << (sizeof(size_t) * 8 - 1);

    if(topBit & len)
    {
        this->len = calc_length_n_(p, len);
    }
}

inline pan_slice_t::pan_slice_t(pan_slice_t const& rhs)
    : len(rhs.len)
    , ptr(rhs.ptr)
{}
inline pan_slice_t& pan_slice_t::operator =(pan_slice_t const& rhs)
{
    len = rhs.len;
    ptr = rhs.ptr;

    return *this;
}

inline pan_slice_t::pan_slice_t(int l, pan_char_t const* p)
    : len(calc_length_n_(p, static_cast<size_t>(l)))
    , ptr(p)
{}

# endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
#endif /* __cplusplus */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */


# ifndef PANTHEIOS_NO_STOCK_LEVELS

/* #define the severity levels at the global level, since the construction
 * of pantheios::PANTHEIOS_SEV_xxx is too wordy and unnecessary.
 */

/** \def PANTHEIOS_SEV_EMERGENCY
 *
 * Equivalent to pantheios::SEV_EMERGENCY
 */

/** \def PANTHEIOS_SEV_ALERT
 *
 * Equivalent to pantheios::SEV_ALERT
 */

/** \def PANTHEIOS_SEV_CRITICAL
 *
 * Equivalent to pantheios::SEV_CRITICAL
 */

/** \def PANTHEIOS_SEV_ERROR
 *
 * Equivalent to pantheios::SEV_ERROR
 */

/** \def PANTHEIOS_SEV_WARNING
 *
 * Equivalent to pantheios::SEV_WARNING
 */

/** \def PANTHEIOS_SEV_NOTICE
 *
 * Equivalent to pantheios::SEV_NOTICE
 */

/** \def PANTHEIOS_SEV_INFORMATIONAL
 *
 * Equivalent to pantheios::SEV_INFORMATIONAL
 */

/** \def PANTHEIOS_SEV_DEBUG
 *
 * Equivalent to pantheios::SEV_DEBUG
 */

#  define PANTHEIOS_SEV_EMERGENCY       ::pantheios::SEV_EMERGENCY
#  define PANTHEIOS_SEV_ALERT           ::pantheios::SEV_ALERT
#  define PANTHEIOS_SEV_CRITICAL        ::pantheios::SEV_CRITICAL
#  define PANTHEIOS_SEV_ERROR           ::pantheios::SEV_ERROR
#  define PANTHEIOS_SEV_WARNING         ::pantheios::SEV_WARNING
#  define PANTHEIOS_SEV_NOTICE          ::pantheios::SEV_NOTICE
#  define PANTHEIOS_SEV_INFORMATIONAL   ::pantheios::SEV_INFORMATIONAL
#  define PANTHEIOS_SEV_DEBUG           ::pantheios::SEV_DEBUG

# endif /* !PANTHEIOS_NO_STOCK_LEVELS */


#ifdef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
/** The <a href = "http://stlsoft.org" target="_blank">STLSoft</a> namespace - \c stlsoft.
 *
 * The Pantheios project inserts overloads of the <strong>c_str_data_a</strong> and <strong>c_str_len_a</strong>
 * <a href = "http://www.drdobbs.com/184401689"><em>String Access Shims</em></a>
 * into the <code>stlsoft</code> namespace, for generalised manipulation of various Pantheios types,
 * including
 * \c pantheios::pan_slice_t, \c pantheios::integer,
 * \c pantheios::pointer, and \c pantheios::real.
 *
 * Because <a href = "http://www.synesis.com.au/resources/articles/cpp/shims.pdf"><em>Shims</em></a>
 * are unbound sets of functions, opening and extending the
 * <code>stlsoft</code> namespace in this way is quite legitimate.
 */
#endif /* PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
namespace stlsoft
{

#ifdef PANTHEIOS_USE_WIDE_STRINGS
    using ::pantheios::shims::c_str_data_w;
    using ::pantheios::shims::c_str_len_w;
    using ::pantheios::shims::c_str_ptr_w;
    using ::pantheios::shims::c_str_ptr_null_w;
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    using ::pantheios::shims::c_str_data_a;
    using ::pantheios::shims::c_str_len_a;
    using ::pantheios::shims::c_str_ptr_a;
    using ::pantheios::shims::c_str_ptr_null_a;
#endif /* PANTHEIOS_USE_WIDE_STRINGS */
    using ::pantheios::shims::c_str_data;
    using ::pantheios::shims::c_str_len;
    using ::pantheios::shims::c_str_ptr;
    using ::pantheios::shims::c_str_ptr_null;

} /* namespace stlsoft */

#else /* ? !PANTHEIOS_NO_NAMESPACE */

# ifdef __cplusplus
namespace stlsoft
{

#ifdef PANTHEIOS_USE_WIDE_STRINGS
    using ::c_str_data_w;
    using ::c_str_len_w;
    using ::c_str_ptr_w;
    using ::c_str_ptr_null_w;
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    using ::c_str_data_a;
    using ::c_str_len_a;
    using ::c_str_ptr_a;
    using ::c_str_ptr_null_a;
#endif /* PANTHEIOS_USE_WIDE_STRINGS */
    using ::c_str_data;
    using ::c_str_len;
    using ::c_str_ptr;
    using ::c_str_ptr_null;

} /* namespace stlsoft */
# endif /* __cplusplus */

#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Inclusion
 */

#ifdef STLSOFT_PPF_pragma_once_SUPPORT
# pragma once
#endif /* STLSOFT_PPF_pragma_once_SUPPORT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

/* ///////////////////////////// end of file //////////////////////////// */
