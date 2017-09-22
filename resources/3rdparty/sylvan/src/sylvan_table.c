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
#include <string.h> // memset
#include <sys/mman.h> // for mmap

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#ifndef cas
#define cas(ptr, old, new) (__sync_bool_compare_and_swap((ptr),(old),(new)))
#endif

DECLARE_THREAD_LOCAL(my_region, uint64_t);

VOID_TASK_0(llmsset_reset_region)
{
    LOCALIZE_THREAD_LOCAL(my_region, uint64_t);
    my_region = (uint64_t)-1; // no region
    SET_THREAD_LOCAL(my_region, my_region);
}

static uint64_t
claim_data_bucket(const llmsset_t dbs)
{
    LOCALIZE_THREAD_LOCAL(my_region, uint64_t);

    for (;;) {
        if (my_region != (uint64_t)-1) {
            // find empty bucket in region <my_region>
            uint64_t *ptr = dbs->bitmap2 + (my_region*8);
            int i=0;
            for (;i<8;) {
                uint64_t v = *ptr;
                if (v != 0xffffffffffffffffLL) {
                    int j = __builtin_clzll(~v);
                    *ptr |= (0x8000000000000000LL>>j);
                    return (8 * my_region + i) * 64 + j;
                }
                i++;
                ptr++;
            }
        } else {
            // special case on startup or after garbage collection
            my_region += (lace_get_worker()->worker*(dbs->table_size/(64*8)))/lace_workers();
        }
        uint64_t count = dbs->table_size/(64*8);
        for (;;) {
            // check if table maybe full
            if (count-- == 0) return (uint64_t)-1;

            my_region += 1;
            if (my_region >= (dbs->table_size/(64*8))) my_region = 0;

            // try to claim it
            uint64_t *ptr = dbs->bitmap1 + (my_region/64);
            uint64_t mask = 0x8000000000000000LL >> (my_region&63);
            uint64_t v;
restart:
            v = *ptr;
            if (v & mask) continue; // taken
            if (cas(ptr, v, v|mask)) break;
            else goto restart;
        }
        SET_THREAD_LOCAL(my_region, my_region);
    }
}

static void
release_data_bucket(const llmsset_t dbs, uint64_t index)
{
    uint64_t *ptr = dbs->bitmap2 + (index/64);
    uint64_t mask = 0x8000000000000000LL >> (index&63);
    *ptr &= ~mask;
}

static void
set_custom_bucket(const llmsset_t dbs, uint64_t index, int on)
{
    uint64_t *ptr = dbs->bitmapc + (index/64);
    uint64_t mask = 0x8000000000000000LL >> (index&63);
    if (on) *ptr |= mask;
    else *ptr &= ~mask;
}

static int
is_custom_bucket(const llmsset_t dbs, uint64_t index)
{
    uint64_t *ptr = dbs->bitmapc + (index/64);
    uint64_t mask = 0x8000000000000000LL >> (index&63);
    return (*ptr & mask) ? 1 : 0;
}

/**
 * This tricks the compiler into generating the bit-wise rotation instruction
 */
static uint64_t __attribute__((unused))
rotr64 (uint64_t n, unsigned int c)
{
    return (n >> c) | (n << (64-c));
}

/**
 * Pseudo-RNG for initializing the hashtab tables.
 * Implementation of xorshift128+ by Vigna 2016, which is
 * based on "Xorshift RNGs", Marsaglia 2003
 */
static uint64_t __attribute__((unused))
xor64(void)
{
    // For the initial state of s, we select two numbers:
    // - the initializer of Marsaglia's original xorshift
    // - the FNV-1a 64-bit offset basis
    static uint64_t s[2] = {88172645463325252LLU, 14695981039346656037LLU};

    uint64_t s1 = s[0];
    const uint64_t s0 = s[1];
    const uint64_t result = s0 + s1;
    s[0] = s0;
    s1 ^= s1 << 23; // a
    s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); // b, c
    return result;
}

/**
 * The table for tabulation hashing
 */
static uint64_t hashtab[256*16];

/**
 * Implementation of simple tabulation.
 * Proposed by e.g. Thorup 2017 "Fast and Powerful Hashing using Tabulation"
 */
