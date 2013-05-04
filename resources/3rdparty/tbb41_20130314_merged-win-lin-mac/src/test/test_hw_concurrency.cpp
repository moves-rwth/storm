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

#include "harness_defs.h"

#if __TBB_TEST_SKIP_AFFINITY
#define HARNESS_NO_PARSE_COMMAND_LINE 1
#include "harness.h"
int TestMain() {
    return Harness::Skipped;
}
#else /* affinity mask can be set and used by TBB */

#include "harness.h"

#include <limits.h>

#if _WIN32||_WIN64
#include "tbb/machine/windows_api.h"
#elif __linux__
#include <unistd.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <sched.h>
#elif __FreeBSD__
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/param.h>  // Required by <sys/cpuset.h>
#include <sys/cpuset.h>
#endif

#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_thread.h"
#include "tbb/enumerable_thread_specific.h"

// The declaration of a global ETS object is needed to check that
// it does not initialize the task scheduler, and in particular
// does not set the default thread number. TODO: add other objects
// that should not initialize the scheduler.
tbb::enumerable_thread_specific<std::size_t> ets;

int TestMain () {
#if _WIN32||_WIN64
    SYSTEM_INFO si;
    GetNativeSystemInfo(&si);
    if ( si.dwNumberOfProcessors < 2 )
        return Harness::Skipped;
    int availableProcs = (int)si.dwNumberOfProcessors / 2;
    DWORD_PTR mask = 1;
    for ( int i = 1; i < availableProcs; ++i )
        mask |= mask << 1;
    bool err = !SetProcessAffinityMask( GetCurrentProcess(), mask );
#else /* !WIN */
#if __linux__
    int maxProcs = get_nprocs();
    typedef cpu_set_t mask_t;
#if __TBB_MAIN_THREAD_AFFINITY_BROKEN
    #define setaffinity(mask) sched_setaffinity(0 /*get the mask of the calling thread*/, sizeof(mask_t), &mask)
#else
    #define setaffinity(mask) sched_setaffinity(getpid(), sizeof(mask_t), &mask)
#endif
#else /* __FreeBSD__ */
    int maxProcs = sysconf(_SC_NPROCESSORS_ONLN);
    typedef cpuset_t mask_t;
#if __TBB_MAIN_THREAD_AFFINITY_BROKEN
    #define setaffinity(mask) cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_TID, -1, sizeof(mask_t), &mask)
#else
    #define setaffinity(mask) cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, -1, sizeof(mask_t), &mask)
#endif
#endif /* __FreeBSD__ */
    if ( maxProcs < 2 )
        return Harness::Skipped;
    mask_t newMask;
    CPU_ZERO(&newMask);
    int availableProcs = min(maxProcs, (int)sizeof(mask_t) * CHAR_BIT) / 2;
    for ( int i = 0; i < availableProcs; ++i )
        CPU_SET( i, &newMask );
    int err = setaffinity( newMask );
#endif /* !WIN */
    ASSERT( !err, "Setting process affinity failed" );
    ASSERT( tbb::task_scheduler_init::default_num_threads() == availableProcs, NULL );
    ASSERT( (int)tbb::tbb_thread::hardware_concurrency() == availableProcs, NULL );
    return Harness::Done;
}
#endif /* __TBB_TEST_SKIP_AFFINITY */
