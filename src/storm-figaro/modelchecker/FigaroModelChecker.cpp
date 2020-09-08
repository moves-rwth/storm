//#define NOAPPROXIMATION

#include "FigaroModelChecker.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/builder/ParallelCompositionBuilder.h"
#include "storm/exceptions/UnmetRequirementException.h"
#include "storm/utility/bitoperations.h"
#include "storm/io/DirectEncodingExporter.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/models/ModelType.h"
#include "storm/settings/modules/EliminationSettings.h"
#include "storm/settings/modules/TransformationSettings.h"
#include "storm-figaro/api/storm-figaro.h"
#include "storm-figaro/builder/ExplicitFigaroModelBuilder.h"
#include "storm-figaro/settings/modules/FIGAROIOSettings.h"
#include "storm-figaro/settings/FIGAROSettings.h"
//#include "storm/transformer/NonMarkovianChainTransformer.h"


namespace storm{
    namespace figaro {
        namespace modelchecker {

            template<typename ValueType>
            typename FigaroModelChecker<ValueType>::figaro_results
            FigaroModelChecker<ValueType>::check(storm::figaro::FigaroProgram &origFigaro,
                                                 typename FigaroModelChecker<ValueType>::property_vector const &properties,
                                                 double approximationError,
                                                 storm::builder::ApproximationHeuristic approximationHeuristic,
                                                 bool eliminateChains) {
                totalTimer.start();
                typename FigaroModelChecker<ValueType>::figaro_results results;

                // Checking Figaro Program

                results = checkFigaro(origFigaro, properties, approximationError, approximationHeuristic,
                                      eliminateChains);
                totalTimer.stop();
                return results;
            }

