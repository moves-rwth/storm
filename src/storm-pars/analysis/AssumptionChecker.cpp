//
// Created by Jip Spel on 12.09.18.
//
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
                    auto val = std::pair<storm::RationalFunctionVariable, storm::RationalFunctionCoefficient>((*itr), storm::utility::convertNumber<storm::RationalFunctionCoefficient>(boost::lexical_cast<std::string>((i+1)/(double (numberOfSamples + 1)))));
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
                                storm::expressions::BinaryRelationExpression::RelationType::GreaterOrEqual,
                                storm::exceptions::NotSupportedException,
                                "Only Greater Or Equal assumptions supported");

                auto row1 = matrix.getRow(std::stoi(assumption->getFirstOperand()->asVariableExpression().getVariableName()));
                auto row2 = matrix.getRow(std::stoi(assumption->getSecondOperand()->asVariableExpression().getVariableName()));

                // Only implemented for two successors
                if (row1.getNumberOfEntries() == 2 && row2.getNumberOfEntries() == 2) {
                    result = validateAssumptionOnFunction(lattice, row1, row2);
                }
            }
            if (result) {
                validatedAssumptions.insert(assumption);
            } else {
                STORM_LOG_DEBUG("Could not validate: " << *assumption << std::endl);
            }
            return result;
        }

        template <typename ValueType>
        bool AssumptionChecker<ValueType>::validateAssumptionOnFunction(storm::analysis::Lattice* lattice, typename storm::storage::SparseMatrix<ValueType>::rows row1, typename storm::storage::SparseMatrix<ValueType>::rows row2) {
            bool result = false;
            auto succ1State1 = row1.begin();
            auto succ2State1 = (++row1.begin());
            auto succ1State2 = row2.begin();
            auto succ2State2 = (++row2.begin());

            if (succ1State1->getColumn() == succ2State2->getColumn()
                    && succ1State2->getColumn() == succ2State1->getColumn()) {
                // swap them
                auto temp = succ2State1;
                succ2State1 = succ1State1;
                succ1State1 = temp;
            }

            if (succ1State1->getColumn() == succ1State2->getColumn() && succ2State1->getColumn() == succ2State2->getColumn()) {
                ValueType prob;
                auto comp = lattice->compare(succ1State1->getColumn(), succ2State1->getColumn());
                if (comp == storm::analysis::Lattice::ABOVE) {
                    prob = succ1State1->getValue() - succ1State2->getValue();
                } else if (comp == storm::analysis::Lattice::BELOW) {
                    prob = succ2State1->getValue() - succ2State2->getValue();
                }
                auto vars = prob.gatherVariables();
                // TODO: Type
                std::map<storm::RationalFunctionVariable, storm::RationalFunctionCoefficient> substitutions;
                for (auto var:vars) {
                    auto derivative = prob.derivative(var);
                    assert(derivative.isConstant());
                    if (derivative.constantPart() >= 0) {
                        substitutions[var] = 0;
                    } else if (derivative.constantPart() <= 0) {
                        substitutions[var] = 1;
                    }
                }
                result = prob.evaluate(substitutions) >= 0;
            }
            return result;
        }

        template <typename ValueType>
        bool AssumptionChecker<ValueType>::validated(std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption) {
            return find(validatedAssumptions.begin(), validatedAssumptions.end(), assumption) != validatedAssumptions.end();
        }

        template class AssumptionChecker<storm::RationalFunction>;
    }
}
