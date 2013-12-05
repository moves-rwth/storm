#include "gtest/gtest.h"
#include "src/storage/BitVector.h"

TEST(BitVectorTest, IterationTest) {
    storm::storage::BitVector vector(819200, true);
    
    for (uint_fast64_t i = 0; i < 10000; ++i) {
        for (auto bit : vector) {
            // The following can never be true, but prevents the compiler from optimizing away the loop.
            if (bit == 819200) {
                ASSERT_TRUE(false);
            }
        }
    }
}