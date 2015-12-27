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


void testFun(BDD p1, BDD p2, BDD r1, BDD r2)
{
    if (r1 == r2) return;

    printf("Parameter 1:\n");
    fflush(stdout);
    sylvan_printdot(p1);
    sylvan_print(p1);printf("\n");

    printf("Parameter 2:\n");
    fflush(stdout);
    sylvan_printdot(p2);
    sylvan_print(p2);printf("\n");

    printf("Result 1:\n");
    fflush(stdout);
    sylvan_printdot(r1);

    printf("Result 2:\n");
    fflush(stdout);
    sylvan_printdot(r2);

    assert(0);
}

int testEqual(BDD a, BDD b)
{
	if (a == b) return 1;

	if (a == sylvan_invalid) {
		printf("a is invalid!\n");
		return 0;
	}

	if (b == sylvan_invalid) {
		printf("b is invalid!\n");
		return 0;
	}

    printf("Not Equal!\n");
    fflush(stdout);

    sylvan_print(a);printf("\n");
    sylvan_print(b);printf("\n");

	return 0;
}

void
test_bdd()
{
    sylvan_gc_disable();

    assert(sylvan_makenode(sylvan_ithvar(1), sylvan_true, sylvan_true) == sylvan_not(sylvan_makenode(sylvan_ithvar(1), sylvan_false, sylvan_false)));
    assert(sylvan_makenode(sylvan_ithvar(1), sylvan_false, sylvan_true) == sylvan_not(sylvan_makenode(sylvan_ithvar(1), sylvan_true, sylvan_false)));
    assert(sylvan_makenode(sylvan_ithvar(1), sylvan_true, sylvan_false) == sylvan_not(sylvan_makenode(sylvan_ithvar(1), sylvan_false, sylvan_true)));
    assert(sylvan_makenode(sylvan_ithvar(1), sylvan_false, sylvan_false) == sylvan_not(sylvan_makenode(sylvan_ithvar(1), sylvan_true, sylvan_true)));

    sylvan_gc_enable();
}

void
test_cube()
{
    LACE_ME;
    BDDSET vars = sylvan_set_fromarray(((BDDVAR[]){1,2,3,4,6,8}), 6);

    uint8_t cube[6], check[6];
    int i, j;
    for (i=0;i<6;i++) cube[i] = rng(0,3);
    BDD bdd = sylvan_cube(vars, cube);

    sylvan_sat_one(bdd, vars, check);
    for (i=0; i<6;i++) assert(cube[i] == check[i] || cube[i] == 2 && check[i] == 0);

    BDD picked = sylvan_pick_cube(bdd);
    assert(testEqual(sylvan_and(picked, bdd), picked));

    BDD t1 = sylvan_cube(vars, ((uint8_t[]){1,1,2,2,0,0}));
    BDD t2 = sylvan_cube(vars, ((uint8_t[]){1,1,1,0,0,2}));
    assert(testEqual(sylvan_union_cube(t1, vars, ((uint8_t[]){1,1,1,0,0,2})), sylvan_or(t1, t2)));
    t2 = sylvan_cube(vars, ((uint8_t[]){2,2,2,1,1,0}));
    assert(testEqual(sylvan_union_cube(t1, vars, ((uint8_t[]){2,2,2,1,1,0})), sylvan_or(t1, t2)));
    t2 = sylvan_cube(vars, ((uint8_t[]){1,1,1,0,0,0}));
    assert(testEqual(sylvan_union_cube(t1, vars, ((uint8_t[]){1,1,1,0,0,0})), sylvan_or(t1, t2)));

    sylvan_gc_disable();
    bdd = make_random(1, 16);
    for (j=0;j<10;j++) {
        for (i=0;i<6;i++) cube[i] = rng(0,3);
        BDD c = sylvan_cube(vars, cube);
        assert(sylvan_union_cube(bdd, vars, cube) == sylvan_or(bdd, c));
    }

    for (i=0;i<10;i++) {
        picked = sylvan_pick_cube(bdd);
        assert(testEqual(sylvan_and(picked, bdd), picked));
    }
    sylvan_gc_enable();
}

