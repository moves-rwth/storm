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

/* Do not include this file directly. Instead, include sylvan.h */

#ifndef SYLVAN_LDD_H
#define SYLVAN_LDD_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef uint64_t MDD;       // Note: low 40 bits only

#define lddmc_false         ((MDD)0)
#define lddmc_true          ((MDD)1)

/* Initialize LDD functionality */
void sylvan_init_ldd();

/* Primitives */
MDD lddmc_makenode(uint32_t value, MDD ifeq, MDD ifneq);
MDD lddmc_extendnode(MDD mdd, uint32_t value, MDD ifeq);
uint32_t lddmc_getvalue(MDD mdd);
MDD lddmc_getdown(MDD mdd);
MDD lddmc_getright(MDD mdd);
MDD lddmc_follow(MDD mdd, uint32_t value);

/**
 * Copy nodes in relations.
 * A copy node represents 'read x, then write x' for every x.
 * In a read-write relation, use copy nodes twice, once on read level, once on write level.
 * Copy nodes are only supported by relprod, relprev and union.
 */

/* Primitive for special 'copy node' (for relprod/relprev) */
MDD lddmc_make_copynode(MDD ifeq, MDD ifneq);
int lddmc_iscopy(MDD mdd);
MDD lddmc_followcopy(MDD mdd);

/* Add or remove external reference to MDD */
MDD lddmc_ref(MDD a);
void lddmc_deref(MDD a);

/* For use in custom mark functions */
VOID_TASK_DECL_1(lddmc_gc_mark_rec, MDD)
#define lddmc_gc_mark_rec(mdd) CALL(lddmc_gc_mark_rec, mdd)

/* Return the number of external references */
size_t lddmc_count_refs();

/* Mark MDD for "notify on dead" */
#define lddmc_notify_ondead(mdd) llmsset_notify_ondead(nodes, mdd)

/* Sanity check - returns depth of MDD including 'true' terminal or 0 for empty set */
#ifndef NDEBUG
size_t lddmc_test_ismdd(MDD mdd);
#endif

/* Operations for model checking */
TASK_DECL_2(MDD, lddmc_union, MDD, MDD);
#define lddmc_union(a, b) CALL(lddmc_union, a, b)

TASK_DECL_2(MDD, lddmc_minus, MDD, MDD);
#define lddmc_minus(a, b) CALL(lddmc_minus, a, b)

TASK_DECL_3(MDD, lddmc_zip, MDD, MDD, MDD*);
#define lddmc_zip(a, b, res) CALL(lddmc_zip, a, b, res)

TASK_DECL_2(MDD, lddmc_intersect, MDD, MDD);
#define lddmc_intersect(a, b) CALL(lddmc_intersect, a, b)

TASK_DECL_3(MDD, lddmc_match, MDD, MDD, MDD);
#define lddmc_match(a, b, proj) CALL(lddmc_match, a, b, proj)

MDD lddmc_union_cube(MDD a, uint32_t* values, size_t count);
int lddmc_member_cube(MDD a, uint32_t* values, size_t count);
MDD lddmc_cube(uint32_t* values, size_t count);

MDD lddmc_union_cube_copy(MDD a, uint32_t* values, int* copy, size_t count);
int lddmc_member_cube_copy(MDD a, uint32_t* values, int* copy, size_t count);
MDD lddmc_cube_copy(uint32_t* values, int* copy, size_t count);

TASK_DECL_3(MDD, lddmc_relprod, MDD, MDD, MDD);
#define lddmc_relprod(a, b, proj) CALL(lddmc_relprod, a, b, proj)

TASK_DECL_4(MDD, lddmc_relprod_union, MDD, MDD, MDD, MDD);
#define lddmc_relprod_union(a, b, meta, un) CALL(lddmc_relprod_union, a, b, meta, un)

/**
 * Calculate all predecessors to a in uni according to rel[proj]
 * <proj> follows the same semantics as relprod
 * i.e. 0 (not in rel), 1 (read+write), 2 (read), 3 (write), -1 (end; rest=0)
 */
TASK_DECL_4(MDD, lddmc_relprev, MDD, MDD, MDD, MDD);
#define lddmc_relprev(a, rel, proj, uni) CALL(lddmc_relprev, a, rel, proj, uni)

