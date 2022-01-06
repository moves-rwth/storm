#include "AssumptionChecker.h"

#include "storm-pars/utility/ModelInstantiator.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/modelchecker/CheckTask.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/results/CheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/storage/expressions/SimpleValuation.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/VariableExpression.h"
#include "storm/storage/expressions/RationalFunctionToExpression.h"
#include "storm/solver/Z3SmtSolver.h"
#include "storm/utility/solver.h"

namespace storm {
    namespace analysis {
        template <typename ValueType, typename ConstantType>
        AssumptionChecker<ValueType, ConstantType>::AssumptionChecker(storage::SparseMatrix<ValueType> matrix, std::shared_ptr<storm::models::sparse::StandardRewardModel<ValueType>> rewardModel){
            this->matrix = matrix;
            if (rewardModel != nullptr) {
                this->rewardModel = rewardModel;
            }
            useSamples = false;
        }

        template <typename ValueType, typename ConstantType>
        void AssumptionChecker<ValueType, ConstantType>::initializeCheckingOnSamples(std::shared_ptr<logic::Formula const> formula, std::shared_ptr<models::sparse::Dtmc<ValueType>> model, storage::ParameterRegion<ValueType> region, uint_fast64_t numberOfSamples) {
            // Create sample points
            auto instantiator = utility::ModelInstantiator<models::sparse::Dtmc<ValueType>, models::sparse::Dtmc<ConstantType>>(*model);
            auto matrix = model->getTransitionMatrix();
            std::set<VariableType> variables = models::sparse::getProbabilityParameters(*model);

            for (uint_fast64_t i = 0; i < numberOfSamples; ++i) {
                auto valuation = utility::parametric::Valuation<ValueType>();
                for (auto var: variables) {
                    auto lb = region.getLowerBoundary(var.name());
                    auto ub = region.getUpperBoundary(var.name());
                    // Creates samples between lb and ub, that is: lb, lb + (ub-lb)/(#samples -1), lb + 2* (ub-lb)/(#samples -1), ..., ub
                    auto val = std::pair<VariableType, CoefficientType>(var, (lb + utility::convertNumber<CoefficientType>(i) * (ub - lb)) / utility::convertNumber<CoefficientType>(numberOfSamples - 1));
                    valuation.insert(val);
                }
                models::sparse::Dtmc<ConstantType> sampleModel = instantiator.instantiate(valuation);
                auto checker = modelchecker::SparseDtmcPrctlModelChecker<models::sparse::Dtmc<ConstantType>>(sampleModel);
                std::unique_ptr<modelchecker::CheckResult> checkResult;
                if (formula->isProbabilityOperatorFormula() &&
                    formula->asProbabilityOperatorFormula().getSubformula().isUntilFormula()) {
                    const modelchecker::CheckTask<logic::UntilFormula, ConstantType> checkTask = modelchecker::CheckTask<logic::UntilFormula, ConstantType>(
                        (*formula).asProbabilityOperatorFormula().getSubformula().asUntilFormula());
                    checkResult = checker.computeUntilProbabilities(Environment(), checkTask);
                } else if (formula->isProbabilityOperatorFormula() &&
                           formula->asProbabilityOperatorFormula().getSubformula().isEventuallyFormula()) {
                    const modelchecker::CheckTask<logic::EventuallyFormula, ConstantType> checkTask = modelchecker::CheckTask<logic::EventuallyFormula, ConstantType>(
                        (*formula).asProbabilityOperatorFormula().getSubformula().asEventuallyFormula());
                    checkResult = checker.computeReachabilityProbabilities(Environment(), checkTask);
                } else {
                    STORM_LOG_THROW(false, exceptions::NotSupportedException,
                                    "Expecting until or eventually formula");
                }
                auto quantitativeResult = checkResult->asExplicitQuantitativeCheckResult<ConstantType>();
                std::vector<ConstantType> values = quantitativeResult.getValueVector();
                samples.push_back(values);
            }
            useSamples = true;
        }

