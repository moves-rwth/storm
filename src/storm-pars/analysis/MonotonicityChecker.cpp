//
// Created by Jip Spel on 05.09.18.
//

#include "MonotonicityChecker.h"
#include "storm-pars/analysis/AssumptionMaker.h"
#include "storm-pars/analysis/AssumptionChecker.h"
#include "storm-pars/analysis/Lattice.h"
#include "storm-pars/analysis/LatticeExtender.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/InvalidOperationException.h"

#include "storm/utility/Stopwatch.h"
#include "storm/models/ModelType.h"

#include "storm/modelchecker/results/CheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/solver/Z3SmtSolver.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/storage/expressions/RationalFunctionToExpression.h"


namespace storm {
    namespace analysis {
        template <typename ValueType>
        MonotonicityChecker<ValueType>::MonotonicityChecker(std::shared_ptr<storm::models::ModelBase> model, std::vector<std::shared_ptr<storm::logic::Formula const>> formulas, bool validate) {
            outfile.open(filename, std::ios_base::app);
            this->model = model;
            this->formulas = formulas;
            this->validate = validate;
            this->resultCheckOnSamples = std::map<carl::Variable, std::pair<bool, bool>>();
            if (model != nullptr) {
                std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel = model->as<storm::models::sparse::Model<ValueType>>();
                this->extender = new storm::analysis::LatticeExtender<ValueType>(sparseModel);
                outfile << model->getNumberOfStates() << "; " << model->getNumberOfTransitions() << "; ";
            }
            outfile.close();
            totalWatch = storm::utility::Stopwatch(true);
        }

        template <typename ValueType>
        std::map<storm::analysis::Lattice*, std::map<carl::Variable, std::pair<bool, bool>>> MonotonicityChecker<ValueType>::checkMonotonicity() {
            totalWatch = storm::utility::Stopwatch(true);
            // TODO: check on samples or not?
            auto map = createLattice();
            std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel = model->as<storm::models::sparse::Model<ValueType>>();
            auto matrix = sparseModel->getTransitionMatrix();
            return checkMonotonicity(map, matrix);
        }

        template <typename ValueType>
        std::map<storm::analysis::Lattice*, std::map<carl::Variable, std::pair<bool, bool>>> MonotonicityChecker<ValueType>::checkMonotonicity(std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> map, storm::storage::SparseMatrix<ValueType> matrix) {
            storm::utility::Stopwatch finalCheckWatch(true);
            std::map<storm::analysis::Lattice *, std::map<carl::Variable, std::pair<bool, bool>>> result;


            if (map.size() == 0) {
                outfile.open(filename, std::ios_base::app);
                outfile << " | No assumptions, ? | ;";
                outfile.close();
                STORM_PRINT(std::endl << "Do not know about monotonicity" << std::endl);
            } else {
                auto i = 0;
                for (auto itr = map.begin(); i < map.size() && itr != map.end(); ++itr) {
                    auto lattice = itr->first;
                    outfile.open(filename, std::ios_base::app);
                    outfile << "|";
                    outfile.close();
                    std::map<carl::Variable, std::pair<bool, bool>> varsMonotone = analyseMonotonicity(i, lattice,
                                                                                                      matrix);

                    outfile.open(filename, std::ios_base::app);
                    auto assumptions = itr->second;
                    if (assumptions.size() > 0) {
                        STORM_PRINT("Given assumptions: " << std::endl);
                        bool first = true;
                        for (auto itr2 = assumptions.begin(); itr2 != assumptions.end(); ++itr2) {
                            if (!first) {
                                STORM_PRINT(" ^ ");
                                outfile << (" ^ ");
                            } else {
                                STORM_PRINT("    ");
                                first = false;
                            }

                            std::shared_ptr<storm::expressions::BinaryRelationExpression> expression = *itr2;
                            auto var1 = expression->getFirstOperand();
                            auto var2 = expression->getSecondOperand();
                            STORM_PRINT(*expression);
                            outfile << (*expression);
                        }
                        STORM_PRINT(std::endl);
                        outfile << ", ";
                    } else {
                        outfile << "No assumptions, ";
                    }


                    if (varsMonotone.size() == 0) {
                        STORM_PRINT("Result is constant" << std::endl);
                        outfile << "No params";
                    } else {
                        auto itr2 = varsMonotone.begin();
                        while (itr2 != varsMonotone.end()) {
                            if (resultCheckOnSamples.find(itr2->first) != resultCheckOnSamples.end() &&
                                (!resultCheckOnSamples[itr2->first].first &&
                                 !resultCheckOnSamples[itr2->first].second)) {
                                STORM_PRINT("  - Not monotone in: " << itr2->first << std::endl);
                                outfile << "X " << itr2->first;
                            } else {
                                if (itr2->second.first && itr2->second.second) {
                                    STORM_PRINT("  - Constant in" << itr2->first << std::endl);
                                    outfile << "C " << itr2->first;
                                } else if (itr2->second.first) {
                                    STORM_PRINT("  - Monotone increasing in: " << itr2->first << std::endl);
                                    outfile << "I " << itr2->first;
                                } else if (itr2->second.second) {
                                    STORM_PRINT("  - Monotone decreasing in: " << itr2->first << std::endl);
                                    outfile << "D " << itr2->first;
                                } else {
                                    STORM_PRINT(
                                            "  - Do not know if monotone incr/decreasing in: " << itr2->first << std::endl);
                                    outfile << "? " << itr2->first;
                                }
                            }
                            ++itr2;
                            if (itr2 != varsMonotone.end()) {
                                outfile << ", ";
                            }
                        }
                        result.insert(
                                std::pair<storm::analysis::Lattice *, std::map<carl::Variable, std::pair<bool, bool>>>(
                                        lattice, varsMonotone));
                    }
                    outfile << "| ";
                    outfile.close();
                    ++i;
                }
            }

            finalCheckWatch.stop();
            STORM_PRINT(std::endl << "Time for monotonicity check on lattice: " << finalCheckWatch << "." << std::endl << std::endl);
            outfile.open(filename, std::ios_base::app);
            totalWatch.stop();
            outfile << totalWatch << "; ";
            outfile.close();
            return result;
        }

