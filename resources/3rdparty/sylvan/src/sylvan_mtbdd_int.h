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

/**
 * Internals for MTBDDs
 */

#ifndef SYLVAN_MTBDD_INT_H
#define SYLVAN_MTBDD_INT_H

/**
 * BDD/MTBDD node structure
 */
typedef struct __attribute__((packed)) mtbddnode {
    uint64_t a, b;
} * mtbddnode_t; // 16 bytes

static inline mtbddnode_t
MTBDD_GETNODE(MTBDD dd)
{
    return (mtbddnode_t)llmsset_index_to_ptr(nodes, dd&0x000000ffffffffff);
}

/**
 * Complement handling macros
 */

static inline int
MTBDD_HASMARK(MTBDD dd)
{
    return (dd & mtbdd_complement) ? 1 : 0;
}

static inline MTBDD
MTBDD_TOGGLEMARK(MTBDD dd)
{
    return dd ^ mtbdd_complement;
}

static inline MTBDD
MTBDD_STRIPMARK(MTBDD dd)
{
    return dd & (~mtbdd_complement);
}

static inline MTBDD
MTBDD_TRANSFERMARK(MTBDD from, MTBDD to)
{
    return (to ^ (from & mtbdd_complement));
}

/**
 * Are two MTBDDs equal modulo mark?
 */
static inline int
MTBDD_EQUALM(MTBDD a, MTBDD b)
{
    return ((a^b)&(~mtbdd_complement)) ? 0 : 1;
}

// Leaf: a = L=1, M, type; b = value
// Node: a = L=0, C, M, high; b = variable, low
// Only complement edge on "high"

static inline int __attribute__((unused))
mtbddnode_isleaf(mtbddnode_t n)
{
    return n->a & 0x4000000000000000 ? 1 : 0;
}

static inline uint32_t __attribute__((unused))
mtbddnode_gettype(mtbddnode_t n)
{
    return n->a & 0x00000000ffffffff;
}

static inline uint64_t __attribute__((unused))
mtbddnode_getvalue(mtbddnode_t n)
{
    return n->b;
}

static inline int __attribute__((unused))
mtbddnode_getcomp(mtbddnode_t n)
{
    return n->a & 0x8000000000000000 ? 1 : 0;
}

static inline uint64_t __attribute__((unused))
mtbddnode_getlow(mtbddnode_t n)
{
    return n->b & 0x000000ffffffffff; // 40 bits
}

static inline uint64_t __attribute__((unused))
mtbddnode_gethigh(mtbddnode_t n)
{
    return n->a & 0x800000ffffffffff; // 40 bits plus high bit of first
}

static inline uint32_t __attribute__((unused))
mtbddnode_getvariable(mtbddnode_t n)
{
    return (uint32_t)(n->b >> 40);
}

static inline int __attribute__((unused))
mtbddnode_getmark(mtbddnode_t n)
{
    return n->a & 0x2000000000000000 ? 1 : 0;
}

static inline void __attribute__((unused))
mtbddnode_setmark(mtbddnode_t n, int mark)
{
    if (mark) n->a |= 0x2000000000000000;
    else n->a &= 0xdfffffffffffffff;
}

static inline void __attribute__((unused))
mtbddnode_makeleaf(mtbddnode_t n, uint32_t type, uint64_t value)
{
    n->a = 0x4000000000000000 | (uint64_t)type;
    n->b = value;
}

static inline void __attribute__((unused))
mtbddnode_makenode(mtbddnode_t n, uint32_t var, uint64_t low, uint64_t high)
{
    n->a = high;
    n->b = ((uint64_t)var)<<40 | low;
}

static inline void __attribute__((unused))
mtbddnode_makemapnode(mtbddnode_t n, uint32_t var, uint64_t low, uint64_t high)
{
    n->a = high | 0x1000000000000000;
    n->b = ((uint64_t)var)<<40 | low;
}

static inline int __attribute__((unused))
mtbddnode_ismapnode(mtbddnode_t n)
{
    return n->a & 0x1000000000000000 ? 1 : 0;
}

static MTBDD __attribute__((unused))
mtbddnode_followlow(MTBDD mtbdd, mtbddnode_t node)
{
    return MTBDD_TRANSFERMARK(mtbdd, mtbddnode_getlow(node));
}

static MTBDD __attribute__((unused))
mtbddnode_followhigh(MTBDD mtbdd, mtbddnode_t node)
{
    return MTBDD_TRANSFERMARK(mtbdd, mtbddnode_gethigh(node));
}

/**
 * Compatibility
 */

#define node_getlow mtbddnode_followlow
#define node_gethigh mtbddnode_followhigh

#define BDD_HASMARK MTBDD_HASMARK
#define BDD_TOGGLEMARK MTBDD_TOGGLEMARK
#define BDD_STRIPMARK MTBDD_STRIPMARK
#define BDD_TRANSFERMARK MTBDD_TRANSFERMARK
#define BDD_EQUALM MTBDD_EQUALM
#define bddnode mtbddnode
#define bddnode_t mtbddnode_t
#define bddnode_getcomp mtbddnode_getcomp
#define bddnode_getlow mtbddnode_getlow
#define bddnode_gethigh mtbddnode_gethigh
#define bddnode_getvariable mtbddnode_getvariable
#define bddnode_getmark mtbddnode_getmark
#define bddnode_setmark mtbddnode_setmark
#define bddnode_makenode mtbddnode_makenode
#define bddnode_makemapnode mtbddnode_makemapnode
#define bddnode_ismapnode mtbddnode_ismapnode
#define node_low node_getlow
#define node_high node_gethigh

#endif
