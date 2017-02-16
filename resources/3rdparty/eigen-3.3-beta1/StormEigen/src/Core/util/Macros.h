// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2008-2015 Gael Guennebaud <gael.guennebaud@inria.fr>
// Copyright (C) 2006-2008 Benoit Jacob <jacob.benoit.1@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef STORMEIGEN_MACROS_H
#define STORMEIGEN_MACROS_H

#define STORMEIGEN_WORLD_VERSION 3
#define STORMEIGEN_MAJOR_VERSION 2
#define STORMEIGEN_MINOR_VERSION 92

#define STORMEIGEN_VERSION_AT_LEAST(x,y,z) (STORMEIGEN_WORLD_VERSION>x || (STORMEIGEN_WORLD_VERSION>=x && \
                                      (STORMEIGEN_MAJOR_VERSION>y || (STORMEIGEN_MAJOR_VERSION>=y && \
                                                                 STORMEIGEN_MINOR_VERSION>=z))))

// Compiler identification, STORMEIGEN_COMP_*

/// \internal STORMEIGEN_COMP_GNUC set to 1 for all compilers compatible with GCC
#ifdef __GNUC__
  #define STORMEIGEN_COMP_GNUC 1
#else
  #define STORMEIGEN_COMP_GNUC 0
#endif

/// \internal STORMEIGEN_COMP_CLANG set to 1 if the compiler is clang (alias for __clang__)
#if defined(__clang__)
  #define STORMEIGEN_COMP_CLANG 1
#else
  #define STORMEIGEN_COMP_CLANG 0
#endif


/// \internal STORMEIGEN_COMP_LLVM set to 1 if the compiler backend is llvm
#if defined(__llvm__)
  #define STORMEIGEN_COMP_LLVM 1
#else
  #define STORMEIGEN_COMP_LLVM 0
#endif

/// \internal STORMEIGEN_COMP_ICC set to __INTEL_COMPILER if the compiler is Intel compiler, 0 otherwise
#if defined(__INTEL_COMPILER)
  #define STORMEIGEN_COMP_ICC __INTEL_COMPILER
#else
  #define STORMEIGEN_COMP_ICC 0
#endif

/// \internal STORMEIGEN_COMP_MINGW set to 1 if the compiler is mingw
#if defined(__MINGW32__)
  #define STORMEIGEN_COMP_MINGW 1
#else
  #define STORMEIGEN_COMP_MINGW 0
#endif

/// \internal STORMEIGEN_COMP_SUNCC set to 1 if the compiler is Solaris Studio
#if defined(__SUNPRO_CC)
  #define STORMEIGEN_COMP_SUNCC 1
#else
  #define STORMEIGEN_COMP_SUNCC 0
#endif

/// \internal STORMEIGEN_COMP_MSVC set to _MSC_VER if the compiler is Microsoft Visual C++, 0 otherwise.
#if defined(_MSC_VER)
  #define STORMEIGEN_COMP_MSVC _MSC_VER
#else
  #define STORMEIGEN_COMP_MSVC 0
#endif

/// \internal STORMEIGEN_COMP_MSVC_STRICT set to 1 if the compiler is really Microsoft Visual C++ and not ,e.g., ICC
#if STORMEIGEN_COMP_MSVC && !(STORMEIGEN_COMP_ICC)
  #define STORMEIGEN_COMP_MSVC_STRICT _MSC_VER
#else
  #define STORMEIGEN_COMP_MSVC_STRICT 0
#endif

/// \internal STORMEIGEN_COMP_IBM set to 1 if the compiler is IBM XL C++
#if defined(__IBMCPP__) || defined(__xlc__)
  #define STORMEIGEN_COMP_IBM 1
#else
  #define STORMEIGEN_COMP_IBM 0
#endif

/// \internal STORMEIGEN_COMP_PGI set to 1 if the compiler is Portland Group Compiler
#if defined(__PGI)
  #define STORMEIGEN_COMP_PGI 1
#else
  #define STORMEIGEN_COMP_PGI 0
#endif

/// \internal STORMEIGEN_COMP_ARM set to 1 if the compiler is ARM Compiler
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
  #define STORMEIGEN_COMP_ARM 1
#else
  #define STORMEIGEN_COMP_ARM 0
#endif


/// \internal STORMEIGEN_GNUC_STRICT set to 1 if the compiler is really GCC and not a compatible compiler (e.g., ICC, clang, mingw, etc.)
#if STORMEIGEN_COMP_GNUC && !(STORMEIGEN_COMP_CLANG || STORMEIGEN_COMP_ICC || STORMEIGEN_COMP_MINGW || STORMEIGEN_COMP_PGI || STORMEIGEN_COMP_IBM || STORMEIGEN_COMP_ARM )
  #define STORMEIGEN_COMP_GNUC_STRICT 1
#else
  #define STORMEIGEN_COMP_GNUC_STRICT 0
#endif


#if STORMEIGEN_COMP_GNUC
  #define STORMEIGEN_GNUC_AT_LEAST(x,y) ((__GNUC__==x && __GNUC_MINOR__>=y) || __GNUC__>x)
  #define STORMEIGEN_GNUC_AT_MOST(x,y)  ((__GNUC__==x && __GNUC_MINOR__<=y) || __GNUC__<x)
  #define STORMEIGEN_GNUC_AT(x,y)       ( __GNUC__==x && __GNUC_MINOR__==y )
#else
  #define STORMEIGEN_GNUC_AT_LEAST(x,y) 0
  #define STORMEIGEN_GNUC_AT_MOST(x,y)  0
  #define STORMEIGEN_GNUC_AT(x,y)       0
#endif