static void
test_operators()
{
    // We need to test: xor, and, or, nand, nor, imp, biimp, invimp, diff, less
    sylvan_gc_disable();

    LACE_ME;

    //int i;
    BDD a = sylvan_ithvar(1);
    BDD b = sylvan_ithvar(2);
    BDD one = make_random(1, 12);
    BDD two = make_random(6, 24);

    // Test or
    assert(testEqual(sylvan_or(a, b), sylvan_makenode(1, b, sylvan_true)));
    assert(testEqual(sylvan_or(a, b), sylvan_or(b, a)));
    assert(testEqual(sylvan_or(one, two), sylvan_or(two, one)));

    // Test and
    assert(testEqual(sylvan_and(a, b), sylvan_makenode(1, sylvan_false, b)));
    assert(testEqual(sylvan_and(a, b), sylvan_and(b, a)));
    assert(testEqual(sylvan_and(one, two), sylvan_and(two, one)));

    // Test xor
    assert(testEqual(sylvan_xor(a, b), sylvan_makenode(1, b, sylvan_not(b))));
    assert(testEqual(sylvan_xor(a, b), sylvan_xor(a, b)));
    assert(testEqual(sylvan_xor(a, b), sylvan_xor(b, a)));
    assert(testEqual(sylvan_xor(one, two), sylvan_xor(two, one)));
    assert(testEqual(sylvan_xor(a, b), sylvan_ite(a, sylvan_not(b), b)));

    // Test diff
    assert(testEqual(sylvan_diff(a, b), sylvan_diff(a, b)));
    assert(testEqual(sylvan_diff(a, b), sylvan_diff(a, sylvan_and(a, b))));
    assert(testEqual(sylvan_diff(a, b), sylvan_and(a, sylvan_not(b))));
    assert(testEqual(sylvan_diff(a, b), sylvan_ite(b, sylvan_false, a)));
    assert(testEqual(sylvan_diff(one, two), sylvan_diff(one, two)));
    assert(testEqual(sylvan_diff(one, two), sylvan_diff(one, sylvan_and(one, two))));
    assert(testEqual(sylvan_diff(one, two), sylvan_and(one, sylvan_not(two))));
    assert(testEqual(sylvan_diff(one, two), sylvan_ite(two, sylvan_false, one)));

    // Test biimp
    assert(testEqual(sylvan_biimp(a, b), sylvan_makenode(1, sylvan_not(b), b)));
    assert(testEqual(sylvan_biimp(a, b), sylvan_biimp(b, a)));
    assert(testEqual(sylvan_biimp(one, two), sylvan_biimp(two, one)));

    // Test nand / and
    assert(testEqual(sylvan_not(sylvan_and(a, b)), sylvan_nand(b, a)));
    assert(testEqual(sylvan_not(sylvan_and(one, two)), sylvan_nand(two, one)));

    // Test nor / or
    assert(testEqual(sylvan_not(sylvan_or(a, b)), sylvan_nor(b, a)));
    assert(testEqual(sylvan_not(sylvan_or(one, two)), sylvan_nor(two, one)));

    // Test xor / biimp
    assert(testEqual(sylvan_xor(a, b), sylvan_not(sylvan_biimp(b, a))));
    assert(testEqual(sylvan_xor(one, two), sylvan_not(sylvan_biimp(two, one))));

    // Test imp
    assert(testEqual(sylvan_imp(a, b), sylvan_ite(a, b, sylvan_true)));
    assert(testEqual(sylvan_imp(one, two), sylvan_ite(one, two, sylvan_true)));
    assert(testEqual(sylvan_imp(one, two), sylvan_not(sylvan_diff(one, two))));
    assert(testEqual(sylvan_invimp(one, two), sylvan_not(sylvan_less(one, two))));
    assert(testEqual(sylvan_imp(a, b), sylvan_invimp(b, a)));
    assert(testEqual(sylvan_imp(one, two), sylvan_invimp(two, one)));

    // Test constrain, exists and forall

    sylvan_gc_enable();
}

