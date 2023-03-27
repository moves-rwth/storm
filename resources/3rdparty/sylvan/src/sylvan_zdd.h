/*
 * Copyright 2011-2016 Tom van Dijk
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
 * This is a multi-core implementation of Zero-suppressed Binary Decision Diagrams.
 * 
 * Unlike BDDs, the interpretation of a ZDD depends on the "domain" of variables.
 * Variables not encountered in the ZDD are *false*.
 * The representation of the universe set is NOT the leaf "true".
 * Also, no complement edges. They do not work here.
 * Thus, computing "not" is not a trivial constant operation.
 * 
 * To represent "domain" and "set of variables" we use the same cubes
 * as for BDDs, i.e., var1 \and var2 \and var3... 
 * 
 * All operations with multiple input ZDDs interpret the ZDDs in the same domain.
 * For some operations, this domain must be supplied.
 */

/* Do not include this file directly. Instead, include sylvan.h */

#ifndef SYLVAN_ZDD_H
#define SYLVAN_ZDD_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * A ZDD is a 64-bit value. The low 40 bits are an index into the unique table.
 * The highest 1 bit is the complement bit.
 */
typedef uint64_t ZDD;
typedef ZDD ZDDMAP;

#ifndef ZDD_COMPLEMENT_EDGES
#define ZDD_COMPLEMENT_EDGES 1
#endif

#define zdd_false       ((uint64_t)0)
#define zdd_complement  ((uint64_t)0x8000000000000000)
#define zdd_invalid     ((uint64_t)0xffffffffffffffff)

#if ZDD_COMPLEMENT_EDGES
#define zdd_true        ((uint64_t)0x8000000000000000)
#else
#define zdd_true        ((uint64_t)1)
#endif

/**
 * Initialize ZDD functionality.
 * This initializes internal and external referencing datastructures
 * and registers them in the garbage collection framework.
 */
void sylvan_init_zdd(void);

/**
 * Create an internal ZDD node of Boolean variable <var>, with low edge <low> and high edge <high>.
 * <var> is a 24-bit integer.
 * This function does NOT check/enforce variable ordering!
 */
ZDD _zdd_makenode(uint32_t var, ZDD low, ZDD high);

static inline ZDD
zdd_makenode(uint32_t var, ZDD low, ZDD high)
{
    if (high == zdd_false) return low;
    else return _zdd_makenode(var, low, high);
}

/**
 * Returns 1 is the ZDD is a leaf, or 0 otherwise.
 */
int zdd_isleaf(ZDD dd);

/**
 * Returns 1 if the ZDD is an interlan node, or 0 otherwise.
 */
#define zdd_isnode(dd) (zdd_isleaf(dd) ? 0 : 1)

/**
 * Given internal node <dd>, obtain the variable.
 */
uint32_t zdd_getvar(ZDD dd);

/**
 * Given internal node <dd>, follow the low edge.
 */
ZDD zdd_getlow(ZDD dd);

/**
 * Given internal node <dd>, follow the high edge.
 */
ZDD zdd_gethigh(ZDD dd);

/**
 * TODO: zdd_gettype, zdd_getvalue etc for leaves
 */

/**
 * Evaluate a ZDD, assigning <value> (1 or 0) to <variables>.
 */
ZDD zdd_eval(ZDD dd, uint32_t var, int value);

/**
 * Obtain a ZDD representing a positive literal of variable <var>.
 */
ZDD zdd_ithvar(uint32_t var);

/**
 * Obtain a ZDD representing a negative literal of variable <var>.
 */
ZDD zdd_nithvar(uint32_t var);

/**
 * Convert an MTBDD to a ZDD.
 */
TASK_DECL_2(ZDD, zdd_from_mtbdd, MTBDD, MTBDD);
#define zdd_from_mtbdd(dd, domain) RUN(zdd_from_mtbdd, dd, domain)

/**
 * Convert a ZDD to an MTBDD.
 */
