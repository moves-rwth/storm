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

#include <sylvan_config.h>

#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avl.h>
#include <refs.h>
#include <sha2.h>
#include <sylvan.h>
#include <sylvan_common.h>

/**
 * Complement handling macros
 */
#define BDD_HASMARK(s)              (s&sylvan_complement?1:0)
#define BDD_TOGGLEMARK(s)           (s^sylvan_complement)
#define BDD_STRIPMARK(s)            (s&~sylvan_complement)
#define BDD_TRANSFERMARK(from, to)  (to ^ (from & sylvan_complement))
// Equal under mark
#define BDD_EQUALM(a, b)            ((((a)^(b))&(~sylvan_complement))==0)

/**
 * BDD node structure
 */
typedef struct __attribute__((packed)) bddnode {
    uint64_t a, b;
} * bddnode_t; // 16 bytes

#define GETNODE(bdd) ((bddnode_t)llmsset_index_to_ptr(nodes, bdd&0x000000ffffffffff))

static inline int __attribute__((unused))
bddnode_getcomp(bddnode_t n)
{
    return n->a & 0x8000000000000000 ? 1 : 0;
}

static inline uint64_t
bddnode_getlow(bddnode_t n)
{
    return n->b & 0x000000ffffffffff; // 40 bits
}

static inline uint64_t
bddnode_gethigh(bddnode_t n)
{
    return n->a & 0x800000ffffffffff; // 40 bits plus high bit of first
}

static inline uint32_t
bddnode_getvariable(bddnode_t n)
{
    return (uint32_t)(n->b >> 40);
}

static inline int
bddnode_getmark(bddnode_t n)
{
    return n->a & 0x2000000000000000 ? 1 : 0;
}

static inline void
bddnode_setmark(bddnode_t n, int mark)
{
    if (mark) n->a |= 0x2000000000000000;
    else n->a &= 0xdfffffffffffffff;
}

static inline void
bddnode_makenode(bddnode_t n, uint32_t var, uint64_t low, uint64_t high)
{
    n->a = high;
    n->b = ((uint64_t)var)<<40 | low;
}

/**
 * Implementation of garbage collection.
 */

/* Recursively mark BDD nodes as 'in use' */
VOID_TASK_IMPL_1(sylvan_gc_mark_rec, BDD, bdd)
{
    if (bdd == sylvan_false || bdd == sylvan_true) return;

    if (llmsset_mark(nodes, bdd&0x000000ffffffffff)) {
        bddnode_t n = GETNODE(bdd);
        SPAWN(sylvan_gc_mark_rec, bddnode_getlow(n));
        CALL(sylvan_gc_mark_rec, bddnode_gethigh(n));
        SYNC(sylvan_gc_mark_rec);
    }
}

/**
 * External references
 */

refs_table_t bdd_refs;
refs_table_t bdd_protected;
static int bdd_protected_created = 0;

BDD
sylvan_ref(BDD a)
{
    if (a == sylvan_false || a == sylvan_true) return a;
    refs_up(&bdd_refs, BDD_STRIPMARK(a));
    return a;
}

void
sylvan_deref(BDD a)
{
    if (a == sylvan_false || a == sylvan_true) return;
    refs_down(&bdd_refs, BDD_STRIPMARK(a));
}

void
sylvan_protect(BDD *a)
{
    if (!bdd_protected_created) {
        // In C++, sometimes sylvan_protect is called before Sylvan is initialized. Just create a table.
        protect_create(&bdd_protected, 4096);
        bdd_protected_created = 1;
    }
    protect_up(&bdd_protected, (size_t)a);
}

void
sylvan_unprotect(BDD *a)
{
    protect_down(&bdd_protected, (size_t)a);
}

size_t
sylvan_count_refs()
{
    return refs_count(&bdd_refs);
}

size_t
sylvan_count_protected()
{
    return protect_count(&bdd_protected);
}

/* Called during garbage collection */
VOID_TASK_0(sylvan_gc_mark_external_refs)
{
    // iterate through refs hash table, mark all found
    size_t count=0;
    uint64_t *it = refs_iter(&bdd_refs, 0, bdd_refs.refs_size);
    while (it != NULL) {
        BDD to_mark = refs_next(&bdd_refs, &it, bdd_refs.refs_size);
        SPAWN(sylvan_gc_mark_rec, to_mark);
        count++;
    }
    while (count--) {
        SYNC(sylvan_gc_mark_rec);
    }
}

VOID_TASK_0(sylvan_gc_mark_protected)
{
    // iterate through refs hash table, mark all found
    size_t count=0;
    uint64_t *it = protect_iter(&bdd_protected, 0, bdd_protected.refs_size);
    while (it != NULL) {
        BDD *to_mark = (BDD*)protect_next(&bdd_protected, &it, bdd_protected.refs_size);
        SPAWN(sylvan_gc_mark_rec, *to_mark);
        count++;
    }
    while (count--) {
        SYNC(sylvan_gc_mark_rec);
    }
}

/* Infrastructure for internal markings */
DECLARE_THREAD_LOCAL(bdd_refs_key, bdd_refs_internal_t);

VOID_TASK_0(bdd_refs_mark_task)
{
    LOCALIZE_THREAD_LOCAL(bdd_refs_key, bdd_refs_internal_t);
    size_t i, j=0;
    for (i=0; i<bdd_refs_key->r_count; i++) {
        if (j >= 40) {
            while (j--) SYNC(sylvan_gc_mark_rec);
            j=0;
        }
        SPAWN(sylvan_gc_mark_rec, bdd_refs_key->results[i]);
        j++;
    }
    for (i=0; i<bdd_refs_key->s_count; i++) {
        Task *t = bdd_refs_key->spawns[i];
        if (!TASK_IS_STOLEN(t)) break;
        if (TASK_IS_COMPLETED(t)) {
            if (j >= 40) {
                while (j--) SYNC(sylvan_gc_mark_rec);
                j=0;
            }
            SPAWN(sylvan_gc_mark_rec, *(BDD*)TASK_RESULT(t));
            j++;
        }
    }
    while (j--) SYNC(sylvan_gc_mark_rec);
}

VOID_TASK_0(bdd_refs_mark)
{
    TOGETHER(bdd_refs_mark_task);
}

VOID_TASK_0(bdd_refs_init_task)
{
    bdd_refs_internal_t s = (bdd_refs_internal_t)malloc(sizeof(struct bdd_refs_internal));
    s->r_size = 128;
    s->r_count = 0;
    s->s_size = 128;
    s->s_count = 0;
    s->results = (BDD*)malloc(sizeof(BDD) * 128);
    s->spawns = (Task**)malloc(sizeof(Task*) * 128);
    SET_THREAD_LOCAL(bdd_refs_key, s);
}

VOID_TASK_0(bdd_refs_init)
{
    INIT_THREAD_LOCAL(bdd_refs_key);
    TOGETHER(bdd_refs_init_task);
    sylvan_gc_add_mark(10, TASK(bdd_refs_mark));
}

/**
 * Initialize and quit functions
 */

static int granularity = 1; // default

static void
sylvan_quit_bdd()
{
    refs_free(&bdd_refs);
    if (bdd_protected_created) {
        protect_free(&bdd_protected);
        bdd_protected_created = 0;
    }
}

void
sylvan_init_bdd(int _granularity)
{
    sylvan_register_quit(sylvan_quit_bdd);
    sylvan_gc_add_mark(10, TASK(sylvan_gc_mark_external_refs));
    sylvan_gc_add_mark(10, TASK(sylvan_gc_mark_protected));

    granularity = _granularity;

    // Sanity check
    if (sizeof(struct bddnode) != 16) {
        fprintf(stderr, "Invalid size of bdd nodes: %ld\n", sizeof(struct bddnode));
        exit(1);
    }

    refs_create(&bdd_refs, 1024);
    if (!bdd_protected_created) {
        protect_create(&bdd_protected, 4096);
        bdd_protected_created = 1;
    }

    LACE_ME;
    CALL(bdd_refs_init);
}

/**
 * Core BDD operations
 */

BDD
sylvan_makenode(BDDVAR level, BDD low, BDD high)
{
    if (low == high) return low;

    // Normalization to keep canonicity
    // low will have no mark

    struct bddnode n;
    int mark;

    if (BDD_HASMARK(low)) {
        mark = 1;
        low = BDD_TOGGLEMARK(low);
        high = BDD_TOGGLEMARK(high);
    } else {
        mark = 0;
    }

    bddnode_makenode(&n, level, low, high);

    BDD result;
    int created;
    uint64_t index = llmsset_lookup(nodes, n.a, n.b, &created);
    if (index == 0) {
        LACE_ME;

        bdd_refs_push(low);
        bdd_refs_push(high);
        sylvan_gc();
        bdd_refs_pop(2);

        index = llmsset_lookup(nodes, n.a, n.b, &created);
        if (index == 0) {
            fprintf(stderr, "BDD Unique table full, %zu of %zu buckets filled!\n", llmsset_count_marked(nodes), llmsset_get_size(nodes));
            exit(1);
        }
    }

    if (created) sylvan_stats_count(BDD_NODES_CREATED);
    else sylvan_stats_count(BDD_NODES_REUSED);

    result = index;
    return mark ? result | sylvan_complement : result;
}

BDD
sylvan_ithvar(BDDVAR level)
{
    return sylvan_makenode(level, sylvan_false, sylvan_true);
}

BDDVAR
sylvan_var(BDD bdd)
{
    return bddnode_getvariable(GETNODE(bdd));
}

static BDD
node_low(BDD bdd, bddnode_t node)
{
    return BDD_TRANSFERMARK(bdd, bddnode_getlow(node));
}

static BDD
node_high(BDD bdd, bddnode_t node)
{
    return BDD_TRANSFERMARK(bdd, bddnode_gethigh(node));
}

BDD
sylvan_low(BDD bdd)
{
    if (sylvan_isconst(bdd)) return bdd;
    return node_low(bdd, GETNODE(bdd));
}

BDD
sylvan_high(BDD bdd)
{
    if (sylvan_isconst(bdd)) return bdd;
    return node_high(bdd, GETNODE(bdd));
}

/**
 * Implementation of unary, binary and if-then-else operators.
 */
TASK_IMPL_3(BDD, sylvan_and, BDD, a, BDD, b, BDDVAR, prev_level)
{
    /* Terminal cases */
    if (a == sylvan_true) return b;
    if (b == sylvan_true) return a;
    if (a == sylvan_false) return sylvan_false;
    if (b == sylvan_false) return sylvan_false;
    if (a == b) return a;
    if (a == BDD_TOGGLEMARK(b)) return sylvan_false;

    sylvan_gc_test();

    sylvan_stats_count(BDD_AND);

    /* Improve for caching */
    if (BDD_STRIPMARK(a) > BDD_STRIPMARK(b)) {
        BDD t = b;
        b = a;
        a = t;
    }

    bddnode_t na = GETNODE(a);
    bddnode_t nb = GETNODE(b);

    BDDVAR va = bddnode_getvariable(na);
    BDDVAR vb = bddnode_getvariable(nb);
    BDDVAR level = va < vb ? va : vb;

    int cachenow = granularity < 2 || prev_level == 0 ? 1 : prev_level / granularity != level / granularity;
    if (cachenow) {
        BDD result;
        if (cache_get3(CACHE_BDD_AND, a, b, sylvan_false, &result)) {
            sylvan_stats_count(BDD_AND_CACHED);
            return result;
        }
    }

    // Get cofactors
    BDD aLow = a, aHigh = a;
    BDD bLow = b, bHigh = b;
    if (level == va) {
        aLow = node_low(a, na);
        aHigh = node_high(a, na);
    }
    if (level == vb) {
        bLow = node_low(b, nb);
        bHigh = node_high(b, nb);
    }

    // Recursive computation
    BDD low=sylvan_invalid, high=sylvan_invalid, result;

    int n=0;

    if (aHigh == sylvan_true) {
        high = bHigh;
    } else if (aHigh == sylvan_false || bHigh == sylvan_false) {
        high = sylvan_false;
    } else if (bHigh == sylvan_true) {
        high = aHigh;
    } else {
        bdd_refs_spawn(SPAWN(sylvan_and, aHigh, bHigh, level));
        n=1;
    }

    if (aLow == sylvan_true) {
        low = bLow;
    } else if (aLow == sylvan_false || bLow == sylvan_false) {
        low = sylvan_false;
    } else if (bLow == sylvan_true) {
        low = aLow;
    } else {
        low = CALL(sylvan_and, aLow, bLow, level);
    }

    if (n) {
        bdd_refs_push(low);
        high = bdd_refs_sync(SYNC(sylvan_and));
        bdd_refs_pop(1);
    }

    result = sylvan_makenode(level, low, high);

    if (cachenow) {
        if (cache_put3(CACHE_BDD_AND, a, b, sylvan_false, result)) sylvan_stats_count(BDD_AND_CACHEDPUT);
    }

    return result;
}