// FIXME: could probably be removed as we do not support gcc 3.x anymore
#if STORMEIGEN_COMP_GNUC && (__GNUC__ <= 3)
#define STORMEIGEN_GCC3_OR_OLDER 1
#else
#define STORMEIGEN_GCC3_OR_OLDER 0
#endif


// Architecture identification, STORMEIGEN_ARCH_*

#if defined(__x86_64__) || defined(_M_X64) || defined(__amd64)
  #define STORMEIGEN_ARCH_x86_64 1
#else
  #define STORMEIGEN_ARCH_x86_64 0
#endif

#if defined(__i386__) || defined(_M_IX86) || defined(_X86_) || defined(__i386)
  #define STORMEIGEN_ARCH_i386 1
#else
  #define STORMEIGEN_ARCH_i386 0
#endif

#if STORMEIGEN_ARCH_x86_64 || STORMEIGEN_ARCH_i386
  #define STORMEIGEN_ARCH_i386_OR_x86_64 1
#else
  #define STORMEIGEN_ARCH_i386_OR_x86_64 0
#endif

/// \internal STORMEIGEN_ARCH_ARM set to 1 if the architecture is ARM
#if defined(__arm__)
  #define STORMEIGEN_ARCH_ARM 1
#else
  #define STORMEIGEN_ARCH_ARM 0
#endif

/// \internal STORMEIGEN_ARCH_ARM64 set to 1 if the architecture is ARM64
#if defined(__aarch64__)
  #define STORMEIGEN_ARCH_ARM64 1
#else
  #define STORMEIGEN_ARCH_ARM64 0
#endif

#if STORMEIGEN_ARCH_ARM || STORMEIGEN_ARCH_ARM64
  #define STORMEIGEN_ARCH_ARM_OR_ARM64 1
#else
  #define STORMEIGEN_ARCH_ARM_OR_ARM64 0
#endif

/// \internal STORMEIGEN_ARCH_MIPS set to 1 if the architecture is MIPS
#if defined(__mips__) || defined(__mips)
  #define STORMEIGEN_ARCH_MIPS 1
#else
  #define STORMEIGEN_ARCH_MIPS 0
#endif

/// \internal STORMEIGEN_ARCH_SPARC set to 1 if the architecture is SPARC
#if defined(__sparc__) || defined(__sparc)
  #define STORMEIGEN_ARCH_SPARC 1
#else
  #define STORMEIGEN_ARCH_SPARC 0
#endif

/// \internal STORMEIGEN_ARCH_IA64 set to 1 if the architecture is Intel Itanium
#if defined(__ia64__)
  #define STORMEIGEN_ARCH_IA64 1
#else
  #define STORMEIGEN_ARCH_IA64 0
#endif

/// \internal STORMEIGEN_ARCH_PPC set to 1 if the architecture is PowerPC
#if defined(__powerpc__) || defined(__ppc__) || defined(_M_PPC)
  #define STORMEIGEN_ARCH_PPC 1
#else
  #define STORMEIGEN_ARCH_PPC 0
#endif



// Operating system identification, STORMEIGEN_OS_*

/// \internal STORMEIGEN_OS_UNIX set to 1 if the OS is a unix variant
#if defined(__unix__) || defined(__unix)
  #define STORMEIGEN_OS_UNIX 1
#else
  #define STORMEIGEN_OS_UNIX 0
#endif

/// \internal STORMEIGEN_OS_LINUX set to 1 if the OS is based on Linux kernel
#if defined(__linux__)
  #define STORMEIGEN_OS_LINUX 1
#else
  #define STORMEIGEN_OS_LINUX 0
#endif

/// \internal STORMEIGEN_OS_ANDROID set to 1 if the OS is Android
// note: ANDROID is defined when using ndk_build, __ANDROID__ is defined when using a standalone toolchain.
#if defined(__ANDROID__) || defined(ANDROID)
  #define STORMEIGEN_OS_ANDROID 1
#else
  #define STORMEIGEN_OS_ANDROID 0
#endif

/// \internal STORMEIGEN_OS_GNULINUX set to 1 if the OS is GNU Linux and not Linux-based OS (e.g., not android)
#if defined(__gnu_linux__) && !(STORMEIGEN_OS_ANDROID)
  #define STORMEIGEN_OS_GNULINUX 1
#else
  #define STORMEIGEN_OS_GNULINUX 0
#endif

/// \internal STORMEIGEN_OS_BSD set to 1 if the OS is a BSD variant
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__)
  #define STORMEIGEN_OS_BSD 1
#else
  #define STORMEIGEN_OS_BSD 0
#endif

/// \internal STORMEIGEN_OS_MAC set to 1 if the OS is MacOS
#if defined(__APPLE__)
  #define STORMEIGEN_OS_MAC 1
#else
  #define STORMEIGEN_OS_MAC 0
#endif

/// \internal STORMEIGEN_OS_QNX set to 1 if the OS is QNX
#if defined(__QNX__)
  #define STORMEIGEN_OS_QNX 1
#else
  #define STORMEIGEN_OS_QNX 0
#endif

/// \internal STORMEIGEN_OS_WIN set to 1 if the OS is Windows based
#if defined(_WIN32)
  #define STORMEIGEN_OS_WIN 1
#else
  #define STORMEIGEN_OS_WIN 0
#endif

/// \internal STORMEIGEN_OS_WIN64 set to 1 if the OS is Windows 64bits
#if defined(_WIN64)
  #define STORMEIGEN_OS_WIN64 1
#else
  #define STORMEIGEN_OS_WIN64 0
#endif

/// \internal STORMEIGEN_OS_WINCE set to 1 if the OS is Windows CE
#if defined(_WIN32_WCE)
  #define STORMEIGEN_OS_WINCE 1
