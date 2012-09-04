/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/backends/bec.file.h
 *
 * Purpose:     Declaration of the Pantheios file Stock Back-end API.
 *
 * Created:     10th July 2006
 * Updated:     27th December 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2006-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/backends/bec.file.h
 *
 * [C, C++] Pantheios file Back-end Common API
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BEC_FILE
#define PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BEC_FILE

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_FILE_MAJOR      4
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_FILE_MINOR      3
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_FILE_REVISION   1
# define PANTHEIOS_VER_PANTHEIOS_BACKENDS_H_BEC_FILE_EDIT       33
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */
#ifndef PANTHEIOS_INCL_PANTHEIOS_H_BACKEND
# include <pantheios/backend.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_BACKEND */

/* /////////////////////////////////////////////////////////////////////////
 * Documentation
 */

/** \defgroup group__backend__stock_backends__file Pantheios file Stock Back-end
 * \ingroup group__backend__stock_backends
 * Back-end library that outputs to a file.
 */

/* /////////////////////////////////////////////////////////////////////////
 * Constants
 */

/** \defgroup group__backend__stock_backends__file__flags Pantheios file Stock Back-end Flags
 * \ingroup group__backend__stock_backends__file
 * Flags for the \ref group__backend__stock_backends__file
 */

/** \def PANTHEIOS_BE_FILE_F_TRUNCATE
 * Causes the \ref group__backend__stock_backends__file to
 *   truncate the log file.
 *
 * \ingroup group__backend__stock_backends__file__flags
 *
 * \note If more than one process is using the same file and either, or
 *  both, uses this flag, the log file contents cannot be guaranteed to
 *  be consistent.
 */

/** \def PANTHEIOS_BE_FILE_F_DISCARD_CACHED_CONTENTS
 * Causes any log statements cached in the
 *   \ref group__backend__stock_backends__file prior to assignment of the
 * file name (via pantheios_be_file_setFilePath()) to be discarded.
 *
 * \ingroup group__backend__stock_backends__file__flags
 */

/** \def PANTHEIOS_BE_FILE_F_SHARE_ON_WINDOWS
 * By default, logging on Windows takes a write-exclusive lock on the log
 * file. Specifying this flag allows multiple processes to write to the
 * same log file. This flag is ignored on UNIX.
 *
 * \ingroup group__backend__stock_backends__file__flags
 */

/** \def PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
 * By default, files are written in wide or multibyte string encoding
 * according to the encoding of the build: e.g. a Windows' wide string build
 * program will, by default, write characters in wide string encoding.
 * Specifying this flag causes output to be written in widestring encoding
 * regardless of the program's encoding.
 *
 * \ingroup group__backend__stock_backends__file__flags
 *
 * \see PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
 */

/** \def PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS
 * By default, files are written in wide or multibyte string encoding
 * according to the encoding of the build: e.g. a Windows' wide string build
 * program will, by default, write characters in wide string encoding.
 * Specifying this flag causes output to be written in multibyte string
 * encoding regardless of the program's encoding.
 *
 * \ingroup group__backend__stock_backends__file__flags
 *
 * \see PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS
 */

/** \def PANTHEIOS_BE_FILE_F_DELETE_IF_EMPTY
 * Specifying this flag causes the log file to be deleted, if and only if it
 * is empty, when it is closed (which occurs during back-end
 * uninitialisation (at program exit), or when another file path is
 * specified, for the given back-end, via pantheios_be_file_setFilePath()).
 * If the file cannot be deleted, a bail-out message will be written.
 *
 * \note The current implementation checks the actual file size, as opposed
 *   to "remembering" whether any writes were made (by Pantheios).
 *
 * \note No attempt will be made to determine whether another program (such
 *   as a log viewer) will (be attempting to) use the file; use at your own
 *   risk.
 *
 * \ingroup group__backend__stock_backends__file__flags
 */


