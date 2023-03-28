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

#include <sylvan_hash.h>

/**
 * This tricks the compiler into generating the bit-wise rotation instruction
 */
static uint64_t __attribute__((unused))
rotr64(uint64_t n, unsigned int c)
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
uint64_t sylvan_tabhash_table[256*16];

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

void
sylvan_init_hash(void)
{
    // initialize sylvan_tabhash_table
    for (int i=0; i<256*16; i++) sylvan_tabhash_table[i] = CWhash(i);
}