uint64_t
llmsset_tabhash(uint64_t a, uint64_t b, uint64_t seed)
{
    // we use the seed as base
    uint64_t *t = hashtab;
    for (int i=0; i<8; i++) {
        seed ^= t[(uint8_t)a];
        t += 256; // next table
        a >>= 8;
    }
    for (int i=0; i<8; i++) {
        seed ^= t[(uint8_t)b];
        t += 256; // next table
        b >>= 8;
    }
    return seed;
}

/**
 * Encoding of the prime 2^89-1 for CWhash
 */
static const uint64_t Prime89_0 = (((uint64_t)1)<<32)-1;
static const uint64_t Prime89_1 = (((uint64_t)1)<<32)-1;
static const uint64_t Prime89_2 = (((uint64_t)1)<<25)-1;
static const uint64_t Prime89_21 = (((uint64_t)1)<<57)-1;

typedef uint64_t INT96[3];

/**
 * Computes (r mod Prime89) mod 2ˆ64
 * (for CWhash, implementation by Thorup et al.)
 */
static uint64_t
Mod64Prime89(INT96 r)
{
    uint64_t r0, r1, r2;
    r2 = r[2];
    r1 = r[1];
    r0 = r[0] + (r2>>25);
    r2 &= Prime89_2;
    return (r2 == Prime89_2 && r1 == Prime89_1 && r0 >= Prime89_0) ? (r0 - Prime89_0) : (r0 + (r1<<32));
}

/**
 * Computes a 96-bit r such that r = ax+b (mod Prime89)
 * (for CWhash, implementation by Thorup et al.)
 */
static void
MultAddPrime89(INT96 r, uint64_t x, const INT96 a, const INT96 b)
{
#define LOW(x) ((x)&0xFFFFFFFF)
#define HIGH(x) ((x)>>32)
    uint64_t x1, x0, c21, c20, c11, c10, c01, c00;
    uint64_t d0, d1, d2, d3;
    uint64_t s0, s1, carry;
    x1 = HIGH(x);
    x0 = LOW(x);
    c21 = a[2]*x1;
    c11 = a[1]*x1;
    c01 = a[0]*x1;
    c20 = a[2]*x0;
    c10 = a[1]*x0;
    c00 = a[0]*x0;
    d0 = (c20>>25)+(c11>>25)+(c10>>57)+(c01>>57);
    d1 = (c21<<7);
    d2 = (c10&Prime89_21) + (c01&Prime89_21);
    d3 = (c20&Prime89_2) + (c11&Prime89_2) + (c21>>57);
    s0 = b[0] + LOW(c00) + LOW(d0) + LOW(d1);
    r[0] = LOW(s0);
    carry = HIGH(s0);
    s1 = b[1] + HIGH(c00) + HIGH(d0) + HIGH(d1) + LOW(d2) + carry;
    r[1] = LOW(s1);
    carry = HIGH(s1);
    r[2] = b[2] + HIGH(d2) + d3 + carry;
#undef LOW
#undef HIGH
}

/**
 * Compute Carter/Wegman k-independent hash
 * Implementation by Thorup et al.
 * - compute polynomial on prime field of 2^89-1 (10th Marsenne prime)
 * - random coefficients from random.org
 */
static uint64_t
CWhash(uint64_t x)
{
    INT96 A = {0xcf90094b0ab9939e, 0x817f998697604ff3, 0x1a6e6f08b65440ea};
    INT96 B = {0xb989a05a5dcf57f1, 0x7c007611f28daee7, 0xd8bd809d68c26854};
    INT96 C = {0x1041070633a92679, 0xba9379fd71cd939d, 0x271793709e1cd781};
    INT96 D = {0x5c240a710b0c6beb, 0xc24ac3b68056ea1c, 0xd46c9c7f2adfaf71};
    INT96 E = {0xa527cea74b053a87, 0x69ba4a5e23f90577, 0x707b6e053c7741e7};
    INT96 F = {0xa6c0812cdbcdb982, 0x8cb0c8b73f701489, 0xee08c4dc1dbef243};
    INT96 G = {0xcf3ab0ec9d538853, 0x982a8457b6db03a9, 0x8659cf6b636c9d37};
    INT96 H = {0x905d5d14efefc0dd, 0x7e9870e018ead6a2, 0x47e2c9af0ea9325a};
    INT96 I = {0xc59351a9bf283b09, 0x4a39e35dbc280c7f, 0xc5f160732996be4f};
    INT96 J = {0x4d58e0b7a57ccddf, 0xc362a25c267d1db4, 0x7c79d2fcd89402b2};
    INT96 K = {0x62ac342c4393930c, 0xdb2fd2740ebef2a0, 0xc672fd5e72921377};
    INT96 L = {0xbdae267838862c6d, 0x0e0ee206fdbaf1d1, 0xc270e26fd8dfbae7};

    INT96 r;
    MultAddPrime89(r, x, A, B);
    MultAddPrime89(r, x, r, C);
    MultAddPrime89(r, x, r, D);
    MultAddPrime89(r, x, r, E);
    MultAddPrime89(r, x, r, F);
    MultAddPrime89(r, x, r, G);
    MultAddPrime89(r, x, r, H);
    MultAddPrime89(r, x, r, I);
    MultAddPrime89(r, x, r, J);
    MultAddPrime89(r, x, r, K);
    MultAddPrime89(r, x, r, L);
    return Mod64Prime89(r);
}

