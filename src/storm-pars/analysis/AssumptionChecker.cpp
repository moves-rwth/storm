//
// Created by Jip Spel on 12.09.18.
//
#include <storm/solver/Z3SmtSolver.h>
#include "AssumptionChecker.h"

#include "storm-pars/utility/ModelInstantiator.h"

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
#include "storm/utility/constants.h"
#include "storm/storage/expressions/RationalFunctionToExpression.h"

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
                results.push_back(values);
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
                results.push_back(values);
            }
        }

        template <typename ValueType>
        bool AssumptionChecker<ValueType>::checkOnSamples(std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption) {
            bool result = true;
            std::set<storm::expressions::Variable> vars = std::set<storm::expressions::Variable>({});
            assumption->gatherVariables(vars);
            for (auto itr = results.begin(); result && itr != results.end(); ++itr) {
                std::shared_ptr<storm::expressions::ExpressionManager const> manager = assumption->getManager().getSharedPointer();
                auto valuation = storm::expressions::SimpleValuation(manager);
                auto values = (*itr);
                for (auto var = vars.begin(); result && var != vars.end(); ++var) {
                    storm::expressions::Variable par = *var;
                    auto index = std::stoi(par.getName());
                    valuation.setRationalValue(par, values[index]);
                }
                assert(assumption->hasBooleanType());
                result &= assumption->evaluateAsBool(&valuation);
            }
            return result;
        }

        template <typename ValueType>
        bool AssumptionChecker<ValueType>::validateAssumption(std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption, storm::analysis::Lattice* lattice) {
            bool result = validated(assumption);
            if (!result) {
                std::set<storm::expressions::Variable> vars = std::set<storm::expressions::Variable>({});
                assumption->gatherVariables(vars);

                STORM_LOG_THROW(assumption->getRelationType() ==
                                storm::expressions::BinaryRelationExpression::RelationType::Greater ||assumption->getRelationType() ==
                                                                                                             storm::expressions::BinaryRelationExpression::RelationType::Equal,
                                storm::exceptions::NotSupportedException,
                                "Only Greater Or Equal assumptions supported");

                // TODO: implement validation of equal/greater equations
                auto row1 = matrix.getRow(std::stoi(assumption->getFirstOperand()->asVariableExpression().getVariableName()));
                auto row2 = matrix.getRow(std::stoi(assumption->getSecondOperand()->asVariableExpression().getVariableName()));

                if (row1.getNumberOfEntries() == 2 && row2.getNumberOfEntries() == 2) {
                    auto state1succ1 = row1.begin();
                    auto state1succ2 = (++row1.begin());
                    auto state2succ1 = row2.begin();
                    auto state2succ2 = (++row2.begin());

                    if (state1succ1->getColumn() == state2succ2->getColumn()
                        && state2succ1->getColumn() == state1succ2->getColumn()) {
                        std::swap(state1succ1, state1succ2);
                    }

                    if (state1succ1->getColumn() == state2succ1->getColumn() && state1succ2->getColumn() == state2succ2->getColumn()) {
                        auto comp = lattice->compare(state1succ1->getColumn(), state1succ2->getColumn());
                        if (comp != storm::analysis::Lattice::UNKNOWN) {
                            result = validateAssumptionFunction(lattice, state1succ1, state1succ2, state2succ1,
                                                                state2succ2);

                        if (!result) {
                            result = validateAssumptionSMTSolver(lattice, state1succ1, state1succ2, state2succ1,
                                                                 state2succ2);
                        }

                            validatedAssumptions.insert(assumption);
                            if (result) {
                                validAssumptions.insert(assumption);
                            }
                        }
                    }
                } else {
                    bool subset = true;

                    if (row1.getNumberOfEntries() > row2.getNumberOfEntries()) {
                        std::swap(row1, row2);
                    }
                    storm::storage::BitVector stateNumbers(matrix.getColumnCount());
                    for (auto itr1 = row1.begin(); subset && itr1 != row1.end(); ++itr1) {
                        bool found = false;
                        stateNumbers.set(itr1->getColumn());
                        for (auto itr2 = row2.begin(); !found && itr2 != row2.end(); ++itr2) {
                            found = itr1->getColumn() == itr2->getColumn();
                        }
                        subset &= found;
                    }

                    if (subset) {
                        // Check if they all are in the lattice
                        bool allInLattice = true;
                        for (auto i = stateNumbers.getNextSetIndex(0); allInLattice && i < stateNumbers.size(); i = stateNumbers.getNextSetIndex(i+1)) {
                            for (auto j = stateNumbers.getNextSetIndex(i+1); allInLattice && j < stateNumbers.size(); j = stateNumbers.getNextSetIndex(j+1)) {
                                auto comp = lattice->compare(i,j);
                                allInLattice &= comp == storm::analysis::Lattice::ABOVE || comp == storm::analysis::Lattice::BELOW || comp == storm::analysis::Lattice::SAME;
                            }
                        }

                        if (allInLattice) {
                            result = validateAssumptionSMTSolver(lattice, assumption);
                            validatedAssumptions.insert(assumption);
                            if (result) {
                                validAssumptions.insert(assumption);
                            }
                        }
                    }
                }
            }
            return result;
        }

        template <typename ValueType>
        bool AssumptionChecker<ValueType>::validateAssumptionFunction(storm::analysis::Lattice* lattice,
                typename storm::storage::SparseMatrix<ValueType>::iterator state1succ1,
                typename storm::storage::SparseMatrix<ValueType>::iterator state1succ2,
                typename storm::storage::SparseMatrix<ValueType>::iterator state2succ1,
                typename storm::storage::SparseMatrix<ValueType>::iterator state2succ2) {
            assert((state1succ1->getColumn() == state2succ1->getColumn()
                && state1succ2->getColumn() == state2succ2->getColumn())
                || (state1succ1->getColumn() == state2succ2->getColumn()
                && state1succ2->getColumn() == state2succ1->getColumn()));

            bool result = true;

            ValueType prob;
            auto comp = lattice->compare(state1succ1->getColumn(), state1succ2->getColumn());
            assert (comp == storm::analysis::Lattice::ABOVE || comp == storm::analysis::Lattice::BELOW);
            if (comp == storm::analysis::Lattice::ABOVE) {
                prob = state1succ1->getValue() - state2succ1->getValue();
            } else if (comp == storm::analysis::Lattice::BELOW) {
                prob = state1succ2->getValue() - state2succ2->getValue();
            }

            auto vars = prob.gatherVariables();
            
            // TODO: Type
            std::map<storm::RationalFunctionVariable, typename ValueType::CoeffType> substitutions;
            for (auto var : vars) {
                auto derivative = prob.derivative(var);
                if(derivative.isConstant()) {
                    if (derivative.constantPart() >= 0) {
                        substitutions[var] = 0;
                    } else if (derivative.constantPart() <= 0) {
                        substitutions[var] = 1;
                    }
                } else {
                    result = false;
                }
            }
//            return result && prob.evaluate(substitutions) >= 0;
//TODO check for > and =
            return false;
        }

        template <typename ValueType>
        bool AssumptionChecker<ValueType>::validateAssumptionSMTSolver(storm::analysis::Lattice* lattice,
                                                                       typename storm::storage::SparseMatrix<ValueType>::iterator state1succ1,
                                                                       typename storm::storage::SparseMatrix<ValueType>::iterator state1succ2,
                                                                       typename storm::storage::SparseMatrix<ValueType>::iterator state2succ1,
                                                                       typename storm::storage::SparseMatrix<ValueType>::iterator state2succ2) {
            assert((state1succ1->getColumn() == state2succ1->getColumn()
                    && state1succ2->getColumn() == state2succ2->getColumn())
                   || (state1succ1->getColumn() == state2succ2->getColumn()
                       && state1succ2->getColumn() == state2succ1->getColumn()));
            std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();
            std::shared_ptr<storm::expressions::ExpressionManager> manager(
                    new storm::expressions::ExpressionManager());

            storm::solver::Z3SmtSolver s(*manager);
            storm::solver::SmtSolver::CheckResult smtResult = storm::solver::SmtSolver::CheckResult::Unknown;

            storm::expressions::Variable succ1 = manager->declareRationalVariable(std::to_string(state1succ1->getColumn()));
            storm::expressions::Variable succ2 = manager->declareRationalVariable(std::to_string(state1succ2->getColumn()));
            auto comp = lattice->compare(state1succ1->getColumn(), state1succ2->getColumn());

            storm::expressions::Expression exprGiven;
            if (comp == storm::analysis::Lattice::ABOVE) {
                exprGiven = succ1 >= succ2;
            } else if (comp == storm::analysis::Lattice::BELOW) {
                exprGiven = succ1 <= succ2;
            } else {
                assert (comp != storm::analysis::Lattice::UNKNOWN);
                exprGiven = succ1 = succ2;
            }

            auto valueTypeToExpression = storm::expressions::RationalFunctionToExpression<ValueType>(manager);
            storm::expressions::Expression exprToCheck =
                    (valueTypeToExpression.toExpression(state1succ1->getValue())*succ1
                     + valueTypeToExpression.toExpression(state1succ2->getValue())*succ2
                     >= valueTypeToExpression.toExpression(state2succ1->getValue())*succ1
                        + valueTypeToExpression.toExpression(state2succ2->getValue())*succ2);

            storm::expressions::Expression exprBounds = manager->boolean(true);
            auto variables = manager->getVariables();
            for (auto var : variables) {
                if (var != succ1 && var != succ2) {
                    // ensure graph-preserving
                    exprBounds = exprBounds && manager->rational(0) <= var && manager->rational(1) >= var;
                } else {
                    exprBounds = exprBounds && var >= manager->rational(0) && var <= manager->rational(1);
                }
            }

            s.add(exprGiven);
            s.add(exprToCheck);
            s.add(exprBounds);
            smtResult = s.check();

//            return smtResult == storm::solver::SmtSolver::CheckResult::Sat;
//TODO check for > and =
            return false;
        }

        template <typename ValueType>
        bool AssumptionChecker<ValueType>::validateAssumptionSMTSolver(storm::analysis::Lattice* lattice, std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption) {
            assert (!validated(assumption));

            std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();
            std::shared_ptr<storm::expressions::ExpressionManager> manager(new storm::expressions::ExpressionManager());

            bool result = true;
            auto row1 = matrix.getRow(std::stoi(assumption->getFirstOperand()->asVariableExpression().getVariableName()));
            auto row2 = matrix.getRow(std::stoi(assumption->getSecondOperand()->asVariableExpression().getVariableName()));

            storm::solver::Z3SmtSolver s(*manager);
            std::set<storm::expressions::Variable> stateVariables;
            if (row1.getNumberOfEntries() >= row2.getNumberOfEntries()) {
                for (auto itr = row1.begin(); itr != row1.end(); ++itr) {
                    stateVariables.insert(manager->declareRationalVariable(std::to_string(itr->getColumn())));
                }
            } else {
                for (auto itr = row2.begin(); itr != row2.end(); ++itr) {
                    stateVariables.insert(manager->declareRationalVariable(std::to_string(itr->getColumn())));
                }
            }

            storm::expressions::Expression exprGiven = manager->boolean(true);

            for (auto itr1 = row1.begin(); result && itr1 != row1.end(); ++itr1) {
                for (auto itr2 = row1.begin(); result && itr2 != row1.end(); ++itr2) {
                    if (itr1->getColumn() != itr2->getColumn()) {
                        auto comp = lattice->compare(itr1->getColumn(), itr2->getColumn());

                        if (comp == storm::analysis::Lattice::ABOVE) {
                            exprGiven = exprGiven && (manager->getVariable(std::to_string(itr1->getColumn())) >=
                                                      manager->getVariable(std::to_string(itr2->getColumn())));
                        } else if (comp == storm::analysis::Lattice::BELOW) {
                            exprGiven = exprGiven && (manager->getVariable(std::to_string(itr1->getColumn())) <=
                                                      manager->getVariable(std::to_string(itr2->getColumn())));
                        } else {
                            assert (comp != storm::analysis::Lattice::UNKNOWN);
                            exprGiven = exprGiven &&
                                        (manager->getVariable(std::to_string(itr1->getColumn())) = manager->getVariable(
                                                std::to_string(itr2->getColumn())));
                        }
                    }
                }
            }

            auto valueTypeToExpression = storm::expressions::RationalFunctionToExpression<ValueType>(manager);
            storm::expressions::Expression expr1 = manager->rational(0);
            for (auto itr1 = row1.begin(); result && itr1 != row1.end(); ++itr1) {
                expr1 = expr1 + (valueTypeToExpression.toExpression(itr1->getValue()) * manager->getVariable(std::to_string(itr1->getColumn())));
            }

            storm::expressions::Expression expr2 = manager->rational(0);
            for (auto itr2 = row2.begin(); result && itr2 != row2.end(); ++itr2) {
                expr2 = expr2 + (valueTypeToExpression.toExpression(itr2->getValue()) * manager->getVariable(std::to_string(itr2->getColumn())));
            }
            storm::expressions::Expression exprToCheck = expr1 >= expr2;

            storm::expressions::Expression exprProb1 = manager->rational(0);
            for (auto itr1 = row1.begin(); result && itr1 != row1.end(); ++itr1) {
                exprProb1 = exprProb1 + (valueTypeToExpression.toExpression(itr1->getValue()));
            }

            storm::expressions::Expression exprProb2 = manager->rational(0);
            for (auto itr2 = row2.begin(); result && itr2 != row2.end(); ++itr2) {
                exprProb2 = exprProb2 + (valueTypeToExpression.toExpression(itr2->getValue()));
            }

            storm::expressions::Expression exprBounds = exprProb1 >= manager->rational(0)
                         && exprProb1 <= manager->rational(1)
                         && exprProb2 >= manager->rational(0)
                         && exprProb2 <= manager->rational(1);

            auto variables = manager->getVariables();
            for (auto var : variables) {
                if (find(stateVariables.begin(), stateVariables.end(), var) != stateVariables.end()) {
                    // ensure graph-preserving
                    exprBounds = exprBounds && manager->rational(0) <= var && manager->rational(1) >= var;
                } else {
                    exprBounds = exprBounds && var >= manager->rational(0) && var <= manager->rational(1);
                }
            }

            s.add(exprGiven);
            s.add(exprBounds);
            assert(s.check() == storm::solver::SmtSolver::CheckResult::Sat);
            s.add(exprToCheck);
            auto smtRes = s.check();
            //TODO check for > and =
            result = result &&  smtRes == storm::solver::SmtSolver::CheckResult::Sat;
            return false;
        }

        template <typename ValueType>
        bool AssumptionChecker<ValueType>::validated(std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption) {
            return find(validatedAssumptions.begin(), validatedAssumptions.end(), assumption) != validatedAssumptions.end();
        }

        template <typename ValueType>
        bool AssumptionChecker<ValueType>::valid(std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption) {
            assert(find(validatedAssumptions.begin(), validatedAssumptions.end(), assumption) != validatedAssumptions.end());
            return find(validAssumptions.begin(), validAssumptions.end(), assumption) != validAssumptions.end();
        }

        template class AssumptionChecker<storm::RationalFunction>;
    }
}
