#include <argp.h>
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#ifdef HAVE_PROFILER
#include <gperftools/profiler.h>
#endif

#include <getrss.h>
#include <sylvan.h>
#include <llmsset.h>

/* Configuration */
static int report_levels = 0; // report states at start of every level
static int report_table = 0; // report table size at end of every level
static int strategy = 1; // set to 1 = use PAR strategy; set to 0 = use BFS strategy
static int check_deadlocks = 0; // set to 1 to check for deadlocks
static int print_transition_matrix = 1; // print transition relation matrix
static int workers = 0; // autodetect
static char* model_filename = NULL; // filename of model
#ifdef HAVE_PROFILER
static char* profile_filename = NULL; // filename for profiling
#endif

/* argp configuration */
static struct argp_option options[] =
{
    {"workers", 'w', "<workers>", 0, "Number of workers (default=0: autodetect)", 0},
    {"strategy", 's', "<bfs|par|sat>", 0, "Strategy for reachability (default=par)", 0},
#ifdef HAVE_PROFILER
    {"profiler", 'p', "<filename>", 0, "Filename for profiling", 0},
#endif
    {"deadlocks", 3, 0, 0, "Check for deadlocks", 1},
    {"count-states", 1, 0, 0, "Report #states at each level", 1},
    {"count-table", 2, 0, 0, "Report table usage at each level", 1},
    {0, 0, 0, 0, 0, 0}
};
static error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
    switch (key) {
    case 'w':
        workers = atoi(arg);
        break;
    case 's':
        if (strcmp(arg, "bfs")==0) strategy = 0;
        else if (strcmp(arg, "par")==0) strategy = 1;
        else if (strcmp(arg, "sat")==0) strategy = 2;
        else argp_usage(state);
        break;
    case 3:
        check_deadlocks = 1;
        break;
    case 1:
        report_levels = 1;
        break;
    case 2:
        report_table = 1;
        break;
#ifdef HAVE_PROFILER
    case 'p':
        profile_filename = arg;
        break;
#endif
    case ARGP_KEY_ARG:
        if (state->arg_num >= 1) argp_usage(state);
        model_filename = arg;
        break; 
    case ARGP_KEY_END:
        if (state->arg_num < 1) argp_usage(state);
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}
static struct argp argp = { options, parse_opt, "<model>", 0, 0, 0, 0 };

/* Globals */
typedef struct set
{
    MDD mdd;
    MDD proj;
    int size;
} *set_t;

typedef struct relation
{
    MDD mdd;
    MDD meta;
    int size;
} *rel_t;

static size_t vector_size; // size of vector
static int next_count; // number of partitions of the transition relation
static rel_t *next; // each partition of the transition relation

#define Abort(...) { fprintf(stderr, __VA_ARGS__); exit(-1); }

/* Load a set from file */
static set_t
set_load(FILE* f)
{
    lddmc_serialize_fromfile(f);

    size_t mdd;
    size_t proj;
    int size;

    if (fread(&mdd, sizeof(size_t), 1, f) != 1) Abort("Invalid input file!\n");
    if (fread(&proj, sizeof(size_t), 1, f) != 1) Abort("Invalid input file!\n");
    if (fread(&size, sizeof(int), 1, f) != 1) Abort("Invalid input file!\n");

    LACE_ME;

    set_t set = (set_t)malloc(sizeof(struct set));
    set->mdd = lddmc_ref(lddmc_serialize_get_reversed(mdd));
    set->proj = lddmc_ref(lddmc_serialize_get_reversed(proj));
    set->size = size;

    return set;
}

static int
calculate_size(MDD meta)
{
    int result = 0;
    uint32_t val = lddmc_getvalue(meta);
    while (val != (uint32_t)-1) {
        if (val != 0) result += 1;
        meta = lddmc_follow(meta, val);
        assert(meta != lddmc_true && meta != lddmc_false);
        val = lddmc_getvalue(meta);
    }
    return result;
}

