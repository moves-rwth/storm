#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sylvan.h"
#include "sylvan_int.h"

#include "test_assert.h"

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
    return seed * (1.0 / ((double)(UINT64_MAX) + 1.0));
}

int
rng(int low, int high)
{
    return low + uniform_deviate(xorshift_rand()) * (high-low);
}

/**
 * Some infrastructure to test evaluation of ZDDs
 */

uint8_t **enum_arrs;
size_t enum_len;
int enum_idx;
int enum_max;

VOID_TASK_3(test_zdd_enum_cb, void*, ctx, uint8_t*, arr, size_t, len)
{
    assert(len == enum_len);
    assert(enum_idx != enum_max);
    assert(memcmp(arr, enum_arrs[enum_idx++], len) == 0);
    (void)ctx;
    (void)arr;
    (void)len;
}

TASK_0(int, test_zdd_eval)
{
    /**
     * Test zdd_from_mtbdd and zdd_eval
     */

    /**
     * Create simple MTBDD domain and singleton set
     */

    BDD dom = mtbdd_fromarray((uint32_t[]){0,1,2,3,4,5,6}, 7);
    BDD dd = mtbdd_cube(dom, (uint8_t[]){0,0,2,2,0,2,0}, mtbdd_true);

    /**
     * Convert to ZDD domain and set
     */

    ZDD zdd_dom = zdd_set_from_mtbdd(dom);
    ZDD zdd = zdd_from_mtbdd(dd, dom);

    test_assert(zdd_dom == zdd_set_from_array((uint32_t[]){0,1,2,3,4,5,6}, 7));

    /**
     * We just test if it evaluates correctly.
     */

    test_assert(zdd_eval(zdd, 0, 1) == zdd_false);
    test_assert(zdd_eval(zdd, 0, 0) != zdd_false);
    zdd = zdd_eval(zdd, 0, 0);
    test_assert(zdd_eval(zdd, 1, 1) == zdd_false);
    test_assert(zdd_eval(zdd, 1, 0) != zdd_false);
    zdd = zdd_eval(zdd, 1, 0);
    test_assert(zdd_eval(zdd, 2, 1) == zdd_eval(zdd, 2, 0));
    zdd = zdd_eval(zdd, 2, 0);
    test_assert(zdd_eval(zdd, 3, 1) == zdd_eval(zdd, 3, 0));
    zdd = zdd_eval(zdd, 3, 1);
    test_assert(zdd_eval(zdd, 4, 1) == zdd_false);
    test_assert(zdd_eval(zdd, 4, 0) != zdd_false);
    zdd = zdd_eval(zdd, 4, 0);
    test_assert(zdd_eval(zdd, 5, 1) == zdd_eval(zdd, 5, 0));
    zdd = zdd_eval(zdd, 5, 0);
    test_assert(zdd_eval(zdd, 6, 1) == zdd_false);
    test_assert(zdd_eval(zdd, 6, 0) != zdd_false);

    return 0;
}

TASK_0(int, test_zdd_ithvar)
{
    /**
     * Test zdd_ithvar
     */

    uint32_t var = rng(0, 0xfffff);
    ZDD a = zdd_makenode(var, zdd_false, zdd_true);
    test_assert(a == zdd_ithvar(var));
    test_assert(a == zdd_from_mtbdd(sylvan_ithvar(var), sylvan_ithvar(var)));

    return 0;
}

TASK_0(int, test_zdd_from_mtbdd)
{
    /**
     * Test zdd_from_mtbdd, zdd_to_mtbdd and zdd_cube with random sets
     */

    BDD bdd_dom = mtbdd_fromarray((uint32_t[]){0,1,2,3,4,5,6,7}, 8);
    ZDD zdd_dom = zdd_set_from_array((uint32_t[]){0,1,2,3,4,5,6,7}, 8);
    test_assert(zdd_set_from_mtbdd(bdd_dom) == zdd_dom);

    int count = rng(10,100);
    for (int i=0; i<count; i++) {
        uint8_t arr[8];
        for (int j=0; j<8; j++) arr[j] = rng(0, 2);
        BDD bdd_set = sylvan_cube(bdd_dom, arr);
        ZDD zdd_set = zdd_cube(zdd_dom, arr, zdd_true);
        test_assert(zdd_from_mtbdd(bdd_set, bdd_dom) == zdd_set);
        test_assert(zdd_to_mtbdd(zdd_set, zdd_dom) == bdd_set);
    }

    return 0;
}