            template<typename ValueType>
            typename FigaroModelChecker<ValueType>::figaro_results
            FigaroModelChecker<ValueType>::checkFigaro(storm::figaro::FigaroProgram &origFigaro,
                                                       typename FigaroModelChecker<ValueType>::property_vector const &properties,
                                                       double approximationError,
                                                       storm::builder::ApproximationHeuristic approximationHeuristic,
                                                       bool eliminateChains) {
                typename FigaroModelChecker<ValueType>::figaro_results results; //@shahid: remove this line later
                explorationTimer.start();
                auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
                auto figaroIOSettings = storm::settings::getModule<storm::settings::modules::FIGAROIOSettings>();

#ifdef NOAPPROXIMATION
                STORM_LOG_DEBUG("Building Model...");
                //************************************* @TODO: Move this part into explict model builder later
                storm::figaro::builder::ExplicitFigaroModelBuilder<ValueType> builder(origFigaro);
                std::shared_ptr<storm::models::sparse::Model<ValueType>> model = builder.buildModelNoApproximation();
                model->printModelInformationToStream(std::cout);


                if (model->isOfType(storm::models::ModelType::MarkovAutomaton)) {
                    auto ma = std::static_pointer_cast<storm::models::sparse::MarkovAutomaton<ValueType>>(
                            model);
                    if (ma->hasOnlyTrivialNondeterminism()) {
                        model = ma->convertToCtmc();
                    } else {
                        model = storm::transformer::NonMarkovianChainTransformer<ValueType>::eliminateNonmarkovianStates(
                                ma, storm::transformer::EliminationLabelBehavior::MergeLabels);
                    }
                }
                model->printModelInformationToStream(std::cout);
                std::ofstream stream;
                // Check the model
                STORM_LOG_DEBUG("Model checking...");
                modelCheckingTimer.start();
                std::vector<ValueType> results_new;
                for (auto property : properties) {

                    STORM_PRINT_AND_LOG("Model checking property " << *property << " ..." << std::endl);
                    stream << "Model checking property " << *property << " ..." << std::endl;
                    std::unique_ptr<storm::modelchecker::CheckResult> result(
                            storm::api::verifyWithSparseEngine<ValueType>(model,
                                                                          storm::api::createTask<ValueType>(property,
                                                                                                            true)));

                    if (result) {
                        result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                        ValueType resultValue = result->asExplicitQuantitativeCheckResult<ValueType>().getValueMap().begin()->second;
                        results_new.push_back(resultValue);
                        STORM_PRINT_AND_LOG("Result (initial states): " << resultValue << std::endl);
                        stream << "Result (initial states): " << resultValue << std::endl;
                    } else {
                        STORM_LOG_WARN(
                                "The property '" << *property << "' could not be checked with the current settings.");
                        results_new.push_back(storm::utility::one<ValueType>());
                        stream << "The property '" << *property << "' could not be checked with the current settings.";
                    }
                }
                explorationTimer.stop();
                if (printInfo) {
                    model->printModelInformationToStream(std::cout);
                }
                    // Export the model if required
                    // TODO move this outside of the model checker?
                    if (ioSettings.isExportExplicitSet()) {
                        std::vector<std::string> parameterNames;
                        // TODO fill parameter names
                        storm::api::exportSparseModelAsDrn(model, ioSettings.getExportExplicitFilename(), parameterNames, !ioSettings.isExplicitExportPlaceholdersDisabled());
                    }
                    if (ioSettings.isExportDotSet()) {
                        storm::api::exportSparseModelAsDot(model, ioSettings.getExportDotFilename(), ioSettings.getExportDotMaxWidth());
                    }
                // Model checking
                std::vector<ValueType> resultsValue = checkModel(model, properties);
                for (ValueType result : resultsValue) {
                    results.push_back(result);
                }
                return results;
            }
#else

// start approximation from here Comment it for the momment and
//@TODO: come back to this part for abstractions.
                if (false) {
//                    (approximationError > 0.0) {
//                        // Comparator for checking the error of the approximation
//                        storm::utility::ConstantsComparator<ValueType> comparator;
//                        // Build approximate Markov Automata for lower and upper bound
//                        approximation_result approxResult = std::make_pair(storm::utility::zero<ValueType>(),
//                                                                           storm::utility::zero<ValueType>());
//                        std::shared_ptr<storm::models::sparse::Model<ValueType>> model;
//                        std::vector<ValueType> newResult;
//                        storm::builder::ExplicitDFTModelBuilder<ValueType> builder(dft, symmetries);
//
//                        // TODO: compute approximation for all properties simultaneously?
//                        std::shared_ptr<const storm::logic::Formula> property = properties[0];
//                        if (properties.size() > 1) {
//                            STORM_LOG_WARN("Computing approximation only for first property: " << *property);
//                        }
//
//                        bool probabilityFormula = property->isProbabilityOperatorFormula();
//                        STORM_LOG_ASSERT((property->isTimeOperatorFormula() && !probabilityFormula) ||
//                        (!property->isTimeOperatorFormula() && probabilityFormula),
//                        "Probability formula not initialized correctly");
//                        size_t iteration = 0;
//                        do {
//                            // Iteratively build finer models
//                            if (iteration > 0) {
//                                explorationTimer.start();
//                            }
//                            STORM_LOG_DEBUG("Building model...");
//                            // TODO refine model using existing model and MC results
//                            builder.buildModel(iteration, approximationError, approximationHeuristic);
//                            explorationTimer.stop();
//                            buildingTimer.start();
//
//                            // TODO: possible to do bisimulation on approximated model and not on concrete one?
//
//                            // Build model for lower bound
//                            STORM_LOG_DEBUG("Getting model for lower bound...");
//                            model = builder.getModelApproximation(true, !probabilityFormula);
//                            // We only output the info from the lower bound as the info for the upper bound is the same
//                            if (printInfo && dftIOSettings.isShowDftStatisticsSet()) {
//                                std::cout << "Model in iteration " << (iteration + 1) << ":" << std::endl;
//                                model->printModelInformationToStream(std::cout);
//                            }
//                            buildingTimer.stop();
//
//                            if (ioSettings.isExportExplicitSet()) {
//                                std::vector<std::string> parameterNames;
//                                // TODO fill parameter names
//                                storm::api::exportSparseModelAsDrn(model, ioSettings.getExportExplicitFilename(), parameterNames, !ioSettings.isExplicitExportPlaceholdersDisabled());
//                            }
//
//                            // Check lower bounds
//                            newResult = checkModel(model, {property});
//                            STORM_LOG_ASSERT(newResult.size() == 1, "Wrong size for result vector.");
//                            STORM_LOG_ASSERT(iteration == 0 || !comparator.isLess(newResult[0], approxResult.first),
//                                             "New under-approximation " << newResult[0] << " is smaller than old result "
//                                             << approxResult.first);
//                            approxResult.first = newResult[0];
//
//                            // Build model for upper bound
//                            STORM_LOG_DEBUG("Getting model for upper bound...");
//                            buildingTimer.start();
//                            model = builder.getModelApproximation(false, !probabilityFormula);
//                            buildingTimer.stop();
//                            // Check upper bound
//                            newResult = checkModel(model, {property});
//                            STORM_LOG_ASSERT(newResult.size() == 1, "Wrong size for result vector.");
//                            STORM_LOG_ASSERT(iteration == 0 || !comparator.isLess(approxResult.second, newResult[0]),
//                                             "New over-approximation " << newResult[0] << " is greater than old result "
//                                             << approxResult.second);
//                            approxResult.second = newResult[0];
//
//                            STORM_LOG_ASSERT(comparator.isLess(approxResult.first, approxResult.second) ||
//                            comparator.isEqual(approxResult.first, approxResult.second),
//                            "Under-approximation " << approxResult.first
//                            << " is greater than over-approximation "
//                            << approxResult.second);
//                            totalTimer.stop();
//                            if (printInfo && dftIOSettings.isShowDftStatisticsSet()) {
//                                std::cout << "Result after iteration " << (iteration + 1) << ": (" << std::setprecision(10) << approxResult.first << ", " << approxResult.second << ")" << std::endl;
//                                printTimings();
//                                std::cout << std::endl;
//                            } else {
//                                STORM_LOG_DEBUG("Result after iteration " << (iteration + 1) << ": (" << std::setprecision(10) << approxResult.first << ", " << approxResult.second << ")");
//                            }
//
//                            totalTimer.start();
//                            STORM_LOG_THROW(!storm::utility::isInfinity<ValueType>(approxResult.first) &&
//                            !storm::utility::isInfinity<ValueType>(approxResult.second),
//                            storm::exceptions::NotSupportedException,
//                            "Approximation does not work if result might be infinity.");
//                            ++iteration;
//                        } while (!isApproximationSufficient(approxResult.first, approxResult.second, approximationError,
//                                                            probabilityFormula));
//
//                        //STORM_LOG_INFO("Finished approximation after " << iteration << " iteration" << (iteration > 1 ? "s." : "."));
//                        if (printInfo) {
//                            model->printModelInformationToStream(std::cout);
//                        }
//                        dft_results results;
//                        results.push_back(approxResult);
//                        return results;
//                    }
                } else {
//                    // Build a single Markov Automaton/ Markov Chain
                    auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
                    STORM_LOG_DEBUG("Building Model...");
                    //************************************* @TODO: Move this part into explict model builder later
                    storm::figaro::builder::ExplicitFigaroModelBuilder<ValueType> builder(origFigaro);
                    builder.buildModel(0, 0.0);
                    std::shared_ptr<storm::models::sparse::Model<ValueType>> model = builder.getModel();
                    if (eliminateChains && model->isOfType(storm::models::ModelType::MarkovAutomaton)) {

                        if (model->isOfType(storm::models::ModelType::MarkovAutomaton)) {
                            auto ma = std::static_pointer_cast<storm::models::sparse::MarkovAutomaton<ValueType>>(
                                    model);
                            if (ma->hasOnlyTrivialNondeterminism()) {
                                // Markov automaton can be converted into CTMC
                                model = ma->convertToCtmc();
                            } else {
                                model = storm::transformer::NonMarkovianChainTransformer<ValueType>::eliminateNonmarkovianStates(
                                        ma, storm::transformer::EliminationLabelBehavior::MergeLabels);
                            }
                        }
                        model->printModelInformationToStream(std::cout);
//                            storm::api::exportSparseModelAsDot(model,"hello.dot");
//        storm::api::exportSparseModelAsDrn(model,"hello.drn");
                        std::ofstream stream;


                        // Check the model
                        STORM_LOG_DEBUG("Model checking...");
//    modelCheckingTimer.start();
                        std::vector<ValueType> results_new;
                        for (auto property : properties) {

                            STORM_PRINT_AND_LOG("Model checking property " << *property << " ..." << std::endl);
                            stream << "Model checking property " << *property << " ..." << std::endl;
                            std::unique_ptr<storm::modelchecker::CheckResult> result(
                                    storm::api::verifyWithSparseEngine<ValueType>(model,
                                                                                  storm::api::createTask<ValueType>(
                                                                                          property,
                                                                                          true)));

                            if (result) {
                                result->filter(
                                        storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                                ValueType resultValue = result->asExplicitQuantitativeCheckResult<ValueType>().getValueMap().begin()->second;
                                results_new.push_back(resultValue);
                                STORM_PRINT_AND_LOG("Result (initial states): " << resultValue << std::endl);
                                stream << "Result (initial states): " << resultValue << std::endl;
                            } else {
                                STORM_LOG_WARN(
                                        "The property '" << *property
                                                         << "' could not be checked with the current settings.");
                                results_new.push_back(storm::utility::one<ValueType>());
                                stream << "The property '" << *property
                                       << "' could not be checked with the current settings.";
                            }

//
//                        model = storm::transformer::NonMarkovianChainTransformer<ValueType>::eliminateNonmarkovianStates(
//                model->template as<storm::models::sparse::MarkovAutomaton<ValueType>>(),
//                storm::transformer::EliminationLabelBehavior::MergeLabels);

                        }
                        explorationTimer.stop();

                        // Print model information

                        if (true) {//(printInfo) {
                            model->printModelInformationToStream(std::cout);
                        }

//                    // Export the model if required
//                    // TODO move this outside of the model checker?
//                    if (ioSettings.isExportExplicitSet()) {
//                        std::vector<std::string> parameterNames;
//                        // TODO fill parameter names
//                        storm::api::exportSparseModelAsDrn(model, ioSettings.getExportExplicitFilename(), parameterNames, !ioSettings.isExplicitExportPlaceholdersDisabled());
//                    }
//                    if (ioSettings.isExportDotSet()) {
//                        storm::api::exportSparseModelAsDot(model, ioSettings.getExportDotFilename(), ioSettings.getExportDotMaxWidth());
//                    }
//
                        // Model checking
                        std::vector<ValueType> resultsValue = checkModel(model, properties);
                        figaro_results results;
                        for (ValueType result : resultsValue) {
                            results.push_back(result);
                        }
                        return results;
                    }
                }
            }

#endif
            template<typename ValueType>
            std::vector<ValueType>
            FigaroModelChecker<ValueType>::checkModel(std::shared_ptr<storm::models::sparse::Model<ValueType>> &model,
                                                   property_vector const &properties) {
                // Bisimulation
                if (model->isOfType(storm::models::ModelType::Ctmc) &&
                    storm::settings::getModule<storm::settings::modules::GeneralSettings>().isBisimulationSet()) {
                    bisimulationTimer.start();
                    STORM_LOG_DEBUG("Bisimulation...");
                    model = storm::api::performDeterministicSparseBisimulationMinimization<storm::models::sparse::Ctmc<ValueType>>(
                            model->template as<storm::models::sparse::Ctmc<ValueType>>(), properties,
                            storm::storage::BisimulationType::Weak)->template as<storm::models::sparse::Ctmc<ValueType>>();
                    STORM_LOG_DEBUG("No. states (Bisimulation): " << model->getNumberOfStates());
                    STORM_LOG_DEBUG("No. transitions (Bisimulation): " << model->getNumberOfTransitions());
                    bisimulationTimer.stop();
                }

                // Check the model
                STORM_LOG_DEBUG("Model checking...");
                modelCheckingTimer.start();
                std::vector<ValueType> results;

                // Check each property
                storm::utility::Stopwatch singleModelCheckingTimer;
                for (auto property : properties) {
                    singleModelCheckingTimer.reset();
                    singleModelCheckingTimer.start();
                    //STORM_PRINT_AND_LOG("Model checking property " << *property << " ..." << std::endl);
                    std::unique_ptr<storm::modelchecker::CheckResult> result(
                            storm::api::verifyWithSparseEngine<ValueType>(model, storm::api::createTask<ValueType>(property,true)));

                    if (result) {
                        result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                        ValueType resultValue = result->asExplicitQuantitativeCheckResult<ValueType>().getValueMap().begin()->second;
                        results.push_back(resultValue);
                    } else {
                        STORM_LOG_WARN("The property '" << *property << "' could not be checked with the current settings.");
                        results.push_back(-storm::utility::one<ValueType>());
                    }
//                    STORM_PRINT_AND_LOG("Result (initial states): " << resultValue << std::endl);
                    singleModelCheckingTimer.stop();
                    STORM_PRINT_AND_LOG("Time for model checking: " << singleModelCheckingTimer << "." << std::endl);
                }
                modelCheckingTimer.stop();
                STORM_LOG_DEBUG("Model checking done.");
                return results;
            }

