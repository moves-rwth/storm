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

#include <sylvan_int.h>

#include <errno.h>  // for errno
#include <string.h> // memset
#include <sys/mman.h>
#include <inttypes.h>

#if SYLVAN_STATS

#ifdef __ELF__
__thread sylvan_stats_t sylvan_stats;
#else
pthread_key_t sylvan_stats_key;
#endif

/**
 * Instructions for sylvan_stats_report
 */
struct
{
    int type; /* 0 for print line, 1 for simple counter, 2 for operation with CACHED and CACHEDPUT */
              /* 3 for timer, 4 for report table data */
    int id;
    const char *key;
} sylvan_report_info[] =
{
    {0, 0, "Tables"},
    {1, BDD_NODES_CREATED, "MTBDD nodes created"},
    {1, BDD_NODES_REUSED, "MTBDD nodes reused"},
    {1, LDD_NODES_CREATED, "LDD nodes created"},
    {1, LDD_NODES_REUSED, "LDD nodes reused"},
    {1, LLMSSET_LOOKUP, "Lookup iterations"},
    {4, 0, NULL}, /* trigger to report unique nodes and operation cache */

    {0, 0, "Operation            Count            Cache get        Cache put"},
    {2, BDD_AND, "BDD and"},
    {2, BDD_XOR, "BDD xor"},
    {2, BDD_ITE, "BDD ite"},
    {2, BDD_EXISTS, "BDD exists"},
    {2, BDD_PROJECT, "BDD project"},
    {2, BDD_AND_EXISTS, "BDD andexists"},
    {2, BDD_AND_PROJECT, "BDD andproject"},
    {2, BDD_RELNEXT, "BDD relnext"},
    {2, BDD_RELPREV, "BDD relprev"},
    {2, BDD_CLOSURE, "BDD closure"},
    {2, BDD_COMPOSE, "BDD compose"},
    {2, BDD_RESTRICT, "BDD restrict"},
    {2, BDD_CONSTRAIN, "BDD constrain"},
    {2, BDD_SUPPORT, "BDD support"},
    {2, BDD_SATCOUNT, "BDD satcount"},
    {2, BDD_PATHCOUNT, "BDD pathcount"},
    {2, BDD_ISBDD, "BDD isbdd"},

    {2, MTBDD_APPLY, "MTBDD binary apply"},
    {2, MTBDD_UAPPLY, "MTBDD unary apply"},
    {2, MTBDD_ABSTRACT, "MTBDD abstract"},
    {2, MTBDD_ITE, "MTBDD ite"},
    {2, MTBDD_EQUAL_NORM, "MTBDD eq norm"},
    {2, MTBDD_EQUAL_NORM_REL, "MTBDD eq norm rel"},
    {2, MTBDD_LEQ, "MTBDD leq"},
    {2, MTBDD_LESS, "MTBDD less"},
    {2, MTBDD_GEQ, "MTBDD geq"},
    {2, MTBDD_GREATER, "MTBDD greater"},
    {2, MTBDD_AND_ABSTRACT_PLUS, "MTBDD and_abs_plus"},
    {2, MTBDD_AND_ABSTRACT_MAX, "MTBDD and_abs_max"},
    {2, MTBDD_COMPOSE, "MTBDD compose"},
    {2, MTBDD_MINIMUM, "MTBDD minimum"},
    {2, MTBDD_MAXIMUM, "MTBDD maximum"},
    {2, MTBDD_EVAL_COMPOSE, "MTBDD eval_compose"},

    {2, LDD_UNION, "LDD union"},
    {2, LDD_MINUS, "LDD minus"},
    {2, LDD_INTERSECT, "LDD intersect"},
    {2, LDD_RELPROD, "LDD relprod"},
    {2, LDD_RELPREV, "LDD relprev"},
    {2, LDD_PROJECT, "LDD project"},
    {2, LDD_JOIN, "LDD join"},
    {2, LDD_MATCH, "LDD match"},
    {2, LDD_SATCOUNT, "LDD satcount"},
    {2, LDD_SATCOUNTL, "LDD satcountl"},
    {2, LDD_ZIP, "LDD zip"},
    {2, LDD_RELPROD_UNION, "LDD relprod_union"},
    {2, LDD_PROJECT_MINUS, "LDD project_minus"},

    {0, 0, "Garbage collection"},
    {1, SYLVAN_GC_COUNT, "GC executions"},
    {3, SYLVAN_GC, "Total time spent"},

    {-1, -1, NULL},
};

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
    }
    pthread_setspecific(sylvan_stats_key, sylvan_stats);
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
    TOGETHER(sylvan_stats_reset_perthread);
}

