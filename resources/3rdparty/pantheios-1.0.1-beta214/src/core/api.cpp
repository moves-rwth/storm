/* /////////////////////////////////////////////////////////////////////////
 * File:        src/core/api.cpp
 *
 * Purpose:     Implementation file for Pantheios core API.
 *
 * Created:     21st June 2005
 * Updated:     7th August 2012
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


/* Pantheios header files
 *
 * NOTE: We do _not_ include pantheios/pantheios.hpp here, since we are
 *  not using any of the Application Layer.
 */
#include <pantheios/pantheios.h>
#include <pantheios/internal/lean.h>
#include <pantheios/internal/nox.h>
#include <pantheios/backend.h>
#include <pantheios/frontend.h>

#include <pantheios/init_codes.h>
#include <pantheios/quality/contract.h>
#include <pantheios/internal/safestr.h>
#ifndef UNIXSTL_NO_ATOMIC_INTEGER_OPERATIONS_ON_WINDOWS
# define UNIXSTL_NO_ATOMIC_INTEGER_OPERATIONS_ON_WINDOWS
#endif /* !UNIXSTL_NO_ATOMIC_INTEGER_OPERATIONS_ON_WINDOWS */
#include <pantheios/internal/threading.h>
#include <pantheios/util/string/snprintf.h>
#include <pantheios/util/string/strdup.h>

/* STLSoft header files */

#include <stlsoft/conversion/char_conversions.hpp>
#include <stlsoft/iterators/cstring_concatenator_iterator.hpp>
#include <stlsoft/iterators/member_selector_iterator.hpp>
#include <stlsoft/memory/allocator_selector.hpp>
#include <pantheios/util/memory/auto_buffer_selector.hpp>
#include <stlsoft/shims/access/string.hpp>
#include <stlsoft/smartptr/scoped_handle.hpp>
#include <stlsoft/synch/lock_scope.hpp>

#include <platformstl/platformstl.h>
#include <platformstl/synch/util/features.h>
#ifdef PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS
# include <platformstl/synch/atomic_functions.h>
# include <platformstl/synch/spin_mutex.hpp>
#endif /* PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS */
#if defined(PANTHEIOS_MT) && \
    !defined(PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS)
# include <platformstl/synch/thread_mutex.hpp>
#else /* ? PANTHEIOS_MT */
# include <stlsoft/synch/null_mutex.hpp>
#endif /* PANTHEIOS_MT */

/* Standard C++ header files */

#include <algorithm>
#include <map>
#include <new>
#include <numeric>

/* Standard C header files */

#if defined(STLSOFT_COMPILER_IS_BORLAND)
# include <memory.h>
#endif /* compiler */
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Operating-system header files */

#ifdef PANTHEIOS_MT
# ifndef PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS
#  if defined(PLATFORMSTL_OS_IS_UNIX)
#   include <pthread.h>
#   include <platformstl/synch/thread_mutex.hpp>
#  endif /* OS */
# endif /* !PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS */
#endif /* PANTHEIOS_MT */

#if defined(PLATFORMSTL_OS_IS_UNIX)
# include <syslog.h>
#endif /* OS */

/* /////////////////////////////////////////////////////////////////////////
 * Features
 */

#ifdef PANTHEIOS_DEFINE_BACK_END_MAP
# undef PANTHEIOS_DEFINE_BACK_END_MAP /* Not yet released */
#endif /* PANTHEIOS_DEFINE_BACK_END_MAP */

/* /////////////////////////////////////////////////////////////////////////
 * Compiler compatibility
 */

#if (   defined(STLSOFT_COMPILER_IS_MSVC) && \
        _MSC_VER < 1200)
# define _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS
#endif /* compiler */

#if defined(STLSOFT_COMPILER_IS_MSVC) && \
    _MSC_VER < 1310
# define _PANTHEIOS_COMPILER_CANNOT_USE_ANONYMOUS_NAMESPACES
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * Warning suppression
 */

#if defined(STLSOFT_COMPILER_IS_BORLAND)
# pragma warn -8080
#endif /* compiler */

#if defined(STLSOFT_COMPILER_IS_MSVC) && \
    _MSC_VER >= 1310
# pragma warning(disable : 4702)    /* don't warn about unused catch blocks */
#endif /* compiler */


#if defined(STLSOFT_COMPILER_IS_MSVC) && \
    defined(PANTHEIOS_USING_SAFE_STR_FUNCTIONS)
/* Who would credit that I'd have to pull such a hack just to avoid
 * being told that in using a standard function I'm using deprecated
 * functionality. Deprecated by whom?
 */
namespace std
{

    template<   typename I
            ,   typename O
            >
    inline O daft_msvc_copy_workaround_(I first, I last, O out)
    {
        for(; first != last; ++first, ++out)
        {
            *out = *first;
        }

        return out;
    }
}

# define copy       daft_msvc_copy_workaround_

/* Also pulling the same trick for wcstombs
 */

namespace
{

    size_t daft_msvc_wcstombs_workaround_(
        char*           s1
    ,   const wchar_t*  s2
    ,   size_t          n
    )
    {
    # pragma warning(push)
    # pragma warning(disable : 4996)

        return ::wcstombs(s1, s2, n);

    # pragma warning(pop)
    }
}

# define wcstombs   daft_msvc_wcstombs_workaround_


# include <stlsoft/algorithms/std/alt.hpp>
namespace std
{
    using stlsoft::std_fill_n;

#  define fill_n std_fill_n

} // namespace std

#endif /* STLSOFT_COMPILER_IS_MSVC && PANTHEIOS_USING_SAFE_STR_FUNCTIONS */

/* /////////////////////////////////////////////////////////////////////////
 * String encoding compatibility
 */

#ifdef PANTHEIOS_USE_WIDE_STRINGS

# define pan_strlen_                    wcslen

#else /* ? PANTHEIOS_USE_WIDE_STRINGS */

# define pan_strlen_                    strlen

#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* /////////////////////////////////////////////////////////////////////////
 * Helper functions
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
namespace ximpl_core
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* To be declared in pantheios.core.h */
void pantheios_initPad_(void);

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace ximpl_core */
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

