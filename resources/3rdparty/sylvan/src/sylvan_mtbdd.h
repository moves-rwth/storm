/*
 * Copyright 2011-2015 Formal Methods and Tools, University of Twente
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

/**
 * This is an implementation of Multi-Terminal Binary Decision Diagrams.
 * They encode functions on Boolean variables to any domain.
 *
 * Three domains are supported by default: Boolean, Integer and Real.
 * Boolean MTBDDs are identical to BDDs (as supported by the bdd subpackage).
 * Integer MTBDDs are encoded using "uint64_t" terminals.
 * Real MTBDDs are encoded using "double" terminals.
 * Negative integers/reals are encoded using the complement edge.
 *
 * Labels of Boolean variables of MTBDD nodes are 24-bit integers.
 *
 * Custom terminals are supported. For notification when nodes are deleted in gc,
 * set a callback using sylvan_set_ondead and for each custom terminal node, call
 * the function mtbdd_notify_ondead.
 *
 * Terminal type "0" is the Integer type, type "1" is the Real type.
 * Type "2" is the Fraction type, consisting of two 32-bit integers (numerator and denominator)
 * For non-Boolean MTBDDs, mtbdd_false is used for partial functions, i.e. mtbdd_false
 * indicates that the function is not defined for a certain input.
 */

/* Do not include this file directly. Instead, include sylvan.h */

#ifndef SYLVAN_MTBDD_H
#define SYLVAN_MTBDD_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * An MTBDD is a 64-bit value. The low 40 bits are an index into the unique table.
 * The highest 1 bit is the complement edge, indicating negation.
 * For Boolean MTBDDs, this means "not X", for Integer and Real MTBDDs, this means "-X".
 */
typedef uint64_t MTBDD;
typedef MTBDD MTBDDMAP;

/**
 * mtbdd_true is only used in Boolean MTBDDs. mtbdd_false has multiple roles (see above).
 */
#define mtbdd_complement    ((MTBDD)0x8000000000000000LL)
#define mtbdd_false         ((MTBDD)0)
#define mtbdd_true          (mtbdd_false|mtbdd_complement)
#define mtbdd_invalid       ((MTBDD)0xffffffffffffffffLL)

/**
 * Initialize MTBDD functionality.
 * This initializes internal and external referencing datastructures,
 * and registers them in the garbage collection framework.
 */
void sylvan_init_mtbdd();

/**
 * Create a MTBDD terminal of type <type> and value <value>.
 * For custom types, the value could be a pointer to some external struct.
 */
MTBDD mtbdd_makeleaf(uint32_t type, uint64_t value);

/**
 * Create an internal MTBDD node of Boolean variable <var>, with low edge <low> and high edge <high>.
 * <var> is a 24-bit integer.
 */
MTBDD mtbdd_makenode(uint32_t var, MTBDD low, MTBDD high);

/**
 * Returns 1 is the MTBDD is a terminal, or 0 otherwise.
 */
int mtbdd_isleaf(MTBDD mtbdd);
#define mtbdd_isnode(mtbdd) (mtbdd_isleaf(mtbdd) ? 0 : 1)

/**
 * For MTBDD terminals, returns <type> and <value>
 */
uint32_t mtbdd_gettype(MTBDD terminal);
uint64_t mtbdd_getvalue(MTBDD terminal);

/**
 * For internal MTBDD nodes, returns <var>, <low> and <high>
 */
uint32_t mtbdd_getvar(MTBDD node);
MTBDD mtbdd_getlow(MTBDD node);
MTBDD mtbdd_gethigh(MTBDD node);

/**
 * Compute the negation of the MTBDD
 * For Boolean MTBDDs, this means "not X", for integer and reals, this means "-X".
 */
#define mtbdd_isnegated(dd) ((dd & mtbdd_complement) ? 1 : 0)
#define mtbdd_negate(dd) (dd ^ mtbdd_complement)
#define mtbdd_not(dd) (dd ^ mtbdd_complement)

/**
 * Create terminals representing uint64_t (type 0), double (type 1), or fraction (type 2) values
 */
MTBDD mtbdd_uint64(uint64_t value);
MTBDD mtbdd_double(double value);
MTBDD mtbdd_fraction(uint64_t numer, uint64_t denom);

