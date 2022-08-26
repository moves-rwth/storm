#pragma once
#include "storm-pars/analysis/OrderExtender.h"

namespace storm {
namespace analysis {
template<typename ValueType, typename ConstantType>
class RewardOrderExtender : public OrderExtender<ValueType, ConstantType> {
   public:
    typedef typename utility::parametric::CoefficientType<ValueType>::type CoefficientType;
    typedef typename utility::parametric::VariableType<ValueType>::type VariableType;
    typedef typename MonotonicityResult<VariableType>::Monotonicity Monotonicity;

    // Used to call the constructor of OrderExtender
    RewardOrderExtender(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula);

    // Used to call the constructor of OrderExtender
    RewardOrderExtender(storm::storage::BitVector& topStates, storm::storage::BitVector& bottomStates, storm::storage::SparseMatrix<ValueType> matrix,
                        storm::models::sparse::StandardRewardModel<ValueType> rewardModel, bool prMax);
    // Used to call the constructor of OrderExtender
    RewardOrderExtender(storm::storage::BitVector& topStates, storm::storage::BitVector& bottomStates, storm::storage::SparseMatrix<ValueType> matrix,
                        storm::models::sparse::StandardRewardModel<ValueType> rewardModel);

    std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> extendOrder(
        std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes = nullptr,
        std::shared_ptr<expressions::BinaryRelationExpression> assumption = nullptr) override;

   protected:
    // Override methods from OrderExtender
    void handleOneSuccessor(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t successor) override;
    std::shared_ptr<Order> getInitialOrder() override;
    std::pair<uint_fast64_t, uint_fast64_t> extendByBackwardReasoning(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region,
                                                                      uint_fast64_t currentState) override;
    std::pair<uint_fast64_t, uint_fast64_t> extendByForwardReasoning(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region,
                                                                     uint_fast64_t currentState) override;

   private:
    // There is only one interesting successor, as the other one is either the current state, or a bottom state
    bool extendByForwardReasoningOneSucc(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t currentState);

    bool rewardHack(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t succ0, uint_fast64_t succ1);

    /*** Model Info ***/
    // Reward model of our model
    storm::models::sparse::StandardRewardModel<ValueType> rewardModel;
    storage::SparseMatrix<ValueType> transposeMatrix;
    storage::BitVector assumptionsCreated;
};
}  // namespace analysis
}  // namespace storm