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

/** This test checks the automatic propagation of master thread FPU settings
    into the worker threads. **/

#define TBB_PREVIEW_LOCAL_OBSERVER 1

#include "harness.h"
#include "tbb/parallel_for.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_machine.h"

#if __TBB_SCHEDULER_OBSERVER
#include "tbb/task_scheduler_observer.h"
#endif

const int N = 500000;

#if ( __TBB_x86_32 || __TBB_x86_64 ) && __TBB_CPU_CTL_ENV_PRESENT

const int FE_TONEAREST = 0x0000,
          FE_DOWNWARD = 0x0400,
          FE_UPWARD = 0x0800,
          FE_TOWARDZERO = 0x0c00,
          FE_RND_MODE_MASK = FE_TOWARDZERO,
          SSE_RND_MODE_MASK = FE_RND_MODE_MASK << 3,
          SSE_DAZ = 0x0040,
          SSE_FTZ = 0x8000,
          SSE_MODE_MASK = SSE_DAZ | SSE_FTZ;

const int NumSseModes = 4;
const int SseModes[NumSseModes] = { 0, SSE_DAZ, SSE_FTZ, SSE_DAZ | SSE_FTZ };

#if _WIN64 && !__TBB_X86_MSVC_INLINE_ASM_AVAILABLE && !__MINGW64__
// MinGW uses inline implementation from tbb/machine/linux_intel64.h

#include <float.h>

inline void __TBB_get_cpu_ctl_env ( __TBB_cpu_ctl_env_t* fe ) {
    fe->x87cw = short(_control87(0, 0) & _MCW_RC) << 2;
    fe->mxcsr = _mm_getcsr();
}
inline void __TBB_set_cpu_ctl_env ( const __TBB_cpu_ctl_env_t* fe ) {
    ASSERT( (fe->x87cw & FE_RND_MODE_MASK) == ((fe->x87cw & FE_RND_MODE_MASK) >> 2 & _MCW_RC) << 2, "Check float.h constants" );
    _control87( (fe->x87cw & FE_RND_MODE_MASK) >> 6, _MCW_RC );
    _mm_setcsr( fe->mxcsr );
}

#endif /*  _WIN64 && !__TBB_X86_MSVC_INLINE_ASM_AVAILABLE && !__MINGW64__ */

inline int GetRoundingMode ( bool checkConsistency = true ) {
    __TBB_cpu_ctl_env_t ctl = { 0, 0 };
    __TBB_get_cpu_ctl_env(&ctl);
    ASSERT( !checkConsistency || (ctl.mxcsr & SSE_RND_MODE_MASK) >> 3 == (ctl.x87cw & FE_RND_MODE_MASK), NULL );
    return ctl.x87cw & FE_RND_MODE_MASK;
}

inline void SetRoundingMode ( int mode ) {
    __TBB_cpu_ctl_env_t ctl = { 0, 0 };
    __TBB_get_cpu_ctl_env(&ctl);
    ctl.mxcsr = (ctl.mxcsr & ~SSE_RND_MODE_MASK) | (mode & FE_RND_MODE_MASK) << 3;
    ctl.x87cw = short((ctl.x87cw & ~FE_RND_MODE_MASK) | (mode & FE_RND_MODE_MASK));
    __TBB_set_cpu_ctl_env(&ctl);
}

inline int GetSseMode () {
    __TBB_cpu_ctl_env_t ctl = { 0, 0 };
    __TBB_get_cpu_ctl_env(&ctl);
    return ctl.mxcsr & SSE_MODE_MASK;
}

inline void SetSseMode ( int mode ) {
    __TBB_cpu_ctl_env_t ctl = { 0, 0 };
    __TBB_get_cpu_ctl_env(&ctl);
    ctl.mxcsr = (ctl.mxcsr & ~SSE_MODE_MASK) | (mode & SSE_MODE_MASK);
    __TBB_set_cpu_ctl_env(&ctl);
}


#else /* Other archs */

#include <fenv.h>

const int RND_MODE_MASK = FE_TONEAREST | FE_DOWNWARD | FE_UPWARD | FE_TOWARDZERO;

const int NumSseModes = 1;
const int SseModes[NumSseModes] = { 0 };

inline int GetRoundingMode ( bool = true ) { return fegetround(); }
inline void SetRoundingMode ( int rnd ) { fesetround(rnd); }

inline int GetSseMode () { return 0; }
inline void SetSseMode ( int ) {}

#endif /* Other archs */

const int NumRoundingModes = 4;
const int RoundingModes[NumRoundingModes] = { FE_TONEAREST, FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO };

class RoundingModeCheckBody {
    Harness::tid_t m_tidMaster;
    int m_masterMode;
    int m_workerMode;
    int m_masterSseMode;
    int m_workerSseMode;
public:
    void operator() ( int /*iter*/ ) const {
        if ( Harness::CurrentTid() == m_tidMaster ) {
            ASSERT( GetRoundingMode() == m_masterMode, "Master's FPU control state was corrupted" );
            ASSERT( GetSseMode() == m_masterSseMode, "Master's SSE control state was corrupted" );
        }
        else {
            ASSERT( GetRoundingMode() == m_workerMode, "FPU control state has not been propagated to a worker" );
            ASSERT( GetSseMode() == m_workerSseMode, "SSE control state has not been propagated to a worker" );
        }
    }

    RoundingModeCheckBody ( Harness::tid_t tidMaster, int masterMode, int workerMode, int masterSseMode, int workerSseMode )
        : m_tidMaster(tidMaster)
        , m_masterMode(masterMode)
        , m_workerMode(workerMode)
        , m_masterSseMode(masterSseMode)
        , m_workerSseMode(workerSseMode)
    {}
};

#if __TBB_SCHEDULER_OBSERVER
class LocalObserver : public tbb::task_scheduler_observer {
    int m_sse, m_rounding;
    /*override*/
    void on_scheduler_entry( bool is_worker ) {
        if(is_worker) {
            SetSseMode( m_sse );
            SetRoundingMode( m_rounding );
        }
    }
public:
    LocalObserver(int fpu, int sse) : tbb::task_scheduler_observer(true), m_sse(sse), m_rounding(fpu) {
        observe(true);
    }
};
#endif

class LauncherBody {
public:
    void operator() ( int id ) const {
        Harness::tid_t tid = Harness::CurrentTid();
        // TBB scheduler instance in a master thread captures the FPU control state
        // at the moment of its initialization and passes it to the workers toiling
        // on its behalf.
        for( int k = 0; k < NumSseModes; ++k ) {
            int sse_mode = SseModes[(k + id) % NumSseModes];
            SetSseMode( sse_mode );
            for( int i = 0; i < NumRoundingModes; ++i ) {
                int mode = RoundingModes[(i + id) % NumRoundingModes];
                SetRoundingMode( mode );
                // New mode must be set before TBB scheduler is initialized
                tbb::task_scheduler_init init;
                tbb::parallel_for( 0, N, 1, RoundingModeCheckBody(tid, mode, mode, sse_mode, sse_mode) );
                ASSERT( GetRoundingMode() == mode, NULL );
            }
        }
        // Since the following loop uses auto-initialization, the scheduler instance
        // implicitly created by the first parallel_for invocation will persist
        // until the thread ends, and thus workers will use the mode set by the
        // first iteration.
        int captured_mode = RoundingModes[id % NumRoundingModes];
        int captured_sse_mode = SseModes[id % NumSseModes];
        for( int k = 0; k < NumSseModes; ++k ) {
            int sse_mode = SseModes[(k + id) % NumSseModes];
            SetSseMode( sse_mode );
            for( int i = 0; i < NumRoundingModes; ++i ) {
                int mode = RoundingModes[(i + id) % NumRoundingModes];
                SetRoundingMode( mode );
                tbb::parallel_for( 0, N, 1, RoundingModeCheckBody(tid, mode, captured_mode, sse_mode, captured_sse_mode) );
                ASSERT( GetRoundingMode() == mode, NULL );
            }
        }
#if __TBB_SCHEDULER_OBSERVER
        // but using observers, it is possible to redefine the mode again
        for( int k = 0; k < NumSseModes; ++k ) {
            int sse_mode = SseModes[(k + id) % NumSseModes];
            SetSseMode( sse_mode );
            for( int i = 0; i < NumRoundingModes; ++i ) {
                int mode = RoundingModes[(i + id) % NumRoundingModes];
                SetRoundingMode( mode );
                // New mode must be set before TBB scheduler is initialized
                LocalObserver restorer(mode, sse_mode);
                tbb::parallel_for( 0, N, 1, RoundingModeCheckBody(tid, mode, mode, sse_mode, sse_mode) );
                ASSERT( GetRoundingMode() == mode, NULL );
            }
        }
#endif
    }
};

void TestFpuEnvPropagation () {
    NativeParallelFor ( tbb::task_scheduler_init::default_num_threads() * NumRoundingModes, LauncherBody() );
}

void TestCpuCtlEnvApi () {
    for( int k = 0; k < NumSseModes; ++k ) {
        SetSseMode( SseModes[k] );
        for( int i = 0; i < NumRoundingModes; ++i ) {
            SetRoundingMode( RoundingModes[i] );
            ASSERT( GetRoundingMode() == RoundingModes[i], NULL );
            ASSERT( GetSseMode() == SseModes[k], NULL );
        }
    }
}

int TestMain () {
#if defined(__TBB_CPU_CTL_ENV_PRESENT) && !__TBB_CPU_CTL_ENV_PRESENT
    return Harness::Skipped;
#else
    TestCpuCtlEnvApi();
    TestFpuEnvPropagation();
    return Harness::Done;
#endif
}