#ifndef PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS

namespace pantheios_x
{

    class thread_mutex_t
# ifdef PANTHEIOS_MT
        : public platformstl::thread_mutex
# else /* ? PANTHEIOS_MT */
        : public stlsoft::null_mutex
# endif /* PANTHEIOS_MT */
    {
    public: // Member types
# ifdef PANTHEIOS_MT
        typedef platformstl::thread_mutex   parent_class_type;
# else /* ? PANTHEIOS_MT */
        typedef stlsoft::null_mutex         parent_class_type;
# endif /* PANTHEIOS_MT */
    public: // Construction
        explicit thread_mutex_t(bool b)
# if defined(PANTHEIOS_MT) && \
     defined(PLATFORMSTL_OS_IS_UNIX)
            : parent_class_type(b)
# endif /* PANTHEIOS_MT && OS */
        {
            STLSOFT_SUPPRESS_UNUSED(b);
        }

    private:
        thread_mutex_t(thread_mutex_t const&);
        thread_mutex_t& operator =(thread_mutex_t const&);
    };

} // namespace pantheios_x

namespace stlsoft
{
    inline void lock_instance(::pantheios_x::thread_mutex_t& mx)
    {
        ::stlsoft::lock_instance(static_cast< ::pantheios_x::thread_mutex_t::parent_class_type&>(mx));
    }
    inline void unlock_instance(::pantheios_x::thread_mutex_t& mx)
    {
        ::stlsoft::unlock_instance(static_cast< ::pantheios_x::thread_mutex_t::parent_class_type&>(mx));
    }

} // namespace stlsoft

#endif /* !PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Constants
 */

#ifndef PANTHEIOS_CORE_LOG_N_INTERNAL_BUFFER_SIZE
# define PANTHEIOS_CORE_LOG_N_INTERNAL_BUFFER_SIZE      (2048)
#endif /* !PANTHEIOS_CORE_LOG_N_INTERNAL_BUFFER_SIZE */

#ifndef PANTHEIOS_CORE_LOGPRINTF_STACK_SIZE
# define PANTHEIOS_CORE_LOGPRINTF_STACK_SIZE            (4096)
#endif /* !PANTHEIOS_CORE_LOG_BUFFER_SIZE */

#ifndef PANTHEIOS_CORE_MINIMUM_PAD_CHARACTERS
# define PANTHEIOS_CORE_MINIMUM_PAD_CHARACTERS          (1000)
#else /* ? PANTHEIOS_CORE_MINIMUM_PAD_CHARACTERS */
# if PANTHEIOS_CORE_MINIMUM_PAD_CHARACTERS < (1000)
#  error Compile-time customisations of pad length must define at least 1000 characters
# endif
#endif /* !PANTHEIOS_CORE_MINIMUM_PAD_CHARACTERS */

#ifndef PANTHEIOS_CORE_BACKENDID_BASE
# define PANTHEIOS_CORE_BACKENDID_BASE                  (1000)
#else /* ? PANTHEIOS_CORE_BACKENDID_BASE */
# if PANTHEIOS_CORE_BACKENDID_BASE < (1)
#  error Compile-time customisations of backEndId base must be a +ve integer
# endif
#endif /* PANTHEIOS_CORE_BACKENDID_BASE */

#define PANTHEIOS_MAXIMUM_MAX_PROCESS_IDENTITY_LENGTH   (1000)

#ifndef _PANTHEIOS_COMPILER_CANNOT_USE_ANONYMOUS_NAMESPACES
namespace
{
#endif /* !_PANTHEIOS_COMPILER_CANNOT_USE_ANONYMOUS_NAMESPACES */

const size_t    LOG_STACK_BUFFER_SIZE       =   PANTHEIOS_CORE_LOG_N_INTERNAL_BUFFER_SIZE -1;
const size_t    PRINTF_STACK_BUFFER_SIZE    =   PANTHEIOS_CORE_LOGPRINTF_STACK_SIZE - 1;
size_t const    PAD_BUFFER_SIZE             =   PANTHEIOS_CORE_MINIMUM_PAD_CHARACTERS;

struct pantheios_log_n_buffer_size_constraint_
{
    ~pantheios_log_n_buffer_size_constraint_()
    {
        // The pantheios_log_n() internal buffer size must be between 1B and 1MB
        STLSOFT_STATIC_ASSERT(LOG_STACK_BUFFER_SIZE >= 1);
        STLSOFT_STATIC_ASSERT(LOG_STACK_BUFFER_SIZE <= 0x100000);
    }
};

struct pantheios_logprintf_stack_size_constraint_
{
    ~pantheios_logprintf_stack_size_constraint_()
    {
        // The pantheios_log_n() internal buffer size must be between 64B and 1MB
        STLSOFT_STATIC_ASSERT(PRINTF_STACK_BUFFER_SIZE >= 64);
        STLSOFT_STATIC_ASSERT(PRINTF_STACK_BUFFER_SIZE < 0x100000);
    }
};

#ifndef _PANTHEIOS_COMPILER_CANNOT_USE_ANONYMOUS_NAMESPACES
} // anonymous namespace
#endif /* !_PANTHEIOS_COMPILER_CANNOT_USE_ANONYMOUS_NAMESPACES */

/* /////////////////////////////////////////////////////////////////////////
 * Statics
 */

#ifndef _PANTHEIOS_COMPILER_CANNOT_USE_ANONYMOUS_NAMESPACES
namespace
{
#endif /* !_PANTHEIOS_COMPILER_CANNOT_USE_ANONYMOUS_NAMESPACES */


#ifndef PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS

    using pantheios_x::thread_mutex_t;

#endif /* !PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS */

    // Forward declarations
    class BackEndMap;

    //////////////////////////////////////////////////////////////////
    // API initialisation control variables

    // The API initialisation count
    platformstl::sint32_t                       s_apiInit;

#ifdef PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS

    // The spin mutex used to guard the API init count
    //
    // See section 10.2.2 of Imperfect C++ (http://imperfectcplusplus.com/)
    //
    // When using spin-mutexes, the API initialisation itself can be
    // safely invoked by multiple concurrently contending threads.
    platformstl::spin_mutex::atomic_int_type    s_mx;

    // The spin mutex used to guard the memory pool allocation list
    platformstl::spin_mutex::atomic_int_type    s_mxMem;

#else /* ? PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS */

    // When the architecture does not support atomic integer
    // operations, we have to use a bona-fide mutex. Naturally,
    // the question is how to ensure that the mutex itself is
    // (un)initialised.
    //
    // When not using spin-mutexes, the API initialisation itself cannot
    // be safely invoked by multiple concurrently contending threads.
    // Only a single thread can be allowed (by the user) to invoke
    // application initialisation (via pantheios_init()) for the first
    // time. Any number of subsequent initialisation attempts (calls to
    // pantheios_init()) may then be allowed.
    //

    // A pointer to the API mutex instance. Also serves as a Boolean
    // indicator as to whether the API is initialised.
    thread_mutex_t*                             s_pmxApi    =   NULL;

    // A pointer to the back-end Id mutex instance
    thread_mutex_t*                             s_pmxBEI    =   NULL;

    // A pointer to the memory pool mutex instance
    thread_mutex_t*                             s_pmxMem    =   NULL;

    // Memory used for the mutexes
    union mx_union_t
    {
        long            l;
        double          d;
        long double     ld;
        stlsoft::byte_t bytes[sizeof(thread_mutex_t)];

    }                                           s_mx1_
    ,                                           s_mx2_
    ,                                           s_mx3_;

#endif /* PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS */

    // The value of the first API initialisation return
    int                                         s_init      =   0;

    // A Boolean that indicates whether the application is
    // currently undergoing its (first) initialisation.
    int                                         s_isInitialising = 0;

    //////////////////////////////////////////////////////////////////
    // X
#ifdef PANTHEIOS_DEFINE_BACK_END_MAP
    BackEndMap*                                 s_backEndMap =  NULL;
#endif /* PANTHEIOS_DEFINE_BACK_END_MAP */

    //////////////////////////////////////////////////////////////////
    // API context variables

    // The memory pool
    struct memory_pool_entry_t;
    struct memory_pool_entry_t
    {
    public:
        memory_pool_entry_t*    next;
        size_t                  cbEntry;
        long double             padding;
        stlsoft::byte_t         bytes[1];

    private:
        static
        size_t
        round_up_(
            size_t  n
        )
        {
            return (n + 15) & ~(15);
        }

    public:
        static
        memory_pool_entry_t*
        alloc(
            size_t                  cb
        ,   memory_pool_entry_t*    next
        )
        {
            size_t cbActual = offsetof(memory_pool_entry_t, bytes) + cb;

            cbActual = round_up_(cbActual);

#if defined(_DEBUG) && \
    defined(PLATFORMSTL_OS_IS_WINDOWS)
            memory_pool_entry_t*  entry = static_cast<memory_pool_entry_t*>(::HeapAlloc(::GetProcessHeap(), 0, cbActual));
#else /* ? VC++ _DEBUG */
            memory_pool_entry_t*  entry = static_cast<memory_pool_entry_t*>(::malloc(cbActual));
#endif /* VC++ _DEBUG */

            if(NULL != entry)
            {
                entry->next     =   next;
                entry->cbEntry  =   cb;

                ::memset(&entry->bytes[0], 0, cb);
            }

            return entry;
        }
        static
        void
        free(void* pv)
        {
#if defined(_DEBUG) && \
    defined(PLATFORMSTL_OS_IS_WINDOWS)
            ::HeapFree(::GetProcessHeap(), 0, pv);
#else /* ? VC++ _DEBUG */
            ::free(pv);
#endif /* VC++ _DEBUG */
        }

        static
        void
        deallocate(
            memory_pool_entry_t*    head
        )
        {
            { for(memory_pool_entry_t* entry = head; NULL != entry; )
            {
                void* pv = entry;

                entry = entry->next;

                memory_pool_entry_t::free(pv);
            }}
        }
    };
    memory_pool_entry_t*                        s_memPoolHead   =   NULL;

    // The front-end initialisation token
    void*                                       s_feToken       =   NULL;

    // The back-end initialisation token
    void*                                       s_beToken       =   NULL;

    // Process identity (1.0.1 b214+)
    //
    // If this is NULL, then the front-end indicated has its dynamic nature
    // by returning NULL for the first call of
    // pantheios_fe_getProcessIdentity(), and will be called each time
    // process identity is required; otherwise, it's (assumed to be) a 
    // "classic" front-end, and the value has been cached by this
    // variable.
    pan_char_t*                                 s_internalProcessIdentity   =   NULL;

    // The padding characters
    pan_char_t                                  s_padCharacters[PAD_BUFFER_SIZE + 1u];

    //////////////////////////////////////////////////////////////////

#ifndef _PANTHEIOS_COMPILER_CANNOT_USE_ANONYMOUS_NAMESPACES
} // anonymous namespace
#endif /* !_PANTHEIOS_COMPILER_CANNOT_USE_ANONYMOUS_NAMESPACES */

/* /////////////////////////////////////////////////////////////////////////
 * Core functions
 */

#ifndef _PANTHEIOS_COMPILER_CANNOT_USE_ANONYMOUS_NAMESPACES
namespace
{
#endif /* !_PANTHEIOS_COMPILER_CANNOT_USE_ANONYMOUS_NAMESPACES */

#if !defined(PANTHEIOS_NO_NAMESPACE)
    using ximpl_core::pantheios_initPad_;
#endif /* !PANTHEIOS_NO_NAMESPACE */

#if !defined(PANTHEIOS_NO_NAMESPACE)
    using pantheios::util::pantheios_onBailOut3;
    using pantheios::util::pantheios_onBailOut4;
#endif /* !PANTHEIOS_NO_NAMESPACE */

    //////////////////////////////////////////////////////////////////

#ifdef PANTHEIOS_DEFINE_BACK_END_MAP

    class BackEndMap
    {
    public:
        typedef BackEndMap      class_type;

