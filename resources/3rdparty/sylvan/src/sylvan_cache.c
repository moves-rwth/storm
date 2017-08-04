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

#include <sylvan_int.h>

#include <errno.h>  // for errno
#include <string.h> // for strerror
#include <sys/mman.h> // for mmap

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#ifndef CACHE_MASK
#define CACHE_MASK 1
#endif

#ifndef compiler_barrier
#define compiler_barrier() { asm volatile("" ::: "memory"); }
#endif

#ifndef cas
#define cas(ptr, old, new) (__sync_bool_compare_and_swap((ptr),(old),(new)))
#endif

/**
 * This cache is designed to store a,b,c->res, with a,b,c,res 64-bit integers.
 *
 * Each cache bucket takes 32 bytes, 2 per cache line.
 * Each cache status bucket takes 4 bytes, 16 per cache line.
 * Therefore, size 2^N = 36*(2^N) bytes.
 */

struct __attribute__((packed)) cache6_entry {
    uint64_t            a;
    uint64_t            b;
    uint64_t            c;
    uint64_t            res;
    uint64_t            d;
    uint64_t            e;
    uint64_t            f;
    uint64_t            res2;
};
typedef struct cache6_entry *cache6_entry_t;

struct __attribute__((packed)) cache_entry {
    uint64_t            a;
    uint64_t            b;
    uint64_t            c;
    uint64_t            res;
};

static size_t             cache_size;         // power of 2
static size_t             cache_max;          // power of 2
#if CACHE_MASK
static size_t             cache_mask;         // cache_size-1
#endif
static cache_entry_t      cache_table;
static uint32_t*          cache_status;

static uint64_t           next_opid;

uint64_t
cache_next_opid()
{
    return __sync_fetch_and_add(&next_opid, 1LL<<40);
}

// status: 0x80000000 - bitlock
//         0x7fff0000 - hash (part of the 64-bit hash not used to position)
//         0x0000ffff - tag (every put increases tag field)

/* Rotating 64-bit FNV-1a hash */
static uint64_t
cache_hash(uint64_t a, uint64_t b, uint64_t c)
{
    const uint64_t prime = 1099511628211;
    uint64_t hash = 14695981039346656037LLU;
    hash = (hash ^ (a>>32));
    hash = (hash ^ a) * prime;
    hash = (hash ^ b) * prime;
    hash = (hash ^ c) * prime;
    return hash;
}

static uint64_t
cache_hash6(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f)
{
    const uint64_t prime = 1099511628211;
    uint64_t hash = 14695981039346656037LLU;
    hash = (hash ^ (a>>32));
    hash = (hash ^ a) * prime;
    hash = (hash ^ b) * prime;
    hash = (hash ^ c) * prime;
    hash = (hash ^ d) * prime;
    hash = (hash ^ e) * prime;
    hash = (hash ^ f) * prime;
    return hash;
}

int
cache_get6(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t *res1, uint64_t *res2)
{
    const uint64_t hash = cache_hash6(a, b, c, d, e, f);
#if CACHE_MASK
    volatile uint64_t *s_bucket = (uint64_t*)cache_status + (hash & cache_mask)/2;
    cache6_entry_t bucket = (cache6_entry_t)cache_table + (hash & cache_mask)/2;
#else
    volatile uint64_t *s_bucket = (uint64_t*)cache_status + (hash % cache_size)/2;
    cache6_entry_t bucket = (cache6_entry_t)cache_table + (hash % cache_size)/2;
#endif
    const uint64_t s = *s_bucket;
    compiler_barrier();
    // abort if locked or second part of 2-part entry or if different hash
    uint64_t x = ((hash>>32) & 0x7fff0000) | 0x04000000;
    x = x | (x<<32);
    if ((s & 0xffff0000ffff0000) != x) return 0;
    // abort if key different
    if (bucket->a != a || bucket->b != b || bucket->c != c) return 0;
    if (bucket->d != d || bucket->e != e || bucket->f != f) return 0;
    *res1 = bucket->res;
    if (res2) *res2 = bucket->res2;
    compiler_barrier();
    // abort if status field changed after compiler_barrier()
    return *s_bucket == s ? 1 : 0;
}

int
cache_put6(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t res1, uint64_t res2)
{
    const uint64_t hash = cache_hash6(a, b, c, d, e, f);
#if CACHE_MASK
    volatile uint64_t *s_bucket = (uint64_t*)cache_status + (hash & cache_mask)/2;
    cache6_entry_t bucket = (cache6_entry_t)cache_table + (hash & cache_mask)/2;
#else
    volatile uint64_t *s_bucket = (uint64_t*)cache_status + (hash % cache_size)/2;
    cache6_entry_t bucket = (cache6_entry_t)cache_table + (hash % cache_size)/2;
#endif
    const uint64_t s = *s_bucket;
    // abort if locked
    if (s & 0x8000000080000000LL) return 0;
    // create new
    uint64_t new_s = ((hash>>32) & 0x7fff0000) | 0x04000000;
    new_s |= (new_s<<32);
    new_s |= (((s>>32)+1)&0xffff)<<32;
    new_s |= (s+1)&0xffff;
    // use cas to claim bucket
    if (!cas(s_bucket, s, new_s | 0x8000000080000000LL)) return 0;
    // cas succesful: write data
    bucket->a = a;
    bucket->b = b;
    bucket->c = c;
    bucket->d = d;
    bucket->e = e;
    bucket->f = f;
    bucket->res = res1;
    bucket->res2 = res2;
    compiler_barrier();
    // after compiler_barrier(), unlock status field
    *s_bucket = new_s;
    return 1;
}

