/*
 * Copyright 2013-2016 Formal Methods and Tools, University of Twente
 * Copyright 2016-2017 Tom van Dijk, Johannes Kepler University Linz
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define _GNU_SOURCE
#include <errno.h> // for errno
#include <sched.h> // for sched_getaffinity
#include <stdio.h>  // for fprintf
#include <stdlib.h> // for memalign, malloc
#include <string.h> // for memset
#include <time.h> // for clock_gettime
#include <pthread.h> // for POSIX threading
#include <stdatomic.h>

#ifndef _WIN32
#include <sys/resource.h> // for getrlimit
#endif

#ifdef _WIN32
#include <windows.h> // to use GetSystemInfo
#undef NEWFRAME // otherwise we can't use NEWFRAME Lace macro
#endif

#if defined(__APPLE__)
/* Mac OS X defines sem_init but actually does not implement them */
#include <mach/mach.h>

typedef semaphore_t sem_t;
#define sem_init(sem, x, value)	semaphore_create(mach_task_self(), sem, SYNC_POLICY_FIFO, value)
#define sem_wait(sem)           semaphore_wait(*sem)
#define sem_post(sem)           semaphore_signal(*sem)
#define sem_destroy(sem)        semaphore_destroy(mach_task_self(), *sem)
#else
#include <semaphore.h> // for sem_*
#endif

#include <lace.h>

#if LACE_USE_MMAP
#include <sys/mman.h> // for mmap, etc
#endif

#if LACE_USE_HWLOC
#include <hwloc.h>

/**
 * HWLOC information
 */
static hwloc_topology_t topo;
static unsigned int n_nodes, n_cores, n_pus;
#endif

/**
 * (public) Worker data
 */
static Worker **workers = NULL;

/**
 * Default sizes for program stack and task deque
 */
static size_t stacksize = 0; // 0 means just take default
static size_t default_dqsize = 100000;

/**
 * Verbosity flag, set with lace_set_verbosity
 */
static int verbosity = 0;

/**
 * Number of workers
 */
static unsigned int n_workers = 0;

/**
 * Datastructure of the task deque etc for each worker.
 * - first public cachelines (accessible via global "workers" variable)
 * - then private cachelines
 * - then the deque array
 */
typedef struct {
    Worker worker_public;
    char pad1[PAD(sizeof(Worker), LINE_SIZE)];
    WorkerP worker_private;
    char pad2[PAD(sizeof(WorkerP), LINE_SIZE)];
    Task deque[];
} worker_data;

/**
 * (Secret) holds pointers to the memory block allocated for each worker
 */
static worker_data **workers_memory = NULL;

/**
 * Number of bytes allocated for each worker's worker data.
 */
static size_t workers_memory_size = 0;

/**
 * (Secret) holds pointer to private Worker data, just for stats collection at end
 */
static WorkerP **workers_p;

/**
 * Flag to signal all workers to quit.
 */
static atomic_int lace_quits = 0;
static atomic_uint workers_running = 0;

/**
 * Thread-specific mechanism to access current worker data
 */
#ifdef __linux__
static __thread WorkerP *current_worker;
#else
static pthread_key_t current_worker_key;
#endif

/**
 * Global newframe variable used for the implementation of NEWFRAME and TOGETHER
 */
lace_newframe_t lace_newframe;

/**
 * Retrieve whether we are running as a Lace worker
 */
int
lace_is_worker()
{
    return lace_get_worker() != NULL ? 1 : 0;
}

/**
 * Get the private Worker data of the current thread
 */
WorkerP*
lace_get_worker()
{
#ifdef __linux__
    return current_worker;
#else
    return (WorkerP*)pthread_getspecific(current_worker_key);
#endif
}

/**
 * Find the head of the task deque, using the given private Worker data
 */
Task*
lace_get_head(WorkerP *self)
{
    Task *dq = self->dq;

    /* First check the first tasks linearly */
    if (dq[0].thief == 0) return dq;
    if (dq[1].thief == 0) return dq+1;
    if (dq[2].thief == 0) return dq+2;
    if (dq[3].thief == 0) return dq+3;

    /* Then fast search for a low/high bound using powers of 2: 4, 8, 16... */
    size_t low = 2;
    size_t high = self->end - self->dq;

    for (;;) {
        if (low*2 >= high) {
            break;
        } else if (dq[low*2].thief == 0) {
            high=low*2;
            break;
        } else {
            low*=2;
        }
    }

    /* Finally zoom in using binary search */
    while (low < high) {
        size_t mid = low + (high-low)/2;
        if (dq[mid].thief == 0) high = mid;
        else low = mid + 1;
    }

    return dq+low;
}

/**
 * Get the number of workers
 */
unsigned int
lace_workers()
{
    return n_workers;
}

/**
 * Get the default stack size (or 0 for automatically determine)
 */
size_t
lace_get_stacksize()
{
    return stacksize;
}

/**
 * If we are collecting PIE times, then we need some helper functions.
 */
#if LACE_PIE_TIMES
static uint64_t count_at_start, count_at_end;
static uint64_t us_elapsed_timer;

static void
us_elapsed_start(void)
{
    struct timespec ts_now;
    clock_gettime(CLOCK_MONOTONIC, &ts_now);
    us_elapsed_timer = ts_now.tv_sec * 1000000LL + ts_now.tv_nsec/1000;
}

