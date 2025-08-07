/*
 * Copyright 2016 Tom van Dijk, Johannes Kepler University Linz
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
 * Internals for multi-terminal ZDDs
 *
 * Bits
 * 127        1 complement      complement bit of the high edge
 * 126        1 custom leaf     this is a custom leaf node
 * 125        1 map             is a MAP node, for compose etc
 * 124        1 mark            used for node marking
 * 123..104  20 unused          unused space
 * 103..64   40 true index      index of the true edge
 *  63..40   24 variable        variable of this node
 *  39..0    40 false index     index of the false edge
 *
 * Lil endian: TTTT TTTT TT** **x* | FFFF FFFF FFVV VVVV
 * Big endian: x*** **TT TTTT TTTT | VVVV VVFF FFFF FFFF
 *
 * Leaf nodes:
 * 127        1 unused          set to 0
 * 126        1 custom leaf     set to 1
 * 125        1 unused          set to 0
 * 124        1 mark            used for node marking
 * 123..80   46 unused          set to 0
 *  79..64   16 type            the type of the leaf
 *  63..0    64 value           the value of the leaf
 *
 * Lil endian: x*** **** **** TTTT | VVVV VVVV VVVV VVVV (big endian)
 */

#ifndef SYLVAN_ZDD_INT_H
#define SYLVAN_ZDD_INT_H

/**
 * ZDD node structure
 */

typedef struct __attribute__((packed)) zddnode {
    uint64_t a, b;
} * zddnode_t; // 16 bytes

/**
 * Some inlines to work with the ZDD type and the complement marks
 */

static inline uint64_t
ZDD_GETINDEX(ZDD dd)
{
    return dd & 0x000000ffffffffff;
}

static inline ZDD
ZDD_SETINDEX(ZDD dd, uint64_t idx)
{
    return ((dd & 0xffffff0000000000) | (idx & 0x000000ffffffffff));
}

static inline zddnode_t
ZDD_GETNODE(ZDD dd)
{
    return (zddnode_t)llmsset_index_to_ptr(nodes, dd & 0x000000ffffffffff);
}

static inline int
ZDD_HASMARK(ZDD dd)
{
    return (dd & zdd_complement) ? 1 : 0;
}

static inline ZDD
ZDD_TOGGLEMARK(ZDD dd)
{
    return dd ^ zdd_complement;
}

static inline ZDD
ZDD_STRIPMARK(ZDD dd)
{
    return dd & (~zdd_complement);
}

static inline ZDD
ZDD_TRANSFERMARK(ZDD from, ZDD to)
{
    return (to ^ (from & zdd_complement));
}

/**
 * Are two ZDDs equal modulo the complement mark?
 */
static inline int
ZDD_EQUALM(ZDD a, ZDD b)
{
    return ((a^b)&(~zdd_complement)) ? 0 : 1;
}

/**
 * Whether a node is a leaf node
 */
static inline int __attribute__((unused))
zddnode_isleaf(zddnode_t n)
{
    return n->a & 0x4000000000000000 ? 1 : 0;
}

/**
 * For leaf nodes, get the type of the leaf
 */
static inline uint16_t __attribute__((unused))
zddnode_gettype(zddnode_t n)
{
    return (uint16_t)(n->a);
}

/**
 * For leaf nodes, get the value of the leaf
 */
static inline uint64_t __attribute__((unused))
zddnode_getvalue(zddnode_t n)
{
    return n->b;
}

/**
 * For internal nodes, get the complement on the high edge
 */
static inline int __attribute__((unused))
zddnode_getcomp(zddnode_t n)
{
    return n->a & 0x8000000000000800 ? 1 : 0;
}

/**
 * For internal nodes, get the low edge
 */
static inline uint64_t __attribute__((unused))
zddnode_getlow(zddnode_t n)
{
    return n->b & 0x000000ffffffffff;
}

/**
 * For internal nodes, get the high edge
 */
static inline uint64_t __attribute__((unused))
zddnode_gethigh(zddnode_t n)
{
    return n->a & 0x800000ffffffffff;
}

/**
 * For internal nodes, get the DD variable
 */
static inline uint32_t __attribute__((unused))
zddnode_getvariable(zddnode_t n)
{
    return (uint32_t)(n->b >> 40);
}

/**
 * Get whether the node is currently marked
 */
static inline int __attribute__((unused))
zddnode_getmark(zddnode_t n)
{
    return n->a & 0x1000000000000000 ? 1 : 0;
}

/**
 * Set or reset the mark on the node
 */
static inline void __attribute__((unused))
zddnode_setmark(zddnode_t n, int mark)
{
    if (mark) n->a |= 0x1000000000000000;
    else n->a &= 0xefffffffffffffff;
}

/**
 * Initialize a zddnode_t struct as a leaf node
 */
static inline void __attribute__((unused))
zddnode_makeleaf(zddnode_t n, uint16_t type, uint64_t value)
{
    n->a = 0x4000000000000000 | (uint64_t)type;
    n->b = value;
}

/**
 * Initialize a zddnode_t struct as an internal ZDD node
 */
static inline void __attribute__((unused))
zddnode_makenode(zddnode_t n, uint32_t var, uint64_t low, uint64_t high)
{
    n->a = high;
    n->b = ((uint64_t)var)<<40 | low;
}

/**
 * Initialize a zddnode_t struct as a "map" node
 */
static inline void __attribute__((unused))
zddnode_makemapnode(zddnode_t n, uint32_t var, uint64_t low, uint64_t high)
{
    n->a = high | 0x2000000000000000;
    n->b = ((uint64_t)var)<<40 | low;
}

/**
 * Whether a node is a "map" node
 */
static inline int __attribute__((unused))
zddnode_ismapnode(zddnode_t n)
{
    return n->a & 0x2000000000000000 ? 1 : 0;
}

/**
 * Return the low edge of a ZDD, taking into account the complement on the ZDD
 */
static ZDD __attribute__((unused))
zddnode_low(ZDD zdd, zddnode_t node)
{
    return ZDD_TRANSFERMARK(zdd, zddnode_getlow(node));
}

/**
 * Return the high edge of a ZDD, taking into account the complement on the ZDD
 */
static ZDD __attribute__((unused))
zddnode_high(ZDD zdd, zddnode_t node)
{
    return zddnode_gethigh(node);
    (void)zdd;
}

#endif
