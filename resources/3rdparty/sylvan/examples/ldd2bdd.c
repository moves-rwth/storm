#include <argp.h>
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <sylvan_int.h>

/* Configuration */
static int workers = 0; // autodetect
static int verbose = 0;
static char* model_filename = NULL; // filename of model
static char* bdd_filename = NULL; // filename of output BDD
static char* sizes = "22,27,21,26"; // default sizes
static int check_results = 0;

/* argp configuration */
static struct argp_option options[] =
{
    {"workers", 'w', "<workers>", 0, "Number of workers (default=0: autodetect)", 0},
    {"table-sizes", 1, "<tablesize>,<tablemax>,<cachesize>,<cachemax>", 0, "Sizes of nodes table and operation cache as powers of 2", 0},
    {"check-results", 2, 0, 0, "Check new transition relations ", 0},
    {"verbose", 'v', 0, 0, "Set verbose", 0},
    {0, 0, 0, 0, 0, 0}
};

static error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
    switch (key) {
    case 'w':
        workers = atoi(arg);
        break;
    case 'v':
        verbose = 1;
        break;
    case 1:
        sizes = arg;
        break;
    case 2:
        check_results = 1;
        break;
    case ARGP_KEY_ARG:
        if (state->arg_num == 0) model_filename = arg;
        if (state->arg_num == 1) bdd_filename = arg;
        if (state->arg_num >= 2) argp_usage(state);
        break; 
    case ARGP_KEY_END:
        if (state->arg_num < 2) argp_usage(state);
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, "<model> [<output-bdd>]", 0, 0, 0, 0 };

/* Globals */
typedef struct set
{
    MDD mdd;
    MDD proj;
} *set_t;

typedef struct relation
{
    MDD mdd;
    MDD meta;
} *rel_t;

static size_t vector_size; // size of vector
static int next_count; // number of partitions of the transition relation
static rel_t *next; // each partition of the transition relation
static int actionbits = 0;
static int has_actions = 0;

#define Abort(...) { fprintf(stderr, __VA_ARGS__); exit(-1); }

/* Load a set from file */
#define set_load(f) CALL(set_load, f)
TASK_1(set_t, set_load, FILE*, f)
{
    lddmc_serialize_fromfile(f);

    size_t mdd;
    size_t proj;
    int size;

    if (fread(&mdd, sizeof(size_t), 1, f) != 1) Abort("Invalid input file!\n");
    if (fread(&proj, sizeof(size_t), 1, f) != 1) Abort("Invalid input file!\n");
    if (fread(&size, sizeof(int), 1, f) != 1) Abort("Invalid input file!\n");

    set_t set = (set_t)malloc(sizeof(struct set));
    set->mdd = lddmc_ref(lddmc_serialize_get_reversed(mdd));
    set->proj = lddmc_ref(lddmc_serialize_get_reversed(proj));

    return set;
}

/* Load a relation from file */
#define rel_load(f) CALL(rel_load, f)
TASK_1(rel_t, rel_load, FILE*, f)
{
    lddmc_serialize_fromfile(f);

    size_t mdd;
    size_t meta;

    if (fread(&mdd, sizeof(size_t), 1, f) != 1) Abort("Invalid input file!\n");
    if (fread(&meta, sizeof(size_t), 1, f) != 1) Abort("Invalid input file!\n");

    rel_t rel = (rel_t)malloc(sizeof(struct relation));
    rel->mdd = lddmc_ref(lddmc_serialize_get_reversed(mdd));
    rel->meta = lddmc_ref(lddmc_serialize_get_reversed(meta));

    return rel;
}

/**
 * Compute the highest value for each variable level.
 * This method is called for the set of reachable states.
 */
