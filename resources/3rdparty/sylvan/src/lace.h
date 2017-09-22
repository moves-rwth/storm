/* 
 * Copyright 2013-2016 Formal Methods and Tools, University of Twente
 * Copyright 2016-2017 Tom van Dijk, Johannes Kepler University Linz
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h> /* for pthread_t */

#ifndef __LACE_H__
#define __LACE_H__

#ifdef __has_include
#  if __has_include("lace_config.h")
#    include <lace_config.h>
#  else
#    define LACE_PIE_TIMES     0
#    define LACE_COUNT_TASKS   0
#    define LACE_COUNT_STEALS  0
#    define LACE_COUNT_SPLITS  0
#    define LACE_USE_HWLOC     0
#  endif
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Using Lace.
 *
 * Optionally set the verbosity level with lace_set_verbosity.
 * Then call lace_init to initialize the system.
 * - lace_init(n_workers, deque_size);
 *   set both parameters to 0 for reasonable defaults, using all available cores.
 *
 * You can create Worker threads yourself or let Lace create threads with lace_startup.
 *
 * When creating threads yourself, call the following functions:
 *   - lace_init_worker to allocate and initialize the worker data structures
 *     this method returns when all workers have called lace_init_worker
 *   - lace_pin_worker (optional) to pin the thread and memory to a core
 * The main worker can now start its root task. All other workers:
 *   - lace_run_worker to perform work-stealing until the main worker calls lace_exit
 *
 * When letting Lace create threads with lace_startup
 * - Call lace_startup with a callback to create N threads.
 *   Returns after the callback has returned and all created threads are destroyed
 * - Call lace_startup without a callback to create N-1 threads.
 *   Returns control to the caller. When lace_exit is called, all created threads are terminated.
 */

/**
 * Type definitions used in the functions below.
 * - WorkerP contains the (private) Worker data
 * - Task contains a single Task
 */
typedef struct _WorkerP WorkerP;
typedef struct _Task Task;

/**
 * The macro LACE_TYPEDEF_CB(typedefname, taskname, parametertypes) defines
 * a Task for use as a callback function.
 */
#define LACE_TYPEDEF_CB(t, f, ...) typedef t (*f)(WorkerP *, Task *, ##__VA_ARGS__);

/**
 * The lace_startup_cb type for a void Task with one void* parameter.
 */
LACE_TYPEDEF_CB(void, lace_startup_cb, void*);

/**
 * Set verbosity level (0 = no startup messages, 1 = startup messages)
 * Default level: 0
 */
void lace_set_verbosity(int level);

/**
 * Initialize Lace for <n_workers> workers with a deque size of <dqsize> per worker.
 * If <n_workers> is set to 0, automatically detects available cores.
 * If <dqsize> is est to 0, uses a reasonable default value.
 */
void lace_init(unsigned int n_workers, size_t dqsize);

/**
 * Let Lace create worker threads.
 * If <stacksize> is set to 0, uses a reaonable default value.
 * If cb, arg are set to 0, then the current thread is initialized as the main Worker (Worker 0).
 *
 * If cb,arg are set, then the current thread is suspended. A new thread is made for Worker 0 and
 * the task cb with paremeter arg is called; when cb returns, Lace is exited automatically.
 */
void lace_startup(size_t stacksize, lace_startup_cb, void* arg);

/**
 * Initialize worker <worker>, allocating memory.
 * If <worker> is 0, then the current thread is the main worker.
 */
void lace_init_worker(unsigned int worker);

/**
 * Use hwloc to pin the current thread to a CPU and its allocated memory in the closest domain.
 * Call this *after* lace_init_worker and *before* lace_run_worker.
 */
void lace_pin_worker(void);

/**
 * Perform work-stealing until lace_exit is called.
 */
void lace_run_worker(void);

/**
 * Steal a random task.
 */
#define lace_steal_random() CALL(lace_steal_random)
void lace_steal_random_CALL(WorkerP*, Task*);

/**
 * Enter the Lace barrier. (all active workers must enter it before we can continue)
 */
void lace_barrier();

/**
 * Suspend all workers except the current worker.
 * May only be used when all other workers are idle.
 */
void lace_suspend();

/**
 * Resume all workers.
 */
void lace_resume();

/**
 * When all other workers are suspended, some workers can be disabled using the following functions.
 * With set_workers, all workers 0..(N-1) are enabled and N..max are disabled.
 * You can never disable the current worker or reduce the number of workers below 1.
 * You cannot add workers.
 */
void lace_set_workers(unsigned int workercount);

/**
 * Disable a suspended worker.
 */
void lace_disable_worker(unsigned int worker);

/**
 * Enable a suspended worker.
 */
void lace_enable_worker(unsigned int worker);

/**
 * Retrieve the number of enabled/active workers.
 */
unsigned int lace_enabled_workers();

/**
 * Retrieve the number of Lace workers
 */
unsigned int lace_workers();

/**
 * Retrieve the default program stack size
 */
size_t lace_default_stacksize();

/**
 * Retrieve the current worker data.
 */
WorkerP *lace_get_worker();

/**
 * Retrieve the current head of the deque
 */
Task *lace_get_head(WorkerP *);

/**
 * Exit Lace.
 * This function is automatically called when lace_startup is called with a callback.
 * This function must be called to exit Lace when lace_startup is called without a callback.
 */
void lace_exit();

/**
 * Create a pointer to a Tasks main function.
 */
#define TASK(f)           ( f##_CALL )

/**
 * Call a Tasks implementation (adds Lace variables to call)
 */
#define WRAP(f, ...)      ( f((WorkerP *)__lace_worker, (Task *)__lace_dq_head, ##__VA_ARGS__) )

/**
 * Sync a task.
 */
#define SYNC(f)           ( __lace_dq_head--, WRAP(f##_SYNC) )

/**
 * Sync a task, but if the task is not stolen, then do not execute it.
 */
#define DROP()            ( __lace_dq_head--, WRAP(lace_drop) )

/**
 * Spawn a task.
 */
#define SPAWN(f, ...)     ( WRAP(f##_SPAWN, ##__VA_ARGS__), __lace_dq_head++ )

/**
 * Directly execute a task.
 */
#define CALL(f, ...)      ( WRAP(f##_CALL, ##__VA_ARGS__) )

/**
 * Signal all workers to interrupt their current tasks and instead perform (a personal copy of) the given task.
 */
#define TOGETHER(f, ...)  ( WRAP(f##_TOGETHER, ##__VA_ARGS__) )

/**
 * Signal all workers to interrupt their current tasks and help the current thread with the given task.
 */
#define NEWFRAME(f, ...)  ( WRAP(f##_NEWFRAME, ##__VA_ARGS__) )

/**
 * (Try to) steal a task from a random worker.
 */
#define STEAL_RANDOM()    ( CALL(lace_steal_random) )

/**
 * Get the current worker id.
 */
#define LACE_WORKER_ID    ( __lace_worker->worker )

/**
 * Get the core where the current worker is pinned.
 */
#define LACE_WORKER_PU    ( __lace_worker->pu )

/**
 * Initialize local variables __lace_worker and __lace_dq_head which are required for most Lace functionality.
 */
#define LACE_ME WorkerP * __attribute__((unused)) __lace_worker = lace_get_worker(); Task * __attribute__((unused)) __lace_dq_head = lace_get_head(__lace_worker);

/**
 * Check if current tasks must be interrupted, and if so, interrupt.
 */
void lace_yield(WorkerP *__lace_worker, Task *__lace_dq_head);
#define YIELD_NEWFRAME() { if (unlikely((*(Task* volatile *)&lace_newframe.t) != NULL)) lace_yield(__lace_worker, __lace_dq_head); }

/**
 * True if the given task is stolen, False otherwise.
 */
#define TASK_IS_STOLEN(t) ((size_t)t->thief > 1)

/**
 * True if the given task is completed, False otherwise.
 */
#define TASK_IS_COMPLETED(t) ((size_t)t->thief == 2)

/**
 * Retrieves a pointer to the result of the given task.
 */
#define TASK_RESULT(t) (&t->d[0])

/**
 * Compute a random number, thread-local (so scalable)
 */
#define LACE_TRNG (__lace_worker->rng = 2862933555777941757ULL * __lace_worker->rng + 3037000493ULL)

/* Some flags that influence Lace behavior */

#ifndef LACE_DEBUG_PROGRAMSTACK /* Write to stderr when 95% program stack reached */
#define LACE_DEBUG_PROGRAMSTACK 0
#endif

#ifndef LACE_LEAP_RANDOM /* Use random leaping when leapfrogging fails */
#define LACE_LEAP_RANDOM 1
#endif

#ifndef LACE_COUNT_EVENTS
#define LACE_COUNT_EVENTS (LACE_PIE_TIMES || LACE_COUNT_TASKS || LACE_COUNT_STEALS || LACE_COUNT_SPLITS)
#endif

/**
 * Now follows the implementation of Lace
 */

/* Typical cacheline size of system architectures */
#ifndef LINE_SIZE
#define LINE_SIZE 64
#endif

/* The size of a pointer, 8 bytes on a 64-bit architecture */
#define P_SZ (sizeof(void *))

#define PAD(x,b) ( ( (b) - ((x)%(b)) ) & ((b)-1) ) /* b must be power of 2 */
#define ROUND(x,b) ( (x) + PAD( (x), (b) ) )

/* The size is in bytes. Note that this is without the extra overhead from Lace.
   The value must be greater than or equal to the maximum size of your tasks.
   The task size is the maximum of the size of the result or of the sum of the parameter sizes. */
#ifndef LACE_TASKSIZE
#define LACE_TASKSIZE (6)*P_SZ
#endif

/* Some fences */
#ifndef compiler_barrier
#define compiler_barrier() { asm volatile("" ::: "memory"); }
#endif

#ifndef mfence
#define mfence() { asm volatile("mfence" ::: "memory"); }
#endif

/* Compiler specific branch prediction optimization */
#ifndef likely
#define likely(x)       __builtin_expect((x),1)
#endif

#ifndef unlikely
#define unlikely(x)     __builtin_expect((x),0)
#endif

#if LACE_PIE_TIMES
/* High resolution timer */
static inline uint64_t gethrtime()
{
    uint32_t hi, lo;
    asm volatile ("rdtsc" : "=a"(lo), "=d"(hi) :: "memory");
    return (uint64_t)hi<<32 | lo;
}
#endif

#if LACE_COUNT_EVENTS
void lace_count_reset();
void lace_count_report_file(FILE *file);
#endif

#if LACE_COUNT_TASKS
#define PR_COUNTTASK(s) PR_INC(s,CTR_tasks)
#else
#define PR_COUNTTASK(s) /* Empty */
#endif

#if LACE_COUNT_STEALS
#define PR_COUNTSTEALS(s,i) PR_INC(s,i)
#else
#define PR_COUNTSTEALS(s,i) /* Empty */
#endif

#if LACE_COUNT_SPLITS
#define PR_COUNTSPLITS(s,i) PR_INC(s,i)
#else
#define PR_COUNTSPLITS(s,i) /* Empty */
#endif

#if LACE_COUNT_EVENTS
#define PR_ADD(s,i,k) ( ((s)->ctr[i])+=k )
#else
#define PR_ADD(s,i,k) /* Empty */
#endif
#define PR_INC(s,i) PR_ADD(s,i,1)

typedef enum {
#ifdef LACE_COUNT_TASKS
    CTR_tasks,       /* Number of tasks spawned */
#endif
#ifdef LACE_COUNT_STEALS
    CTR_steal_tries, /* Number of steal attempts */
    CTR_leap_tries,  /* Number of leap attempts */
    CTR_steals,      /* Number of succesful steals */
    CTR_leaps,       /* Number of succesful leaps */
    CTR_steal_busy,  /* Number of steal busies */
    CTR_leap_busy,   /* Number of leap busies */
#endif
#ifdef LACE_COUNT_SPLITS
    CTR_split_grow,  /* Number of split right */
    CTR_split_shrink,/* Number of split left */
    CTR_split_req,   /* Number of split requests */
#endif
    CTR_fast_sync,   /* Number of fast syncs */
    CTR_slow_sync,   /* Number of slow syncs */
#ifdef LACE_PIE_TIMES
    CTR_init,        /* Timer for initialization */
    CTR_close,       /* Timer for shutdown */
    CTR_wapp,        /* Timer for application code (steal) */
    CTR_lapp,        /* Timer for application code (leap) */
    CTR_wsteal,      /* Timer for steal code (steal) */
    CTR_lsteal,      /* Timer for steal code (leap) */
    CTR_wstealsucc,  /* Timer for succesful steal code (steal) */
    CTR_lstealsucc,  /* Timer for succesful steal code (leap) */
    CTR_wsignal,     /* Timer for signal after work (steal) */
    CTR_lsignal,     /* Timer for signal after work (leap) */
#endif
    CTR_MAX
} CTR_index;