static long long unsigned
us_elapsed(void)
{
    struct timespec ts_now;
    clock_gettime(CLOCK_MONOTONIC, &ts_now);
    uint64_t t = ts_now.tv_sec * 1000000LL + ts_now.tv_nsec/1000;
    return t - us_elapsed_timer;
}
#endif

/**
 * Lace barrier implementation, that synchronizes on all workers.
 */
typedef struct {
    atomic_int __attribute__((aligned(LINE_SIZE))) count;
    atomic_int __attribute__((aligned(LINE_SIZE))) leaving;
    atomic_int __attribute__((aligned(LINE_SIZE))) wait;
} barrier_t;

barrier_t lace_bar;

/**
 * Enter the Lace barrier and wait until all workers have entered the Lace barrier.
 */
void
lace_barrier()
{
    int wait = lace_bar.wait;
    if ((int)n_workers == 1+(atomic_fetch_add_explicit(&lace_bar.count, 1, memory_order_relaxed))) {
        // we know for sure no other thread can be here
        atomic_store_explicit(&lace_bar.count, 0, memory_order_relaxed);
        atomic_store_explicit(&lace_bar.leaving, n_workers, memory_order_relaxed);
        // flip wait
        atomic_store_explicit(&lace_bar.wait, 1-wait, memory_order_relaxed);
    } else {
        while (wait == lace_bar.wait) {} // wait
    }
    lace_bar.leaving -= 1;
}

/**
 * Initialize the Lace barrier
 */
static void
lace_barrier_init()
{
    memset(&lace_bar, 0, sizeof(barrier_t));
}

/**
 * Destroy the Lace barrier (just wait until all are exited)
 */
static void
lace_barrier_destroy()
{
    // wait for all to exit
    while (lace_bar.leaving != 0) continue;
}

/**
 * For debugging purposes, check if memory is allocated on the correct memory nodes.
 */
static void __attribute__((unused))
lace_check_memory(void)
{
#if LACE_USE_HWLOC
    // get our current worker
    WorkerP *w = lace_get_worker();
    void* mem = workers_memory[w->worker];

    // get pinned PUs
    hwloc_cpuset_t cpuset = hwloc_bitmap_alloc();
    hwloc_get_cpubind(topo, cpuset, HWLOC_CPUBIND_THREAD);

    // get nodes of pinned PUs
    hwloc_nodeset_t cpunodes = hwloc_bitmap_alloc();
    hwloc_cpuset_to_nodeset(topo, cpuset, cpunodes);

    // get location of memory
    hwloc_nodeset_t memlocation = hwloc_bitmap_alloc();
#ifdef hwloc_get_area_memlocation
    hwloc_get_area_memlocation(topo, mem, sizeof(worker_data), memlocation, HWLOC_MEMBIND_BYNODESET);
#else
    hwloc_membind_policy_t policy;
    int res = hwloc_get_area_membind_nodeset(topo, mem, sizeof(worker_data), memlocation, &policy, HWLOC_MEMBIND_STRICT);
    if (res == -1) {
        fprintf(stdout, "Lace warning: hwloc_get_area_membind_nodeset returned -1!\n");
    }
    if (policy != HWLOC_MEMBIND_BIND) {
        fprintf(stdout, "Lace warning: Lace worker memory not bound with BIND policy!\n");
    }
#endif

    // check if CPU and node are on the same place
    if (!hwloc_bitmap_isincluded(memlocation, cpunodes)) {
        fprintf(stdout, "Lace warning: Lace thread not on same memory domain as data!\n");

        char *strp, *strp2, *strp3;
        hwloc_bitmap_list_asprintf(&strp, cpuset);
        hwloc_bitmap_list_asprintf(&strp2, cpunodes);
        hwloc_bitmap_list_asprintf(&strp3, memlocation);
        fprintf(stdout, "Worker %d is pinned on PUs %s, node %s; memory is pinned on node %s\n", w->worker, strp, strp2, strp3);
        free(strp);
        free(strp2);
        free(strp3);
    }

    // free allocated memory
    hwloc_bitmap_free(cpuset);
    hwloc_bitmap_free(cpunodes);
    hwloc_bitmap_free(memlocation);
#endif
}

