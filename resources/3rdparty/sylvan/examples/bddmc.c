#include <argp.h>
#include <inttypes.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <getrss.h>

#include <sylvan.h>
#include <sylvan_int.h>

/* Configuration (via argp) */
static int report_levels = 0; // report states at end of every level
static int report_table = 0; // report table size at end of every level
static int report_nodes = 0; // report number of nodes of BDDs
static int strategy = 2; // 0 = BFS, 1 = PAR, 2 = SAT, 3 = CHAINING
static int check_deadlocks = 0; // set to 1 to check for deadlocks on-the-fly (only bfs/par)
static int merge_relations = 0; // merge relations to 1 relation
static int print_transition_matrix = 0; // print transition relation matrix
static int workers = 0; // autodetect
static char* model_filename = NULL; // filename of model

/* argp configuration */
static struct argp_option options[] =
{
    {"workers", 'w', "<workers>", 0, "Number of workers (default=0: autodetect)", 0},
    {"strategy", 's', "<bfs|par|sat|chaining>", 0, "Strategy for reachability (default=sat)", 0},
    {"deadlocks", 3, 0, 0, "Check for deadlocks", 1},
    {"count-nodes", 5, 0, 0, "Report #nodes for BDDs", 1},
    {"count-states", 1, 0, 0, "Report #states at each level", 1},
    {"count-table", 2, 0, 0, "Report table usage at each level", 1},
    {"merge-relations", 6, 0, 0, "Merge transition relations into one transition relation", 1},
    {"print-matrix", 4, 0, 0, "Print transition matrix", 1},
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
        else if (strcmp(arg, "chaining")==0) strategy = 3;
        else argp_usage(state);
        break;
    case 4:
        print_transition_matrix = 1;
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
    case 5:
        report_nodes = 1;
        break;
    case 6:
        merge_relations = 1;
        break;
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

/**
 * Types (set and relation)
 */
typedef struct set
{
    BDD bdd;
    BDD variables; // all variables in the set (used by satcount)
} *set_t;

typedef struct relation
{
    BDD bdd;
    BDD variables; // all variables in the relation (used by relprod)
    int r_k, w_k, *r_proj, *w_proj;
} *rel_t;

static int vectorsize; // size of vector in integers
static int *statebits; // number of bits for each state integer
static int actionbits; // number of bits for action label
static int totalbits; // total number of bits
static int next_count; // number of partitions of the transition relation
static rel_t *next; // each partition of the transition relation

/**
 * Obtain current wallclock time
 */
static double
wctime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec + 1E-6 * tv.tv_usec);
}

static double t_start;
#define INFO(s, ...) fprintf(stdout, "[% 8.2f] " s, wctime()-t_start, ##__VA_ARGS__)
#define Abort(...) { fprintf(stderr, __VA_ARGS__); fprintf(stderr, "Abort at line %d!\n", __LINE__); exit(-1); }

