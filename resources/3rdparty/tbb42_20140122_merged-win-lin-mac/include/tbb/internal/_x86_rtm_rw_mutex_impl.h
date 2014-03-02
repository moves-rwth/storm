/*
    Copyright 2005-2014 Intel Corporation.  All Rights Reserved.

    This file is part of Threading Building Blocks.

    Threading Building Blocks is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.

    Threading Building Blocks is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Threading Building Blocks; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/

#ifndef __TBB__x86_rtm_rw_mutex_impl_H
#define __TBB__x86_rtm_rw_mutex_impl_H

#ifndef __TBB_spin_rw_mutex_H
#error Do not #include this internal file directly; use public TBB headers instead.
#endif

#if TBB_PREVIEW_SPECULATIVE_SPIN_RW_MUTEX
#if __TBB_TSX_AVAILABLE

#include "tbb/tbb_stddef.h"
#include "tbb/tbb_machine.h"
#include "tbb/tbb_profiling.h"
#include "tbb/spin_rw_mutex.h"

namespace tbb {
namespace interface7 {
namespace internal {

enum RTM_type {
    RTM_not_in_mutex,
    RTM_transacting_reader,
    RTM_transacting_writer,
    RTM_real_reader,
    RTM_real_writer
};

static const unsigned long speculation_granularity = 64;

//! Fast, unfair, spinning speculation-enabled reader-writer lock with backoff and
//  writer-preference
/** @ingroup synchronization */
class x86_rtm_rw_mutex: private spin_rw_mutex {
public:
// bug in gcc 3.x.x causes syntax error in spite of the friend declaration above.
// Make the scoped_lock public in that case.
#if __TBB_USE_X86_RTM_RW_MUTEX || __TBB_GCC_VERSION < 40000
#else
private:
#endif
    friend class padded_mutex<x86_rtm_rw_mutex,true>;
    class scoped_lock;   // should be private 
private:
    //! @cond INTERNAL

    //! Internal acquire write lock.
    // only_speculate == true if we're doing a try_lock, else false.
    void __TBB_EXPORTED_METHOD internal_acquire_writer(x86_rtm_rw_mutex::scoped_lock&, bool only_speculate=false);

    //! Internal acquire read lock.
    // only_speculate == true if we're doing a try_lock, else false.
    void __TBB_EXPORTED_METHOD internal_acquire_reader(x86_rtm_rw_mutex::scoped_lock&, bool only_speculate=false);

    //! Internal upgrade reader to become a writer.
    bool __TBB_EXPORTED_METHOD internal_upgrade( x86_rtm_rw_mutex::scoped_lock& );

    //! Out of line code for downgrading a writer to a reader.
    void __TBB_EXPORTED_METHOD internal_downgrade( x86_rtm_rw_mutex::scoped_lock& );

    //! Internal try_acquire write lock.
    bool __TBB_EXPORTED_METHOD internal_try_acquire_writer( x86_rtm_rw_mutex::scoped_lock& );

    //! Internal release read lock.
    void internal_release_reader( x86_rtm_rw_mutex::scoped_lock& );

    //! Out of line code for releasing a write lock.
    void internal_release_writer(x86_rtm_rw_mutex::scoped_lock& );

    //! @endcond
public:
    //! Construct unacquired mutex.
    x86_rtm_rw_mutex() {
        w_flag = false;
#if TBB_USE_THREADING_TOOLS
        internal_construct();
#endif
    }

#if TBB_USE_ASSERT
    //! Empty destructor.
    ~x86_rtm_rw_mutex() {}
#endif /* TBB_USE_ASSERT */

    // Mutex traits
    static const bool is_rw_mutex = true;
    static const bool is_recursive_mutex = false;
    static const bool is_fair_mutex = false;

#if __TBB_USE_X86_RTM_RW_MUTEX || __TBB_GCC_VERSION < 40000
#else
    // by default we will not provide the scoped_lock interface.  The user
    // should use the padded version of the mutex.  scoped_lock is used in
    // padded_mutex template.
private:
#endif
    //! The scoped locking pattern
    /** It helps to avoid the common problem of forgetting to release lock.
        It also nicely provides the "node" for queuing locks. */
    // Speculation-enabled scoped lock for spin_rw_mutex
    // The idea is to be able to reuse the acquire/release methods of spin_rw_mutex
    // and its scoped lock wherever possible.  The only way to use a speculative lock is to use
    // a scoped_lock. (because transaction_state must be local)