TASK_0(int, test_zdd_merge_domains)
{
    /*
     * Test zdd_merge_domains with random sets
     */

    // Create random domain of 6..14 variables
    int nvars = rng(20,50);

    // Create random subdomain 1
    uint32_t subdom1_arr[nvars];
    int nsub1 = 0;
    for (int i=0; i<nvars; i++) if (rng(0,2)) subdom1_arr[nsub1++] = i;
    BDD bdd_subdom1 = mtbdd_fromarray(subdom1_arr, nsub1);
    ZDD zdd_subdom1 = zdd_set_from_array(subdom1_arr, nsub1);
    test_assert(zdd_subdom1 == zdd_set_from_mtbdd(bdd_subdom1));

    // Create random subdomain 2
    uint32_t subdom2_arr[nvars];
    int nsub2 = 0;
    for (int i=0; i<nvars; i++) if (rng(0,2)) subdom2_arr[nsub2++] = i;
    BDD bdd_subdom2 = mtbdd_fromarray(subdom2_arr, nsub2);
    ZDD zdd_subdom2 = zdd_set_from_array(subdom2_arr, nsub2);
    test_assert(zdd_subdom2 == zdd_set_from_mtbdd(bdd_subdom2));

    // combine subdomains
    BDD bdd_subdom = sylvan_and(bdd_subdom1, bdd_subdom2);
    ZDD zdd_subdom = zdd_set_union(zdd_subdom1, zdd_subdom2);
    test_assert(zdd_subdom == zdd_set_from_mtbdd(bdd_subdom));

    return 0;
}

// TASK_0(int, test_zdd_extend_domain)
// {
//     /**
//      * Test zdd_extend_domain with random sets
//      */
// 
//     // Create random domain of 6..14 variables
//     int nvars = rng(6,14);
//     uint32_t dom_arr[nvars];
//     for (int i=0; i<nvars; i++) dom_arr[i] = i;
//     BDD bdd_dom = mtbdd_fromarray(dom_arr, nvars);
//     ZDD zdd_dom = zdd_from_array(dom_arr, nvars);
//     test_assert(zdd_dom == zdd_from_mtbdd(bdd_dom, bdd_dom));
// 
//     // Create random subdomain
//     uint32_t subdom_arr[nvars];
//     int nsub = 0;
//     for (int i=0; i<nvars; i++) if (rng(0,2)) subdom_arr[nsub++] = i;
//     BDD bdd_subdom = mtbdd_fromarray(subdom_arr, nsub);
//     ZDD zdd_subdom = zdd_from_array(subdom_arr, nsub);
//     test_assert(zdd_subdom == zdd_from_mtbdd(bdd_subdom, bdd_subdom));
// 
//     // Create random set on subdomain
//     BDD bdd_set = sylvan_false;
//     ZDD zdd_set = zdd_false;
//     {
//         int count = rng(10,200);
//         for (int i=0; i<count; i++) {
//             uint8_t arr[nsub];
//             for (int j=0; j<nsub; j++) arr[j] = rng(0, 2);
//             bdd_set = sylvan_union_cube(bdd_set, bdd_subdom, arr);
//             zdd_set = zdd_union_cube(zdd_set, zdd_subdom, arr);
//         }
//     }
//     test_assert(zdd_set == zdd_from_mtbdd(bdd_set, bdd_subdom));
// 
//     ZDD zdd_test_result = zdd_extend_domain(zdd_set, zdd_subdom, zdd_dom);
//     test_assert(zdd_test_result == zdd_from_mtbdd(bdd_set, bdd_dom));
// 
//     return 0;
// }

