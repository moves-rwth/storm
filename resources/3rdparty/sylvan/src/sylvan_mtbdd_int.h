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
 * Internals for MTBDDs
 */

#ifndef SYLVAN_MTBDD_INT_H
#define SYLVAN_MTBDD_INT_H

/**
 * MTBDD node structure
 */
typedef struct __attribute__((packed)) mtbddnode {
    uint64_t a, b;
} * mtbddnode_t; // 16 bytes

#define GETNODE(mtbdd) ((mtbddnode_t)llmsset_index_to_ptr(nodes, mtbdd&0x000000ffffffffff))

/**
 * Complement handling macros
 */
#define MTBDD_HASMARK(s)              (s&mtbdd_complement?1:0)
#define MTBDD_TOGGLEMARK(s)           (s^mtbdd_complement)
#define MTBDD_STRIPMARK(s)            (s&~mtbdd_complement)
#define MTBDD_TRANSFERMARK(from, to)  (to ^ (from & mtbdd_complement))
// Equal under mark
#define MTBDD_EQUALM(a, b)            ((((a)^(b))&(~mtbdd_complement))==0)

// Leaf: a = L=1, M, type; b = value
// Node: a = L=0, C, M, high; b = variable, low
// Only complement edge on "high"

static inline int
mtbddnode_isleaf(mtbddnode_t n)
{
    return n->a & 0x4000000000000000 ? 1 : 0;
}

static inline uint32_t
mtbddnode_gettype(mtbddnode_t n)
{
    return n->a & 0x00000000ffffffff;
}

static inline uint64_t
mtbddnode_getvalue(mtbddnode_t n)
{
    return n->b;
}

static inline int
mtbddnode_getcomp(mtbddnode_t n)
{
    return n->a & 0x8000000000000000 ? 1 : 0;
}

static inline uint64_t
mtbddnode_getlow(mtbddnode_t n)
{
    return n->b & 0x000000ffffffffff; // 40 bits
}

static inline uint64_t
mtbddnode_gethigh(mtbddnode_t n)
{
    return n->a & 0x800000ffffffffff; // 40 bits plus high bit of first
}

static inline uint32_t
mtbddnode_getvariable(mtbddnode_t n)
{
    return (uint32_t)(n->b >> 40);
}

static inline int
mtbddnode_getmark(mtbddnode_t n)
{
    return n->a & 0x2000000000000000 ? 1 : 0;
}

static inline void
mtbddnode_setmark(mtbddnode_t n, int mark)
{
    if (mark) n->a |= 0x2000000000000000;
    else n->a &= 0xdfffffffffffffff;
}

static inline void
mtbddnode_makeleaf(mtbddnode_t n, uint32_t type, uint64_t value)
{
    n->a = 0x4000000000000000 | (uint64_t)type;
    n->b = value;
}

static inline void
mtbddnode_makenode(mtbddnode_t n, uint32_t var, uint64_t low, uint64_t high)
{
    n->a = high;
    n->b = ((uint64_t)var)<<40 | low;
}

static MTBDD
node_getlow(MTBDD mtbdd, mtbddnode_t node)
{
    return MTBDD_TRANSFERMARK(mtbdd, mtbddnode_getlow(node));
}

static MTBDD
node_gethigh(MTBDD mtbdd, mtbddnode_t node)
{
    return MTBDD_TRANSFERMARK(mtbdd, mtbddnode_gethigh(node));
}

#endif