TASK_DECL_2(MTBDD, zdd_to_mtbdd, ZDD, ZDD);
#define zdd_to_mtbdd(dd, domain) RUN(zdd_to_mtbdd, dd, domain)

/**
 * Create a variable set, represented as the function that evaluates
 * to True for all assignments to its variables.
 * This represents sets of variables, also variable domains.
 * The advantage is that this domain can also serve as a representation
 * of True for a given domain.
 */
ZDD zdd_set_from_array(uint32_t *arr, size_t len);

/**
 * Write all variables in a variable set to the given array.
 * The array must be sufficiently large.
 */
void zdd_set_to_array(ZDD set, uint32_t *arr);

/**
 * Compute the number of variables in a given set of variables.
 */
size_t zdd_set_count(ZDD set);

/**
 * Compute the union of <set1> and <set2>.
 */
ZDD zdd_set_union(ZDD set1, ZDD set2);

/**
 * Remove variables in <set2> from <set1>.
 */
ZDD zdd_set_minus(ZDD set1, ZDD set2);

/**
 * Returns 1 if <set> contains <var>, 0 otherwise.
 */
int zdd_set_contains(ZDD set, uint32_t var);

/**
 * Adds the variable <var> to <set>.
 */
ZDD zdd_set_add(ZDD set, uint32_t var);

/**
 * Removes the variable <var> from <set>.
 */
ZDD zdd_set_remove(ZDD set, uint32_t var);

/**
 * Various set operations as macros
 */
#define zdd_set_empty() zdd_true
#define zdd_set_isempty(set) (set == zdd_true)
#define zdd_set_first(set) zdd_getvar(set)
#define zdd_set_next(set) zdd_high(set)

#define zdd_set_from_mtbdd(orig) zdd_from_mtbdd(sylvan_true, orig)
MTBDD zdd_set_to_mtbdd(ZDD set);

/**
 * Create a cube of literals of the given domain with the values given in <arr>.
 * Uses the given leaf as leaf.
 * For values, 0 (negative literal), 1 (positive), 2 (both values).
 * The resulting ZDD is defined on the domain <variables>.
 */
ZDD zdd_cube(ZDD variables, uint8_t *values, ZDD leaf);

/**
 * Same as zdd_cube, but adds the cube to an existing set of the same domain.
 * Elements already in the set are updated with the given leaf.
 */
TASK_DECL_4(ZDD, zdd_union_cube, ZDD, ZDD, uint8_t*, ZDD);
#define zdd_union_cube(set, variables, values, leaf) RUN(zdd_union_cube, set, variables, values, leaf)

/**
 * Compute the irredundant sum of products given lower and upper bounds as BDDs.
 * Returns a ZDD cover between the two bounds; if given bddresptr pointer, then that will have the BDD.
 */
TASK_DECL_3(ZDD, zdd_isop, MTBDD, MTBDD, MTBDD*);
#define zdd_isop(L, U, bddresptr) RUN(zdd_isop, L, U, bddresptr)

/**
 * Compute the BDD representation of a given ZDD cover.
 */
TASK_DECL_1(MTBDD, zdd_cover_to_bdd, ZDD);
#define zdd_cover_to_bdd(zdd) RUN(zdd_cover_to_bdd, zdd)

/**
 * Enumerate the cubes of a ZDD cover
 * <arr> must be a sufficiently large pre-allocated array, i.e., 1 + number_of_bdd_variables
 * Returns zdd_true on success or zdd_false if no more cubes in the cover.
 * The array will be filled with even (positive) and odd (negative) cover variables, ending with -1.
 */
ZDD zdd_cover_enum_first(ZDD dd, int32_t *arr);
ZDD zdd_cover_enum_next(ZDD dd, int32_t *arr);

/**
 * Extend the domain of a ZDD, such that all new variables take the given value.
 * The given value can be 0 (always negative), 1 (always positive), 2 (always dontcare)
 */
TASK_DECL_3(ZDD, zdd_extend_domain, ZDD, ZDD, int);
#define zdd_extend_domain(dd, newvars, value) RUN(zdd_extend_domain, dd, newvars, value);

