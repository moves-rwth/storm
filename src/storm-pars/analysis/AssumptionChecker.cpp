#include "AssumptionChecker.h"

#include "storm-pars/utility/ModelInstantiator.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/modelchecker/CheckTask.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/results/CheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/solver/Z3SmtSolver.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm/storage/expressions/RationalFunctionToExpression.h"
#include "storm/storage/expressions/SimpleValuation.h"
#include "storm/storage/expressions/VariableExpression.h"
#include "storm/utility/solver.h"
namespace storm {
namespace analysis {
template<typename ValueType, typename ConstantType>
AssumptionChecker<ValueType, ConstantType>::AssumptionChecker(storage::SparseMatrix<ValueType> matrix,
                                                              std::shared_ptr<storm::models::sparse::StandardRewardModel<ValueType>> rewardModel) {
    this->matrix = matrix;
    if (rewardModel != nullptr) {
        this->rewardModel = rewardModel;
    }
    useSamples = false;
}

template<typename ValueType, typename ConstantType>
void AssumptionChecker<ValueType, ConstantType>::initializeCheckingOnSamples(const std::shared_ptr<logic::Formula const>& formula,
                                                                             std::shared_ptr<models::sparse::Dtmc<ValueType>> model,
                                                                             storage::ParameterRegion<ValueType> region, uint_fast64_t numberOfSamples) {
    // Create sample points
    auto instantiator = utility::ModelInstantiator<models::sparse::Dtmc<ValueType>, models::sparse::Dtmc<ConstantType>>(*model);
    //            auto matrix = model->getTransitionMatrix();
    std::set<VariableType> variables = models::sparse::getProbabilityParameters(*model);

    for (uint_fast64_t i = 0; i < numberOfSamples; ++i) {
        auto valuation = utility::parametric::Valuation<ValueType>();
        for (auto var : variables) {
            auto lb = region.getLowerBoundary(var.name());
            auto ub = region.getUpperBoundary(var.name());
            // Creates samples between lb and ub, that is: lb, lb + (ub-lb)/(#samples -1), lb + 2* (ub-lb)/(#samples -1), ..., ub
            auto val = std::pair<VariableType, CoefficientType>(
                var, (lb + utility::convertNumber<CoefficientType>(i) * (ub - lb)) / utility::convertNumber<CoefficientType>(numberOfSamples - 1));
            valuation.insert(val);
        }
        models::sparse::Dtmc<ConstantType> sampleModel = instantiator.instantiate(valuation);
        auto checker = modelchecker::SparseDtmcPrctlModelChecker<models::sparse::Dtmc<ConstantType>>(sampleModel);
        std::unique_ptr<modelchecker::CheckResult> checkResult;
        if (formula->isProbabilityOperatorFormula() && formula->asProbabilityOperatorFormula().getSubformula().isUntilFormula()) {
            const modelchecker::CheckTask<logic::UntilFormula, ConstantType> checkTask =
                modelchecker::CheckTask<logic::UntilFormula, ConstantType>((*formula).asProbabilityOperatorFormula().getSubformula().asUntilFormula());
            checkResult = checker.computeUntilProbabilities(Environment(), checkTask);
        } else if (formula->isProbabilityOperatorFormula() && formula->asProbabilityOperatorFormula().getSubformula().isEventuallyFormula()) {
            const modelchecker::CheckTask<logic::EventuallyFormula, ConstantType> checkTask = modelchecker::CheckTask<logic::EventuallyFormula, ConstantType>(
                (*formula).asProbabilityOperatorFormula().getSubformula().asEventuallyFormula());
            checkResult = checker.computeReachabilityProbabilities(Environment(), checkTask);
        } else {
            STORM_LOG_THROW(false, exceptions::NotSupportedException, "Expecting until or eventually formula");
        }
        auto quantitativeResult = checkResult->asExplicitQuantitativeCheckResult<ConstantType>();
        std::vector<ConstantType> values = quantitativeResult.getValueVector();
        samples.push_back(values);
    }
    useSamples = true;
}

template<typename ValueType, typename ConstantType>
void AssumptionChecker<ValueType, ConstantType>::setSampleValues(std::vector<std::vector<ConstantType>> samplesSet) {
    this->samples = samplesSet;
    useSamples = true;
}

template<typename ValueType, typename ConstantType>
AssumptionStatus AssumptionChecker<ValueType, ConstantType>::validateAssumption(uint_fast64_t state1, uint_fast64_t state2, Assumption fullAssumption,
                                                                                std::shared_ptr<Order> order, storage::ParameterRegion<ValueType> region,
                                                                                std::vector<ConstantType> const minValues,
                                                                                std::vector<ConstantType> const maxValues) const {
    AssumptionStatus result = AssumptionStatus::UNKNOWN;
    if (fullAssumption.isStateAssumption()) {
        auto assumption = fullAssumption.getAssumption();
        // First check if based on sample points the assumption can be discharged
        assert(state1 == std::stoull(assumption->getFirstOperand()->asVariableExpression().getVariableName()));
        assert(state2 == std::stoull(assumption->getSecondOperand()->asVariableExpression().getVariableName()));
        if (useSamples) {
            result = checkOnSamples(fullAssumption);
        }
        assert(result != AssumptionStatus::VALID);

        // TODO: use Optimistic here as well?
        if (minValues.size() != 0) {
            if (assumption->getRelationType() == expressions::BinaryRelationExpression::RelationType::Greater) {
                if (minValues[state1] > maxValues[state2]) {
                    return AssumptionStatus::VALID;
                } else if (minValues[state1] == maxValues[state2] && minValues[state1] == maxValues[state1] && minValues[state2] == maxValues[state2]) {
                    return AssumptionStatus::INVALID;
                } else if (minValues[state2] > maxValues[state1]) {
                    return AssumptionStatus::INVALID;
                }
            } else {
                if (minValues[state1] == maxValues[state2] && minValues[state1] == maxValues[state1] && minValues[state2] == maxValues[state2]) {
                    return AssumptionStatus::VALID;
                } else if (minValues[state1] > maxValues[state2]) {
                    return AssumptionStatus::INVALID;
                } else if (minValues[state2] > maxValues[state1]) {
                    return AssumptionStatus::INVALID;
                }
            }
        }

        if (result == AssumptionStatus::UNKNOWN) {
            // If result from sample checking was unknown, the assumption might hold
            STORM_LOG_THROW(assumption->getRelationType() == expressions::BinaryRelationExpression::RelationType::Greater ||
                                assumption->getRelationType() == expressions::BinaryRelationExpression::RelationType::Equal,
                            exceptions::NotSupportedException, "Only Greater Or Equal assumptions supported");

            if (order->isActionSetAtState(state1) && order->isActionSetAtState(state2)) {
                STORM_LOG_INFO("Validating assumption " << assumption->toExpression().toString() << " with action " << order->getActionAtState(state1)
                                                        << " for state " << state1 << " and action " << order->getActionAtState(state2) << " for state "
                                                        << state2);
                result = validateAssumptionSMTSolver(state1, state2, order->getActionAtState(state1), order->getActionAtState(state2), fullAssumption, order,
                                                     region, minValues, maxValues);
            } else if (order->isActionSetAtState(state1)) {
                STORM_LOG_INFO("Validating assumption " << assumption->toExpression().toString() << " with action " << order->getActionAtState(state1)
                                                        << " for state " << state1 << " and all actions for " << state2);
                bool initialized = false;
                for (auto action2 = 0; action2 < this->matrix.getRowGroupSize(state2); ++action2) {
                    auto tempResult = validateAssumptionSMTSolver(state1, state2, order->getActionAtState(state1), action2, fullAssumption, order, region,
                                                                  minValues, maxValues);
                    if (!initialized) {
                        result = tempResult;
                        initialized = true;
                    } else if (result == AssumptionStatus::VALID && tempResult == AssumptionStatus::INVALID) {
                        result = AssumptionStatus::UNKNOWN;
                    } else if (result == AssumptionStatus::INVALID && tempResult == AssumptionStatus::VALID) {
                        result = AssumptionStatus::UNKNOWN;
                    }
                    if (result == AssumptionStatus::UNKNOWN || tempResult == AssumptionStatus::UNKNOWN) {
                        break;
                    }
                }
            } else if (order->isActionSetAtState(state2)) {
                STORM_LOG_INFO("Validating assumption " << assumption->toExpression().toString() << " with action " << order->getActionAtState(state2)
                                                        << " for state " << state2 << " and all actions for " << state1);

                bool initialized = false;
                for (auto action1 = 0; action1 < this->matrix.getRowGroupSize(state1); ++action1) {
                    auto tempResult = validateAssumptionSMTSolver(state1, state2, action1, order->getActionAtState(state2), fullAssumption, order, region,
                                                                  minValues, maxValues);
                    if (!initialized) {
                        result = tempResult;
                        initialized = true;
                    } else if (result == AssumptionStatus::VALID && tempResult == AssumptionStatus::INVALID) {
                        result = AssumptionStatus::UNKNOWN;
                    } else if (result == AssumptionStatus::INVALID && tempResult == AssumptionStatus::VALID) {
                        result = AssumptionStatus::UNKNOWN;
                    }
                    if (result == AssumptionStatus::UNKNOWN || tempResult == AssumptionStatus::UNKNOWN) {
                        break;
                    }
                }
            } else {
                STORM_LOG_INFO("Validating assumption " << assumption->toExpression().toString() << " with all actions for " << state1 << " and " << state2);

                bool initialized = false;
                for (auto action1 = 0; action1 < this->matrix.getRowGroupSize(state1); ++action1) {
                    for (auto action2 = 0; action2 < this->matrix.getRowGroupSize(state2); ++action2) {
                        auto tempResult = validateAssumptionSMTSolver(state1, state2, action1, action2, fullAssumption, order, region, minValues, maxValues);
                        if (!initialized) {
                            result = tempResult;
                            initialized = true;
                        } else if (result == AssumptionStatus::VALID && tempResult == AssumptionStatus::INVALID) {
                            result = AssumptionStatus::UNKNOWN;
                        } else if (result == AssumptionStatus::INVALID && tempResult == AssumptionStatus::VALID) {
                            result = AssumptionStatus::UNKNOWN;
                        }
                        if (result == AssumptionStatus::UNKNOWN || tempResult == AssumptionStatus::UNKNOWN) {
                            break;
                        }
                    }
                }
            }
        }
    } else {
        STORM_LOG_WARN("Checking action assumptions not yet implemented, assuming they are valid");
    }
    return result;
}

template<typename ValueType, typename ConstantType>
AssumptionStatus AssumptionChecker<ValueType, ConstantType>::checkOnSamples(const Assumption& assumption) const {
    auto result = AssumptionStatus::UNKNOWN;
    if (assumption.isStateAssumption()) {
        std::set<expressions::Variable> vars = std::set<expressions::Variable>({});
        assumption.getAssumption()->gatherVariables(vars);
        for (auto values : samples) {
            auto valuation = expressions::SimpleValuation(assumption.getAssumption()->getManager().getSharedPointer());
            for (auto var : vars) {
                auto index = std::stoi(var.getName());
                valuation.setRationalValue(var, utility::convertNumber<double>(values[index]));
            }

            assert(assumption.getAssumption()->hasBooleanType());
            if (!assumption.getAssumption()->evaluateAsBool(&valuation)) {
                result = AssumptionStatus::INVALID;
                break;
            }
        }
    } else {
        STORM_LOG_WARN("Checking action assumption on samples not yet implemented");
    }
    return result;
}

template<typename ValueType, typename ConstantType>
expressions::Expression AssumptionChecker<ValueType, ConstantType>::getExpressionBounds(
    const std::shared_ptr<expressions::ExpressionManager>& manager, storage::ParameterRegion<ValueType> const& region, std::string state1, std::string state2,
    std::set<expressions::Variable> const& stateVariables, std::set<expressions::Variable> const& topVariables,
    std::set<expressions::Variable> const& bottomVariables, std::vector<ConstantType> const& minValues, std::vector<ConstantType> const& maxValues) const {
    expressions::Expression exprBounds = manager->boolean(true);
    for (auto& var : manager->getVariables()) {
        if (find(stateVariables.begin(), stateVariables.end(), var) != stateVariables.end()) {
            // the var is a state
            std::string stateName = var.getName();
            auto state = std::stoi(stateName.substr(1, stateName.size() - 1));
            STORM_LOG_ASSERT(state < this->matrix.getRowGroupCount(), "Invalid state number");
            //                    if (stateName != state1 && stateName != state2) {
            if (minValues.size() > 0) {
                exprBounds = exprBounds && manager->rational(minValues[state]) <= var && var <= manager->rational(maxValues[state]);
            } else if (rewardModel == nullptr) {
                // Probability property
                exprBounds = exprBounds && manager->rational(0) <= var && var <= manager->rational(1);
            } else {
                // Reward Property
                exprBounds = exprBounds && manager->rational(0) <= var;
            }
            //                    }
        } else if (find(topVariables.begin(), topVariables.end(), var) != topVariables.end()) {
            // the var is =)
            if (rewardModel == nullptr) {
                exprBounds = exprBounds && var == manager->rational(1);
            }
        } else if (find(bottomVariables.begin(), bottomVariables.end(), var) != bottomVariables.end()) {
            // the var is =(
            exprBounds = exprBounds && var == manager->rational(0);
        } else {
            // the var is a parameter
            auto lb = utility::convertNumber<RationalNumber>(region.getLowerBoundary(var.getName()));
            auto ub = utility::convertNumber<RationalNumber>(region.getUpperBoundary(var.getName()));
            exprBounds = exprBounds && manager->rational(lb) < var && var < manager->rational(ub);
        }
    }
    return exprBounds;
}

template<typename ValueType, typename ConstantType>
expressions::Expression AssumptionChecker<ValueType, ConstantType>::getExpressionOrderSuccessors(const std::shared_ptr<expressions::ExpressionManager>& manager,
                                                                                                 std::shared_ptr<Order> order,
                                                                                                 std::set<uint_fast64_t> const& successors1,
                                                                                                 std::set<uint_fast64_t> const& successors2) const {
    expressions::Expression exprOrderSucc = manager->boolean(true);
    std::set<uint_fast64_t> successors = successors1;
    for (auto& succ : successors2) {
        successors.insert(succ);
    }

    for (auto itr = successors.begin(); itr != successors.end(); ++itr) {
        auto itr2 = itr;
        while (itr2 != successors.end()) {
            auto varname1 = "s" + std::to_string(*itr2);
            if (*itr != *itr2) {
                auto comp = order->compare(*itr, *itr2);
                if (comp == Order::NodeComparison::ABOVE) {
                    exprOrderSucc = exprOrderSucc && manager->getVariable("s" + std::to_string(*itr)) > manager->getVariable("s" + std::to_string(*itr2));
                }
                if (comp == Order::NodeComparison::SAME) {
                    exprOrderSucc = exprOrderSucc && manager->getVariable("s" + std::to_string(*itr)) >= manager->getVariable("s" + std::to_string(*itr2)) &&
                                    manager->getVariable("s" + std::to_string(*itr)) <= manager->getVariable("s" + std::to_string(*itr2));
                }
                if (comp == Order::NodeComparison::BELOW) {
                    exprOrderSucc = exprOrderSucc && manager->getVariable("s" + std::to_string(*itr2)) > manager->getVariable("s" + std::to_string(*itr));
                }
            }
            ++itr2;
        }
    }
    return exprOrderSucc;
}

template<typename ValueType, typename ConstantType>
expressions::Expression AssumptionChecker<ValueType, ConstantType>::getStateExpression(const std::shared_ptr<expressions::ExpressionManager>& manager,
                                                                                       uint_fast64_t state, uint_fast64_t action) const {
    auto valueTypeToExpression = expressions::RationalFunctionToExpression<ValueType>(manager);
    auto row = matrix.getRow(state, action);
    expressions::Expression expr;
    if (rewardModel != nullptr) {
        // We are dealing with a reward property
        if (rewardModel->hasStateActionRewards()) {
            expr = valueTypeToExpression.toExpression(rewardModel->getStateActionReward(matrix.getRowGroupIndices()[state] + action));
        } else if (rewardModel->hasStateRewards()) {
            expr = valueTypeToExpression.toExpression(rewardModel->getStateReward(state));
        } else {
            STORM_LOG_ASSERT(false, "Expecting reward");
        }
    } else {
        // We are dealing with a probability property
        expr = manager->rational(0);
        expr = manager->rational(0);
    }

    for (auto itr1 = row.begin(); itr1 != row.end(); ++itr1) {
        expr = expr + (valueTypeToExpression.toExpression(itr1->getValue()) * manager->getVariable("s" + std::to_string(itr1->getColumn())));
    }

    return expr;
}

template<typename ValueType, typename ConstantType>
expressions::Expression AssumptionChecker<ValueType, ConstantType>::getAdditionalStateExpression(const std::shared_ptr<expressions::ExpressionManager>& manager,
                                                                                                 uint_fast64_t state, uint_fast64_t action) const {
    auto valueTypeToExpression = expressions::RationalFunctionToExpression<ValueType>(manager);
    expressions::Expression exprAdditional = manager->boolean(true);

    auto row = matrix.getRow(state, action);
    std::set<uint_fast64_t> seenStates;
    for (auto& entry : row) {
        auto succ = entry.getColumn();
        seenStates.insert(state);
        bool onlyOneSucc = matrix.getRowGroupSize(succ) == 1 && matrix.getRow(succ, 0).getNumberOfEntries() == 1 && seenStates.find(succ) == seenStates.end();
        while (onlyOneSucc) {
            if (rewardModel != nullptr && succ == matrix.getRow(succ, 0).begin()->getColumn()) {
                // We are at a sink state, so the expRew = 0
                exprAdditional = exprAdditional && (manager->getVariable("s" + std::to_string(succ)) == manager->rational(0));
                onlyOneSucc = false;
            } else {
                expressions::Expression newExpr = getStateExpression(manager, succ, 0);
                exprAdditional = exprAdditional && (manager->getVariable("s" + std::to_string(succ)) == newExpr);
                seenStates.insert(succ);
                succ = matrix.getRow(succ, 0).begin()->getColumn();
                onlyOneSucc =
                    matrix.getRowGroupSize(succ) == 1 && matrix.getRow(succ, 0).getNumberOfEntries() == 1 && seenStates.find(succ) == seenStates.end();
            }
        }
    }
    return exprAdditional;
}

template<typename ValueType, typename ConstantType>
std::set<uint_fast64_t> AssumptionChecker<ValueType, ConstantType>::getSuccessors(uint_fast64_t state, uint_fast64_t action) const {
    auto row1 = matrix.getRow(state, action);
    std::set<uint_fast64_t> result;
    for (auto itr1 = row1.begin(); itr1 != row1.end(); ++itr1) {
        auto succ = itr1->getColumn();
        result.insert(succ);
        bool onlyOneSucc = matrix.getRowGroupSize(succ) == 1 && matrix.getRow(succ, 0).getNumberOfEntries() == 1;
        while (onlyOneSucc) {
            auto entry = matrix.getRow(succ, 0).begin();
            result.insert(succ);
            onlyOneSucc = matrix.getRowGroupSize(succ) == 1 && matrix.getRow(succ, 0).getNumberOfEntries() == 1;
            succ = entry->getColumn();
            onlyOneSucc = onlyOneSucc && result.find(succ) == result.end();
        }
    }
    return result;
}

template<typename ValueType, typename ConstantType>
AssumptionStatus AssumptionChecker<ValueType, ConstantType>::validateAssumptionSMTSolver(
    uint_fast64_t state1, uint_fast64_t state2, uint_fast64_t action1, uint_fast64_t action2, Assumption assumption, std::shared_ptr<Order> order,
    storage::ParameterRegion<ValueType> region, std::vector<ConstantType> const minValues, std::vector<ConstantType> const maxValues) const {
    if (assumption.isStateAssumption()) {
        STORM_LOG_ASSERT(!order->isActionSetAtState(state1) || action1 == order->getActionAtState(state1),
                         "Expecting action to either not be set, or to correspond to the action set in the order");
        STORM_LOG_ASSERT(!order->isActionSetAtState(state2) || action2 == order->getActionAtState(state2),
                         "Expecting action to either not be set, or to correspond to the action set in the order");

        auto var1 = assumption.getAssumption()->getFirstOperand()->asVariableExpression().getVariableName();
        auto var2 = assumption.getAssumption()->getSecondOperand()->asVariableExpression().getVariableName();

        // We first try to validate it with an slightly modification
        auto successors1 = getSuccessors(state1, action1);
        auto successors2 = getSuccessors(state2, action2);

        // We need a new manager as the one from the assumption is const.
        std::shared_ptr<expressions::ExpressionManager> manager(new expressions::ExpressionManager());

        // --------------------------------------------------------------------------------
        // Add all necessary state variables
        // --------------------------------------------------------------------------------
        std::set<expressions::Variable> stateVariables;
        std::set<expressions::Variable> topVariables;
        std::set<expressions::Variable> bottomVariables;
        for (auto state : successors1) {
            auto varname1 = "s" + std::to_string(state);
            if (!manager->hasVariable(varname1)) {
                if (order->isTopState(state)) {
                    topVariables.insert(manager->declareRationalVariable(varname1));
                } else if (order->isBottomState(state)) {
                    bottomVariables.insert(manager->declareRationalVariable(varname1));
                } else {
                    stateVariables.insert(manager->declareRationalVariable(varname1));
                }
            }
        }
        for (auto state : successors2) {
            auto varname1 = "s" + std::to_string(state);
            if (!manager->hasVariable(varname1)) {
                if (order->isTopState(state)) {
                    topVariables.insert(manager->declareRationalVariable(varname1));
                } else if (order->isBottomState(state)) {
                    bottomVariables.insert(manager->declareRationalVariable(varname1));
                } else {
                    stateVariables.insert(manager->declareRationalVariable(varname1));
                }
            }
        }
        auto varname1 = "s" + std::to_string(state1);
        if (!manager->hasVariable(varname1)) {
            if (order->isTopState(state1)) {
                topVariables.insert(manager->declareRationalVariable(varname1));
            } else if (order->isBottomState(state1)) {
                bottomVariables.insert(manager->declareRationalVariable(varname1));
            } else {
                stateVariables.insert(manager->declareRationalVariable(varname1));
            }
        }
        auto varname2 = "s" + std::to_string(state2);
        if (!manager->hasVariable(varname2)) {
            if (order->isTopState(state2)) {
                topVariables.insert(manager->declareRationalVariable(varname2));
            } else if (order->isBottomState(state2)) {
                bottomVariables.insert(manager->declareRationalVariable(varname2));
            } else {
                stateVariables.insert(manager->declareRationalVariable(varname2));
            }
        }
        // --------------------------------------------------------------------------------
        // Expressions for the states of the assumption
        // --------------------------------------------------------------------------------
        // We need to do this first, otherwise the probabilistic parameters might not be there when we get the bounds for the variables
        expressions::Expression expr1 = manager->getVariableExpression(varname1);
        expressions::Expression expr2 = manager->getVariableExpression(varname2);

        solver::Z3SmtSolver s(*manager);
        // --------------------------------------------------------------------------------
        // Expression for the successors of our state
        // --------------------------------------------------------------------------------
        s.add(getExpressionOrderSuccessors(manager, order, successors1, successors2));

        // --------------------------------------------------------------------------------
        // Unrolling for states with only one successor
        // --------------------------------------------------------------------------------
        s.add(getAdditionalStateExpression(manager, state1, action1) && getAdditionalStateExpression(manager, state2, action2));

        // --------------------------------------------------------------------------------
        // Expression for the states
        // --------------------------------------------------------------------------------
        expressions::Expression stateExpression =
            expr1 == getStateExpression(manager, state1, action1) && expr2 == getStateExpression(manager, state2, action2);
        s.add(stateExpression);

        // --------------------------------------------------------------------------------
        // Expression for the bounds on the variables, need to do this last, otherwise we might not have all variables yet
        // --------------------------------------------------------------------------------
        s.add(getExpressionBounds(manager, region, varname1, varname2, stateVariables, topVariables, bottomVariables, minValues, maxValues));

        // --------------------------------------------------------------------------------
        // Check if the order of the successors + the bounds is satisfiable
        // --------------------------------------------------------------------------------
        s.setTimeout(1000);
        // assert that sorting of successors in the order and the bounds on the expression are at least satisfiable
        // when this is not the case, the order is invalid
        // however, it could be that the sat solver didn't finish in time, in that case we just continue.
        if (s.check() == solver::SmtSolver::CheckResult::Unsat) {
            STORM_LOG_WARN("The order of successors plus the bounds should be satisfiable, the order is invalid, this may be due to assumptions");
            order->setInvalid();
            return AssumptionStatus::INVALID;
        }

        // Create expression for the assumption based on the relation to successors
        // It is the negation of actual assumption

        expressions::Expression exprToCheck;
        if (assumption.getAssumption()->getRelationType() == expressions::BinaryRelationExpression::RelationType::Greater) {
            exprToCheck = expr1 <= expr2;
        } else {
            exprToCheck = expr1 != expr2;
        }
        s.add(exprToCheck);
        s.unsetTimeout();
        solver::SmtSolver::CheckResult smtRes = s.check();
        if (smtRes == solver::SmtSolver::CheckResult::Unsat) {
            // If it is unsatisfiable the original assumtpion should be valid
            return AssumptionStatus::VALID;
        }
    } else {
        STORM_LOG_WARN("Validating action assumption not yet implemented");
    }

    return AssumptionStatus::UNKNOWN;
}

template<typename ValueType, typename ConstantType>
AssumptionStatus AssumptionChecker<ValueType, ConstantType>::validateAssumption(Assumption assumption, std::shared_ptr<Order> order,
                                                                                storage::ParameterRegion<ValueType> region) const {
    auto var1 = std::stoi(assumption.getAssumption()->getFirstOperand()->asVariableExpression().getVariableName());
    auto var2 = std::stoi(assumption.getAssumption()->getSecondOperand()->asVariableExpression().getVariableName());
    std::vector<ConstantType> vals;
    return validateAssumption(var1, var2, assumption, order, region, vals, vals);
}

template<typename ValueType, typename ConstantType>
void AssumptionChecker<ValueType, ConstantType>::setRewardModel(std::shared_ptr<storm::models::sparse::StandardRewardModel<ValueType>> rewardModelSet) {
    this->rewardModel = rewardModelSet;
}

template class AssumptionChecker<RationalFunction, double>;
template class AssumptionChecker<RationalFunction, RationalNumber>;
}  // namespace analysis
}  // namespace storm
