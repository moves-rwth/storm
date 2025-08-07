#include <argp.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sstream>

#include <sylvan_int.h>

#include <meddly.h>
#include <meddly_expert.h>

/* Configuration */
static int verbose = 0;
static char* model_filename = NULL; // filename of model
static char* out_filename = NULL; // filename of output BDD
static int no_reachable = 0;

/* argp configuration */
static struct argp_option options[] =
{
    {"no-reachable", 1, 0, 0, "Do not write reachabile states", 0},
    {"verbose", 'v', 0, 0, "Set verbose", 0},
    {0, 0, 0, 0, 0, 0}
};

using namespace sylvan;
using namespace MEDDLY;

FILE_output meddlyout(stdout);

static error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
    switch (key) {
    case 'v':
        verbose = 1;
        break;
    case 1:
        no_reachable = 1;
        break;
    case ARGP_KEY_ARG:
        if (state->arg_num == 0) model_filename = arg;
        if (state->arg_num == 1) out_filename = arg;
        if (state->arg_num >= 2) argp_usage(state);
        break; 
    case ARGP_KEY_END:
        if (state->arg_num < 1) argp_usage(state);
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, "<model> [<output-bdd>]", 0, 0, 0, 0 };

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

/**
 * Types (set and relation)
 */
typedef struct set
{
    MDD dd;
} *set_t;

typedef struct relation
{
    MDD dd;
    MDD meta; // for relprod
    int r_k, w_k, *r_proj, *w_proj;
} *rel_t;

static int vector_size; // size of vector
static int next_count; // number of partitions of the transition relation
static rel_t *next; // each partition of the transition relation
static int has_actions = 0;

#define Abort(...) { fprintf(stderr, __VA_ARGS__); fprintf(stderr, "Abort at line %d!\n", __LINE__); exit(-1); }

/* Load a set from file */
#define set_load(f) RUN(set_load, f)
TASK_1(set_t, set_load, FILE*, f)
{
    set_t set = (set_t)malloc(sizeof(struct set));

    int k;
    if (fread(&k, sizeof(int), 1, f) != 1) Abort("Invalid input file!");
    if (k != -1) Abort("Invalid input file!");

    lddmc_serialize_fromfile(f);
    size_t dd;
    if (fread(&dd, sizeof(size_t), 1, f) != 1) Abort("Invalid input file!");
    set->dd = lddmc_serialize_get_reversed(dd);
    lddmc_protect(&set->dd);

    return set;
}

/* Load a relation from file */
#define rel_load_proj(f) RUN(rel_load_proj, f)
TASK_1(rel_t, rel_load_proj, FILE*, f)
{
    int r_k, w_k;
    if (fread(&r_k, sizeof(int), 1, f) != 1) Abort("Invalid file format.");
    if (fread(&w_k, sizeof(int), 1, f) != 1) Abort("Invalid file format.");

    rel_t rel = (rel_t)malloc(sizeof(struct relation));
    rel->r_k = r_k;
    rel->w_k = w_k;
    rel->r_proj = (int*)malloc(sizeof(int[rel->r_k]));
    rel->w_proj = (int*)malloc(sizeof(int[rel->w_k]));

    if (fread(rel->r_proj, sizeof(int), rel->r_k, f) != (size_t)rel->r_k) Abort("Invalid file format.");
    if (fread(rel->w_proj, sizeof(int), rel->w_k, f) != (size_t)rel->w_k) Abort("Invalid file format.");

    int *r_proj = rel->r_proj;
    int *w_proj = rel->w_proj;

    /* Compute the meta */
    uint32_t meta[vector_size*2+2];
    memset(meta, 0, sizeof(uint32_t[vector_size*2+2]));
    int r_i=0, w_i=0, i=0, j=0;
    for (;;) {
        int type = 0;
        if (r_i < r_k && r_proj[r_i] == i) {
            r_i++;
            type += 1; // read
        }
        if (w_i < w_k && w_proj[w_i] == i) {
            w_i++;
            type += 2; // write
        }
        if (type == 0) meta[j++] = 0;
        else if (type == 1) { meta[j++] = 3; }
        else if (type == 2) { meta[j++] = 4; }
        else if (type == 3) { meta[j++] = 1; meta[j++] = 2; }
        if (r_i == r_k && w_i == w_k) {
            meta[j++] = 5; // action label
            meta[j++] = (uint32_t)-1;
            break;
        }
        i++;
    }

    rel->meta = lddmc_cube((uint32_t*)meta, j);
    rel->dd = lddmc_false;

    lddmc_protect(&rel->meta);
    lddmc_protect(&rel->dd);

    return rel;
}

