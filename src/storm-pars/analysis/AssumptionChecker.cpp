#include <storm/solver/Z3SmtSolver.h>
#include "AssumptionChecker.h"

#include "storm-pars/utility/ModelInstantiator.h"
#include "storm-pars/analysis/MonotonicityChecker.h"

#include "storm/environment/Environment.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/modelchecker/CheckTask.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "storm/modelchecker/results/CheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/storage/expressions/SimpleValuation.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/VariableExpression.h"
#include "storm/storage/expressions/RationalFunctionToExpression.h"
#include "storm/utility/constants.h"

namespace storm {
    namespace analysis {
        template <typename ValueType>
        AssumptionChecker<ValueType>::AssumptionChecker(std::shared_ptr<logic::Formula const> formula, std::shared_ptr<models::sparse::Dtmc<ValueType>> model, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t numberOfSamples) {
            this->formula = formula;
            this->matrix = model->getTransitionMatrix();
            this->region = region;

            // Create sample points
            auto instantiator = utility::ModelInstantiator<models::sparse::Dtmc<ValueType>, models::sparse::Dtmc<double>>(*model);
            auto matrix = model->getTransitionMatrix();
            std::set<typename utility::parametric::VariableType<ValueType>::type> variables =  models::sparse::getProbabilityParameters(*model);

            for (uint_fast64_t i = 0; i < numberOfSamples; ++i) {
                auto valuation = utility::parametric::Valuation<ValueType>();
                for (auto var: variables) {
                    auto lb = region.getLowerBoundary(var.name());
                    auto ub = region.getUpperBoundary(var.name());
                    // Creates samples between lb and ub, that is: lb, lb + (ub-lb)/(#samples -1), lb + 2* (ub-lb)/(#samples -1), ..., ub
                    auto val =
                            std::pair<typename utility::parametric::VariableType<ValueType>::type, typename utility::parametric::CoefficientType<ValueType>::type>
                                    (var,utility::convertNumber<typename utility::parametric::CoefficientType<ValueType>::type>(lb + i*(ub-lb)/(numberOfSamples-1)));
                    valuation.insert(val);
                }
                models::sparse::Dtmc<double> sampleModel = instantiator.instantiate(valuation);
                auto checker = modelchecker::SparseDtmcPrctlModelChecker<models::sparse::Dtmc<double>>(sampleModel);
                std::unique_ptr<modelchecker::CheckResult> checkResult;
                if (formula->isProbabilityOperatorFormula() &&
                    formula->asProbabilityOperatorFormula().getSubformula().isUntilFormula()) {
                    const modelchecker::CheckTask<logic::UntilFormula, double> checkTask = modelchecker::CheckTask<logic::UntilFormula, double>(
                            (*formula).asProbabilityOperatorFormula().getSubformula().asUntilFormula());
                    checkResult = checker.computeUntilProbabilities(Environment(), checkTask);
                } else if (formula->isProbabilityOperatorFormula() &&
                           formula->asProbabilityOperatorFormula().getSubformula().isEventuallyFormula()) {
                    const modelchecker::CheckTask<logic::EventuallyFormula, double> checkTask = modelchecker::CheckTask<logic::EventuallyFormula, double>(
                            (*formula).asProbabilityOperatorFormula().getSubformula().asEventuallyFormula());
                    checkResult = checker.computeReachabilityProbabilities(Environment(), checkTask);
                } else {
                    STORM_LOG_THROW(false, exceptions::NotSupportedException,
                                    "Expecting until or eventually formula");
                }
                auto quantitativeResult = checkResult->asExplicitQuantitativeCheckResult<double>();
                std::vector<double> values = quantitativeResult.getValueVector();
                samples.push_back(values);
            }
        }