/**
 * Get the value of a terminal (for Integer and Real terminals, types 0 and 1)
 */
#define mtbdd_getuint64(terminal) mtbdd_getvalue(terminal)
double mtbdd_getdouble(MTBDD terminal);
#define mtbdd_getnumer(terminal) ((uint32_t)(mtbdd_getvalue(terminal)>>32))
#define mtbdd_getdenom(terminal) ((uint32_t)(mtbdd_getvalue(terminal)&0xffffffff))

/**
 * Create the conjunction of variables in arr.
 * I.e. arr[0] \and arr[1] \and ... \and arr[length-1]
 */
MTBDD mtbdd_fromarray(uint32_t* arr, size_t length);

/**
 * Create a MTBDD cube representing the conjunction of variables in their positive or negative
 * form depending on whether the cube[idx] equals 0 (negative), 1 (positive) or 2 (any).
 * Use cube[idx]==3 for "s=s'" in interleaved variables (matches with next variable)
 * <variables> is the cube of variables (var1 \and var2 \and ... \and varn)
 */
MTBDD mtbdd_cube(MTBDD variables, uint8_t *cube, MTBDD terminal);

/**
 * Same as mtbdd_cube, but extends <mtbdd> with the assignment <cube> \to <terminal>.
 * If <mtbdd> already assigns a value to the cube, the new value <terminal> is taken.
 * Does not support cube[idx]==3.
 */
#define mtbdd_union_cube(mtbdd, variables, cube, terminal) CALL(mtbdd_union_cube, mtbdd, variables, cube, terminal)
TASK_DECL_4(BDD, mtbdd_union_cube, MTBDD, MTBDD, uint8_t*, MTBDD);

/**
 * Count the number of satisfying assignments (minterms) leading to a non-false leaf
 */
TASK_DECL_2(double, mtbdd_satcount, MTBDD, size_t);
#define mtbdd_satcount(dd, nvars) CALL(mtbdd_satcount, dd, nvars)

/**
 * Count the number of MTBDD leaves (excluding mtbdd_false and mtbdd_true) in the MTBDD
 */
size_t mtbdd_leafcount(MTBDD mtbdd);

/**
 * Count the number of MTBDD nodes and terminals (excluding mtbdd_false and mtbdd_true) in a MTBDD
 */
size_t mtbdd_nodecount(MTBDD mtbdd);

/**
 * Callback function types for binary ("dyadic") and unary ("monadic") operations.
 * The callback function returns either the MTBDD that is the result of applying op to the MTBDDs,
 * or mtbdd_invalid if op cannot be applied.
 * The binary function may swap the two parameters (if commutative) to improve caching.
 * The unary function is allowed an extra parameter (be careful of caching)
 */
LACE_TYPEDEF_CB(MTBDD, mtbdd_apply_op, MTBDD*, MTBDD*);
LACE_TYPEDEF_CB(MTBDD, mtbdd_applyp_op, MTBDD*, MTBDD*, size_t);
LACE_TYPEDEF_CB(MTBDD, mtbdd_uapply_op, MTBDD, size_t);

/**
 * Apply a binary operation <op> to <a> and <b>.
 * Callback <op> is consulted before the cache, thus the application to terminals is not cached.
 */
TASK_DECL_3(MTBDD, mtbdd_apply, MTBDD, MTBDD, mtbdd_apply_op);
#define mtbdd_apply(a, b, op) CALL(mtbdd_apply, a, b, op)

/**
 * Apply a binary operation <op> with id <opid> to <a> and <b> with parameter <p>
 * Callback <op> is consulted before the cache, thus the application to terminals is not cached.
 */
TASK_DECL_5(MTBDD, mtbdd_applyp, MTBDD, MTBDD, size_t, mtbdd_applyp_op, uint64_t);
#define mtbdd_applyp(a, b, p, op, opid) CALL(mtbdd_applyp, a, b, p, op, opid)

/**
 * Apply a unary operation <op> to <dd>.
 * Callback <op> is consulted after the cache, thus the application to a terminal is cached.
 */
TASK_DECL_3(MTBDD, mtbdd_uapply, MTBDD, mtbdd_uapply_op, size_t);
#define mtbdd_uapply(dd, op, param) CALL(mtbdd_uapply, dd, op, param)