    public:
        BackEndMap();

    public:
        int lookup(int backEndId, void** ptoken) const;
        int add(int backEndId, void* token);
        int remove(int backEndId);

    private:
        typedef std::map<int, void*>    map_type_;

        thread_mutex_t      m_mx;
        map_type_           m_map;

    private:
        BackEndMap(class_type const&);
        class_type& operator =(class_type const&);
    };

    int pantheios_init_backEndMap()
    {
        PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(NULL == s_backEndMap, "Should not be called if back-end map already created");

# ifdef STLSOFT_CF_EXCEPTION_SUPPORT
        try
        {
# endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

            s_backEndMap = new BackEndMap();

            if(NULL == s_backEndMap)
            {
                return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
            }

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
        }
        catch(std::bad_alloc&)
        {
            return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
        }
        catch(std::exception&)
        {
            return PANTHEIOS_INIT_RC_UNSPECIFIED_FAILURE;
        }
        catch(...)
        {
            return PANTHEIOS_INIT_RC_UNKNOWN_FAILURE;
        }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return 0;
    }

    void pantheios_uninit_backEndMap()
    {
        PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(NULL != s_backEndMap, "Should not be called if no back-end map");

        delete s_backEndMap;

        s_backEndMap = NULL;
    }
#endif /* PANTHEIOS_DEFINE_BACK_END_MAP */

    int pantheios_init_onetime()
    {
        PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(!s_isInitialising, "initialisation already in progress");

        s_isInitialising = 1;

        // 0. Initialise the Pantheios memory pool

            // Nothing to do; the head pointer is set to NULL during static initialisation


            // fill all the pad characters
        pantheios_initPad_();

// TODO: do a table of APIs in Pan core init (somewhat like !(C ^ C++))

        // 1. Initialise front-end

        int r = pantheios_fe_init(0, &s_feToken);

        if(r < 0)
        {
            pantheios_onBailOut4(PANTHEIOS_SEV_ALERT, "front-end did not initialise", NULL, pantheios_getInitCodeString(r));
        }
        else
        {
            // TODO: Mechanism to allow a measure of dynamism in
            // pantheios_fe_getProcessIdentity() without breaking existing
            // existing front-ends (or back-ends):
            //
            // 1. Current behaviour:
            //
            //  pantheios_fe_getProcessIdentity() called at most once during
            //  startup; may be called multiple times by bailout calls
            //  return value may not be NULL
            //
            // 2. Want ability for following:
            //
            // a. Core store process identity, so can implement
            //     pantheios_getProcessIdentity()
            // b. Dynamic process identity (calling
            //     pantheios_fe_getProcessIdentity() every time) when needed
            //     by back-end
            //
            // Solutions:
            // ----------
            //
            // 2a can be handled, without breaking any existing
            //     front/back-ends, by having core take copy and use that
            //     henceforth
            // 2b requires front-ends to return NULL on the FIRST CALL ONLY,
            //     the core take note of this, pass this on to the back-end
            //     initialisation, and then all subsequent calls to
            //     pantheios_getProcessIdentity() and within all back-ends
            //     would call pantheios_fe_getProcessIdentity() every time.

            bool                requirePICopy   =   false;
            PAN_CHAR_T const*   processIdentity =   pantheios_fe_getProcessIdentity(s_feToken);

            if(NULL == processIdentity)
            {
                // Now, according to protocol, we call a second time, and
                // expect to receive a non-NULL pointer to string.

                processIdentity = pantheios_fe_getProcessIdentity(s_feToken);
            }
            else
            {
                // We're dealing with a "classic" front-end - one that does
                // not expect repeated calls and which, presumably, always
                // returns the same string - so we indicate this by setting
                // requirePICopy

                requirePICopy = true;
            }

            PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_API(NULL != processIdentity, "returned string from pantheios_fe_getProcessIdentity() must not be null");
            PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_API(pan_strlen_(processIdentity) <= PANTHEIOS_MAXIMUM_MAX_PROCESS_IDENTITY_LENGTH, "returned string from pantheios_fe_getProcessIdentity() must be no longer than 1000 characters");

            if(requirePICopy)
            {
                s_internalProcessIdentity = pantheios_util_strdup_nothrow(processIdentity);

                if(NULL == s_internalProcessIdentity)
                {
                    r = PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
                }
                else
                {
                    processIdentity = s_internalProcessIdentity;
                }
            }
            else
            {
                PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(NULL == s_internalProcessIdentity);
            }

            // Before we do the back-end initialisation, we need to get the
            // process identity from the front-end and, if in widestring
            // mode, convert it to multibyte in case we need to pass it to
            // bail-out.
            //
            // Note: there is no failure response here, since we only use it
            // in bail-out.

#ifdef PANTHEIOS_USE_WIDE_STRINGS
            char            processIdentity_[PANTHEIOS_MAXIMUM_MAX_PROCESS_IDENTITY_LENGTH + 1];
            size_t const    n = ::wcstombs(processIdentity_, processIdentity, STLSOFT_NUM_ELEMENTS(processIdentity_) - 1);
            char const*     processIdentity_m;

            if(n < STLSOFT_NUM_ELEMENTS(processIdentity_))
            {
                processIdentity_[STLSOFT_NUM_ELEMENTS(processIdentity_) - 1] = '\0';
                processIdentity_m = processIdentity_;
            }
            else
            {
                processIdentity_m = NULL;
            }

# define pantheios_core_impl_getProcessIdentity_a() processIdentity_m
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
# define pantheios_core_impl_getProcessIdentity_a() processIdentity
#endif /* PANTHEIOS_USE_WIDE_STRINGS */

#ifdef PANTHEIOS_DEFINE_BACK_END_MAP
            // 2. Initialise back-end map

            r = pantheios_init_backEndMap();

            if(r < 0)
            {
                pantheios_onBailOut4(PANTHEIOS_SEV_ALERT, "back-end map creation failed", pantheios_core_impl_getProcessIdentity_a(), pantheios_getInitCodeString(r));
            }
            else
#endif /* PANTHEIOS_DEFINE_BACK_END_MAP */
            {
                // 3. Initialise back-end

                r = pantheios_be_init(processIdentity, 0, &s_beToken);

                if(r < 0)
                {
                    pantheios_onBailOut4(PANTHEIOS_SEV_ALERT, "back-end did not initialise", pantheios_core_impl_getProcessIdentity_a(), pantheios_getInitCodeString(r));
                }

#ifdef PANTHEIOS_DEFINE_BACK_END_MAP
                if(r < 0)
                {
                    pantheios_uninit_backEndMap();
                }
#endif /* PANTHEIOS_DEFINE_BACK_END_MAP */
            }

            if(r < 0)
            {
                pantheios_fe_uninit(s_feToken);
                s_feToken = NULL;

                pantheios_util_strfree(s_internalProcessIdentity);
            }
        }

        s_isInitialising = 0;

        return r;

#undef pantheios_core_impl_getProcessIdentity_a

    }

