#include "storm-config.h"
#include "test/storm_gtest.h"

#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <vector>

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/environment/Environment.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/transformer/zeroWeight/ZeroWeightAnalysis.h"
#include "storm/transformer/zeroWeight/ZeroWeightComponentLp.h"
#include "storm/transformer/zeroWeight/ZeroWeightExitLawEnumerator.h"
#include "storm/utility/constants.h"

namespace {

template<typename ValueType>
struct ExitModel {
    storm::storage::SparseMatrix<ValueType> matrix;
    std::vector<ValueType> weights;
    storm::storage::BitVector targets;
    storm::storage::BitVector initials;
};

template<typename ValueType>
ExitModel<ValueType> buildExitModel() {
    auto const one = storm::utility::one<ValueType>();
    auto const zero = storm::utility::zero<ValueType>();
    auto fraction = [](uint64_t numerator, uint64_t denominator) -> ValueType {
        return storm::utility::convertNumber<ValueType>(numerator) / storm::utility::convertNumber<ValueType>(denominator);
    };
    storm::storage::SparseMatrixBuilder<ValueType> builder(13, 7, 25, true, true, 7);
    builder.newRowGroup(0);
    builder.addNextValue(0, 1, fraction(1, 3));
    builder.addNextValue(0, 2, fraction(1, 3));
    builder.addNextValue(0, 3, fraction(1, 3));
    builder.newRowGroup(1);
    builder.addNextValue(1, 5, fraction(2, 5));
    builder.addNextValue(1, 6, fraction(3, 5));
    builder.addNextValue(2, 4, fraction(2, 5));
    builder.addNextValue(2, 6, fraction(3, 5));
    builder.addNextValue(3, 4, fraction(4, 5));
    builder.addNextValue(3, 5, fraction(1, 10));
    builder.addNextValue(3, 6, fraction(1, 10));
    builder.addNextValue(4, 4, fraction(1, 10));
    builder.addNextValue(4, 5, fraction(4, 5));
    builder.addNextValue(4, 6, fraction(1, 10));
    builder.addNextValue(5, 4, fraction(1, 50));
    builder.addNextValue(5, 5, fraction(1, 50));
    builder.addNextValue(5, 6, fraction(24, 25));
    builder.addNextValue(6, 2, one);
    builder.newRowGroup(7);
    builder.addNextValue(7, 5, fraction(2, 5));
    builder.addNextValue(7, 6, fraction(3, 5));
    builder.addNextValue(8, 3, one);
    builder.newRowGroup(9);
    builder.addNextValue(9, 4, fraction(2, 5));
    builder.addNextValue(9, 6, fraction(3, 5));
    builder.newRowGroup(10);
    builder.addNextValue(10, 4, one);
    builder.newRowGroup(11);
    builder.addNextValue(11, 5, one);
    builder.newRowGroup(12);
    builder.addNextValue(12, 6, one);

    ExitModel<ValueType> model{builder.build(), std::vector<ValueType>(13, zero), storm::storage::BitVector(7, false), storm::storage::BitVector(7, false)};
    model.weights[0] = one;
    model.targets.set(4);
    model.targets.set(5);
    model.targets.set(6);
    model.initials.set(0);
    return model;
}

template<typename ValueType>
class ZeroWeightExitLawEnumeratorTest : public ::testing::Test {};

using TestedValueTypes = ::testing::Types<double, storm::RationalNumber>;
TYPED_TEST_SUITE(ZeroWeightExitLawEnumeratorTest, TestedValueTypes, );

TYPED_TEST(ZeroWeightExitLawEnumeratorTest, FindsPolygonSegmentAndPoint) {
#if !defined(STORM_HAVE_GLPK) && !defined(STORM_HAVE_GUROBI) && !defined(STORM_HAVE_Z3) && !defined(STORM_HAVE_SOPLEX)
    GTEST_SKIP() << "No LP solver available.";
#endif
#if !defined(STORM_HAVE_Z3) && !defined(STORM_HAVE_SOPLEX)
    if constexpr (std::is_same_v<TypeParam, storm::RationalNumber>) {
        GTEST_SKIP() << "No exact LP solver available.";
    }
#endif
    using ValueType = TypeParam;
    auto const model = buildExitModel<ValueType>();
    auto analysis = storm::transformer::ZeroWeightAnalysis<ValueType>::analyze(model.matrix, model.weights, model.targets, model.initials);
    storm::transformer::ZeroWeightAnalysis<ValueType>::analyzeComponentInterfaces(model.matrix, analysis);
    ASSERT_EQ(1ull, analysis.weakComponents.size());
    storm::Environment environment;
    storm::transformer::ZeroWeightComponentLp<ValueType> lp(environment, model.matrix, model.weights, analysis.weakComponents.front());
    EXPECT_EQ((std::vector<uint64_t>{1, 2, 3}), lp.getEntryStates());
    EXPECT_EQ((std::vector<uint64_t>{4, 5, 6}), lp.getBoundaryStates());

    auto const one = storm::utility::one<ValueType>();
    auto const zero = storm::utility::zero<ValueType>();
    auto fraction = [](uint64_t numerator, uint64_t denominator) -> ValueType {
        return storm::utility::convertNumber<ValueType>(numerator) / storm::utility::convertNumber<ValueType>(denominator);
    };
    auto const tied = lp.maximize(1, {one, one, zero});
    if constexpr (std::is_same_v<ValueType, double>) {
        EXPECT_NEAR(fraction(9, 10), tied.objectiveValue, 1e-10);
    } else {
        EXPECT_EQ(fraction(9, 10), tied.objectiveValue);
    }

    storm::transformer::ZeroWeightExitLawEnumerator<ValueType> enumerator(lp);
    std::vector<std::vector<ValueType>> polygon{{zero, fraction(2, 5), fraction(3, 5)},
                                                {fraction(2, 5), zero, fraction(3, 5)},
                                                {fraction(4, 5), fraction(1, 10), fraction(1, 10)},
                                                {fraction(1, 10), fraction(4, 5), fraction(1, 10)},
                                                {fraction(1, 50), fraction(1, 50), fraction(24, 25)}};
    std::sort(polygon.begin(), polygon.end());
    auto const checkVertices = [](std::vector<std::vector<ValueType>> const& actual, std::vector<std::vector<ValueType>> const& expected) {
        ASSERT_EQ(expected.size(), actual.size());
        for (uint64_t vertex = 0; vertex < expected.size(); ++vertex) {
            ASSERT_EQ(expected[vertex].size(), actual[vertex].size());
            for (uint64_t coordinate = 0; coordinate < expected[vertex].size(); ++coordinate) {
                if constexpr (std::is_same_v<ValueType, double>) {
                    EXPECT_NEAR(expected[vertex][coordinate], actual[vertex][coordinate], 1e-8);
                } else {
                    EXPECT_EQ(expected[vertex][coordinate], actual[vertex][coordinate]);
                }
            }
        }
    };

    checkVertices(enumerator.enumerate(1), polygon);
    checkVertices(enumerator.enumerate(2), {polygon[0], polygon[3]});
    checkVertices(enumerator.enumerate(3), {polygon[3]});
}

TYPED_TEST(ZeroWeightExitLawEnumeratorTest, UsesTheOnlyBoundaryLaw) {
#if !defined(STORM_HAVE_GLPK) && !defined(STORM_HAVE_GUROBI) && !defined(STORM_HAVE_Z3) && !defined(STORM_HAVE_SOPLEX)
    GTEST_SKIP() << "No LP solver available.";
#endif
#if !defined(STORM_HAVE_Z3) && !defined(STORM_HAVE_SOPLEX)
    if constexpr (std::is_same_v<TypeParam, storm::RationalNumber>) {
        GTEST_SKIP() << "No exact LP solver available.";
    }
#endif
    using ValueType = TypeParam;
    auto const one = storm::utility::one<ValueType>();
    auto const zero = storm::utility::zero<ValueType>();
    storm::storage::SparseMatrixBuilder<ValueType> builder(3, 3, 3, true, true, 3);
    builder.newRowGroup(0);
    builder.addNextValue(0, 1, one);
    builder.newRowGroup(1);
    builder.addNextValue(1, 2, one);
    builder.newRowGroup(2);
    builder.addNextValue(2, 2, one);
    auto const matrix = builder.build();
    auto const weights = std::vector<ValueType>{one, zero, zero};
    storm::storage::BitVector targets(3, false);
    targets.set(2);
    storm::storage::BitVector initials(3, false);
    initials.set(0);
    auto analysis = storm::transformer::ZeroWeightAnalysis<ValueType>::analyze(matrix, weights, targets, initials);
    storm::transformer::ZeroWeightAnalysis<ValueType>::analyzeComponentInterfaces(matrix, analysis);
    storm::Environment environment;
    storm::transformer::ZeroWeightComponentLp<ValueType> lp(environment, matrix, weights, analysis.weakComponents.front());
    storm::transformer::ZeroWeightExitLawEnumerator<ValueType> enumerator(lp);
    EXPECT_EQ((std::vector<std::vector<ValueType>>{{one}}), enumerator.enumerate(1));
}

}  // namespace