#else
  #define STORMEIGEN_OS_WINCE 0
#endif

/// \internal STORMEIGEN_OS_CYGWIN set to 1 if the OS is Windows/Cygwin
#if defined(__CYGWIN__)
  #define STORMEIGEN_OS_CYGWIN 1
#else
  #define STORMEIGEN_OS_CYGWIN 0
#endif

/// \internal STORMEIGEN_OS_WIN_STRICT set to 1 if the OS is really Windows and not some variants
#if STORMEIGEN_OS_WIN && !( STORMEIGEN_OS_WINCE || STORMEIGEN_OS_CYGWIN )
  #define STORMEIGEN_OS_WIN_STRICT 1
#else
  #define STORMEIGEN_OS_WIN_STRICT 0
#endif

/// \internal STORMEIGEN_OS_SUN set to 1 if the OS is SUN
#if (defined(sun) || defined(__sun)) && !(defined(__SVR4) || defined(__svr4__))
  #define STORMEIGEN_OS_SUN 1
#else
  #define STORMEIGEN_OS_SUN 0
#endif

/// \internal STORMEIGEN_OS_SOLARIS set to 1 if the OS is Solaris
#if (defined(sun) || defined(__sun)) && (defined(__SVR4) || defined(__svr4__))
  #define STORMEIGEN_OS_SOLARIS 1
#else
  #define STORMEIGEN_OS_SOLARIS 0
#endif



#if STORMEIGEN_GNUC_AT_MOST(4,3) && !STORMEIGEN_COMP_CLANG
  // see bug 89
  #define STORMEIGEN_SAFE_TO_USE_STANDARD_ASSERT_MACRO 0
#else
  #define STORMEIGEN_SAFE_TO_USE_STANDARD_ASSERT_MACRO 1
#endif

// This macro can be used to prevent from macro expansion, e.g.:
//   std::max STORMEIGEN_NOT_A_MACRO(a,b)
#define STORMEIGEN_NOT_A_MACRO

#ifdef STORMEIGEN_DEFAULT_TO_ROW_MAJOR
#define STORMEIGEN_DEFAULT_MATRIX_STORAGE_ORDER_OPTION StormEigen::RowMajor
#else
#define STORMEIGEN_DEFAULT_MATRIX_STORAGE_ORDER_OPTION StormEigen::ColMajor
#endif

#ifndef STORMEIGEN_DEFAULT_DENSE_INDEX_TYPE
#define STORMEIGEN_DEFAULT_DENSE_INDEX_TYPE std::ptrdiff_t
#endif

// Cross compiler wrapper around LLVM's __has_builtin
#ifdef __has_builtin
#  define STORMEIGEN_HAS_BUILTIN(x) __has_builtin(x)
#else
#  define STORMEIGEN_HAS_BUILTIN(x) 0
#endif

// A Clang feature extension to determine compiler features.
// We use it to determine 'cxx_rvalue_references'
#ifndef __has_feature
# define __has_feature(x) 0
#endif

// Do we support r-value references?
#if (__has_feature(cxx_rvalue_references) || \
    (defined(__cplusplus) && __cplusplus >= 201103L) || \
     defined(__GXX_EXPERIMENTAL_CXX0X__) || \
    (STORMEIGEN_COMP_MSVC >= 1600))
  #define STORMEIGEN_HAVE_RVALUE_REFERENCES
#endif

// Does the compiler support C99?
#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901))       \
  || (defined(__GNUC__) && defined(_GLIBCXX_USE_C99)) \
  || (defined(_LIBCPP_VERSION) && !defined(_MSC_VER))
#define STORMEIGEN_HAS_C99_MATH 1
#endif

// Does the compiler support result_of?
#if (__has_feature(cxx_lambdas) || (defined(__cplusplus) && __cplusplus >= 201103L))
#define STORMEIGEN_HAS_STD_RESULT_OF 1
#endif

// Does the compiler support variadic templates?
#if __cplusplus > 199711L
#define STORMEIGEN_HAS_VARIADIC_TEMPLATES 1
#endif

// Does the compiler support const expressions?
#ifdef __CUDACC__
// Const expressions are supported provided that c++11 is enabled and we're using nvcc 7.5 or above
#if defined(__CUDACC_VER__) &&  __CUDACC_VER__ >= 70500 && __cplusplus > 199711L
  #define STORMEIGEN_HAS_CONSTEXPR 1
#endif
#elif (defined(__cplusplus) && __cplusplus >= 201402L) || \
    STORMEIGEN_GNUC_AT_LEAST(4,8)
#define STORMEIGEN_HAS_CONSTEXPR 1
#endif

// Does the compiler support C++11 math?
// Let's be conservative and enable the default C++11 implementation only if we are sure it exists
#ifndef STORMEIGEN_HAS_CXX11_MATH
  #if (__cplusplus > 201103L) || (__cplusplus >= 201103L) && (STORMEIGEN_COMP_GNUC_STRICT || STORMEIGEN_COMP_CLANG || STORMEIGEN_COMP_MSVC || STORMEIGEN_COMP_ICC)  \
      && (STORMEIGEN_ARCH_i386_OR_x86_64) && (STORMEIGEN_OS_GNULINUX || STORMEIGEN_OS_WIN_STRICT || STORMEIGEN_OS_MAC)
    #define STORMEIGEN_HAS_CXX11_MATH 1
  #else
    #define STORMEIGEN_HAS_CXX11_MATH 0
  #endif
#endif

