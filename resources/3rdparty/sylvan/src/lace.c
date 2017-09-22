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
#include <sys/mman.h> // for mprotect
#include <sys/time.h> // for gettimeofday
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include <lace.h>

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
static size_t default_stacksize = 0; // 0 means "set by lace_init"
static size_t default_dqsize = 100000;

/**
 * Verbosity flag, set with lace_set_verbosity
 */
static int verbosity = 0;

/**
 * Number of workers and number of enabled/active workers
 */
static unsigned int n_workers = 0;
static unsigned int enabled_workers = 0;

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
static int lace_quits = 0;

/**
 * Thread-specific mechanism to access current worker data
 */
#ifdef __linux__ // use gcc thread-local storage (i.e. __thread variables)
static __thread WorkerP *current_worker;
#else
static pthread_key_t worker_key;
#endif

/**
 * worker_attr used for creating threads
 * - initialized by lace_init
 * - used by lace_spawn_worker
 */
static pthread_attr_t worker_attr;

/**
 * The condition/mutex pair for when the root thread sleeps until the end of the program
 */
static pthread_cond_t wait_until_done = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t wait_until_done_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * Data structure that contains the stack and stack size for each worker.
 */
struct lace_worker_init
{
    void* stack;
    size_t stacksize;
};

static struct lace_worker_init *workers_init;

/**
 * Global newframe variable used for the implementation of NEWFRAME and TOGETHER
 */
lace_newframe_t lace_newframe;

/**
 * Get the private Worker data of the current thread
 */
WorkerP*
lace_get_worker()
{
#ifdef __linux__
    return current_worker;
#else
    return (WorkerP*)pthread_getspecific(worker_key);
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
lace_default_stacksize()
{
    return default_stacksize;
}

/**
 * If we are collecting PIE times, then we need some helper functions.
 */
#if LACE_PIE_TIMES
static uint64_t count_at_start, count_at_end;
static long long unsigned us_elapsed_timer;

static void
us_elapsed_start(void)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    us_elapsed_timer = now.tv_sec * 1000000LL + now.tv_usec;
}

static long long unsigned
us_elapsed(void)
{
    struct timeval now;
    long long unsigned t;

    gettimeofday( &now, NULL );

    t = now.tv_sec * 1000000LL + now.tv_usec;

    return t - us_elapsed_timer;
}
#endif

/**
 * Lace barrier implementation, that synchronizes on all currently enabled workers.
 */
typedef struct {
    volatile int __attribute__((aligned(LINE_SIZE))) count;
    volatile int __attribute__((aligned(LINE_SIZE))) leaving;
    volatile int __attribute__((aligned(LINE_SIZE))) wait;
} barrier_t;

barrier_t lace_bar;

/**
 * Enter the Lace barrier and wait until all workers have entered the Lace barrier.
 */
void
lace_barrier()
{
    int wait = lace_bar.wait;
    if ((int)enabled_workers == __sync_add_and_fetch(&lace_bar.count, 1)) {
        lace_bar.count = 0;
        lace_bar.leaving = enabled_workers;
        lace_bar.wait = 1 - wait; // flip wait
    } else {
        while (wait == lace_bar.wait) {} // wait
    }

    __sync_add_and_fetch(&lace_bar.leaving, -1);
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
        fprintf(stderr, "Lace warning: hwloc_get_area_membind_nodeset returned -1!\n");
    }
    if (policy != HWLOC_MEMBIND_BIND) {
        fprintf(stderr, "Lace warning: Lace worker memory not bound with BIND policy!\n");
    }