/**
 * Calculate the support of a ZDD, i.e. the cube of all variables that appear in the ZDD nodes.
 */
TASK_DECL_1(ZDD, zdd_support, ZDD);
#define zdd_support(dd) RUN(zdd_support, dd)

/**
 * Count the number of satisfying assignments (minterms) leading to a non-False leaf.
 * We do not need to give the domain, as skipped variables do not increase the number of minterms.
 * Fun fact: this is the same as zdd_pathcount!
 */
#define zdd_satcount zdd_pathcount

/**
 * Count the number of distinct paths leading to a non-False leaf.
 */
TASK_DECL_1(double, zdd_pathcount, ZDD);
#define zdd_pathcount(dd) RUN(zdd_pathcount, dd)

/**
 * Count the number of nodes (internal nodes plus leaves) in ZDDs.
 * Not thread-safe.
 */
size_t zdd_nodecount(const ZDD *dds, size_t count);

static inline size_t
zdd_nodecount_one(const ZDD dd)
{
    return zdd_nodecount(&dd, 1);
}

/**
 * Compute IF <f> THEN <g> ELSE <h>.
 * Assuming f, g, h are all Boolean and on the same domain <dom>.
 */
TASK_DECL_4(ZDD, zdd_ite, ZDD, ZDD, ZDD, ZDD);
#define zdd_ite(f, g, h, dom) RUN(zdd_ite, f, g, h, dom)

/**
 * Compute the negation of a ZDD w.r.t. the given domain.
 */
TASK_DECL_2(ZDD, zdd_not, ZDD, ZDD);
#define zdd_not(dd, domain) RUN(zdd_not, dd, domain)

/**
 * Compute logical AND of <a> and <b>.
 */
TASK_DECL_2(ZDD, zdd_and, ZDD, ZDD);
#define zdd_and(a, b) RUN(zdd_and, a, b)

/**
 * Compute logical OR of <a> and <b>.
 */
TASK_DECL_2(ZDD, zdd_or, ZDD, ZDD);
#define zdd_or(a, b) RUN(zdd_or, a, b)

/**
 * Compute logical DIFF of <a> and <b>. (set minus)
 */
TASK_DECL_2(ZDD, zdd_diff, ZDD, ZDD);
#define zdd_diff(a, b) RUN(zdd_diff, a, b)

/**
 * Compute logical XOR of <a> and <b>.
 */
// TASK_DECL_2(ZDD, zdd_xor, ZDD, ZDD);
// #define zdd_xor(a, b) RUN(zdd_xor, a, b)

/**
 * Compute logical EQUIV of <a> and <b>.
 * Also called bi-implication. (a <-> b)
 * This operation requires the variable domain <dom>.
 */
// TASK_DECL_3(ZDD, zdd_equiv, ZDD, ZDD, ZDD);
// #define zdd_equiv(a, b, dom) RUN(zdd_equiv, a, b, dom)

/**
 * Compute logical IMP of <a> and <b>. (a -> b)
 * This operation requires the variable domain <dom>.
 */
// TASK_DECL_3(ZDD, zdd_imp, ZDD, ZDD, ZDD);
// #define zdd_imp(a, b, dom) RUN(zdd_imp, a, b, dom)

/**
 * Compute logical INVIMP of <a> and <b>. (b <- a)
 * This operation requires the variable domain <dom>.
 */
// TASK_DECL_3(ZDD, zdd_invimp, ZDD, ZDD, ZDD);
// #define zdd_invimp(a, b, dom) RUN(zdd_invimp, a, b, dom)

// add binary operators
// zdd_diff (no domain) == a and not b
// zdd_less (no domain) == not a and b
// zdd_nand (domain)    == not (a and b)
// zdd_nor  (domain)    == not a and not b

/**
 * Compute \exists <vars>: <dd>.
 * (Stays in same variable domain.)
 */
TASK_DECL_2(ZDD, zdd_exists, ZDD, ZDD);
#define zdd_exists(dd, vars) RUN(zdd_exists, dd, vars)