static uint64_t compute_highest_id;
#define compute_highest(dd, arr) CALL(compute_highest, dd, arr)
VOID_TASK_2(compute_highest, MDD, dd, uint32_t*, arr)
{
    if (dd == lddmc_true || dd == lddmc_false) return;

    uint64_t result = 1;
    if (cache_get3(compute_highest_id, dd, 0, 0, &result)) return;
    cache_put3(compute_highest_id, dd, 0, 0, result);

    mddnode_t n = LDD_GETNODE(dd);

    SPAWN(compute_highest, mddnode_getright(n), arr);
    CALL(compute_highest, mddnode_getdown(n), arr+1);
    SYNC(compute_highest);

    if (!mddnode_getcopy(n)) {
        const uint32_t v = mddnode_getvalue(n);
        while (1) {
            const uint32_t cur = *(volatile uint32_t*)arr;
            if (v <= cur) break;
            if (__sync_bool_compare_and_swap(arr, cur, v)) break;
        }
    }
}

/**
 * Compute the highest value for the action label.
 * This method is called for each transition relation.
 */
static uint64_t compute_highest_action_id;
#define compute_highest_action(dd, meta, arr) CALL(compute_highest_action, dd, meta, arr)
VOID_TASK_3(compute_highest_action, MDD, dd, MDD, meta, uint32_t*, target)
{
    if (dd == lddmc_true || dd == lddmc_false) return;
    if (meta == lddmc_true) return;

    uint64_t result = 1;
    if (cache_get3(compute_highest_action_id, dd, meta, 0, &result)) return;
    cache_put3(compute_highest_action_id, dd, meta, 0, result);

    /* meta:
     *  0 is skip
     *  1 is read
     *  2 is write
     *  3 is only-read
     *  4 is only-write
     *  5 is action label (at end, before -1)
     * -1 is end
     */

    const mddnode_t n = LDD_GETNODE(dd);
    const mddnode_t nmeta = LDD_GETNODE(meta);
    const uint32_t vmeta = mddnode_getvalue(nmeta);
    if (vmeta == (uint32_t)-1) return;

    SPAWN(compute_highest_action, mddnode_getright(n), meta, target);
    CALL(compute_highest_action, mddnode_getdown(n), mddnode_getdown(nmeta), target);
    SYNC(compute_highest_action);

    if (vmeta == 5) {
        has_actions = 1;
        const uint32_t v = mddnode_getvalue(n);
        while (1) {
            const uint32_t cur = *(volatile uint32_t*)target;
            if (v <= cur) break;
            if (__sync_bool_compare_and_swap(target, cur, v)) break;
        }
    }
}

/**
 * Compute the BDD equivalent of the LDD of a set of states.
 */
static uint64_t bdd_from_ldd_id;
#define bdd_from_ldd(dd, bits, firstvar) CALL(bdd_from_ldd, dd, bits, firstvar)
TASK_3(MTBDD, bdd_from_ldd, MDD, dd, MDD, bits_mdd, uint32_t, firstvar)
{
    /* simple for leaves */
    if (dd == lddmc_false) return mtbdd_false;
    if (dd == lddmc_true) return mtbdd_true;

    MTBDD result;
    /* get from cache */
    /* note: some assumptions about the encoding... */
    if (cache_get3(bdd_from_ldd_id, dd, bits_mdd, firstvar, &result)) return result;

    mddnode_t n = LDD_GETNODE(dd);
    mddnode_t nbits = LDD_GETNODE(bits_mdd);
    int bits = (int)mddnode_getvalue(nbits);

    /* spawn right, same bits_mdd and firstvar */
    mtbdd_refs_spawn(SPAWN(bdd_from_ldd, mddnode_getright(n), bits_mdd, firstvar));

    /* call down, with next bits_mdd and firstvar */
    MTBDD down = CALL(bdd_from_ldd, mddnode_getdown(n), mddnode_getdown(nbits), firstvar + 2*bits);

    /* encode current value */
    uint32_t val = mddnode_getvalue(n);
    for (int i=0; i<bits; i++) {
        /* encode with high bit first */
        int bit = bits-i-1;
        if (val & (1LL<<i)) down = mtbdd_makenode(firstvar + 2*bit, mtbdd_false, down);
        else down = mtbdd_makenode(firstvar + 2*bit, down, mtbdd_false);
    }

    /* sync right */
    mtbdd_refs_push(down);
    MTBDD right = mtbdd_refs_sync(SYNC(bdd_from_ldd));

    /* take union of current and right */
    mtbdd_refs_push(right);
    result = sylvan_or(down, right);
    mtbdd_refs_pop(2);

    /* put in cache */
    cache_put3(bdd_from_ldd_id, dd, bits_mdd, firstvar, result);

    return result;
}