// Does the compiler support proper C++11 containers?
#ifndef STORMEIGEN_HAS_CXX11_CONTAINERS
  #if    (__cplusplus > 201103L) \
      || ((__cplusplus >= 201103L) && (STORMEIGEN_COMP_GNUC_STRICT || STORMEIGEN_COMP_CLANG || STORMEIGEN_COMP_ICC>=1400)) \
      || STORMEIGEN_COMP_MSVC >= 1900
    #define STORMEIGEN_HAS_CXX11_CONTAINERS 1
  #else
    #define STORMEIGEN_HAS_CXX11_CONTAINERS 0
  #endif
#endif

// Does the compiler support C++11 noexcept?
#ifndef STORMEIGEN_HAS_CXX11_NOEXCEPT
  #if    (__cplusplus > 201103L) \
      || ((__cplusplus >= 201103L) && (STORMEIGEN_COMP_GNUC_STRICT || STORMEIGEN_COMP_CLANG || STORMEIGEN_COMP_ICC>=1400)) \
      || STORMEIGEN_COMP_MSVC >= 1900
    #define STORMEIGEN_HAS_CXX11_NOEXCEPT 1
  #else
    #define STORMEIGEN_HAS_CXX11_NOEXCEPT 0
  #endif
#endif

/** Allows to disable some optimizations which might affect the accuracy of the result.
  * Such optimization are enabled by default, and set STORMEIGEN_FAST_MATH to 0 to disable them.
  * They currently include:
  *   - single precision ArrayBase::sin() and ArrayBase::cos() for SSE and AVX vectorization.
  */
#ifndef STORMEIGEN_FAST_MATH
#define STORMEIGEN_FAST_MATH 1
#endif

#define STORMEIGEN_DEBUG_VAR(x) std::cerr << #x << " = " << x << std::endl;

// concatenate two tokens
#define STORMEIGEN_CAT2(a,b) a ## b
#define STORMEIGEN_CAT(a,b) STORMEIGEN_CAT2(a,b)

// convert a token to a string
#define STORMEIGEN_MAKESTRING2(a) #a
#define STORMEIGEN_MAKESTRING(a) STORMEIGEN_MAKESTRING2(a)

// STORMEIGEN_STRONG_INLINE is a stronger version of the inline, using __forceinline on MSVC,
// but it still doesn't use GCC's always_inline. This is useful in (common) situations where MSVC needs forceinline
// but GCC is still doing fine with just inline.
#if STORMEIGEN_COMP_MSVC || STORMEIGEN_COMP_ICC
#define STORMEIGEN_STRONG_INLINE __forceinline
#else
#define STORMEIGEN_STRONG_INLINE inline
#endif

// STORMEIGEN_ALWAYS_INLINE is the stronget, it has the effect of making the function inline and adding every possible
// attribute to maximize inlining. This should only be used when really necessary: in particular,
// it uses __attribute__((always_inline)) on GCC, which most of the time is useless and can severely harm compile times.
// FIXME with the always_inline attribute,
// gcc 3.4.x reports the following compilation error:
//   Eval.h:91: sorry, unimplemented: inlining failed in call to 'const StormEigen::Eval<Derived> StormEigen::MatrixBase<Scalar, Derived>::eval() const'
//    : function body not available
#if STORMEIGEN_GNUC_AT_LEAST(4,0)
#define STORMEIGEN_ALWAYS_INLINE __attribute__((always_inline)) inline
#else
#define STORMEIGEN_ALWAYS_INLINE STORMEIGEN_STRONG_INLINE
#endif

#if STORMEIGEN_COMP_GNUC
#define STORMEIGEN_DONT_INLINE __attribute__((noinline))
#elif STORMEIGEN_COMP_MSVC
#define STORMEIGEN_DONT_INLINE __declspec(noinline)
#else
#define STORMEIGEN_DONT_INLINE
#endif

#if STORMEIGEN_COMP_GNUC
#define STORMEIGEN_PERMISSIVE_EXPR __extension__
#else
#define STORMEIGEN_PERMISSIVE_EXPR
#endif

// this macro allows to get rid of linking errors about multiply defined functions.
//  - static is not very good because it prevents definitions from different object files to be merged.
//           So static causes the resulting linked executable to be bloated with multiple copies of the same function.
//  - inline is not perfect either as it unwantedly hints the compiler toward inlining the function.
#define STORMEIGEN_DECLARE_FUNCTION_ALLOWING_MULTIPLE_DEFINITIONS
#define STORMEIGEN_DEFINE_FUNCTION_ALLOWING_MULTIPLE_DEFINITIONS inline

#ifdef NDEBUG
# ifndef STORMEIGEN_NO_DEBUG
#  define STORMEIGEN_NO_DEBUG
# endif
#endif

// eigen_plain_assert is where we implement the workaround for the assert() bug in GCC <= 4.3, see bug 89
#ifdef STORMEIGEN_NO_DEBUG
  #define eigen_plain_assert(x)
#else
  #if STORMEIGEN_SAFE_TO_USE_STANDARD_ASSERT_MACRO
    namespace StormEigen {
    namespace internal {
    inline bool copy_bool(bool b) { return b; }
    }
    }
    #define eigen_plain_assert(x) assert(x)
  #else
    // work around bug 89
    #include <cstdlib>   // for abort
    #include <iostream>  // for std::cerr

    namespace StormEigen {
    namespace internal {
    // trivial function copying a bool. Must be STORMEIGEN_DONT_INLINE, so we implement it after including Eigen headers.
    // see bug 89.
    namespace {
    STORMEIGEN_DONT_INLINE bool copy_bool(bool b) { return b; }
    }
    inline void assert_fail(const char *condition, const char *function, const char *file, int line)
    {
      std::cerr << "assertion failed: " << condition << " in function " << function << " at " << file << ":" << line << std::endl;
      abort();
    }
    }
    }
    #define eigen_plain_assert(x) \
      do { \
        if(!StormEigen::internal::copy_bool(x)) \
          StormEigen::internal::assert_fail(STORMEIGEN_MAKESTRING(x), __PRETTY_FUNCTION__, __FILE__, __LINE__); \
      } while(false)
  #endif
