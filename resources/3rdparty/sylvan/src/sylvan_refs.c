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

#include <sylvan.h>
#include <sylvan_refs.h>

#include <errno.h>  // for errno
#include <string.h> // for strerror
#include <sys/mman.h> // for mmap

#ifndef compiler_barrier
#define compiler_barrier() { asm volatile("" ::: "memory"); }
#endif

#ifndef cas
#define cas(ptr, old, new) (__sync_bool_compare_and_swap((ptr),(old),(new)))
#endif

/**
 * Implementation of external references
 * Based on a hash table for 40-bit non-null values, linear probing
 * Use tombstones for deleting, higher bits for reference count
 */
static const uint64_t refs_ts = 0x7fffffffffffffff; // tombstone

/* FNV-1a 64-bit hash */
static inline uint64_t
fnv_hash(uint64_t a)
{
    const uint64_t prime = 1099511628211;
    uint64_t hash = 14695981039346656037LLU;
    hash = (hash ^ a) * prime;
    hash = (hash ^ ((a << 25) | (a >> 39))) * prime;
    return hash ^ (hash >> 32);
}

// Count number of unique entries (not number of references)
size_t
refs_count(refs_table_t *tbl)
{
    size_t count = 0;
    uint64_t *bucket = tbl->refs_table;
    uint64_t * const end = bucket + tbl->refs_size;
    while (bucket != end) {
        if (*bucket != 0 && *bucket != refs_ts) count++;
        bucket++;
    }
    return count;
}

static inline void
refs_rehash(refs_table_t *tbl, uint64_t v)
{
    if (v == 0) return; // do not rehash empty value
    if (v == refs_ts) return; // do not rehash tombstone

    volatile uint64_t *bucket = tbl->refs_table + (fnv_hash(v & 0x000000ffffffffff) % tbl->refs_size);
    uint64_t * const end = tbl->refs_table + tbl->refs_size;

    int i = 128; // try 128 times linear probing
    while (i--) {
        if (*bucket == 0) { if (cas(bucket, 0, v)) return; }
        if (++bucket == end) bucket = tbl->refs_table;
    }

    // assert(0); // impossible!
}

/**
 * Called internally to assist resize operations
 * Returns 1 for retry, 0 for done
 */
static int
refs_resize_help(refs_table_t *tbl)
{
    if (0 == (tbl->refs_control & 0xf0000000)) return 0; // no resize in progress (anymore)
    if (tbl->refs_control & 0x80000000) return 1; // still waiting for preparation

    if (tbl->refs_resize_part >= tbl->refs_resize_size / 128) return 1; // all parts claimed
    size_t part = __sync_fetch_and_add(&tbl->refs_resize_part, 1);
    if (part >= tbl->refs_resize_size/128) return 1; // all parts claimed

    // rehash all
    int i;
    volatile uint64_t *bucket = tbl->refs_resize_table + part * 128;
    for (i=0; i<128; i++) refs_rehash(tbl, *bucket++);

    __sync_fetch_and_add(&tbl->refs_resize_done, 1);
    return 1;
}