/**
 * Compute the BDD equivalent of an LDD transition relation.
 */
static uint64_t bdd_from_ldd_rel_id;
#define bdd_from_ldd_rel(dd, bits, firstvar, meta) CALL(bdd_from_ldd_rel, dd, bits, firstvar, meta)
TASK_4(MTBDD, bdd_from_ldd_rel, MDD, dd, MDD, bits_mdd, uint32_t, firstvar, MDD, meta)
{
    if (dd == lddmc_false) return mtbdd_false;
    if (dd == lddmc_true) return mtbdd_true;
    assert(meta != lddmc_false && meta != lddmc_true);

    /* meta:
     * -1 is end
     *  0 is skip
     *  1 is read
     *  2 is write
     *  3 is only-read
     *  4 is only-write
     */

    MTBDD result;
    /* note: assumptions */
    if (cache_get4(bdd_from_ldd_rel_id, dd, bits_mdd, firstvar, meta, &result)) return result;

    const mddnode_t n = LDD_GETNODE(dd);
    const mddnode_t nmeta = LDD_GETNODE(meta);
    const mddnode_t nbits = LDD_GETNODE(bits_mdd);
    const int bits = (int)mddnode_getvalue(nbits);

    const uint32_t vmeta = mddnode_getvalue(nmeta);
    assert(vmeta != (uint32_t)-1);

    if (vmeta == 0) {
        /* skip level */
        result = bdd_from_ldd_rel(dd, mddnode_getdown(nbits), firstvar + 2*bits, mddnode_getdown(nmeta));
    } else if (vmeta == 1) {
        /* read level */
        assert(!mddnode_getcopy(n));  // do not process read copy nodes for now
        assert(mddnode_getright(n) != mtbdd_true);

        /* spawn right */
        mtbdd_refs_spawn(SPAWN(bdd_from_ldd_rel, mddnode_getright(n), bits_mdd, firstvar, meta));

        /* compute down with same bits / firstvar */
        MTBDD down = bdd_from_ldd_rel(mddnode_getdown(n), bits_mdd, firstvar, mddnode_getdown(nmeta));
        mtbdd_refs_push(down);

        /* encode read value */
        uint32_t val = mddnode_getvalue(n);
        MTBDD part = mtbdd_true;
        for (int i=0; i<bits; i++) {
            /* encode with high bit first */
            int bit = bits-i-1;
            if (val & (1LL<<i)) part = mtbdd_makenode(firstvar + 2*bit, mtbdd_false, part);
            else part = mtbdd_makenode(firstvar + 2*bit, part, mtbdd_false);
        }

        /* intersect read value with down result */
        mtbdd_refs_push(part);
        down = sylvan_and(part, down);
        mtbdd_refs_pop(2);

        /* sync right */
        mtbdd_refs_push(down);
        MTBDD right = mtbdd_refs_sync(SYNC(bdd_from_ldd_rel));

        /* take union of current and right */
        mtbdd_refs_push(right);
        result = sylvan_or(down, right);
        mtbdd_refs_pop(2);
    } else if (vmeta == 2 || vmeta == 4) {
        /* write or only-write level */

        /* spawn right */
        assert(mddnode_getright(n) != mtbdd_true);
        mtbdd_refs_spawn(SPAWN(bdd_from_ldd_rel, mddnode_getright(n), bits_mdd, firstvar, meta));

        /* get recursive result */
        MTBDD down = CALL(bdd_from_ldd_rel, mddnode_getdown(n), mddnode_getdown(nbits), firstvar + 2*bits, mddnode_getdown(nmeta));

        if (mddnode_getcopy(n)) {
            /* encode a copy node */
            for (int i=0; i<bits; i++) {
                int bit = bits-i-1;
                MTBDD low = mtbdd_makenode(firstvar + 2*bit + 1, down, mtbdd_false);
                mtbdd_refs_push(low);
                MTBDD high = mtbdd_makenode(firstvar + 2*bit + 1, mtbdd_false, down);
                mtbdd_refs_pop(1);
                down = mtbdd_makenode(firstvar + 2*bit, low, high);
            }
        } else {
            /* encode written value */
            uint32_t val = mddnode_getvalue(n);
            for (int i=0; i<bits; i++) {
                /* encode with high bit first */
                int bit = bits-i-1;
                if (val & (1LL<<i)) down = mtbdd_makenode(firstvar + 2*bit + 1, mtbdd_false, down);
                else down = mtbdd_makenode(firstvar + 2*bit + 1, down, mtbdd_false);
            }
        }

        /* sync right */
        mtbdd_refs_push(down);
        MTBDD right = mtbdd_refs_sync(SYNC(bdd_from_ldd_rel));

        /* take union of current and right */
        mtbdd_refs_push(right);
        result = sylvan_or(down, right);
        mtbdd_refs_pop(2);
    } else if (vmeta == 3) {
        /* only-read level */
        assert(!mddnode_getcopy(n));  // do not process read copy nodes

        /* spawn right */
        mtbdd_refs_spawn(SPAWN(bdd_from_ldd_rel, mddnode_getright(n), bits_mdd, firstvar, meta));

        /* get recursive result */
        MTBDD down = CALL(bdd_from_ldd_rel, mddnode_getdown(n), mddnode_getdown(nbits), firstvar + 2*bits, mddnode_getdown(nmeta));

        /* encode read value */
        uint32_t val = mddnode_getvalue(n);
        for (int i=0; i<bits; i++) {
            /* encode with high bit first */
            int bit = bits-i-1;
            /* only-read, so write same value */
            if (val & (1LL<<i)) down = mtbdd_makenode(firstvar + 2*bit + 1, mtbdd_false, down);
            else down = mtbdd_makenode(firstvar + 2*bit + 1, down, mtbdd_false);
            if (val & (1LL<<i)) down = mtbdd_makenode(firstvar + 2*bit, mtbdd_false, down);
            else down = mtbdd_makenode(firstvar + 2*bit, down, mtbdd_false);
        }

        /* sync right */
        mtbdd_refs_push(down);
        MTBDD right = mtbdd_refs_sync(SYNC(bdd_from_ldd_rel));

        /* take union of current and right */
        mtbdd_refs_push(right);
        result = sylvan_or(down, right);
        mtbdd_refs_pop(2);
    } else if (vmeta == 5) {
        assert(!mddnode_getcopy(n));  // not allowed!

        /* we assume this is the last value */
        result = mtbdd_true;

        /* encode action value */
        uint32_t val = mddnode_getvalue(n);
        for (int i=0; i<actionbits; i++) {
            /* encode with high bit first */
            int bit = actionbits-i-1;
            /* only-read, so write same value */
            if (val & (1LL<<i)) result = mtbdd_makenode(1000000 + bit, mtbdd_false, result);
            else result = mtbdd_makenode(1000000 + bit, result, mtbdd_false);
        }
    } else {
        assert(vmeta <= 5);
    }

    cache_put4(bdd_from_ldd_rel_id, dd, bits_mdd, firstvar, meta, result);

    return result;
}