#define PANTHEIOS_BE_FILE_F_TRUNCATE                    (0x00100000)
#define PANTHEIOS_BE_FILE_F_DISCARD_CACHED_CONTENTS     (0x00200000)
#define PANTHEIOS_BE_FILE_F_SHARE_ON_WINDOWS            (0x00400000)
#define PANTHEIOS_BE_FILE_F_WRITE_WIDE_CONTENTS         (0x00800000)
#define PANTHEIOS_BE_FILE_F_WRITE_MULTIBYTE_CONTENTS    (0x00080000)
#define PANTHEIOS_BE_FILE_F_DELETE_IF_EMPTY             (0x00040000)

#if 0 /* None of the following are yet supported: */
#define PANTHEIOS_BE_FILE_F_ROLL_ON_SIZE                (0x01000000)
#define PANTHEIOS_BE_FILE_F_ROLL_ON_ENTRY_COUNT         (0x02000000)
#define PANTHEIOS_BE_FILE_F_ROLL_ON_DATETIME            (0x04000000)

#define PANTHEIOS_BE_FILE_F_ROLL_ON_1MB                 (0x10000000 | PANTHEIOS_BE_FILE_F_ROLL_ON_SIZE)
#define PANTHEIOS_BE_FILE_F_ROLL_ON_8K_ENTRIES          (0x10000000 | PANTHEIOS_BE_FILE_F_ROLL_ON_ENTRY_COUNT)
#define PANTHEIOS_BE_FILE_F_ROLL_ON_DAY                 (0x10000000 | PANTHEIOS_BE_FILE_F_ROLL_ON_DATETIME)

#define PANTHEIOS_BE_FILE_F_ROLL_TO_SELF                (0x20000000)
#endif /* 0 */

/* /////////////////////////////////////////////////////////////////////////
 * External Declarations
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
struct Pantheios_be_file_no_longer_defines_the_function_pantheios_be_file_setFileName_and_Use_pantheios_be_file_setFilePath_instead_;
# define Pantheios_be_file_setFileName (_pantheios_be_file_no_longer_defines_the_function_pantheios_be_file_setFileName_and_Use_pantheios_be_file_setFilePath_instead_)
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

/** \def PANTHEIOS_BE_FILE_MAX_FILE_LEN
 *
 * The maximum length of a file path that can be specified in a
 * pan_be_file_init_t instance using the internal buffer
 * (<code>pan_be_file_init_t::buff</code>). File paths exceeding this size must be
 * represented in caller-side memory (i.e.
 * <code>pan_be_file_init_t::fileName != &pan_be_file_init_t::buff[0]</code>).
 */
#define PANTHEIOS_BE_FILE_MAX_FILE_LEN              (1000)

/** Structure used for specifying initialisation information to the
 *    be.file library.
 * \ingroup group__backend__stock_backends__file
 */
struct pan_be_file_init_t
{
#if !defined(PANTHEIOS_DOCUMENTATION_SKIP_SECTION) && \
    !defined(PANTHEIOS_NO_NAMESPACE)
    typedef pantheios::pan_uint16_t pan_uint16_t;
    typedef pantheios::pan_uint32_t pan_uint32_t;
    typedef pantheios::pan_uint64_t pan_uint64_t;
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION && !PANTHEIOS_NO_NAMESPACE */

    pan_uint32_t        version;    /*!< Must be initialised to the value of PANTHEIOS_VER */
    pan_uint32_t        flags;      /*!< \ref group__backend__stock_backends__file__flags "Flags" that control the information displayed. */
    PAN_CHAR_T          buff[1 + (PANTHEIOS_BE_FILE_MAX_FILE_LEN)]; /*!< Buffer for use by client to write file name, to which \link pan_be_file_init_t::fileName fileName\endlink can be pointed. \see PANTHEIOS_BE_FILE_MAX_FILE_LEN*/
    PAN_CHAR_T const*   fileName;  /*!< Must be pointed to the file name. */

#if 0 /* These features are part of a stream of development for log-file rolling that is incomplete. */
    union
    {
        pan_uint64_t    fileSizeKB; /*!< Size of file (in KB). Only used if flags contain PANTHEIOS_BE_FILE_F_ROLL_ON_SIZE. */
        pan_uint64_t    entryCount; /*!< Number of entries in entry count. Only used if flags contain PANTHEIOS_BE_FILE_F_ROLL_ON_ENTRY_COUNT. */
        pan_uint64_t    interval;   /*!< Number of seconds in interval. Only used if flags contain PANTHEIOS_BE_FILE_F_ROLL_ON_DATETIME. */

    }                   roll;       /*!< Union of measures used when file-rolling is in used. */
#endif /* 0 */


#ifdef __cplusplus
public: /* Construction */
# ifndef PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
    pan_be_file_init_t();
# endif /* !PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT */
#endif /* __cplusplus */
};
#ifndef __cplusplus
typedef struct pan_be_file_init_t   pan_be_file_init_t;
#endif /* !__cplusplus */

