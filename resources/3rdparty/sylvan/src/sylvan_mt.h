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

/**
 * This file contains declarations for custom Multi-Terminal support.
 */

/* Do not include this file directly. Instead, include sylvan.h */

#ifndef SYLVAN_MT_H
#define SYLVAN_MT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Helper implementation for custom terminal (multi-terminal support)
 * Types can implement the following callback functions:
 *
 * hash(value, seed)
 *     return hash of value with given seed
 * equals(value1, value2)
 *     return 1 if equal, 0 if not equal
 * create(&value)
 *     optionally allocate object and update value with the pointer
 * destroy(value)
 *     optionally destroy/free value if value points to an allocated object
 * to_str(complemented, value, buf, bufsize)
 *     return string representation of (complemented) value to buf if bufsize large enough,
 *     otherwise return newly allocated string
 * write_binary(fp, value)
 *     write binary representation of a leaf to given FILE* fp
 *     return 0 if successful
 * read_binary(fp, &value)
 *     read binary representation of a leaf from given FILE* fp
 *     treat allocated objects like create (and destroy)
 *     return 0 if successful
 *
 * If the 64-byte value already completely describes the leaf, then the functions
 * write_binary and read_binary should be set to NULL.
 *
 * If the 64-byte value is also already a canonical representation, then the functions
 * hash, equals, create and destroy should be set to NULL.
 *
 * Two values are equal (with equals) iff they have the same hash (with hash)
 *
 * A value v obtained due to create must be equal to the original value (with equals):
 *     create(v) => equals(\old(v), \new(v))
 *
 * NOTE ON EXPECTED LEAF NODE STRUCTURE: the implementation expects leaves in a specific format:
 * - 16-byte node { uint64_t a, b; }
 * - type  == a & 0x00000000ffffffff
 * - value == b
 */
typedef uint64_t (*sylvan_mt_hash_cb)(uint64_t, uint64_t);
typedef int (*sylvan_mt_equals_cb)(uint64_t, uint64_t);
typedef void (*sylvan_mt_create_cb)(uint64_t*);
typedef void (*sylvan_mt_destroy_cb)(uint64_t);
typedef char* (*sylvan_mt_to_str_cb)(int, uint64_t, char*, size_t);
typedef int (*sylvan_mt_write_binary_cb)(FILE*, uint64_t);
typedef int (*sylvan_mt_read_binary_cb)(FILE*, uint64_t*);

/**
 * Initialize the multi-terminal subsystem
 */
void sylvan_init_mt(void);

/**
 * Register a new leaf type.
 */
uint32_t sylvan_mt_create_type(void);

/**
 * Set the callback handlers for <type>
 */
void sylvan_mt_set_hash(uint32_t type, sylvan_mt_hash_cb hash_cb);
void sylvan_mt_set_equals(uint32_t type, sylvan_mt_equals_cb equals_cb);
void sylvan_mt_set_create(uint32_t type, sylvan_mt_create_cb create_cb);
void sylvan_mt_set_destroy(uint32_t type, sylvan_mt_destroy_cb destroy_cb);
void sylvan_mt_set_to_str(uint32_t type, sylvan_mt_to_str_cb to_str_cb);
void sylvan_mt_set_write_binary(uint32_t type, sylvan_mt_write_binary_cb write_binary_cb);
void sylvan_mt_set_read_binary(uint32_t type, sylvan_mt_read_binary_cb read_binary_cb);

/**
 * Returns 1 if the given type implements hash, or 0 otherwise.
 * (used when inserting into the unique table)
 */
int sylvan_mt_has_custom_hash(uint32_t type);

/**
 * Get a hash for given value (calls hash callback of type).
 * If the type does not implement hash, then this is the same hash as used by the unique table.
 */
uint64_t sylvan_mt_hash(uint32_t type, uint64_t value, uint64_t seed);

/**
 * Get text representation of leaf (calls to_str callback of type).
 */
char *sylvan_mt_to_str(int complement, uint32_t type, uint64_t value, char *buf, size_t buflen);

/**
 * Write a leaf in binary form (calls write_binary callback of type).
 */
int sylvan_mt_write_binary(uint32_t type, uint64_t value, FILE *out);

/**
 * Read a leaf in binary form (calls read_binary callback of type).
 */
int sylvan_mt_read_binary(uint32_t type, uint64_t *value, FILE *in);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