int
cache_get(uint64_t a, uint64_t b, uint64_t c, uint64_t *res)
{
    const uint64_t hash = cache_hash(a, b, c);
#if CACHE_MASK
    volatile uint32_t *s_bucket = cache_status + (hash & cache_mask);
    cache_entry_t bucket = cache_table + (hash & cache_mask);
#else
    volatile uint32_t *s_bucket = cache_status + (hash % cache_size);
    cache_entry_t bucket = cache_table + (hash % cache_size);
#endif
    const uint32_t s = *s_bucket;
    compiler_barrier();
    // abort if locked or if part of a 2-part cache entry
    if (s & 0xc0000000) return 0;
    // abort if different hash
    if ((s ^ (hash>>32)) & 0x3fff0000) return 0;
    // abort if key different
    if (bucket->a != a || bucket->b != b || bucket->c != c) return 0;
    *res = bucket->res;
    compiler_barrier();
    // abort if status field changed after compiler_barrier()
    return *s_bucket == s ? 1 : 0;
}

int
cache_put(uint64_t a, uint64_t b, uint64_t c, uint64_t res)
{
    const uint64_t hash = cache_hash(a, b, c);
#if CACHE_MASK
    volatile uint32_t *s_bucket = cache_status + (hash & cache_mask);
    cache_entry_t bucket = cache_table + (hash & cache_mask);
#else
    volatile uint32_t *s_bucket = cache_status + (hash % cache_size);
    cache_entry_t bucket = cache_table + (hash % cache_size);
#endif
    const uint32_t s = *s_bucket;
    // abort if locked
    if (s & 0x80000000) return 0;
    // abort if hash identical -> no: in iscasmc this occasionally causes timeouts?!
    const uint32_t hash_mask = (hash>>32) & 0x3fff0000;
    // if ((s & 0x7fff0000) == hash_mask) return 0;
    // use cas to claim bucket
    const uint32_t new_s = ((s+1) & 0x0000ffff) | hash_mask;
    if (!cas(s_bucket, s, new_s | 0x80000000)) return 0;
    // cas succesful: write data
    bucket->a = a;
    bucket->b = b;
    bucket->c = c;
    bucket->res = res;
    compiler_barrier();
    // after compiler_barrier(), unlock status field
    *s_bucket = new_s;
    return 1;
}

void
cache_create(size_t _cache_size, size_t _max_size)
{
#if CACHE_MASK
    // Cache size must be a power of 2
    if (__builtin_popcountll(_cache_size) != 1 || __builtin_popcountll(_max_size) != 1) {
        fprintf(stderr, "cache_create: Table size must be a power of 2!\n");
        exit(1);
    }
#endif

    cache_size = _cache_size;
    cache_max  = _max_size;
#if CACHE_MASK
    cache_mask = cache_size - 1;
#endif

    if (cache_size > cache_max) {
        fprintf(stderr, "cache_create: Table size must be <= max size!\n");
        exit(1);
    }

    cache_table = (cache_entry_t)mmap(0, cache_max * sizeof(struct cache_entry), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    cache_status = (uint32_t*)mmap(0, cache_max * sizeof(uint32_t), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (cache_table == (cache_entry_t)-1 || cache_status == (uint32_t*)-1) {
        fprintf(stderr, "cache_create: Unable to allocate memory: %s!\n", strerror(errno));
        exit(1);
    }

    next_opid = 512LL << 40;
}

void
cache_free()
{
    munmap(cache_table, cache_max * sizeof(struct cache_entry));
    munmap(cache_status, cache_max * sizeof(uint32_t));
}

void
cache_clear()
{
    // a bit silly, but this works just fine, and does not require writing 0 everywhere...
    cache_free();
    cache_create(cache_size, cache_max);
}

void
cache_setsize(size_t size)
{
    // easy solution
    cache_free();
    cache_create(size, cache_max);
}

size_t
cache_getsize()
{
    return cache_size;
}

size_t
cache_getused()
{
    size_t result = 0;
    for (size_t i=0;i<cache_size;i++) {
        uint32_t s = cache_status[i];
        if (s & 0x80000000) fprintf(stderr, "cache_getuser: cache in use during cache_getused()\n");
        if (s) result++;
    }
    return result;
}

size_t
cache_getmaxsize()
{
    return cache_max;
}
