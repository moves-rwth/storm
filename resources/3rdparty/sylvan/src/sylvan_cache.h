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

/* Do not include this file directly. Instead, include sylvan_int.h */

#ifndef SYLVAN_CACHE_H
#define SYLVAN_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Operation cache
 *
 * Use cache_next_opid() at initialization time to generate unique "operation identifiers".
 * You should store these operation identifiers as static globals in your implementation .C/.CPP file.
 *
 * For custom operations, just use the following functions:
 * - cache_get3/cache_put3 for any operation with 1 BDD and 2 other values (that can be BDDs)
 *   int success = cache_get3(opid, dd1, value2, value3, &result);
 *   int success = cache_put3(opid, dd1, value2, value3, result);
 * - cache_get4/cache_put4 for any operation with 4 BDDs
 *   int success = cache_get4(opid, dd1, dd2, dd3, dd4, &result);
 *   int success = cache_get4(opid, dd1, dd2, dd3, dd4, result);
 *
 * Notes:
 * - The "result" is any 64-bit value
 * - Use "0" for unused parameters
 */

typedef struct cache_entry *cache_entry_t;

/**
 * Primitives for cache get/put
 */
int cache_get(uint64_t a, uint64_t b, uint64_t c, uint64_t *res);
int cache_put(uint64_t a, uint64_t b, uint64_t c, uint64_t res);

/**
 * Primitives for cache get/put that use two buckets
 */
int cache_get6(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t *res1, uint64_t *res2);
int cache_put6(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t res1, uint64_t res2);

/**
 * Helper function to get next 'operation id' (during initialization of modules)
 */
uint64_t cache_next_opid(void);

/**
 * dd must be MTBDD, d2/d3 can be anything
 */
static inline int __attribute__((unused))
cache_get3(uint64_t opid, uint64_t dd, uint64_t d2, uint64_t d3, uint64_t *res)
{
    return cache_get(dd | opid, d2, d3, res);
}

/**
 * dd/dd2/dd3/dd4 must be MTBDDs
 */
static inline int __attribute__((unused))
cache_get4(uint64_t opid, uint64_t dd, uint64_t dd2, uint64_t dd3, uint64_t dd4, uint64_t *res)
{
    uint64_t p2 = dd2 | ((dd4 & 0x00000000000fffff) << 40); // 20 bits and complement bit
    if (dd4 & 0x8000000000000000) p2 |= 0x4000000000000000;
    uint64_t p3 = dd3 | ((dd4 & 0x000000fffff00000) << 20); // 20 bits

    return cache_get3(opid, dd, p2, p3, res);
}

/**
 * dd must be MTBDD, d2/d3 can be anything
 */
static inline int __attribute__((unused))
cache_put3(uint64_t opid, uint64_t dd, uint64_t d2, uint64_t d3, uint64_t res)
{
    return cache_put(dd | opid, d2, d3, res);
}

/**
 * dd/dd2/dd3/dd4 must be MTBDDs
 */
static inline int __attribute__((unused))
cache_put4(uint64_t opid, uint64_t dd, uint64_t dd2, uint64_t dd3, uint64_t dd4, uint64_t res)
{
    uint64_t p2 = dd2 | ((dd4 & 0x00000000000fffff) << 40); // 20 bits and complement bit
    if (dd4 & 0x8000000000000000) p2 |= 0x4000000000000000;
    uint64_t p3 = dd3 | ((dd4 & 0x000000fffff00000) << 20); // 20 bits

    return cache_put3(opid, dd, p2, p3, res);
}
/**
 * Functions for Sylvan for cache management
 */

void cache_create(size_t _cache_size, size_t _max_size);

void cache_free(void);

void cache_clear(void);

void cache_setsize(size_t size);

size_t cache_getused(void);

size_t cache_getsize(void);

size_t cache_getmaxsize(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