/**
 * The well-known FNV-1a hash for 64 bits.
 * Typical seed value (base offset) is 14695981039346656037LLU.
 *
 * NOTE: this particular hash is bad for certain nodes, resulting in
 * early garbage collection and failure. We xor with shifted hash which
 * suffices as a band-aid, but this is obviously not an ideal solution.
 */
uint64_t
llmsset_fnvhash(const uint64_t a, const uint64_t b, const uint64_t seed)
{
    // The FNV-1a hash for 64 bits
    const uint64_t prime = 1099511628211;
    uint64_t hash = seed;
    hash = (hash ^ a) * prime;
    hash = (hash ^ b) * prime;
    return hash ^ (hash>>32);
}

/*
 * CL_MASK and CL_MASK_R are for the probe sequence calculation.
 * With 64 bytes per cacheline, there are 8 64-bit values per cacheline.
 */
// The LINE_SIZE is defined in lace.h
static const uint64_t CL_MASK     = ~(((LINE_SIZE) / 8) - 1);
static const uint64_t CL_MASK_R   = ((LINE_SIZE) / 8) - 1;

/* 40 bits for the index, 24 bits for the hash */
#define MASK_INDEX ((uint64_t)0x000000ffffffffff)
#define MASK_HASH  ((uint64_t)0xffffff0000000000)

static inline uint64_t
llmsset_lookup2(const llmsset_t dbs, uint64_t a, uint64_t b, int* created, const int custom)
{
    uint64_t hash_rehash = 14695981039346656037LLU;
    if (custom) hash_rehash = dbs->hash_cb(a, b, hash_rehash);
    else hash_rehash = llmsset_hash(a, b, hash_rehash);

    const uint64_t step = (((hash_rehash >> 20) | 1) << 3);
    const uint64_t hash = hash_rehash & MASK_HASH;
    uint64_t idx, last, cidx = 0;
    int i=0;

#if LLMSSET_MASK
    last = idx = hash_rehash & dbs->mask;
#else
    last = idx = hash_rehash % dbs->table_size;
#endif

    for (;;) {
        volatile uint64_t *bucket = dbs->table + idx;
        uint64_t v = *bucket;

        if (v == 0) {
            if (cidx == 0) {
                // Claim data bucket and write data
                cidx = claim_data_bucket(dbs);
                if (cidx == (uint64_t)-1) return 0; // failed to claim a data bucket
                if (custom) dbs->create_cb(&a, &b);
                uint64_t *d_ptr = ((uint64_t*)dbs->data) + 2*cidx;
                d_ptr[0] = a;
                d_ptr[1] = b;
            }
            if (cas(bucket, 0, hash | cidx)) {
                if (custom) set_custom_bucket(dbs, cidx, custom);
                *created = 1;
                return cidx;
            } else {
                v = *bucket;
            }
        }

        if (hash == (v & MASK_HASH)) {
            uint64_t d_idx = v & MASK_INDEX;
            uint64_t *d_ptr = ((uint64_t*)dbs->data) + 2*d_idx;
            if (custom) {
                if (dbs->equals_cb(a, b, d_ptr[0], d_ptr[1])) {
                    if (cidx != 0) {
                        dbs->destroy_cb(a, b);
                        release_data_bucket(dbs, cidx);
                    }
                    *created = 0;
                    return d_idx;
                }
            } else {
                if (d_ptr[0] == a && d_ptr[1] == b) {
                    if (cidx != 0) release_data_bucket(dbs, cidx);
                    *created = 0;
                    return d_idx;
                }
            }
        }

        sylvan_stats_count(LLMSSET_LOOKUP);

        // find next idx on probe sequence
        idx = (idx & CL_MASK) | ((idx+1) & CL_MASK_R);
        if (idx == last) {
            if (++i == dbs->threshold) return 0; // failed to find empty spot in probe sequence

            // go to next cache line in probe sequence
            hash_rehash += step;

#if LLMSSET_MASK
            last = idx = hash_rehash & dbs->mask;
#else
            last = idx = hash_rehash % dbs->table_size;
#endif
        }
    }
}