TASK_IMPL_3(BDD, sylvan_xor, BDD, a, BDD, b, BDDVAR, prev_level)
{
    /* Terminal cases */
    if (a == sylvan_false) return b;
    if (b == sylvan_false) return a;
    if (a == sylvan_true) return sylvan_not(b);
    if (b == sylvan_true) return sylvan_not(a);
    if (a == b) return sylvan_false;
    if (a == sylvan_not(b)) return sylvan_true;

    sylvan_gc_test();

    sylvan_stats_count(BDD_XOR);

    /* Improve for caching */
    if (BDD_STRIPMARK(a) > BDD_STRIPMARK(b)) {
        BDD t = b;
        b = a;
        a = t;
    }

    // XOR(~A,B) => XOR(A,~B)
    if (BDD_HASMARK(a)) {
        a = BDD_STRIPMARK(a);
        b = sylvan_not(b);
    }

    bddnode_t na = GETNODE(a);
    bddnode_t nb = GETNODE(b);

    BDDVAR va = bddnode_getvariable(na);
    BDDVAR vb = bddnode_getvariable(nb);
    BDDVAR level = va < vb ? va : vb;

    int cachenow = granularity < 2 || prev_level == 0 ? 1 : prev_level / granularity != level / granularity;
    if (cachenow) {
        BDD result;
        if (cache_get3(CACHE_BDD_XOR, a, b, sylvan_false, &result)) {
            sylvan_stats_count(BDD_XOR_CACHED);
            return result;
        }
    }

    // Get cofactors
    BDD aLow = a, aHigh = a;
    BDD bLow = b, bHigh = b;
    if (level == va) {
        aLow = node_low(a, na);
        aHigh = node_high(a, na);
    }
    if (level == vb) {
        bLow = node_low(b, nb);
        bHigh = node_high(b, nb);
    }

    // Recursive computation
    BDD low, high, result;

    bdd_refs_spawn(SPAWN(sylvan_xor, aHigh, bHigh, level));
    low = CALL(sylvan_xor, aLow, bLow, level);
    bdd_refs_push(low);
    high = bdd_refs_sync(SYNC(sylvan_xor));
    bdd_refs_pop(1);

    result = sylvan_makenode(level, low, high);

    if (cachenow) {
        if (cache_put3(CACHE_BDD_XOR, a, b, sylvan_false, result)) sylvan_stats_count(BDD_XOR_CACHEDPUT);
    }

    return result;
}


TASK_IMPL_4(BDD, sylvan_ite, BDD, a, BDD, b, BDD, c, BDDVAR, prev_level)
{
    /* Terminal cases */
    if (a == sylvan_true) return b;
    if (a == sylvan_false) return c;
    if (a == b) b = sylvan_true;
    if (a == sylvan_not(b)) b = sylvan_false;
    if (a == c) c = sylvan_false;
    if (a == sylvan_not(c)) c = sylvan_true;
    if (b == c) return b;
    if (b == sylvan_true && c == sylvan_false) return a;
    if (b == sylvan_false && c == sylvan_true) return sylvan_not(a);

    /* Cases that reduce to AND and XOR */

    // ITE(A,B,0) => AND(A,B)
    if (c == sylvan_false) return CALL(sylvan_and, a, b, prev_level);

    // ITE(A,1,C) => ~AND(~A,~C)
    if (b == sylvan_true) return sylvan_not(CALL(sylvan_and, sylvan_not(a), sylvan_not(c), prev_level));

    // ITE(A,0,C) => AND(~A,C)
    if (b == sylvan_false) return CALL(sylvan_and, sylvan_not(a), c, prev_level);

    // ITE(A,B,1) => ~AND(A,~B)
    if (c == sylvan_true) return sylvan_not(CALL(sylvan_and, a, sylvan_not(b), prev_level));

    // ITE(A,B,~B) => XOR(A,~B)
    if (b == sylvan_not(c)) return CALL(sylvan_xor, a, c, 0);

    /* At this point, there are no more terminals */

    /* Canonical for optimal cache use */

    // ITE(~A,B,C) => ITE(A,C,B)
    if (BDD_HASMARK(a)) {
        a = BDD_STRIPMARK(a);
        BDD t = c;
        c = b;
        b = t;
    }

    // ITE(A,~B,C) => ~ITE(A,B,~C)
    int mark = 0;
    if (BDD_HASMARK(b)) {
        b = sylvan_not(b);
        c = sylvan_not(c);
        mark = 1;
    }

    bddnode_t na = GETNODE(a);
    bddnode_t nb = GETNODE(b);
    bddnode_t nc = GETNODE(c);

    BDDVAR va = bddnode_getvariable(na);
    BDDVAR vb = bddnode_getvariable(nb);
    BDDVAR vc = bddnode_getvariable(nc);

    // Get lowest level
    BDDVAR level = vb < vc ? vb : vc;

    // Fast case
    if (va < level && node_low(a, na) == sylvan_false && node_high(a, na) == sylvan_true) {
        BDD result = sylvan_makenode(va, c, b);
        return mark ? sylvan_not(result) : result;
    }

    if (va < level) level = va;

    sylvan_gc_test();

    sylvan_stats_count(BDD_ITE);

    int cachenow = granularity < 2 || prev_level == 0 ? 1 : prev_level / granularity != level / granularity;
    if (cachenow) {
        BDD result;
        if (cache_get3(CACHE_BDD_ITE, a, b, c, &result)) {
            sylvan_stats_count(BDD_ITE_CACHED);
            return mark ? sylvan_not(result) : result;
        }
    }

    // Get cofactors
    BDD aLow = a, aHigh = a;
    BDD bLow = b, bHigh = b;
    BDD cLow = c, cHigh = c;
    if (level == va) {
        aLow = node_low(a, na);
        aHigh = node_high(a, na);
    }
    if (level == vb) {
        bLow = node_low(b, nb);
        bHigh = node_high(b, nb);
    }
    if (level == vc) {
        cLow = node_low(c, nc);
        cHigh = node_high(c, nc);
    }

    // Recursive computation
    BDD low=sylvan_invalid, high=sylvan_invalid, result;

    int n=0;

    if (aHigh == sylvan_true) {
        high = bHigh;
    } else if (aHigh == sylvan_false) {
        high = cHigh;
    } else {
        bdd_refs_spawn(SPAWN(sylvan_ite, aHigh, bHigh, cHigh, level));
        n=1;
    }

    if (aLow == sylvan_true) {
        low = bLow;
    } else if (aLow == sylvan_false) {
        low = cLow;
    } else {
        low = CALL(sylvan_ite, aLow, bLow, cLow, level);
    }

    if (n) {
        bdd_refs_push(low);
        high = bdd_refs_sync(SYNC(sylvan_ite));
        bdd_refs_pop(1);
    }

    result = sylvan_makenode(level, low, high);

    if (cachenow) {
        if (cache_put3(CACHE_BDD_ITE, a, b, c, result)) sylvan_stats_count(BDD_ITE_CACHEDPUT);
    }

    return mark ? sylvan_not(result) : result;
}

/**
 * Calculate constrain a @ c
 */
TASK_IMPL_3(BDD, sylvan_constrain, BDD, a, BDD, b, BDDVAR, prev_level)
{
    /* Trivial cases */
    if (b == sylvan_true) return a;
    if (b == sylvan_false) return sylvan_false;
    if (sylvan_isconst(a)) return a;
    if (a == b) return sylvan_true;
    if (a == sylvan_not(b)) return sylvan_false;

    /* Perhaps execute garbage collection */
    sylvan_gc_test();

    /* Count operation */
    sylvan_stats_count(BDD_CONSTRAIN);

    // a != constant and b != constant
    bddnode_t na = GETNODE(a);
    bddnode_t nb = GETNODE(b);

    BDDVAR va = bddnode_getvariable(na);
    BDDVAR vb = bddnode_getvariable(nb);
    BDDVAR level = va < vb ? va : vb;

    // CONSULT CACHE

    int cachenow = granularity < 2 || prev_level == 0 ? 1 : prev_level / granularity != level / granularity;
    if (cachenow) {
        BDD result;
        if (cache_get3(CACHE_BDD_CONSTRAIN, a, b, 0, &result)) {
            sylvan_stats_count(BDD_CONSTRAIN_CACHED);
            return result;
        }
    }

    // DETERMINE TOP BDDVAR AND COFACTORS

    BDD aLow, aHigh, bLow, bHigh;

    if (level == va) {
        aLow = node_low(a, na);
        aHigh = node_high(a, na);
    } else {
        aLow = aHigh = a;
    }

    if (level == vb) {
        bLow = node_low(b, nb);
        bHigh = node_high(b, nb);
    } else {
        bLow = bHigh = b;
    }

    BDD result;

    BDD low=sylvan_invalid, high=sylvan_invalid;
    if (bLow == sylvan_false) return CALL(sylvan_constrain, aHigh, bHigh, level);
    if (bLow == sylvan_true) {
        if (bHigh == sylvan_false) return aLow;
        if (bHigh == sylvan_true) {
            result = sylvan_makenode(level, aLow, bHigh);
        } else {
            high = CALL(sylvan_constrain, aHigh, bHigh, level);
            result = sylvan_makenode(level, aLow, high);
        }
    } else {
        if (bHigh == sylvan_false) return CALL(sylvan_constrain, aLow, bLow, level);
        if (bHigh == sylvan_true) {
            low = CALL(sylvan_constrain, aLow, bLow, level);
            result = sylvan_makenode(level, low, bHigh);
        } else {
            bdd_refs_spawn(SPAWN(sylvan_constrain, aLow, bLow, level));
            high = CALL(sylvan_constrain, aHigh, bHigh, level);
            bdd_refs_push(high);
            low = bdd_refs_sync(SYNC(sylvan_constrain));
            bdd_refs_pop(1);
            result = sylvan_makenode(level, low, high);
        }
    }

    if (cachenow) {
        if (cache_put3(CACHE_BDD_CONSTRAIN, a, b, 0, result)) sylvan_stats_count(BDD_CONSTRAIN_CACHEDPUT);
    }

    return result;
}

/**
 * Calculate restrict a @ b
 */
TASK_IMPL_3(BDD, sylvan_restrict, BDD, a, BDD, b, BDDVAR, prev_level)
{
    /* Trivial cases */
    if (b == sylvan_true) return a;
    if (b == sylvan_false) return sylvan_false;
    if (sylvan_isconst(a)) return a;
    if (a == b) return sylvan_true;
    if (a == sylvan_not(b)) return sylvan_false;

    /* Perhaps execute garbage collection */
    sylvan_gc_test();

    /* Count operation */
    sylvan_stats_count(BDD_RESTRICT);

    // a != constant and b != constant
    bddnode_t na = GETNODE(a);
    bddnode_t nb = GETNODE(b);

    BDDVAR va = bddnode_getvariable(na);
    BDDVAR vb = bddnode_getvariable(nb);
    BDDVAR level = va < vb ? va : vb;

    /* Consult cache */
    int cachenow = granularity < 2 || prev_level == 0 ? 1 : prev_level / granularity != level / granularity;
    if (cachenow) {
        BDD result;
        if (cache_get3(CACHE_BDD_RESTRICT, a, b, 0, &result)) {
            sylvan_stats_count(BDD_RESTRICT_CACHED);
            return result;
        }
    }

    BDD result;

    if (vb < va) {
        BDD c = CALL(sylvan_ite, node_low(b,nb), sylvan_true, node_high(b,nb), 0);
        bdd_refs_push(c);
        result = CALL(sylvan_restrict, a, c, level);
        bdd_refs_pop(1);
    } else {
        BDD aLow=node_low(a,na),aHigh=node_high(a,na),bLow=b,bHigh=b;
        if (va == vb) {
            bLow = node_low(b,nb);
            bHigh = node_high(b,nb);
        }
        if (bLow == sylvan_false) {
            result = CALL(sylvan_restrict, aHigh, bHigh, level);
        } else if (bHigh == sylvan_false) {
            result = CALL(sylvan_restrict, aLow, bLow, level);
        } else {
            bdd_refs_spawn(SPAWN(sylvan_restrict, aLow, bLow, level));
            BDD high = CALL(sylvan_restrict, aHigh, bHigh, level);
            bdd_refs_push(high);
            BDD low = bdd_refs_sync(SYNC(sylvan_restrict));
            bdd_refs_pop(1);
            result = sylvan_makenode(level, low, high);
        }
    }

    if (cachenow) {
        if (cache_put3(CACHE_BDD_RESTRICT, a, b, 0, result)) sylvan_stats_count(BDD_RESTRICT_CACHEDPUT);
    }

    return result;
}

