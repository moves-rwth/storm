/*
 * Copyright 2011-2016 Formal Methods and Tools, University of Twente
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

/* Do not include this file directly. Instead, include sylvan.h */

#ifndef SYLVAN_STATS_H
#define SYLVAN_STATS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OPCOUNTER(NAME) NAME, NAME ## _CACHEDPUT, NAME ## _CACHED

typedef enum {
    /* Creating nodes */
    BDD_NODES_CREATED,
    BDD_NODES_REUSED,
    LDD_NODES_CREATED,
    LDD_NODES_REUSED,

    /* BDD operations */
    OPCOUNTER(BDD_ITE),
    OPCOUNTER(BDD_AND),
    OPCOUNTER(BDD_XOR),
    OPCOUNTER(BDD_EXISTS),
    OPCOUNTER(BDD_PROJECT),
    OPCOUNTER(BDD_AND_EXISTS),
    OPCOUNTER(BDD_AND_PROJECT),
    OPCOUNTER(BDD_RELNEXT),
    OPCOUNTER(BDD_RELPREV),
    OPCOUNTER(BDD_SATCOUNT),
    OPCOUNTER(BDD_COMPOSE),
    OPCOUNTER(BDD_RESTRICT),
    OPCOUNTER(BDD_CONSTRAIN),
    OPCOUNTER(BDD_CLOSURE),
    OPCOUNTER(BDD_ISBDD),
    OPCOUNTER(BDD_SUPPORT),
    OPCOUNTER(BDD_PATHCOUNT),

    /* MTBDD operations */
    OPCOUNTER(MTBDD_APPLY),
    OPCOUNTER(MTBDD_UAPPLY),
    OPCOUNTER(MTBDD_ABSTRACT),
    OPCOUNTER(MTBDD_ITE),
    OPCOUNTER(MTBDD_EQUAL_NORM),
    OPCOUNTER(MTBDD_EQUAL_NORM_REL),
    OPCOUNTER(MTBDD_LEQ),
    OPCOUNTER(MTBDD_LESS),
    OPCOUNTER(MTBDD_GEQ),
    OPCOUNTER(MTBDD_GREATER),
    OPCOUNTER(MTBDD_AND_ABSTRACT_PLUS),
    OPCOUNTER(MTBDD_AND_ABSTRACT_MAX),
    OPCOUNTER(MTBDD_COMPOSE),
    OPCOUNTER(MTBDD_MINIMUM),
    OPCOUNTER(MTBDD_MAXIMUM),
    OPCOUNTER(MTBDD_EVAL_COMPOSE),

    /* LDD operations */
    OPCOUNTER(LDD_UNION),
    OPCOUNTER(LDD_MINUS),
    OPCOUNTER(LDD_INTERSECT),
    OPCOUNTER(LDD_RELPROD),
    OPCOUNTER(LDD_RELPREV),
    OPCOUNTER(LDD_PROJECT),
    OPCOUNTER(LDD_JOIN),
    OPCOUNTER(LDD_MATCH),
    OPCOUNTER(LDD_SATCOUNT),
    OPCOUNTER(LDD_SATCOUNTL),
    OPCOUNTER(LDD_ZIP),
    OPCOUNTER(LDD_RELPROD_UNION),
    OPCOUNTER(LDD_PROJECT_MINUS),

    /* Other counters */
    SYLVAN_GC_COUNT,
    LLMSSET_LOOKUP,

    SYLVAN_COUNTER_COUNTER
} Sylvan_Counters;

#undef OPCOUNTER

typedef enum
{
    SYLVAN_GC,
    SYLVAN_TIMER_COUNTER
} Sylvan_Timers;

typedef struct
{
    uint64_t counters[SYLVAN_COUNTER_COUNTER];
    /* the timers are in ns */
    uint64_t timers[SYLVAN_TIMER_COUNTER];
    /* startstop is for internal use */
    uint64_t timers_startstop[SYLVAN_TIMER_COUNTER];
} sylvan_stats_t;

/**
 * Initialize stats system (done by sylvan_init_package)
 */
VOID_TASK_DECL_0(sylvan_stats_init);
#define sylvan_stats_init() CALL(sylvan_stats_init)

/**
 * Reset all counters (for statistics)
 */
VOID_TASK_DECL_0(sylvan_stats_reset);
#define sylvan_stats_reset() CALL(sylvan_stats_reset)

/**
 * Obtain current counts (this stops the world during counting)
 */
VOID_TASK_DECL_1(sylvan_stats_snapshot, sylvan_stats_t*);
#define sylvan_stats_snapshot(target) CALL(sylvan_stats_snapshot, target)

/**
 * Write statistic report to file (stdout, stderr, etc)
 */
void sylvan_stats_report(FILE* target);

#if SYLVAN_STATS

#ifdef __MACH__
#define getabstime() mach_absolute_time()
#else
static uint64_t
getabstime(void)
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