TASK_0(int, test_zdd_union_cube)
{
    /**
     * Test zdd_union_cube with random sets
     * This also tests zdd_from_mtbdd...
     */

    BDD bdd_dom = mtbdd_fromarray((uint32_t[]){0,1,2,3,4,5,6,7}, 8);
    ZDD zdd_dom = zdd_set_from_array((uint32_t[]){0,1,2,3,4,5,6,7}, 8);

    BDD bdd_set = sylvan_false;
    ZDD zdd_set = zdd_false;
    int count = rng(100,1000);
    for (int i=0; i<count; i++) {
        uint8_t arr[8];
        for (int j=0; j<8; j++) arr[j] = rng(0, 3);
        bdd_set = sylvan_union_cube(bdd_set, bdd_dom, arr);
        zdd_set = zdd_union_cube(zdd_set, zdd_dom, arr, zdd_true);
        test_assert(zdd_from_mtbdd(bdd_set, bdd_dom) == zdd_set);
    }

    return 0;
}

TASK_0(int, test_zdd_satcount)
{
    /**
     * Test zdd_satcount with random sets
     * This also tests zdd_from_mtbdd...
     */

    BDD bdd_dom = mtbdd_fromarray((uint32_t[]){0,1,2,3,4,5,6,7}, 8);

    int count = rng(0,100);
    BDD bdd_set = sylvan_false;
    for (int i=0; i<count; i++) {
        uint8_t arr[8];
        for (int j=0; j<8; j++) arr[j] = rng(0, 2);
        bdd_set = sylvan_union_cube(bdd_set, bdd_dom, arr);
    }

    ZDD zdd_set = zdd_from_mtbdd(bdd_set, bdd_dom);

    test_assert((size_t)mtbdd_satcount(bdd_set, 8) == (size_t)zdd_satcount(zdd_set));

    return 0;
}

TASK_0(int, test_zdd_enum)
{
    /**
     * Test zdd_enum with random sets
     */

    int nvars = rng(8,12);
    uint8_t arr[nvars];

    // Create random source set
    uint32_t dom_arr[nvars];
    for (int i=0; i<nvars; i++) dom_arr[i] = i*2;
    ZDD zdd_dom = zdd_set_from_array(dom_arr, nvars);

    ZDD zdd_set = zdd_false;
    int count = rng(0,1000);
    for (int i=0; i<count; i++) {
        for (int j=0; j<nvars; j++) arr[j] = rng(0, 2);
        zdd_set = zdd_union_cube(zdd_set, zdd_dom, arr, zdd_true);
    }

    enum_max = zdd_satcount(zdd_set);
    enum_len = nvars;
    enum_idx = 0;

    enum_arrs = malloc(sizeof(uint8_t*[enum_max]));
    ZDD res = zdd_enum_first(zdd_set, zdd_dom, arr, NULL);
    for (int i=0; i<enum_max; i++) {
        test_assert(res != zdd_false);
        enum_arrs[i] = malloc(sizeof(uint8_t[nvars]));
        memcpy(enum_arrs[i], arr, nvars);
        res = zdd_enum_next(zdd_set, zdd_dom, arr, NULL);
    }
    assert(res == zdd_false);

    // zdd_enum_seq(zdd_set, zdd_dom, TASK(test_zdd_enum_cb), NULL);
    for (int i=0; i<enum_max; i++) free(enum_arrs[i]);
    free(enum_arrs);

    return 0;
}

TASK_0(int, test_zdd_and)
{
    /**
     * Test zdd_and with random sets
     */

    // Create random domain of 6..14 variables
    int nvars = rng(6,14);
    uint32_t dom_arr[nvars];
    for (int i=0; i<nvars; i++) dom_arr[i] = i;
    BDD bdd_dom = mtbdd_fromarray(dom_arr, nvars);

    BDD bdd_set_a = sylvan_false;
    BDD bdd_set_b = sylvan_false;

    int count = rng(0,100);
    for (int i=0; i<count; i++) {
        uint8_t arr[nvars];
        for (int j=0; j<nvars; j++) arr[j] = rng(0, 2);
        bdd_set_a = sylvan_union_cube(bdd_set_a, bdd_dom, arr);
        for (int j=0; j<nvars; j++) arr[j] = rng(0, 2);
        bdd_set_b = sylvan_union_cube(bdd_set_b, bdd_dom, arr);
    }

    BDD bdd_set = sylvan_and(bdd_set_a, bdd_set_b);

    ZDD zdd_set_a = zdd_from_mtbdd(bdd_set_a, bdd_dom);
    ZDD zdd_set_b = zdd_from_mtbdd(bdd_set_b, bdd_dom);
    ZDD zdd_set = zdd_from_mtbdd(bdd_set, bdd_dom);
    // ZDD zdd_dom = zdd_set_from_mtbdd(bdd_dom);

    ZDD zdd_test_result = zdd_and(zdd_set_a, zdd_set_b);
    
    test_assert(zdd_set == zdd_test_result);

    return 0;
}