/**
 * Calculates \exists variables . a
 */
TASK_IMPL_3(BDD, sylvan_exists, BDD, a, BDD, variables, BDDVAR, prev_level)
{
    /* Terminal cases */
    if (a == sylvan_true) return sylvan_true;
    if (a == sylvan_false) return sylvan_false;
    if (sylvan_set_isempty(variables)) return a;

    // a != constant
    bddnode_t na = GETNODE(a);
    BDDVAR level = bddnode_getvariable(na);

    bddnode_t nv = GETNODE(variables);
    BDDVAR vv = bddnode_getvariable(nv);
    while (vv < level) {
        variables = node_high(variables, nv);
        if (sylvan_set_isempty(variables)) return a;
        nv = GETNODE(variables);
        vv = bddnode_getvariable(nv);
    }

    sylvan_gc_test();

    sylvan_stats_count(BDD_EXISTS);

    int cachenow = granularity < 2 || prev_level == 0 ? 1 : prev_level / granularity != level / granularity;
    if (cachenow) {
        BDD result;
        if (cache_get3(CACHE_BDD_EXISTS, a, variables, 0, &result)) {
            sylvan_stats_count(BDD_EXISTS_CACHED);
            return result;
        }
    }

    // Get cofactors
    BDD aLow = node_low(a, na);
    BDD aHigh = node_high(a, na);

    BDD result;

    if (vv == level) {
        // level is in variable set, perform abstraction
        if (aLow == sylvan_true || aHigh == sylvan_true || aLow == sylvan_not(aHigh)) {
            result = sylvan_true;
        } else {
            BDD _v = sylvan_set_next(variables);
            BDD low = CALL(sylvan_exists, aLow, _v, level);
            if (low == sylvan_true) {
                result = sylvan_true;
            } else {
                bdd_refs_push(low);
                BDD high = CALL(sylvan_exists, aHigh, _v, level);
                if (high == sylvan_true) {
                    result = sylvan_true;
                    bdd_refs_pop(1);
                } else if (low == sylvan_false && high == sylvan_false) {
                    result = sylvan_false;
                    bdd_refs_pop(1);
                } else {
                    bdd_refs_push(high);
                    result = sylvan_or(low, high);
                    bdd_refs_pop(2);
                }
            }
        }
    } else {
        // level is not in variable set
        BDD low, high;
        bdd_refs_spawn(SPAWN(sylvan_exists, aHigh, variables, level));
        low = CALL(sylvan_exists, aLow, variables, level);
        bdd_refs_push(low);
        high = bdd_refs_sync(SYNC(sylvan_exists));
        bdd_refs_pop(1);
        result = sylvan_makenode(level, low, high);
    }

    if (cachenow) {
        if (cache_put3(CACHE_BDD_EXISTS, a, variables, 0, result)) sylvan_stats_count(BDD_EXISTS_CACHEDPUT);
    }

    return result;
}

/**
 * Calculate exists(a AND b, v)
 */
TASK_IMPL_4(BDD, sylvan_and_exists, BDD, a, BDD, b, BDDSET, v, BDDVAR, prev_level)
{
    /* Terminal cases */
    if (a == sylvan_false) return sylvan_false;
    if (b == sylvan_false) return sylvan_false;
    if (a == sylvan_not(b)) return sylvan_false;
    if (a == sylvan_true && b == sylvan_true) return sylvan_true;

    /* Cases that reduce to "exists" and "and" */
    if (a == sylvan_true) return CALL(sylvan_exists, b, v, 0);
    if (b == sylvan_true) return CALL(sylvan_exists, a, v, 0);
    if (a == b) return CALL(sylvan_exists, a, v, 0);
    if (sylvan_set_isempty(v)) return sylvan_and(a, b);

    /* At this point, a and b are proper nodes, and v is non-empty */

    /* Improve for caching */
    if (BDD_STRIPMARK(a) > BDD_STRIPMARK(b)) {
        BDD t = b;
        b = a;
        a = t;
    }

    /* Maybe perform garbage collection */
    sylvan_gc_test();

    sylvan_stats_count(BDD_AND_EXISTS);

    // a != constant
    bddnode_t na = GETNODE(a);
    bddnode_t nb = GETNODE(b);
    bddnode_t nv = GETNODE(v);

    BDDVAR va = bddnode_getvariable(na);
    BDDVAR vb = bddnode_getvariable(nb);
    BDDVAR vv = bddnode_getvariable(nv);
    BDDVAR level = va < vb ? va : vb;

    /* Skip levels in v that are not in a and b */
    while (vv < level) {
        v = node_high(v, nv); // get next variable in conjunction
        if (sylvan_set_isempty(v)) return sylvan_and(a, b);
        nv = GETNODE(v);
        vv = bddnode_getvariable(nv);
    }

    BDD result;

    int cachenow = granularity < 2 || prev_level == 0 ? 1 : prev_level / granularity != level / granularity;
    if (cachenow) {
        if (cache_get3(CACHE_BDD_AND_EXISTS, a, b, v, &result)) {
            sylvan_stats_count(BDD_AND_EXISTS_CACHED);
            return result;
        }
    }

    // Get cofactors
    BDD aLow, aHigh, bLow, bHigh;
    if (level == va) {
        aLow = node_low(a, na);
        aHigh = node_high(a, na);
    } else {
        aLow = a;
        aHigh = a;
    }
    if (level == vb) {
        bLow = node_low(b, nb);
        bHigh = node_high(b, nb);
    } else {
        bLow = b;
        bHigh = b;
    }

    if (level == vv) {
        // level is in variable set, perform abstraction
        BDD _v = node_high(v, nv);
        BDD low = CALL(sylvan_and_exists, aLow, bLow, _v, level);
        if (low == sylvan_true || low == aHigh || low == bHigh) {
            result = low;
        } else {
            bdd_refs_push(low);
            BDD high;
            if (low == sylvan_not(aHigh)) {
                high = CALL(sylvan_exists, bHigh, _v, 0);
            } else if (low == sylvan_not(bHigh)) {
                high = CALL(sylvan_exists, aHigh, _v, 0);
            } else {
                high = CALL(sylvan_and_exists, aHigh, bHigh, _v, level);
            }
            if (high == sylvan_true) {
                result = sylvan_true;
                bdd_refs_pop(1);
            } else if (high == sylvan_false) {
                result = low;
                bdd_refs_pop(1);
            } else if (low == sylvan_false) {
                result = high;
                bdd_refs_pop(1);
            } else {
                bdd_refs_push(high);
                result = sylvan_or(low, high);
                bdd_refs_pop(2);
            }
        }
    } else {
        // level is not in variable set
        bdd_refs_spawn(SPAWN(sylvan_and_exists, aHigh, bHigh, v, level));
        BDD low = CALL(sylvan_and_exists, aLow, bLow, v, level);
        bdd_refs_push(low);
        BDD high = bdd_refs_sync(SYNC(sylvan_and_exists));
        bdd_refs_pop(1);
        result = sylvan_makenode(level, low, high);
    }

    if (cachenow) {
        if (cache_put3(CACHE_BDD_AND_EXISTS, a, b, v, result)) sylvan_stats_count(BDD_AND_EXISTS_CACHEDPUT);
    }

    return result;
}


TASK_IMPL_4(BDD, sylvan_relnext, BDD, a, BDD, b, BDDSET, vars, BDDVAR, prev_level)
{
    /* Compute R(s) = \exists x: A(x) \and B(x,s) with support(result) = s, support(A) = s, support(B) = s+t
     * if vars == sylvan_false, then every level is in s or t
     * any other levels (outside s,t) in B are ignored / existentially quantified
     */

    /* Terminals */
    if (a == sylvan_true && b == sylvan_true) return sylvan_true;
    if (a == sylvan_false) return sylvan_false;
    if (b == sylvan_false) return sylvan_false;
    if (sylvan_set_isempty(vars)) return a;

    /* Perhaps execute garbage collection */
    sylvan_gc_test();

    /* Count operation */
    sylvan_stats_count(BDD_RELNEXT);

    /* Determine top level */
    bddnode_t na = sylvan_isconst(a) ? 0 : GETNODE(a);
    bddnode_t nb = sylvan_isconst(b) ? 0 : GETNODE(b);

    BDDVAR va = na ? bddnode_getvariable(na) : 0xffffffff;
    BDDVAR vb = nb ? bddnode_getvariable(nb) : 0xffffffff;
    BDDVAR level = va < vb ? va : vb;

    /* Skip vars */
    int is_s_or_t = 0;
    bddnode_t nv = 0;
    if (vars == sylvan_false) {
        is_s_or_t = 1;
    } else {
        nv = GETNODE(vars);
        for (;;) {
            /* check if level is s/t */
            BDDVAR vv = bddnode_getvariable(nv);
            if (level == vv || (level^1) == vv) {
                is_s_or_t = 1;
                break;
            }
            /* check if level < s/t */
            if (level < vv) break;
            vars = node_high(vars, nv); // get next in vars
            if (sylvan_set_isempty(vars)) return a;
            nv = GETNODE(vars);
        }
    }

    /* Consult cache */
    int cachenow = granularity < 2 || prev_level == 0 ? 1 : prev_level / granularity != level / granularity;
    if (cachenow) {
        BDD result;
        if (cache_get3(CACHE_BDD_RELNEXT, a, b, vars, &result)) {
            sylvan_stats_count(BDD_RELNEXT_CACHED);
            return result;
        }
    }

    BDD result;

    if (is_s_or_t) {
        /* Get s and t */
        BDDVAR s = level & (~1);
        BDDVAR t = s+1;

        BDD a0, a1, b0, b1;
        if (na && va == s) {
            a0 = node_low(a, na);
            a1 = node_high(a, na);
        } else {
            a0 = a1 = a;
        }
        if (nb && vb == s) {
            b0 = node_low(b, nb);
            b1 = node_high(b, nb);
        } else {
            b0 = b1 = b;
        }

        BDD b00, b01, b10, b11;
        if (!sylvan_isconst(b0)) {
            bddnode_t nb0 = GETNODE(b0);
            if (bddnode_getvariable(nb0) == t) {
                b00 = node_low(b0, nb0);
                b01 = node_high(b0, nb0);
            } else {
                b00 = b01 = b0;
            }
        } else {
            b00 = b01 = b0;
        }
        if (!sylvan_isconst(b1)) {
            bddnode_t nb1 = GETNODE(b1);
            if (bddnode_getvariable(nb1) == t) {
                b10 = node_low(b1, nb1);
                b11 = node_high(b1, nb1);
            } else {
                b10 = b11 = b1;
            }
        } else {
            b10 = b11 = b1;
        }

        BDD _vars = vars == sylvan_false ? sylvan_false : node_high(vars, nv);

        bdd_refs_spawn(SPAWN(sylvan_relnext, a0, b00, _vars, level));
        bdd_refs_spawn(SPAWN(sylvan_relnext, a1, b10, _vars, level));
        bdd_refs_spawn(SPAWN(sylvan_relnext, a0, b01, _vars, level));
        bdd_refs_spawn(SPAWN(sylvan_relnext, a1, b11, _vars, level));

        BDD f = bdd_refs_sync(SYNC(sylvan_relnext)); bdd_refs_push(f);
        BDD e = bdd_refs_sync(SYNC(sylvan_relnext)); bdd_refs_push(e);
        BDD d = bdd_refs_sync(SYNC(sylvan_relnext)); bdd_refs_push(d);
        BDD c = bdd_refs_sync(SYNC(sylvan_relnext)); bdd_refs_push(c);

        bdd_refs_spawn(SPAWN(sylvan_ite, c, sylvan_true, d, 0)); /* a0 b00  \or  a1 b01 */
        bdd_refs_spawn(SPAWN(sylvan_ite, e, sylvan_true, f, 0)); /* a0 b01  \or  a1 b11 */

        /* R1 */ d = bdd_refs_sync(SYNC(sylvan_ite)); bdd_refs_push(d);
        /* R0 */ c = bdd_refs_sync(SYNC(sylvan_ite)); // not necessary: bdd_refs_push(c);

        bdd_refs_pop(5);
        result = sylvan_makenode(s, c, d);
    } else {
        /* Variable not in vars! Take a, quantify b */
        BDD a0, a1, b0, b1;
        if (na && va == level) {
            a0 = node_low(a, na);
            a1 = node_high(a, na);
        } else {
            a0 = a1 = a;
        }
        if (nb && vb == level) {
            b0 = node_low(b, nb);
            b1 = node_high(b, nb);
        } else {
            b0 = b1 = b;
        }

        if (b0 != b1) {
            if (a0 == a1) {
                /* Quantify "b" variables */
                bdd_refs_spawn(SPAWN(sylvan_relnext, a0, b0, vars, level));
                bdd_refs_spawn(SPAWN(sylvan_relnext, a1, b1, vars, level));

                BDD r1 = bdd_refs_sync(SYNC(sylvan_relnext));
                bdd_refs_push(r1);
                BDD r0 = bdd_refs_sync(SYNC(sylvan_relnext));
                bdd_refs_push(r0);
                result = sylvan_or(r0, r1);
                bdd_refs_pop(2);
            } else {
                /* Quantify "b" variables, but keep "a" variables */
                bdd_refs_spawn(SPAWN(sylvan_relnext, a0, b0, vars, level));
                bdd_refs_spawn(SPAWN(sylvan_relnext, a0, b1, vars, level));
                bdd_refs_spawn(SPAWN(sylvan_relnext, a1, b0, vars, level));
                bdd_refs_spawn(SPAWN(sylvan_relnext, a1, b1, vars, level));

                BDD r11 = bdd_refs_sync(SYNC(sylvan_relnext));
                bdd_refs_push(r11);
                BDD r10 = bdd_refs_sync(SYNC(sylvan_relnext));
                bdd_refs_push(r10);
                BDD r01 = bdd_refs_sync(SYNC(sylvan_relnext));
                bdd_refs_push(r01);
                BDD r00 = bdd_refs_sync(SYNC(sylvan_relnext));
                bdd_refs_push(r00);

                bdd_refs_spawn(SPAWN(sylvan_ite, r00, sylvan_true, r01, 0));
                bdd_refs_spawn(SPAWN(sylvan_ite, r10, sylvan_true, r11, 0));

                BDD r1 = bdd_refs_sync(SYNC(sylvan_ite));
                bdd_refs_push(r1);
                BDD r0 = bdd_refs_sync(SYNC(sylvan_ite));
                bdd_refs_pop(5);

                result = sylvan_makenode(level, r0, r1);
            }
        } else {
            /* Keep "a" variables */
            bdd_refs_spawn(SPAWN(sylvan_relnext, a0, b0, vars, level));
            bdd_refs_spawn(SPAWN(sylvan_relnext, a1, b1, vars, level));

            BDD r1 = bdd_refs_sync(SYNC(sylvan_relnext));
            bdd_refs_push(r1);
            BDD r0 = bdd_refs_sync(SYNC(sylvan_relnext));
            bdd_refs_pop(1);
            result = sylvan_makenode(level, r0, r1);
        }
    }

    if (cachenow) {
        if (cache_put3(CACHE_BDD_RELNEXT, a, b, vars, result)) sylvan_stats_count(BDD_RELNEXT_CACHEDPUT);
    }

    return result;
}