    void pantheios_uninit_onetime()
    {
        // 3. Uninitialise back-end
        pantheios_be_uninit(s_beToken);
        s_beToken = NULL;

#ifdef PANTHEIOS_DEFINE_BACK_END_MAP
        // 2. Uninitialise back-end map
        pantheios_uninit_backEndMap();
#endif /* PANTHEIOS_DEFINE_BACK_END_MAP */

        // 1. Uninitialise front-end
        pantheios_fe_uninit(s_feToken);
        s_feToken = NULL;

        pantheios_util_strfree(s_internalProcessIdentity);
        s_internalProcessIdentity = NULL;

        // 0. Deallocate all entries in memory pool
        memory_pool_entry_t::deallocate(s_memPoolHead);
        s_memPoolHead = NULL;
    }

#ifdef PANTHEIOS_DEFINE_BACK_END_MAP

    inline BackEndMap::BackEndMap()
    {}

    inline int BackEndMap::lookup(int backEndId, void** ptoken) const
    {
        stlsoft::lock_scope<thread_mutex_t>  lock(const_cast<class_type*>(this)->m_mx);

        void*                       token_;
        map_type_::const_iterator   it = m_map.find(backEndId);

        // Null Object (Variable) here, for ptoken;
        if(NULL == ptoken)
        {
            ptoken = &token_;
        }

        return (m_map.end() == it) ? 0 : (*ptoken = (*it).second, 1);
    }

    inline int BackEndMap::add(int backEndId, void* token)
    {
        stlsoft::lock_scope<thread_mutex_t>  lock(m_mx);

# ifdef STLSOFT_CF_EXCEPTION_SUPPORT
        try
        {
# endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

            if(m_map.find(backEndId) != m_map.end())
            {
                return 1;
            }
            else
            {
                // We *never* use operator [] (and no-one in their
                // right mind should!)

                m_map.insert(std::make_pair(backEndId, token));
            }

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
        }
        catch(std::bad_alloc&)
        {
            return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
        }
        catch(std::exception&)
        {
            return PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
        }
        catch(...)
        {
            return PANTHEIOS_INIT_RC_UNKNOWN_FAILURE;
        }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return 0;
    }

    inline int BackEndMap::remove(int backEndId)
    {
        stlsoft::lock_scope<thread_mutex_t>  lock(m_mx);

        return m_map.erase(backEndId) > 0;
    }
#endif /* PANTHEIOS_DEFINE_BACK_END_MAP */

#ifndef _PANTHEIOS_COMPILER_CANNOT_USE_ANONYMOUS_NAMESPACES
} // anonymous namespace
#endif /* !_PANTHEIOS_COMPILER_CANNOT_USE_ANONYMOUS_NAMESPACES */

/* /////////////////////////////////////////////////////////////////////////
 * Core API
 *
 * Note: for those compilers that object to instantiating templates within
 * extern "C" functions, the actual functions and their implementations are
 * separated by the use of intermediary extern "C++" forms. For example,
 * pantheios_init() is implemented in terms of pantheios_init__cpp(). For
 * compilers that do not have such a problem, the intermediary forms are not
 * used
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace core
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* Defined here, for the moment, as not currently declared in pantheios.h */
PANTHEIOS_CALL(int) pantheios_dispatch(
    pan_sev_t           severity
,   size_t              cchEntry
,   pan_char_t const*   entry
);

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace core */
#endif /* !PANTHEIOS_NO_NAMESPACE */


#ifdef _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS
extern "C++" pan_uint32_t pantheios_getVersion__cpp();
extern "C++" int pantheios_init__cpp();
extern "C++" void pantheios_uninit__cpp();
extern "C++" int pantheios_isSeverityLogged__cpp(
    pan_sev_t  severity
);
extern "C++" pan_char_t const* pantheios_getProcessIdentity__cpp();
extern "C++" int pantheios_dispatch__cpp(
    pan_sev_t           severity
,   size_t              cchEntry
,   pan_char_t const*   entry
);
extern "C++" int pantheios_log_n__cpp(
    pan_sev_t           severity
,   size_t              numSlices
,   pan_slice_t const*  slices
);
#endif /* _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS */


PANTHEIOS_CALL(pan_uint32_t) pantheios_getVersion()
#ifdef _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS
{
    return pantheios_getVersion__cpp();
}
pan_uint32_t pantheios_getVersion__cpp()
#endif /* _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS */
{
    return PANTHEIOS_VER;
}