        template <typename ValueType>
        std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> MonotonicityChecker<ValueType>::createLattice() {
            // Transform to Lattices
            storm::utility::Stopwatch latticeWatch(true);
            std::tuple<storm::analysis::Lattice*, uint_fast64_t, uint_fast64_t> criticalTuple = extender->toLattice(formulas);

            std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> result;

            auto val1 = std::get<1>(criticalTuple);
            auto val2 = std::get<2>(criticalTuple);
            auto numberOfStates = model->getNumberOfStates();
            std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions;

            if (val1 == numberOfStates && val2 == numberOfStates) {
                result.insert(std::pair<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>>(std::get<0>(criticalTuple), assumptions));
            } else if (val1 != numberOfStates && val2 != numberOfStates) {

                storm::analysis::AssumptionChecker<ValueType> *assumptionChecker;
                if (model->isOfType(storm::models::ModelType::Dtmc)) {
                    auto dtmc = model->as<storm::models::sparse::Dtmc<ValueType>>();
                    assumptionChecker = new storm::analysis::AssumptionChecker<ValueType>(formulas[0], dtmc, 3);
                } else if (model->isOfType(storm::models::ModelType::Mdp)) {
                    auto mdp = model->as<storm::models::sparse::Mdp<ValueType>>();
                    assumptionChecker = new storm::analysis::AssumptionChecker<ValueType>(formulas[0], mdp, 3);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException,
                                    "Unable to perform monotonicity analysis on the provided model type.");
                }
                auto assumptionMaker = new storm::analysis::AssumptionMaker<ValueType>(assumptionChecker, numberOfStates, validate);
                result = extendLatticeWithAssumptions(std::get<0>(criticalTuple), assumptionMaker, val1, val2, assumptions);
            } else {
                assert(false);
            }
            latticeWatch.stop();
            STORM_PRINT(std::endl << "Total time for lattice creation: " << latticeWatch << "." << std::endl << std::endl);
            outfile << latticeWatch << "; ";
            return result;
        }