// so: proj: -2 (end; quantify rest), -1 (end; keep rest), 0 (quantify), 1 (keep)
TASK_DECL_2(MDD, lddmc_project, MDD, MDD);
#define lddmc_project(mdd, proj) CALL(lddmc_project, mdd, proj)

TASK_DECL_3(MDD, lddmc_project_minus, MDD, MDD, MDD);
#define lddmc_project_minus(mdd, proj, avoid) CALL(lddmc_project_minus, mdd, proj, avoid)

TASK_DECL_4(MDD, lddmc_join, MDD, MDD, MDD, MDD);
#define lddmc_join(a, b, a_proj, b_proj) CALL(lddmc_join, a, b, a_proj, b_proj)

/* Write a DOT representation */
void lddmc_printdot(MDD mdd);
void lddmc_fprintdot(FILE *out, MDD mdd);

void lddmc_fprint(FILE *out, MDD mdd);
void lddmc_print(MDD mdd);

void lddmc_printsha(MDD mdd);
void lddmc_fprintsha(FILE *out, MDD mdd);
void lddmc_getsha(MDD mdd, char *target); // at least 65 bytes...

/**
 * Calculate number of satisfying variable assignments.
 * The set of variables must be >= the support of the MDD.
 * (i.e. all variables in the MDD must be in variables)
 *
 * The cached version uses the operation cache, but is limited to 64-bit floating point numbers.
 */

typedef double lddmc_satcount_double_t;
// if this line below gives an error, modify the above typedef until fixed ;)
typedef char __lddmc_check_float_is_8_bytes[(sizeof(lddmc_satcount_double_t) == sizeof(uint64_t))?1:-1];

TASK_DECL_1(lddmc_satcount_double_t, lddmc_satcount_cached, MDD);
#define lddmc_satcount_cached(mdd) CALL(lddmc_satcount_cached, mdd)

TASK_DECL_1(long double, lddmc_satcount, MDD);
#define lddmc_satcount(mdd) CALL(lddmc_satcount, mdd)

/**
 * A callback for enumerating functions like sat_all_par, collect and match
 * Example:
 * TASK_3(void*, my_function, uint32_t*, values, size_t, count, void*, context) ...
 * For collect, use:
 * TASK_3(MDD, ...)
 */
LACE_TYPEDEF_CB(void, lddmc_enum_cb, uint32_t*, size_t, void*);
LACE_TYPEDEF_CB(MDD, lddmc_collect_cb, uint32_t*, size_t, void*);

VOID_TASK_DECL_5(lddmc_sat_all_par, MDD, lddmc_enum_cb, void*, uint32_t*, size_t);
#define lddmc_sat_all_par(mdd, cb, context) CALL(lddmc_sat_all_par, mdd, cb, context, 0, 0)

VOID_TASK_DECL_3(lddmc_sat_all_nopar, MDD, lddmc_enum_cb, void*);
#define lddmc_sat_all_nopar(mdd, cb, context) CALL(lddmc_sat_all_nopar, mdd, cb, context)

TASK_DECL_5(MDD, lddmc_collect, MDD, lddmc_collect_cb, void*, uint32_t*, size_t);
#define lddmc_collect(mdd, cb, context) CALL(lddmc_collect, mdd, cb, context, 0, 0)

VOID_TASK_DECL_5(lddmc_match_sat_par, MDD, MDD, MDD, lddmc_enum_cb, void*);
#define lddmc_match_sat_par(mdd, match, proj, cb, context) CALL(lddmc_match_sat_par, mdd, match, proj, cb, context)

int lddmc_sat_one(MDD mdd, uint32_t *values, size_t count);
MDD lddmc_sat_one_mdd(MDD mdd);
#define lddmc_pick_cube lddmc_sat_one_mdd

/**
 * Callback functions for visiting nodes.
 * lddmc_visit_seq sequentially visits nodes, down first, then right.
 * lddmc_visit_par visits nodes in parallel (down || right)
 */
LACE_TYPEDEF_CB(int, lddmc_visit_pre_cb, MDD, void*); // int pre(MDD, context)
LACE_TYPEDEF_CB(void, lddmc_visit_post_cb, MDD, void*); // void post(MDD, context)
LACE_TYPEDEF_CB(void, lddmc_visit_init_context_cb, void*, void*, int); // void init_context(context, parent, is_down)