TASK_IMPL_4(BDD, sylvan_relprev, BDD, a, BDD, b, BDDSET, vars, BDDVAR, prev_level)
{
    /* Compute \exists x: A(s,x) \and B(x,t)
     * if vars == sylvan_false, then every level is in s or t
     * any other levels (outside s,t) in A are ignored / existentially quantified
     */

    /* Terminals */
    if (a == sylvan_true && b == sylvan_true) return sylvan_true;
    if (a == sylvan_false) return sylvan_false;
    if (b == sylvan_false) return sylvan_false;
    if (sylvan_set_isempty(vars)) return b;

    /* Perhaps execute garbage collection */
    sylvan_gc_test();

    /* Count operation */
    sylvan_stats_count(BDD_RELPREV);

    /* Determine top level */
    bddnode_t na = sylvan_isconst(a) ? 0 : GETNODE(a);
    bddnode_t nb = sylvan_isconst(b) ? 0 : GETNODE(b);

    BDDVAR va = na ? bddnode_getvariable(na) : 0xffffffff;
    BDDVAR vb = nb ? bddnode_getvariable(nb) : 0xffffffff;
    BDDVAR level = va < vb ? va : vb;

    /* Skip vars */
    int is_s_or_t = 0;
    bddnode_t nv = 0;
    if (vars == sylvan_false) {
        is_s_or_t = 1;
    } else {
        nv = GETNODE(vars);
        for (;;) {
            /* check if level is s/t */
            BDDVAR vv = bddnode_getvariable(nv);
            if (level == vv || (level^1) == vv) {
                is_s_or_t = 1;
                break;
            }
            /* check if level < s/t */
            if (level < vv) break;
            vars = node_high(vars, nv); // get next in vars
            if (sylvan_set_isempty(vars)) return b;
            nv = GETNODE(vars);
        }
    }

    /* Consult cache */
    int cachenow = granularity < 2 || prev_level == 0 ? 1 : prev_level / granularity != level / granularity;
    if (cachenow) {
        BDD result;
        if (cache_get3(CACHE_BDD_RELPREV, a, b, vars, &result)) {
            sylvan_stats_count(BDD_RELPREV_CACHED);
            return result;
        }
    }

    BDD result;

    if (is_s_or_t) {
        /* Get s and t */
        BDDVAR s = level & (~1);
        BDDVAR t = s+1;

        BDD a0, a1, b0, b1;
        if (na && va == s) {
            a0 = node_low(a, na);
            a1 = node_high(a, na);
        } else {
            a0 = a1 = a;
        }
        if (nb && vb == s) {
            b0 = node_low(b, nb);
            b1 = node_high(b, nb);
        } else {
            b0 = b1 = b;
        }

        BDD a00, a01, a10, a11;
        if (!sylvan_isconst(a0)) {
            bddnode_t na0 = GETNODE(a0);
            if (bddnode_getvariable(na0) == t) {
                a00 = node_low(a0, na0);
                a01 = node_high(a0, na0);
            } else {
                a00 = a01 = a0;
            }
        } else {
            a00 = a01 = a0;
        }
        if (!sylvan_isconst(a1)) {
            bddnode_t na1 = GETNODE(a1);
            if (bddnode_getvariable(na1) == t) {
                a10 = node_low(a1, na1);
                a11 = node_high(a1, na1);
            } else {
                a10 = a11 = a1;
            }
        } else {
            a10 = a11 = a1;
        }

        BDD b00, b01, b10, b11;
        if (!sylvan_isconst(b0)) {
            bddnode_t nb0 = GETNODE(b0);
            if (bddnode_getvariable(nb0) == t) {
                b00 = node_low(b0, nb0);
                b01 = node_high(b0, nb0);
            } else {
                b00 = b01 = b0;
            }
        } else {
            b00 = b01 = b0;
        }
        if (!sylvan_isconst(b1)) {
            bddnode_t nb1 = GETNODE(b1);
            if (bddnode_getvariable(nb1) == t) {
                b10 = node_low(b1, nb1);
                b11 = node_high(b1, nb1);
            } else {
                b10 = b11 = b1;
            }
        } else {
            b10 = b11 = b1;
        }

        BDD _vars;
        if (vars != sylvan_false) {
            _vars = node_high(vars, nv);
            if (sylvan_set_var(_vars) == t) _vars = sylvan_set_next(_vars);
        } else {
            _vars = sylvan_false;
        }

        if (b00 == b01) {
            bdd_refs_spawn(SPAWN(sylvan_relprev, a00, b0, _vars, level));
            bdd_refs_spawn(SPAWN(sylvan_relprev, a10, b0, _vars, level));
        } else {
            bdd_refs_spawn(SPAWN(sylvan_relprev, a00, b00, _vars, level));
            bdd_refs_spawn(SPAWN(sylvan_relprev, a00, b01, _vars, level));
            bdd_refs_spawn(SPAWN(sylvan_relprev, a10, b00, _vars, level));
            bdd_refs_spawn(SPAWN(sylvan_relprev, a10, b01, _vars, level));
        }

        if (b10 == b11) {
            bdd_refs_spawn(SPAWN(sylvan_relprev, a01, b1, _vars, level));
            bdd_refs_spawn(SPAWN(sylvan_relprev, a11, b1, _vars, level));
        } else {
            bdd_refs_spawn(SPAWN(sylvan_relprev, a01, b10, _vars, level));
            bdd_refs_spawn(SPAWN(sylvan_relprev, a01, b11, _vars, level));
            bdd_refs_spawn(SPAWN(sylvan_relprev, a11, b10, _vars, level));
            bdd_refs_spawn(SPAWN(sylvan_relprev, a11, b11, _vars, level));
        }

        BDD r00, r01, r10, r11;

        if (b10 == b11) {
            r11 = bdd_refs_push(bdd_refs_sync(SYNC(sylvan_relprev)));
            r01 = bdd_refs_push(bdd_refs_sync(SYNC(sylvan_relprev)));
        } else {
            BDD r111 = bdd_refs_push(bdd_refs_sync(SYNC(sylvan_relprev)));
            BDD r110 = bdd_refs_push(bdd_refs_sync(SYNC(sylvan_relprev)));
            r11 = sylvan_makenode(t, r110, r111);
            bdd_refs_pop(2);
            bdd_refs_push(r11);
            BDD r011 = bdd_refs_push(bdd_refs_sync(SYNC(sylvan_relprev)));
            BDD r010 = bdd_refs_push(bdd_refs_sync(SYNC(sylvan_relprev)));
            r01 = sylvan_makenode(t, r010, r011);
            bdd_refs_pop(2);
            bdd_refs_push(r01);
        }

        if (b00 == b01) {
            r10 = bdd_refs_push(bdd_refs_sync(SYNC(sylvan_relprev)));
            r00 = bdd_refs_push(bdd_refs_sync(SYNC(sylvan_relprev)));
        } else {
            BDD r101 = bdd_refs_push(bdd_refs_sync(SYNC(sylvan_relprev)));
            BDD r100 = bdd_refs_push(bdd_refs_sync(SYNC(sylvan_relprev)));
            r10 = sylvan_makenode(t, r100, r101);
            bdd_refs_pop(2);
            bdd_refs_push(r10);
            BDD r001 = bdd_refs_push(bdd_refs_sync(SYNC(sylvan_relprev)));
            BDD r000 = bdd_refs_push(bdd_refs_sync(SYNC(sylvan_relprev)));
            r00 = sylvan_makenode(t, r000, r001);
            bdd_refs_pop(2);
            bdd_refs_push(r00);
         }

        bdd_refs_spawn(SPAWN(sylvan_and, sylvan_not(r00), sylvan_not(r01), 0));
        bdd_refs_spawn(SPAWN(sylvan_and, sylvan_not(r10), sylvan_not(r11), 0));

        BDD r1 = sylvan_not(bdd_refs_push(bdd_refs_sync(SYNC(sylvan_and))));
        BDD r0 = sylvan_not(bdd_refs_sync(SYNC(sylvan_and)));
        bdd_refs_pop(5);
        result = sylvan_makenode(s, r0, r1);
    } else {
        BDD a0, a1, b0, b1;
        if (na && va == level) {
            a0 = node_low(a, na);
            a1 = node_high(a, na);
        } else {
            a0 = a1 = a;
        }
        if (nb && vb == level) {
            b0 = node_low(b, nb);
            b1 = node_high(b, nb);
        } else {
            b0 = b1 = b;
        }

        if (a0 != a1) {
            if (b0 == b1) {
                /* Quantify "a" variables */
                bdd_refs_spawn(SPAWN(sylvan_relprev, a0, b0, vars, level));
                bdd_refs_spawn(SPAWN(sylvan_relprev, a1, b1, vars, level));

                BDD r1 = bdd_refs_sync(SYNC(sylvan_relprev));
                bdd_refs_push(r1);
                BDD r0 = bdd_refs_sync(SYNC(sylvan_relprev));
                bdd_refs_push(r0);
                result = CALL(sylvan_ite, r0, sylvan_true, r1, 0);
                bdd_refs_pop(2);

            } else {
                /* Quantify "a" variables, but keep "b" variables */
                bdd_refs_spawn(SPAWN(sylvan_relnext, a0, b0, vars, level));
                bdd_refs_spawn(SPAWN(sylvan_relnext, a1, b0, vars, level));
                bdd_refs_spawn(SPAWN(sylvan_relnext, a0, b1, vars, level));
                bdd_refs_spawn(SPAWN(sylvan_relnext, a1, b1, vars, level));

                BDD r11 = bdd_refs_sync(SYNC(sylvan_relnext));
                bdd_refs_push(r11);
                BDD r01 = bdd_refs_sync(SYNC(sylvan_relnext));
                bdd_refs_push(r01);
                BDD r10 = bdd_refs_sync(SYNC(sylvan_relnext));
                bdd_refs_push(r10);
                BDD r00 = bdd_refs_sync(SYNC(sylvan_relnext));
                bdd_refs_push(r00);

                bdd_refs_spawn(SPAWN(sylvan_ite, r00, sylvan_true, r10, 0));
                bdd_refs_spawn(SPAWN(sylvan_ite, r01, sylvan_true, r11, 0));

                BDD r1 = bdd_refs_sync(SYNC(sylvan_ite));
                bdd_refs_push(r1);
                BDD r0 = bdd_refs_sync(SYNC(sylvan_ite));
                bdd_refs_pop(5);

                result = sylvan_makenode(level, r0, r1);
            }
        } else {
            bdd_refs_spawn(SPAWN(sylvan_relprev, a0, b0, vars, level));
            bdd_refs_spawn(SPAWN(sylvan_relprev, a1, b1, vars, level));

            BDD r1 = bdd_refs_sync(SYNC(sylvan_relprev));
            bdd_refs_push(r1);
            BDD r0 = bdd_refs_sync(SYNC(sylvan_relprev));
            bdd_refs_pop(1);
            result = sylvan_makenode(level, r0, r1);
        }
    }

    if (cachenow) {
        if (cache_put3(CACHE_BDD_RELPREV, a, b, vars, result)) sylvan_stats_count(BDD_RELPREV_CACHEDPUT);
    }

    return result;
}