/**
 * Reset all counters (for statistics)
 */
VOID_TASK_IMPL_0(sylvan_stats_reset)
{
    TOGETHER(sylvan_stats_reset_perthread);
}

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

VOID_TASK_IMPL_1(sylvan_stats_snapshot, sylvan_stats_t*, target)
{
    memset(target, 0, sizeof(sylvan_stats_t));
    TOGETHER(sylvan_stats_sum, target);
}

#define BLACK "\33[22;30m"
#define GRAY "\33[1;30m"
#define RED "\33[22;31m"
#define LRED "\33[1;31m"
#define GREEN "\33[22;32m"
#define LGREEN "\33[1;32m"
#define BROWN "\33[22;33m"
#define YELLOW "\33[1;33m"
#define BLUE "\33[22;34m"
#define LBLUE "\33[1;34m"
#define MAGENTA "\33[22;35m"
#define LMAGENTA "\33[1;35m"
#define CYAN "\33[22;36m"
#define LCYAN "\33[1;36m"
#define LGRAY "\33[22;37m"
#define WHITE "\33[1;37m"
#define NC "\33[m"
#define BOLD "\33[1m"
#define ULINE "\33[4m"
#define PINK "\33[38;5;200m"

static char*
to_h(double size, char *buf)
{
    const char* units[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    int i = 0;
    for (;size>1024;size/=1024) i++;
    sprintf(buf, "%.*f %s", i, size, units[i]);
    return buf;
}

void
sylvan_stats_report(FILE *target)
{
    LACE_ME;
    sylvan_stats_t totals;
    sylvan_stats_snapshot(&totals);

    // fix timers for MACH
#ifdef __MACH__
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    uint64_t c = timebase.numer/timebase.denom;
    for (int i=0;i<SYLVAN_TIMER_COUNTER;i++) totals.timers[i]*=c;
#endif

    int color = isatty(fileno(target)) ? 1 : 0;
    if (color) fprintf(target, ULINE WHITE "Sylvan statistics\n" NC);
    else fprintf(target, "Sylvan statistics\n");

    int i=0;
    for (;;) {
        if (sylvan_report_info[i].id == -1) break;
        int id = sylvan_report_info[i].id;
        int type = sylvan_report_info[i].type;
        if (type == 0) {
            if (color) fprintf(target, WHITE "\n%s\n" NC, sylvan_report_info[i].key);
            else fprintf(target, "\n%s\n", sylvan_report_info[i].key);
        } else if (type == 1) {
            if (totals.counters[id] > 0) {
                fprintf(target, "%-20s %'-16"PRIu64"\n", sylvan_report_info[i].key, totals.counters[id]);
            }
        } else if (type == 2) {
            if (totals.counters[id] > 0) {
                fprintf(target, "%-20s %'-16"PRIu64 " %'-16"PRIu64" %'-16"PRIu64 "\n", sylvan_report_info[i].key, totals.counters[id], totals.counters[id+1], totals.counters[id+2]);
            }
        } else if (type == 3) {
            if (totals.timers[id] > 0) {
                fprintf(target, "%-20s %'.6Lf sec.\n", sylvan_report_info[i].key, (long double)totals.timers[id]/1000000000);
            }
        } else if (type == 4) {
            fprintf(target, "%-20s %'zu of %'zu buckets filled.\n", "Unique nodes table", llmsset_count_marked(nodes), llmsset_get_size(nodes));
            fprintf(target, "%-20s %'zu of %'zu buckets filled.\n", "Operation cache", cache_getused(), cache_getsize());
            char buf[64], buf2[64];
            to_h(24ULL * llmsset_get_size(nodes), buf);
            to_h(24ULL * llmsset_get_max_size(nodes), buf2);
            fprintf(target, "%-20s %s (max real) of %s (allocated virtual memory).\n", "Memory (nodes)", buf, buf2);
            to_h(36ULL * cache_getsize(), buf);
            to_h(36ULL * cache_getmaxsize(), buf2);
            fprintf(target, "%-20s %s (max real) of %s (allocated virtual memory).\n", "Memory (cache)", buf, buf2);
        }
        i++;
    }
}

#else

VOID_TASK_IMPL_0(sylvan_stats_init)
{
}

VOID_TASK_IMPL_0(sylvan_stats_reset)
{
}

VOID_TASK_IMPL_1(sylvan_stats_snapshot, sylvan_stats_t*, target)
{
    memset(target, 0, sizeof(sylvan_stats_t));
}

void
sylvan_stats_report(FILE* target)
{
    (void)target;
}

#endif