typedef struct lddmc_visit_node_callbacks {
    lddmc_visit_pre_cb lddmc_visit_pre;
    lddmc_visit_post_cb lddmc_visit_post;
    lddmc_visit_init_context_cb lddmc_visit_init_context;
} lddmc_visit_callbacks_t;

VOID_TASK_DECL_4(lddmc_visit_par, MDD, lddmc_visit_callbacks_t*, size_t, void*);
#define lddmc_visit_par(mdd, cbs, ctx_size, context) CALL(lddmc_visit_par, mdd, cbs, ctx_size, context);

VOID_TASK_DECL_4(lddmc_visit_seq, MDD, lddmc_visit_callbacks_t*, size_t, void*);
#define lddmc_visit_seq(mdd, cbs, ctx_size, context) CALL(lddmc_visit_seq, mdd, cbs, ctx_size, context);

size_t lddmc_nodecount(MDD mdd);
void lddmc_nodecount_levels(MDD mdd, size_t *variables);

/**
 * Functional composition
 * For every node at depth <depth>, call function cb (MDD -> MDD).
 * and replace the node by the result of the function
 */
LACE_TYPEDEF_CB(MDD, lddmc_compose_cb, MDD, void*);
TASK_DECL_4(MDD, lddmc_compose, MDD, lddmc_compose_cb, void*, int);
#define lddmc_compose(mdd, cb, context, depth) CALL(lddmc_compose, mdd, cb, context, depth)

/**
 * SAVING:
 * use lddmc_serialize_add on every MDD you want to store
 * use lddmc_serialize_get to retrieve the key of every stored MDD
 * use lddmc_serialize_tofile
 *
 * LOADING:
 * use lddmc_serialize_fromfile (implies lddmc_serialize_reset)
 * use lddmc_serialize_get_reversed for every key
 *
 * MISC:
 * use lddmc_serialize_reset to free all allocated structures
 * use lddmc_serialize_totext to write a textual list of tuples of all MDDs.
 *         format: [(<key>,<level>,<key_low>,<key_high>,<complement_high>),...]
 *
 * for the old lddmc_print functions, use lddmc_serialize_totext
 */
size_t lddmc_serialize_add(MDD mdd);
size_t lddmc_serialize_get(MDD mdd);
MDD lddmc_serialize_get_reversed(size_t value);
void lddmc_serialize_reset();
void lddmc_serialize_totext(FILE *out);
void lddmc_serialize_tofile(FILE *out);
void lddmc_serialize_fromfile(FILE *in);

/* Infrastructure for internal markings */
typedef struct lddmc_refs_internal
{
    size_t r_size, r_count;
    size_t s_size, s_count;
    MDD *results;
    Task **spawns;
} *lddmc_refs_internal_t;

extern DECLARE_THREAD_LOCAL(lddmc_refs_key, lddmc_refs_internal_t);

static inline MDD
lddmc_refs_push(MDD ldd)
{
    LOCALIZE_THREAD_LOCAL(lddmc_refs_key, lddmc_refs_internal_t);
    if (lddmc_refs_key->r_count >= lddmc_refs_key->r_size) {
        lddmc_refs_key->r_size *= 2;
        lddmc_refs_key->results = (MDD*)realloc(lddmc_refs_key->results, sizeof(MDD) * lddmc_refs_key->r_size);
    }
    lddmc_refs_key->results[lddmc_refs_key->r_count++] = ldd;
    return ldd;
}

static inline void
lddmc_refs_pop(int amount)
{
    LOCALIZE_THREAD_LOCAL(lddmc_refs_key, lddmc_refs_internal_t);
    lddmc_refs_key->r_count-=amount;
}

static inline void
lddmc_refs_spawn(Task *t)
{
    LOCALIZE_THREAD_LOCAL(lddmc_refs_key, lddmc_refs_internal_t);
    if (lddmc_refs_key->s_count >= lddmc_refs_key->s_size) {
        lddmc_refs_key->s_size *= 2;
        lddmc_refs_key->spawns = (Task**)realloc(lddmc_refs_key->spawns, sizeof(Task*) * lddmc_refs_key->s_size);
    }
    lddmc_refs_key->spawns[lddmc_refs_key->s_count++] = t;
}

static inline MDD
lddmc_refs_sync(MDD result)
{
    LOCALIZE_THREAD_LOCAL(lddmc_refs_key, lddmc_refs_internal_t);
    lddmc_refs_key->s_count--;
    return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