    class scoped_lock {
        friend class x86_rtm_rw_mutex;
        spin_rw_mutex::scoped_lock my_scoped_lock;

        RTM_type transaction_state;

    public:
        //! Construct lock that has not acquired a mutex.
        /** Equivalent to zero-initialization of *this. */
        scoped_lock() : my_scoped_lock(), transaction_state(RTM_not_in_mutex) {
        }

        //! Acquire lock on given mutex.
        scoped_lock( x86_rtm_rw_mutex& m, bool write = true ) : my_scoped_lock(),
            transaction_state(RTM_not_in_mutex) {
            acquire(m, write);
        }

        //! Release lock (if lock is held).
        ~scoped_lock() {
            if(transaction_state != RTM_not_in_mutex) release();
        }

        //! Acquire lock on given mutex.
        void acquire( x86_rtm_rw_mutex& m, bool write = true ) {
            if( write ) m.internal_acquire_writer(*this);
            else        m.internal_acquire_reader(*this);
        }

        void __TBB_EXPORTED_METHOD release();

        //! Upgrade reader to become a writer.
        /** Returns whether the upgrade happened without releasing and re-acquiring the lock */
        bool upgrade_to_writer() {
            x86_rtm_rw_mutex* mutex = static_cast<x86_rtm_rw_mutex*>(my_scoped_lock.__internal_get_mutex());
            __TBB_ASSERT( mutex, "lock is not acquired" );
            return mutex->internal_upgrade(*this);
        }

        //! Downgrade writer to become a reader.
        bool downgrade_to_reader() {
            x86_rtm_rw_mutex* mutex = static_cast<x86_rtm_rw_mutex*>(my_scoped_lock.__internal_get_mutex());
            __TBB_ASSERT( mutex, "lock is not acquired" );
            mutex->internal_downgrade(*this);
            return true;  // real writer -> reader returns true, speculative only changes local state.
        }

        //! Attempt to acquire mutex.
        /** returns true if successful.  */
        bool try_acquire( x86_rtm_rw_mutex& m, bool write = true ) {
#if TBB_USE_DEBUG
            x86_rtm_rw_mutex* mutex = static_cast<x86_rtm_rw_mutex*>(my_scoped_lock.__internal_get_mutex());
            __TBB_ASSERT( !mutex, "holding mutex already" );
#endif
            // have to assign m to our mutex.
            // cannot set the mutex, because try_acquire in spin_rw_mutex depends on it being NULL.
            if(write) return m.internal_try_acquire_writer(*this);
            // speculatively acquire the lock.  If this fails, do try_acquire on the spin_rw_mutex.
            m.internal_acquire_reader(*this, /*only_speculate=*/true);
            if(transaction_state == RTM_transacting_reader) return true;
            if( my_scoped_lock.try_acquire(m, false)) {
                transaction_state = RTM_real_reader;
                return true;
            }
            return false;
        }

        };  // class x86_rtm_rw_mutex::scoped_lock

    // ISO C++0x compatibility methods not provided because we cannot maintain
    // state about whether a thread is in a transaction.

private:
    char pad[speculation_granularity-sizeof(spin_rw_mutex)]; // padding

    // If true, writer holds the spin_rw_mutex.
    tbb::atomic<bool> w_flag;  // want this on a separate cache line

    void __TBB_EXPORTED_METHOD internal_construct();
};  // x86_rtm_rw_mutex

}  // namespace internal
}  // namespace interface7
}  // namespace tbb

#endif  /* ( __TBB_x86_32 || __TBB_x86_64 ) */
#endif  /* TBB_PREVIEW_SPECULATIVE_SPIN_RW_MUTEX */
#endif /* __TBB__x86_rtm_rw_mutex_impl_H */