/* Core initialisation function */
PANTHEIOS_CALL(int) pantheios_init()
#ifdef _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS
{
    return pantheios_init__cpp();
}
int pantheios_init__cpp()
#endif /* _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS */
{
#ifdef PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS

    // If we can make the initialisation thread-safe, without
    // expense, then we do so.
    platformstl::spin_mutex                         mx(&s_mx);
    stlsoft::lock_scope<platformstl::spin_mutex>    lock(mx);

    PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(!s_isInitialising);

    if(1 == ++s_apiInit)
    {
        s_init = pantheios_init_onetime();
    }

    return s_init;

#else /* ? PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS */

    if(NULL == s_pmxApi)
    {
        PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API(0 == s_apiInit, "api initialisation must be 0");
        PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API(NULL == s_pmxBEI, "back-end id mutex must be uninitialised");
        PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API(NULL == s_pmxMem, "memory pool mutex must be uninitialised");

# ifdef STLSOFT_CF_EXCEPTION_SUPPORT
        try
        {
# endif /* STLSOFT_CF_EXCEPTION_SUPPORT */


            s_pmxApi = new(&s_mx1_.bytes[0]) thread_mutex_t(false);

            if(NULL == s_pmxApi)
            {
                pantheios_onBailOut3(PANTHEIOS_SEV_ALERT, "failed to create Pantheios core API mutex", NULL);

                return PANTHEIOS_INIT_RC_UNSPECIFIED_FAILURE;
            }
            else
            {
                s_pmxBEI = new(&s_mx2_.bytes[0]) thread_mutex_t(false);

                if(NULL == s_pmxBEI)
                {
                    pantheios_onBailOut3(PANTHEIOS_SEV_ALERT, "failed to create back-end Id mutex", NULL);

                    s_pmxApi->~thread_mutex_t();
                    s_pmxApi = NULL;

                    return PANTHEIOS_INIT_RC_UNSPECIFIED_FAILURE;
                }
                else
                {
                    s_pmxMem = new(&s_mx3_.bytes[0]) thread_mutex_t(false);

                    if(NULL == s_pmxMem)
                    {
                        pantheios_onBailOut3(PANTHEIOS_SEV_ALERT, "failed to create memory pool mutex", NULL);

                        s_pmxApi->~thread_mutex_t();
                        s_pmxApi = NULL;

                        s_pmxBEI->~thread_mutex_t();
                        s_pmxBEI = NULL;

                        return PANTHEIOS_INIT_RC_UNSPECIFIED_FAILURE;
                    }
                    else
                    {
                        stlsoft::lock_scope<thread_mutex_t>  lock(*s_pmxApi);

                        s_init = pantheios_init_onetime();

                        if(s_init < 0)
                        {
                            s_pmxApi->~thread_mutex_t();
                            s_pmxApi = NULL;

                            s_pmxBEI->~thread_mutex_t();
                            s_pmxBEI = NULL;

                            s_pmxMem->~thread_mutex_t();
                            s_pmxMem = NULL;

                            return s_init;
                        }

                        ++s_apiInit;

                        PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API(1 == s_apiInit, "api initialisation must be counted");

                        return s_init;
                    }
                }
            }

# ifdef STLSOFT_CF_EXCEPTION_SUPPORT
        }
        catch(...)
        {
            pantheios_onBailOut3(PANTHEIOS_SEV_ALERT, "exception occurred when creating Pantheios mutexes", NULL);

            if(NULL != s_pmxApi)
            {
                s_pmxApi->~thread_mutex_t();
                s_pmxApi = NULL;
            }

            if(NULL != s_pmxBEI)
            {
                s_pmxBEI->~thread_mutex_t();
                s_pmxBEI = NULL;
            }

            if(NULL != s_pmxMem)
            {
                s_pmxMem->~thread_mutex_t();
                s_pmxMem = NULL;
            }

            return PANTHEIOS_INIT_RC_UNSPECIFIED_FAILURE;
        }
# endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
    }

    PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API(NULL != s_pmxApi, "api mutex must be initialised");
    PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API(NULL != s_pmxBEI, "back-end id mutex must be initialised");
    PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API(NULL != s_pmxMem, "memory pool mutex must be initialised");
    PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API(0 != s_apiInit, "api initialisation must be counted");

    stlsoft::lock_scope<thread_mutex_t>  lock(*s_pmxApi);

    PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API(0 != s_apiInit, "api initialisation must be counted");

    ++s_apiInit;

    return s_init;

#endif /* PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS */
}

/* Core uninitialisation function */
PANTHEIOS_CALL(void) pantheios_uninit()
#ifdef _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS
{
    pantheios_uninit__cpp();
}
void pantheios_uninit__cpp()
#endif /* _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS */
{
#ifdef PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS

    // If we can make the (un)initialisation thread-safe, without
    // expense, then we do so.
    platformstl::spin_mutex                         mx(&s_mx);
    stlsoft::lock_scope<platformstl::spin_mutex>    lock(mx);

    if(0 == --s_apiInit)
    {
        pantheios_uninit_onetime();
    }

#else /* ? PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS */

    bool    bLast = false;

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(NULL != s_pmxApi, "api mutex must be initialised");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(0 != s_apiInit, "api initialisation must be counted");

    {   stlsoft::lock_scope<thread_mutex_t>  lock(*s_pmxApi);

        if(0 == --s_apiInit)
        {
            bLast = true;
        }
    }

    if(bLast)
    {
        PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API(0 == s_apiInit, "api initialisation must be 0");

        s_pmxApi->~thread_mutex_t();
        s_pmxApi = NULL;

        s_pmxBEI->~thread_mutex_t();
        s_pmxBEI = NULL;

        s_pmxMem->~thread_mutex_t();
        s_pmxMem = NULL;

        PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API(NULL == s_pmxApi, "api mutex must be uninitialised");
        PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API(NULL == s_pmxBEI, "back-end id mutex must be uninitialised");
        PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_STATE_API(NULL == s_pmxMem, "thread pool mutex must be uninitialised");

        pantheios_uninit_onetime();
    }

#endif /* PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS */
}


/* Core severity test function */
PANTHEIOS_CALL(int) pantheios_isSeverityLogged(pan_sev_t severity)
#ifdef _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS
{
    return pantheios_isSeverityLogged__cpp(severity);
}
int pantheios_isSeverityLogged__cpp(pan_sev_t severity)
#endif /* _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS */
{
    return pantheios_fe_isSeverityLogged(s_feToken, static_cast<int>(severity), 0);
}

/* Core-exposed process identity. */
PANTHEIOS_CALL(pan_char_t const*) pantheios_getProcessIdentity()
#ifdef _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS
{
    return pantheios_getProcessIdentity__cpp();
}
int pantheios_getProcessIdentity__cpp()
#endif /* _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS */
{
    if(NULL != s_internalProcessIdentity)
    {
        // use cached

        return s_internalProcessIdentity;
    }
    else
    {
        return pantheios_fe_getProcessIdentity(s_feToken);
    }
}

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace core
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* Core diagnostic logging functions */