#endif

// eigen_assert can be overridden
#ifndef eigen_assert
#define eigen_assert(x) eigen_plain_assert(x)
#endif

#ifdef STORMEIGEN_INTERNAL_DEBUGGING
#define eigen_internal_assert(x) eigen_assert(x)
#else
#define eigen_internal_assert(x)
#endif

#ifdef STORMEIGEN_NO_DEBUG
#define STORMEIGEN_ONLY_USED_FOR_DEBUG(x) STORMEIGEN_UNUSED_VARIABLE(x)
#else
#define STORMEIGEN_ONLY_USED_FOR_DEBUG(x)
#endif

#ifndef STORMEIGEN_NO_DEPRECATED_WARNING
  #if STORMEIGEN_COMP_GNUC
    #define STORMEIGEN_DEPRECATED __attribute__((deprecated))
  #elif STORMEIGEN_COMP_MSVC
    #define STORMEIGEN_DEPRECATED __declspec(deprecated)
  #else
    #define STORMEIGEN_DEPRECATED
  #endif
#else
  #define STORMEIGEN_DEPRECATED
#endif

#if STORMEIGEN_COMP_GNUC
#define STORMEIGEN_UNUSED __attribute__((unused))
#else
#define STORMEIGEN_UNUSED
#endif

// Suppresses 'unused variable' warnings.
namespace StormEigen {
  namespace internal {
    template<typename T> STORMEIGEN_DEVICE_FUNC void ignore_unused_variable(const T&) {}
  }
}
#define STORMEIGEN_UNUSED_VARIABLE(var) StormEigen::internal::ignore_unused_variable(var);

#if !defined(STORMEIGEN_ASM_COMMENT)
  #if STORMEIGEN_COMP_GNUC && (STORMEIGEN_ARCH_i386_OR_x86_64 || STORMEIGEN_ARCH_ARM_OR_ARM64)
    #define STORMEIGEN_ASM_COMMENT(X)  __asm__("#" X)
  #else
    #define STORMEIGEN_ASM_COMMENT(X)
  #endif
#endif


//------------------------------------------------------------------------------------------
// Static and dynamic alignment control
// 
// The main purpose of this section is to define STORMEIGEN_MAX_ALIGN_BYTES and STORMEIGEN_MAX_STATIC_ALIGN_BYTES
// as the maximal boundary in bytes on which dynamically and statically allocated data may be alignment respectively.
// The values of STORMEIGEN_MAX_ALIGN_BYTES and STORMEIGEN_MAX_STATIC_ALIGN_BYTES can be specified by the user. If not,
// a default value is automatically computed based on architecture, compiler, and OS.
// 
// This section also defines macros STORMEIGEN_ALIGN_TO_BOUNDARY(N) and the shortcuts STORMEIGEN_ALIGN{8,16,32,_MAX}
// to be used to declare statically aligned buffers.
//------------------------------------------------------------------------------------------


/* STORMEIGEN_ALIGN_TO_BOUNDARY(n) forces data to be n-byte aligned. This is used to satisfy SIMD requirements.
 * However, we do that EVEN if vectorization (STORMEIGEN_VECTORIZE) is disabled,
 * so that vectorization doesn't affect binary compatibility.
 *
 * If we made alignment depend on whether or not STORMEIGEN_VECTORIZE is defined, it would be impossible to link
 * vectorized and non-vectorized code.
 */
#if (defined __CUDACC__)
  #define STORMEIGEN_ALIGN_TO_BOUNDARY(n) __align__(n)
#elif STORMEIGEN_COMP_GNUC || STORMEIGEN_COMP_PGI || STORMEIGEN_COMP_IBM || STORMEIGEN_COMP_ARM
  #define STORMEIGEN_ALIGN_TO_BOUNDARY(n) __attribute__((aligned(n)))
#elif STORMEIGEN_COMP_MSVC
  #define STORMEIGEN_ALIGN_TO_BOUNDARY(n) __declspec(align(n))
#elif STORMEIGEN_COMP_SUNCC
  // FIXME not sure about this one:
  #define STORMEIGEN_ALIGN_TO_BOUNDARY(n) __attribute__((aligned(n)))
#else
  #error Please tell me what is the equivalent of __attribute__((aligned(n))) for your compiler
#endif

// If the user explicitly disable vectorization, then we also disable alignment
#if defined(STORMEIGEN_DONT_VECTORIZE)
  #define STORMEIGEN_IDEAL_MAX_ALIGN_BYTES 0
#elif defined(__AVX__)
  // 32 bytes static alignmeent is preferred only if really required
  #define STORMEIGEN_IDEAL_MAX_ALIGN_BYTES 32
#else
  #define STORMEIGEN_IDEAL_MAX_ALIGN_BYTES 16
#endif


// STORMEIGEN_MIN_ALIGN_BYTES defines the minimal value for which the notion of explicit alignment makes sense
#define STORMEIGEN_MIN_ALIGN_BYTES 16

// Defined the boundary (in bytes) on which the data needs to be aligned. Note
// that unless STORMEIGEN_ALIGN is defined and not equal to 0, the data may not be
// aligned at all regardless of the value of this #define.