/**
 * Project <dd> onto <domain>, existentially quantifying variables not in the domain.
 * (Changes to the new variable domain.)
 */
TASK_DECL_2(ZDD, zdd_project, ZDD, ZDD);
#define zdd_project(dd, domain) RUN(zdd_project, dd, domain)

/**
 * Compute \forall <vars>: <dd>.
 */
// TASK_DECL_2(ZDD, zdd_forall, ZDD, ZDD);
// #define zdd_forall(dd, vars) RUN(zdd_forall, dd, vars)

/**
 * Compute \exists <vars>: <a> and <b>.
 * Result is in same domain as <a> and <b>.
 */
// TASK_DECL_3(ZDD, zdd_and_exists, ZDD, ZDD, ZDD);
// #define zdd_and_exists(a, b, vars) RUN(zdd_and_exists, a, b, vars)

/**
 * Compute <a> and <b> and project result on <domain>
 */
// TASK_DECL_3(ZDD, zdd_and_project, ZDD, ZDD, ZDD);
// #define zdd_and_project(a, b, domain) RUN(zdd_and_project, a, b, domain)

/**
 * Function composition, for each node with variable <key> which has a <key,value> pair in <map>,
 * replace the node by the result of zdd_ite(<value>, <low>, <high>).
 * Each <value> in <map> must be a Boolean ZDD.
 */
// TASK_DECL_2(ZDD, zdd_compose, ZDD, ZDDMAP);
// #define zdd_compose(dd, map) RUN(zdd_compose, dd, map)

/**
 * For debugging.
 * Tests if all nodes in the ZDD are correctly ``marked'' in the nodes table.
 * Tests if variables in the internal nodes appear in-order.
 * In Debug mode, this will cause assertion failures instead of returning 0.
 * Returns 1 if all is fine, or 0 otherwise.
 */
// TASK_DECL_1(int, zdd_test_isvalid, ZDD);
// #define zdd_test_isvalid(zdd) RUN(zdd_test_isvalid, zdd)

/**
 * Write a DOT representation of a ZDD
 * The callback function is required for custom terminals.
 */
void zdd_fprintdot(FILE *out, ZDD zdd);
#define zdd_printdot(zdd) zdd_fprintdot(stdout, zdd)

/**
 * ZDDMAP, maps uint32_t variables to ZDDs.
 * A ZDDMAP node has variable level, low edge going to the next ZDDMAP, high edge to the mapped ZDD
 */
#define zdd_map_empty() zdd_false
#define zdd_map_isempty(map) (map == zdd_false ? 1 : 0)
#define zdd_map_key(map) zdd_getvar(map)
#define zdd_map_value(map) zdd_gethigh(map)
#define zdd_map_next(map) zdd_getlow(map)

/**
 * Return 1 if the map contains the key, 0 otherwise.
 */
int zdd_map_contains(ZDDMAP map, uint32_t key);

/**
 * Retrieve the number of keys in the map.
 */
size_t zdd_map_count(ZDDMAP map);

/**
 * Add the pair <key,value> to the map, overwrites if key already in map.
 */
ZDDMAP zdd_map_add(ZDDMAP map, uint32_t key, ZDD value);

/**
 * Add all values from map2 to map1, overwrites if key already in map1.
 */
ZDDMAP zdd_map_addall(ZDDMAP map1, ZDDMAP map2);

/**
 * Remove the key <key> from the map and return the result
 */
ZDDMAP zdd_map_remove(ZDDMAP map, uint32_t key);

/**
 * Remove all keys in the cube <variables> from the map and return the result
 */
ZDDMAP zdd_map_removeall(ZDDMAP map, ZDD variables);

