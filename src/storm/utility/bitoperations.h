#pragma once
#include <cstdint>

#include "storm/utility/macros.h"

/**
 * \return 2^n - 1
 */
inline uint64_t smallestIntWithNBitsSet(uint64_t n) {
    STORM_LOG_ASSERT(n < 64, "Input is too large.");
    if (n == 0)
        return 0;
    return (1ul << n) - 1;
}

/**
 * The next bit permutation in a lexicographical sense.
 *
 * Example: 00010011, 00010101, 00010110, 00011001,
 * 00011010, 00011100, 00100011, and so forth
 *
 * From https://graphics.stanford.edu/~seander/bithacks.html#NextBitPermutation
 */
inline uint64_t nextBitPermutation(uint64_t v) {
    if (v == 0)
        return 0;
    uint64_t t = (v | (v - 1)) + 1;
    return t | ((((t & -t) / (v & -v)) >> 1) - 1);
}