#if (defined(STORMEIGEN_DONT_ALIGN_STATICALLY) || defined(STORMEIGEN_DONT_ALIGN))  && defined(STORMEIGEN_MAX_STATIC_ALIGN_BYTES) && STORMEIGEN_MAX_STATIC_ALIGN_BYTES>0
#error STORMEIGEN_MAX_STATIC_ALIGN_BYTES and STORMEIGEN_DONT_ALIGN[_STATICALLY] are both defined with STORMEIGEN_MAX_STATIC_ALIGN_BYTES!=0. Use STORMEIGEN_MAX_STATIC_ALIGN_BYTES=0 as a synonym of STORMEIGEN_DONT_ALIGN_STATICALLY.
#endif

// STORMEIGEN_DONT_ALIGN_STATICALLY and STORMEIGEN_DONT_ALIGN are deprectated
// They imply STORMEIGEN_MAX_STATIC_ALIGN_BYTES=0
#if defined(STORMEIGEN_DONT_ALIGN_STATICALLY) || defined(STORMEIGEN_DONT_ALIGN)
  #ifdef STORMEIGEN_MAX_STATIC_ALIGN_BYTES
    #undef STORMEIGEN_MAX_STATIC_ALIGN_BYTES
  #endif
  #define STORMEIGEN_MAX_STATIC_ALIGN_BYTES 0
#endif

#ifndef STORMEIGEN_MAX_STATIC_ALIGN_BYTES

  // Try to automatically guess what is the best default value for STORMEIGEN_MAX_STATIC_ALIGN_BYTES
  
  // 16 byte alignment is only useful for vectorization. Since it affects the ABI, we need to enable
  // 16 byte alignment on all platforms where vectorization might be enabled. In theory we could always
  // enable alignment, but it can be a cause of problems on some platforms, so we just disable it in
  // certain common platform (compiler+architecture combinations) to avoid these problems.
  // Only static alignment is really problematic (relies on nonstandard compiler extensions),
  // try to keep heap alignment even when we have to disable static alignment.
  #if STORMEIGEN_COMP_GNUC && !(STORMEIGEN_ARCH_i386_OR_x86_64 || STORMEIGEN_ARCH_ARM_OR_ARM64 || STORMEIGEN_ARCH_PPC || STORMEIGEN_ARCH_IA64)
  #define STORMEIGEN_GCC_AND_ARCH_DOESNT_WANT_STACK_ALIGNMENT 1
  #elif STORMEIGEN_ARCH_ARM_OR_ARM64 && STORMEIGEN_COMP_GNUC_STRICT && STORMEIGEN_GNUC_AT_MOST(4, 6)
  // Old versions of GCC on ARM, at least 4.4, were once seen to have buggy static alignment support.
  // Not sure which version fixed it, hopefully it doesn't affect 4.7, which is still somewhat in use.
  // 4.8 and newer seem definitely unaffected.
  #define STORMEIGEN_GCC_AND_ARCH_DOESNT_WANT_STACK_ALIGNMENT 1
  #else
  #define STORMEIGEN_GCC_AND_ARCH_DOESNT_WANT_STACK_ALIGNMENT 0
  #endif

  // static alignment is completely disabled with GCC 3, Sun Studio, and QCC/QNX
  #if !STORMEIGEN_GCC_AND_ARCH_DOESNT_WANT_STACK_ALIGNMENT \
  && !STORMEIGEN_GCC3_OR_OLDER \
  && !STORMEIGEN_COMP_SUNCC \
  && !STORMEIGEN_OS_QNX
    #define STORMEIGEN_ARCH_WANTS_STACK_ALIGNMENT 1
  #else
    #define STORMEIGEN_ARCH_WANTS_STACK_ALIGNMENT 0
  #endif
  
  #if STORMEIGEN_ARCH_WANTS_STACK_ALIGNMENT
    #define STORMEIGEN_MAX_STATIC_ALIGN_BYTES STORMEIGEN_IDEAL_MAX_ALIGN_BYTES
  #else
    #define STORMEIGEN_MAX_STATIC_ALIGN_BYTES 0
  #endif
  
#endif

// If STORMEIGEN_MAX_ALIGN_BYTES is defined, then it is considered as an upper bound for STORMEIGEN_MAX_ALIGN_BYTES
#if defined(STORMEIGEN_MAX_ALIGN_BYTES) && STORMEIGEN_MAX_ALIGN_BYTES<STORMEIGEN_MAX_STATIC_ALIGN_BYTES
#undef STORMEIGEN_MAX_STATIC_ALIGN_BYTES
#define STORMEIGEN_MAX_STATIC_ALIGN_BYTES STORMEIGEN_MAX_ALIGN_BYTES
#endif

#if STORMEIGEN_MAX_STATIC_ALIGN_BYTES==0 && !defined(STORMEIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT)
  #define STORMEIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#endif

// At this stage, STORMEIGEN_MAX_STATIC_ALIGN_BYTES>0 is the true test whether we want to align arrays on the stack or not.
// It takes into account both the user choice to explicitly enable/disable alignment (by settting STORMEIGEN_MAX_STATIC_ALIGN_BYTES)
// and the architecture config (STORMEIGEN_ARCH_WANTS_STACK_ALIGNMENT).
// Henceforth, only STORMEIGEN_MAX_STATIC_ALIGN_BYTES should be used.


// Shortcuts to STORMEIGEN_ALIGN_TO_BOUNDARY
#define STORMEIGEN_ALIGN8  STORMEIGEN_ALIGN_TO_BOUNDARY(8)
#define STORMEIGEN_ALIGN16 STORMEIGEN_ALIGN_TO_BOUNDARY(16)
#define STORMEIGEN_ALIGN32 STORMEIGEN_ALIGN_TO_BOUNDARY(32)
#define STORMEIGEN_ALIGN64 STORMEIGEN_ALIGN_TO_BOUNDARY(64)
#if STORMEIGEN_MAX_STATIC_ALIGN_BYTES>0
#define STORMEIGEN_ALIGN_MAX STORMEIGEN_ALIGN_TO_BOUNDARY(STORMEIGEN_MAX_STATIC_ALIGN_BYTES)
#else
#define STORMEIGEN_ALIGN_MAX
#endif


