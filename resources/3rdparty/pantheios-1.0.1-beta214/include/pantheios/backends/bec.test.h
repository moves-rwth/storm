/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/backends/bec.test.h
 *
 * Purpose:     Declaration of the be.test library.
 *
 * Created:     1st November 2006
 * Updated:     22nd March 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2006-2009, Matthew Wilson and Synesis Software
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


/** \file pantheios/backends/bec.test.h
 *
 * \brief [C, C++] Declaration of the be.test library
 */

#ifndef PANTHEIOS_INCL_BACKENDS_H_BEC_TEST
#define PANTHEIOS_INCL_BACKENDS_H_BEC_TEST

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_BACKENDS_H_BEC_TEST_MAJOR     2
# define PANTHEIOS_VER_BACKENDS_H_BEC_TEST_MINOR     2
# define PANTHEIOS_VER_BACKENDS_H_BEC_TEST_REVISION  1
# define PANTHEIOS_VER_BACKENDS_H_BEC_TEST_EDIT      24
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

#ifndef STLSOFT_INCL_STLSOFT_H_STLSOFT
# include <stlsoft/stlsoft.h>
#endif /* !STLSOFT_INCL_STLSOFT_H_STLSOFT */

#ifdef __cplusplus
 /* We include threading.h to prevent the definition of _REENTRANT via
  * <string> on some UNIX operating systems from confusing the feature
  * discrimination in UNIXSTL and having it think that we're multithreading
  * when we're not.
  */
# include <pantheios/internal/threading.h>
# include <string>
#endif /* __cplusplus */

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/** \defgroup group__backend__stock_backends__test Pantheios Test Stock Back-end
 * \ingroup group__backend__stock_backends
 *  Stub back-end library that does no emission.
 *
 * This library is useful for conducting performance tests.
 */

/** Implements the functionality for pantheios_be_init() over the Test API.
 * \ingroup group__backend__stock_backends__test
 */
PANTHEIOS_CALL(int) pantheios_be_test_init(
    PAN_CHAR_T const*   processIdentity
,   int                 id
,   void*               unused
,   void*               reserved
,   void**              ptoken
);

/** Implements the functionality for pantheios_be_uninit() over the Test API.
 * \ingroup group__backend__stock_backends__test
 */
PANTHEIOS_CALL(void) pantheios_be_test_uninit(
    void* token
);

/** Implements the functionality for pantheios_be_logEntry() over the Test API.
 * \ingroup group__backend__stock_backends__test
 */
PANTHEIOS_CALL(int) pantheios_be_test_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              cchEntry
);

/** \def pantheios_be_test_parseArgs
 *
 * Parses the be.test back-end flags
 *
 * \ingroup group__backend
 *
 * There are currently no arguments whatsoever for be.test, hence
 * pantheios_be_test_parseArgs is actually a \#define for
 * <code>NULL</code>. At such time as back-end specific arguments
 * are required, it will become a first-class function.
 */
#define pantheios_be_test_parseArgs         NULL

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#ifdef __cplusplus

namespace pantheios
{
namespace be
{
namespace test
{

    /** Represents a date/time epoch.
     *
     * \note Currently implemented as a no-op class. Will be fleshed out in
     *   a future version of Pantheios.
     */
    class Time
    {
    public:
        /** Creates an instance representing the current time. */
        static Time now();
    };

    /** Represents an entry in the statement history
     */
    struct Entry
    {
    public: /* Member Types */
        /** The string type */
        typedef std::basic_string<pan_char_t>   string_type;

    public: /* Member Variables */
        /* const */ Time            time;       /*!< The time the entry was logged. */
        /* const */ int             severity;   /*!< The entry's severity. */
        /* const */ string_type     statement;  /*!< The entry's contents. */

    public: /* Construction */
        /** Creates an instance with the given characteristics */
        Entry(int severity, pan_char_t const* entry, size_t cchEntry);
    };

    /** Represents the collection of entries since the last call
     * to reset()
     */
    class Results
    {
    public: /* Member Types */
        /** The type of this class */
        typedef Results     class_type;
        /** The value type */
        typedef Entry       value_type;
    protected:
# if defined(STLSOFT_COMPILER_IS_DMC)
    public:
# endif /* compiler */
        /** Pimpl class */
        struct ResultsImpl;

    protected: /* Construction */
# ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
        Results(ResultsImpl* impl);
# endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */
    public:
        /** Copies the contents */
        Results(class_type const& rhs);
        /** Destroys the instance */
        ~Results()  stlsoft_throw_0();

    public: /* Accessors */
        /** Indicates whether the container is empty */
        bool                empty() const;
        /** Indicates the number of results in the container */
        size_t              size() const;
        /** Requests an element from the container
         *
         * \param index The index of the element. Must be less than the value returned by size()
         */
        value_type const&   operator [](size_t index) const;

    private: /* Implementation */
        ResultsImpl* const  m_impl;

    private: /* Not to be implemented */
        class_type& operator= (class_type const& rhs);
    };

    /** Resets all results */
    void reset();

    /** Obtain a copy of the current test results */
    Results results();

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

} /* namespace test */
} /* namespace be */
} /* namespace pantheios */

#endif /* __cplusplus */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_INCL_BACKENDS_H_BEC_TEST */

/* ///////////////////////////// end of file //////////////////////////// */