#define rel_load(f, rel) RUN(rel_load, f, rel)
VOID_TASK_2(rel_load, FILE*, f, rel_t, rel)
{
    lddmc_serialize_fromfile(f);
    size_t dd;
    if (fread(&dd, sizeof(size_t), 1, f) != 1) Abort("Invalid input file!");
    rel->dd = lddmc_serialize_get_reversed(dd);
}

/**
 * Compute the highest value for each variable level.
 * This method is called for the set of reachable states.
 */
static uint64_t compute_highest_id;
#define compute_highest(dd, arr) RUN(compute_highest, dd, arr)
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
 * Removes the action labels
 */
static uint64_t strip_actions_cache_id;
#define strip_actions(dd, meta) RUN(strip_actions, dd, meta)
TASK_2(MDD, strip_actions, MDD, dd, MDD, meta)
{
    if (dd == lddmc_false) return lddmc_false;
    if (dd == lddmc_true) {
        // now meta must be end...
        if (meta != lddmc_true) {
            const mddnode_t nmeta = LDD_GETNODE(meta);
            const uint32_t vmeta = mddnode_getvalue(nmeta);
            // printf("Vmeta is %d\n", (int)vmeta);
            assert(vmeta == (uint32_t)-1);
        }
        return lddmc_true;
    }

    /* meta:
     *  0 is skip
     *  1 is read
     *  2 is write
     *  3 is only-read
     *  4 is only-write
     *  5 is action label (at end, before -1)
     * -1 is end
     */

    assert(meta != lddmc_false and meta != lddmc_true);
    mddnode_t nmeta = LDD_GETNODE(meta);
    uint32_t vmeta = mddnode_getvalue(nmeta);
 
    while (vmeta == 0) {
        meta = mddnode_getdown(nmeta);
        assert(meta != lddmc_false and meta != lddmc_true);
        nmeta = LDD_GETNODE(meta);
        vmeta = mddnode_getvalue(nmeta);
    }

    assert(vmeta != (uint32_t)-1);

    uint64_t result = 0;
    if (cache_get3(strip_actions_cache_id, dd, meta, 0, &result)) return result;

    const mddnode_t n = LDD_GETNODE(dd);
    MDD down = mddnode_getdown(n);
    MDD right = mddnode_getright(n);

    if (right != lddmc_false) {
        lddmc_refs_spawn(SPAWN(strip_actions, right, meta));
        down = CALL(strip_actions, down, mddnode_getdown(nmeta));
        lddmc_refs_pushptr(&down);
        right = lddmc_refs_sync(SYNC(strip_actions));
        lddmc_refs_popptr(1);
    } else {
        down = CALL(strip_actions, down, mddnode_getdown(nmeta));
    }

    if (vmeta == 5) {
        // printf("Found action label things\n");
        // if (down == lddmc_true) printf("Down is T as expected.\n");
        lddmc_refs_pushptr(&down);
        lddmc_refs_pushptr(&right);
        result = lddmc_union(down, right);
        lddmc_refs_popptr(2);
        // assert(result == lddmc_true);
    } else {
        result = lddmc_makenode(mddnode_getvalue(n), down, right);
    }

    cache_put3(strip_actions_cache_id, dd, meta, 0, result);
    return result;
}

/**
 * Removes the action labels (meta)
 */
MDD
strip_actions_meta(MDD meta)
{
    if (meta == lddmc_true) return lddmc_true;
    assert(meta != lddmc_false);

    mddnode_t nmeta = LDD_GETNODE(meta);
    uint32_t vmeta = mddnode_getvalue(nmeta);
 
    if (vmeta == 5) return strip_actions_meta(mddnode_getdown(nmeta));
    else return lddmc_makenode(vmeta, strip_actions_meta(mddnode_getdown(nmeta)), lddmc_false);
}

/**
 * Compute the BDD equivalent of an LDD transition relation.
 */
