#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <inttypes.h>

#include <assert.h>
#include "test_assert.h"

#include "llmsset.h"
#include "sylvan.h"

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

__thread uint64_t seed = 1;

uint64_t
xorshift_rand(void)
{
    uint64_t x = seed;
    if (seed == 0) seed = rand();
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    seed = x;
    return x * 2685821657736338717LL;
}

double
uniform_deviate(uint64_t seed)
{
    return seed * (1.0 / (0xffffffffffffffffL + 1.0));
}

int
rng(int low, int high)
{
    return low + uniform_deviate(xorshift_rand()) * (high-low);
}

static inline BDD
make_random(int i, int j)
{
    if (i == j) return rng(0, 2) ? sylvan_true : sylvan_false;

    BDD yes = make_random(i+1, j);
    BDD no = make_random(i+1, j);
    BDD result = sylvan_invalid;

    switch(rng(0, 4)) {
    case 0:
        result = no;
        sylvan_deref(yes);
        break;
    case 1:
        result = yes;
        sylvan_deref(no);
        break;
    case 2:
        result = sylvan_ref(sylvan_makenode(i, yes, no));
        sylvan_deref(no);
        sylvan_deref(yes);
        break;
    case 3:
    default:
        result = sylvan_ref(sylvan_makenode(i, no, yes));
        sylvan_deref(no);
        sylvan_deref(yes);
        break;
    }

    return result;
}

/** GC testing */
VOID_TASK_2(gctest_fill, int, levels, int, width)
{
    if (levels > 1) {
        int i;
        for (i=0; i<width; i++) { SPAWN(gctest_fill, levels-1, width); }
        for (i=0; i<width; i++) { SYNC(gctest_fill); }
    } else {
        sylvan_deref(make_random(0, 10));
    }
}

void report_table()
{
    llmsset_t __sylvan_get_internal_data();
    llmsset_t tbl = __sylvan_get_internal_data();
    LACE_ME;
    size_t filled = llmsset_count_marked(tbl);
    size_t total = llmsset_get_size(tbl);
    printf("done, table: %0.1f%% full (%zu nodes).\n", 100.0*(double)filled/total, filled);
}

int test_gc(int threads)
{
    LACE_ME;
    int N_canaries = 16;
    BDD canaries[N_canaries];
    char* hashes[N_canaries];
    char* hashes2[N_canaries];
    int i,j;
    for (i=0;i<N_canaries;i++) {
        canaries[i] = make_random(0, 10);
        hashes[i] = (char*)malloc(80);
        hashes2[i] = (char*)malloc(80);
        sylvan_getsha(canaries[i], hashes[i]);
        sylvan_test_isbdd(canaries[i]);
    }
    test_assert(sylvan_count_refs() == (size_t)N_canaries);
    for (j=0;j<10*threads;j++) {
        CALL(gctest_fill, 6, 5);
        for (i=0;i<N_canaries;i++) {
            sylvan_test_isbdd(canaries[i]);
            sylvan_getsha(canaries[i], hashes2[i]);
            test_assert(strcmp(hashes[i], hashes2[i]) == 0);
        }
    }
    test_assert(sylvan_count_refs() == (size_t)N_canaries);
    return 0;
}

TASK_2(MDD, random_ldd, int, depth, int, count)
{
    uint32_t n[depth];

    MDD result = lddmc_false;

    int i, j;
    for (i=0; i<count; i++) {
        for (j=0; j<depth; j++) {
            n[j] = rng(0, 10);
        }
        //MDD old = result;
        result = lddmc_union_cube(result, n, depth);
        //assert(lddmc_cube(n, depth) != lddmc_true);
        //assert(result == lddmc_union(old, lddmc_cube(n, depth)));
        //assert(result != lddmc_true);
    }

    return result;
}

VOID_TASK_3(enumer, uint32_t*, values, size_t, count, void*, context)
{
    return;
    (void)values;
    (void)count;
    (void)context;
}

