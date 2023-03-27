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

#ifndef REFS_INLINE_H
#define REFS_INLINE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Implementation of external references
 * Based on a hash table for 40-bit non-null values, linear probing
 * Use tombstones for deleting, higher bits for reference count
 */
typedef struct
{
    uint64_t *refs_table;           // table itself
    size_t refs_size;               // number of buckets

    /* helpers during resize operation */
    volatile uint32_t refs_control; // control field
    uint64_t *refs_resize_table;    // previous table
    size_t refs_resize_size;        // size of previous table
    size_t refs_resize_part;        // which part is next
    size_t refs_resize_done;        // how many parts are done
} refs_table_t;

// Count number of unique entries (not number of references)
size_t refs_count(refs_table_t *tbl);

// Increase or decrease reference to 40-bit value a
// Will fail (assertion) if more down than up are called for a
void refs_up(refs_table_t *tbl, uint64_t a);
void refs_down(refs_table_t *tbl, uint64_t a);

// Return a bucket or NULL to start iterating
uint64_t *refs_iter(refs_table_t *tbl, size_t first, size_t end);

// Continue iterating, set bucket to next bucket or NULL
uint64_t refs_next(refs_table_t *tbl, uint64_t **bucket, size_t end);

// User must supply a pointer, refs_create and refs_free handle initialization/destruction
void refs_create(refs_table_t *tbl, size_t _refs_size);
void refs_free(refs_table_t *tbl);

// The same, but now for 64-bit values ("protect pointers")
size_t protect_count(refs_table_t *tbl);
void protect_up(refs_table_t *tbl, uint64_t a);
void protect_down(refs_table_t *tbl, uint64_t a);
uint64_t *protect_iter(refs_table_t *tbl, size_t first, size_t end);
uint64_t protect_next(refs_table_t *tbl, uint64_t **bucket, size_t end);
void protect_create(refs_table_t *tbl, size_t _refs_size);
void protect_free(refs_table_t *tbl);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