/**
 * Enumerate all minterms (non-False assignments)
 *
 * Given a ZDD <dd> and a variable domain <dom>, zdd_enum_first and zdd_enum_next enumerate
 * all assignments to the variables in <dom> that lead to a non-False leaf.
 * 
 * The function returns the leaf (or zdd_false if no new path is found) and encodes the path
 * in the supplied array <arr>: 0 for a low edge, 1 for a high edge.
 *
 * Usage:
 * ZDD leaf = zdd_enum_first(dd, variables, arr, NULL);
 * while (leaf != zdd_false) {
 *     .... // do something with arr/leaf
 *     leaf = zdd_enum_next(dd, variables, arr, NULL);
 * }
 *
 * The callback is an optional function that returns 0 when the given leaf should be skipped, or 1 otherwise.
 */
typedef int (*zdd_enum_filter_cb)(ZDD);
ZDD zdd_enum_first(ZDD dd, ZDD variables, uint8_t *arr, zdd_enum_filter_cb filter_cb);
ZDD zdd_enum_next(ZDD dd, ZDD variables, uint8_t *arr, zdd_enum_filter_cb filter_cb);

/**
 * Enumerate minterms of the ZDD <dd>, interpreted along the domain <dom>.
 * Obtain the first minterm in arr, setting arr values to 0/1 in the order of the variable domain.
 */
// ZDD zdd_enum_first(ZDD dd, ZDD dom, uint8_t *arr);

/**
 * Enumerate minterms of the ZDD <dd>, interpreted along the domain <dom>.
 * Obtain the next minterm in arr, setting arr values to 0/1 in the order of the variable domain.
 */
// ZDD zdd_enum_next(ZDD dd, ZDD dom, uint8_t *arr);

/*
typedef struct zdd_trace {
        struct zdd_trace *prev;
            uint32_t var;
                uint8_t val;
} * zdd_trace_t;

LACE_TYPEDEF_CB(void, zdd_enum_cb, void*, uint8_t*, size_t);
VOID_TASK_DECL_4(zdd_enum, ZDD, ZDD, zdd_enum_cb, void*);
#define zdd_enum(dd, dom, cb, context) RUN(zdd_enum, dd, dom, cb, context)

VOID_TASK_DECL_4(zdd_enum_seq, ZDD, ZDD, zdd_enum_cb, void*);
#define zdd_enum_seq(dd, dom, cb, context) RUN(zdd_enum_seq, dd, dom, cb, context)

LACE_TYPEDEF_CB(ZDD, zdd_collect_cb, void*, uint8_t*, size_t);
TASK_DECL_5(ZDD, zdd_collect, ZDD, ZDD, ZDD, zdd_collect_cb, void*);
#define zdd_collect(dd, dom, res_dom, cb, context) RUN(zdd_collect, dd, dom, res_dom, cb, context)
*/

// sat_one / pick_cube, visitor
// relnext, relprev
// compose
// serialization

/**
 * Visitor functionality for ZDDs.
 * Visits internal nodes, not leaves (TODO FIXME)
 */

/**
 * pre_cb callback: given input ZDD and context, return whether to visit children
 * post_cb callback: given input ZDD and context
 */
LACE_TYPEDEF_CB(int, zdd_visit_pre_cb, ZDD, void*);
LACE_TYPEDEF_CB(void, zdd_visit_post_cb, ZDD, void*);

/**
 * Sequential visit operation
 */
VOID_TASK_DECL_4(zdd_visit_seq, ZDD, zdd_visit_pre_cb, zdd_visit_post_cb, void*);
#define zdd_visit_seq(...) RUN(zdd_visit_seq, __VA_ARGS__)

/**
 * Parallel visit operation
 */
VOID_TASK_DECL_4(zdd_visit_par, ZDD, zdd_visit_pre_cb, zdd_visit_post_cb, void*);
#define zdd_visit_par(...) RUN(zdd_visit_par, __VA_ARGS__)

