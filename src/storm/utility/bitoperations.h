#pragma once
#include<cstddef>
#include "storm/utility/macros.h"

inline size_t smallestIntWithNBitsSet(size_t n) {
    static_assert(sizeof(size_t) == 8, "size_t has wrong size.");
    STORM_LOG_ASSERT(n < 64, "Input is too large."); // TODO fix this for 32 bit architectures!
    if(n==0) return 0;
    return (1 << n) - 1;
}

inline size_t nextBitPermutation(size_t v) {
    if (v==0) return static_cast<size_t>(0);
    // From https://graphics.stanford.edu/~seander/bithacks.html#NextBitPermutation
    size_t t = (v | (v - 1)) + 1;
    return t | ((((t & -t) / (v & -v)) >> 1) - 1);
}