void
lace_pin_worker(void)
{
#if LACE_USE_HWLOC
    // Get our worker
    unsigned int worker = lace_get_worker()->worker;

    // Get our core (hwloc object)
    hwloc_obj_t pu = hwloc_get_obj_by_type(topo, HWLOC_OBJ_CORE, worker % n_cores);

    // Get our copy of the bitmap
    hwloc_cpuset_t bmp = hwloc_bitmap_dup(pu->cpuset);

    // Get number of PUs in bitmap
    int n = -1, count=0;
    while ((n=hwloc_bitmap_next(bmp, n)) != -1) count++;

    // Check if we actually have any logical processors
    if (count == 0) {
        fprintf(stderr, "Lace error: trying to pin a worker on an empty core?\n");
        exit(-1);
    }

    // Select the correct PU on the core (in case of hyperthreading)
    int idx = worker / n_cores;
    if (idx >= count) {
        fprintf(stderr, "Lace warning: more workers than available logical processors!\n");
        idx %= count;
    }

    // Find index of PU and restrict bitmap
    n = -1;
    for (int i=0; i<=idx; i++) n = hwloc_bitmap_next(bmp, n);
    hwloc_bitmap_only(bmp, n);

    // Pin our thread...
    if (hwloc_set_cpubind(topo, bmp, HWLOC_CPUBIND_THREAD) == -1) {
        fprintf(stderr, "Lace warning: hwloc_set_cpubind returned -1!\n");
    }

    // Free our copy of the bitmap
    hwloc_bitmap_free(bmp);

    // Pin the memory area (using the appropriate hwloc function)
#ifdef HWLOC_MEMBIND_BYNODESET
    int res = hwloc_set_area_membind(topo, workers_memory[worker], workers_memory_size, pu->nodeset, HWLOC_MEMBIND_BIND, HWLOC_MEMBIND_STRICT | HWLOC_MEMBIND_MIGRATE | HWLOC_MEMBIND_BYNODESET);
#else
    int res = hwloc_set_area_membind_nodeset(topo, workers_memory[worker], workers_memory_size, pu->nodeset, HWLOC_MEMBIND_BIND, HWLOC_MEMBIND_STRICT | HWLOC_MEMBIND_MIGRATE);
#endif
    if (res != 0) {
        fprintf(stderr, "Lace error: Unable to bind worker memory to node!\n");
    }

    // Check if everything is on the correct node
    lace_check_memory();
#endif
}