TASK_0(int, test_zdd_or)
{
    /**
     * Test zdd_or with random sets
     */

    // Create random domain of 6..14 variables
    int nvars = rng(6,14);
    uint32_t dom_arr[nvars];
    for (int i=0; i<nvars; i++) dom_arr[i] = i;
    BDD bdd_dom = mtbdd_fromarray(dom_arr, nvars);

    BDD bdd_set_a = sylvan_false;
    BDD bdd_set_b = sylvan_false;

    int count = rng(0,100);
    for (int i=0; i<count; i++) {
        uint8_t arr[nvars];
        for (int j=0; j<nvars; j++) arr[j] = rng(0, 2);
        bdd_set_a = sylvan_union_cube(bdd_set_a, bdd_dom, arr);
        for (int j=0; j<nvars; j++) arr[j] = rng(0, 2);
        bdd_set_b = sylvan_union_cube(bdd_set_b, bdd_dom, arr);
    }

    BDD bdd_set = sylvan_or(bdd_set_a, bdd_set_b);

    ZDD zdd_set_a = zdd_from_mtbdd(bdd_set_a, bdd_dom);
    ZDD zdd_set_b = zdd_from_mtbdd(bdd_set_b, bdd_dom);
    ZDD zdd_set = zdd_from_mtbdd(bdd_set, bdd_dom);
    //ZDD zdd_dom = zdd_from_mtbdd(bdd_dom, bdd_dom);

    ZDD zdd_test_result = zdd_or(zdd_set_a, zdd_set_b);

    test_assert(zdd_set == zdd_test_result);

    return 0;
}

TASK_0(int, test_zdd_not)
{
    /**
     * Test negation with random sets
     */

    BDD bdd_dom = mtbdd_fromarray((uint32_t[]){0,1,2,3,4,5,6,7}, 8);

    int count = rng(0,100);
    BDD bdd_set = sylvan_false;
    for (int i=0; i<count; i++) {
        uint8_t arr[8];
        for (int j=0; j<8; j++) arr[j] = rng(0, 2);
        bdd_set = sylvan_union_cube(bdd_set, bdd_dom, arr);
    }

    ZDD zdd_set = zdd_from_mtbdd(bdd_set, bdd_dom);
    ZDD zdd_set_inv = zdd_from_mtbdd(sylvan_not(bdd_set), bdd_dom);
    ZDD zdd_dom = zdd_set_from_mtbdd(bdd_dom);

    test_assert((size_t)mtbdd_satcount(sylvan_not(bdd_set), 8) == (size_t)zdd_satcount(zdd_set_inv));
    test_assert(zdd_set_inv == zdd_not(zdd_set, zdd_dom));

    return 0;
}

TASK_0(int, test_zdd_ite)
{
    /**
     * Test zdd_ite with random sets
     */

    // Create random domain of 6..12 variables
    int nvars = rng(6, 12);
    uint32_t dom_arr[nvars];
    for (int i=0; i<nvars; i++) dom_arr[i] = i;
    BDD bdd_dom = mtbdd_fromarray(dom_arr, nvars);

    // Create three random sets
    BDD set_a, set_b, set_c;
    set_a = set_b = set_c = mtbdd_false;
    uint8_t arr[nvars];

    {
        int count = rng(0, 100);
        for (int i=0; i<count; i++) {
            for (int j=0; j<nvars; j++) arr[j] = rng(0, 2);
            set_a = sylvan_union_cube(set_a, bdd_dom, arr);
        }
    }

    {
        int count = rng(0, 100);
        for (int i=0; i<count; i++) {
            for (int j=0; j<nvars; j++) arr[j] = rng(0, 2);
            set_b = sylvan_union_cube(set_b, bdd_dom, arr);
        }
    }

    {
        int count = rng(0, 100);
        for (int i=0; i<count; i++) {
            for (int j=0; j<nvars; j++) arr[j] = rng(0, 2);
            set_c = sylvan_union_cube(set_c, bdd_dom, arr);
        }
    }

    ZDD zdd_set_a = zdd_from_mtbdd(set_a, bdd_dom);
    ZDD zdd_set_b = zdd_from_mtbdd(set_b, bdd_dom);
    ZDD zdd_set_c = zdd_from_mtbdd(set_c, bdd_dom);
    ZDD zdd_dom = zdd_set_from_mtbdd(bdd_dom);

    MTBDD bdd_test_result = sylvan_ite(set_a, set_b, set_c);
    ZDD zdd_test_result = zdd_ite(zdd_set_a, zdd_set_b, zdd_set_c, zdd_dom);
    test_assert(zdd_from_mtbdd(bdd_test_result, bdd_dom) == zdd_test_result);

    return 0;
}

