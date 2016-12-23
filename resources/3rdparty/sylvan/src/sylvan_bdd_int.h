/*
 * Copyright 2011-2015 Formal Methods and Tools, University of Twente
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
 * Internals for BDDs
 */
 
#ifndef SYLVAN_BDD_INT_H
#define SYLVAN_BDD_INT_H

/**
 * Complement handling macros
 */
#define BDD_HASMARK(s)              (s&sylvan_complement?1:0)
#define BDD_TOGGLEMARK(s)           (s^sylvan_complement)
#define BDD_STRIPMARK(s)            (s&~sylvan_complement)
#define BDD_TRANSFERMARK(from, to)  (to ^ (from & sylvan_complement))
// Equal under mark
#define BDD_EQUALM(a, b)            ((((a)^(b))&(~sylvan_complement))==0)

/**
 * BDD node structure
 */
typedef struct __attribute__((packed)) bddnode {
    uint64_t a, b;
} * bddnode_t; // 16 bytes

#define BDD_GETNODE(bdd) ((bddnode_t)llmsset_index_to_ptr(nodes, bdd&0x000000ffffffffff))

static inline int __attribute__((unused))
bddnode_getcomp(bddnode_t n)
{
    return n->a & 0x8000000000000000 ? 1 : 0;
}

static inline uint64_t
bddnode_getlow(bddnode_t n)
{
    return n->b & 0x000000ffffffffff; // 40 bits
}

static inline uint64_t
bddnode_gethigh(bddnode_t n)
{
    return n->a & 0x800000ffffffffff; // 40 bits plus high bit of first
}

static inline uint32_t
bddnode_getvariable(bddnode_t n)
{
    return (uint32_t)(n->b >> 40);
}

static inline int
bddnode_getmark(bddnode_t n)
{
    return n->a & 0x2000000000000000 ? 1 : 0;
}

static inline void
bddnode_setmark(bddnode_t n, int mark)
{
    if (mark) n->a |= 0x2000000000000000;
    else n->a &= 0xdfffffffffffffff;
}

static inline void
bddnode_makenode(bddnode_t n, uint32_t var, uint64_t low, uint64_t high)
{
    n->a = high;
    n->b = ((uint64_t)var)<<40 | low;
}

#endif