void
lace_init_worker(unsigned int worker)
{
    // Allocate our memory
#if LACE_USE_MMAP
    workers_memory[worker] = mmap(NULL, workers_memory_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (workers_memory[worker] == MAP_FAILED) {
        fprintf(stderr, "Lace error: Unable to allocate mmapped memory for the Lace worker!\n");
        exit(1);
    }
#else
#if defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR)
    workers_memory[worker] = _aligned_malloc(workers_memory_size, LINE_SIZE);
#elif defined(__MINGW32__)
    workers_memory[worker] = __mingw_aligned_malloc(workers_memory_size, LINE_SIZE);
#else
    workers_memory[worker] = aligned_alloc(LINE_SIZE, workers_memory_size);
#endif
    if (workers_memory[worker] == 0) {
        fprintf(stderr, "Lace error: Unable to allocate memory for the Lace worker!\n");
        exit(1);
    }
    memset(workers_memory[worker], 0, workers_memory_size);
#endif

    // Set pointers
    Worker *wt = workers[worker] = &workers_memory[worker]->worker_public;
    WorkerP *w = workers_p[worker] = &workers_memory[worker]->worker_private;
    w->dq = workers_memory[worker]->deque;
#ifdef __linux__
    current_worker = w;
#else
    pthread_setspecific(current_worker_key, w);
#endif

    // Initialize public worker data
    wt->dq = w->dq;
    wt->ts.v = 0;
    wt->allstolen = 0;
    wt->movesplit = 0;

    // Initialize private worker data
    w->_public = wt;
    w->end = w->dq + default_dqsize;
    w->split = w->dq;
    w->allstolen = 0;
    w->worker = worker;
#if LACE_USE_HWLOC
    w->pu = worker % n_cores;
#else
    w->pu = -1;
#endif
    w->rng = (((uint64_t)rand())<<32 | rand());

#if LACE_COUNT_EVENTS
    // Initialize counters
    { int k; for (k=0; k<CTR_MAX; k++) w->ctr[k] = 0; }
#endif

#if LACE_PIE_TIMES
    w->time = gethrtime();
    w->level = 0;
#endif
}

static atomic_int must_suspend = 0;
static sem_t suspend_semaphore;
static atomic_int lace_awaken_count = 0;

void
lace_suspend()
{
    while (1) {
        int state = atomic_load_explicit(&lace_awaken_count, memory_order_consume);
        // state "should" be >= 1 !!
        if (state <= 0) {
            continue; // ???
        } else if (state == 1) {
            int next = -1; // intermediate state
            if (atomic_compare_exchange_weak(&lace_awaken_count, &state, next) == 1) {
                while (workers_running != n_workers) {} // they must first run, to avoid rare condition
                atomic_thread_fence(memory_order_seq_cst);
                atomic_store_explicit(&must_suspend, 1, memory_order_relaxed);
                while (workers_running != 0) {}
                atomic_thread_fence(memory_order_seq_cst);
                atomic_store_explicit(&must_suspend, 0, memory_order_relaxed);
                atomic_store_explicit(&lace_awaken_count, 0, memory_order_release);
                break;
            }
        } else {
            int next = state - 1;
            if (atomic_compare_exchange_weak(&lace_awaken_count, &state, next) == 1) break;
        }
    }
}

void
lace_resume()
{
    while (1) {
        int state = atomic_load_explicit(&lace_awaken_count, memory_order_consume);
        if (state < 0) {
            continue; // wait until suspending is done
        } else if (state == 0) {
            int next = -1; // intermediate state
            if (atomic_compare_exchange_weak(&lace_awaken_count, &state, next) == 1) {
                for (unsigned int i=0; i<n_workers; i++) sem_post(&suspend_semaphore);
                atomic_store_explicit(&lace_awaken_count, 1, memory_order_release);
                break;
            }
        } else {
            int next = state + 1;
            if (atomic_compare_exchange_weak(&lace_awaken_count, &state, next) == 1) break;
        }
    }
}

/**
 * Simple random number generated (like rand) using the given seed.
 * (Used for thread-specific (scalable) random number generation.
 */
static inline uint32_t
rng(uint32_t *seed, int max)
{
    uint32_t next = *seed;

    next *= 1103515245;
    next += 12345;

    *seed = next;

    return next % max;
}

/**
 * "External" task management
 */

/**
 * Global "external" task
 */
typedef struct _ExtTask {
    Task *task;
    sem_t sem;
} ExtTask;

static _Atomic(ExtTask*) external_task = NULL;

void
lace_run_task(Task *task)
{
    // check if we are really not in a Lace thread
    WorkerP* self = lace_get_worker();
    if (self != 0) {
        task->f(self, lace_get_head(self), task);
    } else {
        // if needed, wake up the workers
        lace_resume();

        ExtTask et;
        et.task = task;
        atomic_store_explicit(&et.task->thief, 0, memory_order_relaxed);
        sem_init(&et.sem, 0, 0);

        ExtTask *exp = 0;
        while (atomic_compare_exchange_weak(&external_task, &exp, &et) != 1) {}

        sem_wait(&et.sem);
        sem_destroy(&et.sem);

        // allow Lace workers to sleep again
        lace_suspend();
    }
}

static inline void
lace_steal_external(WorkerP *self, Task *dq_head) 
{
    ExtTask *stolen_task = atomic_exchange(&external_task, NULL);
    if (stolen_task != 0) {
        // execute task
        stolen_task->task->thief = self->_public;
        atomic_store_explicit(&stolen_task->task->thief, self->_public, memory_order_relaxed);
        lace_time_event(self, 1);
        // atomic_thread_fence(memory_order_relaxed);
        stolen_task->task->f(self, dq_head, stolen_task->task);
        // atomic_thread_fence(memory_order_relaxed);
        lace_time_event(self, 2);
        // atomic_thread_fence(memory_order_relaxed);
        atomic_store_explicit(&stolen_task->task->thief, THIEF_COMPLETED, memory_order_relaxed);
        // atomic_thread_fence(memory_order_relaxed);
        sem_post(&stolen_task->sem);
        lace_time_event(self, 8);
    }
}

/**
 * (Try to) steal and execute a task from a random worker.
 */
VOID_TASK_0(lace_steal_random)
{
    YIELD_NEWFRAME();

    if (unlikely(atomic_load_explicit(&external_task, memory_order_acquire) != 0)) {
        lace_steal_external(__lace_worker, __lace_dq_head);
    } else if (n_workers > 1) {
        Worker *victim = workers[(__lace_worker->worker + 1 + rng(&__lace_worker->seed, n_workers-1)) % n_workers];

        PR_COUNTSTEALS(__lace_worker, CTR_steal_tries);
        Worker *res = lace_steal(__lace_worker, __lace_dq_head, victim);
        if (res == LACE_STOLEN) {
            PR_COUNTSTEALS(__lace_worker, CTR_steals);
        } else if (res == LACE_BUSY) {
            PR_COUNTSTEALS(__lace_worker, CTR_steal_busy);
        }
    }
}

/**
 * Main Lace worker implementation.
 * Steal from random victims until "quit" is set.
 */
VOID_TASK_1(lace_steal_loop, atomic_int*, quit)
{
    // Determine who I am
    const int worker_id = __lace_worker->worker;

    // Prepare self, victim
    Worker ** const self = &workers[worker_id];
    Worker ** victim = self;

#if LACE_PIE_TIMES
    __lace_worker->time = gethrtime();
#endif

    uint32_t seed = worker_id;
    unsigned int n = n_workers;
    int i=0;

    while(*quit == 0) {
        if (n > 1) {
            // Select victim
            if( i>0 ) {
                i--;
                victim++;
                if (victim == self) victim++;
                if (victim >= workers + n) victim = workers;
                if (victim == self) victim++;
            } else {
                i = rng(&seed, 40); // compute random i 0..40
                victim = workers + (rng(&seed, n-1) + worker_id + 1) % n;
            }

            PR_COUNTSTEALS(__lace_worker, CTR_steal_tries);
            Worker *res = lace_steal(__lace_worker, __lace_dq_head, *victim);
            if (res == LACE_STOLEN) {
                PR_COUNTSTEALS(__lace_worker, CTR_steals);
            } else if (res == LACE_BUSY) {
                PR_COUNTSTEALS(__lace_worker, CTR_steal_busy);
            }
        }

        YIELD_NEWFRAME();

        if (unlikely(atomic_load_explicit(&external_task, memory_order_acquire) != 0)) {
            lace_steal_external(__lace_worker, __lace_dq_head);
        }

        if (unlikely(atomic_load_explicit(&must_suspend, memory_order_acquire))) {
            workers_running -= 1;
            sem_wait(&suspend_semaphore);
            lace_barrier(); // ensure we're all back before continuing
            workers_running += 1;
        }
    }
}

/**
 * Initialize the current thread as a Lace thread, and perform work-stealing
 * as worker <worker> until lace_exit() is called.
 */
static void*
lace_worker_thread(void* arg)
{
    int worker = (int)(size_t)arg;

    // Initialize data structures
    lace_init_worker(worker);

    // Pin CPU
    lace_pin_worker();

    // Wait for the first time we are resumed
    sem_wait(&suspend_semaphore);

    // Signal that we are running
    workers_running += 1;

    // Run the steal loop
    WorkerP *__lace_worker = lace_get_worker();
    Task *__lace_dq_head = lace_get_head(__lace_worker);
    lace_steal_loop_WORK(__lace_worker, __lace_dq_head, &lace_quits);

    // Time worker exit event
    lace_time_event(__lace_worker, 9);

    // Signal that we stopped
    workers_running -= 1;

    return NULL;
}

/**
 * Set the verbosity of Lace.
 */
void
lace_set_verbosity(int level)
{
    verbosity = level;
}

/**
 * Set the program stack size of Lace threads
 */
void
lace_set_stacksize(size_t new_stacksize)
{
    stacksize = new_stacksize;
}

unsigned int
lace_get_pu_count(void)
{
#ifdef WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    unsigned int n_pus = sysinfo.dwNumberOfProcessors;
#elif defined(sched_getaffinity)
    cpu_set_t cs;
    CPU_ZERO(&cs);
    sched_getaffinity(0, sizeof(cs), &cs);
    unsigned int n_pus = CPU_COUNT(&cs);
#else
    unsigned int n_pus = sysconf(_SC_NPROCESSORS_ONLN);
#endif
    return n_pus;
}

/**
 * Initialize Lace for work-stealing with <n> workers, where
 * each worker gets a task deque with <dqsize> elements.
 */
void
lace_start(unsigned int _n_workers, size_t dqsize)
{
#if LACE_USE_HWLOC
    // Initialize topology and information about cpus
    hwloc_topology_init(&topo);
    hwloc_topology_load(topo);

    n_nodes = hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_NODE);
    n_cores = hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_CORE);
    n_pus = hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_PU);