static void
test_relprod()
{
    LACE_ME;

    sylvan_gc_disable();

    BDDVAR vars[] = {0,2,4};
    BDDVAR all_vars[] = {0,1,2,3,4,5};

    BDDSET vars_set = sylvan_set_fromarray(vars, 3);
    BDDSET all_vars_set = sylvan_set_fromarray(all_vars, 6);

    BDD s, t, next, prev;
    BDD zeroes, ones;

    // transition relation: 000 --> 111 and !000 --> 000
    t = sylvan_false;
    t = sylvan_union_cube(t, all_vars_set, ((uint8_t[]){0,1,0,1,0,1}));
    t = sylvan_union_cube(t, all_vars_set, ((uint8_t[]){1,0,2,0,2,0}));
    t = sylvan_union_cube(t, all_vars_set, ((uint8_t[]){2,0,1,0,2,0}));
    t = sylvan_union_cube(t, all_vars_set, ((uint8_t[]){2,0,2,0,1,0}));

    s = sylvan_cube(vars_set, (uint8_t[]){0,0,1});
    zeroes = sylvan_cube(vars_set, (uint8_t[]){0,0,0});
    ones = sylvan_cube(vars_set, (uint8_t[]){1,1,1});

    next = sylvan_relnext(s, t, all_vars_set);
    prev = sylvan_relprev(t, next, all_vars_set);
    assert(next == zeroes);
    assert(prev == sylvan_not(zeroes));

    next = sylvan_relnext(next, t, all_vars_set);
    prev = sylvan_relprev(t, next, all_vars_set);
    assert(next == ones);
    assert(prev == zeroes);

    t = sylvan_cube(all_vars_set, (uint8_t[]){0,0,0,0,0,1});
    assert(sylvan_relprev(t, s, all_vars_set) == zeroes);
    assert(sylvan_relprev(t, sylvan_not(s), all_vars_set) == sylvan_false);
    assert(sylvan_relnext(s, t, all_vars_set) == sylvan_false);
    assert(sylvan_relnext(zeroes, t, all_vars_set) == s);

    t = sylvan_cube(all_vars_set, (uint8_t[]){0,0,0,0,0,2});
    assert(sylvan_relprev(t, s, all_vars_set) == zeroes);
    assert(sylvan_relprev(t, zeroes, all_vars_set) == zeroes);
    assert(sylvan_relnext(sylvan_not(zeroes), t, all_vars_set) == sylvan_false);

    sylvan_gc_enable();
}

