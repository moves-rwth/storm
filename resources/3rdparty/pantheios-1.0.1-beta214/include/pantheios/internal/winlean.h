/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/internal/winlean.h
 *
 * Purpose:     Suppresses as much as possible of the Windows headers, to
 *              decrease compilation times.
 *
 * Created:     22nd April 2008
 * Updated:     15th February 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2008-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/internal/winlean.h
 *
 * [C, C++] INTERNAL FILE: Suppresses as much as possible of the Windows
 *   headers, to decrease compilation times.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_INTERNAL_H_WINLEAN
#define PANTHEIOS_INCL_PANTHEIOS_INTERNAL_H_WINLEAN

/* /////////////////////////////////////////////////////////////////////////
 * Compatility
 */

#if !defined(_WIN32) && \
    !defined(_WIN64)
# error This file can only be used when compiling for Windows platforms
#endif /* _WIN32 && _WIN64 */

#if defined(_WINDOWS_) || \
    defined(_WINDOWS_H)
# error Cannot include after windows.h
#endif /* _WINDOWS_ */

/* /////////////////////////////////////////////////////////////////////////
 * Feature suppression
 */

#if !defined(__MINGW32__)
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS       /* CC_*, LC_*, PC_*, CP_*, TC_*, RC_ */
#define NOVIRTUALKEYCODES   /* VK_* */
#define NOWINMESSAGES       /* WM_*, EM_*, LB_*, CB_* */
#define NOWINSTYLES         /* WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_* */
#define NOSYSMETRICS        /* SM_* */
#define NOMENUS             /* MF_* */
#define NOICONS             /* IDI_* */
#define NOKEYSTATES         /* MK_* */
#define NOSYSCOMMANDS       /* SC_* */
#define NORASTEROPS         /* Binary and Tertiary raster ops */
#define NOSHOWWINDOW        /* SW_* */
#define OEMRESOURCE         /* OEM Resource values */
#define NOATOM              /* Atom Manager routines */
#define NOCLIPBOARD         /* Clipboard routines */
#define NOCOLOR             /* Screen colors */
#define NOCTLMGR            /* Control and Dialog routines */
#define NODRAWTEXT          /* DrawText() and DT_* */
/* #define NOGDI */               /* All GDI defines and routines */
#define NOKERNEL            /* All KERNEL defines and routines */
/* #define NOUSER */              /* All USER defines and routines */
/* #define NONLS */               /* All NLS defines and routines */
/* #define NOMB */                /* MB_* and MessageBox() */
#define NOMEMMGR            /* GMEM_*, LMEM_*, GHND, LHND, associated routines */
#define NOMETAFILE          /* typedef METAFILEPICT */
#define NOMINMAX            /* Macros min(a,b) and max(a,b) */
/* #define NOMSG */               /* typedef MSG and associated routines */
#define NOOPENFILE          /* OpenFile(), OemToAnsi, AnsiToOem, and OF_* */
#define NOSCROLL            /* SB_* and scrolling routines */
#define NOSERVICE           /* All Service Controller routines, SERVICE_ equates, etc. */
#define NOSOUND             /* Sound driver routines */
#define NOTEXTMETRIC        /* typedef TEXTMETRIC and associated routines */
#define NOWH                /* SetWindowsHook and WH_* */
#define NOWINOFFSETS        /* GWL_*, GCL_*, associated routines */
#define NOCOMM              /* COMM driver routines */
#define NOKANJI             /* Kanji support stuff. */
#define NOHELP              /* Help engine interface. */
#define NOPROFILER          /* Profiler interface. */
#define NODEFERWINDOWPOS    /* DeferWindowPos routines */
#define NOMCX               /* Modem Configuration Extensions */
#endif /* 0 */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_INTERNAL_H_WINLEAN */

/* ///////////////////////////// end of file //////////////////////////// */