#else
    unsigned int n_pus = lace_get_pu_count();
#endif

    // Initialize globals
    n_workers = _n_workers == 0 ? n_pus : _n_workers;
    if (dqsize != 0) default_dqsize = dqsize;
    else dqsize = default_dqsize;
    lace_quits = 0;
    atomic_store_explicit(&workers_running, 0, memory_order_relaxed);

    // Initialize Lace barrier
    lace_barrier_init();

    // Create suspend semaphore
    memset(&suspend_semaphore, 0, sizeof(sem_t));
    sem_init(&suspend_semaphore, 0, 0);

    must_suspend = 0;
    lace_awaken_count = 0;

    // Allocate array with all workers
    // first make sure that the amount to allocate (n_workers times pointer) is a multiple of LINE_SIZE
    size_t to_allocate = n_workers * sizeof(void*);
    to_allocate = (to_allocate+LINE_SIZE-1) & (~(LINE_SIZE-1));
#if defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR)
    workers = _aligned_malloc(to_allocate, LINE_SIZE);
    workers_p = _aligned_malloc(to_allocate, LINE_SIZE);
    workers_memory = _aligned_malloc(to_allocate, LINE_SIZE);
#elif defined(__MINGW32__)
    workers = __mingw_aligned_malloc(to_allocate, LINE_SIZE);
    workers_p = __mingw_aligned_malloc(to_allocate, LINE_SIZE);
    workers_memory = __mingw_aligned_malloc(to_allocate, LINE_SIZE);
#else
    workers = aligned_alloc(LINE_SIZE, to_allocate);
    workers_p = aligned_alloc(LINE_SIZE, to_allocate);
    workers_memory = aligned_alloc(LINE_SIZE, to_allocate);
#endif
    if (workers == 0 || workers_p == 0 || workers_memory == 0) {
        fprintf(stderr, "Lace error: unable to allocate memory for the workers!\n");
        exit(1);
    }

    // Ensure worker array is set to 0 initially
    memset(workers, 0, n_workers*sizeof(Worker*));

    // Compute memory size for each worker
    workers_memory_size = sizeof(worker_data) + sizeof(Task) * dqsize;

#ifndef __linux__
    // Create pthread key
    pthread_key_create(&current_worker_key, NULL);
#endif

    // Prepare structures for thread creation
    pthread_attr_t worker_attr;
    pthread_attr_init(&worker_attr);

    // Set the stack size
    if (stacksize != 0) {
        pthread_attr_setstacksize(&worker_attr, stacksize);
    } else {
	// on certain systems, the default stack size is too small (e.g. OSX)
	// so by default, we just pick the current RLIMIT_STACK or 16M whichever is smallest
#ifndef _WIN32
	struct rlimit lim;
	getrlimit(RLIMIT_STACK, &lim);
	size_t size = lim.rlim_cur;
	if (size > 16*1024*1024) size = 16*1024*1024;
#else
	size_t size = 16*1024*1024;
#endif
        pthread_attr_setstacksize(&worker_attr, size);
    }

    if (verbosity) {
#if LACE_USE_HWLOC
        fprintf(stdout, "Initializing Lace, %u nodes, %u cores, %u logical processors, %d workers.\n", n_nodes, n_cores, n_pus, n_workers);
#else
        fprintf(stdout, "Initializing Lace, %u available cores, %d workers.\n", n_pus, n_workers);
#endif
    }

    // Prepare lace_init structure
    atomic_store_explicit(&lace_newframe.t, NULL, memory_order_relaxed);

#if LACE_PIE_TIMES
    // Initialize counters for pie times
    us_elapsed_start();
    count_at_start = gethrtime();