TASK_0(int, test_zdd_exists)
{
    /**
     * Test zdd_exists with random sets
     */

    // Create random domain of 6..12 variables
    int nvars = rng(6, 12);
    uint32_t dom_arr[nvars];
    for (int i=0; i<nvars; i++) dom_arr[i] = i;
    BDD bdd_dom = mtbdd_fromarray(dom_arr, nvars);
    ZDD zdd_dom = zdd_set_from_array(dom_arr, nvars);

    // Create random subdomain and quotiented variables (qdom)
    uint32_t subdom_arr[nvars], q_arr[nvars];
    int nsub = 0, nq = 0;
    for (int i=0; i<nvars; i++) {
        if (rng(0,2)) subdom_arr[nsub++] = i;
        else q_arr[nq++] = i;
    }
    BDD bdd_subdom = mtbdd_fromarray(subdom_arr, nsub);
    ZDD zdd_subdom = zdd_set_from_array(subdom_arr, nsub);
    BDD bdd_qdom = mtbdd_fromarray(q_arr, nq);
    ZDD zdd_qdom = zdd_set_from_array(q_arr, nq);

    // Create random set on subdomain
    BDD bdd_set = sylvan_false;
    ZDD zdd_set = zdd_false;
    {
        uint8_t arr[nvars];
        int count = rng(10,200);
        for (int i=0; i<count; i++) {
            for (int j=0; j<nvars; j++) arr[j] = rng(0, 2);
            bdd_set = sylvan_union_cube(bdd_set, bdd_dom, arr);
            zdd_set = zdd_union_cube(zdd_set, zdd_dom, arr, zdd_true);
        }
        test_assert(zdd_set == zdd_from_mtbdd(bdd_set, bdd_dom));
    }

    BDD bdd_qset = sylvan_exists(bdd_set, bdd_qdom);
    ZDD zdd_test_result = zdd_exists(zdd_set, zdd_qdom);
    test_assert(zdd_test_result == zdd_from_mtbdd(bdd_qset, bdd_dom));
    ZDD zdd_test_result2 = zdd_project(zdd_set, zdd_subdom);
    test_assert(zdd_test_result2 == zdd_from_mtbdd(bdd_qset, bdd_subdom));

    return 0;
}

