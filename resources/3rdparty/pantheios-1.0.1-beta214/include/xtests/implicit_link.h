/* /////////////////////////////////////////////////////////////////////////
 * File:    xtests/implicit_link.h (formerly xtests/xtests_implicit_link.h)
 *
 * Purpose: Implicit linking for the xTests API
 *
 * Created: 3rd March 2003
 * Updated: 11th August 2010
 *
 * Home:    http://xtests.org/
 *
 * Copyright (c) 2003-2010, Matthew Wilson and Synesis Software
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


/** \file xtests/implicit_link.h
 *
 * [C, C++] Implicit linking for the xTests library.
 *
 * This file may be <code>\#include</code>d in any compilation unit within a
 * given link unit to implicitly link in the appropriate <b>xTests</b>
 * archive (library).
 *
 * \note For compilers that do not support implicit linking, inclusion of
 *  the file has no effect.
 */

#ifndef XTESTS_INCL_XTESTS_H_IMPLICIT_LINK
#define XTESTS_INCL_XTESTS_H_IMPLICIT_LINK

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef XTESTS_DOCUMENTATION_SKIP_SECTION
# define XTESTS_VER_XTESTS_H_IMPLICIT_LINK_MAJOR    1
# define XTESTS_VER_XTESTS_H_IMPLICIT_LINK_MINOR    9
# define XTESTS_VER_XTESTS_H_IMPLICIT_LINK_REVISION 1
# define XTESTS_VER_XTESTS_H_IMPLICIT_LINK_EDIT     29
#endif /* !XTESTS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef XTESTS_INCL_XTESTS_H_XTESTS
# include <xtests/xtests.h>
#endif /* !XTESTS_INCL_XTESTS_H_XTESTS */

#include <platformstl/platformstl.h>

/* /////////////////////////////////////////////////////////////////////////
 * Implicit linking
 */

#if defined(_WIN32) || \
    defined(_WIN64)

# if defined(__BORLANDC__) || \
     /* defined(__DMC__) || */ \
     defined(__INTEL_COMPILER) || \
     defined(__MWERKS__) || \
     defined(_MSC_VER)
#  if !defined(__COMO__)
#   define XTESTS_IMPLICIT_LINK_SUPPORT
#  endif /* compiler */
# endif /* compiler */

# if defined(XTESTS_IMPLICIT_LINK_SUPPORT) && \
     defined(XTESTS_NO_IMPLICIT_LINK)
#  undef XTESTS_IMPLICIT_LINK_SUPPORT
# endif /* XTESTS_IMPLICIT_LINK_SUPPORT && XTESTS_NO_IMPLICIT_LINK */

# if defined(XTESTS_IMPLICIT_LINK_SUPPORT)

  /* prefix */

#  define XTESTS_IMPL_LINK_PREFIX

  /* library basename */

#  define XTESTS_IMPL_LINK_LIBRARY_BASENAME     "xtests"

  /* major version */

#  define XTESTS_IMPL_LINK_MAJOR_VERSION        "." STLSOFT_STRINGIZE(_XTESTS_VER_MAJOR)

  /* module name */

#  define XTESTS_IMPL_LINK_MODULE_NAME          ".core"

  /* compiler tag */

#  if defined(__BORLANDC__)
#   if __BORLANDC__ == 0x0550
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "bc55"
#   elif (__BORLANDC__ == 0x0551)
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "bc551"
#   elif (__BORLANDC__ == 0x0560)
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "bc56"
#   elif (__BORLANDC__ == 0x0564)
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "bc564"
#   elif (__BORLANDC__ == 0x0582)
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "bc582"
#   elif (0x0590 == (__BORLANDC__ & 0xfff0))
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "bc59x"
#   elif (0x0610 == (__BORLANDC__ & 0xfff0))
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "bc61x"
#   else /* ? __BORLANDC__ */
#    error Unrecognised value of __BORLANDC__
#   endif /* __BORLANDC__ */

/*
#  elif defined(__DMC__)
#   define XTESTS_IMPL_LINK_COMPILER_NAME       "dm"
 */

#  elif defined(__INTEL_COMPILER)
#   if __INTEL_COMPILER == 600
#    define   XTESTS_IMPL_LINK_COMPILER_NAME    "icl6"
#   elif __INTEL_COMPILER == 700
#    define   XTESTS_IMPL_LINK_COMPILER_NAME    "icl7"
#   elif __INTEL_COMPILER == 800
#    define   XTESTS_IMPL_LINK_COMPILER_NAME    "icl8"
#   elif __INTEL_COMPILER == 900
#    define   XTESTS_IMPL_LINK_COMPILER_NAME    "icl9"
#   elif __INTEL_COMPILER == 1000
#    define   XTESTS_IMPL_LINK_COMPILER_NAME    "icl10"
#   elif __INTEL_COMPILER == 1100
#    define   XTESTS_IMPL_LINK_COMPILER_NAME    "icl11"
#   else /* ? __INTEL_COMPILER */
#    error Intel C/C++ version not supported
#   endif /* __INTEL_COMPILER */

#  elif defined(__MWERKS__)
#   if ((__MWERKS__ & 0xFF00) == 0x2400)
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "cw7"
#   elif ((__MWERKS__ & 0xFF00) == 0x3000)
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "cw8"
#   elif ((__MWERKS__ & 0xFF00) == 0x3200)
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "cw9"
#   else /* ? __MWERKS__ */
#    error Unrecognised value of __MWERKS__
#   endif /* __MWERKS__ */

