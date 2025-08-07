/*
 * Copyright 2019 Tom van Dijk, Formal Methods and Tools, University of Twente
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

#include <stdint.h>

#ifndef SYLVAN_HASH_H
#define SYLVAN_HASH_H

#ifdef __cplusplus
namespace sylvan {
extern "C" {
#endif /* __cplusplus */

extern uint64_t sylvan_tabhash_table[256*16];

/**
 * Implementation of simple tabulation hashing.
 * Proposed by e.g. Thorup 2017 "Fast and Powerful Hashing using Tabulation"
 */
static inline uint64_t
sylvan_tabhash16(uint64_t a, uint64_t b, uint64_t seed)
{
    uint64_t *t = sylvan_tabhash_table;
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
 * The well-known FNV-1a hash for 64 bits.
 * Typical seed value (base offset) is 14695981039346656037LLU.
 *
 * NOTE: this particular hash is bad for certain nodes, resulting in
 * early garbage collection and failure. We xor with shifted hash which
 * suffices as a band-aid, but this is obviously not an ideal solution.
 */

static inline uint64_t
sylvan_fnvhash16(uint64_t a, uint64_t b, uint64_t seed)
{
    // The FNV-1a hash for 64 bits
    const uint64_t prime = 1099511628211;
    uint64_t hash = seed;
    hash = (hash ^ a) * prime;
    hash = (hash ^ b) * prime;
    return hash ^ (hash >> 32);
}

static inline uint64_t
sylvan_fnvhash8(uint64_t a, uint64_t seed)
{
    const uint64_t prime = 1099511628211;
    uint64_t hash = seed;
    hash = (hash ^ a) * prime;
    hash = (hash ^ ((a << 25) | (a >> 39))) * prime;
    return hash ^ (hash >> 32);
}

/**
 * Called by Sylvan's hash table initializer to initialize the tables for
 * tabulation hashing.
 */
void sylvan_init_hash(void);

#ifdef __cplusplus
}
}
#endif /* __cplusplus */

#endif