        template <typename ValueType, typename ConstantType>
        void AssumptionChecker<ValueType, ConstantType>::setSampleValues(std::vector<std::vector<ConstantType>> samples) {
            this->samples = samples;
            useSamples = true;
        }

        template <typename ValueType, typename ConstantType>
        AssumptionChecker<ValueType, ConstantType>::AssumptionChecker(std::shared_ptr<logic::Formula const> formula, std::shared_ptr<models::sparse::Mdp<ValueType>> model, uint_fast64_t numberOfSamples) {
            STORM_LOG_THROW(false, exceptions::NotSupportedException, "Assumption checking for mdps not yet implemented");
        }

        template <typename ValueType, typename ConstantType>
        AssumptionStatus AssumptionChecker<ValueType, ConstantType>::validateAssumption(uint_fast64_t state1, uint_fast64_t state2,std::shared_ptr<expressions::BinaryRelationExpression> assumption, std::shared_ptr<Order> order, storage::ParameterRegion<ValueType> region, std::vector<ConstantType>const minValues, std::vector<ConstantType>const maxValues) const {
            // First check if based on sample points the assumption can be discharged
            assert (state1 == std::stoull(assumption->getFirstOperand()->asVariableExpression().getVariableName()));
            assert (state2 == std::stoull(assumption->getSecondOperand()->asVariableExpression().getVariableName()));
            AssumptionStatus result = AssumptionStatus::UNKNOWN;
            if (useSamples) {
                result = checkOnSamples(assumption);
            }
            assert (result != AssumptionStatus::VALID);

            if (minValues.size() != 0) {
                if (assumption->getRelationType() == expressions::BinaryRelationExpression::RelationType::Greater) {
                    if (minValues[state1] > maxValues[state2]) {
                        return AssumptionStatus::VALID;
                    } else  if (minValues[state1] == maxValues[state2] && minValues[state1] == maxValues[state1] && minValues[state2] == maxValues[state2]) {
                        return AssumptionStatus::INVALID;
                    }  else if (minValues[state2] > maxValues[state1]) {
                        return AssumptionStatus::INVALID;
                    }
                } else {
                    if (minValues[state1] == maxValues[state2] && minValues[state1] == maxValues[state1] && minValues[state2] == maxValues[state2]) {
                        return AssumptionStatus::VALID;
                    } else if (minValues[state1] > maxValues[state2]) {
                        return AssumptionStatus::INVALID;
                    }  else if (minValues[state2] > maxValues[state1]) {
                        return AssumptionStatus::INVALID;
                    }
                }
            }

            if (result == AssumptionStatus::UNKNOWN) {
                // If result from sample checking was unknown, the assumption might hold
                STORM_LOG_THROW(assumption->getRelationType() ==
                                        expressions::BinaryRelationExpression::RelationType::Greater ||
                                    assumption->getRelationType() ==
                                        expressions::BinaryRelationExpression::RelationType::Equal,
                                exceptions::NotSupportedException,
                                "Only Greater Or Equal assumptions supported");

                result = validateAssumptionSMTSolver(state1, state2, assumption, order, region, minValues, maxValues);
            }
            return result;
        }

        template <typename ValueType, typename ConstantType>
        AssumptionStatus AssumptionChecker<ValueType, ConstantType>::checkOnSamples(std::shared_ptr<expressions::BinaryRelationExpression> assumption) const {
            auto result = AssumptionStatus::UNKNOWN;
            std::set<expressions::Variable> vars = std::set<expressions::Variable>({});
            assumption->gatherVariables(vars);
            for (auto values : samples) {
                auto valuation = expressions::SimpleValuation(assumption->getManager().getSharedPointer());
                for (auto var : vars) {
                    auto index = std::stoi(var.getName());
                    valuation.setRationalValue(var, utility::convertNumber<double>(values[index]));
                }

                assert (assumption->hasBooleanType());
                if (!assumption->evaluateAsBool(&valuation)) {
                    result = AssumptionStatus::INVALID;
                    break;
                }
            }
            return result;
        }

