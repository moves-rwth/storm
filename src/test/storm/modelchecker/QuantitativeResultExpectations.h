#pragma once

#include <memory>

#include "test/storm_gtest.h"

#include "storm/modelchecker/results/CheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/models/Model.h"

namespace storm::test {

/*!
 * The expectations on a quantitative check result and the solution bounds it may carry, to be mixed into a model
 * checker test fixture. The fixture has to provide getQuantitativeResultAtInitialState, precision and parseNumber,
 * and the test type has to provide isExact.
 */
template<typename Fixture, typename TestType>
class QuantitativeResultExpectations {
   public:
    typedef typename TestType::ValueType ValueType;
    typedef std::shared_ptr<storm::models::Model<ValueType>> ModelPtr;
    typedef std::unique_ptr<storm::modelchecker::CheckResult> ResultPtr;

    /*!
     * Expects the value at the initial states to be the given one. Where the configuration reports sound bounds on
     * the solution, they must enclose that value as well.
     */
    void expectQuantitativeResultAtInitialState(ModelPtr const& model, ResultPtr& result, ValueType const& expected) {
        EXPECT_NEAR(expected, fixture().getQuantitativeResultAtInitialState(model, result), fixture().precision());
        if (!result->isExplicitQuantitativeCheckResult()) {
            return;  // Only explicit results carry bounds.
        }
        // The result is filtered to the initial states at this point, so the first entry belongs to the first initial state.
        auto const& explicitResult = result->template asExplicitQuantitativeCheckResult<ValueType>();
        ValueType const tolerance = TestType::isExact ? fixture().parseNumber("0") : fixture().parseNumber("1e-12");
        if (explicitResult.hasLowerBounds()) {
            EXPECT_LE(explicitResult.getLowerBoundVector().front(), expected + tolerance) << "The lower bound exceeds the expected value.";
        }
        if (explicitResult.hasUpperBounds()) {
            EXPECT_LE(expected, explicitResult.getUpperBoundVector().front() + tolerance) << "The upper bound falls below the expected value.";
        }
    }

    /*!
     * Expects the value at the initial states to be the given one and the result to report it as exact.
     */
    void expectExactResultAtInitialState(ModelPtr const& model, ResultPtr& result, ValueType const& expected) {
        expectQuantitativeResultAtInitialState(model, result, expected);
        if (!result->isExplicitQuantitativeCheckResult()) {
            return;  // Only explicit results carry bounds.
        }
        auto const& explicitResult = result->template asExplicitQuantitativeCheckResult<ValueType>();
        ASSERT_TRUE(explicitResult.hasLowerBounds()) << "No lower bound was reported although the values are exact.";
        ASSERT_TRUE(explicitResult.hasUpperBounds()) << "No upper bound was reported although the values are exact.";
        EXPECT_EQ(explicitResult.getLowerBoundVector(), explicitResult.getUpperBoundVector()) << "The bounds do not pin the values down.";
    }

    /*!
     * Expects the value at the initial states to be the given one, enclosed by bounds the result must carry.
     */
    void expectBoundedResultAtInitialState(ModelPtr const& model, ResultPtr& result, ValueType const& expected) {
        expectQuantitativeResultAtInitialState(model, result, expected);
        if (!result->isExplicitQuantitativeCheckResult()) {
            return;  // Only explicit results carry bounds.
        }
        auto const& explicitResult = result->template asExplicitQuantitativeCheckResult<ValueType>();
        EXPECT_TRUE(explicitResult.hasLowerBounds()) << "No lower bound on the solution was reported.";
        EXPECT_TRUE(explicitResult.hasUpperBounds()) << "No upper bound on the solution was reported.";
    }

   private:
    Fixture& fixture() {
        return static_cast<Fixture&>(*this);
    }
};

}  // namespace storm::test
