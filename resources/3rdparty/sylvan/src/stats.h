/*
 * Copyright 2011-2015 Formal Methods and Tools, University of Twente
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

#include <lace.h>
#include <sylvan_config.h>

#ifndef SYLVAN_STATS_H
#define SYLVAN_STATS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    BDD_ITE,
    BDD_AND,
    BDD_XOR,
    BDD_EXISTS,
    BDD_AND_EXISTS,
    BDD_RELNEXT,
    BDD_RELPREV,
    BDD_SATCOUNT,
    BDD_COMPOSE,
    BDD_RESTRICT,
    BDD_CONSTRAIN,
    BDD_CLOSURE,
    BDD_ISBDD,
    BDD_SUPPORT,
    BDD_PATHCOUNT,
    BDD_ITE_CACHEDPUT,
    BDD_AND_CACHEDPUT,
    BDD_XOR_CACHEDPUT,
    BDD_EXISTS_CACHEDPUT,
    BDD_AND_EXISTS_CACHEDPUT,
    BDD_RELNEXT_CACHEDPUT,
    BDD_RELPREV_CACHEDPUT,
    BDD_SATCOUNT_CACHEDPUT,
    BDD_COMPOSE_CACHEDPUT,
    BDD_RESTRICT_CACHEDPUT,
    BDD_CONSTRAIN_CACHEDPUT,
    BDD_CLOSURE_CACHEDPUT,
    BDD_ISBDD_CACHEDPUT,
    BDD_SUPPORT_CACHEDPUT,
    BDD_PATHCOUNT_CACHEDPUT,
    BDD_ITE_CACHED,
    BDD_AND_CACHED,
    BDD_XOR_CACHED,
    BDD_EXISTS_CACHED,
    BDD_AND_EXISTS_CACHED,
    BDD_RELNEXT_CACHED,
    BDD_RELPREV_CACHED,
    BDD_SATCOUNT_CACHED,
    BDD_COMPOSE_CACHED,
    BDD_RESTRICT_CACHED,
    BDD_CONSTRAIN_CACHED,
    BDD_CLOSURE_CACHED,
    BDD_ISBDD_CACHED,
    BDD_SUPPORT_CACHED,
    BDD_PATHCOUNT_CACHED,
    BDD_NODES_CREATED,
    BDD_NODES_REUSED,

    LDD_UNION,
    LDD_MINUS,
    LDD_INTERSECT,
    LDD_RELPROD,
    LDD_RELPREV,
    LDD_PROJECT,
    LDD_JOIN,
    LDD_MATCH,
    LDD_SATCOUNT,
    LDD_SATCOUNTL,
    LDD_ZIP,
    LDD_RELPROD_UNION,
    LDD_PROJECT_MINUS,
    LDD_UNION_CACHEDPUT,
    LDD_MINUS_CACHEDPUT,
    LDD_INTERSECT_CACHEDPUT,
    LDD_RELPROD_CACHEDPUT,
    LDD_RELPREV_CACHEDPUT,
    LDD_PROJECT_CACHEDPUT,
    LDD_JOIN_CACHEDPUT,
    LDD_MATCH_CACHEDPUT,
    LDD_SATCOUNT_CACHEDPUT,
    LDD_SATCOUNTL_CACHEDPUT,
    LDD_ZIP_CACHEDPUT,
    LDD_RELPROD_UNION_CACHEDPUT,
    LDD_PROJECT_MINUS_CACHEDPUT,
    LDD_UNION_CACHED,
    LDD_MINUS_CACHED,
    LDD_INTERSECT_CACHED,
    LDD_RELPROD_CACHED,
    LDD_RELPREV_CACHED,
    LDD_PROJECT_CACHED,
    LDD_JOIN_CACHED,
    LDD_MATCH_CACHED,
    LDD_SATCOUNT_CACHED,
    LDD_SATCOUNTL_CACHED,
    LDD_ZIP_CACHED,
    LDD_RELPROD_UNION_CACHED,
    LDD_PROJECT_MINUS_CACHED,
    LDD_NODES_CREATED,
    LDD_NODES_REUSED,

    LLMSSET_LOOKUP,

    SYLVAN_GC_COUNT,
    SYLVAN_COUNTER_COUNTER
} Sylvan_Counters;

typedef enum
{
    SYLVAN_GC,
    SYLVAN_TIMER_COUNTER
} Sylvan_Timers;

/**
 * Initialize stats system (done by sylvan_init_package)
 */
#define sylvan_stats_init() CALL(sylvan_stats_init)
VOID_TASK_DECL_0(sylvan_stats_init)

/**
 * Reset all counters (for statistics)
 */
#define sylvan_stats_reset() CALL(sylvan_stats_reset)
VOID_TASK_DECL_0(sylvan_stats_reset)

/**
 * Write statistic report to file (stdout, stderr, etc)
 */
void sylvan_stats_report(FILE* target, int color);

#if SYLVAN_STATS

/* Infrastructure for internal markings */
typedef struct
{
    uint64_t counters[SYLVAN_COUNTER_COUNTER];
    uint64_t timers[SYLVAN_TIMER_COUNTER];
    uint64_t timers_startstop[SYLVAN_TIMER_COUNTER];
} sylvan_stats_t;

#ifdef __MACH__
#include <mach/mach_time.h>
#define getabstime() mach_absolute_time()
#else
#include <time.h>
static uint64_t
getabstime()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint64_t t = ts.tv_sec;
    t *= 1000000000UL;
    t += ts.tv_nsec;
    return t;
}
#endif

#ifdef __ELF__
extern __thread sylvan_stats_t sylvan_stats;
#else
#include <pthread.h>
extern pthread_key_t sylvan_stats_key;
#endif

static inline void
sylvan_stats_count(size_t counter)
{
#ifdef __ELF__
    sylvan_stats.counters[counter]++;
#else
    sylvan_stats_t *sylvan_stats = (sylvan_stats_t*)pthread_getspecific(sylvan_stats_key);
    sylvan_stats->counters[counter]++;
#endif
}

static inline void
sylvan_stats_add(size_t counter, size_t amount)
{
#ifdef __ELF__
    sylvan_stats.counters[counter]+=amount;
#else
    sylvan_stats_t *sylvan_stats = (sylvan_stats_t*)pthread_getspecific(sylvan_stats_key);
    sylvan_stats->counters[counter]+=amount;
#endif
}

static inline void
sylvan_timer_start(size_t timer)
{
    uint64_t t = getabstime();

#ifdef __ELF__
    sylvan_stats.timers_startstop[timer] = t;
#else
    sylvan_stats_t *sylvan_stats = (sylvan_stats_t*)pthread_getspecific(sylvan_stats_key);
    sylvan_stats->timers_startstop[timer] = t;
#endif
}

static inline void
sylvan_timer_stop(size_t timer)
{
    uint64_t t = getabstime();

#ifdef __ELF__
    sylvan_stats.timers[timer] += (t - sylvan_stats.timers_startstop[timer]);
#else
    sylvan_stats_t *sylvan_stats = (sylvan_stats_t*)pthread_getspecific(sylvan_stats_key);
    sylvan_stats->timers[timer] += (t - sylvan_stats->timers_startstop[timer]);
#endif
}

#else

static inline void
sylvan_stats_count(size_t counter)
{
    (void)counter;
}

static inline void
sylvan_stats_add(size_t counter, size_t amount)
{
    (void)counter;
    (void)amount;
}

static inline void
sylvan_timer_start(size_t timer)
{
    (void)timer;
}

static inline void
sylvan_timer_stop(size_t timer)
{
    (void)timer;
}

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
