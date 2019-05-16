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

#include "storm/api/verification.h"
#include "storm-pars/api/storm-pars.h"

#include "storm/modelchecker/results/CheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm-pars/modelchecker/region/SparseDtmcParameterLiftingModelChecker.h"



namespace storm {
    namespace analysis {
        template <typename ValueType>
        MonotonicityChecker<ValueType>::MonotonicityChecker(std::shared_ptr<storm::models::ModelBase> model, std::vector<std::shared_ptr<storm::logic::Formula const>> formulas, bool validate, uint_fast64_t numberOfSamples, double const& precision) {
            assert (model != nullptr);
            this->model = model;
            this->formulas = formulas;
            this->validate = validate;
            this->precision = precision;

            if (numberOfSamples > 0) {
                // sampling
                if (model->isOfType(storm::models::ModelType::Dtmc)) {
                    this->resultCheckOnSamples = std::map<typename utility::parametric::VariableType<ValueType>::type, std::pair<bool, bool>>(
                            checkOnSamples(model->as<storm::models::sparse::Dtmc<ValueType>>(), numberOfSamples));
                } else if (model->isOfType(storm::models::ModelType::Mdp)) {
                    this->resultCheckOnSamples = std::map<typename utility::parametric::VariableType<ValueType>::type, std::pair<bool, bool>>(
                            checkOnSamples(model->as<storm::models::sparse::Mdp<ValueType>>(), numberOfSamples));

                }
                checkSamples= true;
            } else {
                checkSamples= false;
            }

            std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel = model->as<storm::models::sparse::Model<ValueType>>();
            this->extender = new storm::analysis::LatticeExtender<ValueType>(sparseModel);
        }

        template <typename ValueType>
        std::map<storm::analysis::Lattice*, std::map<typename utility::parametric::VariableType<ValueType>::type, std::pair<bool, bool>>> MonotonicityChecker<ValueType>::checkMonotonicity() {
            auto map = createLattice();
            std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel = model->as<storm::models::sparse::Model<ValueType>>();
            auto matrix = sparseModel->getTransitionMatrix();
            return checkMonotonicity(map, matrix);
        }