static void
test_compose()
{
    sylvan_gc_disable();

    LACE_ME;

    BDD a = sylvan_ithvar(1);
    BDD b = sylvan_ithvar(2);

    BDD a_or_b = sylvan_or(a, b);

    BDD one = make_random(3, 16);
    BDD two = make_random(8, 24);

    BDDMAP map = sylvan_map_empty();

    map = sylvan_map_add(map, 1, one);
    map = sylvan_map_add(map, 2, two);

    assert(sylvan_map_key(map) == 1);
    assert(sylvan_map_value(map) == one);
    assert(sylvan_map_key(sylvan_map_next(map)) == 2);
    assert(sylvan_map_value(sylvan_map_next(map)) == two);

    assert(testEqual(one, sylvan_compose(a, map)));
    assert(testEqual(two, sylvan_compose(b, map)));

    assert(testEqual(sylvan_or(one, two), sylvan_compose(a_or_b, map)));

    map = sylvan_map_add(map, 2, one);
    assert(testEqual(sylvan_compose(a_or_b, map), one));

    map = sylvan_map_add(map, 1, two);
    assert(testEqual(sylvan_or(one, two), sylvan_compose(a_or_b, map)));

    assert(testEqual(sylvan_and(one, two), sylvan_compose(sylvan_and(a, b), map)));

    sylvan_gc_enable();
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

void test_gc(int threads)
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
    assert(sylvan_count_refs() == (size_t)N_canaries);
    for (j=0;j<10*threads;j++) {
        CALL(gctest_fill, 6, 5);
        for (i=0;i<N_canaries;i++) {
            sylvan_test_isbdd(canaries[i]);
            sylvan_getsha(canaries[i], hashes2[i]);
            assert(strcmp(hashes[i], hashes2[i]) == 0);
        }
    }
    assert(sylvan_count_refs() == (size_t)N_canaries);
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
        MDD old = result;
        result = lddmc_union_cube(result, n, depth);
        assert(lddmc_cube(n, depth) != lddmc_true);
        assert(result == lddmc_union(old, lddmc_cube(n, depth)));
        assert(result != lddmc_true);
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

void
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

    assert(lddmc_member_cube(a, (uint32_t[]){2,3,3,5,4,3}, 6));
    assert(lddmc_member_cube(a, (uint32_t[]){1,2,3,5,4,3}, 6));
    assert(lddmc_member_cube(a, (uint32_t[]){2,2,3,5,4,3}, 6));
    assert(lddmc_member_cube(a, (uint32_t[]){2,2,3,5,4,2}, 6));

    assert(lddmc_satcount(a) == 5);

    lddmc_sat_all_par(a, TASK(enumer), NULL);

    // Test minus, member_cube, satcount

    a = lddmc_minus(a, b);
    assert(lddmc_member_cube(a, (uint32_t[]){2,3,3,5,4,3}, 6));
    assert(!lddmc_member_cube(a, (uint32_t[]){1,2,3,5,4,3}, 6));
    assert(!lddmc_member_cube(a, (uint32_t[]){2,2,3,5,4,3}, 6));
    assert(!lddmc_member_cube(a, (uint32_t[]){2,2,3,5,4,2}, 6));
    assert(lddmc_member_cube(a, (uint32_t[]){2,3,4,4,4,3}, 6));

    assert(lddmc_satcount(a) == 2);

    // Test intersect

    assert(lddmc_satcount(lddmc_intersect(a,b)) == 0);
    assert(lddmc_intersect(b,c)==lddmc_intersect(c,b));
    assert(lddmc_intersect(b,c)==c);

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
    assert(lddmc_project(a, proj)==b);
    assert(lddmc_project_minus(a, proj, lddmc_false)==b);
    assert(lddmc_project_minus(a, proj, b)==lddmc_false);

    // Test relprod

    a = lddmc_cube((uint32_t[]){1},1);
    b = lddmc_cube((uint32_t[]){1,2},2);
    proj = lddmc_cube((uint32_t[]){1,2,-1}, 3);
    assert(lddmc_cube((uint32_t[]){2},1) == lddmc_relprod(a, b, proj));
    assert(lddmc_cube((uint32_t[]){3},1) == lddmc_relprod(a, lddmc_cube((uint32_t[]){1,3},2), proj));
    a = lddmc_union_cube(a, (uint32_t[]){2},1);
    assert(lddmc_satcount(a) == 2);
    assert(lddmc_cube((uint32_t[]){2},1) == lddmc_relprod(a, b, proj));
    b = lddmc_union_cube(b, (uint32_t[]){2,2},2);
    assert(lddmc_cube((uint32_t[]){2},1) == lddmc_relprod(a, b, proj));
    b = lddmc_union_cube(b, (uint32_t[]){2,3},2);
    assert(lddmc_satcount(lddmc_relprod(a, b, proj)) == 2);
    assert(lddmc_union(lddmc_cube((uint32_t[]){2},1),lddmc_cube((uint32_t[]){3},1)) == lddmc_relprod(a, b, proj));

    // Test relprev
    MDD universe = lddmc_union(lddmc_cube((uint32_t[]){1},1), lddmc_cube((uint32_t[]){2},1));
    a = lddmc_cube((uint32_t[]){2},1);
    b = lddmc_cube((uint32_t[]){1,2},2);
    assert(lddmc_cube((uint32_t[]){1},1) == lddmc_relprev(a, b, proj, universe));
    assert(lddmc_cube((uint32_t[]){1},1) == lddmc_relprev(a, b, proj, lddmc_cube((uint32_t[]){1},1)));
    a = lddmc_cube((uint32_t[]){1},1);
    MDD next = lddmc_relprod(a, b, proj);
    assert(lddmc_relprev(next, b, proj, a) == a);

    // Random tests

    MDD rnd1, rnd2;

    int i;
    for (i=0; i<200; i++) {
        int depth = rng(1, 20);
        rnd1 = CALL(random_ldd, depth, rng(0, 30));
        rnd2 = CALL(random_ldd, depth, rng(0, 30));
        assert(rnd1 != lddmc_true);
        assert(rnd2 != lddmc_true);
        assert(lddmc_intersect(rnd1,rnd2) == lddmc_intersect(rnd2,rnd1));
        assert(lddmc_union(rnd1,rnd2) == lddmc_union(rnd2,rnd1));
        MDD tmp = lddmc_union(lddmc_minus(rnd1, rnd2), lddmc_minus(rnd2, rnd1));
        assert(lddmc_intersect(tmp, lddmc_intersect(rnd1, rnd2)) == lddmc_false);
        assert(lddmc_union(tmp, lddmc_intersect(rnd1, rnd2)) == lddmc_union(rnd1, rnd2));
        assert(lddmc_minus(rnd1,rnd2) == lddmc_minus(rnd1, lddmc_intersect(rnd1,rnd2)));
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
        for (j=0;j<N;j++) assert(a[j] == lddmc_serialize_get(rnd[j]));
        for (j=0;j<N;j++) assert(rnd[j] == lddmc_serialize_get_reversed(a[j]));
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
        for (j=0;j<N;j++) assert(memcmp(sha[j], sha2[j], 64)==0);
    
        lddmc_serialize_reset();
    }

    sylvan_quit();
}