int
test_lddmc()
{
    LACE_ME;

    sylvan_init_package(1LL<<24, 1LL<<24, 1LL<<24, 1LL<<24);
    sylvan_init_ldd();
    sylvan_gc_disable();

    MDD a, b, c;

    // Test union, union_cube, member_cube, satcount

    a = lddmc_cube((uint32_t[]){1,2,3,5,4,3}, 6);
    a = lddmc_union(a,lddmc_cube((uint32_t[]){2,2,3,5,4,3}, 6));
    c = b = a = lddmc_union_cube(a, (uint32_t[]){2,2,3,5,4,2}, 6);
    a = lddmc_union_cube(a, (uint32_t[]){2,3,3,5,4,3}, 6);
    a = lddmc_union(a, lddmc_cube((uint32_t[]){2,3,4,4,4,3}, 6));

    test_assert(lddmc_member_cube(a, (uint32_t[]){2,3,3,5,4,3}, 6));
    test_assert(lddmc_member_cube(a, (uint32_t[]){1,2,3,5,4,3}, 6));
    test_assert(lddmc_member_cube(a, (uint32_t[]){2,2,3,5,4,3}, 6));
    test_assert(lddmc_member_cube(a, (uint32_t[]){2,2,3,5,4,2}, 6));

    test_assert(lddmc_satcount(a) == 5);

    lddmc_sat_all_par(a, TASK(enumer), NULL);

    // Test minus, member_cube, satcount

    a = lddmc_minus(a, b);
    test_assert(lddmc_member_cube(a, (uint32_t[]){2,3,3,5,4,3}, 6));
    test_assert(!lddmc_member_cube(a, (uint32_t[]){1,2,3,5,4,3}, 6));
    test_assert(!lddmc_member_cube(a, (uint32_t[]){2,2,3,5,4,3}, 6));
    test_assert(!lddmc_member_cube(a, (uint32_t[]){2,2,3,5,4,2}, 6));
    test_assert(lddmc_member_cube(a, (uint32_t[]){2,3,4,4,4,3}, 6));

    test_assert(lddmc_satcount(a) == 2);

    // Test intersect

    test_assert(lddmc_satcount(lddmc_intersect(a,b)) == 0);
    test_assert(lddmc_intersect(b,c)==lddmc_intersect(c,b));
    test_assert(lddmc_intersect(b,c)==c);

    // Test project, project_minus
    a = lddmc_cube((uint32_t[]){1,2,3,5,4,3}, 6);
    a = lddmc_union_cube(a, (uint32_t[]){2,2,3,5,4,3}, 6);
    a = lddmc_union_cube(a, (uint32_t[]){2,2,3,5,4,2}, 6);
    a = lddmc_union_cube(a, (uint32_t[]){2,3,3,5,4,3}, 6);
    a = lddmc_union_cube(a, (uint32_t[]){2,3,4,4,4,3}, 6);
    // a = {<1,2,3,5,4,3>,<2,2,3,5,4,3>,<2,2,3,5,4,2>,<2,3,3,5,4,3>,<2,3,4,4,4,3>}
    MDD proj = lddmc_cube((uint32_t[]){1,1,-2},3);
    b = lddmc_cube((uint32_t[]){1,2}, 2);
    b = lddmc_union_cube(b, (uint32_t[]){2,2}, 2);
    b = lddmc_union_cube(b, (uint32_t[]){2,3}, 2);
    test_assert(lddmc_project(a, proj)==b);
    test_assert(lddmc_project_minus(a, proj, lddmc_false)==b);
    test_assert(lddmc_project_minus(a, proj, b)==lddmc_false);

    // Test relprod

    a = lddmc_cube((uint32_t[]){1},1);
    b = lddmc_cube((uint32_t[]){1,2},2);
    proj = lddmc_cube((uint32_t[]){1,2,-1}, 3);
    test_assert(lddmc_cube((uint32_t[]){2},1) == lddmc_relprod(a, b, proj));
    test_assert(lddmc_cube((uint32_t[]){3},1) == lddmc_relprod(a, lddmc_cube((uint32_t[]){1,3},2), proj));
    a = lddmc_union_cube(a, (uint32_t[]){2},1);
    test_assert(lddmc_satcount(a) == 2);
    test_assert(lddmc_cube((uint32_t[]){2},1) == lddmc_relprod(a, b, proj));
    b = lddmc_union_cube(b, (uint32_t[]){2,2},2);
    test_assert(lddmc_cube((uint32_t[]){2},1) == lddmc_relprod(a, b, proj));
    b = lddmc_union_cube(b, (uint32_t[]){2,3},2);
    test_assert(lddmc_satcount(lddmc_relprod(a, b, proj)) == 2);
    test_assert(lddmc_union(lddmc_cube((uint32_t[]){2},1),lddmc_cube((uint32_t[]){3},1)) == lddmc_relprod(a, b, proj));

    // Test relprev
    MDD universe = lddmc_union(lddmc_cube((uint32_t[]){1},1), lddmc_cube((uint32_t[]){2},1));
    a = lddmc_cube((uint32_t[]){2},1);
    b = lddmc_cube((uint32_t[]){1,2},2);
    test_assert(lddmc_cube((uint32_t[]){1},1) == lddmc_relprev(a, b, proj, universe));
    test_assert(lddmc_cube((uint32_t[]){1},1) == lddmc_relprev(a, b, proj, lddmc_cube((uint32_t[]){1},1)));
    a = lddmc_cube((uint32_t[]){1},1);
    MDD next = lddmc_relprod(a, b, proj);
    test_assert(lddmc_relprev(next, b, proj, a) == a);

    // Random tests

    MDD rnd1, rnd2;

    int i;
    for (i=0; i<200; i++) {
        int depth = rng(1, 20);
        rnd1 = CALL(random_ldd, depth, rng(0, 30));
        rnd2 = CALL(random_ldd, depth, rng(0, 30));
        test_assert(rnd1 != lddmc_true);
        test_assert(rnd2 != lddmc_true);
        test_assert(lddmc_intersect(rnd1,rnd2) == lddmc_intersect(rnd2,rnd1));
        test_assert(lddmc_union(rnd1,rnd2) == lddmc_union(rnd2,rnd1));
        MDD tmp = lddmc_union(lddmc_minus(rnd1, rnd2), lddmc_minus(rnd2, rnd1));
        test_assert(lddmc_intersect(tmp, lddmc_intersect(rnd1, rnd2)) == lddmc_false);
        test_assert(lddmc_union(tmp, lddmc_intersect(rnd1, rnd2)) == lddmc_union(rnd1, rnd2));
        test_assert(lddmc_minus(rnd1,rnd2) == lddmc_minus(rnd1, lddmc_intersect(rnd1,rnd2)));
    }

    // Test file stuff
    for (i=0; i<10; i++) {
        FILE *f = fopen("__lddmc_test_bdd", "w+");
        int N = 20;
        MDD rnd[N];
        size_t a[N];
        char sha[N][65];
        int j;
        for (j=0;j<N;j++) rnd[j] = CALL(random_ldd, 5, 500);
        for (j=0;j<N;j++) lddmc_getsha(rnd[j], sha[j]);
        for (j=0;j<N;j++) { a[j] = lddmc_serialize_add(rnd[j]); lddmc_serialize_tofile(f); }
        for (j=0;j<N;j++) test_assert(a[j] == lddmc_serialize_get(rnd[j]));
        for (j=0;j<N;j++) test_assert(rnd[j] == lddmc_serialize_get_reversed(a[j]));
        fseek(f, 0, SEEK_SET);
        lddmc_serialize_reset();

        sylvan_quit();
        sylvan_init_package(1LL<<24, 1LL<<24, 1LL<<24, 1LL<<24);
        sylvan_init_ldd();
        sylvan_gc_disable();

        for (j=0;j<N;j++) lddmc_serialize_fromfile(f);
        fclose(f);
        unlink("__lddmc_test_bdd");

        for (j=0;j<N;j++) rnd[j] = lddmc_serialize_get_reversed(a[j]);
        char sha2[N][65];
        for (j=0;j<N;j++) lddmc_getsha(rnd[j], sha2[j]);
        for (j=0;j<N;j++) test_assert(memcmp(sha[j], sha2[j], 64)==0);
    
        lddmc_serialize_reset();
    }

    sylvan_quit();
    return 0;
}

int runtests(int threads)
{
    lace_init(threads, 100000);
    lace_startup(0, NULL, NULL);

    printf(BOLD "Testing LDDMC... ");
    fflush(stdout);
    if (test_lddmc()) return 1;
    printf(LGREEN "success" NC "!\n");

    printf(NC "Testing garbage collection... ");
    fflush(stdout);
    sylvan_init_package(1LL<<14, 1LL<<14, 1LL<<20, 1LL<<20);
    sylvan_init_bdd(1);
    sylvan_gc_enable();
    if (test_gc(threads)) return 1;
    sylvan_quit();
    printf(LGREEN "success" NC "!\n");

    lace_exit();
    return 0;
}

int main(int argc, char **argv)
{
    int threads = 2;
    if (argc > 1) sscanf(argv[1], "%d", &threads);

    if (runtests(threads)) exit(1);
    printf(NC);
    exit(0);
}