#endif

    /* Report startup if verbose */
    if (verbosity) {
        fprintf(stdout, "Lace startup, creating %d worker threads with program stack %zu bytes.\n", n_workers, stacksize);
    }

    /* Spawn all workers */
    for (unsigned int i=0; i<n_workers; i++) {
        pthread_t res;
        pthread_create(&res, &worker_attr, lace_worker_thread, (void*)(size_t)i);
    }

    /* Make sure we start resumed */
    lace_resume();

    pthread_attr_destroy(&worker_attr);
}


#if LACE_COUNT_EVENTS
static uint64_t ctr_all[CTR_MAX];
#endif

/**
 * Reset the counters of Lace.
 */
void
lace_count_reset()
{
#if LACE_COUNT_EVENTS
    unsigned int i;
    size_t j;

    for (i=0;i<n_workers;i++) {
        for (j=0;j<CTR_MAX;j++) {
            workers_p[i]->ctr[j] = 0;
        }
    }

#if LACE_PIE_TIMES
    for (i=0;i<n_workers;i++) {
        workers_p[i]->time = gethrtime();
        if (i != 0) workers_p[i]->level = 0;
    }

    us_elapsed_start();
    count_at_start = gethrtime();
#endif
#endif
}

/**
 * Report counters to the given file.
 */
void
lace_count_report_file(FILE *file)
{
#if LACE_COUNT_EVENTS
    unsigned int i;
    size_t j;

    for (j=0;j<CTR_MAX;j++) ctr_all[j] = 0;
    for (i=0;i<n_workers;i++) {
        uint64_t *wctr = workers_p[i]->ctr;
        for (j=0;j<CTR_MAX;j++) {
            ctr_all[j] += wctr[j];
        }
    }

#if LACE_COUNT_TASKS
    for (i=0;i<n_workers;i++) {
        fprintf(file, "Tasks (%d): %zu\n", i, workers_p[i]->ctr[CTR_tasks]);
    }
    fprintf(file, "Tasks (sum): %zu\n", ctr_all[CTR_tasks]);
    fprintf(file, "\n");
#endif

#if LACE_COUNT_STEALS
    for (i=0;i<n_workers;i++) {
        fprintf(file, "Steals (%d): %zu good/%zu busy of %zu tries; leaps: %zu good/%zu busy of %zu tries\n", i,
            workers_p[i]->ctr[CTR_steals], workers_p[i]->ctr[CTR_steal_busy],
            workers_p[i]->ctr[CTR_steal_tries], workers_p[i]->ctr[CTR_leaps],
            workers_p[i]->ctr[CTR_leap_busy], workers_p[i]->ctr[CTR_leap_tries]);
    }
    fprintf(file, "Steals (sum): %zu good/%zu busy of %zu tries; leaps: %zu good/%zu busy of %zu tries\n", 
        ctr_all[CTR_steals], ctr_all[CTR_steal_busy],
        ctr_all[CTR_steal_tries], ctr_all[CTR_leaps],
        ctr_all[CTR_leap_busy], ctr_all[CTR_leap_tries]);
    fprintf(file, "\n");
#endif

#if LACE_COUNT_STEALS && LACE_COUNT_TASKS
    for (i=0;i<n_workers;i++) {
        if ((workers_p[i]->ctr[CTR_steals]+workers_p[i]->ctr[CTR_leaps]) > 0) {
            fprintf(file, "Tasks per steal (%d): %zu\n", i,
                workers_p[i]->ctr[CTR_tasks]/(workers_p[i]->ctr[CTR_steals]+workers_p[i]->ctr[CTR_leaps]));
        }
    }
    if ((ctr_all[CTR_steals]+ctr_all[CTR_leaps]) > 0) {
        fprintf(file, "Tasks per steal (sum): %zu\n", ctr_all[CTR_tasks]/(ctr_all[CTR_steals]+ctr_all[CTR_leaps]));
    }
    fprintf(file, "\n");
#endif

#if LACE_COUNT_SPLITS
    for (i=0;i<n_workers;i++) {
        fprintf(file, "Splits (%d): %zu shrinks, %zu grows, %zu outgoing requests\n", i,
            workers_p[i]->ctr[CTR_split_shrink], workers_p[i]->ctr[CTR_split_grow], workers_p[i]->ctr[CTR_split_req]);
    }
    fprintf(file, "Splits (sum): %zu shrinks, %zu grows, %zu outgoing requests\n",
        ctr_all[CTR_split_shrink], ctr_all[CTR_split_grow], ctr_all[CTR_split_req]);
    fprintf(file, "\n");
#endif

#if LACE_PIE_TIMES
    count_at_end = gethrtime();

    uint64_t count_per_ms = (count_at_end - count_at_start) / (us_elapsed() / 1000);
    double dcpm = (double)count_per_ms;

    uint64_t sum_count;
    sum_count = ctr_all[CTR_init] + ctr_all[CTR_wapp] + ctr_all[CTR_lapp] + ctr_all[CTR_wsteal] + ctr_all[CTR_lsteal]
              + ctr_all[CTR_close] + ctr_all[CTR_wstealsucc] + ctr_all[CTR_lstealsucc] + ctr_all[CTR_wsignal]
              + ctr_all[CTR_lsignal];

    fprintf(file, "Measured clock (tick) frequency: %.2f GHz\n", count_per_ms / 1000000.0);
    fprintf(file, "Aggregated time per pie slice, total time: %.2f CPU seconds\n\n", sum_count / (1000*dcpm));

    for (i=0;i<n_workers;i++) {
        fprintf(file, "Startup time (%d):    %10.2f ms\n", i, workers_p[i]->ctr[CTR_init] / dcpm);
        fprintf(file, "Steal work (%d):      %10.2f ms\n", i, workers_p[i]->ctr[CTR_wapp] / dcpm);
        fprintf(file, "Leap work (%d):       %10.2f ms\n", i, workers_p[i]->ctr[CTR_lapp] / dcpm);
        fprintf(file, "Steal overhead (%d):  %10.2f ms\n", i, (workers_p[i]->ctr[CTR_wstealsucc]+workers_p[i]->ctr[CTR_wsignal]) / dcpm);
        fprintf(file, "Leap overhead (%d):   %10.2f ms\n", i, (workers_p[i]->ctr[CTR_lstealsucc]+workers_p[i]->ctr[CTR_lsignal]) / dcpm);
        fprintf(file, "Steal search (%d):    %10.2f ms\n", i, (workers_p[i]->ctr[CTR_wsteal]-workers_p[i]->ctr[CTR_wstealsucc]-workers_p[i]->ctr[CTR_wsignal]) / dcpm);
        fprintf(file, "Leap search (%d):     %10.2f ms\n", i, (workers_p[i]->ctr[CTR_lsteal]-workers_p[i]->ctr[CTR_lstealsucc]-workers_p[i]->ctr[CTR_lsignal]) / dcpm);
        fprintf(file, "Exit time (%d):       %10.2f ms\n", i, workers_p[i]->ctr[CTR_close] / dcpm);
        fprintf(file, "\n");
    }

    fprintf(file, "Startup time (sum):    %10.2f ms\n", ctr_all[CTR_init] / dcpm);
    fprintf(file, "Steal work (sum):      %10.2f ms\n", ctr_all[CTR_wapp] / dcpm);
    fprintf(file, "Leap work (sum):       %10.2f ms\n", ctr_all[CTR_lapp] / dcpm);
    fprintf(file, "Steal overhead (sum):  %10.2f ms\n", (ctr_all[CTR_wstealsucc]+ctr_all[CTR_wsignal]) / dcpm);
    fprintf(file, "Leap overhead (sum):   %10.2f ms\n", (ctr_all[CTR_lstealsucc]+ctr_all[CTR_lsignal]) / dcpm);
    fprintf(file, "Steal search (sum):    %10.2f ms\n", (ctr_all[CTR_wsteal]-ctr_all[CTR_wstealsucc]-ctr_all[CTR_wsignal]) / dcpm);
    fprintf(file, "Leap search (sum):     %10.2f ms\n", (ctr_all[CTR_lsteal]-ctr_all[CTR_lstealsucc]-ctr_all[CTR_lsignal]) / dcpm);
    fprintf(file, "Exit time (sum):       %10.2f ms\n", ctr_all[CTR_close] / dcpm);
    fprintf(file, "\n" );
#endif
#endif
    return;
    (void)file;
}