/* Load a relation from file */
static rel_t
rel_load(FILE* f)
{
    lddmc_serialize_fromfile(f);

    size_t mdd;
    size_t meta;

    if (fread(&mdd, sizeof(size_t), 1, f) != 1) Abort("Invalid input file!\n");
    if (fread(&meta, sizeof(size_t), 1, f) != 1) Abort("Invalid input file!\n");

    LACE_ME;

    rel_t rel = (rel_t)malloc(sizeof(struct relation));
    rel->mdd = lddmc_ref(lddmc_serialize_get_reversed(mdd));
    rel->meta = lddmc_ref(lddmc_serialize_get_reversed(meta));
    rel->size = calculate_size(rel->meta);

    return rel;
}

static void
print_example(MDD example)
{
    if (example != lddmc_false) {
        LACE_ME;
        uint32_t vec[vector_size];
        lddmc_sat_one(example, vec, vector_size);

        size_t i;
        printf("[");
        for (i=0; i<vector_size; i++) {
            if (i>0) printf(",");
            printf("%" PRIu32, vec[i]);
        }
        printf("]");
    }
}

static void
print_matrix(size_t size, MDD meta)
{
    if (size == 0) return;
    uint32_t val = lddmc_getvalue(meta);
    if (val == 1) {
        printf("+");
        print_matrix(size-1, lddmc_follow(lddmc_follow(meta, 1), 2));
    } else {
        if (val == (uint32_t)-1) printf("-");
        else if (val == 0) printf("-");
        else if (val == 3) printf("r");
        else if (val == 4) printf("w");
        print_matrix(size-1, lddmc_follow(meta, val));
    }
}