#endif

    // check if CPU and node are on the same place
    if (!hwloc_bitmap_isincluded(memlocation, cpunodes)) {
        fprintf(stderr, "Lace warning: Lace thread not on same memory domain as data!\n");

        char *strp, *strp2, *strp3;
        hwloc_bitmap_list_asprintf(&strp, cpuset);
        hwloc_bitmap_list_asprintf(&strp2, cpunodes);
        hwloc_bitmap_list_asprintf(&strp3, memlocation);
        fprintf(stderr, "Worker %d is pinned on PUs %s, node %s; memory is pinned on node %s\n", w->worker, strp, strp2, strp3);
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
    workers_memory[worker] = mmap(NULL, workers_memory_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (workers_memory[worker] == MAP_FAILED) {
        fprintf(stderr, "Lace error: Unable to allocate memory for the Lace worker!\n");
        exit(1);
    }

    // Set pointers
    Worker *wt = workers[worker] = &workers_memory[worker]->worker_public;
    WorkerP *w = workers_p[worker] = &workers_memory[worker]->worker_private;
    w->dq = workers_memory[worker]->deque;
#ifdef __linux__
    current_worker = w;
#else
    pthread_setspecific(worker_key, w);
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
    w->enabled = 1;
    if (workers_init[worker].stack != 0) {
        w->stack_trigger = ((size_t)workers_init[worker].stack) + workers_init[worker].stacksize/20;
    } else {
        w->stack_trigger = 0;
    }
    w->rng = (((uint64_t)rand())<<32 | rand());

#if LACE_COUNT_EVENTS
    // Initialize counters
    { int k; for (k=0; k<CTR_MAX; k++) w->ctr[k] = 0; }
#endif

    // Synchronize with others
    lace_barrier();

#if LACE_PIE_TIMES
    w->time = gethrtime();
    w->level = 0;
#endif

    if (worker == 0) {
        lace_time_event(w, 1);
    }
}

/**
 * Some OSX systems do not implement pthread_barrier_t, so we provide an implementation here.
 */
#if defined(__APPLE__) && !defined(pthread_barrier_t)

typedef int pthread_barrierattr_t;
typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int tripCount;
} pthread_barrier_t;

static int
pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
    if(count == 0)
    {
        errno = EINVAL;
        return -1;
    }
    if(pthread_mutex_init(&barrier->mutex, 0) < 0)
    {
        return -1;
    }
    if(pthread_cond_init(&barrier->cond, 0) < 0)
    {
        pthread_mutex_destroy(&barrier->mutex);
        return -1;
    }
    barrier->tripCount = count;
    barrier->count = 0;

    return 0;
    (void)attr;
}

static int
pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    pthread_cond_destroy(&barrier->cond);
    pthread_mutex_destroy(&barrier->mutex);
    return 0;
}

static int
pthread_barrier_wait(pthread_barrier_t *barrier)
{
    pthread_mutex_lock(&barrier->mutex);
    ++(barrier->count);
    if(barrier->count >= barrier->tripCount)
    {
        barrier->count = 0;
        pthread_cond_broadcast(&barrier->cond);
        pthread_mutex_unlock(&barrier->mutex);
        return 1;
    }
    else
    {
        pthread_cond_wait(&barrier->cond, &(barrier->mutex));
        pthread_mutex_unlock(&barrier->mutex);
        return 0;
    }
}

#endif // defined(__APPLE__) && !defined(pthread_barrier_t)

static pthread_barrier_t suspend_barrier;
static volatile int must_suspend = 0, suspended = 0;

void
lace_suspend()
{
    if (suspended == 0) {
        suspended = 1;
        must_suspend = 1;
        lace_barrier();
        must_suspend = 0;
    }
}

void
lace_resume()
{
    if (suspended == 1) {
        suspended = 0;
        pthread_barrier_wait(&suspend_barrier);
    }
}

/**
 * Disable worker <worker>.
 * If the given worker is the current worker, this function does nothing.
 */
void
lace_disable_worker(unsigned int worker)
{
    unsigned int self = lace_get_worker()->worker;
    if (worker == self) return;
    if (workers_p[worker]->enabled == 1) {
        workers_p[worker]->enabled = 0;
        enabled_workers--;
    }
}

/**
 * Enable worker <worker>.
 * If the given worker is the current worker, this function does nothing.
 */
