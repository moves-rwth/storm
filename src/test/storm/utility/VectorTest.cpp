#include "storm-config.h"
#include "storm/storage/BitVector.h"
#include "storm/utility/permutation.h"
#include "storm/utility/vector.h"
#include "test/storm_gtest.h"

TEST(VectorTest, sum_if) {
    std::vector<double> a = {1.0, 2.0, 4.0, 8.0, 16.0};
    storm::storage::BitVector f1(5, {2, 4});
    storm::storage::BitVector f2(5, {3, 4});

    ASSERT_EQ(20.0, storm::utility::vector::sum_if(a, f1));
    ASSERT_EQ(24.0, storm::utility::vector::sum_if(a, f2));
}

TEST(VectorTest, max_if) {
    std::vector<double> a = {1.0, 2.0, 34.0, 8.0, 16.0};
    storm::storage::BitVector f1(5, {2, 4});
    storm::storage::BitVector f2(5, {3, 4});

    ASSERT_EQ(34.0, storm::utility::vector::max_if(a, f1));
    ASSERT_EQ(16.0, storm::utility::vector::max_if(a, f2));
}

TEST(VectorTest, min_if) {
    std::vector<double> a = {1.0, 2.0, 34.0, 8.0, 16.0};
    storm::storage::BitVector f1(5, {2, 4});
    storm::storage::BitVector f2(5, {3, 4});

    ASSERT_EQ(16.0, storm::utility::vector::min_if(a, f1));
    ASSERT_EQ(8.0, storm::utility::vector::min_if(a, f2));
}

TEST(VectorTest, inverse_permute) {
    std::vector<double> a = {1.0, 2.0, 3.0, 4.0};
    std::vector<uint64_t> inversePermutation = {0, 3, 1, 2};
    std::vector<double> aperm = storm::utility::vector::applyInversePermutation(inversePermutation, a);
    EXPECT_EQ(aperm[0], a[0]);
    EXPECT_EQ(aperm[1], a[3]);
    EXPECT_EQ(aperm[2], a[1]);
    EXPECT_EQ(aperm[3], a[2]);
}

TEST(VectorTest, grouped_inverse_permute) {
    std::vector<double> a = {1.0, 2.0, 3.0, 4.0, 5.0};
    std::vector<uint64_t> groupIndices = {0, 3, 5, 5};  // last group empty
    std::vector<uint64_t> inversePermutation = {1, 2, 0};
    std::vector<double> aperm = storm::utility::vector::applyInversePermutationToGroupedVector(inversePermutation, a, groupIndices);
    std::vector<double> expected = {4.0, 5.0, 1.0, 2.0, 3.0};
    EXPECT_EQ(aperm, expected);
}