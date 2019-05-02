//
// Created by Jip Spel on 12.09.18.
//
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
        AssumptionChecker<ValueType>::AssumptionChecker(std::shared_ptr<storm::logic::Formula const> formula, std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> model, uint_fast64_t numberOfSamples) {
            this->formula = formula;
            this->matrix = model->getTransitionMatrix();

            // Create sample points
            auto instantiator = storm::utility::ModelInstantiator<storm::models::sparse::Dtmc<ValueType>, storm::models::sparse::Dtmc<double>>(*model);
            auto matrix = model->getTransitionMatrix();
            std::set<storm::RationalFunctionVariable> variables =  storm::models::sparse::getProbabilityParameters(*model);

            // TODO: sampling part also done in MonotonicityChecker, would be better to use this one instead of creating it again
            for (auto i = 0; i < numberOfSamples; ++i) {
                auto valuation = storm::utility::parametric::Valuation<ValueType>();
                for (auto itr = variables.begin(); itr != variables.end(); ++itr) {
                    // TODO: Type
                    // Creates samples between 0 and 1, 1/(#samples+2), 2/(#samples+2), ..., (#samples+1)/(#samples+2)
                    auto val = std::pair<storm::RationalFunctionVariable, storm::RationalFunctionCoefficient>((*itr), storm::utility::convertNumber<storm::RationalFunctionCoefficient>(boost::lexical_cast<std::string>((i+1)/(double (numberOfSamples + 2)))));
                    valuation.insert(val);
                }
                storm::models::sparse::Dtmc<double> sampleModel = instantiator.instantiate(valuation);
                auto checker = storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<double>>(sampleModel);
                std::unique_ptr<storm::modelchecker::CheckResult> checkResult;
                if (formula->isProbabilityOperatorFormula() &&
                    formula->asProbabilityOperatorFormula().getSubformula().isUntilFormula()) {
                    const storm::modelchecker::CheckTask<storm::logic::UntilFormula, double> checkTask = storm::modelchecker::CheckTask<storm::logic::UntilFormula, double>(
                            (*formula).asProbabilityOperatorFormula().getSubformula().asUntilFormula());
                    checkResult = checker.computeUntilProbabilities(Environment(), checkTask);
                } else if (formula->isProbabilityOperatorFormula() &&
                           formula->asProbabilityOperatorFormula().getSubformula().isEventuallyFormula()) {
                    const storm::modelchecker::CheckTask<storm::logic::EventuallyFormula, double> checkTask = storm::modelchecker::CheckTask<storm::logic::EventuallyFormula, double>(
                            (*formula).asProbabilityOperatorFormula().getSubformula().asEventuallyFormula());
                    checkResult = checker.computeReachabilityProbabilities(Environment(), checkTask);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                                    "Expecting until or eventually formula");
                }
                auto quantitativeResult = checkResult->asExplicitQuantitativeCheckResult<double>();
                std::vector<double> values = quantitativeResult.getValueVector();
                samples.push_back(values);
            }
        }

        template <typename ValueType>
        AssumptionChecker<ValueType>::AssumptionChecker(std::shared_ptr<storm::logic::Formula const> formula, std::shared_ptr<storm::models::sparse::Mdp<ValueType>> model, uint_fast64_t numberOfSamples) {
            this->formula = formula;
            this->matrix = model->getTransitionMatrix();

            // Create sample points
            auto instantiator = storm::utility::ModelInstantiator<storm::models::sparse::Mdp<ValueType>, storm::models::sparse::Mdp<double>>(*model);
            auto matrix = model->getTransitionMatrix();
            std::set<storm::RationalFunctionVariable> variables =  storm::models::sparse::getProbabilityParameters(*model);

            // TODO: sampling part also done in MonotonicityChecker, would be better to use this one instead of creating it again
            for (auto i = 0; i < numberOfSamples; ++i) {
                auto valuation = storm::utility::parametric::Valuation<ValueType>();
                for (auto itr = variables.begin(); itr != variables.end(); ++itr) {
                    // TODO: Type
                    auto val = std::pair<storm::RationalFunctionVariable, storm::RationalFunctionCoefficient>((*itr), storm::utility::convertNumber<storm::RationalFunctionCoefficient>(boost::lexical_cast<std::string>((i+1)/(double (numberOfSamples + 1)))));
                    valuation.insert(val);
                }
                storm::models::sparse::Mdp<double> sampleModel = instantiator.instantiate(valuation);
                auto checker = storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>>(sampleModel);
                std::unique_ptr<storm::modelchecker::CheckResult> checkResult;
                if (formula->isProbabilityOperatorFormula() &&
                    formula->asProbabilityOperatorFormula().getSubformula().isUntilFormula()) {
                    const storm::modelchecker::CheckTask<storm::logic::UntilFormula, double> checkTask = storm::modelchecker::CheckTask<storm::logic::UntilFormula, double>(
                            (*formula).asProbabilityOperatorFormula().getSubformula().asUntilFormula());
                    checkResult = checker.computeUntilProbabilities(Environment(), checkTask);
                } else if (formula->isProbabilityOperatorFormula() &&
                           formula->asProbabilityOperatorFormula().getSubformula().isEventuallyFormula()) {
                    const storm::modelchecker::CheckTask<storm::logic::EventuallyFormula, double> checkTask = storm::modelchecker::CheckTask<storm::logic::EventuallyFormula, double>(
                            (*formula).asProbabilityOperatorFormula().getSubformula().asEventuallyFormula());
                    checkResult = checker.computeReachabilityProbabilities(Environment(), checkTask);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                                    "Expecting until or eventually formula");
                }
                auto quantitativeResult = checkResult->asExplicitQuantitativeCheckResult<double>();
                std::vector<double> values = quantitativeResult.getValueVector();
                samples.push_back(values);
            }
        }

        template <typename ValueType>
        AssumptionStatus AssumptionChecker<ValueType>::checkOnSamples(std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption) {
            auto result = AssumptionStatus::UNKNOWN;
            std::set<storm::expressions::Variable> vars = std::set<storm::expressions::Variable>({});
            assumption->gatherVariables(vars);
            for (auto itr = samples.begin(); result == AssumptionStatus::UNKNOWN && itr != samples.end(); ++itr) {
                std::shared_ptr<storm::expressions::ExpressionManager const> manager = assumption->getManager().getSharedPointer();
                auto valuation = storm::expressions::SimpleValuation(manager);
                auto values = (*itr);
                for (auto var = vars.begin(); result == AssumptionStatus::UNKNOWN && var != vars.end(); ++var) {
                    storm::expressions::Variable par = *var;
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
        AssumptionStatus AssumptionChecker<ValueType>::validateAssumption(std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption, storm::analysis::Lattice* lattice) {
            // First check if based on sample points the assumption can be discharged
            auto result = checkOnSamples(assumption);
            assert (result != storm::analysis::AssumptionStatus::VALID);

            if (result == storm::analysis::AssumptionStatus::UNKNOWN) {
                // If result from sample checking was unknown, the assumption might hold, so we continue,
                // otherwise we return INVALID
                std::set<storm::expressions::Variable> vars = std::set<storm::expressions::Variable>({});
                assumption->gatherVariables(vars);

                STORM_LOG_THROW(assumption->getRelationType() ==
                                storm::expressions::BinaryRelationExpression::RelationType::Greater ||
                                assumption->getRelationType() ==
                                storm::expressions::BinaryRelationExpression::RelationType::Equal,
                                storm::exceptions::NotSupportedException,
                                "Only Greater Or Equal assumptions supported");

                // Row with successors of the first state
                auto row1 = matrix.getRow(
                        std::stoi(assumption->getFirstOperand()->asVariableExpression().getVariableName()));
                // Row with successors of the second state
                auto row2 = matrix.getRow(
                        std::stoi(assumption->getSecondOperand()->asVariableExpression().getVariableName()));

                if (row1.getNumberOfEntries() == 2 && row2.getNumberOfEntries() == 2) {
                    // If the states have the same successors for which we know the position in the lattice
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
                        if (assumption->getRelationType() == storm::expressions::BinaryRelationExpression::RelationType::Greater
                            && lattice->compare(state1succ1->getColumn(), state1succ2->getColumn()) != storm::analysis::Lattice::UNKNOWN) {
                            // The assumption should be the greater assumption
                            // If the result is unknown, we cannot compare, also SMTSolver will not help
                            result = validateAssumptionFunction(lattice, state1succ1, state1succ2, state2succ1,
                                                                state2succ2);
                        } else if (assumption->getRelationType() == storm::expressions::BinaryRelationExpression::RelationType::Equal) {
                            // The assumption is equal, the successors are the same,
                            // so if the probability of reaching the successors is the same, we have a valid assumption
                            if (state1succ1->getValue() == state2succ1->getValue()) {
                                result = AssumptionStatus::VALID;
                            }
                        } else {
                            result = AssumptionStatus::UNKNOWN;
                        }
                    } else {
                        result = validateAssumptionSMTSolver(lattice, assumption);
                    }
                } else {
                    result = validateAssumptionSMTSolver(lattice, assumption);
                }
            }
            return result;
        }

        template <typename ValueType>
        AssumptionStatus AssumptionChecker<ValueType>::validateAssumptionFunction(storm::analysis::Lattice* lattice,
                typename storm::storage::SparseMatrix<ValueType>::iterator state1succ1,
                typename storm::storage::SparseMatrix<ValueType>::iterator state1succ2,
                typename storm::storage::SparseMatrix<ValueType>::iterator state2succ1,
                typename storm::storage::SparseMatrix<ValueType>::iterator state2succ2) {
            assert((state1succ1->getColumn() == state2succ1->getColumn()
                && state1succ2->getColumn() == state2succ2->getColumn())
                || (state1succ1->getColumn() == state2succ2->getColumn()
                && state1succ2->getColumn() == state2succ1->getColumn()));

            AssumptionStatus result;

            // Calculate the difference in probability for the "highest" successor state
            ValueType prob;
            auto comp = lattice->compare(state1succ1->getColumn(), state1succ2->getColumn());
            assert (comp == storm::analysis::Lattice::ABOVE || comp == storm::analysis::Lattice::BELOW);
            if (comp == storm::analysis::Lattice::ABOVE) {
                prob = state1succ1->getValue() - state2succ1->getValue();
            } else if (comp == storm::analysis::Lattice::BELOW) {
                prob = state1succ2->getValue() - state2succ2->getValue();
            }

            auto vars = prob.gatherVariables();

            // If the result in monotone increasing (decreasing), then choose 0 (1) for the substitutions
            // This will give the smallest result
            // TODO: Type
            std::map<storm::RationalFunctionVariable, typename ValueType::CoeffType> substitutions;
            for (auto var : vars) {
                auto monotonicity = MonotonicityChecker<ValueType>::checkDerivative(prob.derivative(var));
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
        AssumptionStatus AssumptionChecker<ValueType>::validateAssumptionSMTSolver(storm::analysis::Lattice* lattice, std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption) {
            std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();
            std::shared_ptr<storm::expressions::ExpressionManager> manager(new storm::expressions::ExpressionManager());

            AssumptionStatus result;
            auto var1 = assumption->getFirstOperand()->asVariableExpression().getVariableName();
            auto var2 = assumption->getSecondOperand()->asVariableExpression().getVariableName();
            auto row1 = matrix.getRow(std::stoi(var1));
            auto row2 = matrix.getRow(std::stoi(var2));

            bool orderKnown = true;
            // Check if the order between the different successors is known
            // Also start creating expression for order of states
            storm::expressions::Expression exprOrderSucc = manager->boolean(true);
            std::set<storm::expressions::Variable> stateVariables;
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
                        auto comp = lattice->compare(itr1->getColumn(), itr2->getColumn());
                        if (comp == storm::analysis::Lattice::ABOVE) {
                            exprOrderSucc = exprOrderSucc && !(manager->getVariable(varname1) <=
                                                              manager->getVariable(varname2));
                        } else if (comp == storm::analysis::Lattice::BELOW) {
                            exprOrderSucc = exprOrderSucc && !(manager->getVariable(varname1) >=
                                                              manager->getVariable(varname2));
                        } else if (comp == storm::analysis::Lattice::SAME) {
                            exprOrderSucc = exprOrderSucc &&
                                            (manager->getVariable(varname1) = manager->getVariable(varname2));
                        } else {
                            orderKnown = false;
                        }
                    }
                }
            }

            if (orderKnown) {
                storm::solver::Z3SmtSolver s(*manager);
                auto valueTypeToExpression = storm::expressions::RationalFunctionToExpression<ValueType>(manager);
                storm::expressions::Expression expr1 = manager->rational(0);
                for (auto itr1 = row1.begin(); itr1 != row1.end(); ++itr1) {
                    expr1 = expr1 + (valueTypeToExpression.toExpression(itr1->getValue()) * manager->getVariable("s" + std::to_string(itr1->getColumn())));
                }

                storm::expressions::Expression expr2 = manager->rational(0);
                for (auto itr2 = row2.begin(); itr2 != row2.end(); ++itr2) {
                    expr2 = expr2 + (valueTypeToExpression.toExpression(itr2->getValue()) * manager->getVariable("s" + std::to_string(itr2->getColumn())));
                }

                // Create expression for the assumption based on the relation to successors
                // It is the negation of actual assumption
                // TODO: use same manager s.t. assumption can be used directly ?
                storm::expressions::Expression exprToCheck ;
                if (assumption->getRelationType() ==
                    storm::expressions::BinaryRelationExpression::RelationType::Greater) {
                    exprToCheck = expr1 <= expr2;
                } else {
                    assert (assumption->getRelationType() ==
                        storm::expressions::BinaryRelationExpression::RelationType::Equal);
                    exprToCheck = expr1 > expr2;
                }

                auto variables = manager->getVariables();
                // Bounds for the state probabilities and parameters
                storm::expressions::Expression exprBounds =  manager->boolean(true);
                for (auto var : variables) {
                    if (find(stateVariables.begin(), stateVariables.end(), var) != stateVariables.end()) {
                        // the var is a state
                        exprBounds = exprBounds && manager->rational(0) <= var && var <= manager->rational(1);
                    } else {
                        // the var is a parameter
                        // TODO: graph-preserving
                        exprBounds = exprBounds && manager->rational(0) < var && var < manager->rational(1);
                    }
                }

                s.add(exprOrderSucc);
                s.add(exprBounds);
                // assert that sorting of successors in the lattice and the bounds on the expression are at least satisfiable
                assert (s.check() == storm::solver::SmtSolver::CheckResult::Sat);
                // TODO: kijken of t SAT moet zijn?
                s.add(exprToCheck);
                auto smtRes = s.check();
                if (smtRes == storm::solver::SmtSolver::CheckResult::Unsat) {
                    // If there is no thing satisfying the negation we are safe.
                    result = AssumptionStatus::VALID;
                } else if (smtRes == storm::solver::SmtSolver::CheckResult::Sat) {
                    assert (smtRes == storm::solver::SmtSolver::CheckResult::Sat);
                    result = AssumptionStatus::INVALID;
                } else {
                    result = AssumptionStatus::UNKNOWN;
                }
            } else {
                result = AssumptionStatus::UNKNOWN;
            }

            return result;
        }

        template class AssumptionChecker<storm::RationalFunction>;
    }
}