/**
 * Computes the transitive closure by traversing the BDD recursively.
 * See Y. Matsunaga, P. C. McGeer, R. K. Brayton
 *     On Computing the Transitive Closre of a State Transition Relation
 *     30th ACM Design Automation Conference, 1993.
 */
TASK_IMPL_2(BDD, sylvan_closure, BDD, a, BDDVAR, prev_level)
{
    /* Terminals */
    if (a == sylvan_true) return a;
    if (a == sylvan_false) return a;

    /* Perhaps execute garbage collection */
    sylvan_gc_test();

    /* Count operation */
    sylvan_stats_count(BDD_CLOSURE);

    /* Determine top level */
    bddnode_t n = GETNODE(a);
    BDDVAR level = bddnode_getvariable(n);

    /* Consult cache */
    int cachenow = granularity < 2 || prev_level == 0 ? 1 : prev_level / granularity != level / granularity;
    if (cachenow) {
        BDD result;
        if (cache_get3(CACHE_BDD_CLOSURE, a, 0, 0, &result)) {
            sylvan_stats_count(BDD_CLOSURE_CACHED);
            return result;
        }
    }

    BDDVAR s = level & (~1);
    BDDVAR t = s+1;

    BDD a0, a1;
    if (level == s) {
        a0 = node_low(a, n);
        a1 = node_high(a, n);
    } else {
        a0 = a1 = a;
    }

    BDD a00, a01, a10, a11;
    if (!sylvan_isconst(a0)) {
        bddnode_t na0 = GETNODE(a0);
        if (bddnode_getvariable(na0) == t) {
            a00 = node_low(a0, na0);
            a01 = node_high(a0, na0);
        } else {
            a00 = a01 = a0;
        }
    } else {
        a00 = a01 = a0;
    }
    if (!sylvan_isconst(a1)) {
        bddnode_t na1 = GETNODE(a1);
        if (bddnode_getvariable(na1) == t) {
            a10 = node_low(a1, na1);
            a11 = node_high(a1, na1);
        } else {
            a10 = a11 = a1;
        }
    } else {
        a10 = a11 = a1;
    }

    BDD u1 = CALL(sylvan_closure, a11, level);
    bdd_refs_push(u1);
    /* u3 = */ bdd_refs_spawn(SPAWN(sylvan_relprev, a01, u1, sylvan_false, level));
    BDD u2 = CALL(sylvan_relprev, u1, a10, sylvan_false, level);
    bdd_refs_push(u2);
    BDD e = CALL(sylvan_relprev, a01, u2, sylvan_false, level);
    bdd_refs_push(e);
    e = CALL(sylvan_ite, a00, sylvan_true, e, level);
    bdd_refs_pop(1);
    bdd_refs_push(e);
    e = CALL(sylvan_closure, e, level);
    bdd_refs_pop(1);
    bdd_refs_push(e);
    BDD g = CALL(sylvan_relprev, u2, e, sylvan_false, level);
    bdd_refs_push(g);
    BDD u3 = bdd_refs_sync(SYNC(sylvan_relprev));
    bdd_refs_push(u3);
    BDD f = CALL(sylvan_relprev, e, u3, sylvan_false, level);
    bdd_refs_push(f);
    BDD h = CALL(sylvan_relprev, u2, f, sylvan_false, level);
    bdd_refs_push(h);
    h = CALL(sylvan_ite, u1, sylvan_true, h, level);
    bdd_refs_pop(1);
    bdd_refs_push(h);

    BDD r0, r1;
    /* R0 */ r0 = sylvan_makenode(t, e, f);
    bdd_refs_pop(7);
    bdd_refs_push(r0);
    /* R1 */ r1 = sylvan_makenode(t, g, h);
    bdd_refs_pop(1);
    BDD result = sylvan_makenode(s, r0, r1);

    if (cachenow) {
        if (cache_put3(CACHE_BDD_CLOSURE, a, 0, 0, result)) sylvan_stats_count(BDD_CLOSURE_CACHEDPUT);
    }

    return result;
}


/**
 * Function composition
 */
TASK_IMPL_3(BDD, sylvan_compose, BDD, a, BDDMAP, map, BDDVAR, prev_level)
{
    /* Trivial cases */
    if (a == sylvan_false || a == sylvan_true) return a;
    if (sylvan_map_isempty(map)) return a;

    /* Perhaps execute garbage collection */
    sylvan_gc_test();

    /* Count operation */
    sylvan_stats_count(BDD_COMPOSE);

    /* Determine top level */
    bddnode_t n = GETNODE(a);
    BDDVAR level = bddnode_getvariable(n);

    /* Skip map */
    bddnode_t map_node = GETNODE(map);
    BDDVAR map_var = bddnode_getvariable(map_node);
    while (map_var < level) {
        map = node_low(map, map_node);
        if (sylvan_map_isempty(map)) return a;
        map_node = GETNODE(map);
        map_var = bddnode_getvariable(map_node);
    }

    /* Consult cache */
    int cachenow = granularity < 2 || prev_level == 0 ? 1 : prev_level / granularity != level / granularity;
    if (cachenow) {
        BDD result;
        if (cache_get3(CACHE_BDD_COMPOSE, a, map, 0, &result)) {
            sylvan_stats_count(BDD_COMPOSE_CACHED);
            return result;
        }
    }

    /* Recursively calculate low and high */
    bdd_refs_spawn(SPAWN(sylvan_compose, node_low(a, n), map, level));
    BDD high = CALL(sylvan_compose, node_high(a, n), map, level);
    bdd_refs_push(high);
    BDD low = bdd_refs_sync(SYNC(sylvan_compose));
    bdd_refs_push(low);

    /* Calculate result */
    BDD root = map_var == level ? node_high(map, map_node) : sylvan_ithvar(level);
    bdd_refs_push(root);
    BDD result = CALL(sylvan_ite, root, high, low, 0);
    bdd_refs_pop(3);

    if (cachenow) {
        if (cache_put3(CACHE_BDD_COMPOSE, a, map, 0, result)) sylvan_stats_count(BDD_COMPOSE_CACHEDPUT);
    }

    return result;
}

/**
 * Count number of nodes in BDD
 */
uint64_t sylvan_nodecount_do_1(BDD a)
{
    if (sylvan_isconst(a)) return 0;
    bddnode_t na = GETNODE(a);
    if (bddnode_getmark(na)) return 0;
    bddnode_setmark(na, 1);
    uint64_t result = 1;
    result += sylvan_nodecount_do_1(bddnode_getlow(na));
    result += sylvan_nodecount_do_1(bddnode_gethigh(na));
    return result;
}

void sylvan_nodecount_do_2(BDD a)
{
    if (sylvan_isconst(a)) return;
    bddnode_t na = GETNODE(a);
    if (!bddnode_getmark(na)) return;
    bddnode_setmark(na, 0);
    sylvan_nodecount_do_2(bddnode_getlow(na));
    sylvan_nodecount_do_2(bddnode_gethigh(na));
}

size_t sylvan_nodecount(BDD a)
{
    uint32_t result = sylvan_nodecount_do_1(a);
    sylvan_nodecount_do_2(a);
    return result;
}

/**
 * Calculate the number of distinct paths to True.
 */
TASK_IMPL_2(double, sylvan_pathcount, BDD, bdd, BDDVAR, prev_level)
{
    /* Trivial cases */
    if (bdd == sylvan_false) return 0.0;
    if (bdd == sylvan_true) return 1.0;

    /* Perhaps execute garbage collection */
    sylvan_gc_test();

    sylvan_stats_count(BDD_PATHCOUNT);

    BDD level = sylvan_var(bdd);

    /* Consult cache */
    int cachenow = granularity < 2 || prev_level == 0 ? 1 : prev_level / granularity != level / granularity;
    if (cachenow) {
        double result;
        if (cache_get3(CACHE_BDD_PATHCOUNT, bdd, 0, 0, (uint64_t*)&result)) {
            sylvan_stats_count(BDD_PATHCOUNT_CACHED);
            return result;
        }
    }

    SPAWN(sylvan_pathcount, sylvan_low(bdd), level);
    SPAWN(sylvan_pathcount, sylvan_high(bdd), level);
    double res1 = SYNC(sylvan_pathcount);
    res1 += SYNC(sylvan_pathcount);

    if (cachenow) {
        if (cache_put3(CACHE_BDD_PATHCOUNT, bdd, 0, 0, *(uint64_t*)&res1)) sylvan_stats_count(BDD_PATHCOUNT_CACHEDPUT);
    }

    return res1;
}

/**
 * Calculate the number of satisfying variable assignments according to <variables>.
 */
TASK_IMPL_3(double, sylvan_satcount, BDD, bdd, BDDSET, variables, BDDVAR, prev_level)
{
    /* Trivial cases */
    if (bdd == sylvan_false) return 0.0;
    if (bdd == sylvan_true) return powl(2.0L, sylvan_set_count(variables));

    /* Perhaps execute garbage collection */
    sylvan_gc_test();

    sylvan_stats_count(BDD_SATCOUNT);

    /* Count variables before var(bdd) */
    size_t skipped = 0;
    BDDVAR var = sylvan_var(bdd);
    bddnode_t set_node = GETNODE(variables);
    BDDVAR set_var = bddnode_getvariable(set_node);
    while (var != set_var) {
        skipped++;
        variables = node_high(variables, set_node);
        // if this assertion fails, then variables is not the support of <bdd>
        assert(!sylvan_set_isempty(variables));
        set_node = GETNODE(variables);
        set_var = bddnode_getvariable(set_node);
    }

    union {
        double d;
        uint64_t s;
    } hack;

    /* Consult cache */
    int cachenow = granularity < 2 || prev_level == 0 ? 1 : prev_level / granularity != var / granularity;
    if (cachenow) {
        if (cache_get3(CACHE_BDD_SATCOUNT, bdd, variables, 0, &hack.s)) {
            sylvan_stats_count(BDD_SATCOUNT_CACHED);
            return hack.d * powl(2.0L, skipped);
        }
    }

    SPAWN(sylvan_satcount, sylvan_high(bdd), node_high(variables, set_node), var);
    double low = CALL(sylvan_satcount, sylvan_low(bdd), node_high(variables, set_node), var);
    double result = low + SYNC(sylvan_satcount);

    if (cachenow) {
        hack.d = result;
        if (cache_put3(CACHE_BDD_SATCOUNT, bdd, variables, 0, hack.s)) sylvan_stats_count(BDD_SATCOUNT_CACHEDPUT);
    }

    return result * powl(2.0L, skipped);
}