void runtests(int threads)
{
    lace_init(threads, 100000);
    lace_startup(0, NULL, NULL);

    printf(BOLD "Testing LDDMC... ");
    fflush(stdout);
    test_lddmc();
    printf(LGREEN "success" NC "!\n");

    printf(BOLD "Testing Sylvan\n");

    printf(NC "Testing basic bdd functionality... ");
    fflush(stdout);
    sylvan_init_package(1LL<<16, 1LL<<16, 1LL<<16, 1LL<<16);
    sylvan_init_bdd(1);
    test_bdd();
    sylvan_quit();
    printf(LGREEN "success" NC "!\n");

    // what happens if we make a cube
    printf(NC "Testing cube function... ");
    fflush(stdout);
    int j;
    sylvan_init_package(1LL<<24, 1LL<<24, 1LL<<24, 1LL<<24);
    sylvan_init_bdd(1);
    for (j=0;j<20;j++) test_cube();
    sylvan_quit();
    printf(LGREEN "success" NC "!\n");

    printf(NC "Testing relational products... ");
    fflush(stdout);
    sylvan_init_package(1LL<<24, 1LL<<24, 1LL<<24, 1LL<<24);
    sylvan_init_bdd(1);
    for (j=0;j<20;j++) test_relprod();
    sylvan_quit();
    printf(LGREEN "success" NC "!\n");

    printf(NC "Testing function composition... ");
    fflush(stdout);
    sylvan_init_package(1LL<<24, 1LL<<24, 1LL<<24, 1LL<<24);
    sylvan_init_bdd(1);
    for (j=0;j<20;j++) test_compose();
    sylvan_quit();
    printf(LGREEN "success" NC "!\n");

    printf(NC "Testing garbage collection... ");
    fflush(stdout);
    sylvan_init_package(1LL<<14, 1LL<<14, 1LL<<20, 1LL<<20);
    sylvan_init_bdd(1);
    test_gc(threads);
    sylvan_quit();
    printf(LGREEN "success" NC "!\n");

    printf(NC "Testing operators... ");
    fflush(stdout);
    sylvan_init_package(1LL<<24, 1LL<<24, 1LL<<24, 1LL<<24);
    sylvan_init_bdd(1);
    for (j=0;j<20;j++) test_operators();
    sylvan_quit();
    printf(LGREEN "success" NC "!\n");

    lace_exit();
}

int main(int argc, char **argv)
{
    int threads = 2;
    if (argc > 1) sscanf(argv[1], "%d", &threads);

    runtests(threads);
    printf(NC);
    exit(0);
}