static uint64_t ldd_rel_to_meddly_cache_id;
int
ldd_rel_to_meddly(const MDD dd, const MDD meta, expert_forest *F, const int level)
{
    if (dd == lddmc_false) return F->handleForValue(false);
    if (dd == lddmc_true) return F->handleForValue(true);

    if (level == 0) {
        printf("We are unexpectedly at level 0. Remaining meta:");
        MDD _meta = meta;
        while (_meta != lddmc_true) {
            printf(" %d", lddmc_getvalue(_meta));
            _meta = lddmc_getdown(_meta);
        }
        printf("\n");
        lddmc_print(dd);
        printf("\n");
        // if (lddmc_getvalue(meta) == -1) return F->handleForValue(true);
    }

    assert(level != 0);
    assert(level > 0);

    assert(meta != lddmc_false && meta != lddmc_true);

    /* meta:
     * -1 is end
     *  0 is skip
     *  1 is read
     *  2 is write
     *  3 is only-read
     *  4 is only-write
     *  5 is "label" and should not occur!
     */

    uint64_t result;
    if (cache_get3(ldd_rel_to_meddly_cache_id, dd, meta, level, &result)) {
        assert (F->isActiveNode(result));
        return (int)result;
    }

    const mddnode_t n = LDD_GETNODE(dd);

    const mddnode_t nmeta = LDD_GETNODE(meta);
    const uint32_t vmeta = mddnode_getvalue(nmeta);
    const MDD next_meta = mddnode_getdown(nmeta);

    if (vmeta == (uint32_t)-1) {
#ifndef NDEBUG
        printf("Oh noes.\nlevel is %d, remainder is %zu levels deep\n", level, lddmc_test_ismdd(dd));
#endif
    }
    assert(vmeta <= 4);

    if (vmeta == 0) {
        /* skip level */
        result = ldd_rel_to_meddly(dd, next_meta, F, level-1);
    } else if (vmeta == 1) {
        /* read level, a write level will follow */
        assert(!mddnode_getcopy(n));  // do not process read copy nodes for now
        assert(mddnode_getvalue(LDD_GETNODE(next_meta)) == 2); // write must follow

        // Determine length of LDD list
        int len = 0;
        for (MDD x = dd; x != lddmc_false; x = lddmc_getright(x)) len++;

        unpacked_node* nb = unpacked_node::newSparse(F, level, len);
        MDD x = dd;
        for (int i=0; i<len; i++) {
            nb->i_ref(i) = lddmc_getvalue(x);
            int m_d = ldd_rel_to_meddly(lddmc_getdown(x), next_meta, F, level);
            nb->d_ref(i) = F->linkNode(m_d);
            x = lddmc_getright(x);
        }

        result = F->createReducedNode(-1, nb);
    } else if (vmeta == 3) {
        /* only-read level */
        assert(!mddnode_getcopy(n));  // do not process read copy nodes for now

        // Determine length of LDD list
        int len = 0;
        for (MDD x = dd; x != lddmc_false; x = lddmc_getright(x)) len++;

        unpacked_node* nb = unpacked_node::newSparse(F, level, len);
        MDD x = dd;
        for (int i=0; i<len; i++) {
            // Now create a reduced WRITE node on top of the recursive result
            unpacked_node *nx = unpacked_node::newSparse(F, -level, 1);
            nx->i_ref(0) = lddmc_getvalue(x);
            int m_d = ldd_rel_to_meddly(lddmc_getdown(x), next_meta, F, level-1);
            nx->d_ref(0) = F->linkNode(m_d);
            nb->i_ref(i) = lddmc_getvalue(x);
            nb->d_ref(i) = F->linkNode(F->createReducedNode(-1, nx));
            x = lddmc_getright(x);
        }

        result = F->createReducedNode(-1, nb);
    } else if (vmeta == 2 or vmeta == 4) {
        /* write or only-write level */
        assert(!mddnode_getcopy(n));  // do not process read copy nodes for now

        // Determine length of LDD list
        int len = 0;
        for (MDD x = dd; x != lddmc_false; x = lddmc_getright(x)) len++;

        unpacked_node* nb = unpacked_node::newSparse(F, -level, len);
        MDD x = dd;
        for (int i=0; i<len; i++) {
            nb->i_ref(i) = lddmc_getvalue(x);
            int m_d = ldd_rel_to_meddly(lddmc_getdown(x), next_meta, F, level-1);
            nb->d_ref(i) = F->linkNode(m_d);
            x = lddmc_getright(x);
        }

        result = F->createReducedNode(-1, nb);
    } else {
        assert(0);
    }

    F->linkNode(result); // never delete...
    cache_put3(ldd_rel_to_meddly_cache_id, dd, meta, level, result);

    return result;
}