/**
 * Callback function types for abstraction.
 * MTBDD mtbdd_abstract_op(MTBDD a, MTBDD b, int k).
 * The function is either called with k==0 (apply to two arguments) or k>0 (k skipped BDD variables)
 * k == 0  =>  res := apply op to a and b
 * k  > 0  =>  res := apply op to op(a, a, k-1) and op(a, a, k-1)
 */
LACE_TYPEDEF_CB(MTBDD, mtbdd_abstract_op, MTBDD, MTBDD, int);

/**
 * Abstract the variables in <v> from <a> using the binary operation <op>.
 */
TASK_DECL_3(MTBDD, mtbdd_abstract, MTBDD, MTBDD, mtbdd_abstract_op);
#define mtbdd_abstract(a, v, op) CALL(mtbdd_abstract, a, v, op)

/**
 * Binary operation Plus (for MTBDDs of same type)
 * Only for MTBDDs where either all leaves are Boolean, or Integer, or Double.
 * For Integer/Double MTBDDs, mtbdd_false is interpreted as "0" or "0.0".
 */
TASK_DECL_2(MTBDD, mtbdd_op_plus, MTBDD*, MTBDD*);
TASK_DECL_3(MTBDD, mtbdd_abstract_op_plus, MTBDD, MTBDD, int);

/**
 * Binary operation Times (for MTBDDs of same type)
 * Only for MTBDDs where either all leaves are Boolean, or Integer, or Double.
 * For Integer/Double MTBDD, if either operand is mtbdd_false (not defined),
 * then the result is mtbdd_false (i.e. not defined).
 */
TASK_DECL_2(MTBDD, mtbdd_op_times, MTBDD*, MTBDD*);
TASK_DECL_3(MTBDD, mtbdd_abstract_op_times, MTBDD, MTBDD, int);

/**
 * Binary operation Minimum (for MTBDDs of same type)
 * Only for MTBDDs where either all leaves are Boolean, or Integer, or Double.
 * For Integer/Double MTBDD, if either operand is mtbdd_false (not defined),
 * then the result is the other operand.
 */
TASK_DECL_2(MTBDD, mtbdd_op_min, MTBDD*, MTBDD*);
TASK_DECL_3(MTBDD, mtbdd_abstract_op_min, MTBDD, MTBDD, int);

/**
 * Binary operation Maximum (for MTBDDs of same type)
 * Only for MTBDDs where either all leaves are Boolean, or Integer, or Double.
 * For Integer/Double MTBDD, if either operand is mtbdd_false (not defined),
 * then the result is the other operand.
 */
TASK_DECL_2(MTBDD, mtbdd_op_max, MTBDD*, MTBDD*);
TASK_DECL_3(MTBDD, mtbdd_abstract_op_max, MTBDD, MTBDD, int);

/**
 * Compute a + b
 */
#define mtbdd_plus(a, b) mtbdd_apply(a, b, TASK(mtbdd_op_plus))

/**
 * Compute a - b
 */
#define mtbdd_minus(a, b) mtbdd_plus(a, mtbdd_negate(minus))

/**
 * Compute a * b
 */
#define mtbdd_times(a, b) mtbdd_apply(a, b, TASK(mtbdd_op_times))

/**
 * Compute min(a, b)
 */
#define mtbdd_min(a, b) mtbdd_apply(a, b, TASK(mtbdd_op_min))

/**
 * Compute max(a, b)
 */
#define mtbdd_max(a, b) mtbdd_apply(a, b, TASK(mtbdd_op_max))

/**
 * Abstract the variables in <v> from <a> by taking the sum of all values
 */
#define mtbdd_abstract_plus(dd, v) mtbdd_abstract(dd, v, TASK(mtbdd_abstract_op_plus))

/**
 * Abstract the variables in <v> from <a> by taking the product of all values
 */
#define mtbdd_abstract_times(dd, v) mtbdd_abstract(dd, v, TASK(mtbdd_abstract_op_times))

/**
 * Abstract the variables in <v> from <a> by taking the minimum of all values
 */
#define mtbdd_abstract_min(dd, v) mtbdd_abstract(dd, v, TASK(mtbdd_abstract_op_min))

