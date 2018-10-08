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


namespace storm {
    namespace analysis {
        template <typename ValueType>
        MonotonicityChecker<ValueType>::MonotonicityChecker(std::shared_ptr<storm::models::ModelBase> model, std::vector<std::shared_ptr<storm::logic::Formula const>> formulas, bool validate) {
            this->model = model;
            this->formulas = formulas;
            this->validate = validate;
        }

        template <typename ValueType>
        std::map<storm::analysis::Lattice*, std::map<carl::Variable, std::pair<bool, bool>>> MonotonicityChecker<ValueType>::checkMonotonicity() {
            bool maybeMonotone = true;
            if (model->isOfType(storm::models::ModelType::Dtmc)) {
                auto dtmcModel = model->as<storm::models::sparse::Dtmc<ValueType>>();
                maybeMonotone = checkOnSamples(dtmcModel,3);
            } //TODO mdp
            if (maybeMonotone) {
                auto map = createLattice();
                std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel = model->as<storm::models::sparse::Model<ValueType>>();
                auto matrix = sparseModel->getTransitionMatrix();
                return checkMonotonicity(map, matrix);
            } else {
                std::map<storm::analysis::Lattice*, std::map<carl::Variable, std::pair<bool, bool>>> result;
                std::cout << "Not monotone" << std::endl;
                return result;
            }
        }

        template <typename ValueType>
        std::map<storm::analysis::Lattice*, std::map<carl::Variable, std::pair<bool, bool>>> MonotonicityChecker<ValueType>::checkMonotonicity(std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> map, storm::storage::SparseMatrix<ValueType> matrix) {
            auto i = 0;
            std::map<storm::analysis::Lattice*, std::map<carl::Variable, std::pair<bool, bool>>> result;
            for (auto itr = map.begin(); i < map.size() && itr != map.end(); ++itr) {
                auto lattice = itr->first;
                auto assumptions = itr->second;
                std::ofstream myfile;
                std::string filename = "lattice" + std::to_string(i) + ".dot";
                myfile.open (filename);
                lattice->toDotFile(myfile);
                myfile.close();

                if (assumptions.size() > 0) {
                    STORM_PRINT("Given assumptions: " << std::endl);
                    bool first = true;
                    for (auto itr2 = assumptions.begin(); itr2 != assumptions.end(); ++itr2) {
                        if (!first) {
                            STORM_PRINT(" ^ ");
                        } else {
                            STORM_PRINT("    ");
                            first = false;
                        }

                        std::shared_ptr<storm::expressions::BinaryRelationExpression> expression = *itr2;
                        auto var1 = expression->getFirstOperand();
                        auto var2 = expression->getSecondOperand();
                        STORM_PRINT(*expression);
                    }
                    STORM_PRINT(std::endl);
                }

                std::map<carl::Variable, std::pair<bool, bool>> varsMonotone = analyseMonotonicity(i, lattice, matrix);
                if (varsMonotone.size() == 0) {
                    STORM_PRINT("Result is constant" << std::endl);
                } else {
                    for (auto itr2 = varsMonotone.begin(); itr2 != varsMonotone.end(); ++itr2) {
                        if (itr2->second.first) {
                            STORM_PRINT("  - Monotone increasing in: " << itr2->first << std::endl);
                        } else {
                            STORM_PRINT("  - Do not know if monotone increasing in: " << itr2->first << std::endl);
                        }
                        if (itr2->second.second) {
                            STORM_PRINT("  - Monotone decreasing in: " << itr2->first << std::endl);
                        } else {
                            STORM_PRINT("  - Do not know if monotone decreasing in: " << itr2->first << std::endl);
                        }
                    }
                    result.insert(
                            std::pair<storm::analysis::Lattice *, std::map<carl::Variable, std::pair<bool, bool>>>(
                                    lattice, varsMonotone));
                }
                ++i;
            }
            return result;
        }

        template <typename ValueType>
        std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> MonotonicityChecker<ValueType>::createLattice() {
            // Transform to Lattices
            storm::utility::Stopwatch latticeWatch(true);
            std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel = model->as<storm::models::sparse::Model<ValueType>>();
            storm::analysis::LatticeExtender<ValueType> *extender = new storm::analysis::LatticeExtender<ValueType>(sparseModel);
            std::tuple<storm::analysis::Lattice*, uint_fast64_t, uint_fast64_t> criticalTuple = extender->toLattice(formulas);
            std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> result;
            if (model->isOfType(storm::models::ModelType::Dtmc)) {
                auto dtmcModel = model->as<storm::models::sparse::Dtmc<ValueType>>();
                auto assumptionChecker = storm::analysis::AssumptionChecker<ValueType>(formulas[0], dtmcModel, 3);
                auto assumptionMaker = storm::analysis::AssumptionMaker<ValueType>(extender, &assumptionChecker, sparseModel->getNumberOfStates(), validate);
                result = assumptionMaker.makeAssumptions(std::get<0>(criticalTuple), std::get<1>(criticalTuple), std::get<2>(criticalTuple));
            } else if (model->isOfType(storm::models::ModelType::Dtmc)) {
                auto mdpModel = model->as<storm::models::sparse::Mdp<ValueType>>();
                auto assumptionChecker = storm::analysis::AssumptionChecker<ValueType>(formulas[0], mdpModel, 3);
                auto assumptionMaker = storm::analysis::AssumptionMaker<ValueType>(extender, &assumptionChecker, sparseModel->getNumberOfStates(), validate);
                result = assumptionMaker.makeAssumptions(std::get<0>(criticalTuple), std::get<1>(criticalTuple), std::get<2>(criticalTuple));
            } else {
               STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Unable to perform monotonicity analysis on the provided model type.");
            }

            latticeWatch.stop();
            STORM_PRINT(std::endl << "Time for lattice creation: " << latticeWatch << "." << std::endl << std::endl);
            return result;
        }