/**
 * End Lace. All Workers are signaled to quit.
 * This function waits until all threads are done, then returns.
 */
void lace_stop()
{
    // Workers need to be awake for this
    lace_resume();

    // Do not stop if not all workers are running yet
    while (workers_running != n_workers) {}

    lace_quits = 1;

    while (workers_running != 0) {}

#if LACE_COUNT_EVENTS
    lace_count_report_file(stdout);
#endif

    // finally, destroy the barriers
    lace_barrier_destroy();
    sem_destroy(&suspend_semaphore);

    for (unsigned int i=0; i<n_workers; i++) {
#if LACE_USE_MMAP
        munmap(workers_memory[i], workers_memory_size);
#elif defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR)
	_aligned_free(workers_memory[i]);
#elif defined(__MINGW32__)
	__mingw_aligned_free(workers_memory[i]);
#else
        free(workers_memory[i]);
#endif
    }

#if defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR)
    _aligned_free(workers);
    _aligned_free(workers_p);
    _aligned_free(workers_memory);
#elif defined(__MINGW32__)
    __mingw_aligned_free(workers);
    __mingw_aligned_free(workers_p);
    __mingw_aligned_free(workers_memory);
#else
    free(workers);
    free(workers_p);
    free(workers_memory);
#endif

    workers = 0;
    workers_p = 0;
    workers_memory = 0;
}

/**
 * Execute the given <root> task in a new frame (synchronizing with all Lace threads)
 * 1) Creates a new frame
 * 2) LACE BARRIER
 * 3) Execute the <root> task
 * 4) LACE BARRIER
 * 5) Restore the old frame
 */
