#pragma once
#include "Assumption.h"
#include "Order.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "storm/environment/Environment.h"
#include "storm/logic/Formula.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/expressions/ExpressionManager.h"

namespace storm {
namespace analysis {
/*!
 * Constants for status of assumption
 */
enum AssumptionStatus {
    VALID,
    INVALID,
    UNKNOWN,
};

template<typename ValueType, typename ConstantType>
class AssumptionChecker {
   public:
    typedef typename utility::parametric::VariableType<ValueType>::type VariableType;
    typedef typename utility::parametric::CoefficientType<ValueType>::type CoefficientType;

    /*!
     * Constructs an AssumptionChecker.
     *
     * @param matrix The matrix of the considered model.
     */
    AssumptionChecker(storage::SparseMatrix<ValueType> matrix, std::shared_ptr<storm::models::sparse::StandardRewardModel<ValueType>> rewardModel = nullptr);

    /*!
     * Initializes the given number of sample points for a given model, formula and region.
     *
     * @param formula The formula to compute the samples for.
     * @param model The considered model.
     * @param region The region of the model's parameters.
     * @param numberOfSamples Number of sample points.
     */
    void initializeCheckingOnSamples(const std::shared_ptr<logic::Formula const>& formula, std::shared_ptr<models::sparse::Dtmc<ValueType>> model,
                                     storage::ParameterRegion<ValueType> region, uint_fast64_t numberOfSamples);

    /*!
     * Sets the sample values to the given vector and useSamples to true.
     *
     * @param samples The new value for samples.
     */
    void setSampleValues(std::vector<std::vector<ConstantType>> samples);

    /*!
     * Tries to validate an assumption based on the order and underlying transition matrix.
     *
     * @param assumption The assumption to validate.
     * @param order The order.
     * @param region The region of the considered model.
     * @return AssumptionStatus::VALID, or AssumptionStatus::UNKNOWN, or AssumptionStatus::INVALID
     */
    AssumptionStatus validateAssumption(uint_fast64_t state1, uint_fast64_t state2, Assumption assumption, std::shared_ptr<Order> order,
                                        storage::ParameterRegion<ValueType> region, std::vector<ConstantType> const minValues,
                                        std::vector<ConstantType> const maxValue) const;
    AssumptionStatus validateAssumption(Assumption assumption, std::shared_ptr<Order> order, storage::ParameterRegion<ValueType> region) const;

    void setRewardModel(std::shared_ptr<storm::models::sparse::StandardRewardModel<ValueType>> rewardModel);

   private:
    AssumptionStatus validateAssumptionSMTSolver(uint_fast64_t state1, uint_fast64_t state2, uint_fast64_t action1, uint_fast64_t action2,
                                                 Assumption assumption, std::shared_ptr<Order> order, storage::ParameterRegion<ValueType> region,
                                                 std::vector<ConstantType> const minValues, std::vector<ConstantType> const maxValue) const;

    AssumptionStatus checkOnSamples(const Assumption& assumption) const;

    bool useSamples;

    std::vector<std::vector<ConstantType>> samples;

    storage::SparseMatrix<ValueType> matrix;

    // Reward model of our model, ONLY to be initialized if we are checking a reward property
    std::shared_ptr<storm::models::sparse::StandardRewardModel<ValueType>> rewardModel;

    std::set<uint_fast64_t> getSuccessors(uint_fast64_t state, uint_fast64_t action) const;
    expressions::Expression getExpressionBounds(const std::shared_ptr<expressions::ExpressionManager>& manager,
                                                const storage::ParameterRegion<ValueType>& region, std::string state1, std::string state2,
                                                const std::set<expressions::Variable>& stateVariables, const std::set<expressions::Variable>& topVariables,
                                                const std::set<expressions::Variable>& bottomVariables, const std::vector<ConstantType>& minValues,
                                                const std::vector<ConstantType>& maxValues) const;
    expressions::Expression getExpressionOrderSuccessors(const std::shared_ptr<expressions::ExpressionManager>& manager, std::shared_ptr<Order> order,
                                                         const std::set<uint_fast64_t>& successors, const std::set<uint_fast64_t>& successors2 = {}) const;
    expressions::Expression getStateExpression(const std::shared_ptr<expressions::ExpressionManager>& manager, uint_fast64_t state, uint_fast64_t action) const;
    expressions::Expression getAdditionalStateExpression(const std::shared_ptr<expressions::ExpressionManager>& manager, uint_fast64_t state,
                                                         uint_fast64_t action) const;
};
}  // namespace analysis
}  // namespace storm
