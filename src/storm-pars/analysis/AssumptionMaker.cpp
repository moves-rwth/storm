#include "AssumptionMaker.h"

namespace storm {
namespace analysis {
template<typename ValueType, typename ConstantType>
AssumptionMaker<ValueType, ConstantType>::AssumptionMaker(storage::SparseMatrix<ValueType> matrix,
                                                          std::shared_ptr<storm::models::sparse::StandardRewardModel<ValueType>> rewardModel)
    : assumptionChecker(matrix, rewardModel) {
    numberOfStates = matrix.getColumnCount();
    expressionManager = std::make_shared<expressions::ExpressionManager>(expressions::ExpressionManager());
    for (uint_fast64_t i = 0; i < this->numberOfStates; ++i) {
        expressionManager->declareRationalVariable(std::to_string(i));
    }
    this->matrix = matrix;
}

template<typename ValueType, typename ConstantType>
std::map<Assumption, AssumptionStatus> AssumptionMaker<ValueType, ConstantType>::createAndCheckAssumptions(uint_fast64_t val1, uint_fast64_t val2,
                                                                                                           std::shared_ptr<Order> order,
                                                                                                           storage::ParameterRegion<ValueType> region) const {
    auto vec1 = std::vector<ConstantType>();
    auto vec2 = std::vector<ConstantType>();
    return createAndCheckAssumptions(val1, val2, order, region, vec1, vec2);
}

template<typename ValueType, typename ConstantType>
std::map<Assumption, AssumptionStatus> AssumptionMaker<ValueType, ConstantType>::createAndCheckAssumptions(uint_fast64_t val1, uint_fast64_t val2,
                                                                                                           std::shared_ptr<Order> order,
                                                                                                           storage::ParameterRegion<ValueType> region,
                                                                                                           std::vector<ConstantType> const minValues,
                                                                                                           std::vector<ConstantType> const maxValues) const {
    std::map<Assumption, AssumptionStatus> result;
    if (val1 != val2) {
        STORM_LOG_INFO("Creating assumptions for " << val1 << " and " << val2);
        STORM_LOG_ASSERT(order->compare(val1, val2) == Order::UNKNOWN, "It makes no sense to create assumptions when the order is known");
        auto assumptionPair =
            createAndCheckAssumption(val1, val2, expressions::BinaryRelationExpression::RelationType::Greater, order, region, minValues, maxValues);
        if (assumptionPair.second != AssumptionStatus::INVALID) {
            result.insert(assumptionPair);
            if (assumptionPair.second == AssumptionStatus::VALID) {
                STORM_LOG_ASSERT(
                    createAndCheckAssumption(val2, val1, expressions::BinaryRelationExpression::RelationType::Greater, order, region, minValues, maxValues)
                            .second != AssumptionStatus::VALID,
                    "Assumption " << val2 << " > " << val1 << " cannot be valid as " << val1 << " > " << val2 << " holds.");
                STORM_LOG_ASSERT(
                    createAndCheckAssumption(val2, val1, expressions::BinaryRelationExpression::RelationType::Equal, order, region, minValues, maxValues)
                            .second != AssumptionStatus::VALID,
                    "Assumption " << val2 << " == " << val1 << " cannot be valid as " << val1 << " > " << val2 << " holds.");
                STORM_LOG_INFO("Assumption " << *(assumptionPair.first.getAssumption()) << " is valid" << std::endl);
                return result;
            }
        }
        assert(order->compare(val1, val2) == Order::UNKNOWN);
        assumptionPair =
            createAndCheckAssumption(val2, val1, expressions::BinaryRelationExpression::RelationType::Greater, order, region, minValues, maxValues);
        if (assumptionPair.second != AssumptionStatus::INVALID) {
            if (assumptionPair.second == AssumptionStatus::VALID) {
                result.clear();
                result.insert(assumptionPair);
                STORM_LOG_ASSERT(
                    createAndCheckAssumption(val1, val2, expressions::BinaryRelationExpression::RelationType::Greater, order, region, minValues, maxValues)
                            .second != AssumptionStatus::VALID,
                    "Assumption " << val1 << " > " << val2 << " cannot be valid as " << val2 << " > " << val1 << " holds.");
                STORM_LOG_ASSERT(
                    createAndCheckAssumption(val2, val1, expressions::BinaryRelationExpression::RelationType::Equal, order, region, minValues, maxValues)
                            .second != AssumptionStatus::VALID,
                    "Assumption " << val2 << " == " << val1 << " cannot be valid as " << val2 << " > " << val1 << " holds.");
                STORM_LOG_INFO("Assumption " << *(assumptionPair.first.getAssumption()) << " is valid" << std::endl);
                return result;
            }
            result.insert(assumptionPair);
        }
        assert(order->compare(val1, val2) == Order::UNKNOWN);
        assumptionPair = createAndCheckAssumption(val1, val2, expressions::BinaryRelationExpression::RelationType::Equal, order, region, minValues, maxValues);
        if (assumptionPair.second != AssumptionStatus::INVALID) {
            if (assumptionPair.second == AssumptionStatus::VALID) {
                result.clear();
                result.insert(assumptionPair);
                STORM_LOG_ASSERT(
                    createAndCheckAssumption(val1, val2, expressions::BinaryRelationExpression::RelationType::Greater, order, region, minValues, maxValues)
                            .second != AssumptionStatus::VALID,
                    "Assumption " << val1 << " > " << val2 << " cannot be valid as " << val2 << " == " << val1 << " holds.");
                STORM_LOG_ASSERT(
                    createAndCheckAssumption(val2, val1, expressions::BinaryRelationExpression::RelationType::Greater, order, region, minValues, maxValues)
                            .second != AssumptionStatus::VALID,
                    "Assumption " << val2 << " > " << val1 << " cannot be valid as " << val2 << " == " << val1 << " holds.");
                STORM_LOG_INFO("Assumption " << *(assumptionPair.first.getAssumption()) << " is valid" << std::endl);
                return result;
            }
            result.insert(assumptionPair);
        }
        assert(order->compare(val1, val2) == Order::UNKNOWN);
        STORM_LOG_INFO("None of the assumptions can be proven valid on the entire region, number of possible assumptions:  " << result.size() << std::endl);
    } else {
        STORM_LOG_INFO("Creating assumptions for the actions of state " << val1);
        auto numberOfActions = matrix.getRowGroupSize(val1);
        for (auto i = 0; i < numberOfActions; ++i) {
            result.insert(createAndCheckAssumptionAction(val1, i));
        }
    }
    return result;
}

template<typename ValueType, typename ConstantType>
void AssumptionMaker<ValueType, ConstantType>::initializeCheckingOnSamples(std::shared_ptr<logic::Formula const> formula,
                                                                           std::shared_ptr<models::sparse::Dtmc<ValueType>> model,
                                                                           storage::ParameterRegion<ValueType> region, uint_fast64_t numberOfSamples) {
    assumptionChecker.initializeCheckingOnSamples(formula, model, region, numberOfSamples);
}

template<typename ValueType, typename ConstantType>
void AssumptionMaker<ValueType, ConstantType>::setSampleValues(std::vector<std::vector<ConstantType>> const& samples) {
    assumptionChecker.setSampleValues(samples);
}

template<typename ValueType, typename ConstantType>
std::pair<Assumption, AssumptionStatus> AssumptionMaker<ValueType, ConstantType>::createAndCheckAssumption(
    uint_fast64_t val1, uint_fast64_t val2, expressions::BinaryRelationExpression::RelationType relationType, std::shared_ptr<Order> order,
    storage::ParameterRegion<ValueType> region, std::vector<ConstantType> const minValues, std::vector<ConstantType> const maxValues) const {
    STORM_LOG_ASSERT(val1 != val2, "Cannot make assumption for the same state");
    auto assumption = Assumption(true, expressionManager, val1, val2, relationType);
    AssumptionStatus validationResult = assumptionChecker.validateAssumption(val1, val2, assumption, order, region, minValues, maxValues);
    return std::pair<Assumption, AssumptionStatus>(assumption, validationResult);
}

template<typename ValueType, typename ConstantType>
std::pair<Assumption, AssumptionStatus> AssumptionMaker<ValueType, ConstantType>::createAndCheckAssumptionAction(uint_fast64_t val1,
                                                                                                                 uint_fast64_t action) const {
    auto assumption = Assumption(false, expressionManager, val1, action, expressions::BinaryRelationExpression::RelationType::Equal);
    AssumptionStatus validationResult = UNKNOWN;
    // TODO   STORM_LOG_WARN("Checking status of assumption for actions not yet implemented, therefore we assume status is unknown");
    return std::pair<Assumption, AssumptionStatus>(assumption, validationResult);
}
template<typename ValueType, typename ConstantType>
void AssumptionMaker<ValueType, ConstantType>::setRewardModel(std::shared_ptr<storm::models::sparse::StandardRewardModel<ValueType>> rewardModel) {
    this->assumptionChecker.setRewardModel(rewardModel);
}

template class AssumptionMaker<RationalFunction, double>;
template class AssumptionMaker<RationalFunction, RationalNumber>;
}  // namespace analysis
}  // namespace storm