/**
 * Compute the BDD equivalent of the meta variable (to a variables cube)
 */
MTBDD
meta_to_bdd(MDD meta, MDD bits_mdd, uint32_t firstvar)
{
    if (meta == lddmc_false || meta == lddmc_true) return mtbdd_true;

    /* meta:
     * -1 is end
     *  0 is skip (no variables)
     *  1 is read (variables added by write)
     *  2 is write
     *  3 is only-read
     *  4 is only-write
     */

    const mddnode_t nmeta = LDD_GETNODE(meta);
    const uint32_t vmeta = mddnode_getvalue(nmeta);
    if (vmeta == (uint32_t)-1) return mtbdd_true;
    
    if (vmeta == 1) {
        /* return recursive result, don't go down on bits */
        return meta_to_bdd(mddnode_getdown(nmeta), bits_mdd, firstvar);
    }

    const mddnode_t nbits = LDD_GETNODE(bits_mdd);
    const int bits = (int)mddnode_getvalue(nbits);

    /* compute recursive result */
    MTBDD res = meta_to_bdd(mddnode_getdown(nmeta), mddnode_getdown(nbits), firstvar + 2*bits);

    /* add our variables if meta is 2,3,4 */
    if (vmeta != 0 && vmeta != 5) {
        for (int i=0; i<bits; i++) {
            res = mtbdd_makenode(firstvar + 2*(bits-i-1) + 1, mtbdd_false, res);
            res = mtbdd_makenode(firstvar + 2*(bits-i-1), mtbdd_false, res);
        }
    }

    return res;
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

VOID_TASK_0(gc_start)
{
    printf("Starting garbage collection\n");
}

VOID_TASK_0(gc_end)
{
    printf("Garbage collection done\n");
}

int
main(int argc, char **argv)
{
    argp_parse(&argp, argc, argv, 0, 0, 0);

    // Parse table sizes
    int tablesize, maxtablesize, cachesize, maxcachesize;
    if (sscanf(sizes, "%d,%d,%d,%d", &tablesize, &maxtablesize, &cachesize, &maxcachesize) != 4) {
        Abort("Invalid string for --table-sizes, try e.g. --table-sizes=23,28,22,27");
    }
    if (tablesize < 10 || maxtablesize < 10 || cachesize < 10 || maxcachesize < 10 ||
            tablesize > 40 || maxtablesize > 40 || cachesize > 40 || maxcachesize > 40) {
        Abort("Invalid string for --table-sizes, must be between 10 and 40");
    }
    if (tablesize > maxtablesize) {
        Abort("Invalid string for --table-sizes, tablesize is larger than maxtablesize");
    }
    if (cachesize > maxcachesize) {
        Abort("Invalid string for --table-sizes, cachesize is larger than maxcachesize");
    }

    // Report table sizes
    char buf[32];
    to_h((1ULL<<maxtablesize)*24+(1ULL<<maxcachesize)*36, buf);
    printf("Sylvan allocates %s virtual memory for nodes table and operation cache.\n", buf);
    to_h((1ULL<<tablesize)*24+(1ULL<<cachesize)*36, buf);
    printf("Initial nodes table and operation cache requires %s.\n", buf);

    // Init Lace
    lace_init(workers, 1000000); // auto-detect number of workers, use a 1,000,000 size task queue
    lace_startup(0, NULL, NULL); // auto-detect program stack, do not use a callback for startup

    LACE_ME;

    // Init Sylvan
    sylvan_set_sizes(1LL<<21, 1LL<<27, 1LL<<20, 1LL<<26);
    sylvan_init_package();
    sylvan_init_ldd();
    sylvan_init_mtbdd();
    sylvan_gc_hook_pregc(TASK(gc_start));
    sylvan_gc_hook_postgc(TASK(gc_end));

    // Obtain operation ids for the operation cache
    compute_highest_id = cache_next_opid();
    compute_highest_action_id = cache_next_opid();
    bdd_from_ldd_id = cache_next_opid();
    bdd_from_ldd_rel_id = cache_next_opid();

    // Open file
    FILE *f = fopen(model_filename, "r");
    if (f == NULL) Abort("Cannot open file '%s'!\n", model_filename);

    // Read integers per vector
    if (fread(&vector_size, sizeof(size_t), 1, f) != 1) Abort("Invalid input file!\n");

    // Read initial state
    if (verbose) {
        printf("Loading initial state... ");
        fflush(stdout);
    }
    set_t initial = set_load(f);
    if (verbose) printf("done.\n");

    // Read number of transitions
    if (fread(&next_count, sizeof(int), 1, f) != 1) Abort("Invalid input file!\n");
    next = (rel_t*)malloc(sizeof(rel_t) * next_count);

    // Read transitions
    if (verbose) {
        printf("Loading transition relations... ");
        fflush(stdout);
    }
    int i;
    for (i=0; i<next_count; i++) {
        next[i] = rel_load(f);
        if (verbose) {
            printf("%d, ", i);
            fflush(stdout);
        }
    }
    if (verbose) printf("done.\n");

    // Read whether reachable states are stored
    int has_reachable = 0;
    if (fread(&has_reachable, sizeof(int), 1, f) != 1) Abort("Input file missing reachable states!\n");
    if (has_reachable == 0) Abort("Input file missing reachable states!\n");

    // Read reachable states
    if (verbose) {
        printf("Loading reachable states... ");
        fflush(stdout);
    }
    set_t states = set_load(f);
    if (verbose) printf("done.\n");
    
    // Read number of action labels
    int action_labels_count = 0;
    if (fread(&action_labels_count, sizeof(int), 1, f) != 1) Abort("Input file missing action label count!\n");

    // Read action labels
    char *action_labels[action_labels_count];
    for (int i=0; i<action_labels_count; i++) {
        uint32_t len;
        if (fread(&len, sizeof(uint32_t), 1, f) != 1) Abort("Invalid input file!\n");
        action_labels[i] = (char*)malloc(sizeof(char[len+1]));
        if (fread(action_labels[i], sizeof(char), len, f) != len) Abort("Invalid input file!\n");
        action_labels[i][len] = 0;
    }

    // Close file
    fclose(f);

    // Report that we have read the input file
    printf("Read file %s.\n", argv[1]);

    // Report statistics
    if (verbose) {
        printf("%zu integers per state, %d transition groups\n", vector_size, next_count);
        printf("LDD nodes:\n");
        printf("Initial states: %zu LDD nodes\n", lddmc_nodecount(initial->mdd));
        for (i=0; i<next_count; i++) {
            printf("Transition %d: %zu LDD nodes\n", i, lddmc_nodecount(next[i]->mdd));
        }
    }

    // Report that we prepare BDD conversion
    if (verbose) printf("Preparing conversion to BDD...\n");

    // Compute highest value at each level (from reachable states)
    uint32_t highest[vector_size];
    for (size_t i=0; i<vector_size; i++) highest[i] = 0;
    compute_highest(states->mdd, highest);

    // Compute highest action label value (from transition relations)
    uint32_t highest_action = 0;
    for (int i=0; i<next_count; i++) {
        compute_highest_action(next[i]->mdd, next[i]->meta, &highest_action);
    }

    // Report highest integers
    /*
    printf("Highest integer per level: ");
    for (size_t i=0; i<vector_size; i++) {
        if (i>0) printf(", ");
        printf("%u", highest[i]);
    }
    printf("\n");
    */

    // Compute number of bits for each level
    int bits[vector_size];
    for (size_t i=0; i<vector_size; i++) {
        bits[i] = 0;
        while (highest[i] != 0) {
            bits[i]++;
            highest[i]>>=1;
        }
        if (bits[i] == 0) bits[i] = 1;
    }

    // Compute number of bits for action label
    actionbits = 0;
    while (highest_action != 0) {
        actionbits++;
        highest_action>>=1;
    }
    if (actionbits == 0 && has_actions) actionbits = 1;

    // Report number of bits
    if (verbose) {
        printf("Bits per level: ");
        for (size_t i=0; i<vector_size; i++) {
            if (i>0) printf(", ");
            printf("%d", bits[i]);
        }
        printf("\n");
        printf("Action bits: %d.\n", actionbits);
    }

    // Compute bits MDD
    MDD bits_mdd = lddmc_true;
    for (size_t i=0; i<vector_size; i++) {
        bits_mdd = lddmc_makenode(bits[vector_size-i-1], bits_mdd, lddmc_false);
    }
    lddmc_ref(bits_mdd);

    // Compute total number of bits
    int totalbits = 0;
    for (size_t i=0; i<vector_size; i++) {
        totalbits += bits[i];
    }

    // Compute state variables
    MTBDD state_vars = mtbdd_true;
    for (int i=0; i<totalbits; i++) {
        state_vars = mtbdd_makenode(2*(totalbits-i-1), mtbdd_false, state_vars);
    }
    mtbdd_protect(&state_vars);

    // Report that we begin the actual conversion
    if (verbose) printf("Converting to BDD...\n");

    // Create BDD file
    f = fopen(bdd_filename, "w");
    if (f == NULL) Abort("Cannot open file '%s'!\n", bdd_filename);

    // Write domain...
    int vector_size = 1;
    fwrite(&totalbits, sizeof(int), 1, f);  // use number of bits as vector size
    fwrite(&vector_size, sizeof(int), 1, f);  // set each to 1
    fwrite(&actionbits, sizeof(int), 1, f);

    // Write initial state...
    MTBDD new_initial = bdd_from_ldd(initial->mdd, bits_mdd, 0);
    assert((size_t)mtbdd_satcount(new_initial, totalbits) == (size_t)lddmc_satcount_cached(initial->mdd));
    mtbdd_refs_push(new_initial);
    {
        size_t a = sylvan_serialize_add(new_initial);
        size_t b = sylvan_serialize_add(state_vars);
        size_t s = totalbits;
        sylvan_serialize_tofile(f);
        fwrite(&a, sizeof(size_t), 1, f);
        fwrite(&s, sizeof(size_t), 1, f);
        fwrite(&b, sizeof(size_t), 1, f);
    }

    // Custom operation that converts to BDD given number of bits for each level
    MTBDD new_states = bdd_from_ldd(states->mdd, bits_mdd, 0);
    assert((size_t)mtbdd_satcount(new_states, totalbits) == (size_t)lddmc_satcount_cached(states->mdd));
    mtbdd_refs_push(new_states);

    // Report size of BDD
    if (verbose) {
        printf("Initial states: %zu BDD nodes\n", mtbdd_nodecount(new_initial));
        printf("Reachable states: %zu BDD nodes\n", mtbdd_nodecount(new_states));
    }

    // Write number of transitions
    fwrite(&next_count, sizeof(int), 1, f);

    // Write transitions
    for (int i=0; i<next_count; i++) {
        // Compute new transition relation
        MTBDD new_rel = bdd_from_ldd_rel(next[i]->mdd, bits_mdd, 0, next[i]->meta);
        mtbdd_refs_push(new_rel);

        // Compute new <variables> for the current transition relation
        MTBDD new_vars = meta_to_bdd(next[i]->meta, bits_mdd, 0);
        mtbdd_refs_push(new_vars);

        if (check_results) {
            // Test if the transition is correctly converted
            MTBDD test = sylvan_relnext(new_states, new_rel, new_vars);
            mtbdd_refs_push(test);
            MDD succ = lddmc_relprod(states->mdd, next[i]->mdd, next[i]->meta);
            lddmc_refs_push(succ);
            MTBDD test2 = bdd_from_ldd(succ, bits_mdd, 0);
            if (test != test2) Abort("Conversion error!\n");
            mtbdd_refs_pop(1);
            lddmc_refs_pop(1);
        }

        // Report number of nodes
        if (verbose) printf("Transition %d: %zu BDD nodes\n", i, mtbdd_nodecount(new_rel));

        size_t a = sylvan_serialize_add(new_rel);
        size_t b = sylvan_serialize_add(new_vars);
        sylvan_serialize_tofile(f);
        fwrite(&a, sizeof(size_t), 1, f);
        fwrite(&b, sizeof(size_t), 1, f);
    }

    // Write reachable states
    has_reachable = 1;
    fwrite(&has_reachable, sizeof(int), 1, f);

    {
        size_t a = sylvan_serialize_add(new_states);
        size_t b = sylvan_serialize_add(state_vars);
        size_t s = totalbits;
        sylvan_serialize_tofile(f);
        fwrite(&a, sizeof(size_t), 1, f);
        fwrite(&s, sizeof(size_t), 1, f);
        fwrite(&b, sizeof(size_t), 1, f);
    }

    // Write action labels
    fwrite(&action_labels_count, sizeof(int), 1, f);
    for (int i=0; i<action_labels_count; i++) {
        uint32_t len = strlen(action_labels[i]);
        fwrite(&len, sizeof(uint32_t), 1, f);
        fwrite(action_labels[i], sizeof(char), len, f);
    }

    // Close the file
    fclose(f);

    // Report to the user
    printf("Written file %s.\n", bdd_filename);

    // Report Sylvan statistics (if SYLVAN_STATS is set)
    if (verbose) sylvan_stats_report(stdout);

    return 0;
}