// Dynamic alignment control

#if defined(STORMEIGEN_DONT_ALIGN) && defined(STORMEIGEN_MAX_ALIGN_BYTES) && STORMEIGEN_MAX_ALIGN_BYTES>0
#error STORMEIGEN_MAX_ALIGN_BYTES and STORMEIGEN_DONT_ALIGN are both defined with STORMEIGEN_MAX_ALIGN_BYTES!=0. Use STORMEIGEN_MAX_ALIGN_BYTES=0 as a synonym of STORMEIGEN_DONT_ALIGN.
#endif

#ifdef STORMEIGEN_DONT_ALIGN
  #ifdef STORMEIGEN_MAX_ALIGN_BYTES
    #undef STORMEIGEN_MAX_ALIGN_BYTES
  #endif
  #define STORMEIGEN_MAX_ALIGN_BYTES 0
#elif !defined(STORMEIGEN_MAX_ALIGN_BYTES)
  #define STORMEIGEN_MAX_ALIGN_BYTES STORMEIGEN_IDEAL_MAX_ALIGN_BYTES
#endif

#if STORMEIGEN_IDEAL_MAX_ALIGN_BYTES > STORMEIGEN_MAX_ALIGN_BYTES
#define STORMEIGEN_DEFAULT_ALIGN_BYTES STORMEIGEN_IDEAL_MAX_ALIGN_BYTES
#else
#define STORMEIGEN_DEFAULT_ALIGN_BYTES STORMEIGEN_MAX_ALIGN_BYTES
#endif

//----------------------------------------------------------------------


#ifdef STORMEIGEN_DONT_USE_RESTRICT_KEYWORD
  #define STORMEIGEN_RESTRICT
#endif
#ifndef STORMEIGEN_RESTRICT
  #define STORMEIGEN_RESTRICT __restrict
#endif

#ifndef STORMEIGEN_STACK_ALLOCATION_LIMIT
// 131072 == 128 KB
#define STORMEIGEN_STACK_ALLOCATION_LIMIT 131072
#endif

#ifndef STORMEIGEN_DEFAULT_IO_FORMAT
#ifdef STORMEIGEN_MAKING_DOCS
// format used in Eigen's documentation
// needed to define it here as escaping characters in CMake add_definition's argument seems very problematic.
#define STORMEIGEN_DEFAULT_IO_FORMAT StormEigen::IOFormat(3, 0, " ", "\n", "", "")
#else
#define STORMEIGEN_DEFAULT_IO_FORMAT StormEigen::IOFormat()
#endif
#endif

// just an empty macro !
#define STORMEIGEN_EMPTY

#if STORMEIGEN_COMP_MSVC_STRICT && STORMEIGEN_COMP_MSVC < 1900 // for older MSVC versions using the base operator is sufficient (cf Bug 1000)
  #define STORMEIGEN_INHERIT_ASSIGNMENT_EQUAL_OPERATOR(Derived) \
    using Base::operator =;
#elif STORMEIGEN_COMP_CLANG // workaround clang bug (see http://forum.kde.org/viewtopic.php?f=74&t=102653)
  #define STORMEIGEN_INHERIT_ASSIGNMENT_EQUAL_OPERATOR(Derived) \
    using Base::operator =; \
    STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE Derived& operator=(const Derived& other) { Base::operator=(other); return *this; } \
    template <typename OtherDerived> \
    STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE Derived& operator=(const DenseBase<OtherDerived>& other) { Base::operator=(other.derived()); return *this; }
#else
  #define STORMEIGEN_INHERIT_ASSIGNMENT_EQUAL_OPERATOR(Derived) \
    using Base::operator =; \
    STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE Derived& operator=(const Derived& other) \
    { \
      Base::operator=(other); \
      return *this; \
    }
#endif


/** \internal
 * \brief Macro to manually inherit assignment operators.
 * This is necessary, because the implicitly defined assignment operator gets deleted when a custom operator= is defined.
 */
#define STORMEIGEN_INHERIT_ASSIGNMENT_OPERATORS(Derived) STORMEIGEN_INHERIT_ASSIGNMENT_EQUAL_OPERATOR(Derived)

/**
* Just a side note. Commenting within defines works only by documenting
* behind the object (via '!<'). Comments cannot be multi-line and thus
* we have these extra long lines. What is confusing doxygen over here is
* that we use '\' and basically have a bunch of typedefs with their
* documentation in a single line.
**/