            template<typename ValueType>
            bool FigaroModelChecker<ValueType>::isApproximationSufficient(ValueType, ValueType, double, bool) {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Approximation works only for double.");
            }

            template<>
            bool FigaroModelChecker<double>::isApproximationSufficient(double lowerBound, double upperBound,
                                                                    double approximationError, bool relative) {
                STORM_LOG_THROW(!std::isnan(lowerBound) && !std::isnan(upperBound),
                                storm::exceptions::NotSupportedException, "Approximation does not work if result is NaN.");
                if (relative) {
                    return upperBound - lowerBound <= approximationError;
                } else {
                    return upperBound - lowerBound <= approximationError * (lowerBound + upperBound) / 2;
                }
            }

            template<typename ValueType>
            void FigaroModelChecker<ValueType>::printTimings(std::ostream &os) {
                os << "Times:" << std::endl;
                os << "Exploration:\t" << explorationTimer << std::endl;
                os << "Building:\t" << buildingTimer << std::endl;
                os << "Bisimulation:\t" << bisimulationTimer << std::endl;
                os << "Modelchecking:\t" << modelCheckingTimer << std::endl;
                os << "Total:\t\t" << totalTimer << std::endl;
            }

            template<typename ValueType>
            void FigaroModelChecker<ValueType>::printResults(figaro_results const &results, std::ostream &os) {
                bool first = true;
                os << "Result: [";
                for (auto result : results) {
                    if (first) {
                        first = false;
                    } else {
                        os << ", ";
                    }
                    boost::variant<std::ostream&> stream(os);
                    boost::apply_visitor(ResultOutputVisitor(), result, stream);
                }
                os << "]" << std::endl;
            }


            template
            class FigaroModelChecker<double>;

#ifdef STORM_HAVE_CARL

        //    template
        //class FigaroModelChecker<storm::RationalFunction>;

#endif
        }
    }
}
