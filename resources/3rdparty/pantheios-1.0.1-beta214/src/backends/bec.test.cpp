/* /////////////////////////////////////////////////////////////////////////
 * File:        src/backends/bec.test.cpp
 *
 * Purpose:     Implementation for the be.test back-end
 *
 * Created:     1st November 2006
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


/* Warning suppressions */
#include <stlsoft/stlsoft.h>
#if defined(STLSOFT_COMPILER_IS_MSVC)
# if defined(STLSOFT_CF_EXCEPTION_SUPPORT)
#  pragma warning(disable : 4702)
# else /* ? STLSOFT_CF_EXCEPTION_SUPPORT */
# endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
#endif /* compiler */

/* Pantheios.Test Header files */
#include <pantheios/internal/nox.h>
#define PANTHEIOS_BE_INIT_NO_CPP_STRUCT_INIT
#include <pantheios/backends/bec.test.h>

/* Pantheios Header files */

#include <pantheios/init_codes.h>
#include <pantheios/frontends/stock.h>
#include <pantheios/backend.h>
#include <pantheios/quality/contract.h>
#include <pantheios/internal/threading.h>

/* STLSoft Header files */

#ifdef PANTHEIOS_MT
# include <platformstl/synch/thread_mutex.hpp>
#else /* ? PANTHEIOS_MT */
# include <stlsoft/synch/null_mutex.hpp>
#endif /* PANTHEIOS_MT */

#include <stlsoft/conversion/integer_to_string.hpp>
#include <stlsoft/shims/access/string.hpp>
#include <stlsoft/synch/lock_scope.hpp>

/* Standard C/C++ Header files */

#include <vector>

/* Warning suppressions */
#if defined(STLSOFT_COMPILER_IS_MSVC)
# if defined(STLSOFT_CF_EXCEPTION_SUPPORT)
# else /* ? STLSOFT_CF_EXCEPTION_SUPPORT */
# endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

namespace pantheios
{
namespace be
{
namespace test
{
    /* static */ Time Time::now()
    {
        return Time();
    }

    Entry::Entry(
        int                 severity
    ,   pan_char_t const*   entry
    ,   size_t              cchEntry
    )
        : time(Time::now())
        , severity(severity)
        , statement(entry, cchEntry)
    {}

    namespace impl
    {
        class Context
        {
        public:
            typedef Context                     class_type;
            typedef Entry                       value_type;
            typedef std::vector<value_type>     entries_type;
        private:
#ifdef PANTHEIOS_MT
            typedef platformstl::thread_mutex   mutex_type_;
#else /* ? PANTHEIOS_MT */
            typedef stlsoft::null_mutex         mutex_type_;
#endif /* PANTHEIOS_MT */
        public:
            Context(pan_char_t const* processIdentity, int backEndId)
                : m_processIdentity(processIdentity)
                , m_backEndId(backEndId)
            {}

        public:
            void logEntry(
                void*               /* feToken */
            ,   int                 severity
            ,   pan_char_t const*   entry
            ,   size_t              cchEntry
            )
            {
                stlsoft::lock_scope<mutex_type_> lock(m_mx);

                // Get the time
                // Get the process Id

                m_entries.push_back(Entry(severity, entry, cchEntry));
            }

        public:
            void reset()
            {
                stlsoft::lock_scope<mutex_type_> lock(m_mx);

                entries_type dummy; // Have to use explicit dummy variable, otherwise CW 8 spits (the dummy, that is)

                m_entries.swap(dummy);
            }

        public:
            void lock()
            {
                m_mx.lock();
            }
            void unlock()
            {
                m_mx.unlock();
            }

        public:
            entries_type const& entries() const
            {
                return m_entries;
            }

/*
            size_t  size() const
            {
                stlsoft::lock_scope<mutex_type_> lock(m_mx);

                return m_entries.size();
            }
            value_type const&   operator [](size_t index) const
            {
                stlsoft::lock_scope<mutex_type_> lock(m_mx);

                return m_entries[index];
            }
*/

        private: // Member Variables
            const Entry::string_type    m_processIdentity;
            const int                   m_backEndId;
            entries_type                m_entries;
            mutex_type_                 m_mx;

        private: // Not to be implemented
            Context(class_type const&);
            class_type& operator =(class_type const&);
        };

    } // namespace impl

    struct Results::ResultsImpl
    {
    public:
        typedef ResultsImpl                 class_type;
        typedef Results::value_type         value_type;
        typedef std::vector<value_type>     entries_type;

    public:
        ResultsImpl(entries_type const& entries)
            : m_refCount(1)
            , m_entries(entries)
        {}

    public: // Reference-counting
        void    AddRef()
        {
            ++m_refCount;
        }
        void    Release()
        {
            if(0 == --m_refCount)
            {
                delete this;
            }
        }