// TASK_0(int, test_zdd_relnext)
// {
//     /**
//      * Test zdd_relnext with random sets
//      */
//     int nvars = rng(8,12);
// 
//     // Create random source set
//     uint32_t dom_arr[nvars];
//     for (int i=0; i<nvars; i++) dom_arr[i] = i*2;
//     BDD bdd_dom = mtbdd_fromarray(dom_arr, nvars);
//     ZDD zdd_dom = zdd_from_array(dom_arr, nvars);
// 
//     BDD bdd_set = sylvan_false;
//     ZDD zdd_set = zdd_false;
//     {
//         int count = rng(4,100);
//         for (int i=0; i<count; i++) {
//             uint8_t arr[nvars];
//             for (int j=0; j<nvars; j++) arr[j] = rng(0, 2);
//             bdd_set = sylvan_union_cube(bdd_set, bdd_dom, arr);
//             zdd_set = zdd_union_cube(zdd_set, zdd_dom, arr);
//         }
//     }
//     test_assert(zdd_set == zdd_from_mtbdd(bdd_set, bdd_dom));
// 
//     // Create random transition relation domain
//     BDD bdd_vars;
//     ZDD zdd_vars;
//     uint32_t vars_arr[2*nvars];
//     int len = 0;
//     {
//         int _vars = rng(1, 256);
//         for (int i=0; i<nvars; i++) {
//             if (_vars & (1<<i)) {
//                 vars_arr[len++] = i*2;
//                 vars_arr[len++] = i*2+1;
//             }
//         }
//         bdd_vars = mtbdd_fromarray(vars_arr, len);
//         zdd_vars = zdd_from_array(vars_arr, len);
//     }
//     test_assert(zdd_vars == zdd_from_mtbdd(bdd_vars, bdd_vars));
// 
//     // Create random transitions
//     BDD bdd_rel = sylvan_false;
//     ZDD zdd_rel = zdd_false;
//     {
//         int count = rng(100, 200);
//         for (int i=0; i<count; i++) {
//             uint8_t arr[len];
//             for (int j=0; j<len; j++) arr[j] = rng(0, 2);
//             bdd_rel = sylvan_union_cube(bdd_rel, bdd_vars, arr);
//             zdd_rel = zdd_union_cube(zdd_rel, zdd_vars, arr);
//         }
//     }
//     test_assert(zdd_rel == zdd_from_mtbdd(bdd_rel, bdd_vars));
// 
//     // Check if sat counts are the same
//     test_assert(sylvan_satcount(bdd_set, bdd_dom) == zdd_satcount(zdd_set, zdd_dom));
//     test_assert(sylvan_satcount(bdd_rel, bdd_vars) == zdd_satcount(zdd_rel, zdd_vars));
// 
//     BDD bdd_succ = sylvan_relnext(bdd_set, bdd_rel, bdd_vars);
//     ZDD zdd_succ = zdd_relnext(zdd_set, zdd_rel, zdd_vars, zdd_dom);
// 
//     test_assert(zdd_succ == zdd_from_mtbdd(bdd_succ, bdd_dom));
// 
//     return 0;
// }
// 
// TASK_0(int, test_zdd_and_dom)
// {
//     /**
//      * Test zdd_and_dom with random sets
//      */
// 
//     // Create random domain of 6..14 variables
//     int nvars = rng(6,14);
//     uint32_t dom_arr[nvars];
//     for (int i=0; i<nvars; i++) dom_arr[i] = i;
//     BDD bdd_dom = mtbdd_fromarray(dom_arr, nvars);
//     ZDD zdd_dom = zdd_from_array(dom_arr, nvars);
//     test_assert(zdd_dom == zdd_from_mtbdd(bdd_dom, bdd_dom));
// 
//     // Create random subdomain 1
//     uint32_t subdom1_arr[nvars];
//     int nsub1 = 0;
//     for (int i=0; i<nvars; i++) if (rng(0,2)) subdom1_arr[nsub1++] = i;
//     BDD bdd_subdom1 = mtbdd_fromarray(subdom1_arr, nsub1);
//     ZDD zdd_subdom1 = zdd_from_array(subdom1_arr, nsub1);
//     test_assert(zdd_subdom1 == zdd_from_mtbdd(bdd_subdom1, bdd_subdom1));
// 
//     // Create random subdomain 2
//     uint32_t subdom2_arr[nvars];
//     int nsub2 = 0;
//     for (int i=0; i<nvars; i++) if (rng(0,2)) subdom2_arr[nsub2++] = i;
//     BDD bdd_subdom2 = mtbdd_fromarray(subdom2_arr, nsub2);
//     ZDD zdd_subdom2 = zdd_from_array(subdom2_arr, nsub2);
//     test_assert(zdd_subdom2 == zdd_from_mtbdd(bdd_subdom2, bdd_subdom2));
// 
//     // Create random set on subdomain 1
//     BDD bdd_set1 = sylvan_false;
//     ZDD zdd_set1 = zdd_false;
//     {
//         int count = rng(10,200);
//         for (int i=0; i<count; i++) {
//             uint8_t arr[nsub1];
//             for (int j=0; j<nsub1; j++) arr[j] = rng(0, 2);
//             bdd_set1 = sylvan_union_cube(bdd_set1, bdd_subdom1, arr);
//             zdd_set1 = zdd_union_cube(zdd_set1, zdd_subdom1, arr);
//         }
//     }
//     test_assert(zdd_set1 == zdd_from_mtbdd(bdd_set1, bdd_subdom1));
// 
//     // Create random set on subdomain 2
//     BDD bdd_set2 = sylvan_false;
//     ZDD zdd_set2 = zdd_false;
//     {
//         int count = rng(10,200);
//         for (int i=0; i<count; i++) {
//             uint8_t arr[nsub2];
//             for (int j=0; j<nsub2; j++) arr[j] = rng(0, 2);
//             bdd_set2 = sylvan_union_cube(bdd_set2, bdd_subdom2, arr);
//             zdd_set2 = zdd_union_cube(zdd_set2, zdd_subdom2, arr);
//         }
//     }
//     test_assert(zdd_set2 == zdd_from_mtbdd(bdd_set2, bdd_subdom2));
// 
//     BDD bdd_set = sylvan_and(bdd_set1, bdd_set2);
//     BDD bdd_subdom = sylvan_and(bdd_subdom1, bdd_subdom2);
//     ZDD zdd_set = zdd_and_dom(zdd_set1, zdd_subdom1, zdd_set2, zdd_subdom2);
//     test_assert(zdd_set == zdd_from_mtbdd(bdd_set, bdd_subdom));
// 
//     return 0;
// }