static char*
to_h(double size, char *buf)
{
    const char* units[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    int i = 0;
    for (;size>1024;size/=1024) i++;
    sprintf(buf, "%.*f %s", i, size, units[i]);
    return buf;
}

static int
get_first(MDD meta)
{
    uint32_t val = lddmc_getvalue(meta);
    if (val != 0) return 0;
    return 1+get_first(lddmc_follow(meta, val));
}

/* Straight-forward implementation of parallel reduction */
TASK_5(MDD, go_par, MDD, cur, MDD, visited, size_t, from, size_t, len, MDD*, deadlocks)
{
    if (len == 1) {
        // Calculate NEW successors (not in visited)
        MDD succ = lddmc_ref(lddmc_relprod(cur, next[from]->mdd, next[from]->meta));
        if (deadlocks) {
            // check which MDDs in deadlocks do not have a successor in this relation
            MDD anc = lddmc_ref(lddmc_relprev(succ, next[from]->mdd, next[from]->meta, cur));
            *deadlocks = lddmc_ref(lddmc_minus(*deadlocks, anc));
            lddmc_deref(anc);
        }
        MDD result = lddmc_ref(lddmc_minus(succ, visited));
        lddmc_deref(succ);
        return result;
    } else {
        MDD deadlocks_left;
        MDD deadlocks_right;
        if (deadlocks) {
            deadlocks_left = *deadlocks;
            deadlocks_right = *deadlocks;
        }

        // Recursively calculate left+right
        SPAWN(go_par, cur, visited, from, (len+1)/2, deadlocks ? &deadlocks_left: NULL);
        MDD right = CALL(go_par, cur, visited, from+(len+1)/2, len/2, deadlocks ? &deadlocks_right : NULL);
        MDD left = SYNC(go_par);

        // Merge results of left+right
        MDD result = lddmc_ref(lddmc_union(left, right));
        lddmc_deref(left);
        lddmc_deref(right);

        if (deadlocks) {
            *deadlocks = lddmc_ref(lddmc_intersect(deadlocks_left, deadlocks_right));
            lddmc_deref(deadlocks_left);
            lddmc_deref(deadlocks_right);
        }

        return result;
    }
}

/* PAR strategy, parallel strategy (operations called in parallel *and* parallelized by Sylvan) */
VOID_TASK_1(par, set_t, set)
{
    MDD visited = set->mdd;
    MDD new = lddmc_ref(visited);
    size_t counter = 1;
    do {
        char buf[32];
        to_h(getCurrentRSS(), buf);
        printf("Memory usage: %s\n", buf);
        printf("Level %zu... ", counter++);
        if (report_levels) {
            printf("%zu states... ", (size_t)lddmc_satcount_cached(visited));
        }
        fflush(stdout);

        // calculate successors in parallel
        MDD cur = new;
        MDD deadlocks = cur;
        new = CALL(go_par, cur, visited, 0, next_count, check_deadlocks ? &deadlocks : NULL);
        lddmc_deref(cur);

        if (check_deadlocks) {
            printf("found %zu deadlock states... ", (size_t)lddmc_satcount_cached(deadlocks));
            if (deadlocks != lddmc_false) {
                printf("example: ");
                print_example(deadlocks);
                printf("... ");
                check_deadlocks = 0;
            }
        }

        // visited = visited + new
        MDD old_visited = visited;
        visited = lddmc_ref(lddmc_union(visited, new));
        lddmc_deref(old_visited);

        if (report_table) {
            size_t filled, total;
            sylvan_table_usage(&filled, &total);
            printf("done, table: %0.1f%% full (%zu nodes).\n", 100.0*(double)filled/total, filled);
        } else {
            printf("done.\n");
        }
    } while (new != lddmc_false);
    lddmc_deref(new);
    set->mdd = visited;
}

/* Sequential version of merge-reduction */
TASK_5(MDD, go_bfs, MDD, cur, MDD, visited, size_t, from, size_t, len, MDD*, deadlocks)
{
    if (len == 1) {
        // Calculate NEW successors (not in visited)
        MDD succ = lddmc_ref(lddmc_relprod(cur, next[from]->mdd, next[from]->meta));
        if (deadlocks) {
            // check which MDDs in deadlocks do not have a successor in this relation
            MDD anc = lddmc_ref(lddmc_relprev(succ, next[from]->mdd, next[from]->meta, cur));
            *deadlocks = lddmc_ref(lddmc_minus(*deadlocks, anc));
            lddmc_deref(anc);
        }
        MDD result = lddmc_ref(lddmc_minus(succ, visited));
        lddmc_deref(succ);
        return result;
    } else {
        MDD deadlocks_left;
        MDD deadlocks_right;
        if (deadlocks) {
            deadlocks_left = *deadlocks;
            deadlocks_right = *deadlocks;
        }

        // Recursively calculate left+right
        MDD left = CALL(go_bfs, cur, visited, from, (len+1)/2, deadlocks ? &deadlocks_left : NULL);
        MDD right = CALL(go_bfs, cur, visited, from+(len+1)/2, len/2, deadlocks ? &deadlocks_right : NULL);

        // Merge results of left+right
        MDD result = lddmc_ref(lddmc_union(left, right));
        lddmc_deref(left);
        lddmc_deref(right);

        if (deadlocks) {
            *deadlocks = lddmc_ref(lddmc_intersect(deadlocks_left, deadlocks_right));
            lddmc_deref(deadlocks_left);
            lddmc_deref(deadlocks_right);
        }

        return result;
    }
}

/* BFS strategy, sequential strategy (but operations are parallelized by Sylvan) */
VOID_TASK_1(bfs, set_t, set)
{
    MDD visited = set->mdd;
    MDD new = lddmc_ref(visited);
    size_t counter = 1;
    do {
        char buf[32];
        to_h(getCurrentRSS(), buf);
        printf("Memory usage: %s\n", buf);
        printf("Level %zu... ", counter++);
        if (report_levels) {
            printf("%zu states... ", (size_t)lddmc_satcount_cached(visited));
        }
        fflush(stdout);

        MDD cur = new;
        MDD deadlocks = cur;
        new = CALL(go_bfs, cur, visited, 0, next_count, check_deadlocks ? &deadlocks : NULL);
        lddmc_deref(cur);

        if (check_deadlocks) {
            printf("found %zu deadlock states... ", (size_t)lddmc_satcount_cached(deadlocks));
            if (deadlocks != lddmc_false) {
                printf("example: ");
                print_example(deadlocks);
                printf("... ");
                check_deadlocks = 0;
            }
        }

        // visited = visited + new
        MDD old_visited = visited;
        visited = lddmc_ref(lddmc_union(visited, new));
        lddmc_deref(old_visited);

        if (report_table) {
            size_t filled, total;
            sylvan_table_usage(&filled, &total);
            printf("done, table: %0.1f%% full (%zu nodes).\n", 100.0*(double)filled/total, filled);
        } else {
            printf("done.\n");
        }
    } while (new != lddmc_false);
    lddmc_deref(new);
    set->mdd = visited;
}

/* Obtain current wallclock time */
static double
wctime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec + 1E-6 * tv.tv_usec);
}