        template <typename ValueType>
        std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> MonotonicityChecker<ValueType>::extendLatticeWithAssumptions(storm::analysis::Lattice* lattice, storm::analysis::AssumptionMaker<ValueType>* assumptionMaker, uint_fast64_t val1, uint_fast64_t val2, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions) {
            std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> result;

            auto numberOfStates = model->getNumberOfStates();
            if (val1 == numberOfStates || val2 == numberOfStates) {
                assert (val1 == val2);
                result.insert(std::pair<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>>(lattice, assumptions));
            } else {
                auto assumptionPair = assumptionMaker->createAndCheckAssumption(val1, val2, lattice);
                assert (assumptionPair.size() == 2);
                auto itr = assumptionPair.begin();
                auto assumption1 = *itr;
                ++itr;
                auto assumption2 = *itr;

                if (!assumption1.second && !assumption2.second) {
                    // TODO: hier niet verder gaan als je iets gevonden hebt?
                    auto assumptionsCopy = std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>(assumptions);
                    auto latticeCopy = new storm::analysis::Lattice(lattice);
                    assumptions.push_back(assumption1.first);
                    assumptionsCopy.push_back(assumption2.first);

                    auto criticalTuple = extender->extendLattice(lattice, assumption1.first);
                    if (somewhereMonotonicity(std::get<0>(criticalTuple))) {
                        auto map = extendLatticeWithAssumptions(std::get<0>(criticalTuple), assumptionMaker, std::get<1>(criticalTuple), std::get<2>(criticalTuple), assumptions);
                        result.insert(map.begin(), map.end());
                    }

                    criticalTuple = extender->extendLattice(latticeCopy, assumption2.first);
                    if (somewhereMonotonicity(std::get<0>(criticalTuple))) {
                        auto map = extendLatticeWithAssumptions(std::get<0>(criticalTuple), assumptionMaker,
                                                           std::get<1>(criticalTuple), std::get<2>(criticalTuple),
                                                           assumptionsCopy);
                        result.insert(map.begin(), map.end());
                    }
                } else if (assumption1.second && assumption2.second) {
                    auto assumption = assumptionMaker->createEqualAssumption(val1, val2);
                    if (!validate) {
                        assumptions.push_back(assumption);
                    }
                    // if validate is true and both hold, then they must be valid, so no need to add to assumptions
                    auto criticalTuple = extender->extendLattice(lattice, assumption);
                    if (somewhereMonotonicity(std::get<0>(criticalTuple))) {
                        result = extendLatticeWithAssumptions(std::get<0>(criticalTuple), assumptionMaker, std::get<1>(criticalTuple), std::get<2>(criticalTuple), assumptions);
                    }
                } else if (assumption1.second) {
                    if (!validate) {
                        assumptions.push_back(assumption1.first);
                    }
                    // if validate is true and both hold, then they must be valid, so no need to add to assumptions

                    auto criticalTuple = extender->extendLattice(lattice, assumption1.first);

                    if (somewhereMonotonicity(std::get<0>(criticalTuple))) {
                        result = extendLatticeWithAssumptions(std::get<0>(criticalTuple), assumptionMaker, std::get<1>(criticalTuple), std::get<2>(criticalTuple), assumptions);
                    }

                } else {
                    assert (assumption2.second);
                    if (!validate) {
                        assumptions.push_back(assumption2.first);
                    }
                    // if validate is true and both hold, then they must be valid, so no need to add to assumptions
                    auto criticalTuple = extender->extendLattice(lattice, assumption2.first);
                    if (somewhereMonotonicity(std::get<0>(criticalTuple))) {
                        result = extendLatticeWithAssumptions(std::get<0>(criticalTuple), assumptionMaker, std::get<1>(criticalTuple), std::get<2>(criticalTuple), assumptions);
                    }
                }
            }
            return result;
        }

