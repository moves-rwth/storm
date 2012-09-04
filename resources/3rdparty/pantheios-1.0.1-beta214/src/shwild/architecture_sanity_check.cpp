/* /////////////////////////////////////////////////////////////////////////
 * File:    src/architecture_sanity_check.cpp
 *
 * Purpose: Used to verify the architecture for which the shwild library is
 *          being compiled.
 *
 * Created: 2nd May 2008
 * Updated: 2nd May 2008
 *
 * Home:    http://shwild.org/
 *
 * Copyright (c) 2008, Matthew Wilson and Sean Kelly
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


/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef SHWILD_DOCUMENTATION_SKIP_SECTION
# define SHWILD_VER_CPP_MATCHES_MAJOR       1
# define SHWILD_VER_CPP_MATCHES_MINOR       2
# define SHWILD_VER_CPP_MATCHES_REVISION    8
# define SHWILD_VER_CPP_MATCHES_EDIT        13
#endif /* !SHWILD_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#include <shwild/shwild.h>

#include <platformstl/platformstl.h>

/* The following code is architecture sanity check: on Windows we need to
 * ensure that the correct makefile is being used, so we verify that one of
 * the three shwild-makefile-specific valid architecture symbols are
 * defined.
 */

#if defined(PLATFORMSTL_OS_IS_UNIX)
#else defined(PLATFORMSTL_OS_IS_WIN32)
# ifndef _SHWILD_ARCH_EXPECTED_WIN32
#  error This file is not being build with the correct makefile. Please ensure that you're using makefile.win32
# endif /* !_SHWILD_ARCH_EXPECTED_WIN32 */
#else defined(PLATFORMSTL_OS_IS_WIN64)
# ifndef _SHWILD_ARCH_EXPECTED_WIN64_IA64
#  error This file is not being build with the correct makefile. Please ensure that you're using makefile.win64
# endif /* !_SHWILD_ARCH_EXPECTED_WIN64_IA64 */

borland:
	win32

codewarrior:
	win32
	unix

digitalmars:
	win32

VC++ 8:
	win32					.win32
	win32 safe				.win32_safe
	win64 (x64)				.win64_x64
	win64 (x64) safe		.win64_x64_safe
	win64 (ia64)			.win64_ia64
	win64 (ia64) safe		.win64_ia64_safe




#if defined(WIN64)
  # 
#if defined(WIN32)
  
	defined(_WIN64)
# if defined(


/* ////////////////////////////////////////////////////////////////////// */