        template <typename ValueType>
        AssumptionChecker<ValueType>::AssumptionChecker(std::shared_ptr<logic::Formula const> formula, std::shared_ptr<models::sparse::Mdp<ValueType>> model, uint_fast64_t numberOfSamples) {
            STORM_LOG_THROW(false, exceptions::NotImplementedException, "Assumption checking for mdps not yet implemented");
            this->formula = formula;
            this->matrix = model->getTransitionMatrix();

            // Create sample points
            auto instantiator = utility::ModelInstantiator<models::sparse::Mdp<ValueType>, models::sparse::Mdp<double>>(*model);
            auto matrix = model->getTransitionMatrix();
            std::set<typename utility::parametric::VariableType<ValueType>::type> variables =  models::sparse::getProbabilityParameters(*model);

            for (auto i = 0; i < numberOfSamples; ++i) {
                auto valuation = utility::parametric::Valuation<ValueType>();
                for (auto itr = variables.begin(); itr != variables.end(); ++itr) {
                    auto val = std::pair<typename utility::parametric::VariableType<ValueType>::type,
                                        typename utility::parametric::CoefficientType<ValueType>::type>((*itr), utility::convertNumber<typename utility::parametric::CoefficientType<ValueType>::type>(boost::lexical_cast<std::string>((i+1)/(double (numberOfSamples + 1)))));
                    valuation.insert(val);
                }
                models::sparse::Mdp<double> sampleModel = instantiator.instantiate(valuation);
                auto checker = modelchecker::SparseMdpPrctlModelChecker<models::sparse::Mdp<double>>(sampleModel);
                std::unique_ptr<modelchecker::CheckResult> checkResult;
                if (formula->isProbabilityOperatorFormula() &&
                    formula->asProbabilityOperatorFormula().getSubformula().isUntilFormula()) {
                    const modelchecker::CheckTask<logic::UntilFormula, double> checkTask = modelchecker::CheckTask<logic::UntilFormula, double>(
                            (*formula).asProbabilityOperatorFormula().getSubformula().asUntilFormula());
                    checkResult = checker.computeUntilProbabilities(Environment(), checkTask);
                } else if (formula->isProbabilityOperatorFormula() &&
                           formula->asProbabilityOperatorFormula().getSubformula().isEventuallyFormula()) {
                    const modelchecker::CheckTask<logic::EventuallyFormula, double> checkTask = modelchecker::CheckTask<logic::EventuallyFormula, double>(
                            (*formula).asProbabilityOperatorFormula().getSubformula().asEventuallyFormula());
                    checkResult = checker.computeReachabilityProbabilities(Environment(), checkTask);
                } else {
                    STORM_LOG_THROW(false, exceptions::NotSupportedException,
                                    "Expecting until or eventually formula");
                }
                auto quantitativeResult = checkResult->asExplicitQuantitativeCheckResult<double>();
                std::vector<double> values = quantitativeResult.getValueVector();
                samples.push_back(values);
            }
        }

        template <typename ValueType>
        AssumptionStatus AssumptionChecker<ValueType>::checkOnSamples(std::shared_ptr<expressions::BinaryRelationExpression> assumption) {
            auto result = AssumptionStatus::UNKNOWN;
            std::set<expressions::Variable> vars = std::set<expressions::Variable>({});
            assumption->gatherVariables(vars);
            for (auto itr = samples.begin(); result == AssumptionStatus::UNKNOWN && itr != samples.end(); ++itr) {
                std::shared_ptr<expressions::ExpressionManager const> manager = assumption->getManager().getSharedPointer();
                auto valuation = expressions::SimpleValuation(manager);
                auto values = (*itr);
                for (auto var = vars.begin(); result == AssumptionStatus::UNKNOWN && var != vars.end(); ++var) {
                    expressions::Variable par = *var;
                    auto index = std::stoi(par.getName());
                    valuation.setRationalValue(par, values[index]);
                }
                assert(assumption->hasBooleanType());
                if (!assumption->evaluateAsBool(&valuation)) {
                    result = AssumptionStatus::INVALID;
                }
            }
            return result;
        }