        template <typename ValueType>
        std::map<storm::analysis::Lattice*, std::map<typename utility::parametric::VariableType<ValueType>::type, std::pair<bool, bool>>> MonotonicityChecker<ValueType>::checkMonotonicity(std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> map, storm::storage::SparseMatrix<ValueType> matrix) {
            storm::utility::Stopwatch monotonicityCheckWatch(true);
            std::map<storm::analysis::Lattice *, std::map<typename utility::parametric::VariableType<ValueType>::type, std::pair<bool, bool>>> result;

            outfile.open(filename, std::ios_base::app);

            if (map.size() == 0) {
                // Nothing is known
                outfile << " No assumptions -";
                STORM_PRINT("No valid assumptions, couldn't build a sufficient lattice");
                if (resultCheckOnSamples.size() != 0) {
                    STORM_PRINT("\n" << "Based results on samples");
                } else {
                    outfile << " ?";
                }

                for (auto entry : resultCheckOnSamples) {
                    if (!entry.second.first && ! entry.second.second) {
                        outfile << " SX " << entry.first << " ";
                    } else if (entry.second.first && ! entry.second.second) {
                        outfile << " SI " << entry.first << " ";
                    } else if (entry.second.first && entry.second.second) {
                        outfile << " SC " << entry.first << " ";
                    } else {
                        outfile << " SD " << entry.first << " ";
                    }
                }

            } else {
                auto i = 0;
                for (auto itr = map.begin(); i < map.size() && itr != map.end(); ++itr) {
                    auto lattice = itr->first;

                    auto addedStates = lattice->getAddedStates()->getNumberOfSetBits();
                    assert (addedStates == lattice->getAddedStates()->size());
                    std::map<typename utility::parametric::VariableType<ValueType>::type, std::pair<bool, bool>> varsMonotone = analyseMonotonicity(i, lattice,
                                                                                                      matrix);

                    auto assumptions = itr->second;
                    if (assumptions.size() > 0) {
                        bool first = true;
                        for (auto itr2 = assumptions.begin(); itr2 != assumptions.end(); ++itr2) {
                            if (!first) {
                                outfile << (" ^ ");
                            } else {
                                first = false;
                            }
                            outfile << (*(*itr2));
                        }
                        outfile << " - ";
                    } else if (assumptions.size() == 0) {
                        outfile << "No assumptions - ";
                    }

                    if (varsMonotone.size() == 0) {
                        outfile << "No params";
                    } else {
                        auto itr2 = varsMonotone.begin();
                        while (itr2 != varsMonotone.end()) {
                            if (checkSamples &&
                                resultCheckOnSamples.find(itr2->first) != resultCheckOnSamples.end() &&
                                (!resultCheckOnSamples[itr2->first].first &&
                                 !resultCheckOnSamples[itr2->first].second)) {
                                outfile << "X " << itr2->first;
                            } else {
                                if (itr2->second.first && itr2->second.second) {
                                    outfile << "C " << itr2->first;
                                } else if (itr2->second.first) {
                                    outfile << "I " << itr2->first;
                                } else if (itr2->second.second) {
                                    outfile << "D " << itr2->first;
                                } else {
                                    outfile << "? " << itr2->first;
                                }
                            }
                            ++itr2;
                            if (itr2 != varsMonotone.end()) {
                                outfile << " ";
                            }
                        }
                        result.insert(
                                std::pair<storm::analysis::Lattice *, std::map<typename utility::parametric::VariableType<ValueType>::type, std::pair<bool, bool>>>(
                                        lattice, varsMonotone));
                    }
                    ++i;
                    outfile << ";";
                }
            }
            outfile << ", ";

            monotonicityCheckWatch.stop();
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
            return result;
        }

        template <typename ValueType>
        std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> MonotonicityChecker<ValueType>::extendLatticeWithAssumptions(storm::analysis::Lattice* lattice, storm::analysis::AssumptionMaker<ValueType>* assumptionMaker, uint_fast64_t val1, uint_fast64_t val2, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions) {
            std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> result;

            auto numberOfStates = model->getNumberOfStates();
            if (val1 == numberOfStates || val2 == numberOfStates) {
                assert (val1 == val2);
                assert (lattice->getAddedStates()->size() == lattice->getAddedStates()->getNumberOfSetBits());
                result.insert(std::pair<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>>(lattice, assumptions));
            } else {
                // Make the three assumptions
                auto assumptionTriple = assumptionMaker->createAndCheckAssumption(val1, val2, lattice);
                assert (assumptionTriple.size() == 3);
                auto itr = assumptionTriple.begin();
                auto assumption1 = *itr;
                ++itr;
                auto assumption2 = *itr;
                ++itr;
                auto assumption3 = *itr;

                if (assumption1.second != AssumptionStatus::INVALID) {
                    auto latticeCopy = new Lattice(lattice);
                    auto assumptionsCopy = std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>(assumptions);

                    if (assumption1.second == AssumptionStatus::UNKNOWN) {
                        // only add assumption to the set of assumptions if it is unknown if it is valid
                        assumptionsCopy.push_back(assumption1.first);
                    }

                    auto criticalTuple = extender->extendLattice(latticeCopy, assumption1.first);
                    if (somewhereMonotonicity(std::get<0>(criticalTuple))) {
                        auto map = extendLatticeWithAssumptions(std::get<0>(criticalTuple), assumptionMaker,
                                                                std::get<1>(criticalTuple), std::get<2>(criticalTuple),
                                                                assumptionsCopy);
                        result.insert(map.begin(), map.end());
                    }
                }

                if (assumption2.second != AssumptionStatus::INVALID) {
                    auto latticeCopy = new Lattice(lattice);
                    auto assumptionsCopy = std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>(assumptions);

                    if (assumption2.second == AssumptionStatus::UNKNOWN) {
                        assumptionsCopy.push_back(assumption2.first);
                    }

                    auto criticalTuple = extender->extendLattice(latticeCopy, assumption2.first);
                    if (somewhereMonotonicity(std::get<0>(criticalTuple))) {
                        auto map = extendLatticeWithAssumptions(std::get<0>(criticalTuple), assumptionMaker,
                                                                std::get<1>(criticalTuple), std::get<2>(criticalTuple),
                                                                assumptionsCopy);
                        result.insert(map.begin(), map.end());
                    }
                }

                if (assumption3.second != AssumptionStatus::INVALID) {
                    // Here we can use the original lattice and assumptions set
                    if (assumption3.second == AssumptionStatus::UNKNOWN) {
                        assumptions.push_back(assumption3.first);
                    }

                    auto criticalTuple = extender->extendLattice(lattice, assumption3.first);
                    if (somewhereMonotonicity(std::get<0>(criticalTuple))) {
                        auto map = extendLatticeWithAssumptions(std::get<0>(criticalTuple), assumptionMaker,
                                                                std::get<1>(criticalTuple), std::get<2>(criticalTuple),
                                                                assumptions);
                        result.insert(map.begin(), map.end());
                    }
                }
            }
            return result;
        }