    public: // Accessors
        bool empty() const
        {
            return m_entries.empty();
        }
        size_t size() const
        {
            return m_entries.size();
        }
        value_type const&   operator [](size_t index) const
        {
            return m_entries[index];
        }
    private:
        stlsoft::int32_t    m_refCount;
        const entries_type  m_entries;

    private: // Not to be implemented
        class_type& operator =(class_type const&);
    };

    Results::Results(ResultsImpl* impl)
        : m_impl(impl)
    {
        PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_INTERNAL(NULL != impl, "implementation class pointer may not be null");
    }

    Results::Results(Results::class_type const& rhs)
        : m_impl(rhs.m_impl)
    {
        PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_INTERNAL(NULL != rhs.m_impl, "implementation class pointer of rhs may not be null");

        m_impl->AddRef();
    }
    Results::~Results() stlsoft_throw_0()
    {
        PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_INTERNAL(NULL != m_impl, "implementation class pointer may not be null");

        m_impl->Release();
    }
    //Results::class_type& Results::operator= (Results::class_type const& rhs)
    //{
    //  PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_INTERNAL(NULL != rhs.m_impl, "implementation class pointer of rhs may not be null");
    //  PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_INTERNAL(NULL != m_impl, "implementation class pointer may not be null");

    //  ResultsImpl*    old = m_impl;

    //  m_impl = rhs.m_impl;
    //  m_impl->AddRef();

    //  old->Release();

    //  return *this;
    //}


    bool Results::empty() const
    {
        return m_impl->empty();
    }
    size_t Results::size() const
    {
        return m_impl->size();
    }
    Results::value_type const& Results::operator [](size_t index) const
    {
        return (*m_impl)[index];
    }

    class CreatableResults
        : public Results
    {
    public:
        typedef Results                             parent_class_type;
        typedef CreatableResults                    class_type;
        typedef Results::ResultsImpl::entries_type  entries_type;

    public:
        CreatableResults(entries_type const& entries)
            : parent_class_type(create_impl_(entries))
        {}

    private:
        static Results::ResultsImpl* create_impl_(entries_type const& entries)
        {
            return new Results::ResultsImpl(entries);
        }
    };

} // namespace test
} // namespace be
} // namespace pantheios


namespace
{

    ::pantheios::be::test::impl::Context* s_ctxt;

} // anonymous namespace


/* /////////////////////////////////////////////////////////////////////////
 * API
 */

namespace pantheios
{
namespace be
{
namespace test
{

    void reset()
    {
        s_ctxt->reset();
    }

    Results results()
    {
        return CreatableResults(s_ctxt->entries());
    }

} // namespace test
} // namespace be
} // namespace pantheios

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

namespace
{

#if !defined(PANTHEIOS_NO_NAMESPACE)

    using ::pantheios::pan_char_t;

#endif /* !PANTHEIOS_NO_NAMESPACE */

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * Front-end & Back-end Implementations
 */

PANTHEIOS_CALL(int) pantheios_be_test_init(
    pan_char_t const*   processIdentity
,   int                 id
,   void*               /* unused */
,   void*               /* reserved */
,   void**              ptoken
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != processIdentity, "process identity may not be the null string");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API('\0' != 0[processIdentity], "process identity may not be the empty string");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != ptoken, "token pointer may not be null");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(NULL == s_ctxt, "pantheios_be_init() can only be called once per-process");

    *ptoken = NULL;

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        s_ctxt = new pantheios::be::test::impl::Context(processIdentity, id);

        if(NULL == s_ctxt)
        {
            goto out_of_memory;
        }

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        goto out_of_memory;
    }
    catch(std::exception& /* x */)
    {
        return PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
    }
    catch(...)
    {
        return PANTHEIOS_INIT_RC_UNKNOWN_FAILURE;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

    return 0;

out_of_memory:

    return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
}

PANTHEIOS_CALL(void) pantheios_be_test_uninit(void* /* token */)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(NULL != s_ctxt, "pantheios_be_init() must be called after pantheios_be_init() has successfully initialised");

    delete s_ctxt;
    s_ctxt = NULL;
}

PANTHEIOS_CALL(int) pantheios_be_test_logEntry(
    void*               feToken
,   void*               /* beToken */
,   int                 severity
,   pan_char_t const*   entry
,   size_t              cchEntry
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_STATE_API(NULL != s_ctxt, "pantheios_fe_init() must be called first");

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        s_ctxt->logEntry(feToken, severity, entry, cchEntry);

        return 0;

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc& /* x */)
    {
        return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
    }
    catch(std::exception& /* x */)
    {
        return PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
    }
    catch(...)
    {
        return PANTHEIOS_INIT_RC_UNKNOWN_FAILURE;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

/* ///////////////////////////// end of file //////////////////////////// */