/* /////////////////////////////////////////////////////////////////////////
 * Application-defined functions
 */

/** \ref page__backend__callbacks "Callback" function defined by
 *    the application, invoked when the
 *    API is initialised with a NULL <code>init</code> parameter.
 * \ingroup group__backend__stock_backends__file
 *
 * \param backEndId The back-end identifier passed to the back-end
 *   during its initialisation.
 * \param init A pointer to an already-initialised instance of
 *   pan_be_file_init_t.
 *
 * If any application-specific changes are required they can be made to
 * the structure to which <code>init</code> points, which will already
 * have been initialised. These changes will then be incorporated into
 * the back-end state, and reflected in its behaviour.
 *
 * If no changes are required, then the function can be a simple stub,
 * containing no instructions.
 *
 * \note This function is only required when the
 *   \ref page__backend__callbacks "callback" version of the library is
 *   used.
 *
 * \exception "throw()" This function must <b>not</b> throw any exceptions!
 *
 * \warning This function will be called during the initialisation of
 *   Pantheios, and so <b>must not</b> make any calls into Pantheios, either
 *   directly or indirectly!
 */
PANTHEIOS_CALL(void) pantheios_be_file_getAppInit(
    int                     backEndId
,   pan_be_file_init_t*     init
) /* throw() */;

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/** Fills out a copy of the initialisation structure with default
 *    values (representative of the default behaviour of the library),
 *    ready to be customised and passed to the API initialiser function
 *    pantheios_be_file_init().
 */
PANTHEIOS_CALL(void) pantheios_be_file_getDefaultAppInit(
    pan_be_file_init_t* init
) /* throw() */;

/** Implements the functionality for pantheios_be_init() over the file API.
 *
 * \ingroup group__backend__stock_backends__file
 */
PANTHEIOS_CALL(int) pantheios_be_file_init(
    PAN_CHAR_T const*           processIdentity
,   int                         id
,   pan_be_file_init_t const*   init
,   void*                       reserved
,   void**                      ptoken
);

/** Implements the functionality for pantheios_be_uninit() over the file API.
 *
 * \ingroup group__backend__stock_backends__file
 */
PANTHEIOS_CALL(void) pantheios_be_file_uninit(void* token);

/** Implements the functionality for pantheios_be_logEntry() over the file API.
 *
 * \ingroup group__backend__stock_backends__file
 */
PANTHEIOS_CALL(int) pantheios_be_file_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
);

/** \fn pantheios_be_file_setFilePath(PAN_CHAR_T const*, pan_be_file_init_t::pan_uint32_t, pan_be_file_init_t::pan_uint32_t, int)
 *
 * Sets/changes the log file name for a single back-end.
 *
 * \ingroup group__backend__stock_backends__file
 *
 * \param fileName The (absolute or relative) name of the log file to be used
 *   with the given back-end.
 * \param fileMask A bitmask that controls which values of
 *   the \ref group__backend__stock_backends__file__flags "flags" are to be
 *   interpreted in the <code>fileFlags</code> parameter.
 * \param fileFlags The \ref group__backend__stock_backends__file__flags "flags"
 *   that control the file creation - only
 *   \ref PANTHEIOS_BE_FILE_F_TRUNCATE
 *   is recognised by this function.
 * \param backEndId The back-end identifier. If this is
 *   \ref PANTHEIOS_BEID_ALL,
 *   then all back-ends will be initialised with the file-name.
 *
 * \note In the case where you are using
 *   \link group__backend__stock_backends__file be.file\endlink for both
 *   local <i>and</i> remote back-ends, specifying \ref PANTHEIOS_BEID_ALL
 *   will, on most platforms, result in the remote back-end failing to write
 *   output, because the local back-end will hold an exclusive lock on
 *   the file.
 */