/**
 * Abstract the variables in <v> from <a> by taking the maximum of all values
 */
#define mtbdd_abstract_max(dd, v) mtbdd_abstract(dd, v, TASK(mtbdd_abstract_op_max))

/**
 * Compute IF <f> THEN <g> ELSE <h>.
 * <f> must be a Boolean MTBDD (or standard BDD).
 */
TASK_DECL_3(MTBDD, mtbdd_ite, MTBDD, MTBDD, MTBDD);
#define mtbdd_ite(f, g, h) CALL(mtbdd_ite, f, g, h);

/**
 * Multiply <a> and <b>, and abstract variables <vars> using summation.
 * This is similar to the "and_exists" operation in BDDs.
 */
TASK_DECL_3(MTBDD, mtbdd_and_exists, MTBDD, MTBDD, MTBDD);
#define mtbdd_and_exists(a, b, vars) CALL(mtbdd_and_exists, a, b, vars)

/**
 * Monad that converts double to a Boolean MTBDD, translate terminals >= value to 1 and to 0 otherwise;
 */
TASK_DECL_2(MTBDD, mtbdd_op_threshold_double, MTBDD, size_t)

/**
 * Monad that converts double to a Boolean MTBDD, translate terminals > value to 1 and to 0 otherwise;
 */
TASK_DECL_2(MTBDD, mtbdd_op_strict_threshold_double, MTBDD, size_t)

/**
 * Convert double to a Boolean MTBDD, translate terminals >= value to 1 and to 0 otherwise;
 */
TASK_DECL_2(MTBDD, mtbdd_threshold_double, MTBDD, double);
#define mtbdd_threshold_double(dd, value) CALL(mtbdd_threshold_double, dd, value)

/**
 * Convert double to a Boolean MTBDD, translate terminals > value to 1 and to 0 otherwise;
 */
TASK_DECL_2(MTBDD, mtbdd_strict_threshold_double, MTBDD, double);
#define mtbdd_strict_threshold_double(dd, value) CALL(mtbdd_strict_threshold_double, dd, value)

/**
 * For two Double MTBDDs, calculate whether they are equal module some value epsilon
 * i.e. abs(a-b)<3
 */
TASK_DECL_3(MTBDD, mtbdd_equal_norm_d, MTBDD, MTBDD, double);
#define mtbdd_equal_norm_d(a, b, epsilon) CALL(mtbdd_equal_norm_d, a, b, epsilon)

/**
 * For two Double MTBDDs, calculate whether they are relatively equal module some value epsilon
 * i.e. abs((a-b)/a) < e
 */
TASK_DECL_3(MTBDD, mtbdd_equal_norm_rel_d, MTBDD, MTBDD, double);
#define mtbdd_equal_norm_rel_d(a, b, epsilon) CALL(mtbdd_equal_norm_rel_d, a, b, epsilon)

/**
 * For two MTBDDs a, b, return mtbdd_true if all common assignments a(s) <= b(s), mtbdd_false otherwise.
 * For domains not in a / b, assume True.
 */
TASK_DECL_2(MTBDD, mtbdd_leq, MTBDD, MTBDD);
#define mtbdd_leq(a, b) CALL(mtbdd_leq, a, b)

/**
 * For two MTBDDs a, b, return mtbdd_true if all common assignments a(s) < b(s), mtbdd_false otherwise.
 * For domains not in a / b, assume True.
 */
TASK_DECL_2(MTBDD, mtbdd_less, MTBDD, MTBDD);
#define mtbdd_less(a, b) CALL(mtbdd_less, a, b)

/**
 * For two MTBDDs a, b, return mtbdd_true if all common assignments a(s) >= b(s), mtbdd_false otherwise.
 * For domains not in a / b, assume True.
 */
TASK_DECL_2(MTBDD, mtbdd_geq, MTBDD, MTBDD);
#define mtbdd_geq(a, b) CALL(mtbdd_geq, a, b)

/**
 * For two MTBDDs a, b, return mtbdd_true if all common assignments a(s) > b(s), mtbdd_false otherwise.
 * For domains not in a / b, assume True.
 */
