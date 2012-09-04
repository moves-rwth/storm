/* /////////////////////////////////////////////////////////////////////////
 * File:    pantheios/implicit_link/implicit_link_base_.h
 *
 * Purpose: Implicit linking for the Pantheios libraries
 *
 * Created: 18th July 2005
 * Updated: 9th March 2010
 *
 * Home:    http://pantheios.org/
 *
 * Copyright (c) 2005-2010, Matthew Wilson and Synesis Software
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
 * - Neither the names of Matthew Wilson and Synesis Software nor the names
 *   of any contributors may be used to endorse or promote products derived
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


/** \file pantheios/implicit_link/implicit_link_base_.h
 *
 *  Implicit linking for the Pantheios libraries.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_IMPLICIT_LINK_H_IMPLICIT_LINK_BASE_
#define PANTHEIOS_INCL_PANTHEIOS_IMPLICIT_LINK_H_IMPLICIT_LINK_BASE_

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_IMPLICIT_LINK_H_IMPLICIT_LINK_BASE__MAJOR      1
# define PANTHEIOS_VER_PANTHEIOS_IMPLICIT_LINK_H_IMPLICIT_LINK_BASE__MINOR      7
# define PANTHEIOS_VER_PANTHEIOS_IMPLICIT_LINK_H_IMPLICIT_LINK_BASE__REVISION   1
# define PANTHEIOS_VER_PANTHEIOS_IMPLICIT_LINK_H_IMPLICIT_LINK_BASE__EDIT       18
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

#include <platformstl/platformstl.h>

/* /////////////////////////////////////////////////////////////////////////
 * Exception support
 */

#if defined(__MWERKS__)
# if __option(exceptions)
# else /* ? exceptions */
#  define PANTHEIOS_CF_NOX
# endif /* exceptions */
#elif defined(__WATCOMC__) || \
      defined(__VECTORC)
# if defined(__CPPUNWIND)
# else /* ? __CPPUNWIND */
#  define PANTHEIOS_CF_NOX
# endif /* __CPPUNWIND */
#elif defined(__DMC__) || \
      defined(__INTEL_COMPILER) || \
      defined(_MSC_VER)
# if defined(_CPPUNWIND)
# else /* ? _CPPUNWIND */
#  define PANTHEIOS_CF_NOX
# endif /* _CPPUNWIND */
#endif /* compiler */

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
#   define PANTHEIOS_IMPLICIT_LINK_SUPPORT
#  endif /* compiler */
# endif /* compiler */

#if defined(PANTHEIOS_IMPLICIT_LINK_SUPPORT) && \
    defined(PANTHEIOS_NO_IMPLICIT_LINK)
# undef PANTHEIOS_IMPLICIT_LINK_SUPPORT
#endif /* PANTHEIOS_IMPLICIT_LINK_SUPPORT && PANTHEIOS_NO_IMPLICIT_LINK */

# if defined(PANTHEIOS_IMPLICIT_LINK_SUPPORT)

  /* prefix */

#  define PANTHEIOS_IMPL_LINK_PREFIX

  /* library basename */

#  define PANTHEIOS_IMPL_LINK_LIBRARY_BASENAME          "pantheios"

  /* major version */

#  define PANTHEIOS_IMPL_LINK_MAJOR_VERSION             "." STLSOFT_STRINGIZE(PANTHEIOS_VER_MAJOR)

  /* module name */

   /* This is not defined here. Each module uses PANTHEIOS_IMPL_LINK_LIBRARY_NAME_() as base for generating its name */

  /* compiler tag */

#  if defined(__BORLANDC__)
#   if __BORLANDC__ == 0x0550
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "bc55"
#   elif (__BORLANDC__ == 0x0551)
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "bc551"
#   elif (__BORLANDC__ == 0x0560)
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "bc56"
#   elif (__BORLANDC__ == 0x0564)
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "bc564"
#   elif (__BORLANDC__ == 0x0582)
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "bc582"
#   elif (0x0590 == (__BORLANDC__ & 0xfff0))
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "bc59x"
#   elif (0x0610 == (__BORLANDC__ & 0xfff0))
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "bc61x"
#   else /* ? __BORLANDC__ */
#    error Unrecognised value of __BORLANDC__
#   endif /* __BORLANDC__ */

/*
#  elif defined(__DMC__)
#   define PANTHEIOS_IMPL_LINK_COMPILER_NAME            "dm"
 */

#  elif defined(__INTEL_COMPILER)
#   if __INTEL_COMPILER == 600
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "icl6"
#   elif __INTEL_COMPILER == 700
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "icl7"
#   elif __INTEL_COMPILER == 800
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "icl8"
#   elif __INTEL_COMPILER == 900
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "icl9"
#   elif __INTEL_COMPILER == 1000
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "icl10"
#   elif __INTEL_COMPILER == 1100
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "icl11"
#   else /* ? __INTEL_COMPILER */
#    error Intel C/C++ version not supported
#   endif /* __INTEL_COMPILER */

#  elif defined(__MWERKS__)
#   if ((__MWERKS__ & 0xFF00) == 0x2400)
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "cw7"
#   elif ((__MWERKS__ & 0xFF00) == 0x3000)
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "cw8"
#   elif ((__MWERKS__ & 0xFF00) == 0x3200)
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "cw9"
#   else /* ? __MWERKS__ */
#    error CodeWarrior version not supported
#   endif /* __MWERKS__ */