        template <typename ValueType>
        std::map<carl::Variable, std::pair<bool, bool>> MonotonicityChecker<ValueType>::analyseMonotonicity(uint_fast64_t j, storm::analysis::Lattice* lattice, storm::storage::SparseMatrix<ValueType> matrix) {
            std::map<carl::Variable, std::pair<bool, bool>> varsMonotone;
            std::ofstream myfile;
            std::string filename = "mc" + std::to_string(j) + ".dot";
            myfile.open (filename);
            myfile << "digraph \"MC\" {" << std::endl;
            myfile << "\t" << "node [shape=ellipse]" << std::endl;

            // print all nodes
            for (uint_fast64_t i = 0; i < matrix.getColumnCount(); ++i) {
                myfile << "\t\"" << i << "\" [label = \"" << i << "\"]" << std::endl;
            }

            for (uint_fast64_t i = 0; i < matrix.getColumnCount(); ++i) {
                // go over all rows
                auto row = matrix.getRow(i);
                auto first = (*row.begin());
                if (first.getValue() != ValueType(1)) {
                    std::map<uint_fast64_t, ValueType> transitions;

                    for (auto itr = row.begin(); itr != row.end(); ++itr) {
                        transitions.insert(std::pair<uint_fast64_t, ValueType>((*itr).getColumn(), (*itr).getValue()));
                    }

                    std::string color = "";
                    auto val = first.getValue();
                    auto vars = val.gatherVariables();
                    for (auto itr = vars.begin(); itr != vars.end(); ++itr) {
                        if (varsMonotone.find(*itr) == varsMonotone.end()) {
                            varsMonotone[*itr].first = true;
                            varsMonotone[*itr].second = true;
                        }
                        std::pair<bool, bool>* value = &varsMonotone.find(*itr)->second;
                        std::pair<bool, bool> old = *value;

                        for (auto itr2 = transitions.begin(); itr2 != transitions.end(); ++itr2) {
                            for (auto itr3 = transitions.begin(); itr3 != transitions.end(); ++itr3) {
                                auto derivative2 = (*itr2).second.derivative(*itr);
                                auto derivative3 = (*itr3).second.derivative(*itr);
                                STORM_LOG_THROW(derivative2.isConstant() && derivative3.isConstant(), storm::exceptions::NotSupportedException, "Expecting derivative to be constant");

                                auto compare = lattice->compare((*itr2).first, (*itr3).first);

                                if (compare == storm::analysis::Lattice::ABOVE) {
                                    // As the first state (itr2) is above the second state (itr3) it is sufficient to look at the derivative of itr2.
                                    value->first &=derivative2.constantPart() >= 0;
                                    value->second &=derivative2.constantPart() <= 0;
                                } else if (compare == storm::analysis::Lattice::BELOW) {
                                    // As the second state (itr3) is above the first state (itr2) it is sufficient to look at the derivative of itr3.
                                    value->first &=derivative3.constantPart() >= 0;
                                    value->second &=derivative3.constantPart() <= 0;
                                } else if (compare == storm::analysis::Lattice::SAME) {
                                    // Behaviour doesn't matter, as the states are at the same level.
                                } else {
                                    // As the relation between the states is unknown, we can't claim anything about the monotonicity.
                                    value->first = false;
                                    value->second = false;
                                }
                            }
                        }

                        if ((value->first != old.first) && (value->second != old.second)) {
                            color = "color = red, ";
                        } else if ((value->first != old.first)) {
                            myfile << "\t edge[style=dashed];" << std::endl;
                            color = "color = blue, ";
                        } else if ((value->second != old.second)) {
                            myfile << "\t edge[style=dotted];" << std::endl;
                            color = "color = blue, ";
                        }
                    }

                    for (auto itr = transitions.begin(); itr != transitions.end(); ++itr) {
                        myfile << "\t" << i << " -> " << itr->first << "[" << color << "label=\"" << itr->second << "\"];"
                               << std::endl;
                    }

                    myfile << "\t edge[style=\"\"];" << std::endl;
                } else {
                    myfile << "\t" << i << " -> " << first.getColumn() << "[label=\"" << first.getValue() << "\"];"
                           << std::endl;
                }
            }

            myfile << "\tsubgraph legend {" << std::endl;
            myfile << "\t\tnode [color=white];" << std::endl;
            myfile << "\t\tedge [style=invis];" << std::endl;
            myfile << "\t\tt0 [label=\"incr? and decr?\", fontcolor=red];" << std::endl;
            myfile << "\t\tt1 [label=\"incr? (dashed)\", fontcolor=blue];" << std::endl;
            myfile << "\t\tt2 [label=\"decr? (dotted)\", fontcolor=blue];" << std::endl;

            myfile << "\t}" << std::endl;
            myfile << "}" << std::endl;
            myfile.close();
            return varsMonotone;
        }

        template <typename ValueType>
        bool MonotonicityChecker<ValueType>::checkOnSamples(std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> model, uint_fast64_t numberOfSamples) {
            bool monDecr = true;
            bool monIncr = true;

            auto instantiator = storm::utility::ModelInstantiator<storm::models::sparse::Dtmc<ValueType>, storm::models::sparse::Dtmc<double>>(*model);
            auto matrix = model->getTransitionMatrix();
            std::set<storm::RationalFunctionVariable> variables =  storm::models::sparse::getProbabilityParameters(*model);
            double previous = -1;
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

            bool result = monDecr || monIncr;
            return result;
        }

        template class MonotonicityChecker<storm::RationalFunction>;
    }
}