static uint64_t ldd_to_meddly_cache_id;
int
ldd_to_meddly(MDD inp, expert_forest *F, int level)
{
    if (level == 0) {
        assert(inp == lddmc_true);
        return F->handleForValue(1);
    }
    assert(inp != lddmc_true);
    assert(inp != lddmc_false); // should NOT happen!

    uint64_t result;
    /* get from cache */
    /* note: some assumptions about the encoding... */
    if (cache_get3(ldd_to_meddly_cache_id, inp, 0, 0, &result)) {
        assert(F->isActiveNode(result));
        return (int)result;
    }

    // Determine length of LDD list
    int len = 0;
    MDD x = inp;
    while (x != lddmc_false) {
        len++;
        x = lddmc_getright(x);
    }

    unpacked_node* nb = unpacked_node::newSparse(F, level, len);
    x = inp;
    for (int i=0; i<len; i++) {
        nb->i_ref(i) = lddmc_getvalue(x);
        if (level == 1) {
            nb->d_ref(i) = F->handleForValue(1);
        } else {
            nb->d_ref(i) = F->linkNode(ldd_to_meddly(lddmc_getdown(x), F, level-1));
        }
        x = lddmc_getright(x);
    }

    node_handle res = F->createReducedNode(-1, nb);

    /* put in cache */
    F->linkNode(res); // never delete...
    cache_put3(ldd_to_meddly_cache_id, inp, 0, 0, res);

    return res;
}

VOID_TASK_0(gc_start)
{
    printf("Starting garbage collection\n");
}

VOID_TASK_0(gc_end)
{
    printf("Garbage collection done\n");
}

static void
print_matrix(size_t size, MDD meta)
{
    if (size == 0 and meta == lddmc_true) return;

    uint32_t val = lddmc_getvalue(meta);
    if (val == 5) {
        printf("A");
        print_matrix(size, lddmc_follow(meta, val));
    } else if (val == 1) {
        printf("+");
        print_matrix(size-1, lddmc_follow(lddmc_follow(meta, 1), 2));
    } else {
        if (val == (uint32_t)-1) {
            while (size) {
                printf("-");
                size--;
            }
            return;
        }
        else if (val == 0) printf("-");
        else if (val == 3) printf("r");
        else if (val == 4) printf("w");
        print_matrix(size-1, lddmc_follow(meta, val));
    }
}

