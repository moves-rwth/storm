/*
    Copyright 2005-2013 Intel Corporation.  All Rights Reserved.

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

#if __TBB_CPF_BUILD
// undefine __TBB_CPF_BUILD to simulate user's setup
#undef __TBB_CPF_BUILD

#define TBB_PREVIEW_TASK_ARENA 1

#if !TBB_USE_EXCEPTIONS && _MSC_VER
    // Suppress "C++ exception handler used, but unwind semantics are not enabled" warning in STL headers
    #pragma warning (push)
    #pragma warning (disable: 4530)
#endif

#include <stdexcept>

#if !TBB_USE_EXCEPTIONS && _MSC_VER
    #pragma warning (pop)
#endif

#include <cstdlib>
#include <cstdio>

#include "tbb/task_arena.h"
#include "tbb/task_scheduler_observer.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "tbb/enumerable_thread_specific.h"

#include "harness_assert.h"
#include "harness.h"
#include "harness_barrier.h"
#include "harness_concurrency_tracker.h"

#if _MSC_VER
// plays around __TBB_NO_IMPLICIT_LINKAGE. __TBB_LIB_NAME should be defined (in makefiles)
    #pragma comment(lib, __TBB_STRING(__TBB_LIB_NAME))
#endif

//! Test that task_arena::initialize and task_arena::terminate work when doing nothing else.
/** maxthread is treated as the biggest possible concurrency level. */
void InitializeAndTerminate( int maxthread ) {
    __TBB_TRY {
        for( int i=0; i<200; ++i ) {
            switch( i&3 ) {
                // Arena is created inactive, initialization is always explicit. Lazy initialization is covered by other test functions.
                // Explicit initialization can either keep the original values or change those.
                // Arena termination can be explicit or implicit (in the destructor).
                // TODO: extend with concurrency level checks if such a method is added.
                // TODO: test for different master slot reservation values (perhaps in another function)
                default: {
                    tbb::task_arena arena( std::rand() % maxthread + 1 );
                    ASSERT(!arena.is_active(), "arena should not be active until initialized");
                    arena.initialize();
                    ASSERT(arena.is_active(), NULL);
                    arena.terminate();
                    ASSERT(!arena.is_active(), "arena should not be active; it was terminated");
                    break;
                }
                case 0: {   
                    tbb::task_arena arena( 1 );
                    ASSERT(!arena.is_active(), "arena should not be active until initialized");
                    arena.initialize( std::rand() % maxthread + 1 ); // change the parameters
                    ASSERT(arena.is_active(), NULL);
                    break;
                }
                case 1: {
                    tbb::task_arena arena( tbb::task_arena::automatic );
                    ASSERT(!arena.is_active(), NULL);
                    arena.initialize();
                    ASSERT(arena.is_active(), NULL);
                    break;
                }
                case 2: {
                    tbb::task_arena arena;
                    ASSERT(!arena.is_active(), "arena should not be active until initialized");
                    arena.initialize( std::rand() % maxthread + 1 );
                    ASSERT(arena.is_active(), NULL);
                    arena.terminate();
                    ASSERT(!arena.is_active(), "arena should not be active; it was terminated");
                    break;
                }
            }
        }
    } __TBB_CATCH( std::runtime_error& error ) {
#if TBB_USE_EXCEPTIONS
        REPORT("ERROR: %s\n", error.what() );
#endif /* TBB_USE_EXCEPTIONS */
    }
}

typedef tbb::blocked_range<int> Range;

Harness::SpinBarrier our_barrier;

static tbb::enumerable_thread_specific<int> local_id, old_id, slot_id(-1);
void ResetTLS() {
    local_id.clear();
    old_id.clear();
    slot_id.clear();
}

class ConcurrencyTrackingBody {
public:
    void operator() ( const Range& ) const {
        ASSERT(slot_id.local() == tbb::task_arena::current_slot(), NULL);
        Harness::ConcurrencyTracker ct;
        for ( volatile int i = 0; i < 100000; ++i )
            ;
    }
};