static char*
to_h(double size, char *buf)
{
    const char* units[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    int i = 0;
    for (;size>1024;size/=1024) i++;
    sprintf(buf, "%.*f %s", i, size, units[i]);
    return buf;
}

static void
print_memory_usage(void)
{
    char buf[32];
    to_h(getCurrentRSS(), buf);
    INFO("Memory usage: %s\n", buf);
}

/**
 * Load a set from file
 * The expected binary format:
 * - int k : projection size, or -1 for full state
 * - int[k] proj : k integers specifying the variables of the projection
 * - MTBDD[1] BDD (mtbdd binary format)
 */
#define set_load(f) RUN(set_load, f)
TASK_1(set_t, set_load, FILE*, f)
{
    // allocate set
    set_t set = (set_t)malloc(sizeof(struct set));
    set->bdd = sylvan_false;
    set->variables = sylvan_true;
    sylvan_protect(&set->bdd);
    sylvan_protect(&set->variables);

    // read k
    int k;
    if (fread(&k, sizeof(int), 1, f) != 1) Abort("Invalid input file!\n");

    if (k == -1) {
        // create variables for a full state vector
        uint32_t vars[totalbits];
        for (int i=0; i<totalbits; i++) vars[i] = 2*i;
        set->variables = sylvan_set_fromarray(vars, totalbits);
    } else {
        // read proj
        int proj[k];
        if (fread(proj, sizeof(int), k, f) != (size_t)k) Abort("Invalid input file!\n");
        // create variables for a short/projected state vector
        uint32_t vars[totalbits];
        uint32_t cv = 0;
        int j = 0, n = 0;
        for (int i=0; i<vectorsize && j<k; i++) {
            if (i == proj[j]) {
                for (int x=0; x<statebits[i]; x++) vars[n++] = (cv += 2) - 2;
                j++;
            } else {
                cv += 2 * statebits[i];
            }
        }
        set->variables = sylvan_set_fromarray(vars, n);
    }

    // read bdd
    if (mtbdd_reader_frombinary(f, &set->bdd, 1) != 0) Abort("Invalid input file!\n");

    return set;
}

/**
 * Load a relation from file
 * This part just reads the r_k, w_k, r_proj and w_proj variables.
 */
#define rel_load_proj(f) RUN(rel_load_proj, f)
TASK_1(rel_t, rel_load_proj, FILE*, f)
{
    rel_t rel = (rel_t)malloc(sizeof(struct relation));
    int r_k, w_k;
    if (fread(&r_k, sizeof(int), 1, f) != 1) Abort("Invalid file format.");
    if (fread(&w_k, sizeof(int), 1, f) != 1) Abort("Invalid file format.");
    rel->r_k = r_k;
    rel->w_k = w_k;
    int *r_proj = (int*)malloc(sizeof(int[r_k]));
    int *w_proj = (int*)malloc(sizeof(int[w_k]));
    if (fread(r_proj, sizeof(int), r_k, f) != (size_t)r_k) Abort("Invalid file format.");
    if (fread(w_proj, sizeof(int), w_k, f) != (size_t)w_k) Abort("Invalid file format.");
    rel->r_proj = r_proj;
    rel->w_proj = w_proj;

    rel->bdd = sylvan_false;
    sylvan_protect(&rel->bdd);

    /* Compute a_proj the union of r_proj and w_proj, and a_k the length of a_proj */
    int a_proj[r_k+w_k];
    int r_i = 0, w_i = 0, a_i = 0;
    for (;r_i < r_k || w_i < w_k;) {
        if (r_i < r_k && w_i < w_k) {
            if (r_proj[r_i] < w_proj[w_i]) {
                a_proj[a_i++] = r_proj[r_i++];
            } else if (r_proj[r_i] > w_proj[w_i]) {
                a_proj[a_i++] = w_proj[w_i++];
            } else /* r_proj[r_i] == w_proj[w_i] */ {
                a_proj[a_i++] = w_proj[w_i++];
                r_i++;
            }
        } else if (r_i < r_k) {
            a_proj[a_i++] = r_proj[r_i++];
        } else if (w_i < w_k) {
            a_proj[a_i++] = w_proj[w_i++];
        }
    }
    const int a_k = a_i;

    /* Compute all_variables, which are all variables the transition relation is defined on */
    uint32_t all_vars[totalbits * 2];
    uint32_t curvar = 0; // start with variable 0
    int i=0, j=0, n=0;
    for (; i<vectorsize && j<a_k; i++) {
        if (i == a_proj[j]) {
            for (int k=0; k<statebits[i]; k++) {
                all_vars[n++] = curvar;
                all_vars[n++] = curvar + 1;
                curvar += 2;
            }
            j++;
        } else {
            curvar += 2 * statebits[i];
        }
    }
    rel->variables = sylvan_set_fromarray(all_vars, n);
    sylvan_protect(&rel->variables);

    return rel;
}

/**
 * Load a relation from file
 * This part just reads the bdd of the relation
 */
#define rel_load(rel, f) RUN(rel_load, rel, f)
VOID_TASK_2(rel_load, rel_t, rel, FILE*, f)
{
    if (mtbdd_reader_frombinary(f, &rel->bdd, 1) != 0) Abort("Invalid file format!\n");
}

/**
 * Print a single example of a set to stdout
 * Assumption: the example is a full vector and variables contains all state variables...
 */
#define print_example(example, variables) RUN(print_example, example, variables)
VOID_TASK_2(print_example, BDD, example, BDDSET, variables)
{
    uint8_t str[totalbits];

    if (example != sylvan_false) {
        sylvan_sat_one(example, variables, str);
        int x=0;
        printf("[");
        for (int i=0; i<vectorsize; i++) {
            uint32_t res = 0;
            for (int j=0; j<statebits[i]; j++) {
                if (str[x++] == 1) res++;
                res <<= 1;
            }
            if (i>0) printf(",");
            printf("%" PRIu32, res);
        }
        printf("]");
    }
}

/**
 * Implementation of (parallel) saturation
 * (assumes relations are ordered on first variable)
 */
TASK_2(BDD, go_sat, BDD, set, int, idx)
{
    /* Terminal cases */
    if (set == sylvan_false) return sylvan_false;
    if (idx == next_count) return set;

    /* Consult the cache */
    BDD result;
    const BDD _set = set;
    if (cache_get3(200LL<<40, _set, idx, 0, &result)) return result;
    mtbdd_refs_pushptr(&_set);

    /**
     * Possible improvement: cache more things (like intermediate results?)
     *   and chain-apply more of the current level before going deeper?
     */

    /* Check if the relation should be applied */
    const uint32_t var = sylvan_var(next[idx]->variables);
    if (set == sylvan_true || var <= sylvan_var(set)) {
        /* Count the number of relations starting here */
        int count = idx+1;
        while (count < next_count && var == sylvan_var(next[count]->variables)) count++;
        count -= idx;
        /*
         * Compute until fixpoint:
         * - SAT deeper
         * - chain-apply all current level once
         */
        BDD prev = sylvan_false;
        BDD step = sylvan_false;
        mtbdd_refs_pushptr(&set);
        mtbdd_refs_pushptr(&prev);
        mtbdd_refs_pushptr(&step);
        while (prev != set) {
            prev = set;
            // SAT deeper
            set = CALL(go_sat, set, idx+count);
            // chain-apply all current level once
            for (int i=0;i<count;i++) {
                step = sylvan_relnext(set, next[idx+i]->bdd, next[idx+i]->variables);
                set = sylvan_or(set, step);
                step = sylvan_false; // unset, for gc
            }
        }
        mtbdd_refs_popptr(3);
        result = set;
    } else {
        /* Recursive computation */
        mtbdd_refs_spawn(SPAWN(go_sat, sylvan_low(set), idx));
        BDD high = mtbdd_refs_push(CALL(go_sat, sylvan_high(set), idx));
        BDD low = mtbdd_refs_sync(SYNC(go_sat));
        mtbdd_refs_pop(1);
        result = sylvan_makenode(sylvan_var(set), low, high);
    }

    /* Store in cache */
    cache_put3(200LL<<40, _set, idx, 0, result);
    mtbdd_refs_popptr(1);
    return result;
}

/**
 * Wrapper for the Saturation strategy
 */
VOID_TASK_1(sat, set_t, set)
{
    set->bdd = CALL(go_sat, set->bdd, 0);
}

/**
 * Implement parallel strategy (that performs the relnext operations in parallel)
 * This function does one level...
 */
TASK_5(BDD, go_par, BDD, cur, BDD, visited, size_t, from, size_t, len, BDD*, deadlocks)
{
    if (len == 1) {
        // Calculate NEW successors (not in visited)
        BDD succ = sylvan_relnext(cur, next[from]->bdd, next[from]->variables);
        bdd_refs_push(succ);
        if (deadlocks) {
            // check which BDDs in deadlocks do not have a successor in this relation
            BDD anc = sylvan_relprev(next[from]->bdd, succ, next[from]->variables);
            bdd_refs_push(anc);
            *deadlocks = sylvan_diff(*deadlocks, anc);
            bdd_refs_pop(1);
        }
        BDD result = sylvan_diff(succ, visited);
        bdd_refs_pop(1);
        return result;
    } else {
        BDD deadlocks_left;
        BDD deadlocks_right;
        if (deadlocks) {
            deadlocks_left = *deadlocks;
            deadlocks_right = *deadlocks;
            sylvan_protect(&deadlocks_left);
            sylvan_protect(&deadlocks_right);
        }

        // Recursively calculate left+right
        bdd_refs_spawn(SPAWN(go_par, cur, visited, from, (len+1)/2, deadlocks ? &deadlocks_left: NULL));
        BDD right = bdd_refs_push(CALL(go_par, cur, visited, from+(len+1)/2, len/2, deadlocks ? &deadlocks_right : NULL));
        BDD left = bdd_refs_push(bdd_refs_sync(SYNC(go_par)));

        // Merge results of left+right
        BDD result = sylvan_or(left, right);
        bdd_refs_pop(2);

        if (deadlocks) {
            bdd_refs_push(result);
            *deadlocks = sylvan_and(deadlocks_left, deadlocks_right);
            sylvan_unprotect(&deadlocks_left);
            sylvan_unprotect(&deadlocks_right);
            bdd_refs_pop(1);
        }

        return result;
    }
}

/**
 * Implementation of the PAR strategy
 */
VOID_TASK_1(par, set_t, set)
{
    BDD visited = set->bdd;
    BDD next_level = visited;
    BDD cur_level = sylvan_false;
    BDD deadlocks = sylvan_false;

    sylvan_protect(&visited);
    sylvan_protect(&next_level);
    sylvan_protect(&cur_level);
    sylvan_protect(&deadlocks);

    int iteration = 1;
    do {
        // calculate successors in parallel
        cur_level = next_level;
        deadlocks = cur_level;

        next_level = CALL(go_par, cur_level, visited, 0, next_count, check_deadlocks ? &deadlocks : NULL);

        if (check_deadlocks && deadlocks != sylvan_false) {
            INFO("Found %'0.0f deadlock states... ", sylvan_satcount(deadlocks, set->variables));
            if (deadlocks != sylvan_false) {
                printf("example: ");
                print_example(deadlocks, set->variables);
                check_deadlocks = 0;
            }
            printf("\n");
        }

        // visited = visited + new
        visited = sylvan_or(visited, next_level);

        if (report_table && report_levels) {
            size_t filled, total;
            sylvan_table_usage(&filled, &total);
            INFO("Level %d done, %'0.0f states explored, table: %0.1f%% full (%'zu nodes)\n",
                iteration, sylvan_satcount(visited, set->variables),
                100.0*(double)filled/total, filled);
        } else if (report_table) {
            size_t filled, total;
            sylvan_table_usage(&filled, &total);
            INFO("Level %d done, table: %0.1f%% full (%'zu nodes)\n",
                iteration,
                100.0*(double)filled/total, filled);
        } else if (report_levels) {
            INFO("Level %d done, %'0.0f states explored\n", iteration, sylvan_satcount(visited, set->variables));
        } else {
            INFO("Level %d done\n", iteration);
        }
        iteration++;
    } while (next_level != sylvan_false);

    set->bdd = visited;

    sylvan_unprotect(&visited);
    sylvan_unprotect(&next_level);
    sylvan_unprotect(&cur_level);
    sylvan_unprotect(&deadlocks);
}

/**
 * Implement sequential strategy (that performs the relnext operations one by one)
 * This function does one level...
 */
TASK_5(BDD, go_bfs, BDD, cur, BDD, visited, size_t, from, size_t, len, BDD*, deadlocks)
{
    if (len == 1) {
        // Calculate NEW successors (not in visited)
        BDD succ = sylvan_relnext(cur, next[from]->bdd, next[from]->variables);
        bdd_refs_push(succ);
        if (deadlocks) {
            // check which BDDs in deadlocks do not have a successor in this relation
            BDD anc = sylvan_relprev(next[from]->bdd, succ, next[from]->variables);
            bdd_refs_push(anc);
            *deadlocks = sylvan_diff(*deadlocks, anc);
            bdd_refs_pop(1);
        }
        BDD result = sylvan_diff(succ, visited);
        bdd_refs_pop(1);
        return result;
    } else {
        BDD deadlocks_left;
        BDD deadlocks_right;
        if (deadlocks) {
            deadlocks_left = *deadlocks;
            deadlocks_right = *deadlocks;
            sylvan_protect(&deadlocks_left);
            sylvan_protect(&deadlocks_right);
        }

        // Recursively calculate left+right
        BDD left = CALL(go_bfs, cur, visited, from, (len+1)/2, deadlocks ? &deadlocks_left : NULL);
        bdd_refs_push(left);
        BDD right = CALL(go_bfs, cur, visited, from+(len+1)/2, len/2, deadlocks ? &deadlocks_right : NULL);
        bdd_refs_push(right);

        // Merge results of left+right
        BDD result = sylvan_or(left, right);
        bdd_refs_pop(2);

        if (deadlocks) {
            bdd_refs_push(result);
            *deadlocks = sylvan_and(deadlocks_left, deadlocks_right);
            sylvan_unprotect(&deadlocks_left);
            sylvan_unprotect(&deadlocks_right);
            bdd_refs_pop(1);
        }

        return result;
    }
}

/**
 * Implementation of the BFS strategy
 */
VOID_TASK_1(bfs, set_t, set)
{
    BDD visited = set->bdd;
    BDD next_level = visited;
    BDD cur_level = sylvan_false;
    BDD deadlocks = sylvan_false;

    sylvan_protect(&visited);
    sylvan_protect(&next_level);
    sylvan_protect(&cur_level);
    sylvan_protect(&deadlocks);

    int iteration = 1;
    do {
        // calculate successors in parallel
        cur_level = next_level;
        deadlocks = cur_level;

        next_level = CALL(go_bfs, cur_level, visited, 0, next_count, check_deadlocks ? &deadlocks : NULL);

        if (check_deadlocks && deadlocks != sylvan_false) {
            INFO("Found %'0.0f deadlock states... ", sylvan_satcount(deadlocks, set->variables));
            if (deadlocks != sylvan_false) {
                printf("example: ");
                print_example(deadlocks, set->variables);
                check_deadlocks = 0;
            }
            printf("\n");
        }

        // visited = visited + new
        visited = sylvan_or(visited, next_level);

        if (report_table && report_levels) {
            size_t filled, total;
            sylvan_table_usage(&filled, &total);
            INFO("Level %d done, %'0.0f states explored, table: %0.1f%% full (%'zu nodes)\n",
                iteration, sylvan_satcount(visited, set->variables),
                100.0*(double)filled/total, filled);
        } else if (report_table) {
            size_t filled, total;
            sylvan_table_usage(&filled, &total);
            INFO("Level %d done, table: %0.1f%% full (%'zu nodes)\n",
                iteration,
                100.0*(double)filled/total, filled);
        } else if (report_levels) {
            INFO("Level %d done, %'0.0f states explored\n", iteration, sylvan_satcount(visited, set->variables));
        } else {
            INFO("Level %d done\n", iteration);
        }
        iteration++;
    } while (next_level != sylvan_false);

    set->bdd = visited;

    sylvan_unprotect(&visited);
    sylvan_unprotect(&next_level);
    sylvan_unprotect(&cur_level);
    sylvan_unprotect(&deadlocks);
}

/**
 * Implementation of the Chaining strategy (does not support deadlock detection)
 */
VOID_TASK_1(chaining, set_t, set)
{
    BDD visited = set->bdd;
    BDD next_level = visited;
    BDD succ = sylvan_false;

    bdd_refs_pushptr(&visited);
    bdd_refs_pushptr(&next_level);
    bdd_refs_pushptr(&succ);

    int iteration = 1;
    do {
        // calculate successors in parallel
        for (int i=0; i<next_count; i++) {
            succ = sylvan_relnext(next_level, next[i]->bdd, next[i]->variables);
            next_level = sylvan_or(next_level, succ);
            succ = sylvan_false; // reset, for gc
        }

        // new = new - visited
        // visited = visited + new
        next_level = sylvan_diff(next_level, visited);
        visited = sylvan_or(visited, next_level);

        if (report_table && report_levels) {
            size_t filled, total;
            sylvan_table_usage(&filled, &total);
            INFO("Level %d done, %'0.0f states explored, table: %0.1f%% full (%'zu nodes)\n",
                iteration, sylvan_satcount(visited, set->variables),
                100.0*(double)filled/total, filled);
        } else if (report_table) {
            size_t filled, total;
            sylvan_table_usage(&filled, &total);
            INFO("Level %d done, table: %0.1f%% full (%'zu nodes)\n",
                iteration,
                100.0*(double)filled/total, filled);
        } else if (report_levels) {
            INFO("Level %d done, %'0.0f states explored\n", iteration, sylvan_satcount(visited, set->variables));
        } else {
            INFO("Level %d done\n", iteration);
        }
        iteration++;
    } while (next_level != sylvan_false);

    set->bdd = visited;
    bdd_refs_popptr(3);
}

/**
 * Extend a transition relation to a larger domain (using s=s')
 */
#define extend_relation(rel, vars) RUN(extend_relation, rel, vars)
TASK_2(BDD, extend_relation, MTBDD, relation, MTBDD, variables)
{
    /* first determine which state BDD variables are in rel */
    int has[totalbits];
    for (int i=0; i<totalbits; i++) has[i] = 0;
    MTBDD s = variables;
    while (!sylvan_set_isempty(s)) {
        uint32_t v = sylvan_set_first(s);
        if (v/2 >= (unsigned)totalbits) break; // action labels
        has[v/2] = 1;
        s = sylvan_set_next(s);
    }

    /* create "s=s'" for all variables not in rel */
    BDD eq = sylvan_true;
    for (int i=totalbits-1; i>=0; i--) {
        if (has[i]) continue;
        BDD low = sylvan_makenode(2*i+1, eq, sylvan_false);
        bdd_refs_push(low);
        BDD high = sylvan_makenode(2*i+1, sylvan_false, eq);
        bdd_refs_pop(1);
        eq = sylvan_makenode(2*i, low, high);
    }

    bdd_refs_push(eq);
    BDD result = sylvan_and(relation, eq);
    bdd_refs_pop(1);

    return result;
}

/**
 * Compute \BigUnion ( sets[i] )
 */
#define big_union(first, count) RUN(big_union, first, count)
TASK_2(BDD, big_union, int, first, int, count)
{
    if (count == 1) return next[first]->bdd;

    bdd_refs_spawn(SPAWN(big_union, first, count/2));
    BDD right = bdd_refs_push(CALL(big_union, first+count/2, count-count/2));
    BDD left = bdd_refs_push(bdd_refs_sync(SYNC(big_union)));
    BDD result = sylvan_or(left, right);
    bdd_refs_pop(2);
    return result;
}

/**
 * Print one row of the transition matrix (for vars)
 */
static void
print_matrix_row(rel_t rel)
{
    int r_i = 0, w_i = 0;
    for (int i=0; i<vectorsize; i++) {
        int s = 0;
        if (r_i < rel->r_k && rel->r_proj[r_i] == i) {
            s |= 1;
            r_i++;
        }
        if (w_i < rel->w_k && rel->w_proj[w_i] == i) {
            s |= 2;
            w_i++;
        }
        if (s == 0) fprintf(stdout, "-");
        else if (s == 1) fprintf(stdout, "r");
        else if (s == 2) fprintf(stdout, "w");
        else if (s == 3) fprintf(stdout, "+");
    }
}

VOID_TASK_0(gc_start)
{
    char buf[32];
    to_h(getCurrentRSS(), buf);
    INFO("(GC) Starting garbage collection... (rss: %s)\n", buf);
}

VOID_TASK_0(gc_end)
{
    char buf[32];
    to_h(getCurrentRSS(), buf);
    INFO("(GC) Garbage collection done.       (rss: %s)\n", buf);
}

void
print_h(double size)
{
    const char* units[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    int i = 0;
    for (;size>1024;size/=1024) i++;
    printf("%.*f %s", i, size, units[i]);
}

int
main(int argc, char **argv)
{
    /**
     * Parse command line, set locale, set startup time for INFO messages.
     */
    argp_parse(&argp, argc, argv, 0, 0, 0);
    setlocale(LC_NUMERIC, "en_US.utf-8");
    t_start = wctime();

    /**
     * Initialize Lace.
     *
     * First: setup with given number of workers (0 for autodetect) and some large size task queue.
     * Second: start all worker threads with default settings.
     * Third: setup local variables using the LACE_ME macro.
     */
    lace_start(workers, 1000000);

    /**
     * Initialize Sylvan.
     *
     * First: set memory limits
     * - 2 GB memory, nodes table twice as big as cache, initial size halved 6x
     *   (that means it takes 6 garbage collections to get to the maximum nodes&cache size)
     * Second: initialize package and subpackages
     * Third: add hooks to report garbage collection
     */
    size_t max = 16LL<<30;
    if (max > getMaxMemory()) max = getMaxMemory()/10*9;
    printf("Setting Sylvan main tables memory to ");
    print_h(max);
    printf(" max.\n");

    sylvan_set_limits(max, 1, 6);
    sylvan_init_package();
    sylvan_init_bdd();
    sylvan_gc_hook_pregc(TASK(gc_start));
    sylvan_gc_hook_postgc(TASK(gc_end));

    /**
     * Read the model from file
     */

    /* Open the file */
    FILE *f = fopen(model_filename, "r");
    if (f == NULL) Abort("Cannot open file '%s'!\n", model_filename);

    /* Read domain data */
    if (fread(&vectorsize, sizeof(int), 1, f) != 1) Abort("Invalid input file!\n");
    statebits = (int*)malloc(sizeof(int[vectorsize]));
    if (fread(statebits, sizeof(int), vectorsize, f) != (size_t)vectorsize) Abort("Invalid input file!\n");
    if (fread(&actionbits, sizeof(int), 1, f) != 1) Abort("Invalid input file!\n");
    totalbits = 0;
    for (int i=0; i<vectorsize; i++) totalbits += statebits[i];

    /* Read initial state */
    set_t states = set_load(f);

    /* Read number of transition relations */
    if (fread(&next_count, sizeof(int), 1, f) != 1) Abort("Invalid input file!\n");
    next = (rel_t*)malloc(sizeof(rel_t) * next_count);

    /* Read transition relations */
    for (int i=0; i<next_count; i++) next[i] = rel_load_proj(f);
    for (int i=0; i<next_count; i++) rel_load(next[i], f);

    /* We ignore the reachable states and action labels that are stored after the relations */

    /* Close the file */
    fclose(f);

    /**
     * Pre-processing and some statistics reporting
     */

    if (strategy == 2 || strategy == 3) {
        // for SAT and CHAINING, sort the transition relations (gnome sort because I like gnomes)
        int i = 1, j = 2;
        rel_t t;
        while (i < next_count) {
            rel_t *p = &next[i], *q = p-1;
            if (sylvan_var((*q)->variables) > sylvan_var((*p)->variables)) {
                t = *q;
                *q = *p;
                *p = t;
                if (--i) continue;
            }
            i = j++;
        }
    }

    INFO("Read file '%s'\n", model_filename);
    INFO("%d integers per state, %d bits per state, %d transition groups\n", vectorsize, totalbits, next_count);

    /* if requested, print the transition matrix */
    if (print_transition_matrix) {
        for (int i=0; i<next_count; i++) {
            INFO(""); // print time prefix
            print_matrix_row(next[i]); // print row
            fprintf(stdout, "\n"); // print newline
        }
    }

    /* merge all relations to one big transition relation if requested */
    if (merge_relations) {
        BDD newvars = sylvan_set_empty();
        bdd_refs_pushptr(&newvars);
        for (int i=totalbits-1; i>=0; i--) {
            newvars = sylvan_set_add(newvars, i*2+1);
            newvars = sylvan_set_add(newvars, i*2);
        }

        INFO("Extending transition relations to full domain.\n");
        for (int i=0; i<next_count; i++) {
            next[i]->bdd = extend_relation(next[i]->bdd, next[i]->variables);
            next[i]->variables = newvars;
        }

        bdd_refs_popptr(1);

        INFO("Taking union of all transition relations.\n");
        next[0]->bdd = big_union(0, next_count);

        for (int i=1; i<next_count; i++) {
            next[i]->bdd = sylvan_false;
            next[i]->variables = sylvan_true;
        }
        next_count = 1;
    }

    if (report_nodes) {
        INFO("BDD nodes:\n");
        INFO("Initial states: %zu BDD nodes\n", sylvan_nodecount(states->bdd));
        for (int i=0; i<next_count; i++) {
            INFO("Transition %d: %zu BDD nodes\n", i, sylvan_nodecount(next[i]->bdd));
        }
    }

    print_memory_usage();

    if (strategy == 0) {
        double t1 = wctime();
        RUN(bfs, states);
        double t2 = wctime();
        INFO("BFS Time: %f\n", t2-t1);
    } else if (strategy == 1) {
        double t1 = wctime();
        RUN(par, states);
        double t2 = wctime();
        INFO("PAR Time: %f\n", t2-t1);
    } else if (strategy == 2) {
        double t1 = wctime();
        RUN(sat, states);
        double t2 = wctime();
        INFO("SAT Time: %f\n", t2-t1);
    } else if (strategy == 3) {
        double t1 = wctime();
        RUN(chaining, states);
        double t2 = wctime();
        INFO("CHAINING Time: %f\n", t2-t1);
    } else {
        Abort("Invalid strategy set?!\n");
    }

    // Now we just have states
    INFO("Final states: %'0.0f states\n", sylvan_satcount(states->bdd, states->variables));
    if (report_nodes) {
        INFO("Final states: %'zu BDD nodes\n", sylvan_nodecount(states->bdd));
    }

    print_memory_usage();

    sylvan_stats_report(stdout);

    lace_stop();

    return 0;
}