        template <typename ValueType>
        ValueType MonotonicityChecker<ValueType>::getDerivative(ValueType function, typename utility::parametric::VariableType<ValueType>::type var) {
            if (function.isConstant()) {
                return storm::utility::zero<ValueType>();
            }
            if ((derivatives[function]).find(var) == (derivatives[function]).end()) {
                (derivatives[function])[var] = function.derivative(var);
            }

            return (derivatives[function])[var];
        }

        template <typename ValueType>
        std::map<typename utility::parametric::VariableType<ValueType>::type, std::pair<bool, bool>> MonotonicityChecker<ValueType>::analyseMonotonicity(uint_fast64_t j, storm::analysis::Lattice* lattice, storm::storage::SparseMatrix<ValueType> matrix) {
            std::map<typename utility::parametric::VariableType<ValueType>::type, std::pair<bool, bool>> varsMonotone;

            // go over all rows, check for each row local monotonicity
            for (uint_fast64_t i = 0; i < matrix.getColumnCount(); ++i) {
                auto row = matrix.getRow(i);
                // only enter if you are in a state with at least two successors (so there must be successors,
                // and first prob shouldn't be 1)
                if (row.begin() != row.end() && !row.begin()->getValue().isOne()) {
                    std::map<uint_fast64_t, ValueType> transitions;

                    // Gather all states which are reached with a non constant probability
                    auto states = new storm::storage::BitVector(matrix.getColumnCount());
                    std::set<typename utility::parametric::VariableType<ValueType>::type> vars;
                    for (auto const& entry : row) {
                        if (!entry.getValue().isConstant()) {
                            // only analyse take non constant transitions
                            transitions.insert(std::pair<uint_fast64_t, ValueType>(entry.getColumn(), entry.getValue()));
                            for (auto const& var:entry.getValue().gatherVariables()) {
                                vars.insert(var);
                                states->set(entry.getColumn());
                            }
                        }
                    }

                    // Copy info from checkOnSamples
                    if (checkSamples) {
                        for (auto var : vars) {
                            assert (resultCheckOnSamples.find(var) != resultCheckOnSamples.end());
                            if (varsMonotone.find(var) == varsMonotone.end()) {
                                varsMonotone[var].first = resultCheckOnSamples[var].first;
                                varsMonotone[var].second = resultCheckOnSamples[var].second;
                            } else {
                                varsMonotone[var].first &= resultCheckOnSamples[var].first;
                                varsMonotone[var].second &= resultCheckOnSamples[var].second;
                            }
                        }
                    } else {
                        for (auto var : vars) {
                            if (varsMonotone.find(var) == varsMonotone.end()) {
                                varsMonotone[var].first = true;
                                varsMonotone[var].second = true;
                            }
                        }
                    }



                    // Sort the states based on the lattice
                    auto sortedStates = lattice->sortStates(states);
                    if (sortedStates[sortedStates.size() - 1] == matrix.getColumnCount()) {
                    // If the states are not all sorted, we still might obtain some monotonicity
                        for (auto var: vars) {
                            // current value of monotonicity
                            std::pair<bool, bool> *value = &varsMonotone.find(var)->second;

                            // Go over all transitions to successor states, compare all of them
                            for (auto itr2 = transitions.begin(); (value->first || value->second)
                                    && itr2 != transitions.end(); ++itr2) {
                                for (auto itr3 = transitions.begin(); (value->first || value->second)
                                        && itr3 != transitions.end(); ++itr3) {
                                    if (itr2->first < itr3->first) {

                                        auto derivative2 = getDerivative(itr2->second, var);
                                        auto derivative3 = getDerivative(itr3->second, var);

                                        auto compare = lattice->compare(itr2->first, itr3->first);

                                        if (compare == Lattice::ABOVE) {
                                            // As the first state (itr2) is above the second state (itr3) it
                                            // is sufficient to look at the derivative of itr2.
                                            std::pair<bool, bool> mon2;
                                            mon2 = checkDerivative(derivative2);
                                            value->first &= mon2.first;
                                            value->second &= mon2.second;
                                        } else if (compare == Lattice::BELOW) {
                                            // As the second state (itr3) is above the first state (itr2) it
                                            // is sufficient to look at the derivative of itr3.
                                            std::pair<bool, bool> mon3;

                                            mon3 = checkDerivative(derivative3);
                                            value->first &= mon3.first;
                                            value->second &= mon3.second;
                                        } else if (compare == Lattice::SAME) {
                                            // Behaviour doesn't matter, as the states are at the same level.
                                        } else {
                                            // only if derivatives are the same we can continue
                                            if (derivative2 != derivative3) {
                                                // As the relation between the states is unknown, we can't claim
                                                // anything about the monotonicity.
                                                value->first = false;
                                                value->second = false;
                                            }

                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        // The states are all sorted
                        for (auto var : vars) {
                            std::pair<bool, bool> *value = &varsMonotone.find(var)->second;
                            bool change = false;
                            for (auto const &i : sortedStates) {
                                auto res = checkDerivative(getDerivative(transitions[i], var));
                                change = change || (!(value->first && value->second) // they do not hold both
                                                    && ((value->first && !res.first)
                                                        || (value->second && !res.second)));

                                if (change) {
                                    value->first &= res.second;
                                    value->second &= res.first;
                                } else {
                                    value->first &= res.first;
                                    value->second &= res.second;
                                }
                                if (!value->first && !value->second) {
                                    break;
                                }
                            }

                        }
                    }
                }
            }
            return varsMonotone;
        }

        template <typename ValueType>
        bool MonotonicityChecker<ValueType>::somewhereMonotonicity(Lattice* lattice) {
            std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel = model->as<storm::models::sparse::Model<ValueType>>();
            auto matrix = sparseModel->getTransitionMatrix();

            std::map<typename utility::parametric::VariableType<ValueType>::type, std::pair<bool, bool>> varsMonotone;

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
                    // Copy info from checkOnSamples
                    if (checkSamples) {
                        for (auto var : vars) {
                            assert (resultCheckOnSamples.find(var) != resultCheckOnSamples.end());
                            if (varsMonotone.find(var) == varsMonotone.end()) {
                                varsMonotone[var].first = resultCheckOnSamples[var].first;
                                varsMonotone[var].second = resultCheckOnSamples[var].second;
                            } else {
                                varsMonotone[var].first &= resultCheckOnSamples[var].first;
                                varsMonotone[var].second &= resultCheckOnSamples[var].second;
                            }
                        }
                    } else {
                        for (auto var : vars) {
                            if (varsMonotone.find(var) == varsMonotone.end()) {
                                varsMonotone[var].first = true;
                                varsMonotone[var].second = true;
                            }
                        }
                    }

                    for (auto var: vars) {
                        // current value of monotonicity
                        std::pair<bool, bool> *value = &varsMonotone.find(var)->second;

                        // Go over all transitions to successor states, compare all of them
                        for (auto itr2 = transitions.begin(); (value->first || value->second)
                                                              && itr2 != transitions.end(); ++itr2) {
                            for (auto itr3 = transitions.begin(); (value->first || value->second)
                                                                  && itr3 != transitions.end(); ++itr3) {
                                if (itr2->first < itr3->first) {

                                    auto derivative2 = getDerivative(itr2->second, var);
                                    auto derivative3 = getDerivative(itr3->second, var);

                                    auto compare = lattice->compare(itr2->first, itr3->first);

                                    if (compare == Lattice::ABOVE) {
                                        // As the first state (itr2) is above the second state (itr3) it
                                        // is sufficient to look at the derivative of itr2.
                                        std::pair<bool, bool> mon2;
                                        mon2 = checkDerivative(derivative2);
                                        value->first &= mon2.first;
                                        value->second &= mon2.second;
                                    } else if (compare == Lattice::BELOW) {
                                        // As the second state (itr3) is above the first state (itr2) it
                                        // is sufficient to look at the derivative of itr3.
                                        std::pair<bool, bool> mon3;

                                        mon3 = checkDerivative(derivative3);
                                        value->first &= mon3.first;
                                        value->second &= mon3.second;
                                    } else if (compare == Lattice::SAME) {
                                        // Behaviour doesn't matter, as the states are at the same level.
                                    } else {
                                        // As the relation between the states is unknown, we don't do anything
                                    }
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
        std::map<typename utility::parametric::VariableType<ValueType>::type, std::pair<bool, bool>> MonotonicityChecker<ValueType>::checkOnSamples(std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> model, uint_fast64_t numberOfSamples) {
            storm::utility::Stopwatch samplesWatch(true);

            std::map<typename utility::parametric::VariableType<ValueType>::type, std::pair<bool, bool>> result;

            auto instantiator = storm::utility::ModelInstantiator<storm::models::sparse::Dtmc<ValueType>, storm::models::sparse::Dtmc<double>>(*model);
            auto matrix = model->getTransitionMatrix();
            std::set<typename utility::parametric::VariableType<ValueType>::type> variables =  storm::models::sparse::getProbabilityParameters(*model);

            for (auto itr = variables.begin(); itr != variables.end(); ++itr) {
                double previous = -1;
                bool monDecr = true;
                bool monIncr = true;

                for (auto i = 0; (monDecr || monIncr) && i < numberOfSamples; ++i) {
                    auto valuation = storm::utility::parametric::Valuation<ValueType>();
                    for (auto itr2 = variables.begin(); itr2 != variables.end(); ++itr2) {
                        // Only change value for current variable
                        if ((*itr) == (*itr2)) {
                            auto val = std::pair<typename utility::parametric::VariableType<ValueType>::type, typename utility::parametric::CoefficientType<ValueType>::type>(
                                    (*itr2), storm::utility::convertNumber<typename utility::parametric::CoefficientType<ValueType>::type>(
                                            boost::lexical_cast<std::string>((i + 1) / (double(numberOfSamples + 1)))));
                            valuation.insert(val);
                            assert (0 < val.second && val.second < 1);
                        } else {
                            auto val = std::pair<typename utility::parametric::VariableType<ValueType>::type, typename utility::parametric::CoefficientType<ValueType>::type>(
                                    (*itr2), storm::utility::convertNumber<typename utility::parametric::CoefficientType<ValueType>::type>(
                                            boost::lexical_cast<std::string>((1) / (double(numberOfSamples + 1)))));
                            valuation.insert(val);
                            assert (0 < val.second && val.second < 1);
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
                    for (auto j = initialStates.getNextSetIndex(0); j < model->getNumberOfStates(); j = initialStates.getNextSetIndex(j+1)) {
                        initial += values[j];
                    }
                    assert (initial >= precision && initial <= 1+precision);
                    double diff = previous - initial;
                    assert (previous == -1 || diff >= -1-precision && diff <= 1 + precision);
                    if (previous != -1 && (diff > precision || diff < -precision)) {
                        monDecr &= diff > precision; // then previous value is larger than the current value from the initial states
                        monIncr &= diff < -precision;
                    }
                    previous = initial;
                }
                result.insert(std::pair<typename utility::parametric::VariableType<ValueType>::type, std::pair<bool, bool>>(*itr, std::pair<bool,bool>(monIncr, monDecr)));
            }

            samplesWatch.stop();
            resultCheckOnSamples = result;
            return result;
        }

        template <typename ValueType>
        std::map<typename utility::parametric::VariableType<ValueType>::type, std::pair<bool, bool>> MonotonicityChecker<ValueType>::checkOnSamples(std::shared_ptr<storm::models::sparse::Mdp<ValueType>> model, uint_fast64_t numberOfSamples) {
            storm::utility::Stopwatch samplesWatch(true);

            std::map<typename utility::parametric::VariableType<ValueType>::type, std::pair<bool, bool>> result;

            auto instantiator = storm::utility::ModelInstantiator<storm::models::sparse::Mdp<ValueType>, storm::models::sparse::Mdp<double>>(*model);
            auto matrix = model->getTransitionMatrix();
            std::set<typename utility::parametric::VariableType<ValueType>::type> variables =  storm::models::sparse::getProbabilityParameters(*model);

            for (auto itr = variables.begin(); itr != variables.end(); ++itr) {
                double previous = -1;
                bool monDecr = true;
                bool monIncr = true;

                for (auto i = 0; i < numberOfSamples; ++i) {
                    auto valuation = storm::utility::parametric::Valuation<ValueType>();
                    for (auto itr2 = variables.begin(); itr2 != variables.end(); ++itr2) {
                        // Only change value for current variable
                        if ((*itr) == (*itr2)) {
                            auto val = std::pair<typename utility::parametric::VariableType<ValueType>::type, typename utility::parametric::CoefficientType<ValueType>::type>(
                                    (*itr2), storm::utility::convertNumber<typename utility::parametric::CoefficientType<ValueType>::type>(
                                            boost::lexical_cast<std::string>((i + 1) / (double(numberOfSamples + 1)))));
                            valuation.insert(val);
                        } else {
                            auto val = std::pair<typename utility::parametric::VariableType<ValueType>::type, typename utility::parametric::CoefficientType<ValueType>::type>(
                                    (*itr2), storm::utility::convertNumber<typename utility::parametric::CoefficientType<ValueType>::type>(
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
                    assert (initial >= precision && initial <= 1+precision);
                    double diff = previous - initial;
                    assert (previous == -1 || diff >= -1-precision && diff <= 1 + precision);
                    if (previous != -1 && (diff > precision || diff < -precision)) {
                        monDecr &= diff > precision; // then previous value is larger than the current value from the initial states
                        monIncr &= diff < -precision;
                    }
                    previous = initial;
                }
                result.insert(std::pair<typename utility::parametric::VariableType<ValueType>::type, std::pair<bool, bool>>(*itr, std::pair<bool,bool>(monIncr, monDecr)));
            }

            samplesWatch.stop();
            resultCheckOnSamples = result;
            return result;
        }

        template class MonotonicityChecker<storm::RationalFunction>;
    }
}
