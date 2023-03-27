#include <argp.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <sys/time.h>

#if 0
#include <gmp.h>
#endif

#include <meddly.h>
#include <meddly_expert.h>

/* Configuration */
static int verbose = 0;
static char* model_filename = NULL; // filename of model

/* argp configuration */
static struct argp_option options[] =
{
    {"verbose", 'v', 0, 0, "Set verbose", 0},
    {0, 0, 0, 0, 0, 0}
};

static error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
    switch (key) {
    case 'v':
        verbose = 1;
        break;
    case ARGP_KEY_ARG:
        if (state->arg_num == 0) model_filename = arg;
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

#define Abort(...) { fprintf(stderr, __VA_ARGS__); fprintf(stderr, "Abort at line %d!\n", __LINE__); exit(-1); }

using namespace MEDDLY;

void run()
{
    // Initialize MEDDLY
    MEDDLY::initialize();

    // Open file
    FILE *f = fopen(model_filename, "r");
    if (f == NULL) Abort("Cannot open file '%s'!\n", model_filename);
    FILE_input s(f);
 
    // Read model metadata
    s.stripWS();
    s.consumeKeyword("model");
    s.stripWS();
    int vector_size = s.get_integer();
    s.stripWS();
    int next_count = s.get_integer();

    int sizes[vector_size];
    for (int i=0; i<vector_size; i++) {
        s.stripWS();
        sizes[i] = s.get_integer();
    }

    const int blocks = (next_count+999)/1000;
    int have_block[blocks];
    for (int b=0; b<blocks; b++) {
        s.stripWS();
        have_block[b] = s.get_integer();
    }

    s.stripWS();
    s.consumeKeyword("ledom");

    // Report 
    printf("Making a domain of %d levels; there are %d transition relations (events)\n", vector_size, next_count);

    // Initialize domain and forests
    domain* d = createDomainBottomUp(sizes, vector_size);
    forest* mdd = d->createForest(0, forest::BOOLEAN, forest::MULTI_TERMINAL);
    forest* mxd = d->createForest(1, forest::BOOLEAN, forest::MULTI_TERMINAL);

    // Read transition relation and initial+reachable states
    dd_edge m_next[next_count];
    dd_edge m_sets[2];

    // Due to a bug in Meddly, we can't use next_count if next_count > 1024
    // Due to another bug, we have a workaround if all MDDs in a block are False
    dd_edge false_edge(mxd);
    false_edge.set(((expert_forest*)mxd)->handleForValue(false));

    for (int b=0; b<blocks; b++) {
        int64_t offset = 1000*b;
        int cnt = next_count - b*1000;
        if (cnt > 1000) cnt = 1000;
        if (have_block[b]) {
            mxd->readEdges(s, m_next+offset, cnt);
        } else {
            for (int c=0; c<cnt; c++) {
                m_next[offset+c] = false_edge;
            }
        }
    }

    mdd->readEdges(s, m_sets, 2);

    fclose(f);

    // Report
    if (verbose) {
        printf("Initial states: %u MDD nodes\n", m_sets[0].getNodeCount());
        printf("Reachable states: %u MDD nodes\n", m_sets[1].getNodeCount());
        for (int i=0; i<next_count; i++) {
            printf("Transition %d: %u MDD nodes\n", i, m_next[i].getNodeCount());
        }
    }

    // Prepare Event-Saturation
    satpregen_opname::pregen_relation* ensf = new satpregen_opname::pregen_relation(mdd, mxd, mdd, next_count);
    for (int i=0; i<next_count; i++) ensf->addToRelation(m_next[i]);
    ensf->finalize();
    specialized_operation* sat = SATURATION_FORWARD->buildOperation(ensf);
    dd_edge m_reachable(mdd);

    // Run Event-Saturation
    double t1 = wctime();
    sat->compute(m_sets[0], m_reachable);
    double t2 = wctime();

    // Report
    printf("MEDDLY Time: %f\n", t2-t1);

    double c;
    apply(CARDINALITY, m_reachable, c);
    printf("States: %.0f\n", c);

#if 0
    // Nice idea, but there is a bug in Meddly.

    mpz_t mpz;
    mpz_init(mpz);
    mpz_clear(mpz);
    apply(CARDINALITY, m_reachable, mpz);
    printf("Precise states: ");
    mpz_out_str(0, 10, mpz);
    printf("\n");
#endif

    // Debug check
    if (m_reachable != m_sets[1]) {
        printf("INCORRECT\n");
        assert(m_reachable == m_sets[1]);
    }

    if (verbose) {
        FILE_output meddlyout(stdout);
        printf("MEDDLY Stats:\n");
        ((expert_forest*)mdd)->reportStats(meddlyout, "|| ",
            expert_forest::HUMAN_READABLE_MEMORY |
            expert_forest::BASIC_STATS | expert_forest::EXTRA_STATS |
            expert_forest::STORAGE_STATS);
        ((expert_forest*)mxd)->reportStats(meddlyout, "|| ",
            expert_forest::HUMAN_READABLE_MEMORY |
            expert_forest::BASIC_STATS | expert_forest::EXTRA_STATS |
            expert_forest::STORAGE_STATS);
    }
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