        template <typename ValueType>
        std::map<carl::Variable, std::pair<bool, bool>> MonotonicityChecker<ValueType>::analyseMonotonicity(uint_fast64_t j, storm::analysis::Lattice* lattice, storm::storage::SparseMatrix<ValueType> matrix) {
            storm::utility::Stopwatch analyseWatch(true);

            std::map<carl::Variable, std::pair<bool, bool>> varsMonotone;

            for (uint_fast64_t i = 0; i < matrix.getColumnCount(); ++i) {
                // go over all rows
                auto row = matrix.getRow(i);
                auto first = (*row.begin());
                if (first.getValue() != ValueType(1)) {
                    std::map<uint_fast64_t, ValueType> transitions;

                    for (auto itr = row.begin(); itr != row.end(); ++itr) {
                        transitions.insert(std::pair<uint_fast64_t, ValueType>((*itr).getColumn(), (*itr).getValue()));
                    }

                    auto val = first.getValue();
                    auto vars = val.gatherVariables();
                    for (auto itr = vars.begin(); itr != vars.end(); ++itr) {
                        if (resultCheckOnSamples.find(*itr) != resultCheckOnSamples.end() &&
                            (!resultCheckOnSamples[*itr].first && !resultCheckOnSamples[*itr].second)) {
                            if (varsMonotone.find(*itr) == varsMonotone.end()) {
                                varsMonotone[*itr].first = false;
                                varsMonotone[*itr].second = false;
                            }
                        } else {
                            if (varsMonotone.find(*itr) == varsMonotone.end()) {
                                varsMonotone[*itr].first = true;
                                varsMonotone[*itr].second = true;
                            }
                            std::pair<bool, bool> *value = &varsMonotone.find(*itr)->second;
                            std::pair<bool, bool> old = *value;

                            for (auto itr2 = transitions.begin(); itr2 != transitions.end(); ++itr2) {
                                for (auto itr3 = transitions.begin(); itr3 != transitions.end(); ++itr3) {
                                    auto derivative2 = itr2->second.derivative(*itr);
                                    auto derivative3 = itr3->second.derivative(*itr);

                                    auto compare = lattice->compare(itr2->first, itr3->first);

                                    if (compare == storm::analysis::Lattice::ABOVE) {
                                        // As the first state (itr2) is above the second state (itr3) it is sufficient to look at the derivative of itr2.
                                        std::pair<bool,bool> mon2;
                                        if (derivative2.isConstant()) {
                                            mon2 = std::pair<bool,bool>(derivative2.constantPart() >= 0, derivative2.constantPart() <=0);
                                        } else {
                                            mon2 = checkDerivative(derivative2);
                                        }
                                        value->first &= mon2.first;
                                        value->second &= mon2.second;
                                    } else if (compare == storm::analysis::Lattice::BELOW) {
                                        // As the second state (itr3) is above the first state (itr2) it is sufficient to look at the derivative of itr3.
                                        std::pair<bool,bool> mon3;
                                        if (derivative2.isConstant()) {
                                            mon3 = std::pair<bool,bool>(derivative3.constantPart() >= 0, derivative3.constantPart() <=0);
                                        } else {
                                            mon3 = checkDerivative(derivative3);
                                        }
                                        value->first &= mon3.first;
                                        value->second &= mon3.second;
                                    } else if (compare == storm::analysis::Lattice::SAME) {
                                        // TODO: klopt dit
                                        // Behaviour doesn't matter, as the states are at the same level.
                                    } else {
                                        // As the relation between the states is unknown, we can't claim anything about the monotonicity.
                                        value->first = false;
                                        value->second = false;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            analyseWatch.stop();
            STORM_PRINT(std::endl << "Time to check monotonicity based on the lattice: " << analyseWatch << "." << std::endl << std::endl);
            outfile << analyseWatch << "; ";
            return varsMonotone;
        }

        template <typename ValueType>
        std::pair<bool, bool> MonotonicityChecker<ValueType>::checkDerivative(ValueType derivative) {
            bool monIncr = false;
            bool monDecr = false;

            std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();
            std::shared_ptr<storm::expressions::ExpressionManager> manager(
                    new storm::expressions::ExpressionManager());

            storm::solver::Z3SmtSolver s(*manager);
            storm::solver::SmtSolver::CheckResult smtResult = storm::solver::SmtSolver::CheckResult::Unknown;

            std::set<carl::Variable> variables = derivative.gatherVariables();


            for (auto variable : variables) {
                manager->declareRationalVariable(variable.name());

            }
            storm::expressions::Expression exprBounds = manager->boolean(true);
            auto managervars = manager->getVariables();
            for (auto var : managervars) {
                exprBounds = exprBounds && manager->rational(0) <= var && manager->rational(1) >= var;
            }

            auto converter = storm::expressions::RationalFunctionToExpression<ValueType>(manager);

            storm::expressions::Expression exprToCheck1 = converter.toExpression(derivative) >= manager->rational(0);
            s.add(exprBounds);
            s.add(exprToCheck1);
            smtResult = s.check();
            monIncr = smtResult == storm::solver::SmtSolver::CheckResult::Sat;

            storm::expressions::Expression exprToCheck2 = converter.toExpression(derivative) <= manager->rational(0);
            s.reset();
            smtResult = storm::solver::SmtSolver::CheckResult::Unknown;
            s.add(exprBounds);
            s.add(exprToCheck2);
            smtResult = s.check();
            monDecr = smtResult == storm::solver::SmtSolver::CheckResult::Sat;

            return std::pair<bool, bool>(monIncr, monDecr);
        }


        template <typename ValueType>
        bool MonotonicityChecker<ValueType>::somewhereMonotonicity(storm::analysis::Lattice* lattice) {
            std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel = model->as<storm::models::sparse::Model<ValueType>>();
            auto matrix = sparseModel->getTransitionMatrix();

            // TODO: tussenresultaten hergebruiken
            std::map<carl::Variable, std::pair<bool, bool>> varsMonotone;

            for (uint_fast64_t i = 0; i < matrix.getColumnCount(); ++i) {
                // go over all rows
                auto row = matrix.getRow(i);
                auto first = (*row.begin());
                if (first.getValue() != ValueType(1)) {
                    std::map<uint_fast64_t, ValueType> transitions;

                    for (auto itr = row.begin(); itr != row.end(); ++itr) {
                        transitions.insert(std::pair<uint_fast64_t, ValueType>((*itr).getColumn(), (*itr).getValue()));
                    }

                    auto val = first.getValue();
                    auto vars = val.gatherVariables();
                    for (auto itr = vars.begin(); itr != vars.end(); ++itr) {
                        if (varsMonotone.find(*itr) == varsMonotone.end()) {
                            varsMonotone[*itr].first = true;
                            varsMonotone[*itr].second = true;
                        }
                        std::pair<bool, bool> *value = &varsMonotone.find(*itr)->second;
                        std::pair<bool, bool> old = *value;
// TODO deze ook aanpassen aan deel met smt solver
                        for (auto itr2 = transitions.begin(); itr2 != transitions.end(); ++itr2) {
                            for (auto itr3 = transitions.begin(); itr3 != transitions.end(); ++itr3) {
                                auto derivative2 = itr2->second.derivative(*itr);
                                auto derivative3 = itr3->second.derivative(*itr);

                                auto compare = lattice->compare(itr2->first, itr3->first);

                                if (compare == storm::analysis::Lattice::ABOVE) {
                                    // As the first state (itr2) is above the second state (itr3) it is sufficient to look at the derivative of itr2.
                                    std::pair<bool,bool> mon2;
                                    if (derivative2.isConstant()) {
                                        mon2 = std::pair<bool,bool>(derivative2.constantPart() >= 0, derivative2.constantPart() <=0);
                                    } else {
                                        mon2 = checkDerivative(derivative2);
                                    }
                                    value->first &= mon2.first;
                                    value->second &= mon2.second;
                                } else if (compare == storm::analysis::Lattice::BELOW) {
                                    // As the second state (itr3) is above the first state (itr2) it is sufficient to look at the derivative of itr3.
                                    std::pair<bool,bool> mon3;
                                    if (derivative2.isConstant()) {
                                        mon3 = std::pair<bool,bool>(derivative3.constantPart() >= 0, derivative3.constantPart() <=0);
                                    } else {
                                        mon3 = checkDerivative(derivative3);
                                    }
                                    value->first &= mon3.first;
                                    value->second &= mon3.second;
                                } else if (compare == storm::analysis::Lattice::SAME) {
                                    // TODO: klopt dit
                                    // Behaviour doesn't matter, as the states are at the same level.
                                } else {
                                    // As the relation between the states is unknown, we don't do anything
                                }
                            }
                        }
                    }
                }
            }

            bool result = false;

            for (auto itr = varsMonotone.begin(); !result && itr != varsMonotone.end(); ++itr) {
                result = itr->second.first || itr->second.second;
            }
            return result;
        }


        template <typename ValueType>
        std::map<carl::Variable, std::pair<bool, bool>> MonotonicityChecker<ValueType>::checkOnSamples(std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> model, uint_fast64_t numberOfSamples) {
            storm::utility::Stopwatch samplesWatch(true);

            std::map<carl::Variable, std::pair<bool, bool>> result;

            auto instantiator = storm::utility::ModelInstantiator<storm::models::sparse::Dtmc<ValueType>, storm::models::sparse::Dtmc<double>>(*model);
            auto matrix = model->getTransitionMatrix();
            std::set<carl::Variable> variables =  storm::models::sparse::getProbabilityParameters(*model);

            for (auto itr = variables.begin(); itr != variables.end(); ++itr) {
                double previous = -1;
                bool monDecr = true;
                bool monIncr = true;

                for (auto i = 0; i < numberOfSamples; ++i) {
                    auto valuation = storm::utility::parametric::Valuation<ValueType>();
                    for (auto itr2 = variables.begin(); itr2 != variables.end(); ++itr2) {
                        // Only change value for current variable
                        if ((*itr) == (*itr2)) {
                            auto val = std::pair<carl::Variable, storm::RationalFunctionCoefficient>(
                                    (*itr2), storm::utility::convertNumber<storm::RationalFunctionCoefficient>(
                                            boost::lexical_cast<std::string>((i + 1) / (double(numberOfSamples + 1)))));
                            valuation.insert(val);
                        } else {
                            auto val = std::pair<carl::Variable, storm::RationalFunctionCoefficient>(
                                    (*itr2), storm::utility::convertNumber<storm::RationalFunctionCoefficient>(
                                            boost::lexical_cast<std::string>((1) / (double(numberOfSamples + 1)))));
                            valuation.insert(val);
                        }
                    }
                    storm::models::sparse::Dtmc<double> sampleModel = instantiator.instantiate(valuation);
                    auto checker = storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<double>>(sampleModel);
                    std::unique_ptr<storm::modelchecker::CheckResult> checkResult;
                    auto formula = formulas[0];
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
                    auto initialStates = model->getInitialStates();
                    double initial = 0;
                    for (auto i = initialStates.getNextSetIndex(0); i < model->getNumberOfStates(); i = initialStates.getNextSetIndex(i+1)) {
                        initial += values[i];
                    }
                    float diff = previous - initial;
                    // TODO: define precission
                    if (previous != -1 && diff > 0.000005 && diff < -0.000005) {
                        monDecr &= previous >= initial;
                        monIncr &= previous <= initial;
                    }
                    previous = initial;
                }
                result.insert(std::pair<carl::Variable, std::pair<bool, bool>>(*itr, std::pair<bool,bool>(monIncr, monDecr)));
            }

            samplesWatch.stop();
            STORM_PRINT(std::endl << "Time to check monotonicity on samples: " << samplesWatch << "." << std::endl << std::endl);
            resultCheckOnSamples = result;
            return result;
        }

        template <typename ValueType>
        std::map<carl::Variable, std::pair<bool, bool>> MonotonicityChecker<ValueType>::checkOnSamples(std::shared_ptr<storm::models::sparse::Mdp<ValueType>> model, uint_fast64_t numberOfSamples) {
            storm::utility::Stopwatch samplesWatch(true);

            std::map<carl::Variable, std::pair<bool, bool>> result;

            auto instantiator = storm::utility::ModelInstantiator<storm::models::sparse::Mdp<ValueType>, storm::models::sparse::Mdp<double>>(*model);
            auto matrix = model->getTransitionMatrix();
            std::set<carl::Variable> variables =  storm::models::sparse::getProbabilityParameters(*model);

            for (auto itr = variables.begin(); itr != variables.end(); ++itr) {
                double previous = -1;
                bool monDecr = true;
                bool monIncr = true;

                for (auto i = 0; i < numberOfSamples; ++i) {
                    auto valuation = storm::utility::parametric::Valuation<ValueType>();
                    for (auto itr2 = variables.begin(); itr2 != variables.end(); ++itr2) {
                        // Only change value for current variable
                        if ((*itr) == (*itr2)) {
                            auto val = std::pair<carl::Variable, storm::RationalFunctionCoefficient>(
                                    (*itr2), storm::utility::convertNumber<storm::RationalFunctionCoefficient>(
                                            boost::lexical_cast<std::string>((i + 1) / (double(numberOfSamples + 1)))));
                            valuation.insert(val);
                        } else {
                            auto val = std::pair<carl::Variable, storm::RationalFunctionCoefficient>(
                                    (*itr2), storm::utility::convertNumber<storm::RationalFunctionCoefficient>(
                                            boost::lexical_cast<std::string>((1) / (double(numberOfSamples + 1)))));
                            valuation.insert(val);
                        }
                    }
                    storm::models::sparse::Mdp<double> sampleModel = instantiator.instantiate(valuation);
                    auto checker = storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>>(sampleModel);
                    std::unique_ptr<storm::modelchecker::CheckResult> checkResult;
                    auto formula = formulas[0];
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
                    auto initialStates = model->getInitialStates();
                    double initial = 0;
                    for (auto i = initialStates.getNextSetIndex(0); i < model->getNumberOfStates(); i = initialStates.getNextSetIndex(i+1)) {
                        initial += values[i];
                    }
                    if (previous != -1) {
                        monDecr &= previous >= initial;
                        monIncr &= previous <= initial;
                    }
                    previous = initial;
                }
                result.insert(std::pair<carl::Variable, std::pair<bool, bool>>(*itr, std::pair<bool,bool>(monIncr, monDecr)));
            }

            samplesWatch.stop();
            STORM_PRINT(std::endl << "Time to check monotonicity on samples: " << samplesWatch << "." << std::endl << std::endl);
            resultCheckOnSamples = result;
            return result;
        }

        template class MonotonicityChecker<storm::RationalFunction>;
    }
}