int
sylvan_sat_one(BDD bdd, BDDSET vars, uint8_t *str)
{
    if (bdd == sylvan_false) return 0;
    if (str == NULL) return 0;
    if (sylvan_set_isempty(vars)) return 1;

    for (;;) {
        bddnode_t n_vars = GETNODE(vars);
        if (bdd == sylvan_true) {
            *str = 0;
        } else {
            bddnode_t n_bdd = GETNODE(bdd);
            if (bddnode_getvariable(n_bdd) != bddnode_getvariable(n_vars)) {
                *str = 0;
            } else {
                if (node_low(bdd, n_bdd) == sylvan_false) {
                    // take high edge
                    *str = 1;
                    bdd = node_high(bdd, n_bdd);
                } else {
                    // take low edge
                    *str = 0;
                    bdd = node_low(bdd, n_bdd);
                }
            }
        }
        vars = node_high(vars, n_vars);
        if (sylvan_set_isempty(vars)) break;
        str++;
    }

    return 1;
}

BDD
sylvan_sat_one_bdd(BDD bdd)
{
    if (bdd == sylvan_false) return sylvan_false;
    if (bdd == sylvan_true) return sylvan_true;

    bddnode_t node = GETNODE(bdd);
    BDD low = node_low(bdd, node);
    BDD high = node_high(bdd, node);

    BDD m;

    BDD result;
    if (low == sylvan_false) {
        m = sylvan_sat_one_bdd(high);
        result = sylvan_makenode(bddnode_getvariable(node), sylvan_false, m);
    } else if (high == sylvan_false) {
        m = sylvan_sat_one_bdd(low);
        result = sylvan_makenode(bddnode_getvariable(node), m, sylvan_false);
    } else {
        if (rand() & 0x2000) {
            m = sylvan_sat_one_bdd(low);
            result = sylvan_makenode(bddnode_getvariable(node), m, sylvan_false);
        } else {
            m = sylvan_sat_one_bdd(high);
            result = sylvan_makenode(bddnode_getvariable(node), sylvan_false, m);
        }
    }

    return result;
}

BDD
sylvan_cube(BDDSET vars, uint8_t *cube)
{
    if (sylvan_set_isempty(vars)) return sylvan_true;

    bddnode_t n = GETNODE(vars);
    BDDVAR v = bddnode_getvariable(n);
    vars = node_high(vars, n);

    BDD result = sylvan_cube(vars, cube+1);
    if (*cube == 0) {
        result = sylvan_makenode(v, result, sylvan_false);
    } else if (*cube == 1) {
        result = sylvan_makenode(v, sylvan_false, result);
    }

    return result;
}

TASK_IMPL_3(BDD, sylvan_union_cube, BDD, bdd, BDDSET, vars, uint8_t *, cube)
{
    /* Terminal cases */
    if (bdd == sylvan_true) return sylvan_true;
    if (bdd == sylvan_false) return sylvan_cube(vars, cube);
    if (sylvan_set_isempty(vars)) return sylvan_true;

    bddnode_t nv = GETNODE(vars);

    for (;;) {
        if (*cube == 0 || *cube == 1) break;
        // *cube should be 2
        cube++;
        vars = node_high(vars, nv);
        if (sylvan_set_isempty(vars)) return sylvan_true;
        nv = GETNODE(vars);
    }

    sylvan_gc_test();

    // missing: SV_CNT_OP

    bddnode_t n = GETNODE(bdd);
    BDD result = bdd;
    BDDVAR v = bddnode_getvariable(nv);
    BDDVAR n_level = bddnode_getvariable(n);

    if (v < n_level) {
        vars = node_high(vars, nv);
        if (*cube == 0) {
            result = sylvan_union_cube(bdd, vars, cube+1);
            result = sylvan_makenode(v, result, bdd);
        } else /* *cube == 1 */ {
            result = sylvan_union_cube(bdd, vars, cube+1);
            result = sylvan_makenode(v, bdd, result);
        }
    } else if (v > n_level) {
        BDD high = node_high(bdd, n);
        BDD low = node_low(bdd, n);
        SPAWN(sylvan_union_cube, high, vars, cube);
        BDD new_low = sylvan_union_cube(low, vars, cube);
        bdd_refs_push(new_low);
        BDD new_high = SYNC(sylvan_union_cube);
        bdd_refs_pop(1);
        if (new_low != low || new_high != high) {
            result = sylvan_makenode(n_level, new_low, new_high);
        }
    } else /* v == n_level */ {
        vars = node_high(vars, nv);
        BDD high = node_high(bdd, n);
        BDD low = node_low(bdd, n);
        if (*cube == 0) {
            BDD new_low = sylvan_union_cube(low, vars, cube+1);
            if (new_low != low) {
                result = sylvan_makenode(n_level, new_low, high);
            }
        } else /* *cube == 1 */ {
            BDD new_high = sylvan_union_cube(high, vars, cube+1);
            if (new_high != high) {
                result = sylvan_makenode(n_level, low, new_high);
            }
        }
    }

    return result;
}

struct bdd_path
{
    struct bdd_path *prev;
    BDDVAR var;
    int8_t val; // 0=false, 1=true, 2=both
};

VOID_TASK_5(sylvan_enum_do, BDD, bdd, BDDSET, vars, enum_cb, cb, void*, context, struct bdd_path*, path)
{
    if (bdd == sylvan_false) return;

    if (sylvan_set_isempty(vars)) {
        /* bdd should now be true */
        assert(bdd == sylvan_true);
        /* compute length of path */
        int i=0;
        struct bdd_path *pp;
        for (pp = path; pp != NULL; pp = pp->prev) i++;
        /* if length is 0 (enum called with empty vars??), return */
        if (i == 0) return;
        /* fill cube and vars with trace */
        uint8_t cube[i];
        BDDVAR vars[i];
        int j=0;
        for (pp = path; pp != NULL; pp = pp->prev) {
            cube[i-j-1] = pp->val;
            vars[i-j-1] = pp->var;
            j++;
        }
        /* call callback */
        WRAP(cb, context, vars, cube, i);
        return;
    }

    BDDVAR var = sylvan_var(vars);
    vars = sylvan_set_next(vars);
    BDDVAR bdd_var = sylvan_var(bdd);

    /* assert var <= bdd_var */
    if (bdd == sylvan_true || var < bdd_var) {
        struct bdd_path pp0 = (struct bdd_path){path, var, 0};
        CALL(sylvan_enum_do, bdd, vars, cb, context, &pp0);
        struct bdd_path pp1 = (struct bdd_path){path, var, 1};
        CALL(sylvan_enum_do, bdd, vars, cb, context, &pp1);
    } else if (var == bdd_var) {
        struct bdd_path pp0 = (struct bdd_path){path, var, 0};
        CALL(sylvan_enum_do, sylvan_low(bdd), vars, cb, context, &pp0);
        struct bdd_path pp1 = (struct bdd_path){path, var, 1};
        CALL(sylvan_enum_do, sylvan_high(bdd), vars, cb, context, &pp1);
    } else {
        printf("var %u not expected (expecting %u)!\n", bdd_var, var);
        assert(var <= bdd_var);
    }
}

VOID_TASK_5(sylvan_enum_par_do, BDD, bdd, BDDSET, vars, enum_cb, cb, void*, context, struct bdd_path*, path)
{
    if (bdd == sylvan_false) return;

    if (sylvan_set_isempty(vars)) {
        /* bdd should now be true */
        assert(bdd == sylvan_true);
        /* compute length of path */
        int i=0;
        struct bdd_path *pp;
        for (pp = path; pp != NULL; pp = pp->prev) i++;
        /* if length is 0 (enum called with empty vars??), return */
        if (i == 0) return;
        /* fill cube and vars with trace */
        uint8_t cube[i];
        BDDVAR vars[i];
        int j=0;
        for (pp = path; pp != NULL; pp = pp->prev) {
            cube[i-j-1] = pp->val;
            vars[i-j-1] = pp->var;
            j++;
        }
        /* call callback */
        WRAP(cb, context, vars, cube, i);
        return;
    }

    BDD var = sylvan_var(vars);
    vars = sylvan_set_next(vars);
    BDD bdd_var = sylvan_var(bdd);

    /* assert var <= bdd_var */
    if (var < bdd_var) {
        struct bdd_path pp1 = (struct bdd_path){path, var, 1};
        SPAWN(sylvan_enum_par_do, bdd, vars, cb, context, &pp1);
        struct bdd_path pp0 = (struct bdd_path){path, var, 0};
        CALL(sylvan_enum_par_do, bdd, vars, cb, context, &pp0);
        SYNC(sylvan_enum_par_do);
    } else if (var == bdd_var) {
        struct bdd_path pp1 = (struct bdd_path){path, var, 1};
        SPAWN(sylvan_enum_par_do, sylvan_high(bdd), vars, cb, context, &pp1);
        struct bdd_path pp0 = (struct bdd_path){path, var, 0};
        CALL(sylvan_enum_par_do, sylvan_low(bdd), vars, cb, context, &pp0);
        SYNC(sylvan_enum_par_do);
    } else {
        assert(var <= bdd_var);
    }
}

VOID_TASK_IMPL_4(sylvan_enum, BDD, bdd, BDDSET, vars, enum_cb, cb, void*, context)
{
    CALL(sylvan_enum_do, bdd, vars, cb, context, 0);
}

VOID_TASK_IMPL_4(sylvan_enum_par, BDD, bdd, BDDSET, vars, enum_cb, cb, void*, context)
{
    CALL(sylvan_enum_par_do, bdd, vars, cb, context, 0);
}

TASK_5(BDD, sylvan_collect_do, BDD, bdd, BDDSET, vars, sylvan_collect_cb, cb, void*, context, struct bdd_path*, path)
{
    if (bdd == sylvan_false) return sylvan_false;

    if (sylvan_set_isempty(vars)) {
        /* compute length of path */
        int i=0;
        struct bdd_path *pp;
        for (pp = path; pp != NULL; pp = pp->prev) i++;
        /* if length is 0 (enum called with empty vars??), return */
        if (i == 0) return WRAP(cb, context, NULL);
        /* fill cube and vars with trace */
        uint8_t cube[i];
        int j=0;
        for (pp = path; pp != NULL; pp = pp->prev) {
            cube[i-j-1] = pp->val;
            j++;
        }
        /* call callback */
        return WRAP(cb, context, cube);
    } else {
        BDD var = sylvan_var(vars);
        vars = sylvan_set_next(vars);
        BDD bdd_var = sylvan_var(bdd);

        /* if fails, then <bdd> has variables not in <vars> */
        assert(var <= bdd_var);

        struct bdd_path pp1 = (struct bdd_path){path, var, 1};
        struct bdd_path pp0 = (struct bdd_path){path, var, 0};
        if (var < bdd_var) {
            bdd_refs_spawn(SPAWN(sylvan_collect_do, bdd, vars, cb, context, &pp1));
            BDD low = bdd_refs_push(CALL(sylvan_collect_do, bdd, vars, cb, context, &pp0));
            BDD high = bdd_refs_push(bdd_refs_sync(SYNC(sylvan_collect_do)));
            BDD res = sylvan_or(low, high);
            bdd_refs_pop(2);
            return res;
        } else if (var == bdd_var) {
            bdd_refs_spawn(SPAWN(sylvan_collect_do, sylvan_high(bdd), vars, cb, context, &pp1));
            BDD low = bdd_refs_push(CALL(sylvan_collect_do, sylvan_low(bdd), vars, cb, context, &pp0));
            BDD high = bdd_refs_push(bdd_refs_sync(SYNC(sylvan_collect_do)));
            BDD res = sylvan_or(low, high);
            bdd_refs_pop(2);
            return res;
        } else {
            return sylvan_invalid; // unreachable
        }
    }
}