/**
 * Writing ZDDs to file.
 *
 * Every node that is to be written is assigned a number, starting from 1,
 * such that reading the result in the future can be done in one pass.
 *
 * We use a skiplist to store the assignment.
 *
 * One could use the following two methods to store an array of ZDDs.
 * - call zdd_writer_tobinary to store ZDDs in binary format.
 * - call zdd_writer_totext to store ZDDs in text format.
 *
 * One could also do the procedure manually instead.
 * - call zdd_writer_start to allocate the skiplist.
 * - call zdd_writer_add to add a given ZDD to the skiplist
 * - call zdd_writer_writebinary to write all added nodes to a file
 * - OR:  zdd_writer_writetext to write all added nodes in text format
 * - call zdd_writer_get to obtain the ZDD identifier as stored in the skiplist
 * - call zdd_writer_end to free the skiplist
 */

/**
 * Write <count> decision diagrams given in <dds> in internal binary form to <file>.
 *
 * The internal binary format is as follows, to store <count> decision diagrams...
 * uint64_t: nodecount -- number of nodes
 * <nodecount> times uint128_t: each leaf/node
 * uint64_t: count -- number of stored decision diagrams
 * <count> times uint64_t: each stored decision diagram
 */
VOID_TASK_DECL_3(zdd_writer_tobinary, FILE *, ZDD *, int);
#define zdd_writer_tobinary(file, dds, count) RUN(zdd_writer_tobinary, file, dds, count)

/**
 * Write <count> decision diagrams given in <dds> in ASCII form to <file>.
 *
 * The text format writes in the same order as the binary format, except...
 * [
 *   node(id, var, low, high), -- for a normal node (no complement on high)
 *   node(id, var, low, ~high), -- for a normal node (complement on high)
 * ],[dd1, dd2, dd3, ...,] -- and each the stored decision diagram.
 */

VOID_TASK_DECL_3(zdd_writer_totext, FILE *, ZDD *, int);
#define zdd_writer_totext(file, dds, count) RUN(zdd_writer_totext, file, dds, count)

/**
 * Skeleton typedef for the skiplist
 */
typedef struct sylvan_skiplist *sylvan_skiplist_t;

/**
 * Allocate a skiplist for writing an BDD.
 */
sylvan_skiplist_t zdd_writer_start(void);

/**
 * Add the given ZDD to the skiplist.
 */
VOID_TASK_DECL_2(zdd_writer_add, sylvan_skiplist_t, ZDD);
#define zdd_writer_add(sl, dd) RUN(zdd_writer_add, sl, dd)

/**
 * Write all assigned ZDD nodes in binary format to the file.
 */
void zdd_writer_writebinary(FILE *out, sylvan_skiplist_t sl);

/**
 * Retrieve the identifier of the given stored ZDD.
 * This is useful if you want to be able to retrieve the stored ZDD later.
 */
uint64_t zdd_writer_get(sylvan_skiplist_t sl, ZDD dd);

/**
 * Free the allocated skiplist.
 */
void zdd_writer_end(sylvan_skiplist_t sl);

/**
 * Reading ZDDs from file (binary format).
 *
 * The function zdd_reader_frombinary is basically the reverse of zdd_writer_tobinary.
 *
 * One can also perform the procedure manually.
 * - call zdd_reader_readbinary to read the nodes from file
 * - call zdd_reader_get to obtain the ZDD for the given identifier as stored in the file.
 * - call zdd_reader_end to free the array returned by zdd_reader_readbinary
 *
 * Returns 0 if successful, -1 otherwise.
 */

/*
 * Read <count> decision diagrams to <dds> from <file> in internal binary form.
 */
TASK_DECL_3(int, zdd_reader_frombinary, FILE*, ZDD*, int);
#define zdd_reader_frombinary(file, dds, count) RUN(zdd_reader_frombinary, file, dds, count)

/**
 * Reading a file earlier written with zdd_writer_writebinary
 * Returns an array with the conversion from stored identifier to ZDD
 * This array is allocated with malloc and must be freed afterwards.
 * Returns NULL if there was an error.
 */

TASK_DECL_1(uint64_t*, zdd_reader_readbinary, FILE*);
#define zdd_reader_readbinary(file) RUN(zdd_reader_readbinary, file)

/**
 * Retrieve the ZDD of the given stored identifier.
 */
ZDD zdd_reader_get(uint64_t* arr, uint64_t identifier);