PANTHEIOS_CALL(int) pantheios_be_file_setFilePath(
    PAN_CHAR_T const*                   fileName
#ifndef PANTHEIOS_NO_NAMESPACE
,   pan_be_file_init_t::pan_uint32_t    fileMask
,   pan_be_file_init_t::pan_uint32_t    fileFlags
#else /* ? !PANTHEIOS_NO_NAMESPACE */
,   pan_uint32_t                        fileMask
,   pan_uint32_t                        fileFlags
#endif /* !PANTHEIOS_NO_NAMESPACE */
,   int                                 backEndId
);

#ifdef __cplusplus
/** \overload int pantheios_be_file_setFilePath(PAN_CHAR_T const*)
 *
 * Sets/changes the log file name for all back-ends.
 *
 * \ingroup group__backend__stock_backends__file
 *
 * \param fileName The (absolute or relative) name of the log file to be used
 *   with the given back-end.
 */
inline int pantheios_be_file_setFilePath(PAN_CHAR_T const* fileName)
{
    return pantheios_be_file_setFilePath(fileName, 0, 0, PANTHEIOS_BEID_ALL);
}

/** \overload int pantheios_be_file_setFilePath(PAN_CHAR_T const*, int)
 *
 * Sets/changes the log file name for all back-ends.
 *
 * \ingroup group__backend__stock_backends__file
 *
 * \param fileName The (absolute or relative) name of the log file to be used
 *   with the given back-end.
 * \param backEndId The back-end identifier. If this is
 *   \ref PANTHEIOS_BEID_ALL,
 *   then all back-ends will be initialised with the file-name.
 */
inline int pantheios_be_file_setFilePath(PAN_CHAR_T const* fileName, int backEndId)
{
    return pantheios_be_file_setFilePath(fileName, 0, 0, backEndId);
}
#endif /* __cplusplus */

/** Flushes one/all back-ends.
 *
 * \ingroup group__backend__stock_backends__file
 *
 * \param backEndId The back-end identifier. If this is
 *   \ref PANTHEIOS_BEID_ALL,
 *   then all back-ends will be flushed.
 */
PANTHEIOS_CALL(int) pantheios_be_file_flush(int backEndId);

/** Parses the be.file back-end flags
 *
 * \ingroup group__backend
 *
 * Processes an argument list in the same way as
 * pantheios_be_parseStockArgs(), filling out the
 * pan_be_COMErrorObject_init_t in accordance with the arguments
 * found.
 *
 * Recognises the following standard argument names:
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
 * Recognises the following back-end specific argument names:
 * - "fileName"                 (String)
 * - "truncate"                 (Boolean)
 * - "discardCachedContents"    (Boolean)
 * - "writeMultibyteContents"   (Boolean)
 * - "writeWideContents"        (Boolean)
 */
PANTHEIOS_CALL(int) pantheios_be_file_parseArgs(
    size_t                          numArgs
#ifdef PANTHEIOS_NO_NAMESPACE
,   struct pan_slice_t* const       args
#else /* ? PANTHEIOS_NO_NAMESPACE */
,   pantheios::pan_slice_t* const   args
#endif /* PANTHEIOS_NO_NAMESPACE */
,   pan_be_file_init_t*             init
);

/* ////////////////////////////////////////////////////////////////////// */

#ifdef __cplusplus
# ifndef PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
inline pan_be_file_init_t::pan_be_file_init_t()
{
    pantheios_be_file_getDefaultAppInit(this);
}
# endif /* !PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT */
#endif /* __cplusplus */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_PANTHEIOS_BACKENDS_H_BEC_FILE */

/* ///////////////////////////// end of file //////////////////////////// */