uint64_t
llmsset_lookup(const llmsset_t dbs, const uint64_t a, const uint64_t b, int* created)
{
    return llmsset_lookup2(dbs, a, b, created, 0);
}

uint64_t
llmsset_lookupc(const llmsset_t dbs, const uint64_t a, const uint64_t b, int* created)
{
    return llmsset_lookup2(dbs, a, b, created, 1);
}

int
llmsset_rehash_bucket(const llmsset_t dbs, uint64_t d_idx)
{
    const uint64_t * const d_ptr = ((uint64_t*)dbs->data) + 2*d_idx;
    const uint64_t a = d_ptr[0];
    const uint64_t b = d_ptr[1];

    uint64_t hash_rehash = 14695981039346656037LLU;
    const int custom = is_custom_bucket(dbs, d_idx) ? 1 : 0;
    if (custom) hash_rehash = dbs->hash_cb(a, b, hash_rehash);
    else hash_rehash = llmsset_hash(a, b, hash_rehash);
    const uint64_t step = (((hash_rehash >> 20) | 1) << 3);
    const uint64_t new_v = (hash_rehash & MASK_HASH) | d_idx;
    int i=0;

    uint64_t idx, last;
#if LLMSSET_MASK
    last = idx = hash_rehash & dbs->mask;
#else
    last = idx = hash_rehash % dbs->table_size;
#endif

    for (;;) {
        volatile uint64_t *bucket = &dbs->table[idx];
        if (*bucket == 0 && cas(bucket, 0, new_v)) return 1;

        // find next idx on probe sequence
        idx = (idx & CL_MASK) | ((idx+1) & CL_MASK_R);
        if (idx == last) {
            if (++i == *(volatile int16_t*)&dbs->threshold) {
                // failed to find empty spot in probe sequence
                // solution: increase probe sequence length...
                __sync_fetch_and_add(&dbs->threshold, 1);
            }

            // go to next cache line in probe sequence
            hash_rehash += step;

#if LLMSSET_MASK
            last = idx = hash_rehash & dbs->mask;
#else
            last = idx = hash_rehash % dbs->table_size;
#endif
        }
    }
}