void
lace_enable_worker(unsigned int worker)
{
    unsigned int self = lace_get_worker()->worker;
    if (worker == self) return;
    if (workers_p[worker]->enabled == 0) {
        workers_p[worker]->enabled = 1;
        enabled_workers++;
    }
}

/**
 * Enables all workers 0..(N-1) and disables workers N..max.
 * This function _should_ be called by worker 0.
 * Ignores the current worker if >= N.
 * The number of workers is never reduces below 1.
 */
void
lace_set_workers(unsigned int workercount)
{
    if (workercount < 1) workercount = 1;
    if (workercount > n_workers) workercount = n_workers;
    enabled_workers = workercount;
    unsigned int self = lace_get_worker()->worker;
    if (self >= workercount) workercount--;
    for (unsigned int i=0; i<n_workers; i++) {
        workers_p[i]->enabled = (i < workercount || i == self) ? 1 : 0;
    }
}

/**
 * Get the number of currently enabled workers.
 */
unsigned int
lace_enabled_workers()
{
    return enabled_workers;
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
 * (Try to) steal and execute a task from a random worker.
 */
VOID_TASK_0(lace_steal_random)
{
    Worker *victim = workers[(__lace_worker->worker + 1 + rng(&__lace_worker->seed, n_workers-1)) % n_workers];

    YIELD_NEWFRAME();

    PR_COUNTSTEALS(__lace_worker, CTR_steal_tries);
    Worker *res = lace_steal(__lace_worker, __lace_dq_head, victim);
    if (res == LACE_STOLEN) {
        PR_COUNTSTEALS(__lace_worker, CTR_steals);
    } else if (res == LACE_BUSY) {
        PR_COUNTSTEALS(__lace_worker, CTR_steal_busy);
    }
}

/**
 * Variable to hold the main/root task.
 */
static lace_startup_cb main_cb;

/**
 * Wrapper around the main/root task.
 */
static void*
lace_main_wrapper(void *arg)
{
    lace_init_worker(0);
    lace_pin_worker();
    LACE_ME;
    WRAP(main_cb, arg);
    lace_exit();

    // Now signal that we're done
    pthread_mutex_lock(&wait_until_done_mutex);
    pthread_cond_broadcast(&wait_until_done);
    pthread_mutex_unlock(&wait_until_done_mutex);

    return NULL;
}

/**
 * Main Lace worker implementation.
 * Steal from random victims until "quit" is set.
 */
VOID_TASK_1(lace_steal_loop, int*, quit)
{
    // Determine who I am
    const int worker_id = __lace_worker->worker;

    // Prepare self, victim
    Worker ** const self = &workers[worker_id];
    Worker **victim = self;

#if LACE_PIE_TIMES
    __lace_worker->time = gethrtime();
#endif

    uint32_t seed = worker_id;
    unsigned int n = n_workers;
    int i=0;

    while(*(volatile int*)quit == 0) {
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

        YIELD_NEWFRAME();

        if (must_suspend) {
            lace_barrier();
            do {
                pthread_barrier_wait(&suspend_barrier);
            } while (__lace_worker->enabled == 0);
        }
    }
}

/**
 * Initialize worker 0.
 * Calls lace_init_worker and then signals the event.
 */
void
lace_init_main()
{
    lace_init_worker(0);
}

/**
 * Initialize the current thread as a Lace thread, and perform work-stealing
 * as worker <worker> until lace_exit() is called.
 *
 * For worker 0, use lace_init_main
 */
void
lace_run_worker(void)
{
    // Run the steal loop
    LACE_ME;
    CALL(lace_steal_loop, &lace_quits);

    // Time worker exit event
    lace_time_event(__lace_worker, 9);

    // Synchronize with lace_exit
    lace_barrier();
}

static void*
lace_default_worker_thread(void* arg)
{
    int worker = (int)(size_t)arg;
    lace_init_worker(worker);
    lace_pin_worker();
    lace_run_worker();
    return NULL;
}

pthread_t
lace_spawn_worker(int worker, size_t stacksize, void* (*fun)(void*), void* arg)
{
    // Determine stack size
    if (stacksize == 0) stacksize = default_stacksize;

    size_t pagesize = sysconf(_SC_PAGESIZE);
    stacksize = (stacksize + pagesize - 1) & ~(pagesize - 1); // ceil(stacksize, pagesize)

#if LACE_USE_HWLOC
    // Get our logical processor
    hwloc_obj_t pu = hwloc_get_obj_by_type(topo, HWLOC_OBJ_PU, worker % n_pus);

    // Allocate memory for the program stack
    void *stack_location = hwloc_alloc_membind(topo, stacksize + pagesize, pu->cpuset, HWLOC_MEMBIND_BIND, 0);
    if (stack_location == 0) {
        fprintf(stderr, "Lace error: Unable to allocate memory for the pthread stack!\n");
        exit(1);
    }
#else
    void *stack_location = mmap(NULL, stacksize+  pagesize, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
#endif

    if (0 != mprotect(stack_location, pagesize, PROT_NONE)) {
        fprintf(stderr, "Lace error: Unable to protect the allocated program stack with a guard page!\n");
        exit(1);
    }
    stack_location = (uint8_t *)stack_location + pagesize; // skip protected page.
    if (0 != pthread_attr_setstack(&worker_attr, stack_location, stacksize)) {
        fprintf(stderr, "Lace error: Unable to set the pthread stack in Lace!\n");
        exit(1);
    }

    workers_init[worker].stack = stack_location;
    workers_init[worker].stacksize = stacksize;

    if (fun == 0) {
        fun = lace_default_worker_thread;
        arg = (void*)(size_t)worker;
    }

    pthread_t res;
    pthread_create(&res, &worker_attr, fun, arg);
    return res;
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
 * Initialize Lace for work-stealing with <n> workers, where
 * each worker gets a task deque with <dqsize> elements.
 */
void
lace_init(unsigned int _n_workers, size_t dqsize)
{
#if LACE_USE_HWLOC
    // Initialize topology and information about cpus
    hwloc_topology_init(&topo);
    hwloc_topology_load(topo);

    n_nodes = hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_NODE);
    n_cores = hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_CORE);
    n_pus = hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_PU);
#elif defined(sched_getaffinity)
    cpu_set_t cs;
    CPU_ZERO(&cs);
    sched_getaffinity(0, sizeof(cs), &cs);
    unsigned int n_pus = CPU_COUNT(&cs);
#else
    unsigned int n_pus = sysconf(_SC_NPROCESSORS_ONLN);
#endif

    // Initialize globals
    n_workers = _n_workers == 0 ? n_pus : _n_workers;
    enabled_workers = n_workers;
    if (dqsize != 0) default_dqsize = dqsize;
    else dqsize = default_dqsize;
    lace_quits = 0;

    // Initialize Lace barrier
    lace_barrier_init();

    // Create suspend barrier
    pthread_barrier_init(&suspend_barrier, NULL, n_workers);

    // Allocate array with all workers
    if (posix_memalign((void**)&workers, LINE_SIZE, n_workers*sizeof(Worker*)) != 0 ||
        posix_memalign((void**)&workers_p, LINE_SIZE, n_workers*sizeof(WorkerP*)) != 0 ||
        posix_memalign((void**)&workers_memory, LINE_SIZE, n_workers*sizeof(worker_data*)) != 0) {
        fprintf(stderr, "Lace error: unable to allocate memory!\n");
        exit(1);
    }

    // Compute memory size for each worker
    workers_memory_size = sizeof(worker_data) + sizeof(Task) * dqsize;

    // Create pthread key
#ifndef __linux__
    pthread_key_create(&worker_key, NULL);
#endif

    // Prepare structures for thread creation
    pthread_attr_init(&worker_attr);

    // Set contention scope to system (instead of process)
    pthread_attr_setscope(&worker_attr, PTHREAD_SCOPE_SYSTEM);

    // Get default stack size
    if (pthread_attr_getstacksize(&worker_attr, &default_stacksize) != 0) {
        fprintf(stderr, "Lace warning: pthread_attr_getstacksize returned error!\n");
        default_stacksize = 1048576; // 1 megabyte default
    }

    if (verbosity) {
#if LACE_USE_HWLOC
        fprintf(stderr, "Initializing Lace, %u nodes, %u cores, %u logical processors, %d workers.\n", n_nodes, n_cores, n_pus, n_workers);
#else
        fprintf(stderr, "Initializing Lace, %u available cores, %d workers.\n", n_pus, n_workers);
#endif
    }

    // Prepare lace_init structure
    workers_init = (struct lace_worker_init*)calloc(1, sizeof(struct lace_worker_init) * n_workers);

    lace_newframe.t = NULL;

#if LACE_PIE_TIMES
    // Initialize counters for pie times
    us_elapsed_start();
    count_at_start = gethrtime();
#endif
}