TASK_DECL_2(MTBDD, mtbdd_greater, MTBDD, MTBDD);
#define mtbdd_greater(a, b) CALL(mtbdd_greater, a, b)

/**
 * Calculate the support of a MTBDD, i.e. the cube of all variables that appear in the MTBDD nodes.
 */
TASK_DECL_1(MTBDD, mtbdd_support, MTBDD);
#define mtbdd_support(dd) CALL(mtbdd_support, dd)

/**
 * Function composition, for each node with variable <key> which has a <key,value> pair in <map>,
 * replace the node by the result of mtbdd_ite(<value>, <low>, <high>).
 * Each <value> in <map> must be a Boolean MTBDD.
 */
TASK_DECL_2(MTBDD, mtbdd_compose, MTBDD, MTBDDMAP);
#define mtbdd_compose(dd, map) CALL(mtbdd_compose, dd, map)

/**
 * Compute minimal leaf in the MTBDD (for Integer, Double, Rational MTBDDs)
 */
TASK_DECL_1(MTBDD, mtbdd_minimum, MTBDD);
#define mtbdd_minimum(dd) CALL(mtbdd_minimum, dd)

/**
 * Compute maximal leaf in the MTBDD (for Integer, Double, Rational MTBDDs)
 */
TASK_DECL_1(MTBDD, mtbdd_maximum, MTBDD);
#define mtbdd_maximum(dd) CALL(mtbdd_maximum, dd)

/**
 * Write a DOT representation of a MTBDD
 * The callback function is required for custom terminals.
 */
typedef void (*print_terminal_label_cb)(FILE *out, uint32_t type, uint64_t value);
void mtbdd_fprintdot(FILE *out, MTBDD mtbdd, print_terminal_label_cb cb);
#define mtbdd_printdot(mtbdd, cb) mtbdd_fprintdot(stdout, mtbdd, cb)

/**
 * MTBDDMAP, maps uint32_t variables to MTBDDs.
 * A MTBDDMAP node has variable level, low edge going to the next MTBDDMAP, high edge to the mapped MTBDD
 */
#define mtbdd_map_empty() mtbdd_false
#define mtbdd_map_isempty(map) (map == mtbdd_false ? 1 : 0)
#define mtbdd_map_key(map) mtbdd_getvar(map)
#define mtbdd_map_value(map) mtbdd_gethigh(map)
#define mtbdd_map_next(map) mtbdd_getlow(map)

/**
 * Return 1 if the map contains the key, 0 otherwise.
 */
int mtbdd_map_contains(MTBDDMAP map, uint32_t key);

/**
 * Retrieve the number of keys in the map.
 */
size_t mtbdd_map_count(MTBDDMAP map);

/**
 * Add the pair <key,value> to the map, overwrites if key already in map.
 */
MTBDDMAP mtbdd_map_add(MTBDDMAP map, uint32_t key, MTBDD value);

/**
 * Add all values from map2 to map1, overwrites if key already in map1.
 */
MTBDDMAP mtbdd_map_addall(MTBDDMAP map1, MTBDDMAP map2);

/**
 * Remove the key <key> from the map and return the result
 */
MTBDDMAP mtbdd_map_remove(MTBDDMAP map, uint32_t key);

/**
 * Remove all keys in the cube <variables> from the map and return the result
 */
MTBDDMAP mtbdd_map_removeall(MTBDDMAP map, MTBDD variables);

/**
 * Custom node types
 * Overrides standard hash/equality/notify_on_dead behavior
 * hash(value, seed) return hash version
 * equals(value1, value2) return 1 if equal, 0 if not equal
 * create(&value) replace value by new value for object allocation
 * destroy(value)
 * NOTE: equals(value1, value2) must imply: hash(value1, seed) == hash(value2,seed)
 * NOTE: new value of create must imply: equals(old, new)
 */
typedef uint64_t (*mtbdd_hash_cb)(uint64_t, uint64_t);
typedef int (*mtbdd_equals_cb)(uint64_t, uint64_t);
typedef void (*mtbdd_create_cb)(uint64_t*);
typedef void (*mtbdd_destroy_cb)(uint64_t);

/**
 * Registry callback handlers for <type>.
 */
uint32_t mtbdd_register_custom_leaf(mtbdd_hash_cb hash_cb, mtbdd_equals_cb equals_cb, mtbdd_create_cb create_cb, mtbdd_destroy_cb destroy_cb);

