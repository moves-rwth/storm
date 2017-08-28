/*
 * Copyright 2011-2016 Formal Methods and Tools, University of Twente
 * Copyright 2016-2017 Tom van Dijk, Johannes Kepler University Linz
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

#ifndef SYLVAN_COMMON_H
#define SYLVAN_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Initialize the Sylvan parallel decision diagrams package.
 *
 * First, Sylvan must know how big the nodes table and cache may be.
 * Either use sylvan_set_sizes to explicitly set the table sizes, or use sylvan_set_limits
 * to let Sylvan compute the sizes for you.
 *
 * Then, call sylvan_init_package. This allocates the tables and other support structures.
 * Sylvan allocates virtual memory to accomodate the maximum sizes of both tables.
 * Initially, Sylvan only uses the minimum sizes.
 * During garbage collection, table sizes may be doubled until the maximum size is reached.
 *
 * Then, call initialization functions for the MTBDD/LDD modules like sylvan_init_mtbdd
 * and sylvan_init_ldd.
 *
 * Memory usage:
 * Every node requires 24 bytes memory. (16 bytes data + 8 bytes table overhead)
 * Every operation cache entry requires 36 bytes memory. (32 bytes data + 4 bytes table overhead)
 */
void sylvan_init_package(void);

/**
 * Explicitly set the sizes of the nodes table and the operation cache.
 * The sizes are in bytes, but they must be powers of two.
 * The minimum size is the size initially used.
 * The maximum size is the size allocated in virtual memory.
 */
void sylvan_set_sizes(size_t min_tablesize, size_t max_tablesize, size_t min_cachesize, size_t max_cachesize);

/**
 * Implicitly compute and set the sizes of the nodes table and the operation cache.
 *
 * This function computes max_tablesize and max_cachesize to fit the memory cap.
 * The memory cap is in bytes.
 *
 * The parameter table_ratio controls the ratio between the nodes table and the cache.
 * For the value 0, both tables are of the same size.
 * For values 1, 2, 3 ... the nodes table will be 2x, 4x, 8x ... as big as the cache
 * For values -1, -2, -3 ... the cache will be 2x, 4x, 8x ... as big as the nodes table
 *
 * The parameter initial_ratio controls how much smaller the initial table sizes are.
 * For values of 1, 2, 3, 4 the tables will initially be 2, 4, 8, 16 times smaller.
 */
void sylvan_set_limits(size_t memory_cap, int table_ratio, int initial_ratio);

/**
 * Frees all Sylvan data (also calls the quit() functions of BDD/LDD parts)
 */
void sylvan_quit(void);

/**
 * Registers a hook callback called during sylvan_quit()
 */
typedef void (*quit_cb)(void);
void sylvan_register_quit(quit_cb cb);

/**
 * Return number of occupied buckets in nodes table and total number of buckets.
 */
VOID_TASK_DECL_2(sylvan_table_usage, size_t*, size_t*);
#define sylvan_table_usage(filled, total) (CALL(sylvan_table_usage, filled, total))

/**
 * GARBAGE COLLECTION
 *
 * Garbage collection is performed in a new Lace frame, interrupting all ongoing work
 * until garbage collection is completed.
 *
 * By default, garbage collection is triggered when no new nodes can be added to the nodes table.
 * This is detected when there are no more available buckets in the bounded probe sequence.
 * Garbage collection can also be triggered manually with sylvan_gc()
 *
 * Garbage collection procedure:
 * 1) All installed pre_gc hooks are called.
 *    See sylvan_gc_hook_pre to add hooks.
 * 2) The operation cache is cleared.
 * 3) The nodes table (data part) is cleared.
 * 4) All nodes are marked (to be rehashed) using the various marking callbacks.
 *    See sylvan_gc_add_mark to add marking callbacks.
 *    Afterwards, the ondead hook is called for all now-dead nodes with the custom flag set.
 * 5) The main gc hook is called. The function of this hook is to perform resizing.
 *    The default implementation doubles the nodes table and operation cache sizes.
 *    See sylvan_gc_hook_main to set the hook.
 * 5) The nodes table (hash part) is cleared.
 * 6) All marked nodes are rehashed.
 * 7) All installed post_gc hooks are called.
 *    See sylvan_gc_hook_post to add hooks.
 *
 * For parts of the garbage collection process, specific methods exist.
 * - sylvan_clear_cache() clears the operation cache (step 2)
 * - sylvan_clear_and_mark() performs steps 3 and 4.
 * - sylvan_rehash_all() performs steps 5 and 6.
 */

/**
 * Trigger garbage collection manually.
 */

/**
 * Trigger garbage collection manually.
 */
VOID_TASK_DECL_0(sylvan_gc);
#define sylvan_gc() (CALL(sylvan_gc))

/**
 * Enable or disable garbage collection.
 *
 * This affects both automatic and manual garbage collection, i.e.,
 * calling sylvan_gc() while garbage collection is disabled does not have any effect.
 * If no new nodes can be added, Sylvan will write an error and abort.
 */
void sylvan_gc_enable(void);
void sylvan_gc_disable(void);

/**
 * Test if garbage collection must happen now.
 * This is just a call to the Lace framework to see if NEWFRAME has been used.
 * Before calling this, make sure all used BDDs are referenced.
 */
#define sylvan_gc_test() YIELD_NEWFRAME()

/**
 * Clear the operation cache.
 */
VOID_TASK_DECL_0(sylvan_clear_cache);
#define sylvan_clear_cache() CALL(sylvan_clear_cache)

/**
 * Clear the nodes table (data part) and mark all nodes with the marking mechanisms.
 */
VOID_TASK_DECL_0(sylvan_clear_and_mark);
#define sylvan_clear_and_mark() CALL(sylvan_clear_and_mark)

/**
 * Clear the nodes table (hash part) and rehash all marked nodes.
 */
VOID_TASK_DECL_0(sylvan_rehash_all);
#define sylvan_rehash_all() CALL(sylvan_rehash_all)

/**
 * Callback type
 */
LACE_TYPEDEF_CB(void, gc_hook_cb);

/**
 * Add a hook that is called before garbage collection begins.
 */
void sylvan_gc_hook_pregc(gc_hook_cb callback);

/**
 * Add a hook that is called after garbage collection is finished.
 */
void sylvan_gc_hook_postgc(gc_hook_cb callback);

/**
 * Replace the hook called between node marking and rehashing.
 * Typically, the hook resizes the hash table and operation cache according to some heuristic.
 */
void sylvan_gc_hook_main(gc_hook_cb callback);

/**
 * Add a marking mechanism.
 *
 * The mark_cb callback is called during garbage collection and should call the
 * appropriate recursive marking functions for the decision diagram nodes, for example
 * mtbdd_gc_mark_rec() for MTBDDs or lddmc_gc_mark_rec() for LDDs.
 *
 * The sylvan_count_refs() function uses the count_cb callbacks to compute the number
 * of references.
 */
void sylvan_gc_add_mark(gc_hook_cb mark_cb);

/**
 * One of the hooks for resizing behavior.
 * Default if SYLVAN_AGGRESSIVE_RESIZE is set.
 * Always double size on gc() until maximum reached.
 * Use sylvan_gc_hook_main() to set this heuristic.
 */
VOID_TASK_DECL_0(sylvan_gc_aggressive_resize);

/**
 * One of the hooks for resizing behavior.
 * Default if SYLVAN_AGGRESSIVE_RESIZE is not set.
 * Double size on gc() whenever >50% is used.
 * Use sylvan_gc_hook_main() to set this heuristic.
 */
VOID_TASK_DECL_0(sylvan_gc_normal_resize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