/**
 * Free the allocated translation array
 */
void zdd_reader_end(uint64_t *arr);

/**
 * Garbage collection
 */

/**
 * Call zdd_gc_mark_rec for every zdd you want to keep in your custom mark functions.
 */
VOID_TASK_DECL_1(zdd_gc_mark_rec, ZDD);
#define zdd_gc_mark_rec(zdd) RUN(zdd_gc_mark_rec, zdd)

/**
 * Default external pointer referencing. During garbage collection, the pointers are followed and the ZDD
 * that they refer to are kept in the forest.
 */
void zdd_protect(ZDD* ptr);
void zdd_unprotect(ZDD* ptr);
size_t zdd_count_protected(void);

/**
 * If sylvan_set_ondead is set to a callback, then this function marks ZDDs (terminals).
 * When they are dead after the mark phase in garbage collection, the callback is called for marked ZDDs.
 * The ondead callback can either perform cleanup or resurrect dead terminals.
 */
#define zdd_notify_ondead(dd) llmsset_notify_ondead(nodes, dd&~zdd_complement)

/**
 * Infrastructure for internal references.
 * Every thread has its own reference stacks. There are three stacks: pointer, values, tasks stack.
 * The pointers stack stores pointers to ZDD variables, manipulated with pushptr and popptr.
 * The values stack stores ZDDs, manipulated with push and pop.
 * The tasks stack stores Lace tasks (that return ZDDs), manipulated with spawn and sync.
 *
 * It is recommended to use the pointers stack for local variables and the tasks stack for tasks.
 */

/**
 * Push a ZDD variable to the pointer reference stack.
 * During garbage collection the variable will be inspected and the contents will be marked.
 */
void zdd_refs_pushptr(ZDD *ptr);

/**
 * Pop the last <amount> ZDD variables from the pointer reference stack.
 */
void zdd_refs_popptr(size_t amount);

/**
 * Push an ZDD to the values reference stack.
 * During garbage collection the references ZDD will be marked.
 */
ZDD zdd_refs_push(ZDD zdd);

/**
 * Pop the last <amount> ZDDs from the values reference stack.
 */
void zdd_refs_pop(long amount);

/**
 * Push a Task that returns an ZDD to the tasks reference stack.
 * Usage: zdd_refs_spawn(SPAWN(function, ...));
 */
void zdd_refs_spawn(Task *t);

/**
 * Pop a Task from the task reference stack.
 * Usage: ZDD result = zdd_refs_sync(SYNC(function));
 */
ZDD zdd_refs_sync(ZDD mtbdd);



#ifdef __cplusplus
}
#endif /* __cplusplus */

/* 
 * A lot of functionality is still missing from this initial implementation...
 *
 * Visitor (enumerate using callback)
 * Reader/Writer
 * Enum SAT Par/Seq
 * Enum PATH Par/Seq?
 * Eval_compose (for par learning)
 * printDot
 * compose functions
 *
 * Functions supported by CuDD
 * - zddOne (compute representation of universe given domain)
 * - const functions, specifically diffConst
 * - product and quotient of "unate covers" (zddUnateProduct, zddDivide)
 * - product and quotien of "binate covers" (zddProduct, zddWeakDiv)
 * - complement of a cover (zddComplement)
 * - reorder functions
 * - "realignment of variables"
 * - print a SOP representation of a ZDD
 * Functions that can be done with zdd_compose
 * - zddChange (subst variable by its complement)
 * - cofactor
 * Functions supported by EXTRA
 * - quite a lot.
 * Generic functions for other leaf types
 * - apply_op
 * - uapply_op
 * - applyp_op
 * - abstract_op
 * Specific functions for other leaf types
 * - negate, plus, minus, times, min, max
 * - abstract_plus, abstract_times, abstract_min, abstract_max
 * - non-boolean ite
 * - threshold, strict_threshold
 * - equal_norm_d, equal_norm_rel_d
 * - leq, less, geq, greater
 * - minimum, maximum (gets lowest/highest leaf)
 */

#endif