/* dispatching for *all* diagnostic logging functions */
PANTHEIOS_CALL(int) pantheios_dispatch(
    pan_sev_t           severity
,   size_t              cchEntry
,   pan_char_t const*   entry
)
#ifdef _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS
{
    return pantheios_dispatch__cpp(severity, numSlices, slices);
}
int pantheios_dispatch__cpp(
    pan_sev_t           severity
,   size_t              cchEntry
,   pan_char_t const*   entry
)
#endif /* _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS */
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_INTERNAL(NULL != entry, "entry may not be null");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_INTERNAL('\0' == entry[cchEntry], "entry must be nul-terminated, and equal the given length");

    return pantheios_be_logEntry(s_feToken, s_beToken, severity, entry, cchEntry);
}

/* assembler for all log_???? functions */
PANTHEIOS_CALL(int) pantheios_log_n(    pan_sev_t           severity
                                    ,   size_t              numSlices
                                    ,   pan_slice_t const*  slices)
#ifdef _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS
{
    return pantheios_log_n__cpp(severity, numSlices, slices);
}
int pantheios_log_n__cpp(   pan_sev_t           severity
                        ,   size_t              numSlices
                        ,   pan_slice_t const*  slices)
#endif /* _PANTHEIOS_COMPILER_REQUIRES_EXTERNCPP_DEFINITIONS */
{
    // Precondition check
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API((NULL != slices || 0 == numSlices), "slices may only be null if the number of slices is zero");

#if defined(PANTHEIOS_NO_NAMESPACE)
    typedef auto_buffer_selector<
#else /* ? PANTHEIOS_NO_NAMESPACE */
    typedef ::pantheios::util::auto_buffer_selector<
#endif /* PANTHEIOS_NO_NAMESPACE */
        pan_char_t
    ,   1 + LOG_STACK_BUFFER_SIZE
    >::type                                 buffer_t;

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        // Calculate the total size of the log statement, by summation of the slice array

#if !defined(STLSOFT_COMPILER_IS_BORLAND)

        // The sophisticated way
        const size_t    n   =   std::accumulate(stlsoft::member_selector(slices, &pan_slice_t::len)
                                            ,   stlsoft::member_selector(slices + numSlices, &pan_slice_t::len)
                                            ,   size_t(0));

#else /* ? compiler */

        // The crappy way, for less-than compilers
        size_t n = 0;

        { for(size_t i = 0; i != numSlices; ++i)
        {
            n += slices[i].len;
        }}

#endif /* compiler */

        buffer_t        buffer(1 + n);
        size_t          nWritten    =   0;

        // We do a check here, so as to cater for cases where the allocator does not
        // throw bad_alloc.
        if(0 == buffer.size())
        {
            return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
        }
        else
        {
            std::copy(  slices
                    ,   slices + numSlices
                    ,   stlsoft::cstring_concatenator(&buffer[0], &nWritten));

            PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_INTERNAL(nWritten == n, "Written length differs from allocated length");

            buffer[n] = '\0';

            return pantheios_dispatch(static_cast<int>(severity), n, &buffer[0]);
        }
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc&)
    {
        return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
    }
    catch(std::exception&)
    {
        return PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
    }
    catch(...)
    {
        return PANTHEIOS_INIT_RC_UNKNOWN_FAILURE;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

#if 1 && \
    (   !defined(STLSOFT_COMPILER_IS_BORLAND) || \
        __BORLANDC__ < 0x0560) && \
    !defined(STLSOFT_COMPILER_IS_COMO) && \
    (   !defined(STLSOFT_COMPILER_IS_MSVC) || \
        _MSC_VER < 1300) && \
    (   !defined(STLSOFT_COMPILER_IS_INTEL) || \
        __INTEL_COMPILER < 1200) && \
    1
    return 0;
#endif /* compiler */
}


#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace core */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * printf() functions
 */

PANTHEIOS_CALL(int)
pantheios_logvprintf(
    pan_sev_t           severity
,   pan_char_t const*   format
,   va_list             args
)
{
#if !defined(PANTHEIOS_NO_NAMESPACE)
    using pantheios::core::pantheios_dispatch;
#endif /* !PANTHEIOS_NO_NAMESPACE */

    if(pantheios_isSeverityLogged(severity))
    {
        /* The standard requires that the return value is either:
         *
         * - the number of characters written, if that is < cch
         * - the number of characters that would have been written, if >= cch
         * - a negative value if an error occurs
         *
         * However, some implementations return a negative value in the
         * second case. Consequently, the following code has to be a little
         * bit more intelligent:
         *
         * - write a leading '\0' before calling vsnprintf
         * - write a trailing '\0' after calling vsnprintf
         */
        pan_char_t  sz[PRINTF_STACK_BUFFER_SIZE + 1] = { '\0' };
        int         cch = pantheios_util_vsnprintf(&sz[0], STLSOFT_NUM_ELEMENTS(sz) - 1, format, args);

        sz[STLSOFT_NUM_ELEMENTS(sz) - 1] = '\0';

        if(cch < 0)
        {
            cch = static_cast<int>(pan_strlen_(sz));
        }
        else if(cch >= int(STLSOFT_NUM_ELEMENTS(sz) - 1))
        {
            cch = STLSOFT_NUM_ELEMENTS(sz) - 1;
        }

        return pantheios_dispatch(severity, static_cast<size_t>(cch), &sz[0]);
    }

    return 0;
}

/* /////////////////////////////////////////////////////////////////////////
 * Core functions
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace core
{
#endif /* !PANTHEIOS_NO_NAMESPACE */


PANTHEIOS_CALL(int) pantheios_isInitialising(void)
{
    return s_isInitialising;
}

PANTHEIOS_CALL(int) pantheios_isInitialised(void)
{
    return 0 != s_apiInit;
}

