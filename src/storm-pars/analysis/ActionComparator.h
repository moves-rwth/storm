#pragma once
#include "storm-pars/analysis/Order.h"
#include "storm-pars/storage/ParameterRegion.h"

namespace storm {
namespace analysis {
template<typename ValueType, typename ConstantType>
class ActionComparator {
   public:
    typedef typename utility::parametric::CoefficientType<ValueType>::type CoefficientType;
    typedef typename utility::parametric::VariableType<ValueType>::type VariableType;
    typedef typename MonotonicityResult<VariableType>::Monotonicity Monotonicity;
    typedef typename storage::SparseMatrix<ValueType>::rows* Rows;
    enum ComparisonResult {
        GEQ,
        LEQ,
        UNKNOWN,
    };

    ActionComparator(std::shared_ptr<storm::models::sparse::StandardRewardModel<ValueType>> rewardModel = nullptr);

    expressions::Expression getExpressionBounds(const std::shared_ptr<expressions::ExpressionManager>& manager,
                                                storage::ParameterRegion<ValueType> const& region, std::set<std::string> states,
                                                std::vector<ConstantType> const& minValues, std::vector<ConstantType> const& maxValues) const;

    ComparisonResult actionSMTCompare(std::shared_ptr<Order> order, std::vector<uint64_t> const& orderedSuccs, storage::ParameterRegion<ValueType>& region,
                                      ValueType rew1, ValueType rew2, Rows row1, Rows row2) const;

    ComparisonResult actionSMTCompare(std::shared_ptr<Order> order, std::vector<uint64_t> const& orderedSuccs, storage::ParameterRegion<ValueType>& region,
                                      ValueType rew1, ValueType rew2, Rows row1, Rows row2, std::vector<ConstantType> const& minValues,
                                      std::vector<ConstantType> const& maxValues) const;

   private:
    /*!
     * Compares two rational functions
     * @param f1 The first rational function
     * @param f2 The second reational function
     * @param region The region for parameters
     * @return true iff The first function is greater or equal to the second one
     */
    ConstantType precision;
    std::shared_ptr<storm::models::sparse::StandardRewardModel<ValueType>> rewardModel;

    bool isFunctionGreaterEqual(storm::RationalFunction f1, storm::RationalFunction f2, storage::ParameterRegion<ValueType> region) const;

    ComparisonResult actionQuickCheck(std::shared_ptr<Order> order, std::vector<uint64_t> const& orderedSuccs, storage::ParameterRegion<ValueType>& region,
                                      ValueType rew1, ValueType rew2, Rows action1, Rows action2) const;
    std::pair<uint64_t, uint64_t> rangeOfSuccsForAction(typename storage::SparseMatrix<ValueType>::rows* action, std::vector<uint64_t> orderedSuccs) const;
};
}  // namespace analysis
}  // namespace storm
