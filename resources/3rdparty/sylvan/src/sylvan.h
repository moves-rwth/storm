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
 * Sylvan: parallel BDD/ListDD package.
 *
 * This is a multi-core implementation of BDDs with complement edges.
 *
 * This package requires parallel the work-stealing framework Lace.
 * Lace must be initialized before initializing Sylvan
 *
 * This package uses explicit referencing.
 * Use sylvan_ref and sylvan_deref to manage external references.
 *
 * Garbage collection requires all workers to cooperate. Garbage collection is either initiated
 * by the user (calling sylvan_gc) or when the nodes table is full. All Sylvan operations
 * check whether they need to cooperate on garbage collection. Garbage collection cannot occur
 * otherwise. This means that it is perfectly fine to do this:
 *              BDD a = sylvan_ref(sylvan_and(b, c));
 * since it is not possible that garbage collection occurs between the two calls.
 *
 * To temporarily disable garbage collection, use sylvan_gc_disable() and sylvan_gc_enable().
 */

#include <sylvan_config.h>

#include <stdint.h>
#include <stdio.h> // for FILE
#include <stdlib.h>
#include <lace.h> // for definitions

#include <sylvan_cache.h>
#include <llmsset.h>
#include <stats.h>

#ifndef SYLVAN_H
#define SYLVAN_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef SYLVAN_SIZE_FIBONACCI
#define SYLVAN_SIZE_FIBONACCI 0
#endif

// For now, only support 64-bit systems
typedef char __sylvan_check_size_t_is_8_bytes[(sizeof(uint64_t) == sizeof(size_t))?1:-1];

/**
 * Initialize the Sylvan parallel decision diagrams package.
 *
 * After initialization, call sylvan_init_bdd and/or sylvan_init_ldd if you want to use
 * the BDD and/or LDD functionality.
 *
 * BDDs and LDDs share a common node table and operations cache.
 *
 * The node table is resizable.
 * The table is resized automatically when >50% of the table is filled during garbage collection.
 * This behavior can be customized by overriding the gc hook.
 * 
 * Memory usage:
 * Every node requires 24 bytes memory. (16 bytes data + 8 bytes overhead)
 * Every operation cache entry requires 36 bytes memory. (32 bytes data + 4 bytes overhead)
 *
 * Reasonable defaults: datasize of 1L<<26 (2048 MB), cachesize of 1L<<25 (1152 MB)
 */
void sylvan_init_package(size_t initial_tablesize, size_t max_tablesize, size_t initial_cachesize, size_t max_cachesize);

/**
 * Frees all Sylvan data (also calls the quit() functions of BDD/MDD parts)
 */
void sylvan_quit();

/**
 * Return number of occupied buckets in nodes table and total number of buckets.
 */
VOID_TASK_DECL_2(sylvan_table_usage, size_t*, size_t*);
#define sylvan_table_usage(filled, total) (CALL(sylvan_table_usage, filled, total))

/**
 * Perform garbage collection.
 *
 * Garbage collection is performed in a new Lace frame, interrupting all ongoing work
 * until garbage collection is completed.
 *
 * Garbage collection procedure:
 * 1) The operation cache is cleared and the hash table is reset.
 * 2) All live nodes are marked (to be rehashed). This is done by the "mark" callbacks.
 * 3) The "hook" callback is called.
 *    By default, this doubles the hash table size when it is >50% full.
 * 4) All live nodes are rehashed into the hash table.
 *
 * The behavior of garbage collection can be customized by adding "mark" callbacks and
 * replacing the "hook" callback.
 */
VOID_TASK_DECL_0(sylvan_gc);
#define sylvan_gc() (CALL(sylvan_gc))

/**
 * Enable or disable garbage collection.
 *
 * This affects both automatic and manual garbage collection, i.e.,
 * calling sylvan_gc() while garbage collection is disabled does not have any effect.
 */
void sylvan_gc_enable();
void sylvan_gc_disable();

/**
 * Add a "mark" callback to the list of callbacks.
 *
 * These are called during garbage collection to recursively mark nodes.
 *
 * Default "mark" functions that mark external references (via sylvan_ref) and internal
 * references (inside operations) are added by sylvan_init_bdd/sylvan_init_bdd.
 *
 * Functions are called in order.
 * level 10: marking functions of Sylvan (external/internal references)
 * level 20: call the hook function (for resizing)
 * level 30: rehashing
 */
LACE_TYPEDEF_CB(void, gc_mark_cb);
void sylvan_gc_add_mark(int order, gc_mark_cb callback);

/**
 * Set "hook" callback. There can be only one.
 *
 * The hook is called after the "mark" phase and before the "rehash" phase.
 * This allows users to perform certain actions, such as resizing the nodes table
 * and the operation cache. Also, dynamic resizing could be performed then.
 */
LACE_TYPEDEF_CB(void, gc_hook_cb);
void sylvan_gc_set_hook(gc_hook_cb new_hook);

/**
 * One of the hooks for resizing behavior.
 * Default if SYLVAN_AGGRESSIVE_RESIZE is set.
 * Always double size on gc() until maximum reached.
 */
VOID_TASK_DECL_0(sylvan_gc_aggressive_resize);

/**
 * One of the hooks for resizing behavior.
 * Default if SYLVAN_AGGRESSIVE_RESIZE is not set.
 * Double size on gc() whenever >50% is used.
 */
VOID_TASK_DECL_0(sylvan_gc_default_hook);

/**
 * Set "notify on dead" callback for the nodes table.
 * See also documentation in llmsset.h
 */
#define sylvan_set_ondead(cb, ctx) llmsset_set_ondead(nodes, cb, ctx)

/**
 * Global variables (number of workers, nodes table)
 */

extern llmsset_t nodes;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <sylvan_bdd.h>
#include <sylvan_ldd.h>
#include <sylvan_mtbdd.h>

#endif