        template <typename ValueType>
        AssumptionStatus AssumptionChecker<ValueType>::validateAssumption(std::shared_ptr<expressions::BinaryRelationExpression> assumption, Order* order) {
            // First check if based on sample points the assumption can be discharged
            auto result = checkOnSamples(assumption);
            assert (result != AssumptionStatus::VALID);

            if (result == AssumptionStatus::UNKNOWN) {
                // If result from sample checking was unknown, the assumption might hold, so we continue,
                // otherwise we return INVALID
                std::set<expressions::Variable> vars = std::set<expressions::Variable>({});
                assumption->gatherVariables(vars);

                STORM_LOG_THROW(assumption->getRelationType() ==
                                expressions::BinaryRelationExpression::RelationType::Greater ||
                                assumption->getRelationType() ==
                                expressions::BinaryRelationExpression::RelationType::Equal,
                                exceptions::NotSupportedException,
                                "Only Greater Or Equal assumptions supported");

                // Row with successors of the first state
                auto row1 = matrix.getRow(
                        std::stoi(assumption->getFirstOperand()->asVariableExpression().getVariableName()));
                // Row with successors of the second state
                auto row2 = matrix.getRow(
                        std::stoi(assumption->getSecondOperand()->asVariableExpression().getVariableName()));

                if (row1.getNumberOfEntries() == 2 && row2.getNumberOfEntries() == 2) {
                    // If the states have the same successors for which we know the position in the order
                    // We can check with a function if the assumption holds

                    auto state1succ1 = row1.begin();
                    auto state1succ2 = (++row1.begin());
                    auto state2succ1 = row2.begin();
                    auto state2succ2 = (++row2.begin());

                    if (state1succ1->getColumn() == state2succ2->getColumn()
                        && state2succ1->getColumn() == state1succ2->getColumn()) {
                        std::swap(state1succ1, state1succ2);
                    }

                    if (state1succ1->getColumn() == state2succ1->getColumn() && state1succ2->getColumn() == state2succ2->getColumn()) {
                        if (assumption->getRelationType() == expressions::BinaryRelationExpression::RelationType::Greater
                            && order->compare(state1succ1->getColumn(), state1succ2->getColumn()) != Order::NodeComparison::UNKNOWN) {
                            // The assumption should be the greater assumption
                            // If the result is unknown, we cannot compare, also SMTSolver will not help
                            result = validateAssumptionSMTSolver(assumption, order);

//                            result = validateAssumptionFunction(order, state1succ1, state1succ2, state2succ1,
//                                                                state2succ2);
                        } else if (assumption->getRelationType() == expressions::BinaryRelationExpression::RelationType::Equal) {
                            // The assumption is equal, the successors are the same,
                            // so if the probability of reaching the successors is the same, we have a valid assumption
                            if (state1succ1->getValue() == state2succ1->getValue()) {
                                result = AssumptionStatus::VALID;
                            }
                        } else {
                            result = AssumptionStatus::UNKNOWN;
                        }
                    } else {
                        result = validateAssumptionSMTSolver(assumption, order);
                    }
                } else {
                    result = validateAssumptionSMTSolver(assumption, order);
                }
            }
            return result;
        }

        template <typename ValueType>
        AssumptionStatus AssumptionChecker<ValueType>::validateAssumptionFunction(Order* order,
                typename storage::SparseMatrix<ValueType>::iterator state1succ1,
                typename storage::SparseMatrix<ValueType>::iterator state1succ2,
                typename storage::SparseMatrix<ValueType>::iterator state2succ1,
                typename storage::SparseMatrix<ValueType>::iterator state2succ2) {
            assert((state1succ1->getColumn() == state2succ1->getColumn()
                && state1succ2->getColumn() == state2succ2->getColumn())
                || (state1succ1->getColumn() == state2succ2->getColumn()
                && state1succ2->getColumn() == state2succ1->getColumn()));

            AssumptionStatus result;

            // Calculate the difference in probability for the "highest" successor state
            ValueType prob;
            auto comp = order->compare(state1succ1->getColumn(), state1succ2->getColumn());
            assert (comp == Order::NodeComparison::ABOVE || comp == Order::NodeComparison::BELOW);
            if (comp == Order::NodeComparison::ABOVE) {
                prob = state1succ1->getValue() - state2succ1->getValue();
            } else if (comp == Order::NodeComparison::BELOW) {
                prob = state1succ2->getValue() - state2succ2->getValue();
            }

            auto vars = prob.gatherVariables();

            // If the result in monotone increasing (decreasing), then choose 0 (1) for the substitutions
            // This will give the smallest result
            std::map<typename utility::parametric::VariableType<ValueType>::type, typename utility::parametric::CoefficientType<ValueType>::type> substitutions;
            for (auto var : vars) {
                auto monotonicity = MonotonicityChecker<ValueType>::checkDerivative(prob.derivative(var), region);
                if (monotonicity.first) {
                    // monotone increasing
                    substitutions[var] = 0;
                } else if (monotonicity.second) {
                    // monotone increasing
                    substitutions[var] = 1;
                } else {
                    result = AssumptionStatus::UNKNOWN;
                }
            }

            if (prob.evaluate(substitutions) >= 0) {
                result = AssumptionStatus::VALID;
            }
            return result;
        }