#  elif defined(_MSC_VER)
#   if _MSC_VER == 1000
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "vc4"
#   elif _MSC_VER == 1020
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "vc42"
#   elif _MSC_VER == 1100
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "vc5"
#   elif _MSC_VER == 1200
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "vc6"
#   elif _MSC_VER == 1300
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "vc7"
#   elif _MSC_VER == 1310
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "vc71"
#   elif _MSC_VER == 1400
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "vc8"
#   elif _MSC_VER == 1500
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "vc9"
#   elif _MSC_VER == 1600
#    define XTESTS_IMPL_LINK_COMPILER_NAME      "vc10"
#   else /* ? _MSC_VER */
#    error Visual C++ version not supported
#   endif /* _MSC_VER */

#  else /* ? compiler */
#   error Unrecognised compiler
#  endif /* compiler */


  /* operating system tag */

#  if defined(_STLSOFT_FORCE_ANY_COMPILER) && \
      defined(PLATFORMSTL_OS_IS_UNIX) && \
      defined(_WIN32)
#   define XTESTS_IMPL_LINK_OS_TAG              ".unix"
#  endif /* pseudo UNIX */

#  if !defined(XTESTS_IMPL_LINK_OS_TAG)
#   define XTESTS_IMPL_LINK_OS_TAG              ""
#  endif /* !XTESTS_IMPL_LINK_OS_TAG */


  /* architecture tag */

#  if defined(PLATFORMSTL_ARCH_IS_X86)
#   define XTESTS_IMPL_LINK_ARCH_TAG            ""
#  elif defined(PLATFORMSTL_ARCH_IS_X64)
#   define XTESTS_IMPL_LINK_ARCH_TAG            ".x64"
#  elif defined(PLATFORMSTL_ARCH_IS_IA64)
#   define XTESTS_IMPL_LINK_ARCH_TAG            ".ia64"
#  endif /* arch */

#  if !defined(XTESTS_IMPL_LINK_ARCH_TAG)
#   define XTESTS_IMPL_LINK_ARCH_TAG            ""
#  endif /* !XTESTS_IMPL_LINK_ARCH_TAG */


  /* encoding tag */

#  if defined(XTESTS_USE_WIDE_STRINGS)
#   define XTESTS_IMPL_LINK_ENCODING_TAG        ".widestring"
#  else /* ? XTESTS_USE_WIDE_STRINGS */
#   define XTESTS_IMPL_LINK_ENCODING_TAG        ""
#  endif /* XTESTS_USE_WIDE_STRINGS */


  /* threading tag */

#  if defined(__MT__) || \
      defined(_REENTRANT) || \
      defined(_MT)
#   if defined(_DLL) || \
       defined(__DLL)
#    define XTESTS_IMPL_LINK_THREADING_TAG      ".dll"
#   else /* ? dll */
#    define XTESTS_IMPL_LINK_THREADING_TAG      ".mt"
#   endif /* dll */
#  else /* ? mt */
#    define XTESTS_IMPL_LINK_THREADING_TAG      ""
#  endif /* mt */


  /* NoX */

#  if defined(XTESTS_CF_NOX)
#   define XTESTS_IMPL_LINK_NOX_TAG             ".nox"
#  else /* ? XTESTS_CF_NOX */
#   define XTESTS_IMPL_LINK_NOX_TAG             ""
#  endif /* XTESTS_CF_NOX */


  /* debug tag */

#  if !defined(NDEBUG) && \
      defined(_DEBUG)
#   define XTESTS_IMPL_LINK_DEBUG_TAG           ".debug"
#  else /* ? _DEBUG */
#   define XTESTS_IMPL_LINK_DEBUG_TAG           ""
#  endif /* _DEBUG */


  /* suffix */

#  define XTESTS_IMPL_LINK_SUFFIX               ".lib"


   /* Library name is:
    *
    * [lib]<library-basename>.<major-version>.<module-name>.<compiler-name>[.<os-arch-tag>][.<char-encoding-tag>][.<threading-tag>][.<nox-tag>][.<debug-tag>].{a|lib}
    */

#  define XTESTS_IMPL_LINK_LIBRARY_NAME         XTESTS_IMPL_LINK_PREFIX \
                                                XTESTS_IMPL_LINK_LIBRARY_BASENAME \
                                                XTESTS_IMPL_LINK_MAJOR_VERSION \
                                                XTESTS_IMPL_LINK_MODULE_NAME \
                                                "." XTESTS_IMPL_LINK_COMPILER_NAME \
                                                XTESTS_IMPL_LINK_OS_TAG \
                                                XTESTS_IMPL_LINK_ARCH_TAG \
                                                XTESTS_IMPL_LINK_ENCODING_TAG \
                                                XTESTS_IMPL_LINK_THREADING_TAG \
                                                XTESTS_IMPL_LINK_NOX_TAG \
                                                XTESTS_IMPL_LINK_DEBUG_TAG \
                                                XTESTS_IMPL_LINK_SUFFIX
                                                        

#  pragma message("lib: " XTESTS_IMPL_LINK_LIBRARY_NAME)

#  pragma comment(lib, XTESTS_IMPL_LINK_LIBRARY_NAME)

# endif /* XTESTS_IMPLICIT_LINK_SUPPORT */

#endif /* Win-32 || Win-64 */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !XTESTS_INCL_XTESTS_H_IMPLICIT_LINK */

/* ////////////////////////////////////////////////////////////////////// */