PANTHEIOS_CALL(int) pantheios_getNextBackEndId(void)
{
#ifdef PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS

    static platformstl::atomic_int_t    s_nextId    =   PANTHEIOS_CORE_BACKENDID_BASE;

    return static_cast<int>(platformstl::atomic_preincrement(&s_nextId));

#else /* ? PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS */

    static int                          s_nextId    =   PANTHEIOS_CORE_BACKENDID_BASE;

    stlsoft::lock_scope<thread_mutex_t> lock(*s_pmxBEI);

    return ++s_nextId;
#endif /* PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS */
}

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace core */
#endif /* !PANTHEIOS_NO_NAMESPACE */

PANTHEIOS_CALL(void)
pantheios_logassertfail(
    pan_sev_t   severity
,   char const* fileLine
,   char const* message
)
{
#if !defined(PANTHEIOS_NO_NAMESPACE)
    using pantheios::core::pantheios_dispatch;
#endif /* !PANTHEIOS_NO_NAMESPACE */

    if(pantheios_isSeverityLogged(severity))
    {
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
        try
        {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

# if defined(PANTHEIOS_NO_NAMESPACE)
            typedef auto_buffer_selector<
# else /* ? PANTHEIOS_NO_NAMESPACE */
            typedef ::pantheios::util::auto_buffer_selector<
# endif /* PANTHEIOS_NO_NAMESPACE */
                char
             ,   1 + LOG_STACK_BUFFER_SIZE
             >::type                        buffer_t;

            size_t const    cchFileLine =   stlsoft::c_str_len(fileLine);
            size_t const    cchMessage  =   stlsoft::c_str_len(message);
            buffer_t        buff(1 + cchFileLine + cchMessage);

            if(buff.empty())
            {
                goto logassertfail_no_memory;
            }
            else
            {
                ::memcpy(&buff[0] + 0u,          fileLine, sizeof(char) * cchFileLine);
                ::memcpy(&buff[0] + cchFileLine, message,  sizeof(char) * cchMessage);
                buff[cchFileLine + cchMessage] = '\0';

#ifdef PANTHEIOS_USE_WIDE_STRINGS

                stlsoft::m2w    message2(buff.data(), cchFileLine + cchMessage);

                pantheios_dispatch(severity, message2.size(), message2.data());

#else /* ? PANTHEIOS_USE_WIDE_STRINGS */

                pantheios_dispatch(severity, cchFileLine + cchMessage, buff.data());

#endif /* PANTHEIOS_USE_WIDE_STRINGS */
            }

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
        }
        catch(std::bad_alloc&)
        {
            goto logassertfail_no_memory;
        }
        catch(...)
        {
            pantheios_onBailOut4(PANTHEIOS_SEV_EMERGENCY, "assertion failed", NULL, NULL);
        }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return;

logassertfail_no_memory:

        pantheios_onBailOut4(PANTHEIOS_SEV_EMERGENCY, "out-of-memory condition occurred when reporting assertion failure at", NULL, fileLine);
    }
}

/* /////////////////////////////////////////////////////////////////////////
 * Memory functions
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace core
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

PANTHEIOS_CALL(void*) pantheios_malloc(size_t cb)
{
#ifdef PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS

    platformstl::spin_mutex                         mx(&s_mxMem);
    stlsoft::lock_scope<platformstl::spin_mutex>    lock(mx);

#else /* ? PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS */

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(NULL != s_pmxMem, "memory pool mutex must be initialised");

    stlsoft::lock_scope<thread_mutex_t>             lock(*s_pmxMem);

#endif /* PANTHEIOS_MT_HAS_ATOMIC_INTEGER_OPERATIONS */

    memory_pool_entry_t* entry = memory_pool_entry_t::alloc(cb, s_memPoolHead);

    if(NULL != entry)
    {
        s_memPoolHead = entry;

        return &entry->bytes[0];
    }

    return NULL;
}

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace core */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Inserter memory functions
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace core
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

PANTHEIOS_CALL(void*) pantheios_inserterAllocate(size_t cb)
{
    return ::malloc(cb);
}

PANTHEIOS_CALL(void) pantheios_inserterDeallocate(void* pv)
{
    ::free(pv);
}

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace core */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Pad functions
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace ximpl_core
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* To be declared in pantheios.core.h */
void pantheios_initPad_(void)
{
    std::fill_n(s_padCharacters, STLSOFT_NUM_ELEMENTS(s_padCharacters) - 1u, ' ');
    s_padCharacters[STLSOFT_NUM_ELEMENTS(s_padCharacters) - 1u] = '\0';
}

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace ximpl_core */
#endif /* !PANTHEIOS_NO_NAMESPACE */


#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace core
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

PANTHEIOS_CALL(pan_char_t const*) pantheios_getPad(size_t minimumWidth, size_t* actualWidth)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(NULL != actualWidth, "pointer cannot be null");

    if(minimumWidth > (STLSOFT_NUM_ELEMENTS(s_padCharacters) - 1u))
    {
        *actualWidth = (STLSOFT_NUM_ELEMENTS(s_padCharacters) - 1u);
    }
    else
    {
        *actualWidth = minimumWidth;
    }

    return s_padCharacters;
}

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace core */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Back-end map functions
 */

#ifdef PANTHEIOS_DEFINE_BACK_END_MAP

PANTHEIOS_CALL(int) pantheios_backEndMap_lookup(int backEndId, void** ptoken)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(NULL != s_backEndMap, "Cannot be called when Pantheios core is not initialised");

    return s_backEndMap->lookup(backEndId, ptoken);
}

PANTHEIOS_CALL(int) pantheios_backEndMap_add(int backEndId, void* token)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(NULL != s_backEndMap, "Cannot be called when Pantheios core is not initialised");

    return s_backEndMap->add(backEndId, token);
}

PANTHEIOS_CALL(int) pantheios_backEndMap_remove(int backEndId)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(NULL != s_backEndMap, "Cannot be called when Pantheios core is not initialised");

    return s_backEndMap->remove(backEndId);
}
#endif /* PANTHEIOS_DEFINE_BACK_END_MAP */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