        template <typename ValueType>
        AssumptionStatus AssumptionChecker<ValueType>::validateAssumptionSMTSolver(std::shared_ptr<expressions::BinaryRelationExpression> assumption, Order* order) {
            std::shared_ptr<utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<utility::solver::MathsatSmtSolverFactory>();
            std::shared_ptr<expressions::ExpressionManager> manager(new expressions::ExpressionManager());

            AssumptionStatus result;
            auto var1 = assumption->getFirstOperand()->asVariableExpression().getVariableName();
            auto var2 = assumption->getSecondOperand()->asVariableExpression().getVariableName();
            auto row1 = matrix.getRow(std::stoi(var1));
            auto row2 = matrix.getRow(std::stoi(var2));

            bool orderKnown = true;
            // Check if the order between the different successors is known
            // Also start creating expression for order of states
            expressions::Expression exprOrderSucc = manager->boolean(true);
            std::set<expressions::Variable> stateVariables;
            for (auto itr1 = row1.begin(); orderKnown && itr1 != row1.end(); ++itr1) {
                auto varname1 = "s" + std::to_string(itr1->getColumn());
                if (!manager->hasVariable(varname1)) {
                    stateVariables.insert(manager->declareRationalVariable(varname1));
                }
                for (auto itr2 = row2.begin(); orderKnown && itr2 != row2.end(); ++itr2) {
                    if (itr1->getColumn() != itr2->getColumn()) {
                        auto varname2 = "s" + std::to_string(itr2->getColumn());
                        if (!manager->hasVariable(varname2)) {
                            stateVariables.insert(manager->declareRationalVariable(varname2));
                        }
                        auto comp = order->compare(itr1->getColumn(), itr2->getColumn());
                        if (comp == Order::NodeComparison::ABOVE) {
                            exprOrderSucc = exprOrderSucc && !(manager->getVariable(varname1) <=
                                                              manager->getVariable(varname2));
                        } else if (comp == Order::NodeComparison::BELOW) {
                            exprOrderSucc = exprOrderSucc && !(manager->getVariable(varname1) >=
                                                              manager->getVariable(varname2));
                        } else if (comp == Order::NodeComparison::SAME) {
                            exprOrderSucc = exprOrderSucc &&
                                            (manager->getVariable(varname1) = manager->getVariable(varname2));
                        } else {
                            orderKnown = false;
                        }
                    }
                }
            }

            if (orderKnown) {
                solver::Z3SmtSolver s(*manager);
                auto valueTypeToExpression = expressions::RationalFunctionToExpression<ValueType>(manager);
                expressions::Expression expr1 = manager->rational(0);
                for (auto itr1 = row1.begin(); itr1 != row1.end(); ++itr1) {
                    expr1 = expr1 + (valueTypeToExpression.toExpression(itr1->getValue()) * manager->getVariable("s" + std::to_string(itr1->getColumn())));
                }

                expressions::Expression expr2 = manager->rational(0);
                for (auto itr2 = row2.begin(); itr2 != row2.end(); ++itr2) {
                    expr2 = expr2 + (valueTypeToExpression.toExpression(itr2->getValue()) * manager->getVariable("s" + std::to_string(itr2->getColumn())));
                }

                // Create expression for the assumption based on the relation to successors
                // It is the negation of actual assumption
                expressions::Expression exprToCheck ;
                if (assumption->getRelationType() ==
                    expressions::BinaryRelationExpression::RelationType::Greater) {
                    exprToCheck = expr1 <= expr2;
                } else {
                    assert (assumption->getRelationType() ==
                        expressions::BinaryRelationExpression::RelationType::Equal);
                    exprToCheck = expr1 != expr2 ;
                }

                auto variables = manager->getVariables();
                // Bounds for the state probabilities and parameters
                expressions::Expression exprBounds =  manager->boolean(true);
                for (auto var : variables) {
                    if (find(stateVariables.begin(), stateVariables.end(), var) != stateVariables.end()) {
                        // the var is a state
                        exprBounds = exprBounds && manager->rational(0) <= var && var <= manager->rational(1);
                    } else {
                        // the var is a parameter
                        auto lb = storm::utility::convertNumber<storm::RationalNumber>(region.getLowerBoundary(var.getName()));
                        auto ub = storm::utility::convertNumber<storm::RationalNumber>(region.getUpperBoundary(var.getName()));
                        exprBounds = exprBounds && manager->rational(lb) < var && var < manager->rational(ub);
                    }
                }

                s.add(exprOrderSucc);
                s.add(exprBounds);
                // assert that sorting of successors in the order and the bounds on the expression are at least satisfiable
                assert (s.check() == solver::SmtSolver::CheckResult::Sat);
                s.add(exprToCheck);
                auto smtRes = s.check();
                if (smtRes == solver::SmtSolver::CheckResult::Unsat) {
                    // If there is no thing satisfying the negation we are safe.
                    result = AssumptionStatus::VALID;
                } else if (smtRes == solver::SmtSolver::CheckResult::Sat) {
                    assert (smtRes == solver::SmtSolver::CheckResult::Sat);
                    result = AssumptionStatus::INVALID;
                } else {
                    result = AssumptionStatus::UNKNOWN;
                }
            } else {
                result = AssumptionStatus::UNKNOWN;
            }

            return result;
        }

        template class AssumptionChecker<RationalFunction>;
    }
}