TASK_IMPL_4(BDD, sylvan_collect, BDD, bdd, BDDSET, vars, sylvan_collect_cb, cb, void*, context)
{
    return CALL(sylvan_collect_do, bdd, vars, cb, context, 0);
}

/**
 * IMPLEMENTATION OF BDDSET
 */

int
sylvan_set_in(BDDSET set, BDDVAR level)
{
    while (!sylvan_set_isempty(set)) {
        bddnode_t n = GETNODE(set);
        BDDVAR n_level = bddnode_getvariable(n);
        if (n_level == level) return 1;
        if (n_level > level) return 0; // BDDs are ordered
        set = node_high(set, n);
    }

    return 0;
}

size_t
sylvan_set_count(BDDSET set)
{
    size_t result = 0;
    for (;!sylvan_set_isempty(set);set = sylvan_set_next(set)) result++;
    return result;
}

void
sylvan_set_toarray(BDDSET set, BDDVAR *arr)
{
    size_t i = 0;
    while (!sylvan_set_isempty(set)) {
        bddnode_t n = GETNODE(set);
        arr[i++] = bddnode_getvariable(n);
        set = node_high(set, n);
    }
}

TASK_IMPL_2(BDDSET, sylvan_set_fromarray, BDDVAR*, arr, size_t, length)
{
    if (length == 0) return sylvan_set_empty();
    BDDSET sub = sylvan_set_fromarray(arr+1, length-1);
    bdd_refs_push(sub);
    BDDSET result = sylvan_set_add(sub, *arr);
    bdd_refs_pop(1);
    return result;
}

void
sylvan_test_isset(BDDSET set)
{
    while (set != sylvan_false) {
        assert(set != sylvan_true);
        assert(llmsset_is_marked(nodes, set));
        bddnode_t n = GETNODE(set);
        assert(node_low(set, n) == sylvan_true);
        set = node_high(set, n);
    }
}

/**
 * IMPLEMENTATION OF BDDMAP
 */

BDDMAP
sylvan_map_add(BDDMAP map, BDDVAR key, BDD value)
{
    if (sylvan_map_isempty(map)) return sylvan_makenode(key, sylvan_map_empty(), value);

    bddnode_t n = GETNODE(map);
    BDDVAR key_m = bddnode_getvariable(n);

    if (key_m < key) {
        // add recursively and rebuild tree
        BDDMAP low = sylvan_map_add(node_low(map, n), key, value);
        BDDMAP result = sylvan_makenode(key_m, low, node_high(map, n));
        return result;
    } else if (key_m > key) {
        return sylvan_makenode(key, map, value);
    } else {
        // replace old
        return sylvan_makenode(key, node_low(map, n), value);
    }
}

BDDMAP
sylvan_map_addall(BDDMAP map_1, BDDMAP map_2)
{
    // one of the maps is empty
    if (sylvan_map_isempty(map_1)) return map_2;
    if (sylvan_map_isempty(map_2)) return map_1;

    bddnode_t n_1 = GETNODE(map_1);
    BDDVAR key_1 = bddnode_getvariable(n_1);

    bddnode_t n_2 = GETNODE(map_2);
    BDDVAR key_2 = bddnode_getvariable(n_2);

    BDDMAP result;
    if (key_1 < key_2) {
        // key_1, recurse on n_1->low, map_2
        BDDMAP low = sylvan_map_addall(node_low(map_1, n_1), map_2);
        result = sylvan_makenode(key_1, low, node_high(map_1, n_1));
    } else if (key_1 > key_2) {
        // key_2, recurse on map_1, n_2->low
        BDDMAP low = sylvan_map_addall(map_1, node_low(map_2, n_2));
        result = sylvan_makenode(key_2, low, node_high(map_2, n_2));
    } else {
        // equal: key_2, recurse on n_1->low, n_2->low
        BDDMAP low = sylvan_map_addall(node_low(map_1, n_1), node_low(map_2, n_2));
        result = sylvan_makenode(key_2, low, node_high(map_2, n_2));
    }
    return result;
}

BDDMAP
sylvan_map_remove(BDDMAP map, BDDVAR key)
{
    if (sylvan_map_isempty(map)) return map;

    bddnode_t n = GETNODE(map);
    BDDVAR key_m = bddnode_getvariable(n);

    if (key_m < key) {
        BDDMAP low = sylvan_map_remove(node_low(map, n), key);
        BDDMAP result = sylvan_makenode(key_m, low, node_high(map, n));
        return result;
    } else if (key_m > key) {
        return map;
    } else {
        return node_low(map, n);
    }
}

BDDMAP
sylvan_map_removeall(BDDMAP map, BDDSET toremove)
{
    if (sylvan_map_isempty(map)) return map;
    if (sylvan_set_isempty(toremove)) return map;

    bddnode_t n_1 = GETNODE(map);
    BDDVAR key_1 = bddnode_getvariable(n_1);

    bddnode_t n_2 = GETNODE(toremove);
    BDDVAR key_2 = bddnode_getvariable(n_2);

    if (key_1 < key_2) {
        BDDMAP low = sylvan_map_removeall(node_low(map, n_1), toremove);
        BDDMAP result = sylvan_makenode(key_1, low, node_high(map, n_1));
        return result;
    } else if (key_1 > key_2) {
        return sylvan_map_removeall(map, node_high(toremove, n_2));
    } else {
        return sylvan_map_removeall(node_low(map, n_1), node_high(toremove, n_2));
    }
}

int
sylvan_map_in(BDDMAP map, BDDVAR key)
{
    while (!sylvan_map_isempty(map)) {
        bddnode_t n = GETNODE(map);
        BDDVAR n_level = bddnode_getvariable(n);
        if (n_level == key) return 1;
        if (n_level > key) return 0; // BDDs are ordered
        map = node_low(map, n);
    }

    return 0;
}

size_t
sylvan_map_count(BDDMAP map)
{
    size_t r=0;
    while (!sylvan_map_isempty(map)) { r++; map=sylvan_map_next(map); }
    return r;
}

BDDMAP
sylvan_set_to_map(BDDSET set, BDD value)
{
    if (sylvan_set_isempty(set)) return sylvan_map_empty();
    bddnode_t set_n = GETNODE(set);
    BDD sub = sylvan_set_to_map(node_high(set, set_n), value);
    BDD result = sylvan_makenode(sub, bddnode_getvariable(set_n), value);
    return result;
}

/**
 * Determine the support of a BDD (all variables used in the BDD)
 */
TASK_IMPL_1(BDD, sylvan_support, BDD, bdd)
{
    if (bdd == sylvan_true || bdd == sylvan_false) return sylvan_set_empty(); // return empty set

    sylvan_gc_test();

    sylvan_stats_count(BDD_SUPPORT);

    BDD result;
    if (cache_get3(CACHE_BDD_SUPPORT, bdd, 0, 0, &result)) {
        sylvan_stats_count(BDD_SUPPORT_CACHED);
        return result;
    }

    bddnode_t n = GETNODE(bdd);
    BDD high, low, set;

    /* compute recursively */
    bdd_refs_spawn(SPAWN(sylvan_support, bddnode_getlow(n)));
    high = bdd_refs_push(CALL(sylvan_support, bddnode_gethigh(n)));
    low = bdd_refs_push(bdd_refs_sync(SYNC(sylvan_support)));

    /* take intersection of support of low and support of high */
    set = sylvan_and(low, high);
    bdd_refs_pop(2);

    /* add current level to set */
    result = sylvan_makenode(bddnode_getvariable(n), sylvan_false, set);

    if (cache_put3(CACHE_BDD_SUPPORT, bdd, 0, 0, result)) sylvan_stats_count(BDD_SUPPORT_CACHEDPUT);
    return result;
}

static void
sylvan_unmark_rec(bddnode_t node)
{
    if (bddnode_getmark(node)) {
        bddnode_setmark(node, 0);
        if (!sylvan_isconst(bddnode_getlow(node))) sylvan_unmark_rec(GETNODE(bddnode_getlow(node)));
        if (!sylvan_isconst(bddnode_gethigh(node))) sylvan_unmark_rec(GETNODE(bddnode_gethigh(node)));
    }
}

/**
 * fprint, print
 */
void
sylvan_fprint(FILE *f, BDD bdd)
{
    sylvan_serialize_reset();
    size_t v = sylvan_serialize_add(bdd);
    fprintf(f, "%s%zu,", bdd&sylvan_complement?"!":"", v);
    sylvan_serialize_totext(f);
}

void
sylvan_print(BDD bdd)
{
    sylvan_fprint(stdout, bdd);
}

/**
 * Output to .DOT files
 */

/***
 * We keep a set [level -> [node]] using AVLset
 */
struct level_to_nodeset {
    BDDVAR level;
    avl_node_t *set;
};

AVL(level_to_nodeset, struct level_to_nodeset)
{
    if (left->level > right->level) return 1;
    if (right->level > left->level) return -1;
    return 0;
}

AVL(nodeset, BDD)
{
    if (*left > *right) return 1;
    if (*right > *left) return -1;
    return 0;
}

/* returns 1 if inserted, 0 if already existed */
static int __attribute__((noinline))
sylvan_dothelper_register(avl_node_t **set, BDD bdd)
{
    struct level_to_nodeset s, *ss;
    s.level = sylvan_var(bdd);
    ss = level_to_nodeset_search(*set, &s);
    if (ss == NULL) {
        s.set = NULL;
        ss = level_to_nodeset_put(set, &s, NULL);
    }
    assert(ss != NULL);
    return nodeset_insert(&ss->set, &bdd);
}

static void
sylvan_fprintdot_rec(FILE *out, BDD bdd, avl_node_t **levels)
{
    bdd = BDD_STRIPMARK(bdd);
    if (bdd == sylvan_false) return;
    if (!sylvan_dothelper_register(levels, bdd)) return;

    BDD low = sylvan_low(bdd);
    BDD high = sylvan_high(bdd);
    fprintf(out, "\"%" PRIx64 "\" [label=\"%d\"];\n", bdd, sylvan_var(bdd));
    fprintf(out, "\"%" PRIx64 "\" -> \"%" PRIx64 "\" [style=dashed];\n", bdd, low);
    fprintf(out, "\"%" PRIx64 "\" -> \"%" PRIx64 "\" [style=solid dir=both arrowtail=%s];\n", bdd, BDD_STRIPMARK(high), BDD_HASMARK(high) ? "dot" : "none");
    sylvan_fprintdot_rec(out, low, levels);
    sylvan_fprintdot_rec(out, high, levels);
}

void
sylvan_fprintdot(FILE *out, BDD bdd)
{
    fprintf(out, "digraph \"DD\" {\n");
    fprintf(out, "graph [dpi = 300];\n");
    fprintf(out, "center = true;\n");
    fprintf(out, "edge [dir = forward];\n");
    fprintf(out, "0 [shape=box, label=\"0\", style=filled, shape=box, height=0.3, width=0.3];\n");
    fprintf(out, "root [style=invis];\n");
    fprintf(out, "root -> \"%" PRIx64 "\" [style=solid dir=both arrowtail=%s];\n", BDD_STRIPMARK(bdd), BDD_HASMARK(bdd) ? "dot" : "none");

    avl_node_t *levels = NULL;
    sylvan_fprintdot_rec(out, bdd, &levels);

    if (levels != NULL) {
        size_t levels_count = avl_count(levels);
        struct level_to_nodeset *arr = level_to_nodeset_toarray(levels);
        size_t i;
        for (i=0;i<levels_count;i++) {
            fprintf(out, "{ rank=same; ");
            size_t node_count = avl_count(arr[i].set);
            size_t j;
            BDD *arr_j = nodeset_toarray(arr[i].set);
            for (j=0;j<node_count;j++) {
                fprintf(out, "\"%" PRIx64 "\"; ", arr_j[j]);
            }
            fprintf(out, "}\n");
        }
        level_to_nodeset_free(&levels);
    }

    fprintf(out, "}\n");
}

void
sylvan_printdot(BDD bdd)
{
    sylvan_fprintdot(stdout, bdd);
}