int
main(int argc, char **argv)
{
    argp_parse(&argp, argc, argv, 0, 0, 0);

    FILE *f = fopen(model_filename, "r");
    if (f == NULL) {
        fprintf(stderr, "Cannot open file '%s'!\n", model_filename);
        return -1;
    }

    // Init Lace
    lace_init(workers, 1000000); // auto-detect number of workers, use a 1,000,000 size task queue
    lace_startup(0, NULL, NULL); // auto-detect program stack, do not use a callback for startup

    // Init Sylvan LDDmc
    // Nodes table size: 24 bytes * 2**N_nodes
    // Cache table size: 36 bytes * 2**N_cache
    // With: N_nodes=25, N_cache=24: 1.3 GB memory
    sylvan_init_package(1LL<<21, 1LL<<27, 1LL<<20, 1LL<<26);
    sylvan_init_ldd();

    // Read and report domain info (integers per vector and bits per integer)
    if (fread(&vector_size, sizeof(size_t), 1, f) != 1) Abort("Invalid input file!\n");

    printf("Vector size: %zu\n", vector_size);

    // Read initial state
    printf("Loading initial state... ");
    fflush(stdout);
    set_t states = set_load(f);
    printf("done.\n");

    // Read transitions
    if (fread(&next_count, sizeof(int), 1, f) != 1) Abort("Invalid input file!\n");
    next = (rel_t*)malloc(sizeof(rel_t) * next_count);

    printf("Loading transition relations... ");
    fflush(stdout);
    int i;
    for (i=0; i<next_count; i++) {
        next[i] = rel_load(f);
        printf("%d, ", i);
        fflush(stdout);
    }
    fclose(f);
    printf("done.\n");

    // Report statistics
    printf("Read file '%s'\n", argv[1]);
    printf("%zu integers per state, %d transition groups\n", vector_size, next_count);
    printf("MDD nodes:\n");
    printf("Initial states: %zu MDD nodes\n", lddmc_nodecount(states->mdd));
    for (i=0; i<next_count; i++) {
        printf("Transition %d: %zu MDD nodes\n", i, lddmc_nodecount(next[i]->mdd));
    }

    if (print_transition_matrix) {
        for (i=0; i<next_count; i++) {
            print_matrix(vector_size, next[i]->meta);
            printf(" (%d)\n", get_first(next[i]->meta));
        }
    }

    LACE_ME;

#ifdef HAVE_PROFILER
    if (profile_filename != NULL) ProfilerStart(profile_filename);
#endif
    if (strategy == 1) {
        double t1 = wctime();
        CALL(par, states);
        double t2 = wctime();
        printf("PAR Time: %f\n", t2-t1);
    } else {
        double t1 = wctime();
        CALL(bfs, states);
        double t2 = wctime();
        printf("BFS Time: %f\n", t2-t1);
    }
#ifdef HAVE_PROFILER
    if (profile_filename != NULL) ProfilerStop();
#endif

    // Now we just have states
    printf("Final states: %zu states\n", (size_t)lddmc_satcount_cached(states->mdd));
    printf("Final states: %zu MDD nodes\n", lddmc_nodecount(states->mdd));

    sylvan_stats_report(stdout, 1);

    return 0;
}