/**
 * Garbage collection
 * Sylvan supplies two default methods to handle references to nodes, but the user
 * is encouraged to implement custom handling. Simply add a handler using sylvan_gc_add_mark
 * and let the handler call mtbdd_gc_mark_rec for every MTBDD that should be saved
 * during garbage collection.
 */

/**
 * Call mtbdd_gc_mark_rec for every mtbdd you want to keep in your custom mark functions.
 */
VOID_TASK_DECL_1(mtbdd_gc_mark_rec, MTBDD);
#define mtbdd_gc_mark_rec(mtbdd) CALL(mtbdd_gc_mark_rec, mtbdd)

/**
 * Default external referencing. During garbage collection, MTBDDs marked with mtbdd_ref will
 * be kept in the forest.
 * It is recommended to prefer mtbdd_protect and mtbdd_unprotect.
 */
MTBDD mtbdd_ref(MTBDD a);
void mtbdd_deref(MTBDD a);
size_t mtbdd_count_refs();

/**
 * Default external pointer referencing. During garbage collection, the pointers are followed and the MTBDD
 * that they refer to are kept in the forest.
 */
void mtbdd_protect(MTBDD* ptr);
void mtbdd_unprotect(MTBDD* ptr);
size_t mtbdd_count_protected();

/**
 * If sylvan_set_ondead is set to a callback, then this function marks MTBDDs (terminals).
 * When they are dead after the mark phase in garbage collection, the callback is called for marked MTBDDs.
 * The ondead callback can either perform cleanup or resurrect dead terminals.
 */
#define mtbdd_notify_ondead(dd) llmsset_notify_ondead(nodes, dd&~mtbdd_complement)

/**
 * Infrastructure for internal references (per-thread, e.g. during MTBDD operations)
 * Use mtbdd_refs_push and mtbdd_refs_pop to put MTBDDs on a thread-local reference stack.
 * Use mtbdd_refs_spawn and mtbdd_refs_sync around SPAWN and SYNC operations when the result
 * of the spawned Task is a MTBDD that must be kept during garbage collection.
 */
typedef struct mtbdd_refs_internal
{
    size_t r_size, r_count;
    size_t s_size, s_count;
    MTBDD *results;
    Task **spawns;
} *mtbdd_refs_internal_t;

extern DECLARE_THREAD_LOCAL(mtbdd_refs_key, mtbdd_refs_internal_t);

static inline MTBDD
mtbdd_refs_push(MTBDD mtbdd)
{
    LOCALIZE_THREAD_LOCAL(mtbdd_refs_key, mtbdd_refs_internal_t);
    if (mtbdd_refs_key->r_count >= mtbdd_refs_key->r_size) {
        mtbdd_refs_key->r_size *= 2;
        mtbdd_refs_key->results = (MTBDD*)realloc(mtbdd_refs_key->results, sizeof(MTBDD) * mtbdd_refs_key->r_size);
    }
    mtbdd_refs_key->results[mtbdd_refs_key->r_count++] = mtbdd;
    return mtbdd;
}

static inline void
mtbdd_refs_pop(int amount)
{
    LOCALIZE_THREAD_LOCAL(mtbdd_refs_key, mtbdd_refs_internal_t);
    mtbdd_refs_key->r_count-=amount;
}

static inline void
mtbdd_refs_spawn(Task *t)
{
    LOCALIZE_THREAD_LOCAL(mtbdd_refs_key, mtbdd_refs_internal_t);
    if (mtbdd_refs_key->s_count >= mtbdd_refs_key->s_size) {
        mtbdd_refs_key->s_size *= 2;
        mtbdd_refs_key->spawns = (Task**)realloc(mtbdd_refs_key->spawns, sizeof(Task*) * mtbdd_refs_key->s_size);
    }
    mtbdd_refs_key->spawns[mtbdd_refs_key->s_count++] = t;
}

static inline MTBDD
mtbdd_refs_sync(MTBDD result)
{
    LOCALIZE_THREAD_LOCAL(mtbdd_refs_key, mtbdd_refs_internal_t);
    mtbdd_refs_key->s_count--;
    return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