#  elif defined(_MSC_VER)
#   if _MSC_VER == 1000
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "vc4"
#   elif _MSC_VER == 1020
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "vc42"
#   elif _MSC_VER == 1100
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "vc5"
#   elif _MSC_VER == 1200
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "vc6"
#   elif _MSC_VER == 1300
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "vc7"
#   elif _MSC_VER == 1310
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "vc71"
#   elif _MSC_VER == 1400
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "vc8"
#   elif _MSC_VER == 1500
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "vc9"
#   elif _MSC_VER == 1600
#    define PANTHEIOS_IMPL_LINK_COMPILER_NAME           "vc10"
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
#   define PANTHEIOS_IMPL_LINK_OS_TAG                   ".unix"
#  endif /* pseudo UNIX */

#  if !defined(PANTHEIOS_IMPL_LINK_OS_TAG)
#   define PANTHEIOS_IMPL_LINK_OS_TAG                   ""
#  endif /* !PANTHEIOS_IMPL_LINK_OS_TAG */


  /* architecture tag */

#  if defined(PLATFORMSTL_ARCH_IS_X86)
#   define PANTHEIOS_IMPL_LINK_ARCH_TAG                 ""
#  elif defined(PLATFORMSTL_ARCH_IS_X64)
#   define PANTHEIOS_IMPL_LINK_ARCH_TAG                 ".x64"
#  elif defined(PLATFORMSTL_ARCH_IS_IA64)
#   define PANTHEIOS_IMPL_LINK_ARCH_TAG                 ".ia64"
#  endif /* arch */

#  if !defined(PANTHEIOS_IMPL_LINK_ARCH_TAG)
#   define PANTHEIOS_IMPL_LINK_ARCH_TAG                 ""
#  endif /* !PANTHEIOS_IMPL_LINK_ARCH_TAG */


  /* encoding tag */

#  if defined(PANTHEIOS_USE_WIDE_STRINGS)
#   define PANTHEIOS_IMPL_LINK_ENCODING_TAG             ".widestring"
#  else /* ? PANTHEIOS_USE_WIDE_STRINGS */
#   define PANTHEIOS_IMPL_LINK_ENCODING_TAG             ""
#  endif /* PANTHEIOS_USE_WIDE_STRINGS */


  /* threading tag */

#  if defined(__MT__) || \
      defined(_REENTRANT) || \
      defined(_MT)
#   if defined(_DLL) || \
       defined(__DLL)
#    define PANTHEIOS_IMPL_LINK_THREADING_TAG           ".dll"
#   else /* ? dll */
#    define PANTHEIOS_IMPL_LINK_THREADING_TAG           ".mt"
#   endif /* dll */
#  else /* ? mt */
#    define PANTHEIOS_IMPL_LINK_THREADING_TAG           ""
#  endif /* mt */


  /* NoX */

#  if defined(PANTHEIOS_CF_NOX)
#   define PANTHEIOS_IMPL_LINK_NOX_TYPE                 ".nox"
#  else /* ? PANTHEIOS_CF_NOX */
#   define PANTHEIOS_IMPL_LINK_NOX_TYPE                 ""
#  endif /* PANTHEIOS_CF_NOX */


  /* debug tag */

#  if !defined(NDEBUG) && \
      defined(_DEBUG)
#   define PANTHEIOS_IMPL_LINK_DEBUG_TAG                ".debug"
#  else /* ? debug */
#   define PANTHEIOS_IMPL_LINK_DEBUG_TAG                ""
#  endif /* debug */


  /* suffix */

#  define PANTHEIOS_IMPL_LINK_SUFFIX                    ".lib"


   /* Library name is:
    *
    * [lib]<library-basename>.<major-version>.<module-name>.<compiler-name>[.<os-arch-tag>][.<char-encoding-tag>][.<threading-tag>][.<nox-tag>][.<debug-tag>].{a|lib}
    */

#  define PANTHEIOS_IMPL_LINK_LIBRARY_NAME_(name)       PANTHEIOS_IMPL_LINK_PREFIX \
                                                        PANTHEIOS_IMPL_LINK_LIBRARY_BASENAME \
                                                        PANTHEIOS_IMPL_LINK_MAJOR_VERSION \
                                                        "." name \
                                                        "." PANTHEIOS_IMPL_LINK_COMPILER_NAME \
                                                        PANTHEIOS_IMPL_LINK_OS_TAG \
                                                        PANTHEIOS_IMPL_LINK_ARCH_TAG \
                                                        PANTHEIOS_IMPL_LINK_ENCODING_TAG \
                                                        PANTHEIOS_IMPL_LINK_THREADING_TAG \
                                                        PANTHEIOS_IMPL_LINK_NOX_TYPE \
                                                        PANTHEIOS_IMPL_LINK_DEBUG_TAG \
                                                        PANTHEIOS_IMPL_LINK_SUFFIX

#  pragma message("Implicit linking to Pantheios libraries: (" PANTHEIOS_IMPL_LINK_LIBRARY_NAME_("$(XXXX)") "")

# endif /* PANTHEIOS_IMPLICIT_LINK_SUPPORT */

#endif /* Win-32 || Win-64 */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_IMPLICIT_LINK_H_IMPLICIT_LINK_BASE_ */

/* ///////////////////////////// end of file //////////////////////////// */