/**
 * Start the worker threads.
 * If cb is set, then the current thread is suspended and Worker 0 is a new thread that starts with
 * the given cb(arg) as the root task.
 * If cb is not set, then the current thread is Worker 0 and this function returns.
 */
void
lace_startup(size_t stacksize, lace_startup_cb cb, void *arg)
{
    if (stacksize == 0) stacksize = default_stacksize;

    /* Report startup if verbose */
    if (verbosity) {
        if (cb != 0) {
            fprintf(stderr, "Lace startup, creating %d worker threads with program stack %zu bytes.\n", n_workers, stacksize);
        } else if (n_workers == 1) {
            fprintf(stderr, "Lace startup, creating 0 worker threads.\n");
        } else {
            fprintf(stderr, "Lace startup, creating %d worker threads with program stack %zu bytes.\n", n_workers-1, stacksize);
        }
    }

    /* Spawn all other workers */
    for (unsigned int i=1; i<n_workers; i++) lace_spawn_worker(i, stacksize, 0, 0);

    if (cb != 0) {
        /* If cb set, spawn worker 0 */
        main_cb = cb;
        lace_spawn_worker(0, stacksize, lace_main_wrapper, arg);

        /* Suspend this thread until cb returns */
        pthread_mutex_lock(&wait_until_done_mutex);
        if (lace_quits == 0) pthread_cond_wait(&wait_until_done, &wait_until_done_mutex);
        pthread_mutex_unlock(&wait_until_done_mutex);
    } else {
        /* If cb not set, use current thread as worker 0 */
        lace_init_worker(0);
    }
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
    int i;
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
    int i;
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
        fprintf(file, "Tasks per steal (%d): %zu\n", i,
            workers_p[i]->ctr[CTR_tasks]/(workers_p[i]->ctr[CTR_steals]+workers_p[i]->ctr[CTR_leaps]));
    }
    fprintf(file, "Tasks per steal (sum): %zu\n", ctr_all[CTR_tasks]/(ctr_all[CTR_steals]+ctr_all[CTR_leaps]));
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
 * End Lace. All disabled threads are re-enabled, and then all Workers are signaled to quit.
 * This function waits until all threads are done, then returns.
 */
