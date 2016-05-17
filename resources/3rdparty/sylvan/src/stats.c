/*
 * Copyright 2011-2014 Formal Methods and Tools, University of Twente
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

#include <errno.h>  // for errno
#include <string.h> // memset
#include <stats.h>
#include <sys/mman.h>
#include <inttypes.h>
#include <sylvan.h> // for nodes table

#if SYLVAN_STATS

#ifdef __ELF__
__thread sylvan_stats_t sylvan_stats;
#else
pthread_key_t sylvan_stats_key;
#endif

#ifndef USE_HWLOC
#define USE_HWLOC 0
#endif

#if USE_HWLOC
#include <hwloc.h>
static hwloc_topology_t topo;
#endif

VOID_TASK_0(sylvan_stats_reset_perthread)
{
#ifdef __ELF__
    for (int i=0; i<SYLVAN_COUNTER_COUNTER; i++) {
        sylvan_stats.counters[i] = 0;
    }
    for (int i=0; i<SYLVAN_TIMER_COUNTER; i++) {
        sylvan_stats.timers[i] = 0;
    }
#else
    sylvan_stats_t *sylvan_stats = pthread_getspecific(sylvan_stats_key);
    if (sylvan_stats == NULL) {
        sylvan_stats = mmap(0, sizeof(sylvan_stats_t), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (sylvan_stats == (sylvan_stats_t *)-1) {
            fprintf(stderr, "sylvan_stats: Unable to allocate memory: %s!\n", strerror(errno));
            exit(1);
        }
#if USE_HWLOC
        // Ensure the stats object is on our pu
        hwloc_obj_t pu = hwloc_get_obj_by_type(topo, HWLOC_OBJ_PU, LACE_WORKER_PU);
        hwloc_set_area_membind(topo, sylvan_stats, sizeof(sylvan_stats_t), pu->cpuset, HWLOC_MEMBIND_BIND, 0);
#endif
        pthread_setspecific(sylvan_stats_key, sylvan_stats);
    }
    for (int i=0; i<SYLVAN_COUNTER_COUNTER; i++) {
        sylvan_stats->counters[i] = 0;
    }
    for (int i=0; i<SYLVAN_TIMER_COUNTER; i++) {
        sylvan_stats->timers[i] = 0;
    }
#endif
}

VOID_TASK_IMPL_0(sylvan_stats_init)
{
#ifndef __ELF__
    pthread_key_create(&sylvan_stats_key, NULL);
#endif
#if USE_HWLOC
    hwloc_topology_init(&topo);
    hwloc_topology_load(topo);
#endif
    TOGETHER(sylvan_stats_reset_perthread);
}

/**
 * Reset all counters (for statistics)
 */
VOID_TASK_IMPL_0(sylvan_stats_reset)
{
    TOGETHER(sylvan_stats_reset_perthread);
}

#define BLACK "\33[22;30m"
#define GRAY "\33[01;30m"
#define RED "\33[22;31m"
#define LRED "\33[01;31m"
#define GREEN "\33[22;32m"
#define LGREEN "\33[01;32m"
#define BLUE "\33[22;34m"
#define LBLUE "\33[01;34m"
#define BROWN "\33[22;33m"
#define YELLOW "\33[01;33m"
#define CYAN "\33[22;36m"
#define LCYAN "\33[22;36m"
#define MAGENTA "\33[22;35m"
#define LMAGENTA "\33[01;35m"
#define NC "\33[0m"
#define BOLD "\33[1m"
#define ULINE "\33[4m" //underline
#define BLINK "\33[5m"
#define INVERT "\33[7m"

VOID_TASK_1(sylvan_stats_sum, sylvan_stats_t*, target)
{
#ifdef __ELF__
    for (int i=0; i<SYLVAN_COUNTER_COUNTER; i++) {
        __sync_fetch_and_add(&target->counters[i], sylvan_stats.counters[i]);
    }
    for (int i=0; i<SYLVAN_TIMER_COUNTER; i++) {
        __sync_fetch_and_add(&target->timers[i], sylvan_stats.timers[i]);
    }
#else
    sylvan_stats_t *sylvan_stats = pthread_getspecific(sylvan_stats_key);
    if (sylvan_stats != NULL) {
        for (int i=0; i<SYLVAN_COUNTER_COUNTER; i++) {
            __sync_fetch_and_add(&target->counters[i], sylvan_stats->counters[i]);
        }
        for (int i=0; i<SYLVAN_TIMER_COUNTER; i++) {
            __sync_fetch_and_add(&target->timers[i], sylvan_stats->timers[i]);
        }
    }
#endif
}