void run()
{
    // Init Lace with only 1 worker
    lace_start(1, 1000000); // auto-detect number of workers, use a 1,000,000 size task queue

    // Init Sylvan
    sylvan_set_limits(2LL<<30, 1, 10);
    sylvan_init_package();
    sylvan_init_ldd();
    sylvan_init_mtbdd();
    sylvan_gc_hook_pregc(TASK(gc_start));
    sylvan_gc_hook_postgc(TASK(gc_end));

    // Obtain operation ids for the operation cache
    compute_highest_id = cache_next_opid();
    strip_actions_cache_id = cache_next_opid();
    ldd_to_meddly_cache_id = cache_next_opid();
    ldd_rel_to_meddly_cache_id = cache_next_opid();

    // Open file
    FILE *f = fopen(model_filename, "r");
    if (f == NULL) Abort("Cannot open file '%s'!\n", model_filename);

    // Read integers per vector
    if (fread(&vector_size, sizeof(int), 1, f) != 1) Abort("Invalid input file!\n");

    // Read initial state
    if (verbose) printf("Loading initial state.\n");
    set_t initial = set_load(f);

    // Read number of transitions
    if (fread(&next_count, sizeof(int), 1, f) != 1) Abort("Invalid input file!\n");
    next = (rel_t*)malloc(sizeof(rel_t) * next_count);

    // Read transitions
    if (verbose) printf("Loading transition relations.\n");
    for (int i=0; i<next_count; i++) next[i] = rel_load_proj(f);
    for (int i=0; i<next_count; i++) rel_load(f, next[i]);

    // Read whether reachable states are stored
    int has_reachable = 0;
    if (fread(&has_reachable, sizeof(int), 1, f) != 1) Abort("Input file missing reachable states!\n");
    if (has_reachable == 0) Abort("Input file missing reachable states!\n");

    // Read reachable states
    if (verbose) printf("Loading reachable states.\n");
    set_t states = set_load(f);
    
    // Read number of action labels
    int action_labels_count = 0;
    if (fread(&action_labels_count, sizeof(int), 1, f) != 1) action_labels_count = 0;
    // ignore: Abort("Input file missing action label count!\n");

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
    printf("Read file %s.\n", model_filename);

    // Report statistics
    if (verbose) {
        printf("%d integers per state, %d transition groups\n", vector_size, next_count);
        printf("LDD nodes:\n");
        printf("Initial states: %zu LDD nodes, %.0Lf states\n", lddmc_nodecount(initial->dd), lddmc_satcount(initial->dd));
        printf("Reachable states: %zu LDD nodes, %.0Lf states\n", lddmc_nodecount(states->dd), lddmc_satcount(states->dd));
        for (int i=0; i<next_count; i++) {
            printf("Transition %d: %zu LDD nodes, %.0Lf transitions\n", i, lddmc_nodecount(next[i]->dd), lddmc_satcount(next[i]->dd));
        }
    }

    printf("Removing action labels...\n");

    // get rid of the actions
    for (int i=0; i<next_count; i++) {
        next[i]->dd = strip_actions(next[i]->dd, next[i]->meta);
        next[i]->meta = strip_actions_meta(next[i]->meta);
    }

#if 0
    for (int i=0; i<next_count; i++) {
        char buf[80];
        sprintf(buf, "next-%d.ldd.dot", i);
        FILE* f=fopen(buf, "w");
        lddmc_fprintdot(f, next[i]->dd);
        fclose(f);
        /*
        std::stringstream cmd;
        cmd << "dot -Tpng -o next-" << i << ".ldd.png " << buf;
        if (system(cmd.str().c_str())) {
            std::cerr << __func__ << ": Error executing DOT command: ";
            std::cerr << cmd.str().c_str() << "\n";
        }
        */
    }
#endif

    // Report that we prepare BDD conversion
    printf("Converting to Meddly MDD...\n");

    // Compute highest value at each level (from reachable states)
    uint32_t highest[vector_size];
    for (int i=0; i<vector_size; i++) highest[i] = 0;
    compute_highest(states->dd, highest);

    // Report number of bits
    if (verbose) {
        printf("Number of values per level: ");
        for (int i=0; i<vector_size; i++) {
            if (i>0) printf(", ");
            printf("%d", highest[i]);
        }
        printf("\n");
    }


    // initializer_list* L = defaultInitializerList(0);
    // ct_initializer::setMaxSize(16 * 16777216);
    // MEDDLY::initialize(L);

    MEDDLY::initialize();

    // Initialize domain
    int* sizes = new int[vector_size];
    for (int i=0; i<vector_size; i++) sizes[vector_size-i-1] = highest[i]+1; // TODO actually look at chain lengths?
    domain* d = createDomainBottomUp(sizes, vector_size);

    // Initialize forests
    expert_forest* mdd = (expert_forest*)d->createForest(0, forest::BOOLEAN, forest::MULTI_TERMINAL);
    expert_forest* mxd = (expert_forest*)d->createForest(1, forest::BOOLEAN, forest::MULTI_TERMINAL);

    dd_edge m_initial(mdd);
    m_initial.set(ldd_to_meddly(initial->dd, mdd, vector_size));

    dd_edge m_states(mdd);
    m_states.set(ldd_to_meddly(states->dd, mdd, vector_size));

    dd_edge m_next[next_count];
    dd_edge m_tmp(mxd);
    for (int i=0; i<next_count; i++) {
        if (verbose) printf("Doing transition %d\n", i);
        m_next[i] = m_tmp;
        m_next[i].set(ldd_rel_to_meddly(next[i]->dd, next[i]->meta, mxd, vector_size));
    }

    // Report statistics
    if (verbose) {
        printf("MEDDLY MDD nodes:\n");
        printf("Initial states: %u MDD nodes\n", m_initial.getNodeCount());
        printf("Reachable states: %u MDD nodes\n", m_states.getNodeCount());
        for (int i=0; i<next_count; i++) {
            printf("Transition %d: %u MDD nodes\n", i, m_next[i].getNodeCount());
        }
    }

    printf("Conversion finished.\n");

    if (out_filename != NULL) {
        FILE* out = fopen(out_filename, "w");
        if (out == NULL) {
            printf("Cannot open file for writing!\n");
            exit(0);
        }
        FILE_output m_out(out);

        dd_edge list[2];
        list[0] = m_initial;
        list[1] = m_states;

        fprintf(out, "model %d %d\n\t", vector_size, next_count);
        for (int i=0; i<vector_size; i++) fprintf(out, "%d ", sizes[i]);
        fprintf(out, "\n\t");

        // Workaround for a bug in Meddly, where if we send _only_ empty
        // MDDs to store, the library crashes. So we just store whether we have each block
        const int blocks = (next_count+999)/1000;
        int have_block[blocks];
        for (int b=0; b<blocks; b++) {
            have_block[b] = 0;
            int cnt = next_count - b*1000;
            if (cnt > 1000) cnt = 1000;
            for (int c=0; c<cnt; c++) {
                if (next[1000*b+c]->dd != lddmc_false) {
                    have_block[b] = 1;
                    break;
                }
            }
            fprintf(out, "%d ", have_block[b]);
        }

        fprintf(out, "\n");
        fprintf(out, "ledom\n");

        // Due to a bug in Meddly, we can't use next_count if next_count > 1024
        for (int b=0; b<blocks; b++) {
            if (have_block[b]) {
                int64_t offset = 1000*b;
                int cnt = next_count - b*1000;
                if (cnt > 1000) cnt = 1000;
                mxd->writeEdges(m_out, m_next+offset, cnt);
            }
        }

        mdd->writeEdges(m_out, list, 2);

        fclose(out);

        // Report to the user
        printf("Written file %s.\n", out_filename);
        exit(0);
    }

    // Report Sylvan statistics (if SYLVAN_STATS is set)
    if (verbose) sylvan_stats_report(stdout);
    sylvan_quit();
    lace_stop();

    printf("Testing correctness by running event-saturation on the result...\n");

    satpregen_opname::pregen_relation* ensf = 
        new satpregen_opname::pregen_relation(mdd, mxd, mdd, next_count);
    for (int i=0; i<next_count; i++) ensf->addToRelation(m_next[i]);
    ensf->finalize();

    specialized_operation* sat = SATURATION_FORWARD->buildOperation(ensf);

    dd_edge m_reachable(mdd);
    double t1 = wctime();
    sat->compute(m_initial, m_reachable);
    double t2 = wctime();

    double c;
    apply(CARDINALITY, m_initial, c);
    printf("Approx. %.0f initial states\n", c);
    apply(CARDINALITY, m_reachable, c);
    printf("Approx. %.0f reachable states\n", c);
    apply(CARDINALITY, m_states, c);
    printf("Approx. %.0f expected reachable states\n", c);

    printf("MEDDLY Time: %f\n", t2-t1);

    if (m_reachable == m_states) {
        printf("CORRECT\n");
    } else {
        printf("INCORRECT\n");
    }

    assert(m_reachable == m_states);

    /*
    if (verbose) {
        mdd->reportStats(meddlyout, "\t",
            expert_forest::HUMAN_READABLE_MEMORY |
            expert_forest::BASIC_STATS | expert_forest::EXTRA_STATS |
            expert_forest::STORAGE_STATS | expert_forest::HOLE_MANAGER_STATS |
            expert_forest::HOLE_MANAGER_DETAILED);
    }*/
}

int
main(int argc, char **argv)
{
    argp_parse(&argp, argc, argv, 0, 0, 0);

    try {
        run();
        MEDDLY::cleanup();
        return 0;
    }
    catch (MEDDLY::error e) {
        printf("Caught MEDDLY error: %s in %s:%d\n", e.getName(), e.getFile(), e.getLine());
        return 1;
    }
}