#define THIEF_EMPTY     ((struct _Worker*)0x0)
#define THIEF_TASK      ((struct _Worker*)0x1)
#define THIEF_COMPLETED ((struct _Worker*)0x2)

#define TASK_COMMON_FIELDS(type)                               \
    void (*f)(struct _WorkerP *, struct _Task *, struct type *);  \
    struct _Worker * volatile thief;

struct __lace_common_fields_only { TASK_COMMON_FIELDS(_Task) };
#define LACE_COMMON_FIELD_SIZE sizeof(struct __lace_common_fields_only)

typedef struct _Task {
    TASK_COMMON_FIELDS(_Task);
    char p1[PAD(LACE_COMMON_FIELD_SIZE, P_SZ)];
    char d[LACE_TASKSIZE];
    char p2[PAD(ROUND(LACE_COMMON_FIELD_SIZE, P_SZ) + LACE_TASKSIZE, LINE_SIZE)];
} Task;

typedef union __attribute__((packed)) {
    struct {
        uint32_t tail;
        uint32_t split;
    } ts;
    uint64_t v;
} TailSplit;

typedef struct _Worker {
    Task *dq;
    TailSplit ts;
    uint8_t allstolen;

    char pad1[PAD(P_SZ+sizeof(TailSplit)+1, LINE_SIZE)];

    uint8_t movesplit;
} Worker;

typedef struct _WorkerP {
    Task *dq;                   // same as dq
    Task *split;                // same as dq+ts.ts.split
    Task *end;                  // dq+dq_size
    Worker *_public;            // pointer to public Worker struct
    size_t stack_trigger;       // for stack overflow detection
    uint64_t rng;               // my random seed (for lace_trng)
    uint32_t seed;              // my random seed (for lace_steal_random)
    uint16_t worker;            // what is my worker id?
    uint8_t allstolen;          // my allstolen
    volatile int8_t enabled;    // if this worker is enabled

#if LACE_COUNT_EVENTS
    uint64_t ctr[CTR_MAX];      // counters
    volatile uint64_t time;
    volatile int level;
#endif

    int16_t pu;                 // my pu (for HWLOC)
} WorkerP;

#define LACE_STOLEN   ((Worker*)0)
#define LACE_BUSY     ((Worker*)1)
#define LACE_NOWORK   ((Worker*)2)

#if LACE_DEBUG_PROGRAMSTACK
static inline void CHECKSTACK(WorkerP *w)
{
    if (w->stack_trigger != 0) {
        register size_t rsp;
        asm volatile("movq %%rsp, %0" : "+r"(rsp) : : "cc");
        if (rsp < w->stack_trigger) {
            fputs("Warning: program stack 95% used!\n", stderr);
            w->stack_trigger = 0;
        }
    }
}
#else
#define CHECKSTACK(w) {}
#endif

void lace_abort_stack_overflow(void) __attribute__((noreturn));

typedef struct
{
    Task *t;
    uint8_t all;
    char pad[64-sizeof(Task *)-sizeof(uint8_t)];
} lace_newframe_t;

extern lace_newframe_t lace_newframe;

/**
 * Internal function to start participating on a task in a new frame
 * Usually, <root> is set to NULL and the task is copied from lace_newframe.t
 * It is possible to override the start task by setting <root>.
 */
void lace_do_together(WorkerP *__lace_worker, Task *__lace_dq_head, Task *task);
void lace_do_newframe(WorkerP *__lace_worker, Task *__lace_dq_head, Task *task);

/**
 * Make all tasks of the current worker shared.
 */
#define LACE_MAKE_ALL_SHARED() lace_make_all_shared(__lace_worker, __lace_dq_head)
static inline void __attribute__((unused))
lace_make_all_shared( WorkerP *w, Task *__lace_dq_head)
{
    if (w->split != __lace_dq_head) {
        w->split = __lace_dq_head;
        w->_public->ts.ts.split = __lace_dq_head - w->dq;
    }
}

#if LACE_PIE_TIMES
static void lace_time_event( WorkerP *w, int event )
{
    uint64_t now = gethrtime(),
             prev = w->time;

    switch( event ) {

        // Enter application code
        case 1 :
            if(  w->level /* level */ == 0 ) {
                PR_ADD( w, CTR_init, now - prev );
                w->level = 1;
            } else if( w->level /* level */ == 1 ) {
                PR_ADD( w, CTR_wsteal, now - prev );
                PR_ADD( w, CTR_wstealsucc, now - prev );
            } else {
                PR_ADD( w, CTR_lsteal, now - prev );
                PR_ADD( w, CTR_lstealsucc, now - prev );
            }
            break;

            // Exit application code
        case 2 :
            if( w->level /* level */ == 1 ) {
                PR_ADD( w, CTR_wapp, now - prev );
            } else {
                PR_ADD( w, CTR_lapp, now - prev );
            }
            break;

            // Enter sync on stolen
        case 3 :
            if( w->level /* level */ == 1 ) {
                PR_ADD( w, CTR_wapp, now - prev );
            } else {
                PR_ADD( w, CTR_lapp, now - prev );
            }
            w->level++;
            break;

            // Exit sync on stolen
        case 4 :
            if( w->level /* level */ == 1 ) {
                fprintf( stderr, "This should not happen, level = %d\n", w->level );
            } else {
                PR_ADD( w, CTR_lsteal, now - prev );
            }
            w->level--;
            break;

            // Return from failed steal
        case 7 :
            if( w->level /* level */ == 0 ) {
                PR_ADD( w, CTR_init, now - prev );
            } else if( w->level /* level */ == 1 ) {
                PR_ADD( w, CTR_wsteal, now - prev );
            } else {
                PR_ADD( w, CTR_lsteal, now - prev );
            }
            break;

            // Signalling time
        case 8 :
            if( w->level /* level */ == 1 ) {
                PR_ADD( w, CTR_wsignal, now - prev );
                PR_ADD( w, CTR_wsteal, now - prev );
            } else {
                PR_ADD( w, CTR_lsignal, now - prev );
                PR_ADD( w, CTR_lsteal, now - prev );
            }
            break;

            // Done
        case 9 :
            if( w->level /* level */ == 0 ) {
                PR_ADD( w, CTR_init, now - prev );
            } else {
                PR_ADD( w, CTR_close, now - prev );
            }
            break;

        default: return;
    }

    w->time = now;
}
#else
#define lace_time_event( w, e ) /* Empty */
#endif

static Worker* __attribute__((noinline))
lace_steal(WorkerP *self, Task *__dq_head, Worker *victim)
{
    if (!victim->allstolen) {
        /* Must be a volatile. In GCC 4.8, if it is not declared volatile, the
           compiler will optimize extra memory accesses to victim->ts instead
           of comparing the local values ts.ts.tail and ts.ts.split, causing
           thieves to steal non existent tasks! */
        register TailSplit ts;
        ts.v = *(volatile uint64_t *)&victim->ts.v;
        if (ts.ts.tail < ts.ts.split) {
            register TailSplit ts_new;
            ts_new.v = ts.v;
            ts_new.ts.tail++;
            if (__sync_bool_compare_and_swap(&victim->ts.v, ts.v, ts_new.v)) {
                // Stolen
                Task *t = &victim->dq[ts.ts.tail];
                t->thief = self->_public;
                lace_time_event(self, 1);
                t->f(self, __dq_head, t);
                lace_time_event(self, 2);
                t->thief = THIEF_COMPLETED;
                lace_time_event(self, 8);
                return LACE_STOLEN;
            }

            lace_time_event(self, 7);
            return LACE_BUSY;
        }

        if (victim->movesplit == 0) {
            victim->movesplit = 1;
            PR_COUNTSPLITS(self, CTR_split_req);
        }
    }

    lace_time_event(self, 7);
    return LACE_NOWORK;
}

static int
lace_shrink_shared(WorkerP *w)
{
    Worker *wt = w->_public;
    TailSplit ts;
    ts.v = wt->ts.v; /* Force in 1 memory read */
    uint32_t tail = ts.ts.tail;
    uint32_t split = ts.ts.split;

    if (tail != split) {
        uint32_t newsplit = (tail + split)/2;
        wt->ts.ts.split = newsplit;
        mfence();
        tail = *(volatile uint32_t *)&(wt->ts.ts.tail);
        if (tail != split) {
            if (unlikely(tail > newsplit)) {
                newsplit = (tail + split) / 2;
                wt->ts.ts.split = newsplit;
            }
            w->split = w->dq + newsplit;
            PR_COUNTSPLITS(w, CTR_split_shrink);
            return 0;
        }
    }

    wt->allstolen = 1;
    w->allstolen = 1;
    return 1;
}

static inline void
lace_leapfrog(WorkerP *__lace_worker, Task *__lace_dq_head)
{
    lace_time_event(__lace_worker, 3);
    Task *t = __lace_dq_head;
    Worker *thief = t->thief;
    if (thief != THIEF_COMPLETED) {
        while ((size_t)thief <= 1) thief = t->thief;

        /* PRE-LEAP: increase head again */
        __lace_dq_head += 1;

        /* Now leapfrog */
        int attempts = 32;
        while (thief != THIEF_COMPLETED) {
            PR_COUNTSTEALS(__lace_worker, CTR_leap_tries);
            Worker *res = lace_steal(__lace_worker, __lace_dq_head, thief);
            if (res == LACE_NOWORK) {
                YIELD_NEWFRAME();
                if ((LACE_LEAP_RANDOM) && (--attempts == 0)) { lace_steal_random(); attempts = 32; }
            } else if (res == LACE_STOLEN) {
                PR_COUNTSTEALS(__lace_worker, CTR_leaps);
            } else if (res == LACE_BUSY) {
                PR_COUNTSTEALS(__lace_worker, CTR_leap_busy);
            }
            compiler_barrier();
            thief = t->thief;
        }

        /* POST-LEAP: really pop the finished task */
        /*            no need to decrease __lace_dq_head, since it is a local variable */
        compiler_barrier();
        if (__lace_worker->allstolen == 0) {
            /* Assume: tail = split = head (pre-pop) */
            /* Now we do a real pop ergo either decrease tail,split,head or declare allstolen */
            Worker *wt = __lace_worker->_public;
            wt->allstolen = 1;
            __lace_worker->allstolen = 1;
        }
    }

    compiler_barrier();
    t->thief = THIEF_EMPTY;
    lace_time_event(__lace_worker, 4);
}

static __attribute__((noinline))
void lace_drop_slow(WorkerP *w, Task *__dq_head)
{
    if ((w->allstolen) || (w->split > __dq_head && lace_shrink_shared(w))) lace_leapfrog(w, __dq_head);
}

static inline __attribute__((unused))
void lace_drop(WorkerP *w, Task *__dq_head)
{
    if (likely(0 == w->_public->movesplit)) {
        if (likely(w->split <= __dq_head)) {
            return;
        }
    }
    lace_drop_slow(w, __dq_head);
}



// Task macros for tasks of arity 0

#define TASK_DECL_0(RTYPE, NAME)                                                      \
                                                                                      \
typedef struct _TD_##NAME {                                                           \
  TASK_COMMON_FIELDS(_TD_##NAME)                                                      \
  union {  RTYPE res; } d;                                                            \
} TD_##NAME;                                                                          \
                                                                                      \
/* If this line generates an error, please manually set the define LACE_TASKSIZE to a higher value */\
typedef char assertion_failed_task_descriptor_out_of_bounds_##NAME[(sizeof(TD_##NAME)<=sizeof(Task)) ? 0 : -1];\
                                                                                      \