void
sylvan_stats_report(FILE *target, int color)
{
#if !SYLVAN_STATS
    (void)target;
    (void)color;
    return;
#else
    (void)color;

    sylvan_stats_t totals;
    memset(&totals, 0, sizeof(sylvan_stats_t));

    LACE_ME;
    TOGETHER(sylvan_stats_sum, &totals);

    // fix timers for MACH
#ifdef __MACH__
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    uint64_t c = timebase.numer/timebase.denom;
    for (int i=0;i<SYLVAN_TIMER_COUNTER;i++) totals.timers[i]*=c;
#endif

    if (color) fprintf(target, LRED  "*** " BOLD "Sylvan stats" NC LRED " ***" NC);
    else fprintf(target, "*** Sylvan stats ***");

    if (totals.counters[BDD_NODES_CREATED]) {
        if (color) fprintf(target, ULINE LBLUE);
        fprintf(target, "\nBDD operations count (cache reuse, cache put)\n");
        if (color) fprintf(target, NC);
        if (totals.counters[BDD_ITE]) fprintf(target, "ITE: %'"PRIu64 " (%'"PRIu64", %'"PRIu64 ")\n", totals.counters[BDD_ITE], totals.counters[BDD_ITE_CACHED], totals.counters[BDD_ITE_CACHEDPUT]);
        if (totals.counters[BDD_AND]) fprintf(target, "AND: %'"PRIu64 " (%'"PRIu64", %'"PRIu64 ")\n", totals.counters[BDD_AND], totals.counters[BDD_AND_CACHED], totals.counters[BDD_AND_CACHEDPUT]);
        if (totals.counters[BDD_XOR]) fprintf(target, "XOR: %'"PRIu64 " (%'"PRIu64", %'"PRIu64 ")\n", totals.counters[BDD_XOR], totals.counters[BDD_XOR_CACHED], totals.counters[BDD_XOR_CACHEDPUT]);
        if (totals.counters[BDD_EXISTS]) fprintf(target, "Exists: %'"PRIu64 " (%'"PRIu64", %'"PRIu64 ")\n", totals.counters[BDD_EXISTS], totals.counters[BDD_EXISTS_CACHED], totals.counters[BDD_EXISTS_CACHEDPUT]);
        if (totals.counters[BDD_AND_EXISTS]) fprintf(target, "AndExists: %'"PRIu64 " (%'"PRIu64", %'"PRIu64 ")\n", totals.counters[BDD_AND_EXISTS], totals.counters[BDD_AND_EXISTS_CACHED], totals.counters[BDD_AND_EXISTS_CACHEDPUT]);
        if (totals.counters[BDD_RELNEXT]) fprintf(target, "RelNext: %'"PRIu64 " (%'"PRIu64", %'"PRIu64 ")\n", totals.counters[BDD_RELNEXT], totals.counters[BDD_RELNEXT_CACHED], totals.counters[BDD_RELNEXT_CACHEDPUT]);
        if (totals.counters[BDD_RELPREV]) fprintf(target, "RelPrev: %'"PRIu64 " (%'"PRIu64", %'"PRIu64 ")\n", totals.counters[BDD_RELPREV], totals.counters[BDD_RELPREV_CACHED], totals.counters[BDD_RELPREV_CACHEDPUT]);
        if (totals.counters[BDD_CLOSURE]) fprintf(target, "Closure: %'"PRIu64 " (%'"PRIu64", %'"PRIu64 ")\n", totals.counters[BDD_CLOSURE], totals.counters[BDD_CLOSURE_CACHED], totals.counters[BDD_CLOSURE_CACHEDPUT]);
        if (totals.counters[BDD_COMPOSE]) fprintf(target, "Compose: %'"PRIu64 " (%'"PRIu64", %'"PRIu64 ")\n", totals.counters[BDD_COMPOSE], totals.counters[BDD_COMPOSE_CACHED], totals.counters[BDD_COMPOSE_CACHEDPUT]);
        if (totals.counters[BDD_RESTRICT]) fprintf(target, "Restrict: %'"PRIu64 " (%'"PRIu64", %'"PRIu64 ")\n", totals.counters[BDD_RESTRICT], totals.counters[BDD_RESTRICT_CACHED], totals.counters[BDD_RESTRICT_CACHEDPUT]);
        if (totals.counters[BDD_CONSTRAIN]) fprintf(target, "Constrain: %'"PRIu64 " (%'"PRIu64", %'"PRIu64 ")\n", totals.counters[BDD_CONSTRAIN], totals.counters[BDD_CONSTRAIN_CACHED], totals.counters[BDD_CONSTRAIN_CACHEDPUT]);
        if (totals.counters[BDD_SUPPORT]) fprintf(target, "Support: %'"PRIu64 " (%'"PRIu64", %'"PRIu64 ")\n", totals.counters[BDD_SUPPORT], totals.counters[BDD_SUPPORT_CACHED], totals.counters[BDD_SUPPORT_CACHEDPUT]);
        if (totals.counters[BDD_SATCOUNT]) fprintf(target, "SatCount: %'"PRIu64 " (%'"PRIu64", %'"PRIu64 ")\n", totals.counters[BDD_SATCOUNT], totals.counters[BDD_SATCOUNT_CACHED], totals.counters[BDD_SATCOUNT_CACHEDPUT]);
        if (totals.counters[BDD_PATHCOUNT]) fprintf(target, "PathCount: %'"PRIu64 " (%'"PRIu64", %'"PRIu64 ")\n", totals.counters[BDD_PATHCOUNT], totals.counters[BDD_PATHCOUNT_CACHED], totals.counters[BDD_PATHCOUNT_CACHEDPUT]);
        if (totals.counters[BDD_ISBDD]) fprintf(target, "IsBDD: %'"PRIu64 " (%'"PRIu64", %'"PRIu64 ")\n", totals.counters[BDD_ISBDD], totals.counters[BDD_ISBDD_CACHED], totals.counters[BDD_ISBDD_CACHEDPUT]);
        fprintf(target, "BDD Nodes created: %'"PRIu64"\n", totals.counters[BDD_NODES_CREATED]);
        fprintf(target, "BDD Nodes reused: %'"PRIu64"\n", totals.counters[BDD_NODES_REUSED]);
    }

    if (totals.counters[LDD_NODES_CREATED]) {
        if (color) fprintf(target, ULINE LBLUE);
        fprintf(target, "\nLDD operations count (cache reuse, cache put)\n");
        if (color) fprintf(target, NC);
        if (totals.counters[LDD_UNION]) fprintf(target, "Union: %'"PRIu64 " (%'"PRIu64", %"PRIu64")\n", totals.counters[LDD_UNION], totals.counters[LDD_UNION_CACHED], totals.counters[LDD_UNION_CACHEDPUT]);
        if (totals.counters[LDD_MINUS]) fprintf(target, "Minus: %'"PRIu64 " (%'"PRIu64", %"PRIu64")\n", totals.counters[LDD_MINUS], totals.counters[LDD_MINUS_CACHED], totals.counters[LDD_MINUS_CACHEDPUT]);
        if (totals.counters[LDD_INTERSECT]) fprintf(target, "Intersect: %'"PRIu64 " (%'"PRIu64", %"PRIu64")\n", totals.counters[LDD_INTERSECT], totals.counters[LDD_INTERSECT_CACHED], totals.counters[LDD_INTERSECT_CACHEDPUT]);
        if (totals.counters[LDD_RELPROD]) fprintf(target, "RelProd: %'"PRIu64 " (%'"PRIu64", %"PRIu64")\n", totals.counters[LDD_RELPROD], totals.counters[LDD_RELPROD_CACHED], totals.counters[LDD_RELPROD_CACHEDPUT]);
        if (totals.counters[LDD_RELPREV]) fprintf(target, "RelPrev: %'"PRIu64 " (%'"PRIu64", %"PRIu64")\n", totals.counters[LDD_RELPREV], totals.counters[LDD_RELPREV_CACHED], totals.counters[LDD_RELPREV_CACHEDPUT]);
        if (totals.counters[LDD_PROJECT]) fprintf(target, "Project: %'"PRIu64 " (%'"PRIu64", %"PRIu64")\n", totals.counters[LDD_PROJECT], totals.counters[LDD_PROJECT_CACHED], totals.counters[LDD_PROJECT_CACHEDPUT]);
        if (totals.counters[LDD_JOIN]) fprintf(target, "Join: %'"PRIu64 " (%'"PRIu64", %"PRIu64")\n", totals.counters[LDD_JOIN], totals.counters[LDD_JOIN_CACHED], totals.counters[LDD_JOIN_CACHEDPUT]);
        if (totals.counters[LDD_MATCH]) fprintf(target, "Match: %'"PRIu64 " (%'"PRIu64", %"PRIu64")\n", totals.counters[LDD_MATCH], totals.counters[LDD_MATCH_CACHED], totals.counters[LDD_MATCH_CACHEDPUT]);
        if (totals.counters[LDD_SATCOUNT]) fprintf(target, "SatCount: %'"PRIu64 " (%'"PRIu64", %"PRIu64")\n", totals.counters[LDD_SATCOUNT], totals.counters[LDD_SATCOUNT_CACHED], totals.counters[LDD_SATCOUNT_CACHEDPUT]);
        if (totals.counters[LDD_SATCOUNTL]) fprintf(target, "SatCountL: %'"PRIu64 " (%'"PRIu64", %"PRIu64")\n", totals.counters[LDD_SATCOUNTL], totals.counters[LDD_SATCOUNTL_CACHED], totals.counters[LDD_SATCOUNTL_CACHEDPUT]);
        if (totals.counters[LDD_ZIP]) fprintf(target, "Zip: %'"PRIu64 " (%'"PRIu64", %"PRIu64")\n", totals.counters[LDD_ZIP], totals.counters[LDD_ZIP_CACHED], totals.counters[LDD_ZIP_CACHEDPUT]);
        if (totals.counters[LDD_RELPROD_UNION]) fprintf(target, "RelProdUnion: %'"PRIu64 " (%'"PRIu64", %"PRIu64")\n", totals.counters[LDD_RELPROD_UNION], totals.counters[LDD_RELPROD_UNION_CACHED], totals.counters[LDD_RELPROD_UNION_CACHEDPUT]);
        if (totals.counters[LDD_PROJECT_MINUS]) fprintf(target, "ProjectMinus: %'"PRIu64 " (%'"PRIu64", %"PRIu64")\n", totals.counters[LDD_PROJECT_MINUS], totals.counters[LDD_PROJECT_MINUS_CACHED], totals.counters[LDD_PROJECT_MINUS_CACHEDPUT]);
        fprintf(target, "LDD Nodes created: %'"PRIu64"\n", totals.counters[LDD_NODES_CREATED]);
        fprintf(target, "LDD Nodes reused: %'"PRIu64"\n", totals.counters[LDD_NODES_REUSED]);
    }

    if (color) fprintf(target, ULINE LBLUE);
    fprintf(target, "\nGarbage collection\n");
    if (color) fprintf(target, NC);
    fprintf(target, "Number of GC executions: %'"PRIu64"\n", totals.counters[SYLVAN_GC_COUNT]);
    fprintf(target, "Total time spent: %'.6Lf sec.\n", (long double)totals.timers[SYLVAN_GC]/1000000000);

    if (color) fprintf(target, ULINE LBLUE);
    fprintf(target, "\nTables\n");
    if (color) fprintf(target, NC);
    fprintf(target, "Unique nodes table: %'zu of %'zu buckets filled.\n", llmsset_count_marked(nodes), llmsset_get_size(nodes));
    fprintf(target, "Operation cache: %'zu of %'zu buckets filled.\n", cache_getused(), cache_getsize());

    if (color) fprintf(target, ULINE LBLUE);
    fprintf(target, "\nUnique table\n");
    if (color) fprintf(target, NC);
    fprintf(target, "Number of lookup iterations: %'"PRIu64"\n", totals.counters[LLMSSET_LOOKUP]);
#endif
}

#else

VOID_TASK_IMPL_0(sylvan_stats_init)
{
}

VOID_TASK_IMPL_0(sylvan_stats_reset)
{
}

void
sylvan_stats_report(FILE* target, int color)
{
    (void)target;
    (void)color;
}



#endif
