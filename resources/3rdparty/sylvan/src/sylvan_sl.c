/*
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
#include <sylvan_sl.h>

#include <sys/mman.h> // for mmap, munmap, etc

/* A SL_DEPTH of 6 means 32 bytes per bucket, of 14 means 64 bytes per bucket.
   However, there is a very large performance drop with only 6 levels. */
#define SL_DEPTH 14

typedef struct
{
    BDD dd;
    uint32_t next[SL_DEPTH];
} sl_bucket;

struct sylvan_skiplist
{
    sl_bucket *buckets;
    size_t size;
    size_t next;
};

#ifndef cas
#define cas(ptr, old, new) (__sync_bool_compare_and_swap((ptr),(old),(new)))
#endif

sylvan_skiplist_t
sylvan_skiplist_alloc(size_t size)
{
    if (size >= 0x80000000) {
        fprintf(stderr, "sylvan: Trying to allocate a skiplist >= 0x80000000 buckets!\n");
        exit(1);
    }
    sylvan_skiplist_t l = malloc(sizeof(struct sylvan_skiplist));
    l->buckets = (sl_bucket*)mmap(0, sizeof(sl_bucket)*size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, 0, 0);
    if (l->buckets == (sl_bucket*)-1) {
        fprintf(stderr, "sylvan: Unable to allocate virtual memory (%'zu bytes) for the skiplist!\n", size*sizeof(sl_bucket));
        exit(1);
    }
    l->size = size;
    l->next = 1;
    return l;
}

void
sylvan_skiplist_free(sylvan_skiplist_t l)
{
    munmap(l->buckets, sizeof(sl_bucket)*l->size);
    free(l);
}

/**
 * Return the assigned number of the given dd,
 * or 0 if not found.
 */
uint64_t
sylvan_skiplist_get(sylvan_skiplist_t l, MTBDD dd)
{
    if (dd == mtbdd_false || dd == mtbdd_true) return 0;

    uint32_t loc = 0, k = SL_DEPTH-1;
    for (;;) {
        /* invariant: [loc].dd < dd */
        /* note: this is always true for loc==0 */
        sl_bucket *e = l->buckets + loc;
        uint32_t loc_next = (*(volatile uint32_t*)&e->next[k]) & 0x7fffffff;
        if (loc_next != 0 && l->buckets[loc_next].dd == dd) {
            /* found */
            return loc_next;
        } else if (loc_next != 0 && l->buckets[loc_next].dd < dd) {
            /* go right */
            loc = loc_next;
        } else if (k > 0) {
            /* go down */
            k--;
        } else {
            return 0;
        }
    }
}

VOID_TASK_IMPL_2(sylvan_skiplist_assign_next, sylvan_skiplist_t, l, MTBDD, dd)
{
    if (dd == mtbdd_false || dd == mtbdd_true) return;

    uint32_t trace[SL_DEPTH];
    uint32_t loc = 0, loc_next = 0, k = SL_DEPTH-1;
    for (;;) {
        /* invariant: [loc].dd < dd */
        /* note: this is always true for loc==0 */
        sl_bucket *e = l->buckets + loc;
        loc_next = (*(volatile uint32_t*)&e->next[k]) & 0x7fffffff;
        if (loc_next != 0 && l->buckets[loc_next].dd == dd) {
            /* found */
            return;
        } else if (loc_next != 0 && l->buckets[loc_next].dd < dd) {
            /* go right */
            loc = loc_next;
        } else if (k > 0) {
            /* go down */
            trace[k] = loc;
            k--;
        } else if (!(e->next[0] & 0x80000000) && cas(&e->next[0], loc_next, loc_next|0x80000000)) {
            /* locked */
            break;
        }
    }

    /* claim next item */
    const uint64_t next = __sync_fetch_and_add(&l->next, 1);
    if (next >= l->size) {
        fprintf(stderr, "Out of cheese exception, no more blocks available\n");
        exit(1);
    }

    /* fill next item */
    sl_bucket *a = l->buckets + next;
    a->dd = dd;
    a->next[0] = loc_next;
    compiler_barrier();
    l->buckets[loc].next[0] = next;

    /* determine height */
    uint64_t h = 1 + __builtin_clz(LACE_TRNG) / 2;
    if (h > SL_DEPTH) h = SL_DEPTH;

    /* go up and create links */
    for (k=1;k<h;k++) {
        loc = trace[k];
        for (;;) {
            sl_bucket *e = l->buckets + loc;
            /* note, at k>0, no locks on edges */
            uint32_t loc_next = *(volatile uint32_t*)&e->next[k];
            if (loc_next != 0 && l->buckets[loc_next].dd < dd) {
                loc = loc_next;
            } else {
                a->next[k] = loc_next;
                if (cas(&e->next[k], loc_next, next)) break;
            }
        }
    }
}

size_t
sylvan_skiplist_count(sylvan_skiplist_t l)
{
    return l->next - 1;
}

MTBDD
sylvan_skiplist_getr(sylvan_skiplist_t l, uint64_t index)
{
    return l->buckets[index].dd;
}