        template <typename ValueType, typename ConstantType>
        AssumptionStatus AssumptionChecker<ValueType, ConstantType>::validateAssumptionSMTSolverTwoSucc(uint_fast64_t state1, uint_fast64_t state2, std::shared_ptr<expressions::BinaryRelationExpression> assumption, std::shared_ptr<Order> order, storage::ParameterRegion<ValueType> region, std::vector<ConstantType>const minValues, std::vector<ConstantType>const maxValues) const {
            std::shared_ptr<utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<utility::solver::MathsatSmtSolverFactory>();
            std::shared_ptr<expressions::ExpressionManager> manager(new expressions::ExpressionManager());
            AssumptionStatus result = AssumptionStatus::UNKNOWN;
            auto var1 = assumption->getFirstOperand()->asVariableExpression().getVariableName();
            auto var2 = assumption->getSecondOperand()->asVariableExpression().getVariableName();
            auto row1 = matrix.getRow(state1);
            auto row2 = matrix.getRow(state2);

            STORM_LOG_ASSERT(row1.getNumberOfEntries() == 2 || row2.getNumberOfEntries() == 2, "One of the states should have 2 successors");

            bool swap = false;
            // switch row1 and row2 to make implentation easier
            if (row2.begin()->getColumn() == state1 || ((++row2.begin())->getColumn() == state1)) {
                swap = true;
                std::swap(state1, state2);
                std::swap(var1, var2);
                std::swap(row1, row2);
            }
            // if number of successors of state1 = 2, and one of those successors is state2 we try some inline smtsolving
            // so state1 -> state2 + some other state
            if (!swap && row1.begin()->getColumn() != state2 && (++row1.begin())->getColumn() != state2) {
                return AssumptionStatus::UNKNOWN;
            }

            // --------------------------------------------------------------------------------
            // Expression for the successors of state 1
            // --------------------------------------------------------------------------------
            // Add all variables to the manager
            std::set<expressions::Variable> stateVariables;
            std::set<expressions::Variable> topVariables;
            std::set<expressions::Variable> bottomVariables;
            for (auto itr1 = row1.begin(); itr1 != row1.end(); ++itr1) {
                auto varname1 = "s" + std::to_string(itr1->getColumn());
                if (!manager->hasVariable(varname1)) {
                    if (order->isTopState(itr1->getColumn())) {
                        topVariables.insert(manager->declareRationalVariable(varname1));
                    } else if (order->isBottomState(itr1->getColumn())) {
                        bottomVariables.insert(manager->declareRationalVariable(varname1));
                    } else {
                        stateVariables.insert(manager->declareRationalVariable(varname1));
                    }
                }
            }

            // Add relation on successor states of state1
            expressions::Expression exprOrderSucc = manager->boolean(true);
            for (auto itr = row1.begin(); itr != row1.end(); ++itr) {
                for (auto itr2 = itr+1; itr2 != row1.end(); ++itr2) {
                    auto comp = order->compare(itr->getColumn(), itr2->getColumn());
                    if (comp == Order::NodeComparison::ABOVE) {
                        exprOrderSucc = exprOrderSucc && manager->getVariable("s" + std::to_string(itr->getColumn())) > manager->getVariable("s"+ std::to_string(itr2->getColumn()));
                    }
                    if (comp == Order::NodeComparison::SAME) {
                        exprOrderSucc = exprOrderSucc && manager->getVariable("s" + std::to_string(itr->getColumn())) >= manager->getVariable("s"+ std::to_string(itr2->getColumn())) && manager->getVariable("s" + std::to_string(itr->getColumn())) <= manager->getVariable("s"+ std::to_string(itr2->getColumn()));
                    }
                    if (comp == Order::NodeComparison::BELOW) {
                        exprOrderSucc = exprOrderSucc && manager->getVariable("s"+ std::to_string(itr2->getColumn())) > manager->getVariable("s" + std::to_string(itr->getColumn()));
                    }
                }
            }

            // --------------------------------------------------------------------------------
            // Expressions for the states of the assumption
            // --------------------------------------------------------------------------------
            // state state1 goes with prob f to state state2 and prob 1-f to the other state. Denoted by expr1
            // For state 2 we don't do anything as this is the special case verification
            auto valueTypeToExpression = expressions::RationalFunctionToExpression<ValueType>(manager);
            expressions::Expression expr1;
            expressions::Expression expr2 = manager->getVariable("s" + std::to_string(state2));
            if (rewardModel != nullptr) {
                // We are dealing with a reward property
                if (rewardModel->hasStateActionRewards()) {
                    expr1 = valueTypeToExpression.toExpression(rewardModel->getStateActionReward(state1));
                } else if (rewardModel->hasStateRewards()) {
                    expr1 = valueTypeToExpression.toExpression(rewardModel->getStateReward(state1));
                } else {
                    STORM_LOG_ASSERT(false, "Expecting reward");
                }
            } else {
                // We are dealing with a probability property
                expr1 = manager->rational(0);
                expr2 = manager->rational(0);
            }
            for (auto itr1 = row1.begin(); itr1 != row1.end(); ++itr1) {
                expr1 = expr1 + (valueTypeToExpression.toExpression(itr1->getValue()) *
                                 manager->getVariable("s" + std::to_string(itr1->getColumn())));
            }

            // --------------------------------------------------------------------------------
            // Expression for the bounds on the variables
            // --------------------------------------------------------------------------------
            expressions::Expression exprBounds = manager->boolean(true);

            for (auto& var : manager->getVariables()) {
                if (find(stateVariables.begin(), stateVariables.end(), var) != stateVariables.end()) {
                    // the var is a state
                    if (minValues.size() > 0) {
                        std::string test = var.getName();
                        auto val = std::stoi(test.substr(1,test.size()-1));
                        exprBounds = exprBounds && manager->rational(minValues[val]) <= var &&
                                     var <= manager->rational(maxValues[val]);
                    } else if (rewardModel == nullptr) {
                        // Probability property
                        exprBounds = exprBounds && manager->rational(0) <= var &&
                                     var <= manager->rational(1);
                    } else {
                        // Reward Property
                        exprBounds = exprBounds && manager->rational(0) <= var;
                    }
                } else if (find(topVariables.begin(), topVariables.end(), var) != topVariables.end()) {
                    // the var is =)
                    STORM_LOG_ASSERT(rewardModel == nullptr, "Cannot have top states when also having a reward model");
                    exprBounds = exprBounds && var == manager->rational(1);
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

            // --------------------------------------------------------------------------------
            // Check if the order of the successors + the bounds is satisfiable
            // --------------------------------------------------------------------------------
            solver::Z3SmtSolver s(*manager);
            s.add(exprOrderSucc);
            s.add(exprBounds);
            s.setTimeout(100);
            // assert that sorting of successors in the order and the bounds on the expression are at least satisfiable
            // when this is not the case, the order is invalid
            // however, it could be that the sat solver didn't finish in time, in that case we just continue.
            if (s.check() == solver::SmtSolver::CheckResult::Unsat) {
                STORM_LOG_ASSERT(false, "The order of successors plus the bounds should be satisfiable, probably the order is invalid");
                return AssumptionStatus::INVALID;
            }
            // --------------------------------------------------------------------------------
            // Expression we need to check
            // --------------------------------------------------------------------------------
            // It is the negation of actual assumption
            expressions::Expression exprToCheck;
            if (assumption->getRelationType() == expressions::BinaryRelationExpression::RelationType::Greater) {
                if (swap) {
                    exprToCheck = expr1 >= expr2;
                } else {
                    exprToCheck = expr1 <= expr2;
                }
            } else {
                assert (assumption->getRelationType() == expressions::BinaryRelationExpression::RelationType::Equal);
                exprToCheck = expr1 != expr2;
            }

            s.add(exprToCheck);
            auto smtRes = s.check();
            if (smtRes == solver::SmtSolver::CheckResult::Unsat) {
                // If there is no thing satisfying the negation we are safe.
                return AssumptionStatus::VALID;
            }
            return result;
        }


        template <typename ValueType, typename ConstantType>
        AssumptionStatus AssumptionChecker<ValueType, ConstantType>::validateAssumptionSMTSolver(uint_fast64_t state1, uint_fast64_t state2, std::shared_ptr<expressions::BinaryRelationExpression> assumption, std::shared_ptr<Order> order, storage::ParameterRegion<ValueType> region, std::vector<ConstantType>const minValues, std::vector<ConstantType>const maxValues) const {

            auto var1 = assumption->getFirstOperand()->asVariableExpression().getVariableName();
            auto var2 = assumption->getSecondOperand()->asVariableExpression().getVariableName();
            auto row1 = matrix.getRow(state1);
            auto row2 = matrix.getRow(state2);

            if (row1.getNumberOfEntries() == 2 && row2.getNumberOfEntries() == 2) {
                AssumptionStatus result = validateAssumptionSMTSolverTwoSucc(state1, state2, assumption, order, region, minValues, maxValues);
                if (result != AssumptionStatus::UNKNOWN) {
                    std::cout << *assumption << " Special validation smt"<< std::endl;

                    return result;
                }
            }
            std::cout << *assumption << "Normal validation smt"<< std::endl;

            // if the state with number var1 (var2) occurs in the successors of the state with number var2 (var1) we need to add var1 == expr1 (var2 == expr2) to the bounds
            bool addVar1 = false;
            bool addVar2 = false;

            // Check if the order for all states is known, if this is the case and we find an satisfiable instantion our assumption is invalid.
            bool orderKnown;

            // Start creating expression for order of states
            // We need a new manager as the one from the assumption is const.
            std::shared_ptr<expressions::ExpressionManager> manager(new expressions::ExpressionManager());

            // We keep track of the kind of variables s.t. we can add the bounds later on
            std::set<expressions::Variable> stateVariables;
            std::set<expressions::Variable> topVariables;
            std::set<expressions::Variable> bottomVariables;

            // --------------------------------------------------------------------------------
            // Expression for the successors of our state
            // --------------------------------------------------------------------------------
            auto exprOrderSucc = manager->boolean(true);
            for (auto itr1 = row1.begin(); itr1 != row1.end(); ++itr1) {
                auto varname1 = "s" + std::to_string(itr1->getColumn());
                if (!manager->hasVariable(varname1) ) {
                    if (order->isTopState(itr1->getColumn())) {
                        topVariables.insert(manager->declareRationalVariable(varname1));
                    } else if (order->isBottomState(itr1->getColumn())) {
                        bottomVariables.insert(manager->declareRationalVariable(varname1));
                    } else {
                        stateVariables.insert(manager->declareRationalVariable(varname1));
                    }
                }

                // Check whether we need to add var2 = ... to the expressions.
                addVar2 |= std::to_string(itr1->getColumn()) == var2;

                for (auto itr2 = row2.begin(); itr2 != row2.end(); ++itr2) {
                    // Check whether we need to add var1 = ... to the expressions.
                    addVar1 |= std::to_string(itr2->getColumn()) == var1;
                    // if itr1->getColumn() == itr2->getColumn() we don't need to check for comparison, as this will be SAME
                    if (itr1->getColumn() != itr2->getColumn()) {
                        auto varname2 = "s" + std::to_string(itr2->getColumn());
                        if (!manager->hasVariable(varname2)) {
                            if (order->isTopState(itr2->getColumn())) {
                                topVariables.insert(manager->declareRationalVariable(varname2));
                            } else if (order->isBottomState(itr2->getColumn())) {
                                bottomVariables.insert(manager->declareRationalVariable(varname2));
                            } else {
                                stateVariables.insert(manager->declareRationalVariable(varname2));
                            }
                        }

                        auto comp = order->compare(itr1->getColumn(), itr2->getColumn());
                        if (minValues.size() > 0 && comp == Order::NodeComparison::UNKNOWN) {
                            // Couldn't add relation between varname1 and varname2 but maybe we can based on min/max values;
                            if (minValues[itr2->getColumn()] > maxValues[itr1->getColumn()]) {
                                if (!order->contains(itr1->getColumn())) {
                                    order->add(itr1->getColumn());
                                    order->addStateToHandle(itr1->getColumn());
                                }
                                if (!order->contains(itr2->getColumn())) {
                                    order->add(itr2->getColumn());
                                    order->addStateToHandle(itr2->getColumn());
                                }
                                order->addRelation(itr2->getColumn(), itr1->getColumn());
                                comp = Order::NodeComparison::BELOW;
                            } else if (minValues[itr1->getColumn()] > maxValues[itr2->getColumn()]) {
                                if (!order->contains(itr1->getColumn())) {
                                    order->add(itr1->getColumn());
                                    order->addStateToHandle(itr1->getColumn());
                                }
                                if (!order->contains(itr2->getColumn())) {
                                    order->add(itr2->getColumn());
                                    order->addStateToHandle(itr2->getColumn());
                                }
                                order->addRelation(itr1->getColumn(), itr2->getColumn());
                                comp = Order::NodeComparison::ABOVE;
                            }
                        }
                        if (comp == Order::NodeComparison::ABOVE) {
                            exprOrderSucc = exprOrderSucc && !(manager->getVariable(varname1) <= manager->getVariable(varname2));
                        } else if (comp == Order::NodeComparison::BELOW) {
                            exprOrderSucc = exprOrderSucc && !(manager->getVariable(varname1) >= manager->getVariable(varname2));
                        } else if (comp == Order::NodeComparison::SAME) {
                            exprOrderSucc = exprOrderSucc && (manager->getVariable(varname1) >= manager->getVariable(varname2)) && (manager->getVariable(varname1) <= manager->getVariable(varname2));
                        } else {
                            orderKnown = false;
                        }
                    }
                }
            }


            // --------------------------------------------------------------------------------
            // Expressions for the states of the assumption
            // --------------------------------------------------------------------------------
            auto valueTypeToExpression = expressions::RationalFunctionToExpression<ValueType>(manager);
            expressions::Expression expr1;
            expressions::Expression expr2;
            if (rewardModel != nullptr) {
                // We are dealing with a reward property
                if (rewardModel->hasStateActionRewards()) {
                    expr1 = valueTypeToExpression.toExpression(rewardModel->getStateActionReward(state1));
                    expr2 = valueTypeToExpression.toExpression(rewardModel->getStateActionReward(state2));
                } else if (rewardModel->hasStateRewards()) {
                    expr1 = valueTypeToExpression.toExpression(rewardModel->getStateReward(state1));
                    expr2 = valueTypeToExpression.toExpression(rewardModel->getStateReward(state2));
                } else {
                    STORM_LOG_ASSERT(false, "Expecting reward");
                }

            } else {
                // We are dealing with a probability property
                expr1 = manager->rational(0);
                expr2 = manager->rational(0);
            }
            for (auto itr1 = row1.begin(); itr1 != row1.end(); ++itr1) {
                expr1 = expr1 + (valueTypeToExpression.toExpression(itr1->getValue()) *
                                 manager->getVariable("s" + std::to_string(itr1->getColumn())));
            }

            for (auto itr2 = row2.begin(); itr2 != row2.end(); ++itr2) {
                expr2 = expr2 + (valueTypeToExpression.toExpression(itr2->getValue()) *
                                 manager->getVariable("s" + std::to_string(itr2->getColumn())));
            }

            // --------------------------------------------------------------------------------
            // Expression for the bounds on the variables
            // --------------------------------------------------------------------------------
            auto variables = manager->getVariables();
            // Bounds for the state probabilities and parameters
            expressions::Expression exprBounds = manager->boolean(true);
            if (addVar1) {
                exprBounds = exprBounds && (manager->getVariable("s" + var1) == expr1);
            }
            if (addVar2) {
                exprBounds = exprBounds && (manager->getVariable("s" + var2) == expr2);
            }
            for (auto var : variables) {
                if (find(stateVariables.begin(), stateVariables.end(), var) != stateVariables.end()) {
                    // the var is a state
                    if (minValues.size() > 0) {
                        std::string test = var.getName();
                        auto val = std::stoi(test.substr(1,test.size()-1));
                        std::cout << "Bound from minmax" << std::endl;
                        exprBounds = exprBounds && manager->rational(minValues[val]) <= var &&
                                     var <= manager->rational(maxValues[val]);
                    } else if (rewardModel == nullptr) {
                        // Probability property
                        std::cout << "Bound from probprop" << std::endl;

                        exprBounds = exprBounds && manager->rational(0) <= var &&
                                     var <= manager->rational(1);
                    } else {
                        // Reward Property
                        exprBounds = exprBounds && manager->rational(0) <= var;
                    }
                } else if (find(topVariables.begin(), topVariables.end(), var) != topVariables.end()) {
                    // the var is =)
                    STORM_LOG_ASSERT(rewardModel == nullptr, "topstates not possible if rewards are present");
                    exprBounds = exprBounds && var == manager->rational(1);
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

            // --------------------------------------------------------------------------------
            // Check if the order of the successors + the bounds is satisfiable
            // --------------------------------------------------------------------------------
            solver::Z3SmtSolver s(*manager);
            s.add(exprOrderSucc);
            s.add(exprBounds);
            s.setTimeout(100);
            // assert that sorting of successors in the order and the bounds on the expression are at least satisfiable
            // when this is not the case, the order is invalid
            // however, it could be that the sat solver didn't finish in time, in that case we just continue.
            if (s.check() == solver::SmtSolver::CheckResult::Unsat) {
                STORM_LOG_ASSERT(false, "The order of successors plus the bounds should be satisfiable, probably the order is invalid");
                return AssumptionStatus::INVALID;
            }

            // Create expression for the assumption based on the relation to successors
            // It is the negation of actual assumption

            expressions::Expression exprToCheck;
            if (assumption->getRelationType() == expressions::BinaryRelationExpression::RelationType::Greater) {
                exprToCheck = expr1 <= expr2;
            } else {
                assert (assumption->getRelationType() == expressions::BinaryRelationExpression::RelationType::Equal);
                exprToCheck = expr1 != expr2;
            }

            s.add(exprToCheck);
            auto smtRes = s.check();
            if (smtRes == solver::SmtSolver::CheckResult::Unsat) {
                // If it is unsatisfiable the original assumtpion should be valid
                return AssumptionStatus::VALID;
            } else if (smtRes == solver::SmtSolver::CheckResult::Sat && orderKnown) {
                return AssumptionStatus::INVALID;
            }
            return AssumptionStatus::UNKNOWN;
        }

        template<typename ValueType, typename ConstantType>
        AssumptionStatus AssumptionChecker<ValueType, ConstantType>::validateAssumption(
            std::shared_ptr<expressions::BinaryRelationExpression> assumption, std::shared_ptr<Order> order,
            storage::ParameterRegion<ValueType> region) const {
            auto var1 = std::stoi(assumption->getFirstOperand()->asVariableExpression().getVariableName());
            auto var2 = std::stoi(assumption->getSecondOperand()->asVariableExpression().getVariableName());
            std::vector<ConstantType> vals;
            return validateAssumption(var1, var2, assumption, order, region, vals, vals);
        }
        template<typename ValueType, typename ConstantType>
        void AssumptionChecker<ValueType, ConstantType>::setRewardModel(std::shared_ptr<storm::models::sparse::StandardRewardModel<ValueType>> rewardModel) {
            this->rewardModel = rewardModel;
        }

        template class AssumptionChecker<RationalFunction, double>;
        template class AssumptionChecker<RationalFunction, RationalNumber>;
    }
}