static void
sylvan_fprintdot_nc_rec(FILE *out, BDD bdd, avl_node_t **levels)
{
    if (bdd == sylvan_true || bdd == sylvan_false) return;
    if (!sylvan_dothelper_register(levels, bdd)) return;

    BDD low = sylvan_low(bdd);
    BDD high = sylvan_high(bdd);
    fprintf(out, "\"%" PRIx64 " [label=\"%d\"];\n", bdd, sylvan_var(bdd));
    fprintf(out, "\"%" PRIx64 "\" -> \"%" PRIx64 "\" [style=dashed];\n", bdd, low);
    fprintf(out, "\"%" PRIx64 "\" -> \"%" PRIx64 "\" [style=solid];\n", bdd, high);
    sylvan_fprintdot_nc_rec(out, low, levels);
    sylvan_fprintdot_nc_rec(out, high, levels);
}

void
sylvan_fprintdot_nc(FILE *out, BDD bdd)
{
    fprintf(out, "digraph \"DD\" {\n");
    fprintf(out, "graph [dpi = 300];\n");
    fprintf(out, "center = true;\n");
    fprintf(out, "edge [dir = forward];\n");
    fprintf(out, "\"%" PRIx64 "\" [shape=box, label=\"F\", style=filled, shape=box, height=0.3, width=0.3];\n", sylvan_false);
    fprintf(out, "\"%" PRIx64 "\" [shape=box, label=\"T\", style=filled, shape=box, height=0.3, width=0.3];\n", sylvan_true);
    fprintf(out, "root [style=invis];\n");
    fprintf(out, "root -> \"%" PRIx64 "\" [style=solid];\n", bdd);

    avl_node_t *levels = NULL;
    sylvan_fprintdot_nc_rec(out, bdd, &levels);

    if (levels != NULL) {
        size_t levels_count = avl_count(levels);
        struct level_to_nodeset *arr = level_to_nodeset_toarray(levels);
        size_t i;
        for (i=0;i<levels_count;i++) {
            fprintf(out, "{ rank=same; ");
            size_t node_count = avl_count(arr[i].set);
            size_t j;
            BDD *arr_j = nodeset_toarray(arr[i].set);
            for (j=0;j<node_count;j++) {
                fprintf(out, "\"%" PRIx64 "\"; ", arr_j[j]);
            }
            fprintf(out, "}\n");
        }
        level_to_nodeset_free(&levels);
    }

    fprintf(out, "}\n");
}

void
sylvan_printdot_nc(BDD bdd)
{
    sylvan_fprintdot_nc(stdout, bdd);
}

/**
 * SERIALIZATION
 */

struct sylvan_ser {
    BDD bdd;
    size_t assigned;
};

// Define a AVL tree type with prefix 'sylvan_ser' holding
// nodes of struct sylvan_ser with the following compare() function...
AVL(sylvan_ser, struct sylvan_ser)
{
    if (left->bdd > right->bdd) return 1;
    if (left->bdd < right->bdd) return -1;
    return 0;
}

// Define a AVL tree type with prefix 'sylvan_ser_reversed' holding
// nodes of struct sylvan_ser with the following compare() function...
AVL(sylvan_ser_reversed, struct sylvan_ser)
{
    if (left->assigned > right->assigned) return 1;
    if (left->assigned < right->assigned) return -1;
    return 0;
}

// Initially, both sets are empty
static avl_node_t *sylvan_ser_set = NULL;
static avl_node_t *sylvan_ser_reversed_set = NULL;

// Start counting (assigning numbers to BDDs) at 1
static size_t sylvan_ser_counter = 1;
static size_t sylvan_ser_done = 0;

// Given a BDD, assign unique numbers to all nodes
static size_t
sylvan_serialize_assign_rec(BDD bdd)
{
    if (sylvan_isnode(bdd)) {
        bddnode_t n = GETNODE(bdd);

        struct sylvan_ser s, *ss;
        s.bdd = BDD_STRIPMARK(bdd);
        ss = sylvan_ser_search(sylvan_ser_set, &s);
        if (ss == NULL) {
            // assign dummy value
            s.assigned = 0;
            ss = sylvan_ser_put(&sylvan_ser_set, &s, NULL);

            // first assign recursively
            sylvan_serialize_assign_rec(bddnode_getlow(n));
            sylvan_serialize_assign_rec(bddnode_gethigh(n));

            // assign real value
            ss->assigned = sylvan_ser_counter++;

            // put a copy in the reversed table
            sylvan_ser_reversed_insert(&sylvan_ser_reversed_set, ss);
        }

        return ss->assigned;
    }

    return BDD_STRIPMARK(bdd);
}

size_t
sylvan_serialize_add(BDD bdd)
{
    return BDD_TRANSFERMARK(bdd, sylvan_serialize_assign_rec(bdd));
}

void
sylvan_serialize_reset()
{
    sylvan_ser_free(&sylvan_ser_set);
    sylvan_ser_free(&sylvan_ser_reversed_set);
    sylvan_ser_counter = 1;
    sylvan_ser_done = 0;
}

size_t
sylvan_serialize_get(BDD bdd)
{
    if (!sylvan_isnode(bdd)) return bdd;
    struct sylvan_ser s, *ss;
    s.bdd = BDD_STRIPMARK(bdd);
    ss = sylvan_ser_search(sylvan_ser_set, &s);
    assert(ss != NULL);
    return BDD_TRANSFERMARK(bdd, ss->assigned);
}

BDD
sylvan_serialize_get_reversed(size_t value)
{
    if (!sylvan_isnode(value)) return value;
    struct sylvan_ser s, *ss;
    s.assigned = BDD_STRIPMARK(value);
    ss = sylvan_ser_reversed_search(sylvan_ser_reversed_set, &s);
    assert(ss != NULL);
    return BDD_TRANSFERMARK(value, ss->bdd);
}

void
sylvan_serialize_totext(FILE *out)
{
    fprintf(out, "[");
    avl_iter_t *it = sylvan_ser_reversed_iter(sylvan_ser_reversed_set);
    struct sylvan_ser *s;

    while ((s=sylvan_ser_reversed_iter_next(it))) {
        BDD bdd = s->bdd;
        bddnode_t n = GETNODE(bdd);
        fprintf(out, "(%zu,%u,%zu,%zu,%u),", s->assigned,
                                             bddnode_getvariable(n),
                                             (size_t)bddnode_getlow(n),
                                             (size_t)BDD_STRIPMARK(bddnode_gethigh(n)),
                                             BDD_HASMARK(bddnode_gethigh(n)) ? 1 : 0);
    }

    sylvan_ser_reversed_iter_free(it);
    fprintf(out, "]");
}

void
sylvan_serialize_tofile(FILE *out)
{
    size_t count = avl_count(sylvan_ser_reversed_set);
    assert(count >= sylvan_ser_done);
    assert(count == sylvan_ser_counter-1);
    count -= sylvan_ser_done;
    fwrite(&count, sizeof(size_t), 1, out);

    struct sylvan_ser *s;
    avl_iter_t *it = sylvan_ser_reversed_iter(sylvan_ser_reversed_set);

    /* Skip already written entries */
    size_t index = 0;
    while (index < sylvan_ser_done && (s=sylvan_ser_reversed_iter_next(it))) {
        index++;
        assert(s->assigned == index);
    }

    while ((s=sylvan_ser_reversed_iter_next(it))) {
        index++;
        assert(s->assigned == index);

        bddnode_t n = GETNODE(s->bdd);

        struct bddnode node;
        bddnode_makenode(&node, bddnode_getvariable(n), sylvan_serialize_get(bddnode_getlow(n)), sylvan_serialize_get(bddnode_gethigh(n)));

        fwrite(&node, sizeof(struct bddnode), 1, out);
    }

    sylvan_ser_done = sylvan_ser_counter-1;
    sylvan_ser_reversed_iter_free(it);
}

void
sylvan_serialize_fromfile(FILE *in)
{
    size_t count, i;
    if (fread(&count, sizeof(size_t), 1, in) != 1) {
        // TODO FIXME return error
        printf("sylvan_serialize_fromfile: file format error, giving up\n");
        exit(-1);
    }

    for (i=1; i<=count; i++) {
        struct bddnode node;
        if (fread(&node, sizeof(struct bddnode), 1, in) != 1) {
            // TODO FIXME return error
            printf("sylvan_serialize_fromfile: file format error, giving up\n");
            exit(-1);
        }

        BDD low = sylvan_serialize_get_reversed(bddnode_getlow(&node));
        BDD high = sylvan_serialize_get_reversed(bddnode_gethigh(&node));

        struct sylvan_ser s;
        s.bdd = sylvan_makenode(bddnode_getvariable(&node), low, high);
        s.assigned = ++sylvan_ser_done; // starts at 0 but we want 1-based...

        sylvan_ser_insert(&sylvan_ser_set, &s);
        sylvan_ser_reversed_insert(&sylvan_ser_reversed_set, &s);
    }
}

/**
 * Generate SHA2 structural hashes.
 * Hashes are independent of location.
 * Mainly useful for debugging purposes.
 */
static void
sylvan_sha2_rec(BDD bdd, SHA256_CTX *ctx)
{
    if (bdd == sylvan_true || bdd == sylvan_false) {
        SHA256_Update(ctx, (void*)&bdd, sizeof(BDD));
        return;
    }

    bddnode_t node = GETNODE(bdd);
    if (bddnode_getmark(node) == 0) {
        bddnode_setmark(node, 1);
        uint32_t level = bddnode_getvariable(node);
        if (BDD_STRIPMARK(bddnode_gethigh(node))) level |= 0x80000000;
        SHA256_Update(ctx, (void*)&level, sizeof(uint32_t));
        sylvan_sha2_rec(bddnode_gethigh(node), ctx);
        sylvan_sha2_rec(bddnode_getlow(node), ctx);
    }
}

void
sylvan_printsha(BDD bdd)
{
    sylvan_fprintsha(stdout, bdd);
}

void
sylvan_fprintsha(FILE *f, BDD bdd)
{
    char buf[80];
    sylvan_getsha(bdd, buf);
    fprintf(f, "%s", buf);
}

void
sylvan_getsha(BDD bdd, char *target)
{
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    sylvan_sha2_rec(bdd, &ctx);
    if (bdd != sylvan_true && bdd != sylvan_false) sylvan_unmark_rec(GETNODE(bdd));
    SHA256_End(&ctx, target);
}

/**
 * Debug tool to check that a BDD is properly ordered.
 * Also that every BDD node is marked 'in-use' in the hash table.
 */
TASK_2(int, sylvan_test_isbdd_rec, BDD, bdd, BDDVAR, parent_var)
{
    if (bdd == sylvan_true || bdd == sylvan_false) return 1;
    assert(llmsset_is_marked(nodes, BDD_STRIPMARK(bdd)));

    sylvan_stats_count(BDD_ISBDD);

    uint64_t result;
    if (cache_get3(CACHE_BDD_ISBDD, bdd, 0, 0, &result)) {
        sylvan_stats_count(BDD_ISBDD_CACHED);
        return result;
    }

    bddnode_t n = GETNODE(bdd);
    BDDVAR var = bddnode_getvariable(n);
    if (var <= parent_var) {
        result = 0;
    } else {
        SPAWN(sylvan_test_isbdd_rec, node_low(bdd, n), var);
        result = (uint64_t)CALL(sylvan_test_isbdd_rec, node_high(bdd, n), var);
        if (!SYNC(sylvan_test_isbdd_rec)) result = 0;
    }

    if (cache_put3(CACHE_BDD_ISBDD, bdd, 0, 0, result)) sylvan_stats_count(BDD_ISBDD_CACHEDPUT);
    return result;
}

TASK_IMPL_1(int, sylvan_test_isbdd, BDD, bdd)
{
    if (bdd == sylvan_true) return 1;
    if (bdd == sylvan_false) return 1;

    assert(llmsset_is_marked(nodes, BDD_STRIPMARK(bdd)));

    bddnode_t n = GETNODE(bdd);
    BDDVAR var = bddnode_getvariable(n);
    SPAWN(sylvan_test_isbdd_rec, node_low(bdd, n), var);
    int result = CALL(sylvan_test_isbdd_rec, node_high(bdd, n), var);
    if (!SYNC(sylvan_test_isbdd_rec)) result = 0;
    return result;
}