#define STORMEIGEN_GENERIC_PUBLIC_INTERFACE(Derived) \
  typedef typename StormEigen::internal::traits<Derived>::Scalar Scalar; /*!< \brief Numeric type, e.g. float, double, int or std::complex<float>. */ \
  typedef typename StormEigen::NumTraits<Scalar>::Real RealScalar; /*!< \brief The underlying numeric type for composed scalar types. \details In cases where Scalar is e.g. std::complex<T>, T were corresponding to RealScalar. */ \
  typedef typename Base::CoeffReturnType CoeffReturnType; /*!< \brief The return type for coefficient access. \details Depending on whether the object allows direct coefficient access (e.g. for a MatrixXd), this type is either 'const Scalar&' or simply 'Scalar' for objects that do not allow direct coefficient access. */ \
  typedef typename StormEigen::internal::ref_selector<Derived>::type Nested; \
  typedef typename StormEigen::internal::traits<Derived>::StorageKind StorageKind; \
  typedef typename StormEigen::internal::traits<Derived>::StorageIndex StorageIndex; \
  enum { RowsAtCompileTime = StormEigen::internal::traits<Derived>::RowsAtCompileTime, \
        ColsAtCompileTime = StormEigen::internal::traits<Derived>::ColsAtCompileTime, \
        Flags = StormEigen::internal::traits<Derived>::Flags, \
        SizeAtCompileTime = Base::SizeAtCompileTime, \
        MaxSizeAtCompileTime = Base::MaxSizeAtCompileTime, \
        IsVectorAtCompileTime = Base::IsVectorAtCompileTime }; \
  using Base::derived; \
  using Base::const_cast_derived;


// FIXME Maybe the STORMEIGEN_DENSE_PUBLIC_INTERFACE could be removed as importing PacketScalar is rarely needed
#define STORMEIGEN_DENSE_PUBLIC_INTERFACE(Derived) \
  STORMEIGEN_GENERIC_PUBLIC_INTERFACE(Derived) \
  typedef typename Base::PacketScalar PacketScalar;


#define STORMEIGEN_PLAIN_ENUM_MIN(a,b) (((int)a <= (int)b) ? (int)a : (int)b)
#define STORMEIGEN_PLAIN_ENUM_MAX(a,b) (((int)a >= (int)b) ? (int)a : (int)b)

// STORMEIGEN_SIZE_MIN_PREFER_DYNAMIC gives the min between compile-time sizes. 0 has absolute priority, followed by 1,
// followed by Dynamic, followed by other finite values. The reason for giving Dynamic the priority over
// finite values is that min(3, Dynamic) should be Dynamic, since that could be anything between 0 and 3.
#define STORMEIGEN_SIZE_MIN_PREFER_DYNAMIC(a,b) (((int)a == 0 || (int)b == 0) ? 0 \
                           : ((int)a == 1 || (int)b == 1) ? 1 \
                           : ((int)a == Dynamic || (int)b == Dynamic) ? Dynamic \
                           : ((int)a <= (int)b) ? (int)a : (int)b)

// STORMEIGEN_SIZE_MIN_PREFER_FIXED is a variant of STORMEIGEN_SIZE_MIN_PREFER_DYNAMIC comparing MaxSizes. The difference is that finite values
// now have priority over Dynamic, so that min(3, Dynamic) gives 3. Indeed, whatever the actual value is
// (between 0 and 3), it is not more than 3.
#define STORMEIGEN_SIZE_MIN_PREFER_FIXED(a,b)  (((int)a == 0 || (int)b == 0) ? 0 \
                           : ((int)a == 1 || (int)b == 1) ? 1 \
                           : ((int)a == Dynamic && (int)b == Dynamic) ? Dynamic \
                           : ((int)a == Dynamic) ? (int)b \
                           : ((int)b == Dynamic) ? (int)a \
                           : ((int)a <= (int)b) ? (int)a : (int)b)

// see STORMEIGEN_SIZE_MIN_PREFER_DYNAMIC. No need for a separate variant for MaxSizes here.
#define STORMEIGEN_SIZE_MAX(a,b) (((int)a == Dynamic || (int)b == Dynamic) ? Dynamic \
                           : ((int)a >= (int)b) ? (int)a : (int)b)

#define STORMEIGEN_LOGICAL_XOR(a,b) (((a) || (b)) && !((a) && (b)))

#define STORMEIGEN_IMPLIES(a,b) (!(a) || (b))

#define STORMEIGEN_MAKE_CWISE_BINARY_OP(METHOD,FUNCTOR) \
  template<typename OtherDerived> \
  STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE const CwiseBinaryOp<FUNCTOR<Scalar>, const Derived, const OtherDerived> \
  (METHOD)(const STORMEIGEN_CURRENT_STORAGE_BASE_CLASS<OtherDerived> &other) const \
  { \
    return CwiseBinaryOp<FUNCTOR<Scalar>, const Derived, const OtherDerived>(derived(), other.derived()); \
  }

// the expression type of a cwise product
#define STORMEIGEN_CWISE_PRODUCT_RETURN_TYPE(LHS,RHS) \
    CwiseBinaryOp< \
      internal::scalar_product_op< \
          typename internal::traits<LHS>::Scalar, \
          typename internal::traits<RHS>::Scalar \
      >, \
      const LHS, \
      const RHS \
    >

#ifdef STORMEIGEN_EXCEPTIONS
#  define STORMEIGEN_THROW_X(X) throw X
#  define STORMEIGEN_THROW throw
#  define STORMEIGEN_TRY try
#  define STORMEIGEN_CATCH(X) catch (X)
#else
#  ifdef __CUDA_ARCH__
#    define STORMEIGEN_THROW_X(X) asm("trap;") return {}
#    define STORMEIGEN_THROW asm("trap;"); return {}
#  else
#    define STORMEIGEN_THROW_X(X) std::abort()
#    define STORMEIGEN_THROW std::abort()
#  endif
#  define STORMEIGEN_TRY if (true)
#  define STORMEIGEN_CATCH(X) else
#endif

#if STORMEIGEN_HAS_CXX11_NOEXCEPT
#   define STORMEIGEN_NO_THROW noexcept(true)
#   define STORMEIGEN_EXCEPTION_SPEC(X) noexcept(false)
#else
#   define STORMEIGEN_NO_THROW throw()
#   define STORMEIGEN_EXCEPTION_SPEC(X) throw(X)
#endif

#endif // STORMEIGEN_MACROS_H