void
lace_exec_in_new_frame(WorkerP *__lace_worker, Task *__lace_dq_head, Task *root)
{
    TailSplitNA old;
    uint8_t old_as;

    // save old tail, split, allstolen and initiate new frame
    {
        Worker *wt = __lace_worker->_public;

        old_as = wt->allstolen;
        wt->allstolen = 1;
        old.ts.split = wt->ts.ts.split;
        wt->ts.ts.split = 0;
        atomic_thread_fence(memory_order_seq_cst);
        old.ts.tail = wt->ts.ts.tail;

        TailSplitNA ts_new;
        ts_new.ts.tail = __lace_dq_head - __lace_worker->dq;
        ts_new.ts.split = __lace_dq_head - __lace_worker->dq;
        wt->ts.v = ts_new.v;

        __lace_worker->split = __lace_dq_head;
        __lace_worker->allstolen = 1;
    }

    // wait until all workers are ready
    lace_barrier();

    // execute task
    root->f(__lace_worker, __lace_dq_head, root);

    // wait until all workers are back (else they may steal from previous frame)
    lace_barrier();

    // restore tail, split, allstolen
    {
        Worker *wt = __lace_worker->_public;
        wt->allstolen = old_as;
        wt->ts.v = old.v;
        __lace_worker->split = __lace_worker->dq + old.ts.split;
        __lace_worker->allstolen = old_as;
    }
}

/**
 * This method is called when there is a new frame (NEWFRAME or TOGETHER)
 * Each Lace worker executes lace_yield to execute the task in a new frame.
 */
void
lace_yield(WorkerP *__lace_worker, Task *__lace_dq_head)
{
    // make a local copy of the task
    Task _t;
    memcpy(&_t, lace_newframe.t, sizeof(Task));

    // wait until all workers have made a local copy
    lace_barrier();

    lace_exec_in_new_frame(__lace_worker, __lace_dq_head, &_t);
}

/**
 * Root task for the TOGETHER method.
 * Ensures after executing, to steal random tasks until done.
 */
VOID_TASK_2(lace_together_root, Task*, t, atomic_int*, finished)
{
    // run the root task
    t->f(__lace_worker, __lace_dq_head, t);

    // signal out completion
    *finished -= 1;

    // while threads aren't done, steal randomly
    while (*finished != 0) STEAL_RANDOM();
}

VOID_TASK_1(lace_wrap_together, Task*, task)
{
    /* synchronization integer (decrease by 1 when done...) */
    atomic_int done = n_workers;

    /* wrap task in lace_together_root */
    Task _t2;
    TD_lace_together_root *t2 = (TD_lace_together_root *)&_t2;
    t2->f = lace_together_root_WRAP;
    atomic_store_explicit(&t2->thief, THIEF_TASK, memory_order_relaxed);
    t2->d.args.arg_1 = task;
    t2->d.args.arg_2 = &done;

    /* now try to be the one who sets it! */
    while (1) {
        Task *expected = 0;
        if (atomic_compare_exchange_weak(&lace_newframe.t, &expected, &_t2)) break;
        lace_yield(__lace_worker, __lace_dq_head);
    }

    // wait until other workers have made a local copy
    lace_barrier();

    // reset the newframe struct
    atomic_store_explicit(&lace_newframe.t, NULL, memory_order_relaxed);

    lace_exec_in_new_frame(__lace_worker, __lace_dq_head, &_t2);
}

VOID_TASK_2(lace_newframe_root, Task*, t, atomic_int*, done)
{
    t->f(__lace_worker, __lace_dq_head, t);
    *done = 1;
}

VOID_TASK_1(lace_wrap_newframe, Task*, task)
{
    /* synchronization integer (set to 1 when done...) */
    atomic_int done = 0;

    /* create the lace_steal_loop task for the other workers */
    Task _s;
    TD_lace_steal_loop *s = (TD_lace_steal_loop *)&_s;
    s->f = &lace_steal_loop_WRAP;
    atomic_store_explicit(&s->thief, THIEF_TASK, memory_order_relaxed);
    s->d.args.arg_1 = &done;

    /* now try to be the one who sets it! */
    while (1) {
        Task *expected = 0;
        if (atomic_compare_exchange_weak(&lace_newframe.t, &expected, &_s)) break;
        lace_yield(__lace_worker, __lace_dq_head);
    }

    // wait until other workers have made a local copy
    lace_barrier();

    // reset the newframe struct, then wrap and run ours
    atomic_store_explicit(&lace_newframe.t, NULL, memory_order_relaxed);

    /* wrap task in lace_newframe_root */
    Task _t2;
    TD_lace_newframe_root *t2 = (TD_lace_newframe_root *)&_t2;
    t2->f = lace_newframe_root_WRAP;
    atomic_store_explicit(&t2->thief, THIEF_TASK, memory_order_relaxed);
    t2->d.args.arg_1 = task;
    t2->d.args.arg_2 = &done;

    lace_exec_in_new_frame(__lace_worker, __lace_dq_head, &_t2);
}

void
lace_run_together(Task *t)
{
    WorkerP* self = lace_get_worker();
    if (self != 0) {
        lace_wrap_together_CALL(self, lace_get_head(self), t);
    } else {
        RUN(lace_wrap_together, t);
    }
}

void
lace_run_newframe(Task *t)
{
    WorkerP* self = lace_get_worker();
    if (self != 0) {
        lace_wrap_newframe_CALL(self, lace_get_head(self), t);
    } else {
        RUN(lace_wrap_newframe, t);
    }
}

/**
 * Called by _SPAWN functions when the Task stack is full.
 */
void
lace_abort_stack_overflow(void)
{
    fprintf(stderr, "Lace fatal error: Task stack overflow! Aborting.\n");
    exit(-1);
}
