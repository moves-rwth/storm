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
#include <stdint.h>
#include <unistd.h>

#include <lace.h>

#ifndef LLMSSET_H
#define LLMSSET_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef LLMSSET_MASK
#define LLMSSET_MASK 0 // set to 1 to use bit mask instead of modulo
#endif

/**
 * Lockless hash table (set) to store 16-byte keys.
 * Each unique key is associated with a 42-bit number.
 *
 * The set has support for stop-the-world garbage collection.
 * Methods llmsset_clear, llmsset_mark and llmsset_rehash implement garbage collection.
 * During their execution, llmsset_lookup is not allowed.
 */

/**
 * hash(a, b, seed)
 * equals(lhs_a, lhs_b, rhs_a, rhs_b)
 * create(a, b) -- with a,b pointers, allows changing pointers on create of node,
 *                 but must keep hash/equals same!
 * destroy(a, b)
 */
typedef uint64_t (*llmsset_hash_cb)(uint64_t, uint64_t, uint64_t);
typedef int (*llmsset_equals_cb)(uint64_t, uint64_t, uint64_t, uint64_t);
typedef void (*llmsset_create_cb)(uint64_t *, uint64_t *);
typedef void (*llmsset_destroy_cb)(uint64_t, uint64_t);

typedef struct llmsset
{
    uint64_t          *table;       // table with hashes
    uint8_t           *data;        // table with values
    uint64_t          *bitmap1;     // ownership bitmap (per 512 buckets)
    uint64_t          *bitmap2;     // bitmap for "contains data"
    uint64_t          *bitmapc;     // bitmap for "use custom functions"
    size_t            max_size;     // maximum size of the hash table (for resizing)
    size_t            table_size;   // size of the hash table (number of slots) --> power of 2!
#if LLMSSET_MASK
    size_t            mask;         // size-1
#endif
    size_t            f_size;
    llmsset_hash_cb   hash_cb;      // custom hash function
    llmsset_equals_cb equals_cb;    // custom equals function
    llmsset_create_cb create_cb;    // custom create function
    llmsset_destroy_cb destroy_cb;  // custom destroy function
    int16_t           threshold;    // number of iterations for insertion until returning error
} *llmsset_t;

/**
 * Retrieve a pointer to the data associated with the 42-bit value.
 */
static inline void*
llmsset_index_to_ptr(const llmsset_t dbs, size_t index)
{
    return dbs->data + index * 16;
}

/**
 * Create the set.
 * This will allocate a set of <max_size> buckets in virtual memory.
 * The actual space used is <initial_size> buckets.
 */
llmsset_t llmsset_create(size_t initial_size, size_t max_size);

/**
 * Free the set.
 */
void llmsset_free(llmsset_t dbs);

/**
 * Retrieve the maximum size of the set.
 */
static inline size_t
llmsset_get_max_size(const llmsset_t dbs)
{
    return dbs->max_size;
}

/**
 * Retrieve the current size of the lockless MS set.
 */
static inline size_t
llmsset_get_size(const llmsset_t dbs)
{
    return dbs->table_size;
}

/**
 * Set the table size of the set.
 * Typically called during garbage collection, after clear and before rehash.
 * Returns 0 if dbs->table_size > dbs->max_size!
 */
static inline void
llmsset_set_size(llmsset_t dbs, size_t size)
{
    /* check bounds (don't be rediculous) */
    if (size > 128 && size <= dbs->max_size) {
        dbs->table_size = size;
#if LLMSSET_MASK
        /* Warning: if size is not a power of two, you will get interesting behavior */
        dbs->mask = dbs->table_size - 1;
#endif
        dbs->threshold = (64 - __builtin_clzll(dbs->table_size)) + 4; // doubling table_size increases threshold by 1
    }
}

/**
 * Core function: find existing data or add new.
 * Returns the unique 42-bit value associated with the data, or 0 when table is full.
 * Also, this value will never equal 0 or 1.
 * Note: garbage collection during lookup strictly forbidden
 */
uint64_t llmsset_lookup(const llmsset_t dbs, const uint64_t a, const uint64_t b, int *created);

/**
 * Same as lookup, but use the custom functions
 */
uint64_t llmsset_lookupc(const llmsset_t dbs, const uint64_t a, const uint64_t b, int *created);

/**
 * To perform garbage collection, the user is responsible that no lookups are performed during the process.
 *
 * 1) call llmsset_clear 
 * 2) call llmsset_mark for every bucket to rehash
 * 3) call llmsset_rehash 
 */
VOID_TASK_DECL_1(llmsset_clear, llmsset_t);
#define llmsset_clear(dbs) CALL(llmsset_clear, dbs)

/**
 * Check if a certain data bucket is marked (in use).
 */
int llmsset_is_marked(const llmsset_t dbs, uint64_t index);

/**
 * During garbage collection, buckets are marked (for rehashing) with this function.
 * Returns 0 if the node was already marked, or non-zero if it was not marked.
 * May also return non-zero if multiple workers marked at the same time.
 */
int llmsset_mark(const llmsset_t dbs, uint64_t index);

/**
 * Rehash all marked buckets.
 */
VOID_TASK_DECL_1(llmsset_rehash, llmsset_t);
#define llmsset_rehash(dbs) CALL(llmsset_rehash, dbs)

/**
 * Retrieve number of marked buckets.
 */
TASK_DECL_1(size_t, llmsset_count_marked, llmsset_t);
#define llmsset_count_marked(dbs) CALL(llmsset_count_marked, dbs)

/**
 * During garbage collection, this method calls the destroy callback
 * for all 'custom' data that is not kept.
 */
VOID_TASK_DECL_1(llmsset_destroy_unmarked, llmsset_t);
#define llmsset_destroy_unmarked(dbs) CALL(llmsset_destroy_unmarked, dbs)

/**
 * Set custom functions
 */
void llmsset_set_custom(const llmsset_t dbs, llmsset_hash_cb hash_cb, llmsset_equals_cb equals_cb, llmsset_create_cb create_cb, llmsset_destroy_cb destroy_cb);

/**
 * Default hashing function
 */
uint64_t llmsset_hash(const uint64_t a, const uint64_t b, const uint64_t seed);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