class ArenaObserver : public tbb::task_scheduler_observer {
    int myId;
    tbb::atomic<int> myTrappedSlot;
    /*override*/
    void on_scheduler_entry( bool is_worker ) {
        REMARK("a %s #%p is entering arena %d from %d\n", is_worker?"worker":"master", &local_id.local(), myId, local_id.local());
        ASSERT(!old_id.local(), "double-call to on_scheduler_entry");
        old_id.local() = local_id.local();
        ASSERT(old_id.local() != myId, "double-entry to the same arena");
        local_id.local() = myId;
        slot_id.local() = tbb::task_arena::current_slot();
        if(is_worker) ASSERT(tbb::task_arena::current_slot()>0, NULL);
        else ASSERT(tbb::task_arena::current_slot()==0, NULL);
    }
    /*override*/
    void on_scheduler_exit( bool is_worker ) {
        REMARK("a %s #%p is leaving arena %d to %d\n", is_worker?"worker":"master", &local_id.local(), myId, old_id.local());
        ASSERT(local_id.local() == myId, "nesting of arenas is broken");
        ASSERT(slot_id.local() == tbb::task_arena::current_slot(), NULL);
        slot_id.local() = -1;
        local_id.local() = old_id.local();
        old_id.local() = 0;
    }
    /*override*/
    bool on_scheduler_leaving() {
        ASSERT(slot_id.local() == tbb::task_arena::current_slot(), NULL);
        return tbb::task_arena::current_slot() >= myTrappedSlot;
    }
public:
    ArenaObserver(tbb::task_arena &a, int id, int trap = 0) : tbb::task_scheduler_observer(a) {
        ASSERT(id, NULL);
        myId = id;
        myTrappedSlot = trap;
        observe(true);
    }
    ~ArenaObserver () {
        ASSERT(!old_id.local(), "inconsistent observer state");
    }
};

struct AsynchronousWork : NoAssign {
    Harness::SpinBarrier &my_barrier;
    bool my_is_blocking;
    AsynchronousWork(Harness::SpinBarrier &a_barrier, bool blocking = true)
    : my_barrier(a_barrier), my_is_blocking(blocking) {}
    void operator()() const {
        ASSERT(local_id.local() != 0, "not in explicit arena");
        tbb::parallel_for(Range(0,500), ConcurrencyTrackingBody(), tbb::simple_partitioner());
        if(my_is_blocking) my_barrier.timed_wait(10); // must be asynchronous to master thread
        else my_barrier.signal_nowait();
    }
};

void TestConcurrentArenas(int p) {
    //Harness::ConcurrencyTracker::Reset();
    tbb::task_arena a1;
    a1.initialize(1,0);
    ArenaObserver o1(a1, p*2+1);
    tbb::task_arena a2(2,1);
    ArenaObserver o2(a2, p*2+2);
    Harness::SpinBarrier barrier(2);
    AsynchronousWork work(barrier);
    a1.enqueue(work); // put async work
    barrier.timed_wait(10);
    a2.enqueue(work); // another work
    a2.execute(work); // my_barrier.timed_wait(10) inside
    a1.wait_until_empty();
    a2.wait_until_empty();
}

class MultipleMastersBody : NoAssign {
    int mode;
    tbb::task_arena &my_a;
    Harness::SpinBarrier &my_b;
public:
    MultipleMastersBody(int m, tbb::task_arena &a, Harness::SpinBarrier &b)
    : mode(m), my_a(a), my_b(b) {}
    void operator()(int) const {
        my_a.execute(AsynchronousWork(my_b, /*blocking=*/false));
        if( mode == 0 ) {
            my_a.wait_until_empty();
            // A regression test for bugs 1954 & 1971
            my_a.enqueue(AsynchronousWork(my_b, /*blocking=*/false));
        }
    }
};


void TestMultipleMasters(int p) {
    {
        REMARK("multiple masters, part 1\n");
        tbb::task_arena a(1,0);
        a.initialize();
        ArenaObserver o(a, 1);
        Harness::SpinBarrier barrier(2*p+1); // each of p threads will submit two tasks signaling the barrier
        NativeParallelFor( p, MultipleMastersBody(0, a, barrier) );
        barrier.timed_wait(10);
        a.wait_until_empty();
    } {
        REMARK("multiple masters, part 2\n");
        tbb::task_arena a(2,1);
        ArenaObserver o(a, 2);
        Harness::SpinBarrier barrier(p+2);
        a.enqueue(AsynchronousWork(barrier, /*blocking=*/true)); // occupy the worker, a regression test for bug 1981
        NativeParallelFor( p, MultipleMastersBody(1, a, barrier) );
        barrier.timed_wait(10);
        a.wait_until_empty();
    }
}


int TestMain () {
    // TODO: a workaround for temporary p-1 issue in market
    tbb::task_scheduler_init init_market_p_plus_one(MaxThread+1);
    InitializeAndTerminate(MaxThread);
    for( int p=MinThread; p<=MaxThread; ++p ) {
        REMARK("testing with %d threads\n", p );
        NativeParallelFor( p, &TestConcurrentArenas );
        ResetTLS();
        TestMultipleMasters( p );
        ResetTLS();
    }
    return Harness::Done;
}
#else // __TBB_CPF_BUILD
#include "harness.h"
int TestMain () {
    return Harness::Skipped;
}
#endif