/**
 * Basic test for ISOP on a known small case.
 */
TASK_0(int, test_zdd_isop_basic)
{
    BDD a = sylvan_ithvar(1);
    BDD b = sylvan_ithvar(2);

    BDD a_and_b = sylvan_and(a, b);
    BDD aNot_and_b = sylvan_and(sylvan_not(a), b);
    BDD redundant_b = sylvan_or(a_and_b, aNot_and_b);

    // ab + ~ab == b

    MTBDD bddres;
    ZDD isop_zdd = zdd_isop(redundant_b, redundant_b, &bddres);
    MTBDD bdd2 = zdd_cover_to_bdd(isop_zdd);

    test_assert(bddres == redundant_b);
    test_assert(bdd2 == redundant_b);
    test_assert(zdd_getvar(isop_zdd) == 4);
    test_assert(zdd_gethigh(isop_zdd) == zdd_true); 
    test_assert(zdd_getlow(isop_zdd) == zdd_false);
    return 0;
}

TASK_0(int, test_zdd_isop_random)
{
    BDD bdd_dom = mtbdd_fromarray((uint32_t[]){0,1,2,3,4,5,6,7,8,9,10,11}, 12);

    // create a random BDD
    MTBDD bdd_set = mtbdd_false;
    int cubecount = rng(1,200);
    for (int j=0; j<cubecount; j++) {
        uint8_t arr[11];
        for (int j=0; j<11; j++) arr[j] = rng(0, 2);
        bdd_set = sylvan_or(bdd_set, sylvan_cube(bdd_dom, arr));
    }

    // convert to ISOP cover
    MTBDD isop_bdd;
    ZDD isop_zdd = zdd_isop(bdd_set, bdd_set, &isop_bdd);
    MTBDD remade_bdd = zdd_cover_to_bdd(isop_zdd);

    // manually count cubes
    int arr[12];
    ZDD res = zdd_cover_enum_first(isop_zdd, arr);
    int count1 = 0;
    while (res != zdd_false) {
        res = zdd_cover_enum_next(isop_zdd, arr);
        count1++;
    }

    // count cubes by counting paths
    long zdd_cubes = zdd_pathcount(isop_zdd);

    // printf("%6d cubes, %6ld PIs\n", cubecount, zdd_cubes);

    // check if all is right
    test_assert(isop_bdd == bdd_set);
    test_assert(remade_bdd == bdd_set);
    test_assert(count1 <= cubecount);
    test_assert(count1 == zdd_cubes);

    return 0;
}

