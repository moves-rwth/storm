#include <stdint.h> // for uint32_t etc

#include <sylvan_config.h>

#ifndef CACHE_H
#define CACHE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CACHE_MASK
#define CACHE_MASK 1
#endif

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
 * Helper function to get next 'operation id' (during initialization of modules)
 */
uint64_t cache_next_opid();

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

void cache_free();

void cache_clear();

void cache_setsize(size_t size);

size_t cache_getused();

size_t cache_getsize();

size_t cache_getmaxsize();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