llmsset_t
llmsset_create(size_t initial_size, size_t max_size)
{
    llmsset_t dbs = NULL;
    if (posix_memalign((void**)&dbs, LINE_SIZE, sizeof(struct llmsset)) != 0) {
        fprintf(stderr, "llmsset_create: Unable to allocate memory!\n");
        exit(1);
    }

#if LLMSSET_MASK
    /* Check if initial_size and max_size are powers of 2 */
    if (__builtin_popcountll(initial_size) != 1) {
        fprintf(stderr, "llmsset_create: initial_size is not a power of 2!\n");
        exit(1);
    }

    if (__builtin_popcountll(max_size) != 1) {
        fprintf(stderr, "llmsset_create: max_size is not a power of 2!\n");
        exit(1);
    }
#endif

    if (initial_size > max_size) {
        fprintf(stderr, "llmsset_create: initial_size > max_size!\n");
        exit(1);
    }

    // minimum size is now 512 buckets (region size, but of course, n_workers * 512 is suggested as minimum)

    if (initial_size < 512) {
        fprintf(stderr, "llmsset_create: initial_size too small!\n");
        exit(1);
    }

    dbs->max_size = max_size;
    llmsset_set_size(dbs, initial_size);

    /* This implementation of "resizable hash table" allocates the max_size table in virtual memory,
       but only uses the "actual size" part in real memory */

    dbs->table = (uint64_t*)mmap(0, dbs->max_size * 8, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    dbs->data = (uint8_t*)mmap(0, dbs->max_size * 16, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    /* Also allocate bitmaps. Each region is 64*8 = 512 buckets.
       Overhead of bitmap1: 1 bit per 4096 bucket.
       Overhead of bitmap2: 1 bit per bucket.
       Overhead of bitmapc: 1 bit per bucket. */

    dbs->bitmap1 = (uint64_t*)mmap(0, dbs->max_size / (512*8), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    dbs->bitmap2 = (uint64_t*)mmap(0, dbs->max_size / 8, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    dbs->bitmapc = (uint64_t*)mmap(0, dbs->max_size / 8, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (dbs->table == (uint64_t*)-1 || dbs->data == (uint8_t*)-1 || dbs->bitmap1 == (uint64_t*)-1 || dbs->bitmap2 == (uint64_t*)-1 || dbs->bitmapc == (uint64_t*)-1) {
        fprintf(stderr, "llmsset_create: Unable to allocate memory: %s!\n", strerror(errno));
        exit(1);
    }

#if defined(madvise) && defined(MADV_RANDOM)
    madvise(dbs->table, dbs->max_size * 8, MADV_RANDOM);
#endif

    // forbid first two positions (index 0 and 1)
    dbs->bitmap2[0] = 0xc000000000000000LL;

    dbs->hash_cb = NULL;
    dbs->equals_cb = NULL;
    dbs->create_cb = NULL;
    dbs->destroy_cb = NULL;

    // yes, ugly. for now, we use a global thread-local value.
    // that is a problem with multiple tables.
    // so, for now, do NOT use multiple tables!!

    LACE_ME;
    INIT_THREAD_LOCAL(my_region);
    TOGETHER(llmsset_reset_region);

    // initialize hashtab
    for (int i=0; i<256*16; i++) hashtab[i] = CWhash(i);

    return dbs;
}

void
llmsset_free(llmsset_t dbs)
{
    munmap(dbs->table, dbs->max_size * 8);
    munmap(dbs->data, dbs->max_size * 16);
    munmap(dbs->bitmap1, dbs->max_size / (512*8));
    munmap(dbs->bitmap2, dbs->max_size / 8);
    munmap(dbs->bitmapc, dbs->max_size / 8);
    free(dbs);
}

VOID_TASK_IMPL_1(llmsset_clear, llmsset_t, dbs)
{
    CALL(llmsset_clear_data, dbs);
    CALL(llmsset_clear_hashes, dbs);
}

VOID_TASK_IMPL_1(llmsset_clear_data, llmsset_t, dbs)
{
    if (mmap(dbs->bitmap1, dbs->max_size / (512*8), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0) != (void*)-1) {
    } else {
        memset(dbs->bitmap1, 0, dbs->max_size / (512*8));
    }

    if (mmap(dbs->bitmap2, dbs->max_size / 8, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0) != (void*)-1) {
    } else {
        memset(dbs->bitmap2, 0, dbs->max_size / 8);
    }

    // forbid first two positions (index 0 and 1)
    dbs->bitmap2[0] = 0xc000000000000000LL;

    TOGETHER(llmsset_reset_region);
}

VOID_TASK_IMPL_1(llmsset_clear_hashes, llmsset_t, dbs)
{
    // just reallocate...
    if (mmap(dbs->table, dbs->max_size * 8, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0) != (void*)-1) {
#if defined(madvise) && defined(MADV_RANDOM)
        madvise(dbs->table, sizeof(uint64_t[dbs->max_size]), MADV_RANDOM);
#endif
    } else {
        // reallocate failed... expensive fallback
        memset(dbs->table, 0, dbs->max_size * 8);
    }
}

int
llmsset_is_marked(const llmsset_t dbs, uint64_t index)
{
    volatile uint64_t *ptr = dbs->bitmap2 + (index/64);
    uint64_t mask = 0x8000000000000000LL >> (index&63);
    return (*ptr & mask) ? 1 : 0;
}

int
llmsset_mark(const llmsset_t dbs, uint64_t index)
{
    volatile uint64_t *ptr = dbs->bitmap2 + (index/64);
    uint64_t mask = 0x8000000000000000LL >> (index&63);
    for (;;) {
        uint64_t v = *ptr;
        if (v & mask) return 0;
        if (cas(ptr, v, v|mask)) return 1;
    }
}

TASK_3(int, llmsset_rehash_par, llmsset_t, dbs, size_t, first, size_t, count)
{
    if (count > 512) {
        SPAWN(llmsset_rehash_par, dbs, first, count/2);
        int bad = CALL(llmsset_rehash_par, dbs, first + count/2, count - count/2);
        return bad + SYNC(llmsset_rehash_par);
    } else {
        int bad = 0;
        uint64_t *ptr = dbs->bitmap2 + (first / 64);
        uint64_t mask = 0x8000000000000000LL >> (first & 63);
        for (size_t k=0; k<count; k++) {
            if (*ptr & mask) {
                if (llmsset_rehash_bucket(dbs, first+k) == 0) bad++;
            }
            mask >>= 1;
            if (mask == 0) {
                ptr++;
                mask = 0x8000000000000000LL;
            }
        }
        return bad;
    }
}

TASK_IMPL_1(int, llmsset_rehash, llmsset_t, dbs)
{
    return CALL(llmsset_rehash_par, dbs, 0, dbs->table_size);
}

TASK_3(size_t, llmsset_count_marked_par, llmsset_t, dbs, size_t, first, size_t, count)
{
    if (count > 512) {
        size_t split = count/2;
        SPAWN(llmsset_count_marked_par, dbs, first, split);
        size_t right = CALL(llmsset_count_marked_par, dbs, first + split, count - split);
        size_t left = SYNC(llmsset_count_marked_par);
        return left + right;
    } else {
        size_t result = 0;
        uint64_t *ptr = dbs->bitmap2 + (first / 64);
        if (count == 512) {
            result += __builtin_popcountll(ptr[0]);
            result += __builtin_popcountll(ptr[1]);
            result += __builtin_popcountll(ptr[2]);
            result += __builtin_popcountll(ptr[3]);
            result += __builtin_popcountll(ptr[4]);
            result += __builtin_popcountll(ptr[5]);
            result += __builtin_popcountll(ptr[6]);
            result += __builtin_popcountll(ptr[7]);
        } else {
            uint64_t mask = 0x8000000000000000LL >> (first & 63);
            for (size_t k=0; k<count; k++) {
                if (*ptr & mask) result += 1;
                mask >>= 1;
                if (mask == 0) {
                    ptr++;
                    mask = 0x8000000000000000LL;
                }
            }
        }
        return result;
    }
}

TASK_IMPL_1(size_t, llmsset_count_marked, llmsset_t, dbs)
{
    return CALL(llmsset_count_marked_par, dbs, 0, dbs->table_size);
}

VOID_TASK_3(llmsset_destroy_par, llmsset_t, dbs, size_t, first, size_t, count)
{
    if (count > 1024) {
        size_t split = count/2;
        SPAWN(llmsset_destroy_par, dbs, first, split);
        CALL(llmsset_destroy_par, dbs, first + split, count - split);
        SYNC(llmsset_destroy_par);
    } else {
        for (size_t k=first; k<first+count; k++) {
            volatile uint64_t *ptr2 = dbs->bitmap2 + (k/64);
            volatile uint64_t *ptrc = dbs->bitmapc + (k/64);
            uint64_t mask = 0x8000000000000000LL >> (k&63);

            // if not marked but is custom
            if ((*ptr2 & mask) == 0 && (*ptrc & mask)) {
                uint64_t *d_ptr = ((uint64_t*)dbs->data) + 2*k;
                dbs->destroy_cb(d_ptr[0], d_ptr[1]);
                *ptrc &= ~mask;
            }
        }
    }
}

VOID_TASK_IMPL_1(llmsset_destroy_unmarked, llmsset_t, dbs)
{
    if (dbs->destroy_cb == NULL) return; // no custom function
    CALL(llmsset_destroy_par, dbs, 0, dbs->table_size);
}

/**
 * Set custom functions
 */
void llmsset_set_custom(const llmsset_t dbs, llmsset_hash_cb hash_cb, llmsset_equals_cb equals_cb, llmsset_create_cb create_cb, llmsset_destroy_cb destroy_cb)
{
    dbs->hash_cb = hash_cb;
    dbs->equals_cb = equals_cb;
    dbs->create_cb = create_cb;
    dbs->destroy_cb = destroy_cb;
}