TASK_0(int, test_zdd_read_write)
{
    /**
     * Test reading/writing with random sets
     */
    int nvars = rng(8,12);

    // Create random source sets
    uint32_t dom_arr[nvars];
    for (int i=0; i<nvars; i++) dom_arr[i] = i*2;
    ZDD zdd_dom = zdd_set_from_array(dom_arr, nvars);

    int set_count = rng(1,10);
    ZDD zdd_set[set_count];
    for (int k=0; k<set_count; k++) {
        zdd_set[k] = zdd_false;
        int count = rng(4,100);
        for (int i=0; i<count; i++) {
            uint8_t arr[nvars];
            for (int j=0; j<nvars; j++) arr[j] = rng(0, 2);
            zdd_set[k] = zdd_union_cube(zdd_set[k], zdd_dom, arr, zdd_true);
        }
    }

    FILE *f = tmpfile();
    zdd_writer_tobinary(f, zdd_set, set_count);
    rewind(f);
    ZDD test[set_count];
    test_assert(zdd_reader_frombinary(f, test, set_count) == 0);
    for (int i=0; i<set_count; i++) test_assert(test[i] == zdd_set[i]);

    return 0;
}

TASK_0(int, runtests)
{
    // Testing without garbage collection
    sylvan_gc_disable();

    int test_iterations = 1000;

    printf("test_zdd_eval...\n");
    for (int i=0; i<test_iterations; i++) if (CALL(test_zdd_eval)) return 1;
    printf("test_zdd_ithvar...\n");
    for (int i=0; i<test_iterations; i++) if (CALL(test_zdd_ithvar)) return 1;
    printf("test_zdd_from_mtbdd...\n");
    for (int k=0; k<test_iterations; k++) if (CALL(test_zdd_from_mtbdd)) return 1;
    printf("test_zdd_satcount...\n");
    for (int k=0; k<test_iterations; k++) if (CALL(test_zdd_satcount)) return 1;
    printf("test_zdd_merge_domains...\n");
    for (int k=0; k<test_iterations; k++) if (CALL(test_zdd_merge_domains)) return 1;
    printf("test_zdd_union_cube...\n");
    for (int k=0; k<test_iterations; k++) if (CALL(test_zdd_union_cube)) return 1;
    printf("test_zdd_enum...\n");
    for (int k=0; k<test_iterations; k++) if (CALL(test_zdd_enum)) return 1;
    printf("test_zdd_ite...\n");
    for (int k=0; k<test_iterations; k++) if (CALL(test_zdd_ite)) return 1;
    printf("test_zdd_and...\n");
    for (int k=0; k<test_iterations; k++) if (CALL(test_zdd_and)) return 1;
    printf("test_zdd_or...\n");
    for (int k=0; k<test_iterations; k++) if (CALL(test_zdd_or)) return 1;
    printf("test_zdd_not...\n");
    for (int k=0; k<test_iterations; k++) if (CALL(test_zdd_not)) return 1;
    printf("test_zdd_exists...\n");
    for (int k=0; k<test_iterations; k++) if (CALL(test_zdd_exists)) return 1;
    // for (int k=0; k<test_iterations; k++) if (CALL(test_zdd_relnext)) return 1;
    // for (int k=0; k<test_iterations; k++) if (CALL(test_zdd_and_dom)) return 1;
    printf("test_zdd_read_write...\n");
    for (int k=0; k<test_iterations; k++) if (CALL(test_zdd_read_write)) return 1;
    // for (int k=0; k<test_iterations; k++) if (CALL(test_zdd_extend_domain)) return 1;
    printf("test_zdd_isop_basic...\n");
    if (CALL(test_zdd_isop_basic)) return 1;
    printf("test_zdd_isop_random...\n");
    for (int k=0; k<test_iterations; k++) if (CALL(test_zdd_isop_random)) return 1;

    return 0;
}

int main()
{
    // Standard Lace initialization with 1 worker
	lace_start(1, 0);

    // Simple Sylvan initialization, also initialize BDD, MTBDD and LDD support
	sylvan_set_sizes(1LL<<26, 1LL<<26, 1LL<<20, 1LL<<20);
	sylvan_init_package();
    sylvan_init_mtbdd();
    sylvan_init_zdd();

    int res = RUN(runtests);

    sylvan_quit();
    lace_stop();

    return res;
}