void NAME##_WRAP(WorkerP *, Task *, TD_##NAME *);                                     \
RTYPE NAME##_CALL(WorkerP *, Task * );                                                \
static inline RTYPE NAME##_SYNC(WorkerP *, Task *);                                   \
static RTYPE NAME##_SYNC_SLOW(WorkerP *, Task *);                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SPAWN(WorkerP *w, Task *__dq_head )                                       \
{                                                                                     \
    PR_COUNTTASK(w);                                                                  \
                                                                                      \
    TD_##NAME *t;                                                                     \
    TailSplit ts;                                                                     \
    uint32_t head, split, newsplit;                                                   \
                                                                                      \
    if (__dq_head == w->end) lace_abort_stack_overflow();                             \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (unlikely(w->allstolen)) {                                                     \
        if (wt->movesplit) wt->movesplit = 0;                                         \
        head = __dq_head - w->dq;                                                     \
        ts = (TailSplit){{head,head+1}};                                              \
        wt->ts.v = ts.v;                                                              \
        compiler_barrier();                                                           \
        wt->allstolen = 0;                                                            \
        w->split = __dq_head+1;                                                       \
        w->allstolen = 0;                                                             \
    } else if (unlikely(wt->movesplit)) {                                             \
        head = __dq_head - w->dq;                                                     \
        split = w->split - w->dq;                                                     \
        newsplit = (split + head + 2)/2;                                              \
        wt->ts.ts.split = newsplit;                                                   \
        w->split = w->dq + newsplit;                                                  \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
RTYPE NAME##_NEWFRAME(WorkerP *w, Task *__dq_head )                                   \
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
                                                                                      \
                                                                                      \
    lace_do_newframe(w, __dq_head, &_t);                                              \
    return ((TD_##NAME *)t)->d.res;                                                   \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_TOGETHER(WorkerP *w, Task *__dq_head )                                    \
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
                                                                                      \
                                                                                      \
    lace_do_together(w, __dq_head, &_t);                                              \
}                                                                                     \
                                                                                      \
static __attribute__((noinline))                                                      \
RTYPE NAME##_SYNC_SLOW(WorkerP *w, Task *__dq_head)                                   \
{                                                                                     \
    TD_##NAME *t;                                                                     \
                                                                                      \
    if ((w->allstolen) || (w->split > __dq_head && lace_shrink_shared(w))) {          \
        lace_leapfrog(w, __dq_head);                                                  \
        t = (TD_##NAME *)__dq_head;                                                   \
        return ((TD_##NAME *)t)->d.res;                                               \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (wt->movesplit) {                                                              \
        Task *t = w->split;                                                           \
        size_t diff = __dq_head - t;                                                  \
        diff = (diff + 1) / 2;                                                        \
        w->split = t + diff;                                                          \
        wt->ts.ts.split += diff;                                                      \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->thief = THIEF_EMPTY;                                                           \
    return NAME##_CALL(w, __dq_head );                                                \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
RTYPE NAME##_SYNC(WorkerP *w, Task *__dq_head)                                        \
{                                                                                     \
    /* assert (__dq_head > 0); */  /* Commented out because we assume contract */     \
                                                                                      \
    if (likely(0 == w->_public->movesplit)) {                                         \
        if (likely(w->split <= __dq_head)) {                                          \
            TD_##NAME *t = (TD_##NAME *)__dq_head;                                    \
            t->thief = THIEF_EMPTY;                                                   \
            return NAME##_CALL(w, __dq_head );                                        \
        }                                                                             \
    }                                                                                 \
                                                                                      \
    return NAME##_SYNC_SLOW(w, __dq_head);                                            \
}                                                                                     \
                                                                                      \
                                                                                      \

#define TASK_IMPL_0(RTYPE, NAME)                                                      \
void NAME##_WRAP(WorkerP *w, Task *__dq_head, TD_##NAME *t __attribute__((unused)))   \
{                                                                                     \
    t->d.res = NAME##_CALL(w, __dq_head );                                            \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
RTYPE NAME##_WORK(WorkerP *__lace_worker, Task *__lace_dq_head );                     \
                                                                                      \
/* NAME##_WORK is inlined in NAME##_CALL and the parameter __lace_in_task will disappear */\
RTYPE NAME##_CALL(WorkerP *w, Task *__dq_head )                                       \
{                                                                                     \
    CHECKSTACK(w);                                                                    \
    return NAME##_WORK(w, __dq_head );                                                \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
RTYPE NAME##_WORK(WorkerP *__lace_worker __attribute__((unused)), Task *__lace_dq_head __attribute__((unused)) )\

#define TASK_0(RTYPE, NAME) TASK_DECL_0(RTYPE, NAME) TASK_IMPL_0(RTYPE, NAME)

#define VOID_TASK_DECL_0(NAME)                                                        \
                                                                                      \
typedef struct _TD_##NAME {                                                           \
  TASK_COMMON_FIELDS(_TD_##NAME)                                                      \
                                                                                      \
} TD_##NAME;                                                                          \
                                                                                      \
/* If this line generates an error, please manually set the define LACE_TASKSIZE to a higher value */\
typedef char assertion_failed_task_descriptor_out_of_bounds_##NAME[(sizeof(TD_##NAME)<=sizeof(Task)) ? 0 : -1];\
                                                                                      \
void NAME##_WRAP(WorkerP *, Task *, TD_##NAME *);                                     \
void NAME##_CALL(WorkerP *, Task * );                                                 \
static inline void NAME##_SYNC(WorkerP *, Task *);                                    \
static void NAME##_SYNC_SLOW(WorkerP *, Task *);                                      \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SPAWN(WorkerP *w, Task *__dq_head )                                       \
{                                                                                     \
    PR_COUNTTASK(w);                                                                  \
                                                                                      \
    TD_##NAME *t;                                                                     \
    TailSplit ts;                                                                     \
    uint32_t head, split, newsplit;                                                   \
                                                                                      \
    if (__dq_head == w->end) lace_abort_stack_overflow();                             \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (unlikely(w->allstolen)) {                                                     \
        if (wt->movesplit) wt->movesplit = 0;                                         \
        head = __dq_head - w->dq;                                                     \
        ts = (TailSplit){{head,head+1}};                                              \
        wt->ts.v = ts.v;                                                              \
        compiler_barrier();                                                           \
        wt->allstolen = 0;                                                            \
        w->split = __dq_head+1;                                                       \
        w->allstolen = 0;                                                             \
    } else if (unlikely(wt->movesplit)) {                                             \
        head = __dq_head - w->dq;                                                     \
        split = w->split - w->dq;                                                     \
        newsplit = (split + head + 2)/2;                                              \
        wt->ts.ts.split = newsplit;                                                   \
        w->split = w->dq + newsplit;                                                  \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_NEWFRAME(WorkerP *w, Task *__dq_head )                                    \
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
                                                                                      \
                                                                                      \
    lace_do_newframe(w, __dq_head, &_t);                                              \
    return ;                                                                          \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_TOGETHER(WorkerP *w, Task *__dq_head )                                    \
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
                                                                                      \
                                                                                      \
    lace_do_together(w, __dq_head, &_t);                                              \
}                                                                                     \
                                                                                      \
static __attribute__((noinline))                                                      \
void NAME##_SYNC_SLOW(WorkerP *w, Task *__dq_head)                                    \
{                                                                                     \
    TD_##NAME *t;                                                                     \
                                                                                      \
    if ((w->allstolen) || (w->split > __dq_head && lace_shrink_shared(w))) {          \
        lace_leapfrog(w, __dq_head);                                                  \
        t = (TD_##NAME *)__dq_head;                                                   \
        return ;                                                                      \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (wt->movesplit) {                                                              \
        Task *t = w->split;                                                           \
        size_t diff = __dq_head - t;                                                  \
        diff = (diff + 1) / 2;                                                        \
        w->split = t + diff;                                                          \
        wt->ts.ts.split += diff;                                                      \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->thief = THIEF_EMPTY;                                                           \
    return NAME##_CALL(w, __dq_head );                                                \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SYNC(WorkerP *w, Task *__dq_head)                                         \
{                                                                                     \
    /* assert (__dq_head > 0); */  /* Commented out because we assume contract */     \
                                                                                      \
    if (likely(0 == w->_public->movesplit)) {                                         \
        if (likely(w->split <= __dq_head)) {                                          \
            TD_##NAME *t = (TD_##NAME *)__dq_head;                                    \
            t->thief = THIEF_EMPTY;                                                   \
            return NAME##_CALL(w, __dq_head );                                        \
        }                                                                             \
    }                                                                                 \
                                                                                      \
    return NAME##_SYNC_SLOW(w, __dq_head);                                            \
}                                                                                     \
                                                                                      \
                                                                                      \

#define VOID_TASK_IMPL_0(NAME)                                                        \
void NAME##_WRAP(WorkerP *w, Task *__dq_head, TD_##NAME *t __attribute__((unused)))   \
{                                                                                     \
     NAME##_CALL(w, __dq_head );                                                      \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
void NAME##_WORK(WorkerP *__lace_worker, Task *__lace_dq_head );                      \
                                                                                      \
/* NAME##_WORK is inlined in NAME##_CALL and the parameter __lace_in_task will disappear */\
void NAME##_CALL(WorkerP *w, Task *__dq_head )                                        \
{                                                                                     \
    CHECKSTACK(w);                                                                    \
    return NAME##_WORK(w, __dq_head );                                                \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
void NAME##_WORK(WorkerP *__lace_worker __attribute__((unused)), Task *__lace_dq_head __attribute__((unused)) )\

#define VOID_TASK_0(NAME) VOID_TASK_DECL_0(NAME) VOID_TASK_IMPL_0(NAME)


// Task macros for tasks of arity 1

#define TASK_DECL_1(RTYPE, NAME, ATYPE_1)                                             \
                                                                                      \
typedef struct _TD_##NAME {                                                           \
  TASK_COMMON_FIELDS(_TD_##NAME)                                                      \
  union { struct {  ATYPE_1 arg_1; } args; RTYPE res; } d;                            \
} TD_##NAME;                                                                          \
                                                                                      \
/* If this line generates an error, please manually set the define LACE_TASKSIZE to a higher value */\
typedef char assertion_failed_task_descriptor_out_of_bounds_##NAME[(sizeof(TD_##NAME)<=sizeof(Task)) ? 0 : -1];\
                                                                                      \
void NAME##_WRAP(WorkerP *, Task *, TD_##NAME *);                                     \
RTYPE NAME##_CALL(WorkerP *, Task * , ATYPE_1 arg_1);                                 \
static inline RTYPE NAME##_SYNC(WorkerP *, Task *);                                   \
static RTYPE NAME##_SYNC_SLOW(WorkerP *, Task *);                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SPAWN(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1)                        \
{                                                                                     \
    PR_COUNTTASK(w);                                                                  \
                                                                                      \
    TD_##NAME *t;                                                                     \
    TailSplit ts;                                                                     \
    uint32_t head, split, newsplit;                                                   \
                                                                                      \
    if (__dq_head == w->end) lace_abort_stack_overflow();                             \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1;                                                         \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (unlikely(w->allstolen)) {                                                     \
        if (wt->movesplit) wt->movesplit = 0;                                         \
        head = __dq_head - w->dq;                                                     \
        ts = (TailSplit){{head,head+1}};                                              \
        wt->ts.v = ts.v;                                                              \
        compiler_barrier();                                                           \
        wt->allstolen = 0;                                                            \
        w->split = __dq_head+1;                                                       \
        w->allstolen = 0;                                                             \
    } else if (unlikely(wt->movesplit)) {                                             \
        head = __dq_head - w->dq;                                                     \
        split = w->split - w->dq;                                                     \
        newsplit = (split + head + 2)/2;                                              \
        wt->ts.ts.split = newsplit;                                                   \
        w->split = w->dq + newsplit;                                                  \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
RTYPE NAME##_NEWFRAME(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1)                    \
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1;                                                         \
                                                                                      \
    lace_do_newframe(w, __dq_head, &_t);                                              \
    return ((TD_##NAME *)t)->d.res;                                                   \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_TOGETHER(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1)                     \
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1;                                                         \
                                                                                      \
    lace_do_together(w, __dq_head, &_t);                                              \
}                                                                                     \
                                                                                      \
static __attribute__((noinline))                                                      \
RTYPE NAME##_SYNC_SLOW(WorkerP *w, Task *__dq_head)                                   \
{                                                                                     \
    TD_##NAME *t;                                                                     \
                                                                                      \
    if ((w->allstolen) || (w->split > __dq_head && lace_shrink_shared(w))) {          \
        lace_leapfrog(w, __dq_head);                                                  \
        t = (TD_##NAME *)__dq_head;                                                   \
        return ((TD_##NAME *)t)->d.res;                                               \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (wt->movesplit) {                                                              \
        Task *t = w->split;                                                           \
        size_t diff = __dq_head - t;                                                  \
        diff = (diff + 1) / 2;                                                        \
        w->split = t + diff;                                                          \
        wt->ts.ts.split += diff;                                                      \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->thief = THIEF_EMPTY;                                                           \
    return NAME##_CALL(w, __dq_head , t->d.args.arg_1);                               \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
RTYPE NAME##_SYNC(WorkerP *w, Task *__dq_head)                                        \
{                                                                                     \
    /* assert (__dq_head > 0); */  /* Commented out because we assume contract */     \
                                                                                      \
    if (likely(0 == w->_public->movesplit)) {                                         \
        if (likely(w->split <= __dq_head)) {                                          \
            TD_##NAME *t = (TD_##NAME *)__dq_head;                                    \
            t->thief = THIEF_EMPTY;                                                   \
            return NAME##_CALL(w, __dq_head , t->d.args.arg_1);                       \
        }                                                                             \
    }                                                                                 \
                                                                                      \
    return NAME##_SYNC_SLOW(w, __dq_head);                                            \
}                                                                                     \
                                                                                      \
                                                                                      \

#define TASK_IMPL_1(RTYPE, NAME, ATYPE_1, ARG_1)                                      \
void NAME##_WRAP(WorkerP *w, Task *__dq_head, TD_##NAME *t __attribute__((unused)))   \
{                                                                                     \
    t->d.res = NAME##_CALL(w, __dq_head , t->d.args.arg_1);                           \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
RTYPE NAME##_WORK(WorkerP *__lace_worker, Task *__lace_dq_head , ATYPE_1);            \
                                                                                      \
/* NAME##_WORK is inlined in NAME##_CALL and the parameter __lace_in_task will disappear */\
RTYPE NAME##_CALL(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1)                        \
{                                                                                     \
    CHECKSTACK(w);                                                                    \
    return NAME##_WORK(w, __dq_head , arg_1);                                         \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
RTYPE NAME##_WORK(WorkerP *__lace_worker __attribute__((unused)), Task *__lace_dq_head __attribute__((unused)) , ATYPE_1 ARG_1)\

#define TASK_1(RTYPE, NAME, ATYPE_1, ARG_1) TASK_DECL_1(RTYPE, NAME, ATYPE_1) TASK_IMPL_1(RTYPE, NAME, ATYPE_1, ARG_1)

#define VOID_TASK_DECL_1(NAME, ATYPE_1)                                               \
                                                                                      \
typedef struct _TD_##NAME {                                                           \
  TASK_COMMON_FIELDS(_TD_##NAME)                                                      \
  union { struct {  ATYPE_1 arg_1; } args; } d;                                       \
} TD_##NAME;                                                                          \
                                                                                      \
/* If this line generates an error, please manually set the define LACE_TASKSIZE to a higher value */\
typedef char assertion_failed_task_descriptor_out_of_bounds_##NAME[(sizeof(TD_##NAME)<=sizeof(Task)) ? 0 : -1];\
                                                                                      \
void NAME##_WRAP(WorkerP *, Task *, TD_##NAME *);                                     \
void NAME##_CALL(WorkerP *, Task * , ATYPE_1 arg_1);                                  \
static inline void NAME##_SYNC(WorkerP *, Task *);                                    \
static void NAME##_SYNC_SLOW(WorkerP *, Task *);                                      \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SPAWN(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1)                        \
{                                                                                     \
    PR_COUNTTASK(w);                                                                  \
                                                                                      \
    TD_##NAME *t;                                                                     \
    TailSplit ts;                                                                     \
    uint32_t head, split, newsplit;                                                   \
                                                                                      \
    if (__dq_head == w->end) lace_abort_stack_overflow();                             \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1;                                                         \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (unlikely(w->allstolen)) {                                                     \
        if (wt->movesplit) wt->movesplit = 0;                                         \
        head = __dq_head - w->dq;                                                     \
        ts = (TailSplit){{head,head+1}};                                              \
        wt->ts.v = ts.v;                                                              \
        compiler_barrier();                                                           \
        wt->allstolen = 0;                                                            \
        w->split = __dq_head+1;                                                       \
        w->allstolen = 0;                                                             \
    } else if (unlikely(wt->movesplit)) {                                             \
        head = __dq_head - w->dq;                                                     \
        split = w->split - w->dq;                                                     \
        newsplit = (split + head + 2)/2;                                              \
        wt->ts.ts.split = newsplit;                                                   \
        w->split = w->dq + newsplit;                                                  \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_NEWFRAME(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1)                     \
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1;                                                         \
                                                                                      \
    lace_do_newframe(w, __dq_head, &_t);                                              \
    return ;                                                                          \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_TOGETHER(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1)                     \
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1;                                                         \
                                                                                      \
    lace_do_together(w, __dq_head, &_t);                                              \
}                                                                                     \
                                                                                      \
static __attribute__((noinline))                                                      \
void NAME##_SYNC_SLOW(WorkerP *w, Task *__dq_head)                                    \
{                                                                                     \
    TD_##NAME *t;                                                                     \
                                                                                      \
    if ((w->allstolen) || (w->split > __dq_head && lace_shrink_shared(w))) {          \
        lace_leapfrog(w, __dq_head);                                                  \
        t = (TD_##NAME *)__dq_head;                                                   \
        return ;                                                                      \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (wt->movesplit) {                                                              \
        Task *t = w->split;                                                           \
        size_t diff = __dq_head - t;                                                  \
        diff = (diff + 1) / 2;                                                        \
        w->split = t + diff;                                                          \
        wt->ts.ts.split += diff;                                                      \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->thief = THIEF_EMPTY;                                                           \
    return NAME##_CALL(w, __dq_head , t->d.args.arg_1);                               \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SYNC(WorkerP *w, Task *__dq_head)                                         \
{                                                                                     \
    /* assert (__dq_head > 0); */  /* Commented out because we assume contract */     \
                                                                                      \
    if (likely(0 == w->_public->movesplit)) {                                         \
        if (likely(w->split <= __dq_head)) {                                          \
            TD_##NAME *t = (TD_##NAME *)__dq_head;                                    \
            t->thief = THIEF_EMPTY;                                                   \
            return NAME##_CALL(w, __dq_head , t->d.args.arg_1);                       \
        }                                                                             \
    }                                                                                 \
                                                                                      \
    return NAME##_SYNC_SLOW(w, __dq_head);                                            \
}                                                                                     \
                                                                                      \
                                                                                      \

#define VOID_TASK_IMPL_1(NAME, ATYPE_1, ARG_1)                                        \
void NAME##_WRAP(WorkerP *w, Task *__dq_head, TD_##NAME *t __attribute__((unused)))   \
{                                                                                     \
     NAME##_CALL(w, __dq_head , t->d.args.arg_1);                                     \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
void NAME##_WORK(WorkerP *__lace_worker, Task *__lace_dq_head , ATYPE_1);             \
                                                                                      \
/* NAME##_WORK is inlined in NAME##_CALL and the parameter __lace_in_task will disappear */\
void NAME##_CALL(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1)                         \
{                                                                                     \
    CHECKSTACK(w);                                                                    \
    return NAME##_WORK(w, __dq_head , arg_1);                                         \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
void NAME##_WORK(WorkerP *__lace_worker __attribute__((unused)), Task *__lace_dq_head __attribute__((unused)) , ATYPE_1 ARG_1)\

#define VOID_TASK_1(NAME, ATYPE_1, ARG_1) VOID_TASK_DECL_1(NAME, ATYPE_1) VOID_TASK_IMPL_1(NAME, ATYPE_1, ARG_1)


// Task macros for tasks of arity 2

#define TASK_DECL_2(RTYPE, NAME, ATYPE_1, ATYPE_2)                                    \
                                                                                      \
typedef struct _TD_##NAME {                                                           \
  TASK_COMMON_FIELDS(_TD_##NAME)                                                      \
  union { struct {  ATYPE_1 arg_1; ATYPE_2 arg_2; } args; RTYPE res; } d;             \
} TD_##NAME;                                                                          \
                                                                                      \
/* If this line generates an error, please manually set the define LACE_TASKSIZE to a higher value */\
typedef char assertion_failed_task_descriptor_out_of_bounds_##NAME[(sizeof(TD_##NAME)<=sizeof(Task)) ? 0 : -1];\
                                                                                      \
void NAME##_WRAP(WorkerP *, Task *, TD_##NAME *);                                     \
RTYPE NAME##_CALL(WorkerP *, Task * , ATYPE_1 arg_1, ATYPE_2 arg_2);                  \
static inline RTYPE NAME##_SYNC(WorkerP *, Task *);                                   \
static RTYPE NAME##_SYNC_SLOW(WorkerP *, Task *);                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SPAWN(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2)         \
{                                                                                     \
    PR_COUNTTASK(w);                                                                  \
                                                                                      \
    TD_##NAME *t;                                                                     \
    TailSplit ts;                                                                     \
    uint32_t head, split, newsplit;                                                   \
                                                                                      \
    if (__dq_head == w->end) lace_abort_stack_overflow();                             \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2;                                \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (unlikely(w->allstolen)) {                                                     \
        if (wt->movesplit) wt->movesplit = 0;                                         \
        head = __dq_head - w->dq;                                                     \
        ts = (TailSplit){{head,head+1}};                                              \
        wt->ts.v = ts.v;                                                              \
        compiler_barrier();                                                           \
        wt->allstolen = 0;                                                            \
        w->split = __dq_head+1;                                                       \
        w->allstolen = 0;                                                             \
    } else if (unlikely(wt->movesplit)) {                                             \
        head = __dq_head - w->dq;                                                     \
        split = w->split - w->dq;                                                     \
        newsplit = (split + head + 2)/2;                                              \
        wt->ts.ts.split = newsplit;                                                   \
        w->split = w->dq + newsplit;                                                  \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
RTYPE NAME##_NEWFRAME(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2)     \
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2;                                \
                                                                                      \
    lace_do_newframe(w, __dq_head, &_t);                                              \
    return ((TD_##NAME *)t)->d.res;                                                   \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_TOGETHER(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2)      \
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2;                                \
                                                                                      \
    lace_do_together(w, __dq_head, &_t);                                              \
}                                                                                     \
                                                                                      \
static __attribute__((noinline))                                                      \
RTYPE NAME##_SYNC_SLOW(WorkerP *w, Task *__dq_head)                                   \
{                                                                                     \
    TD_##NAME *t;                                                                     \
                                                                                      \
    if ((w->allstolen) || (w->split > __dq_head && lace_shrink_shared(w))) {          \
        lace_leapfrog(w, __dq_head);                                                  \
        t = (TD_##NAME *)__dq_head;                                                   \
        return ((TD_##NAME *)t)->d.res;                                               \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (wt->movesplit) {                                                              \
        Task *t = w->split;                                                           \
        size_t diff = __dq_head - t;                                                  \
        diff = (diff + 1) / 2;                                                        \
        w->split = t + diff;                                                          \
        wt->ts.ts.split += diff;                                                      \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->thief = THIEF_EMPTY;                                                           \
    return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2);              \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
RTYPE NAME##_SYNC(WorkerP *w, Task *__dq_head)                                        \
{                                                                                     \
    /* assert (__dq_head > 0); */  /* Commented out because we assume contract */     \
                                                                                      \
    if (likely(0 == w->_public->movesplit)) {                                         \
        if (likely(w->split <= __dq_head)) {                                          \
            TD_##NAME *t = (TD_##NAME *)__dq_head;                                    \
            t->thief = THIEF_EMPTY;                                                   \
            return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2);      \
        }                                                                             \
    }                                                                                 \
                                                                                      \
    return NAME##_SYNC_SLOW(w, __dq_head);                                            \
}                                                                                     \
                                                                                      \
                                                                                      \

#define TASK_IMPL_2(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2)                      \
void NAME##_WRAP(WorkerP *w, Task *__dq_head, TD_##NAME *t __attribute__((unused)))   \
{                                                                                     \
    t->d.res = NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2);          \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
RTYPE NAME##_WORK(WorkerP *__lace_worker, Task *__lace_dq_head , ATYPE_1, ATYPE_2);   \
                                                                                      \
/* NAME##_WORK is inlined in NAME##_CALL and the parameter __lace_in_task will disappear */\
RTYPE NAME##_CALL(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2)         \
{                                                                                     \
    CHECKSTACK(w);                                                                    \
    return NAME##_WORK(w, __dq_head , arg_1, arg_2);                                  \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
RTYPE NAME##_WORK(WorkerP *__lace_worker __attribute__((unused)), Task *__lace_dq_head __attribute__((unused)) , ATYPE_1 ARG_1, ATYPE_2 ARG_2)\

#define TASK_2(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2) TASK_DECL_2(RTYPE, NAME, ATYPE_1, ATYPE_2) TASK_IMPL_2(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2)

#define VOID_TASK_DECL_2(NAME, ATYPE_1, ATYPE_2)                                      \
                                                                                      \
typedef struct _TD_##NAME {                                                           \
  TASK_COMMON_FIELDS(_TD_##NAME)                                                      \
  union { struct {  ATYPE_1 arg_1; ATYPE_2 arg_2; } args; } d;                        \
} TD_##NAME;                                                                          \
                                                                                      \
/* If this line generates an error, please manually set the define LACE_TASKSIZE to a higher value */\
typedef char assertion_failed_task_descriptor_out_of_bounds_##NAME[(sizeof(TD_##NAME)<=sizeof(Task)) ? 0 : -1];\
                                                                                      \
void NAME##_WRAP(WorkerP *, Task *, TD_##NAME *);                                     \
void NAME##_CALL(WorkerP *, Task * , ATYPE_1 arg_1, ATYPE_2 arg_2);                   \
static inline void NAME##_SYNC(WorkerP *, Task *);                                    \
static void NAME##_SYNC_SLOW(WorkerP *, Task *);                                      \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SPAWN(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2)         \
{                                                                                     \
    PR_COUNTTASK(w);                                                                  \
                                                                                      \
    TD_##NAME *t;                                                                     \
    TailSplit ts;                                                                     \
    uint32_t head, split, newsplit;                                                   \
                                                                                      \
    if (__dq_head == w->end) lace_abort_stack_overflow();                             \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2;                                \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (unlikely(w->allstolen)) {                                                     \
        if (wt->movesplit) wt->movesplit = 0;                                         \
        head = __dq_head - w->dq;                                                     \
        ts = (TailSplit){{head,head+1}};                                              \
        wt->ts.v = ts.v;                                                              \
        compiler_barrier();                                                           \
        wt->allstolen = 0;                                                            \
        w->split = __dq_head+1;                                                       \
        w->allstolen = 0;                                                             \
    } else if (unlikely(wt->movesplit)) {                                             \
        head = __dq_head - w->dq;                                                     \
        split = w->split - w->dq;                                                     \
        newsplit = (split + head + 2)/2;                                              \
        wt->ts.ts.split = newsplit;                                                   \
        w->split = w->dq + newsplit;                                                  \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_NEWFRAME(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2)      \
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2;                                \
                                                                                      \
    lace_do_newframe(w, __dq_head, &_t);                                              \
    return ;                                                                          \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_TOGETHER(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2)      \
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2;                                \
                                                                                      \
    lace_do_together(w, __dq_head, &_t);                                              \
}                                                                                     \
                                                                                      \
static __attribute__((noinline))                                                      \
void NAME##_SYNC_SLOW(WorkerP *w, Task *__dq_head)                                    \
{                                                                                     \
    TD_##NAME *t;                                                                     \
                                                                                      \
    if ((w->allstolen) || (w->split > __dq_head && lace_shrink_shared(w))) {          \
        lace_leapfrog(w, __dq_head);                                                  \
        t = (TD_##NAME *)__dq_head;                                                   \
        return ;                                                                      \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (wt->movesplit) {                                                              \
        Task *t = w->split;                                                           \
        size_t diff = __dq_head - t;                                                  \
        diff = (diff + 1) / 2;                                                        \
        w->split = t + diff;                                                          \
        wt->ts.ts.split += diff;                                                      \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->thief = THIEF_EMPTY;                                                           \
    return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2);              \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SYNC(WorkerP *w, Task *__dq_head)                                         \
{                                                                                     \
    /* assert (__dq_head > 0); */  /* Commented out because we assume contract */     \
                                                                                      \
    if (likely(0 == w->_public->movesplit)) {                                         \
        if (likely(w->split <= __dq_head)) {                                          \
            TD_##NAME *t = (TD_##NAME *)__dq_head;                                    \
            t->thief = THIEF_EMPTY;                                                   \
            return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2);      \
        }                                                                             \
    }                                                                                 \
                                                                                      \
    return NAME##_SYNC_SLOW(w, __dq_head);                                            \
}                                                                                     \
                                                                                      \
                                                                                      \

#define VOID_TASK_IMPL_2(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2)                        \
void NAME##_WRAP(WorkerP *w, Task *__dq_head, TD_##NAME *t __attribute__((unused)))   \
{                                                                                     \
     NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2);                    \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
void NAME##_WORK(WorkerP *__lace_worker, Task *__lace_dq_head , ATYPE_1, ATYPE_2);    \
                                                                                      \
/* NAME##_WORK is inlined in NAME##_CALL and the parameter __lace_in_task will disappear */\
void NAME##_CALL(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2)          \
{                                                                                     \
    CHECKSTACK(w);                                                                    \
    return NAME##_WORK(w, __dq_head , arg_1, arg_2);                                  \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
void NAME##_WORK(WorkerP *__lace_worker __attribute__((unused)), Task *__lace_dq_head __attribute__((unused)) , ATYPE_1 ARG_1, ATYPE_2 ARG_2)\

#define VOID_TASK_2(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2) VOID_TASK_DECL_2(NAME, ATYPE_1, ATYPE_2) VOID_TASK_IMPL_2(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2)


// Task macros for tasks of arity 3

#define TASK_DECL_3(RTYPE, NAME, ATYPE_1, ATYPE_2, ATYPE_3)                           \
                                                                                      \
typedef struct _TD_##NAME {                                                           \
  TASK_COMMON_FIELDS(_TD_##NAME)                                                      \
  union { struct {  ATYPE_1 arg_1; ATYPE_2 arg_2; ATYPE_3 arg_3; } args; RTYPE res; } d;\
} TD_##NAME;                                                                          \
                                                                                      \
/* If this line generates an error, please manually set the define LACE_TASKSIZE to a higher value */\
typedef char assertion_failed_task_descriptor_out_of_bounds_##NAME[(sizeof(TD_##NAME)<=sizeof(Task)) ? 0 : -1];\
                                                                                      \
void NAME##_WRAP(WorkerP *, Task *, TD_##NAME *);                                     \
RTYPE NAME##_CALL(WorkerP *, Task * , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3);   \
static inline RTYPE NAME##_SYNC(WorkerP *, Task *);                                   \
static RTYPE NAME##_SYNC_SLOW(WorkerP *, Task *);                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SPAWN(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3)\
{                                                                                     \
    PR_COUNTTASK(w);                                                                  \
                                                                                      \
    TD_##NAME *t;                                                                     \
    TailSplit ts;                                                                     \
    uint32_t head, split, newsplit;                                                   \
                                                                                      \
    if (__dq_head == w->end) lace_abort_stack_overflow();                             \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3;       \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (unlikely(w->allstolen)) {                                                     \
        if (wt->movesplit) wt->movesplit = 0;                                         \
        head = __dq_head - w->dq;                                                     \
        ts = (TailSplit){{head,head+1}};                                              \
        wt->ts.v = ts.v;                                                              \
        compiler_barrier();                                                           \
        wt->allstolen = 0;                                                            \
        w->split = __dq_head+1;                                                       \
        w->allstolen = 0;                                                             \
    } else if (unlikely(wt->movesplit)) {                                             \
        head = __dq_head - w->dq;                                                     \
        split = w->split - w->dq;                                                     \
        newsplit = (split + head + 2)/2;                                              \
        wt->ts.ts.split = newsplit;                                                   \
        w->split = w->dq + newsplit;                                                  \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
RTYPE NAME##_NEWFRAME(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3)\
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3;       \
                                                                                      \
    lace_do_newframe(w, __dq_head, &_t);                                              \
    return ((TD_##NAME *)t)->d.res;                                                   \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_TOGETHER(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3)\
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3;       \
                                                                                      \
    lace_do_together(w, __dq_head, &_t);                                              \
}                                                                                     \
                                                                                      \
static __attribute__((noinline))                                                      \
RTYPE NAME##_SYNC_SLOW(WorkerP *w, Task *__dq_head)                                   \
{                                                                                     \
    TD_##NAME *t;                                                                     \
                                                                                      \
    if ((w->allstolen) || (w->split > __dq_head && lace_shrink_shared(w))) {          \
        lace_leapfrog(w, __dq_head);                                                  \
        t = (TD_##NAME *)__dq_head;                                                   \
        return ((TD_##NAME *)t)->d.res;                                               \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (wt->movesplit) {                                                              \
        Task *t = w->split;                                                           \
        size_t diff = __dq_head - t;                                                  \
        diff = (diff + 1) / 2;                                                        \
        w->split = t + diff;                                                          \
        wt->ts.ts.split += diff;                                                      \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->thief = THIEF_EMPTY;                                                           \
    return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3);\
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
RTYPE NAME##_SYNC(WorkerP *w, Task *__dq_head)                                        \
{                                                                                     \
    /* assert (__dq_head > 0); */  /* Commented out because we assume contract */     \
                                                                                      \
    if (likely(0 == w->_public->movesplit)) {                                         \
        if (likely(w->split <= __dq_head)) {                                          \
            TD_##NAME *t = (TD_##NAME *)__dq_head;                                    \
            t->thief = THIEF_EMPTY;                                                   \
            return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3);\
        }                                                                             \
    }                                                                                 \
                                                                                      \
    return NAME##_SYNC_SLOW(w, __dq_head);                                            \
}                                                                                     \
                                                                                      \
                                                                                      \

#define TASK_IMPL_3(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3)      \
void NAME##_WRAP(WorkerP *w, Task *__dq_head, TD_##NAME *t __attribute__((unused)))   \
{                                                                                     \
    t->d.res = NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3);\
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
RTYPE NAME##_WORK(WorkerP *__lace_worker, Task *__lace_dq_head , ATYPE_1, ATYPE_2, ATYPE_3);\
                                                                                      \
/* NAME##_WORK is inlined in NAME##_CALL and the parameter __lace_in_task will disappear */\
RTYPE NAME##_CALL(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3)\
{                                                                                     \
    CHECKSTACK(w);                                                                    \
    return NAME##_WORK(w, __dq_head , arg_1, arg_2, arg_3);                           \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
RTYPE NAME##_WORK(WorkerP *__lace_worker __attribute__((unused)), Task *__lace_dq_head __attribute__((unused)) , ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3)\

#define TASK_3(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3) TASK_DECL_3(RTYPE, NAME, ATYPE_1, ATYPE_2, ATYPE_3) TASK_IMPL_3(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3)

#define VOID_TASK_DECL_3(NAME, ATYPE_1, ATYPE_2, ATYPE_3)                             \
                                                                                      \
typedef struct _TD_##NAME {                                                           \
  TASK_COMMON_FIELDS(_TD_##NAME)                                                      \
  union { struct {  ATYPE_1 arg_1; ATYPE_2 arg_2; ATYPE_3 arg_3; } args; } d;         \
} TD_##NAME;                                                                          \
                                                                                      \
/* If this line generates an error, please manually set the define LACE_TASKSIZE to a higher value */\
typedef char assertion_failed_task_descriptor_out_of_bounds_##NAME[(sizeof(TD_##NAME)<=sizeof(Task)) ? 0 : -1];\
                                                                                      \
void NAME##_WRAP(WorkerP *, Task *, TD_##NAME *);                                     \
void NAME##_CALL(WorkerP *, Task * , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3);    \
static inline void NAME##_SYNC(WorkerP *, Task *);                                    \
static void NAME##_SYNC_SLOW(WorkerP *, Task *);                                      \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SPAWN(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3)\
{                                                                                     \
    PR_COUNTTASK(w);                                                                  \
                                                                                      \
    TD_##NAME *t;                                                                     \
    TailSplit ts;                                                                     \
    uint32_t head, split, newsplit;                                                   \
                                                                                      \
    if (__dq_head == w->end) lace_abort_stack_overflow();                             \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3;       \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (unlikely(w->allstolen)) {                                                     \
        if (wt->movesplit) wt->movesplit = 0;                                         \
        head = __dq_head - w->dq;                                                     \
        ts = (TailSplit){{head,head+1}};                                              \
        wt->ts.v = ts.v;                                                              \
        compiler_barrier();                                                           \
        wt->allstolen = 0;                                                            \
        w->split = __dq_head+1;                                                       \
        w->allstolen = 0;                                                             \
    } else if (unlikely(wt->movesplit)) {                                             \
        head = __dq_head - w->dq;                                                     \
        split = w->split - w->dq;                                                     \
        newsplit = (split + head + 2)/2;                                              \
        wt->ts.ts.split = newsplit;                                                   \
        w->split = w->dq + newsplit;                                                  \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_NEWFRAME(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3)\
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3;       \
                                                                                      \
    lace_do_newframe(w, __dq_head, &_t);                                              \
    return ;                                                                          \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_TOGETHER(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3)\
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3;       \
                                                                                      \
    lace_do_together(w, __dq_head, &_t);                                              \
}                                                                                     \
                                                                                      \
static __attribute__((noinline))                                                      \
void NAME##_SYNC_SLOW(WorkerP *w, Task *__dq_head)                                    \
{                                                                                     \
    TD_##NAME *t;                                                                     \
                                                                                      \
    if ((w->allstolen) || (w->split > __dq_head && lace_shrink_shared(w))) {          \
        lace_leapfrog(w, __dq_head);                                                  \
        t = (TD_##NAME *)__dq_head;                                                   \
        return ;                                                                      \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (wt->movesplit) {                                                              \
        Task *t = w->split;                                                           \
        size_t diff = __dq_head - t;                                                  \
        diff = (diff + 1) / 2;                                                        \
        w->split = t + diff;                                                          \
        wt->ts.ts.split += diff;                                                      \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->thief = THIEF_EMPTY;                                                           \
    return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3);\
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SYNC(WorkerP *w, Task *__dq_head)                                         \
{                                                                                     \
    /* assert (__dq_head > 0); */  /* Commented out because we assume contract */     \
                                                                                      \
    if (likely(0 == w->_public->movesplit)) {                                         \
        if (likely(w->split <= __dq_head)) {                                          \
            TD_##NAME *t = (TD_##NAME *)__dq_head;                                    \
            t->thief = THIEF_EMPTY;                                                   \
            return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3);\
        }                                                                             \
    }                                                                                 \
                                                                                      \
    return NAME##_SYNC_SLOW(w, __dq_head);                                            \
}                                                                                     \
                                                                                      \
                                                                                      \

#define VOID_TASK_IMPL_3(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3)        \
void NAME##_WRAP(WorkerP *w, Task *__dq_head, TD_##NAME *t __attribute__((unused)))   \
{                                                                                     \
     NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3);   \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
void NAME##_WORK(WorkerP *__lace_worker, Task *__lace_dq_head , ATYPE_1, ATYPE_2, ATYPE_3);\
                                                                                      \
/* NAME##_WORK is inlined in NAME##_CALL and the parameter __lace_in_task will disappear */\
void NAME##_CALL(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3)\
{                                                                                     \
    CHECKSTACK(w);                                                                    \
    return NAME##_WORK(w, __dq_head , arg_1, arg_2, arg_3);                           \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
void NAME##_WORK(WorkerP *__lace_worker __attribute__((unused)), Task *__lace_dq_head __attribute__((unused)) , ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3)\

#define VOID_TASK_3(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3) VOID_TASK_DECL_3(NAME, ATYPE_1, ATYPE_2, ATYPE_3) VOID_TASK_IMPL_3(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3)


// Task macros for tasks of arity 4

#define TASK_DECL_4(RTYPE, NAME, ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4)                  \
                                                                                      \
typedef struct _TD_##NAME {                                                           \
  TASK_COMMON_FIELDS(_TD_##NAME)                                                      \
  union { struct {  ATYPE_1 arg_1; ATYPE_2 arg_2; ATYPE_3 arg_3; ATYPE_4 arg_4; } args; RTYPE res; } d;\
} TD_##NAME;                                                                          \
                                                                                      \
/* If this line generates an error, please manually set the define LACE_TASKSIZE to a higher value */\
typedef char assertion_failed_task_descriptor_out_of_bounds_##NAME[(sizeof(TD_##NAME)<=sizeof(Task)) ? 0 : -1];\
                                                                                      \
void NAME##_WRAP(WorkerP *, Task *, TD_##NAME *);                                     \
RTYPE NAME##_CALL(WorkerP *, Task * , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4);\
static inline RTYPE NAME##_SYNC(WorkerP *, Task *);                                   \
static RTYPE NAME##_SYNC_SLOW(WorkerP *, Task *);                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SPAWN(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4)\
{                                                                                     \
    PR_COUNTTASK(w);                                                                  \
                                                                                      \
    TD_##NAME *t;                                                                     \
    TailSplit ts;                                                                     \
    uint32_t head, split, newsplit;                                                   \
                                                                                      \
    if (__dq_head == w->end) lace_abort_stack_overflow();                             \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4;\
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (unlikely(w->allstolen)) {                                                     \
        if (wt->movesplit) wt->movesplit = 0;                                         \
        head = __dq_head - w->dq;                                                     \
        ts = (TailSplit){{head,head+1}};                                              \
        wt->ts.v = ts.v;                                                              \
        compiler_barrier();                                                           \
        wt->allstolen = 0;                                                            \
        w->split = __dq_head+1;                                                       \
        w->allstolen = 0;                                                             \
    } else if (unlikely(wt->movesplit)) {                                             \
        head = __dq_head - w->dq;                                                     \
        split = w->split - w->dq;                                                     \
        newsplit = (split + head + 2)/2;                                              \
        wt->ts.ts.split = newsplit;                                                   \
        w->split = w->dq + newsplit;                                                  \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
RTYPE NAME##_NEWFRAME(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4)\
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4;\
                                                                                      \
    lace_do_newframe(w, __dq_head, &_t);                                              \
    return ((TD_##NAME *)t)->d.res;                                                   \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_TOGETHER(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4)\
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4;\
                                                                                      \
    lace_do_together(w, __dq_head, &_t);                                              \
}                                                                                     \
                                                                                      \
static __attribute__((noinline))                                                      \
RTYPE NAME##_SYNC_SLOW(WorkerP *w, Task *__dq_head)                                   \
{                                                                                     \
    TD_##NAME *t;                                                                     \
                                                                                      \
    if ((w->allstolen) || (w->split > __dq_head && lace_shrink_shared(w))) {          \
        lace_leapfrog(w, __dq_head);                                                  \
        t = (TD_##NAME *)__dq_head;                                                   \
        return ((TD_##NAME *)t)->d.res;                                               \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (wt->movesplit) {                                                              \
        Task *t = w->split;                                                           \
        size_t diff = __dq_head - t;                                                  \
        diff = (diff + 1) / 2;                                                        \
        w->split = t + diff;                                                          \
        wt->ts.ts.split += diff;                                                      \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->thief = THIEF_EMPTY;                                                           \
    return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4);\
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
RTYPE NAME##_SYNC(WorkerP *w, Task *__dq_head)                                        \
{                                                                                     \
    /* assert (__dq_head > 0); */  /* Commented out because we assume contract */     \
                                                                                      \
    if (likely(0 == w->_public->movesplit)) {                                         \
        if (likely(w->split <= __dq_head)) {                                          \
            TD_##NAME *t = (TD_##NAME *)__dq_head;                                    \
            t->thief = THIEF_EMPTY;                                                   \
            return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4);\
        }                                                                             \
    }                                                                                 \
                                                                                      \
    return NAME##_SYNC_SLOW(w, __dq_head);                                            \
}                                                                                     \
                                                                                      \
                                                                                      \

#define TASK_IMPL_4(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4)\
void NAME##_WRAP(WorkerP *w, Task *__dq_head, TD_##NAME *t __attribute__((unused)))   \
{                                                                                     \
    t->d.res = NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4);\
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
RTYPE NAME##_WORK(WorkerP *__lace_worker, Task *__lace_dq_head , ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4);\
                                                                                      \
/* NAME##_WORK is inlined in NAME##_CALL and the parameter __lace_in_task will disappear */\
RTYPE NAME##_CALL(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4)\
{                                                                                     \
    CHECKSTACK(w);                                                                    \
    return NAME##_WORK(w, __dq_head , arg_1, arg_2, arg_3, arg_4);                    \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
RTYPE NAME##_WORK(WorkerP *__lace_worker __attribute__((unused)), Task *__lace_dq_head __attribute__((unused)) , ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4)\

#define TASK_4(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4) TASK_DECL_4(RTYPE, NAME, ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4) TASK_IMPL_4(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4)

#define VOID_TASK_DECL_4(NAME, ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4)                    \
                                                                                      \
typedef struct _TD_##NAME {                                                           \
  TASK_COMMON_FIELDS(_TD_##NAME)                                                      \
  union { struct {  ATYPE_1 arg_1; ATYPE_2 arg_2; ATYPE_3 arg_3; ATYPE_4 arg_4; } args; } d;\
} TD_##NAME;                                                                          \
                                                                                      \
/* If this line generates an error, please manually set the define LACE_TASKSIZE to a higher value */\
typedef char assertion_failed_task_descriptor_out_of_bounds_##NAME[(sizeof(TD_##NAME)<=sizeof(Task)) ? 0 : -1];\
                                                                                      \
void NAME##_WRAP(WorkerP *, Task *, TD_##NAME *);                                     \
void NAME##_CALL(WorkerP *, Task * , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4);\
static inline void NAME##_SYNC(WorkerP *, Task *);                                    \
static void NAME##_SYNC_SLOW(WorkerP *, Task *);                                      \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SPAWN(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4)\
{                                                                                     \
    PR_COUNTTASK(w);                                                                  \
                                                                                      \
    TD_##NAME *t;                                                                     \
    TailSplit ts;                                                                     \
    uint32_t head, split, newsplit;                                                   \
                                                                                      \
    if (__dq_head == w->end) lace_abort_stack_overflow();                             \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4;\
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (unlikely(w->allstolen)) {                                                     \
        if (wt->movesplit) wt->movesplit = 0;                                         \
        head = __dq_head - w->dq;                                                     \
        ts = (TailSplit){{head,head+1}};                                              \
        wt->ts.v = ts.v;                                                              \
        compiler_barrier();                                                           \
        wt->allstolen = 0;                                                            \
        w->split = __dq_head+1;                                                       \
        w->allstolen = 0;                                                             \
    } else if (unlikely(wt->movesplit)) {                                             \
        head = __dq_head - w->dq;                                                     \
        split = w->split - w->dq;                                                     \
        newsplit = (split + head + 2)/2;                                              \
        wt->ts.ts.split = newsplit;                                                   \
        w->split = w->dq + newsplit;                                                  \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_NEWFRAME(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4)\
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4;\
                                                                                      \
    lace_do_newframe(w, __dq_head, &_t);                                              \
    return ;                                                                          \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_TOGETHER(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4)\
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4;\
                                                                                      \
    lace_do_together(w, __dq_head, &_t);                                              \
}                                                                                     \
                                                                                      \
static __attribute__((noinline))                                                      \
void NAME##_SYNC_SLOW(WorkerP *w, Task *__dq_head)                                    \
{                                                                                     \
    TD_##NAME *t;                                                                     \
                                                                                      \
    if ((w->allstolen) || (w->split > __dq_head && lace_shrink_shared(w))) {          \
        lace_leapfrog(w, __dq_head);                                                  \
        t = (TD_##NAME *)__dq_head;                                                   \
        return ;                                                                      \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (wt->movesplit) {                                                              \
        Task *t = w->split;                                                           \
        size_t diff = __dq_head - t;                                                  \
        diff = (diff + 1) / 2;                                                        \
        w->split = t + diff;                                                          \
        wt->ts.ts.split += diff;                                                      \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->thief = THIEF_EMPTY;                                                           \
    return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4);\
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SYNC(WorkerP *w, Task *__dq_head)                                         \
{                                                                                     \
    /* assert (__dq_head > 0); */  /* Commented out because we assume contract */     \
                                                                                      \
    if (likely(0 == w->_public->movesplit)) {                                         \
        if (likely(w->split <= __dq_head)) {                                          \
            TD_##NAME *t = (TD_##NAME *)__dq_head;                                    \
            t->thief = THIEF_EMPTY;                                                   \
            return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4);\
        }                                                                             \
    }                                                                                 \
                                                                                      \
    return NAME##_SYNC_SLOW(w, __dq_head);                                            \
}                                                                                     \
                                                                                      \
                                                                                      \

#define VOID_TASK_IMPL_4(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4)\
void NAME##_WRAP(WorkerP *w, Task *__dq_head, TD_##NAME *t __attribute__((unused)))   \
{                                                                                     \
     NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4);\
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
void NAME##_WORK(WorkerP *__lace_worker, Task *__lace_dq_head , ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4);\
                                                                                      \
/* NAME##_WORK is inlined in NAME##_CALL and the parameter __lace_in_task will disappear */\
void NAME##_CALL(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4)\
{                                                                                     \
    CHECKSTACK(w);                                                                    \
    return NAME##_WORK(w, __dq_head , arg_1, arg_2, arg_3, arg_4);                    \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
void NAME##_WORK(WorkerP *__lace_worker __attribute__((unused)), Task *__lace_dq_head __attribute__((unused)) , ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4)\

#define VOID_TASK_4(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4) VOID_TASK_DECL_4(NAME, ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4) VOID_TASK_IMPL_4(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4)


// Task macros for tasks of arity 5

#define TASK_DECL_5(RTYPE, NAME, ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4, ATYPE_5)         \
                                                                                      \
typedef struct _TD_##NAME {                                                           \
  TASK_COMMON_FIELDS(_TD_##NAME)                                                      \
  union { struct {  ATYPE_1 arg_1; ATYPE_2 arg_2; ATYPE_3 arg_3; ATYPE_4 arg_4; ATYPE_5 arg_5; } args; RTYPE res; } d;\
} TD_##NAME;                                                                          \
                                                                                      \
/* If this line generates an error, please manually set the define LACE_TASKSIZE to a higher value */\
typedef char assertion_failed_task_descriptor_out_of_bounds_##NAME[(sizeof(TD_##NAME)<=sizeof(Task)) ? 0 : -1];\
                                                                                      \
void NAME##_WRAP(WorkerP *, Task *, TD_##NAME *);                                     \
RTYPE NAME##_CALL(WorkerP *, Task * , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5);\
static inline RTYPE NAME##_SYNC(WorkerP *, Task *);                                   \
static RTYPE NAME##_SYNC_SLOW(WorkerP *, Task *);                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SPAWN(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5)\
{                                                                                     \
    PR_COUNTTASK(w);                                                                  \
                                                                                      \
    TD_##NAME *t;                                                                     \
    TailSplit ts;                                                                     \
    uint32_t head, split, newsplit;                                                   \
                                                                                      \
    if (__dq_head == w->end) lace_abort_stack_overflow();                             \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4; t->d.args.arg_5 = arg_5;\
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (unlikely(w->allstolen)) {                                                     \
        if (wt->movesplit) wt->movesplit = 0;                                         \
        head = __dq_head - w->dq;                                                     \
        ts = (TailSplit){{head,head+1}};                                              \
        wt->ts.v = ts.v;                                                              \
        compiler_barrier();                                                           \
        wt->allstolen = 0;                                                            \
        w->split = __dq_head+1;                                                       \
        w->allstolen = 0;                                                             \
    } else if (unlikely(wt->movesplit)) {                                             \
        head = __dq_head - w->dq;                                                     \
        split = w->split - w->dq;                                                     \
        newsplit = (split + head + 2)/2;                                              \
        wt->ts.ts.split = newsplit;                                                   \
        w->split = w->dq + newsplit;                                                  \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
RTYPE NAME##_NEWFRAME(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5)\
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4; t->d.args.arg_5 = arg_5;\
                                                                                      \
    lace_do_newframe(w, __dq_head, &_t);                                              \
    return ((TD_##NAME *)t)->d.res;                                                   \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_TOGETHER(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5)\
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4; t->d.args.arg_5 = arg_5;\
                                                                                      \
    lace_do_together(w, __dq_head, &_t);                                              \
}                                                                                     \
                                                                                      \
static __attribute__((noinline))                                                      \
RTYPE NAME##_SYNC_SLOW(WorkerP *w, Task *__dq_head)                                   \
{                                                                                     \
    TD_##NAME *t;                                                                     \
                                                                                      \
    if ((w->allstolen) || (w->split > __dq_head && lace_shrink_shared(w))) {          \
        lace_leapfrog(w, __dq_head);                                                  \
        t = (TD_##NAME *)__dq_head;                                                   \
        return ((TD_##NAME *)t)->d.res;                                               \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (wt->movesplit) {                                                              \
        Task *t = w->split;                                                           \
        size_t diff = __dq_head - t;                                                  \
        diff = (diff + 1) / 2;                                                        \
        w->split = t + diff;                                                          \
        wt->ts.ts.split += diff;                                                      \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->thief = THIEF_EMPTY;                                                           \
    return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4, t->d.args.arg_5);\
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
RTYPE NAME##_SYNC(WorkerP *w, Task *__dq_head)                                        \
{                                                                                     \
    /* assert (__dq_head > 0); */  /* Commented out because we assume contract */     \
                                                                                      \
    if (likely(0 == w->_public->movesplit)) {                                         \
        if (likely(w->split <= __dq_head)) {                                          \
            TD_##NAME *t = (TD_##NAME *)__dq_head;                                    \
            t->thief = THIEF_EMPTY;                                                   \
            return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4, t->d.args.arg_5);\
        }                                                                             \
    }                                                                                 \
                                                                                      \
    return NAME##_SYNC_SLOW(w, __dq_head);                                            \
}                                                                                     \
                                                                                      \
                                                                                      \

#define TASK_IMPL_5(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5)\
void NAME##_WRAP(WorkerP *w, Task *__dq_head, TD_##NAME *t __attribute__((unused)))   \
{                                                                                     \
    t->d.res = NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4, t->d.args.arg_5);\
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
RTYPE NAME##_WORK(WorkerP *__lace_worker, Task *__lace_dq_head , ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4, ATYPE_5);\
                                                                                      \
/* NAME##_WORK is inlined in NAME##_CALL and the parameter __lace_in_task will disappear */\
RTYPE NAME##_CALL(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5)\
{                                                                                     \
    CHECKSTACK(w);                                                                    \
    return NAME##_WORK(w, __dq_head , arg_1, arg_2, arg_3, arg_4, arg_5);             \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
RTYPE NAME##_WORK(WorkerP *__lace_worker __attribute__((unused)), Task *__lace_dq_head __attribute__((unused)) , ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5)\

#define TASK_5(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5) TASK_DECL_5(RTYPE, NAME, ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4, ATYPE_5) TASK_IMPL_5(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5)

#define VOID_TASK_DECL_5(NAME, ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4, ATYPE_5)           \
                                                                                      \
typedef struct _TD_##NAME {                                                           \
  TASK_COMMON_FIELDS(_TD_##NAME)                                                      \
  union { struct {  ATYPE_1 arg_1; ATYPE_2 arg_2; ATYPE_3 arg_3; ATYPE_4 arg_4; ATYPE_5 arg_5; } args; } d;\
} TD_##NAME;                                                                          \
                                                                                      \
/* If this line generates an error, please manually set the define LACE_TASKSIZE to a higher value */\
typedef char assertion_failed_task_descriptor_out_of_bounds_##NAME[(sizeof(TD_##NAME)<=sizeof(Task)) ? 0 : -1];\
                                                                                      \
void NAME##_WRAP(WorkerP *, Task *, TD_##NAME *);                                     \
void NAME##_CALL(WorkerP *, Task * , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5);\
static inline void NAME##_SYNC(WorkerP *, Task *);                                    \
static void NAME##_SYNC_SLOW(WorkerP *, Task *);                                      \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SPAWN(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5)\
{                                                                                     \
    PR_COUNTTASK(w);                                                                  \
                                                                                      \
    TD_##NAME *t;                                                                     \
    TailSplit ts;                                                                     \
    uint32_t head, split, newsplit;                                                   \
                                                                                      \
    if (__dq_head == w->end) lace_abort_stack_overflow();                             \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4; t->d.args.arg_5 = arg_5;\
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (unlikely(w->allstolen)) {                                                     \
        if (wt->movesplit) wt->movesplit = 0;                                         \
        head = __dq_head - w->dq;                                                     \
        ts = (TailSplit){{head,head+1}};                                              \
        wt->ts.v = ts.v;                                                              \
        compiler_barrier();                                                           \
        wt->allstolen = 0;                                                            \
        w->split = __dq_head+1;                                                       \
        w->allstolen = 0;                                                             \
    } else if (unlikely(wt->movesplit)) {                                             \
        head = __dq_head - w->dq;                                                     \
        split = w->split - w->dq;                                                     \
        newsplit = (split + head + 2)/2;                                              \
        wt->ts.ts.split = newsplit;                                                   \
        w->split = w->dq + newsplit;                                                  \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_NEWFRAME(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5)\
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4; t->d.args.arg_5 = arg_5;\
                                                                                      \
    lace_do_newframe(w, __dq_head, &_t);                                              \
    return ;                                                                          \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_TOGETHER(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5)\
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4; t->d.args.arg_5 = arg_5;\
                                                                                      \
    lace_do_together(w, __dq_head, &_t);                                              \
}                                                                                     \
                                                                                      \
static __attribute__((noinline))                                                      \
void NAME##_SYNC_SLOW(WorkerP *w, Task *__dq_head)                                    \
{                                                                                     \
    TD_##NAME *t;                                                                     \
                                                                                      \
    if ((w->allstolen) || (w->split > __dq_head && lace_shrink_shared(w))) {          \
        lace_leapfrog(w, __dq_head);                                                  \
        t = (TD_##NAME *)__dq_head;                                                   \
        return ;                                                                      \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (wt->movesplit) {                                                              \
        Task *t = w->split;                                                           \
        size_t diff = __dq_head - t;                                                  \
        diff = (diff + 1) / 2;                                                        \
        w->split = t + diff;                                                          \
        wt->ts.ts.split += diff;                                                      \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->thief = THIEF_EMPTY;                                                           \
    return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4, t->d.args.arg_5);\
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SYNC(WorkerP *w, Task *__dq_head)                                         \
{                                                                                     \
    /* assert (__dq_head > 0); */  /* Commented out because we assume contract */     \
                                                                                      \
    if (likely(0 == w->_public->movesplit)) {                                         \
        if (likely(w->split <= __dq_head)) {                                          \
            TD_##NAME *t = (TD_##NAME *)__dq_head;                                    \
            t->thief = THIEF_EMPTY;                                                   \
            return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4, t->d.args.arg_5);\
        }                                                                             \
    }                                                                                 \
                                                                                      \
    return NAME##_SYNC_SLOW(w, __dq_head);                                            \
}                                                                                     \
                                                                                      \
                                                                                      \

#define VOID_TASK_IMPL_5(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5)\
void NAME##_WRAP(WorkerP *w, Task *__dq_head, TD_##NAME *t __attribute__((unused)))   \
{                                                                                     \
     NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4, t->d.args.arg_5);\
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
void NAME##_WORK(WorkerP *__lace_worker, Task *__lace_dq_head , ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4, ATYPE_5);\
                                                                                      \
/* NAME##_WORK is inlined in NAME##_CALL and the parameter __lace_in_task will disappear */\
void NAME##_CALL(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5)\
{                                                                                     \
    CHECKSTACK(w);                                                                    \
    return NAME##_WORK(w, __dq_head , arg_1, arg_2, arg_3, arg_4, arg_5);             \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
void NAME##_WORK(WorkerP *__lace_worker __attribute__((unused)), Task *__lace_dq_head __attribute__((unused)) , ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5)\

#define VOID_TASK_5(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5) VOID_TASK_DECL_5(NAME, ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4, ATYPE_5) VOID_TASK_IMPL_5(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5)


// Task macros for tasks of arity 6

#define TASK_DECL_6(RTYPE, NAME, ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4, ATYPE_5, ATYPE_6)\
                                                                                      \
typedef struct _TD_##NAME {                                                           \
  TASK_COMMON_FIELDS(_TD_##NAME)                                                      \
  union { struct {  ATYPE_1 arg_1; ATYPE_2 arg_2; ATYPE_3 arg_3; ATYPE_4 arg_4; ATYPE_5 arg_5; ATYPE_6 arg_6; } args; RTYPE res; } d;\
} TD_##NAME;                                                                          \
                                                                                      \
/* If this line generates an error, please manually set the define LACE_TASKSIZE to a higher value */\
typedef char assertion_failed_task_descriptor_out_of_bounds_##NAME[(sizeof(TD_##NAME)<=sizeof(Task)) ? 0 : -1];\
                                                                                      \
void NAME##_WRAP(WorkerP *, Task *, TD_##NAME *);                                     \
RTYPE NAME##_CALL(WorkerP *, Task * , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5, ATYPE_6 arg_6);\
static inline RTYPE NAME##_SYNC(WorkerP *, Task *);                                   \
static RTYPE NAME##_SYNC_SLOW(WorkerP *, Task *);                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SPAWN(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5, ATYPE_6 arg_6)\
{                                                                                     \
    PR_COUNTTASK(w);                                                                  \
                                                                                      \
    TD_##NAME *t;                                                                     \
    TailSplit ts;                                                                     \
    uint32_t head, split, newsplit;                                                   \
                                                                                      \
    if (__dq_head == w->end) lace_abort_stack_overflow();                             \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4; t->d.args.arg_5 = arg_5; t->d.args.arg_6 = arg_6;\
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (unlikely(w->allstolen)) {                                                     \
        if (wt->movesplit) wt->movesplit = 0;                                         \
        head = __dq_head - w->dq;                                                     \
        ts = (TailSplit){{head,head+1}};                                              \
        wt->ts.v = ts.v;                                                              \
        compiler_barrier();                                                           \
        wt->allstolen = 0;                                                            \
        w->split = __dq_head+1;                                                       \
        w->allstolen = 0;                                                             \
    } else if (unlikely(wt->movesplit)) {                                             \
        head = __dq_head - w->dq;                                                     \
        split = w->split - w->dq;                                                     \
        newsplit = (split + head + 2)/2;                                              \
        wt->ts.ts.split = newsplit;                                                   \
        w->split = w->dq + newsplit;                                                  \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
RTYPE NAME##_NEWFRAME(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5, ATYPE_6 arg_6)\
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4; t->d.args.arg_5 = arg_5; t->d.args.arg_6 = arg_6;\
                                                                                      \
    lace_do_newframe(w, __dq_head, &_t);                                              \
    return ((TD_##NAME *)t)->d.res;                                                   \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_TOGETHER(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5, ATYPE_6 arg_6)\
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4; t->d.args.arg_5 = arg_5; t->d.args.arg_6 = arg_6;\
                                                                                      \
    lace_do_together(w, __dq_head, &_t);                                              \
}                                                                                     \
                                                                                      \
static __attribute__((noinline))                                                      \
RTYPE NAME##_SYNC_SLOW(WorkerP *w, Task *__dq_head)                                   \
{                                                                                     \
    TD_##NAME *t;                                                                     \
                                                                                      \
    if ((w->allstolen) || (w->split > __dq_head && lace_shrink_shared(w))) {          \
        lace_leapfrog(w, __dq_head);                                                  \
        t = (TD_##NAME *)__dq_head;                                                   \
        return ((TD_##NAME *)t)->d.res;                                               \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (wt->movesplit) {                                                              \
        Task *t = w->split;                                                           \
        size_t diff = __dq_head - t;                                                  \
        diff = (diff + 1) / 2;                                                        \
        w->split = t + diff;                                                          \
        wt->ts.ts.split += diff;                                                      \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->thief = THIEF_EMPTY;                                                           \
    return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4, t->d.args.arg_5, t->d.args.arg_6);\
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
RTYPE NAME##_SYNC(WorkerP *w, Task *__dq_head)                                        \
{                                                                                     \
    /* assert (__dq_head > 0); */  /* Commented out because we assume contract */     \
                                                                                      \
    if (likely(0 == w->_public->movesplit)) {                                         \
        if (likely(w->split <= __dq_head)) {                                          \
            TD_##NAME *t = (TD_##NAME *)__dq_head;                                    \
            t->thief = THIEF_EMPTY;                                                   \
            return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4, t->d.args.arg_5, t->d.args.arg_6);\
        }                                                                             \
    }                                                                                 \
                                                                                      \
    return NAME##_SYNC_SLOW(w, __dq_head);                                            \
}                                                                                     \
                                                                                      \
                                                                                      \

#define TASK_IMPL_6(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6)\
void NAME##_WRAP(WorkerP *w, Task *__dq_head, TD_##NAME *t __attribute__((unused)))   \
{                                                                                     \
    t->d.res = NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4, t->d.args.arg_5, t->d.args.arg_6);\
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
RTYPE NAME##_WORK(WorkerP *__lace_worker, Task *__lace_dq_head , ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4, ATYPE_5, ATYPE_6);\
                                                                                      \
/* NAME##_WORK is inlined in NAME##_CALL and the parameter __lace_in_task will disappear */\
RTYPE NAME##_CALL(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5, ATYPE_6 arg_6)\
{                                                                                     \
    CHECKSTACK(w);                                                                    \
    return NAME##_WORK(w, __dq_head , arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);      \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
RTYPE NAME##_WORK(WorkerP *__lace_worker __attribute__((unused)), Task *__lace_dq_head __attribute__((unused)) , ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6)\

#define TASK_6(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6) TASK_DECL_6(RTYPE, NAME, ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4, ATYPE_5, ATYPE_6) TASK_IMPL_6(RTYPE, NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6)

#define VOID_TASK_DECL_6(NAME, ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4, ATYPE_5, ATYPE_6)  \
                                                                                      \
typedef struct _TD_##NAME {                                                           \
  TASK_COMMON_FIELDS(_TD_##NAME)                                                      \
  union { struct {  ATYPE_1 arg_1; ATYPE_2 arg_2; ATYPE_3 arg_3; ATYPE_4 arg_4; ATYPE_5 arg_5; ATYPE_6 arg_6; } args; } d;\
} TD_##NAME;                                                                          \
                                                                                      \
/* If this line generates an error, please manually set the define LACE_TASKSIZE to a higher value */\
typedef char assertion_failed_task_descriptor_out_of_bounds_##NAME[(sizeof(TD_##NAME)<=sizeof(Task)) ? 0 : -1];\
                                                                                      \
void NAME##_WRAP(WorkerP *, Task *, TD_##NAME *);                                     \
void NAME##_CALL(WorkerP *, Task * , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5, ATYPE_6 arg_6);\
static inline void NAME##_SYNC(WorkerP *, Task *);                                    \
static void NAME##_SYNC_SLOW(WorkerP *, Task *);                                      \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SPAWN(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5, ATYPE_6 arg_6)\
{                                                                                     \
    PR_COUNTTASK(w);                                                                  \
                                                                                      \
    TD_##NAME *t;                                                                     \
    TailSplit ts;                                                                     \
    uint32_t head, split, newsplit;                                                   \
                                                                                      \
    if (__dq_head == w->end) lace_abort_stack_overflow();                             \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4; t->d.args.arg_5 = arg_5; t->d.args.arg_6 = arg_6;\
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (unlikely(w->allstolen)) {                                                     \
        if (wt->movesplit) wt->movesplit = 0;                                         \
        head = __dq_head - w->dq;                                                     \
        ts = (TailSplit){{head,head+1}};                                              \
        wt->ts.v = ts.v;                                                              \
        compiler_barrier();                                                           \
        wt->allstolen = 0;                                                            \
        w->split = __dq_head+1;                                                       \
        w->allstolen = 0;                                                             \
    } else if (unlikely(wt->movesplit)) {                                             \
        head = __dq_head - w->dq;                                                     \
        split = w->split - w->dq;                                                     \
        newsplit = (split + head + 2)/2;                                              \
        wt->ts.ts.split = newsplit;                                                   \
        w->split = w->dq + newsplit;                                                  \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_NEWFRAME(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5, ATYPE_6 arg_6)\
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4; t->d.args.arg_5 = arg_5; t->d.args.arg_6 = arg_6;\
                                                                                      \
    lace_do_newframe(w, __dq_head, &_t);                                              \
    return ;                                                                          \
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_TOGETHER(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5, ATYPE_6 arg_6)\
{                                                                                     \
    Task _t;                                                                          \
    TD_##NAME *t = (TD_##NAME *)&_t;                                                  \
    t->f = &NAME##_WRAP;                                                              \
    t->thief = THIEF_TASK;                                                            \
     t->d.args.arg_1 = arg_1; t->d.args.arg_2 = arg_2; t->d.args.arg_3 = arg_3; t->d.args.arg_4 = arg_4; t->d.args.arg_5 = arg_5; t->d.args.arg_6 = arg_6;\
                                                                                      \
    lace_do_together(w, __dq_head, &_t);                                              \
}                                                                                     \
                                                                                      \
static __attribute__((noinline))                                                      \
void NAME##_SYNC_SLOW(WorkerP *w, Task *__dq_head)                                    \
{                                                                                     \
    TD_##NAME *t;                                                                     \
                                                                                      \
    if ((w->allstolen) || (w->split > __dq_head && lace_shrink_shared(w))) {          \
        lace_leapfrog(w, __dq_head);                                                  \
        t = (TD_##NAME *)__dq_head;                                                   \
        return ;                                                                      \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    Worker *wt = w->_public;                                                          \
    if (wt->movesplit) {                                                              \
        Task *t = w->split;                                                           \
        size_t diff = __dq_head - t;                                                  \
        diff = (diff + 1) / 2;                                                        \
        w->split = t + diff;                                                          \
        wt->ts.ts.split += diff;                                                      \
        compiler_barrier();                                                           \
        wt->movesplit = 0;                                                            \
        PR_COUNTSPLITS(w, CTR_split_grow);                                            \
    }                                                                                 \
                                                                                      \
    compiler_barrier();                                                               \
                                                                                      \
    t = (TD_##NAME *)__dq_head;                                                       \
    t->thief = THIEF_EMPTY;                                                           \
    return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4, t->d.args.arg_5, t->d.args.arg_6);\
}                                                                                     \
                                                                                      \
static inline __attribute__((unused))                                                 \
void NAME##_SYNC(WorkerP *w, Task *__dq_head)                                         \
{                                                                                     \
    /* assert (__dq_head > 0); */  /* Commented out because we assume contract */     \
                                                                                      \
    if (likely(0 == w->_public->movesplit)) {                                         \
        if (likely(w->split <= __dq_head)) {                                          \
            TD_##NAME *t = (TD_##NAME *)__dq_head;                                    \
            t->thief = THIEF_EMPTY;                                                   \
            return NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4, t->d.args.arg_5, t->d.args.arg_6);\
        }                                                                             \
    }                                                                                 \
                                                                                      \
    return NAME##_SYNC_SLOW(w, __dq_head);                                            \
}                                                                                     \
                                                                                      \
                                                                                      \

#define VOID_TASK_IMPL_6(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6)\
void NAME##_WRAP(WorkerP *w, Task *__dq_head, TD_##NAME *t __attribute__((unused)))   \
{                                                                                     \
     NAME##_CALL(w, __dq_head , t->d.args.arg_1, t->d.args.arg_2, t->d.args.arg_3, t->d.args.arg_4, t->d.args.arg_5, t->d.args.arg_6);\
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
void NAME##_WORK(WorkerP *__lace_worker, Task *__lace_dq_head , ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4, ATYPE_5, ATYPE_6);\
                                                                                      \
/* NAME##_WORK is inlined in NAME##_CALL and the parameter __lace_in_task will disappear */\
void NAME##_CALL(WorkerP *w, Task *__dq_head , ATYPE_1 arg_1, ATYPE_2 arg_2, ATYPE_3 arg_3, ATYPE_4 arg_4, ATYPE_5 arg_5, ATYPE_6 arg_6)\
{                                                                                     \
    CHECKSTACK(w);                                                                    \
    return NAME##_WORK(w, __dq_head , arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);      \
}                                                                                     \
                                                                                      \
static inline __attribute__((always_inline))                                          \
void NAME##_WORK(WorkerP *__lace_worker __attribute__((unused)), Task *__lace_dq_head __attribute__((unused)) , ATYPE_1 ARG_1, ATYPE_2 ARG_2, ATYPE_3 ARG_3, ATYPE_4 ARG_4, ATYPE_5 ARG_5, ATYPE_6 ARG_6)\

#define VOID_TASK_6(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6) VOID_TASK_DECL_6(NAME, ATYPE_1, ATYPE_2, ATYPE_3, ATYPE_4, ATYPE_5, ATYPE_6) VOID_TASK_IMPL_6(NAME, ATYPE_1, ARG_1, ATYPE_2, ARG_2, ATYPE_3, ARG_3, ATYPE_4, ARG_4, ATYPE_5, ARG_5, ATYPE_6, ARG_6)



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