static void
refs_resize(refs_table_t *tbl)
{
    while (1) {
        uint32_t v = tbl->refs_control;
        if (v & 0xf0000000) {
            // someone else started resize
            // just rehash blocks until done
            while (refs_resize_help(tbl)) continue;
            return;
        }
        if (cas(&tbl->refs_control, v, 0x80000000 | v)) {
            // wait until all users gone
            while (tbl->refs_control != 0x80000000) continue;
            break;
        }
    }

    tbl->refs_resize_table = tbl->refs_table;
    tbl->refs_resize_size = tbl->refs_size;
    tbl->refs_resize_part = 0;
    tbl->refs_resize_done = 0;

    // calculate new size
    size_t new_size = tbl->refs_size;
    size_t count = refs_count(tbl);
    if (count*4 > tbl->refs_size) new_size *= 2;

    // allocate new table
    uint64_t *new_table = (uint64_t*)mmap(0, new_size * sizeof(uint64_t), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (new_table == (uint64_t*)-1) {
        fprintf(stderr, "refs: Unable to allocate memory: %s!\n", strerror(errno));
        exit(1);
    }

    // set new data and go
    tbl->refs_table = new_table;
    tbl->refs_size = new_size;
    compiler_barrier();
    tbl->refs_control = 0x40000000;

    // until all parts are done, rehash blocks
    while (tbl->refs_resize_done != tbl->refs_resize_size/128) refs_resize_help(tbl);

    // done!
    compiler_barrier();
    tbl->refs_control = 0;

    // unmap old table
    munmap(tbl->refs_resize_table, tbl->refs_resize_size * sizeof(uint64_t));
}

/* Enter refs_modify */
static inline void
refs_enter(refs_table_t *tbl)
{
    for (;;) {
        uint32_t v = tbl->refs_control;
        if (v & 0xf0000000) {
            while (refs_resize_help(tbl)) continue;
        } else {
            if (cas(&tbl->refs_control, v, v+1)) return;
        }
    }
}

/* Leave refs_modify */
static inline void
refs_leave(refs_table_t *tbl)
{
    for (;;) {
        uint32_t v = tbl->refs_control;
        if (cas(&tbl->refs_control, v, v-1)) return;
    }
}

static inline int
refs_modify(refs_table_t *tbl, const uint64_t a, const int dir)
{
    volatile uint64_t *bucket;
    volatile uint64_t *ts_bucket;
    uint64_t v, new_v;
    int res, i;

    refs_enter(tbl);

ref_retry:
    bucket = tbl->refs_table + (fnv_hash(a) & (tbl->refs_size - 1));
    ts_bucket = NULL; // tombstone
    i = 128; // try 128 times linear probing

    while (i--) {
ref_restart:
        v = *bucket;
        if (v == refs_ts) {
            if (ts_bucket == NULL) ts_bucket = bucket;
        } else if (v == 0) {
            // not found
            res = 0;
            if (dir < 0) goto ref_exit;
            if (ts_bucket != NULL) {
                bucket = ts_bucket;
                ts_bucket = NULL;
                v = refs_ts;
            }
            new_v = a | (1ULL << 40);
            goto ref_mod;
        } else if ((v & 0x000000ffffffffff) == a) {
            // found
            res = 1;
            uint64_t count = v >> 40;
            if (count == 0x7fffff) goto ref_exit;
            count += dir;
            if (count == 0) new_v = refs_ts;
            else new_v = a | (count << 40);
            goto ref_mod;
        }

        if (++bucket == tbl->refs_table + tbl->refs_size) bucket = tbl->refs_table;
    }

    // not found after linear probing
    if (dir < 0) {
        res = 0;
        goto ref_exit;
    } else if (ts_bucket != NULL) {
        bucket = ts_bucket;
        ts_bucket = NULL;
        v = refs_ts;
        new_v = a | (1ULL << 40);
        if (!cas(bucket, v, new_v)) goto ref_retry;
        res = 1;
        goto ref_exit;
    } else {
        // hash table full
        refs_leave(tbl);
        refs_resize(tbl);
        return refs_modify(tbl, a, dir);
    }

ref_mod:
    if (!cas(bucket, v, new_v)) goto ref_restart;

ref_exit:
    refs_leave(tbl);
    return res;
}

void
refs_up(refs_table_t *tbl, uint64_t a)
{
    refs_modify(tbl, a, 1);
}

void
refs_down(refs_table_t *tbl, uint64_t a)
{
#ifdef NDEBUG
    refs_modify(tbl, a, -1);
#else
    int res = refs_modify(tbl, a, -1);
    assert(res != 0);
#endif
}

uint64_t*
refs_iter(refs_table_t *tbl, size_t first, size_t end)
{
    // assert(first < tbl->refs_size);
    // assert(end <= tbl->refs_size);

    uint64_t *bucket = tbl->refs_table + first;
    while (bucket != tbl->refs_table + end) {
        if (*bucket != 0 && *bucket != refs_ts) return bucket;
        bucket++;
    }
    return NULL;
}

uint64_t
refs_next(refs_table_t *tbl, uint64_t **_bucket, size_t end)
{
    uint64_t *bucket = *_bucket;
    // assert(bucket != NULL);
    // assert(end <= tbl->refs_size);
    uint64_t result = *bucket & 0x000000ffffffffff;
    bucket++;
    while (bucket != tbl->refs_table + end) {
        if (*bucket != 0 && *bucket != refs_ts) {
            *_bucket = bucket;
            return result;
        }
        bucket++;
    }
    *_bucket = NULL;
    return result;
}

void
refs_create(refs_table_t *tbl, size_t _refs_size)
{
    if (__builtin_popcountll(_refs_size) != 1) {
        fprintf(stderr, "refs: Table size must be a power of 2!\n");
        exit(1);
    }

    tbl->refs_size = _refs_size;
    tbl->refs_table = (uint64_t*)mmap(0, tbl->refs_size * sizeof(uint64_t), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (tbl->refs_table == (uint64_t*)-1) {
        fprintf(stderr, "refs: Unable to allocate memory: %s!\n", strerror(errno));
        exit(1);
    }
}

void
refs_free(refs_table_t *tbl)
{
    munmap(tbl->refs_table, tbl->refs_size * sizeof(uint64_t));
}

/**
 * Simple implementation of a 64-bit resizable hash-table
 * No idea if this is scalable... :( but it seems thread-safe
 */

// Count number of unique entries (not number of references)
size_t
protect_count(refs_table_t *tbl)
{
    size_t count = 0;
    uint64_t *bucket = tbl->refs_table;
    uint64_t * const end = bucket + tbl->refs_size;
    while (bucket != end) {
        if (*bucket != 0 && *bucket != refs_ts) count++;
        bucket++;
    }
    return count;
}

static inline void
protect_rehash(refs_table_t *tbl, uint64_t v)
{
    if (v == 0) return; // do not rehash empty value
    if (v == refs_ts) return; // do not rehash tombstone

    volatile uint64_t *bucket = tbl->refs_table + (fnv_hash(v) % tbl->refs_size);
    uint64_t * const end = tbl->refs_table + tbl->refs_size;

    int i = 128; // try 128 times linear probing
    while (i--) {
        if (*bucket == 0 && cas(bucket, 0, v)) return;
        if (++bucket == end) bucket = tbl->refs_table;
    }

    assert(0); // whoops!
}

/**
 * Called internally to assist resize operations
 * Returns 1 for retry, 0 for done
 */
static int
protect_resize_help(refs_table_t *tbl)
{
    if (0 == (tbl->refs_control & 0xf0000000)) return 0; // no resize in progress (anymore)
    if (tbl->refs_control & 0x80000000) return 1; // still waiting for preparation
    if (tbl->refs_resize_part >= tbl->refs_resize_size / 128) return 1; // all parts claimed
    size_t part = __sync_fetch_and_add(&tbl->refs_resize_part, 1);
    if (part >= tbl->refs_resize_size/128) return 1; // all parts claimed

    // rehash all
    int i;
    volatile uint64_t *bucket = tbl->refs_resize_table + part * 128;
    for (i=0; i<128; i++) protect_rehash(tbl, *bucket++);

    __sync_fetch_and_add(&tbl->refs_resize_done, 1);
    return 1;
}

static void
protect_resize(refs_table_t *tbl)
{
    while (1) {
        uint32_t v = tbl->refs_control;
        if (v & 0xf0000000) {
            // someone else started resize
            // just rehash blocks until done
            while (protect_resize_help(tbl)) continue;
            return;
        }
        if (cas(&tbl->refs_control, v, 0x80000000 | v)) {
            // wait until all users gone
            while (tbl->refs_control != 0x80000000) continue;
            break;
        }
    }

    tbl->refs_resize_table = tbl->refs_table;
    tbl->refs_resize_size = tbl->refs_size;
    tbl->refs_resize_part = 0;
    tbl->refs_resize_done = 0;

    // calculate new size
    size_t new_size = tbl->refs_size;
    size_t count = refs_count(tbl);
    if (count*4 > tbl->refs_size) new_size *= 2;

    // allocate new table
    uint64_t *new_table = (uint64_t*)mmap(0, new_size * sizeof(uint64_t), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (new_table == (uint64_t*)-1) {
        fprintf(stderr, "refs: Unable to allocate memory: %s!\n", strerror(errno));
        exit(1);
    }

    // set new data and go
    tbl->refs_table = new_table;
    tbl->refs_size = new_size;
    compiler_barrier();
    tbl->refs_control = 0x40000000;

    // until all parts are done, rehash blocks
    while (tbl->refs_resize_done < tbl->refs_resize_size/128) protect_resize_help(tbl);

    // done!
    compiler_barrier();
    tbl->refs_control = 0;

    // unmap old table
    munmap(tbl->refs_resize_table, tbl->refs_resize_size * sizeof(uint64_t));
}

static inline void
protect_enter(refs_table_t *tbl)
{
    for (;;) {
        uint32_t v = tbl->refs_control;
        if (v & 0xf0000000) {
            while (protect_resize_help(tbl)) continue;
        } else {
            if (cas(&tbl->refs_control, v, v+1)) return;
        }
    }
}

static inline void
protect_leave(refs_table_t *tbl)
{
    for (;;) {
        uint32_t v = tbl->refs_control;
        if (cas(&tbl->refs_control, v, v-1)) return;
    }
}

void
protect_up(refs_table_t *tbl, uint64_t a)
{
    volatile uint64_t *bucket;
    volatile uint64_t *ts_bucket;
    uint64_t v;
    int i;

    protect_enter(tbl);

ref_retry:
    bucket = tbl->refs_table + (fnv_hash(a) & (tbl->refs_size - 1));
    ts_bucket = NULL; // tombstone
    i = 128; // try 128 times linear probing

    while (i--) {
ref_restart:
        v = *bucket;
        if (v == refs_ts) {
            if (ts_bucket == NULL) ts_bucket = bucket;
        } else if (v == 0) {
            // go go go
            if (ts_bucket != NULL) {
                if (cas(ts_bucket, refs_ts, a)) {
                    protect_leave(tbl);
                    return;
                } else {
                    goto ref_retry;
                }
            } else {
                if (cas(bucket, 0, a)) {
                    protect_leave(tbl);
                    return;
                } else {
                    goto ref_restart;
                }
            }
        }

        if (++bucket == tbl->refs_table + tbl->refs_size) bucket = tbl->refs_table;
    }

    // not found after linear probing
    if (ts_bucket != NULL) {
        if (cas(ts_bucket, refs_ts, a)) {
            protect_leave(tbl);
            return;
        } else {
            goto ref_retry;
        }
    } else {
        // hash table full
        protect_leave(tbl);
        protect_resize(tbl);
        protect_enter(tbl);
        goto ref_retry;
    }
}

void
protect_down(refs_table_t *tbl, uint64_t a)
{
    volatile uint64_t *bucket;
    protect_enter(tbl);

    bucket = tbl->refs_table + (fnv_hash(a) & (tbl->refs_size - 1));
    int i = 128; // try 128 times linear probing

    while (i--) {
        if (*bucket == a) {
            *bucket = refs_ts;
            protect_leave(tbl);
            return;
        }
        if (++bucket == tbl->refs_table + tbl->refs_size) bucket = tbl->refs_table;
    }

    // not found after linear probing
    assert(0);
}

uint64_t*
protect_iter(refs_table_t *tbl, size_t first, size_t end)
{
    // assert(first < tbl->refs_size);
    // assert(end <= tbl->refs_size);

    uint64_t *bucket = tbl->refs_table + first;
    while (bucket != tbl->refs_table + end) {
        if (*bucket != 0 && *bucket != refs_ts) return bucket;
        bucket++;
    }
    return NULL;
}

uint64_t
protect_next(refs_table_t *tbl, uint64_t **_bucket, size_t end)
{
    uint64_t *bucket = *_bucket;
    // assert(bucket != NULL);
    // assert(end <= tbl->refs_size);
    uint64_t result = *bucket;
    bucket++;
    while (bucket != tbl->refs_table + end) {
        if (*bucket != 0 && *bucket != refs_ts) {
            *_bucket = bucket;
            return result;
        }
        bucket++;
    }
    *_bucket = NULL;
    return result;
}

void
protect_create(refs_table_t *tbl, size_t _refs_size)
{
    if (__builtin_popcountll(_refs_size) != 1) {
        fprintf(stderr, "refs: Table size must be a power of 2!\n");
        exit(1);
    }

    tbl->refs_size = _refs_size;
    tbl->refs_table = (uint64_t*)mmap(0, tbl->refs_size * sizeof(uint64_t), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (tbl->refs_table == (uint64_t*)-1) {
        fprintf(stderr, "refs: Unable to allocate memory: %s!\n", strerror(errno));
        exit(1);
    }
}

void
protect_free(refs_table_t *tbl)
{
    munmap(tbl->refs_table, tbl->refs_size * sizeof(uint64_t));
    tbl->refs_table = 0;
}