void lace_exit()
{
    lace_time_event(lace_get_worker(), 2);

    // first suspend all enabled threads
    lace_suspend();

    // now enable all threads and tell them to quit
    lace_set_workers(n_workers);
    lace_quits = 1;

    // now resume all threads and wait until they all pass the barrier
    lace_resume();
    lace_barrier();

    // finally, destroy the barriers
    lace_barrier_destroy();
    pthread_barrier_destroy(&suspend_barrier);

#if LACE_COUNT_EVENTS
    lace_count_report_file(stderr);
#endif
}

void
lace_exec_in_new_frame(WorkerP *__lace_worker, Task *__lace_dq_head, Task *root)
{
    TailSplit old;
    uint8_t old_as;

    // save old tail, split, allstolen and initiate new frame
    {
        Worker *wt = __lace_worker->_public;

        old_as = wt->allstolen;
        wt->allstolen = 1;
        old.ts.split = wt->ts.ts.split;
        wt->ts.ts.split = 0;
        mfence();
        old.ts.tail = wt->ts.ts.tail;

        TailSplit ts_new;
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
    compiler_barrier();

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

VOID_TASK_2(lace_steal_loop_root, Task*, t, int*, done)
{
    t->f(__lace_worker, __lace_dq_head, t);
    *done = 1;
}

VOID_TASK_2(lace_together_helper, Task*, t, volatile int*, finished)
{
    t->f(__lace_worker, __lace_dq_head, t);

    for (;;) {
        int f = *finished;
        if (__sync_bool_compare_and_swap(finished, f, f-1)) break;
    }

    while (*finished != 0) STEAL_RANDOM();
}

static void
lace_sync_and_exec(WorkerP *__lace_worker, Task *__lace_dq_head, Task *root)
{
    // wait until other workers have made a local copy
    lace_barrier();

    // one worker sets t to 0 again
    if (LACE_WORKER_ID == 0) lace_newframe.t = 0;
    // else while (*(Task* volatile *)&lace_newframe.t != 0) {}

    // the above line is commented out since lace_exec_in_new_frame includes
    // a lace_barrier before the task is executed

    lace_exec_in_new_frame(__lace_worker, __lace_dq_head, root);
}

void
lace_yield(WorkerP *__lace_worker, Task *__lace_dq_head)
{
    // make a local copy of the task
    Task _t;
    memcpy(&_t, lace_newframe.t, sizeof(Task));

    // wait until all workers have made a local copy
    lace_barrier();

    // one worker sets t to 0 again
    if (LACE_WORKER_ID == 0) lace_newframe.t = 0;
    // else while (*(Task* volatile *)&lace_newframe.t != 0) {}

    // the above line is commented out since lace_exec_in_new_frame includes
    // a lace_barrier before the task is executed

    lace_exec_in_new_frame(__lace_worker, __lace_dq_head, &_t);
}

void
lace_do_together(WorkerP *__lace_worker, Task *__lace_dq_head, Task *t)
{
    /* synchronization integer */
    int done = n_workers;

    /* wrap task in lace_together_helper */
    Task _t2;
    TD_lace_together_helper *t2 = (TD_lace_together_helper *)&_t2;
    t2->f = lace_together_helper_WRAP;
    t2->thief = THIEF_TASK;
    t2->d.args.arg_1 = t;
    t2->d.args.arg_2 = &done;

    while (!__sync_bool_compare_and_swap(&lace_newframe.t, 0, &_t2)) lace_yield(__lace_worker, __lace_dq_head);
    lace_sync_and_exec(__lace_worker, __lace_dq_head, &_t2);
}

void
lace_do_newframe(WorkerP *__lace_worker, Task *__lace_dq_head, Task *t)
{
    /* synchronization integer */
    int done = 0;

    /* wrap task in lace_steal_loop_root */
    Task _t2;
    TD_lace_steal_loop_root *t2 = (TD_lace_steal_loop_root *)&_t2;
    t2->f = lace_steal_loop_root_WRAP;
    t2->thief = THIEF_TASK;
    t2->d.args.arg_1 = t;
    t2->d.args.arg_2 = &done;

    /* and create the lace_steal_loop task for other workers */
    Task _s;
    TD_lace_steal_loop *s = (TD_lace_steal_loop *)&_s;
    s->f = &lace_steal_loop_WRAP;
    s->thief = THIEF_TASK;
    s->d.args.arg_1 = &done;

    compiler_barrier();

    while (!__sync_bool_compare_and_swap(&lace_newframe.t, 0, &_s)) lace_yield(__lace_worker, __lace_dq_head);
    lace_sync_and_exec(__lace_worker, __lace_dq_head, &_t2);
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
