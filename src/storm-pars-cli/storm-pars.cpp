#include "storm-cli-utilities/cli.h"
#include "storm-cli-utilities/model-handling.h"

#include "storm-pars/api/storm-pars.h"
#include "storm-pars/api/region.h"
#include "storm-pars/analysis/MonotonicityHelper.h"

#include "storm-pars/derivative/GradientDescentInstantiationSearcher.h"
#include "storm-pars/derivative/SparseDerivativeInstantiationModelChecker.h"
#include "storm-pars/modelchecker/instantiation/SparseCtmcInstantiationModelChecker.h"
#include "storm-pars/modelchecker/region/SparseParameterLiftingModelChecker.h"
#include "storm-pars/modelchecker/region/SparseDtmcParameterLiftingModelChecker.h"

#include "storm-pars/settings/ParsSettings.h"
#include "storm-pars/settings/modules/ParametricSettings.h"
#include "storm-pars/settings/modules/MonotonicitySettings.h"
#include "storm-pars/settings/modules/DerivativeSettings.h"
#include "storm-pars/settings/modules/RegionSettings.h"

#include "storm-pars/transformer/SparseParametricMdpSimplifier.h"
#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"
#include "storm-pars/derivative/GradientDescentMethod.h"

#include "storm-parsers/parser/KeyValueParser.h"
#include "storm/api/storm.h"

#include "storm/exceptions/BaseException.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/NotSupportedException.h"

#include "storm/models/ModelBase.h"

#include "storm/settings/SettingsManager.h"

#include "storm/solver/stateelimination/NondeterministicModelStateEliminator.h"

#include "storm/storage/StronglyConnectedComponentDecomposition.h"
#include "storm/storage/SymbolicModelDescription.h"

#include "storm/io/file.h"
#include "storm/utility/initialize.h"
#include "storm/utility/Stopwatch.h"
#include "storm/utility/macros.h"
#include "storm/utility/Engine.h"

#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/modules/BisimulationSettings.h"
#include "storm/settings/modules/TransformationSettings.h"


namespace storm {
    namespace pars {

        typedef typename storm::cli::SymbolicInput SymbolicInput;

        template <typename ValueType>
        struct SampleInformation {
            SampleInformation(bool graphPreserving = false, bool exact = false) : graphPreserving(graphPreserving), exact(exact) {
                // Intentionally left empty.
            }

            bool empty() const {
                return cartesianProducts.empty();
            }

            std::vector<std::map<typename utility::parametric::VariableType<ValueType>::type, std::vector<typename utility::parametric::CoefficientType<ValueType>::type>>> cartesianProducts;
            bool graphPreserving;
            bool exact;
        };

        struct PreprocessResult {
            PreprocessResult(std::shared_ptr<storm::models::ModelBase> const& model, bool changed) : changed(changed), model(model) {
                // Intentionally left empty.
            }

            bool changed;
            std::shared_ptr<storm::models::ModelBase> model;
            boost::optional<std::vector<std::shared_ptr<storm::logic::Formula const>>> formulas;
        };

        template <typename ValueType>
        std::vector<storm::storage::ParameterRegion<ValueType>> parseRegions(std::shared_ptr<storm::models::ModelBase> const& model) {
            std::vector<storm::storage::ParameterRegion<ValueType>> result;
            auto regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
            boost::optional<int> splittingThreshold;
            if (regionSettings.isSplittingThresholdSet()) {
                splittingThreshold = regionSettings.getSplittingThreshold();
            }
            if (regionSettings.isRegionSet()) {
                result = storm::api::parseRegions<ValueType>(regionSettings.getRegionString(), *model, splittingThreshold);
            } else if (regionSettings.isRegionBoundSet()) {
                result = storm::api::createRegion<ValueType>(regionSettings.getRegionBoundString(), *model, splittingThreshold);
            }
            return result;
        }

        template <typename ValueType>
        SampleInformation<ValueType> parseSamples(std::shared_ptr<storm::models::ModelBase> const& model, std::string const& sampleString, bool graphPreserving) {
            STORM_LOG_THROW(!model || model->isSparseModel(), storm::exceptions::NotSupportedException, "Sampling is only supported for sparse models.");

            SampleInformation<ValueType> sampleInfo(graphPreserving);
            if (sampleString.empty()) {
                return sampleInfo;
            }

            // Get all parameters from the model.
            std::set<typename utility::parametric::VariableType<ValueType>::type> modelParameters;
            auto const& sparseModel = *model->as<storm::models::sparse::Model<ValueType>>();
            modelParameters = storm::models::sparse::getProbabilityParameters(sparseModel);
            auto rewParameters = storm::models::sparse::getRewardParameters(sparseModel);
            modelParameters.insert(rewParameters.begin(), rewParameters.end());

            std::vector<std::string> cartesianProducts;
            boost::split(cartesianProducts, sampleString, boost::is_any_of(";"));
            for (auto& product : cartesianProducts) {
                boost::trim(product);

                // Get the values string for each variable.
                std::vector<std::string> valuesForVariables;
                boost::split(valuesForVariables, product, boost::is_any_of(","));
                for (auto& values : valuesForVariables) {
                    boost::trim(values);
                }

                std::set<typename utility::parametric::VariableType<ValueType>::type> encounteredParameters;
                sampleInfo.cartesianProducts.emplace_back();
                auto& newCartesianProduct = sampleInfo.cartesianProducts.back();
                for (auto const& varValues : valuesForVariables) {
                    auto equalsPosition = varValues.find("=");
                    STORM_LOG_THROW(equalsPosition != varValues.npos, storm::exceptions::WrongFormatException, "Incorrect format of samples.");
                    std::string variableName = varValues.substr(0, equalsPosition);
                    boost::trim(variableName);
                    std::string values = varValues.substr(equalsPosition + 1);
                    boost::trim(values);

                    bool foundParameter = false;
                    typename utility::parametric::VariableType<ValueType>::type theParameter;
                    for (auto const& parameter : modelParameters) {
                        std::stringstream parameterStream;
                        parameterStream << parameter;
                        if (parameterStream.str() == variableName) {
                            foundParameter = true;
                            theParameter = parameter;
                            encounteredParameters.insert(parameter);
                        }
                    }
                    STORM_LOG_THROW(foundParameter, storm::exceptions::WrongFormatException, "Unknown parameter '" << variableName << "'.");

                    std::vector<std::string> splitValues;
                    boost::split(splitValues, values, boost::is_any_of(":"));
                    STORM_LOG_THROW(!splitValues.empty(), storm::exceptions::WrongFormatException, "Expecting at least one value per parameter.");

                    auto& list = newCartesianProduct[theParameter];

                    for (auto& value : splitValues) {
                        boost::trim(value);
                        list.push_back(storm::utility::convertNumber<typename utility::parametric::CoefficientType<ValueType>::type>(value));
                    }
                }

                STORM_LOG_THROW(encounteredParameters == modelParameters, storm::exceptions::WrongFormatException, "Variables for all parameters are required when providing samples.");
            }

            return sampleInfo;
        }

        template <typename ValueType>
        std::shared_ptr<storm::models::ModelBase> eliminateScc(std::shared_ptr<storm::models::ModelBase> const& model) {
                storm::utility::Stopwatch eliminationWatch(true);
                std::shared_ptr<storm::models::ModelBase> result;
                if (model->isOfType(storm::models::ModelType::Dtmc)) {
                    STORM_PRINT("Applying scc elimination" << std::endl);
                    auto sparseModel = model->as<storm::models::sparse::Model<ValueType>>();
                    auto matrix = sparseModel->getTransitionMatrix();
                    auto backwardsTransitionMatrix = matrix.transpose();

                    storm::storage::StronglyConnectedComponentDecompositionOptions const options;
                    auto decomposition = storm::storage::StronglyConnectedComponentDecomposition<ValueType>(matrix, options);

                    storm::storage::BitVector selectedStates(matrix.getRowCount());
                    storm::storage::BitVector selfLoopStates(matrix.getRowCount());
                    for (size_t i = 0; i < decomposition.size(); ++i) {
                        auto scc = decomposition.getBlock(i);
                        if (scc.size() > 1) {
                            auto statesScc = scc.getStates();
                            std::vector<uint_fast64_t> entryStates;
                            for (auto state : statesScc) {
                                auto row = backwardsTransitionMatrix.getRow(state);
                                bool found = false;
                                for (auto backState : row) {
                                    if (!scc.containsState(backState.getColumn())) {
                                        found = true;
                                    }
                                }
                                if (found) {
                                    entryStates.push_back(state);
                                    selfLoopStates.set(state);
                                } else {
                                    selectedStates.set(state);
                                }
                            }

                            if (entryStates.size() != 1) {
                                STORM_LOG_THROW(entryStates.size() > 1, storm::exceptions::NotImplementedException,
                                                "state elimination not implemented for scc with more than 1 entry points");
                            }
                        }
                    }

                    storm::storage::FlexibleSparseMatrix<ValueType> flexibleMatrix(matrix);
                    storm::storage::FlexibleSparseMatrix<ValueType> flexibleBackwardTransitions(backwardsTransitionMatrix, true);
                    auto actionRewards = std::vector<ValueType>(matrix.getRowCount(), storm::utility::zero<ValueType>());
                    storm::solver::stateelimination::NondeterministicModelStateEliminator<ValueType> stateEliminator(flexibleMatrix, flexibleBackwardTransitions, actionRewards);
                    for(auto state : selectedStates) {
                        stateEliminator.eliminateState(state, true);
                    }
                    for (auto state : selfLoopStates) {
                        auto row = flexibleMatrix.getRow(state);
                        stateEliminator.eliminateLoop(state);
                    }
                    selectedStates.complement();
                    auto keptRows = matrix.getRowFilter(selectedStates);
                    storm::storage::SparseMatrix<ValueType> newTransitionMatrix = flexibleMatrix.createSparseMatrix(keptRows, selectedStates);
                    // TODO @Jip: note that rewards get lost
                    result = std::make_shared<storm::models::sparse::Dtmc<ValueType>>(std::move(newTransitionMatrix), sparseModel->getStateLabeling().getSubLabeling(selectedStates));

                    eliminationWatch.stop();
                    STORM_PRINT(std::endl << "Time for scc elimination: " << eliminationWatch << "." << std::endl << std::endl);
                    result->printModelInformationToStream(std::cout);
                } else if (model->isOfType(storm::models::ModelType::Mdp)) {
                    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Unable to perform SCC elimination for monotonicity analysis on MDP: Not mplemented");
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Unable to perform monotonicity analysis on the provided model type.");
                }
                return result;
        }

        template <typename ValueType>
        std::shared_ptr<storm::models::ModelBase> simplifyModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            storm::utility::Stopwatch simplifyingWatch(true);
            std::shared_ptr<storm::models::ModelBase> result;
            if (model->isOfType(storm::models::ModelType::Dtmc)) {
                storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<ValueType>> simplifier(*(model->template as<storm::models::sparse::Dtmc<ValueType>>()));

                std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(input.properties);
                STORM_LOG_THROW(formulas.begin()!=formulas.end(), storm::exceptions::NotSupportedException, "Only one formula at the time supported");

                if (!simplifier.simplify(*(formulas[0]))) {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Simplifying the model was not successfull.");
                }
                result = simplifier.getSimplifiedModel();
            } else if (model->isOfType(storm::models::ModelType::Mdp)) {
                storm::transformer::SparseParametricMdpSimplifier<storm::models::sparse::Mdp<ValueType>> simplifier(*(model->template as<storm::models::sparse::Mdp<ValueType>>()));

                std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(input.properties);
                STORM_LOG_THROW(formulas.begin()!=formulas.end(), storm::exceptions::NotSupportedException, "Only one formula at the time supported");

                if (!simplifier.simplify(*(formulas[0]))) {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Simplifying the model was not successfull.");
                }
                result = simplifier.getSimplifiedModel();
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Unable to perform monotonicity analysis on the provided model type.");
            }

            simplifyingWatch.stop();
            STORM_PRINT(std::endl << "Time for model simplification: " << simplifyingWatch << "." << std::endl << std::endl);
            result->printModelInformationToStream(std::cout);
            return result;
        }

        template <typename ValueType>
        PreprocessResult preprocessSparseModel(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input, storm::cli::ModelProcessingInformation const& mpi) {
            auto bisimulationSettings = storm::settings::getModule<storm::settings::modules::BisimulationSettings>();
            auto parametricSettings = storm::settings::getModule<storm::settings::modules::ParametricSettings>();
            auto transformationSettings = storm::settings::getModule<storm::settings::modules::TransformationSettings>();
            auto monSettings = storm::settings::getModule<storm::settings::modules::MonotonicitySettings>();
            auto derSettings = storm::settings::getModule<storm::settings::modules::DerivativeSettings>();

            PreprocessResult result(model, false);
            if (monSettings.isMonotonicityAnalysisSet() || parametricSettings.isUseMonotonicitySet() || derSettings.isFeasibleInstantiationSearchSet() || derSettings.getDerivativeAtInstantiation()) {
                result.model = storm::pars::simplifyModel<ValueType>(result.model, input);
                result.changed = true;
            }
            
            if (result.model->isOfType(storm::models::ModelType::MarkovAutomaton)) {
                result.model = storm::cli::preprocessSparseMarkovAutomaton(result.model->template as<storm::models::sparse::MarkovAutomaton<ValueType>>());
                result.changed = true;
            }

            if (mpi.applyBisimulation) {
                result.model = storm::cli::preprocessSparseModelBisimulation(result.model->template as<storm::models::sparse::Model<ValueType>>(), input, bisimulationSettings);
                result.changed = true;
            }

            if (transformationSettings.isChainEliminationSet() &&
                model->isOfType(storm::models::ModelType::MarkovAutomaton)) {
                auto eliminationResult = storm::api::eliminateNonMarkovianChains(
                        result.model->template as<storm::models::sparse::MarkovAutomaton<ValueType>>(),
                        storm::api::extractFormulasFromProperties(input.properties),
                        transformationSettings.getLabelBehavior());
                result.model = eliminationResult.first;
                // Set transformed properties as new properties in input
                result.formulas = eliminationResult.second;
                result.changed = true;
            }
            
            if (parametricSettings.transformContinuousModel() && (model->isOfType(storm::models::ModelType::Ctmc) || model->isOfType(storm::models::ModelType::MarkovAutomaton))) {
                auto transformResult = storm::api::transformContinuousToDiscreteTimeSparseModel(std::move(*model->template as<storm::models::sparse::Model<ValueType>>()), storm::api::extractFormulasFromProperties(input.properties));
                result.model = transformResult.first;
                // Set transformed properties as new properties in input
                result.formulas = transformResult.second;
                result.changed = true;
            }

            if (monSettings.isSccEliminationSet()) {
                result.model = storm::pars::eliminateScc<ValueType>(result.model);
                result.changed = true;
            }

            return result;
        }

        template <storm::dd::DdType DdType, typename ValueType>
        PreprocessResult preprocessDdModel(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model, SymbolicInput const& input, storm::cli::ModelProcessingInformation const& mpi) {
            auto bisimulationSettings = storm::settings::getModule<storm::settings::modules::BisimulationSettings>();
            
            PreprocessResult result(model, false);

            if (mpi.engine == storm::utility::Engine::Hybrid) {
                // Currently, hybrid engine for parametric models just refers to building the model symbolically.
                STORM_LOG_INFO("Translating symbolic model to sparse model...");
                result.model = storm::api::transformSymbolicToSparseModel(model);
                result.changed = true;
                // Invoke preprocessing on the sparse model
                PreprocessResult sparsePreprocessingResult = storm::pars::preprocessSparseModel<ValueType>(result.model->as<storm::models::sparse::Model<ValueType>>(), input, mpi);
                if (sparsePreprocessingResult.changed) {
                    result.model = sparsePreprocessingResult.model;
                    result.formulas = sparsePreprocessingResult.formulas;
                }
            } else {
                STORM_LOG_ASSERT(mpi.engine == storm::utility::Engine::Dd, "Expected Dd engine.");
                if (mpi.applyBisimulation) {
                    result.model = storm::cli::preprocessDdModelBisimulation(result.model->template as<storm::models::symbolic::Model<DdType, ValueType>>(), input, bisimulationSettings, mpi);
                    result.changed = true;
                }
            }
            return result;
        }

        template <storm::dd::DdType DdType, typename ValueType>
        PreprocessResult preprocessModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input, storm::cli::ModelProcessingInformation const& mpi) {
            storm::utility::Stopwatch preprocessingWatch(true);

            PreprocessResult result(model, false);
            if (model->isSparseModel()) {
                result = storm::pars::preprocessSparseModel<ValueType>(result.model->as<storm::models::sparse::Model<ValueType>>(), input, mpi);
            } else {
                STORM_LOG_ASSERT(model->isSymbolicModel(), "Unexpected model type.");
                result = storm::pars::preprocessDdModel<DdType, ValueType>(result.model->as<storm::models::symbolic::Model<DdType, ValueType>>(), input, mpi);
            }

            if (result.changed) {
                STORM_PRINT_AND_LOG(std::endl << "Time for model preprocessing: " << preprocessingWatch << "." << std::endl << std::endl);
            }
            return result;
        }

        template<typename ValueType>
        void printInitialStatesResult(std::unique_ptr<storm::modelchecker::CheckResult> const& result, storm::jani::Property const& property, storm::utility::Stopwatch* watch = nullptr, storm::utility::parametric::Valuation<ValueType> const* valuation = nullptr) {
            if (result) {
                STORM_PRINT_AND_LOG("Result (initial states)");
                if (valuation) {
                    bool first = true;
                    std::stringstream ss;
                    for (auto const& entry : *valuation) {
                        if (!first) {
                            ss << ", ";
                        } else {
                            first = false;
                        }
                        ss << entry.first << "=" << entry.second;
                    }

                    STORM_PRINT_AND_LOG(" for instance [" << ss.str() << "]");
                }
                STORM_PRINT_AND_LOG(": ")

                auto const* regionCheckResult = dynamic_cast<storm::modelchecker::RegionCheckResult<ValueType> const*>(result.get());
                if (regionCheckResult != nullptr) {
                    auto regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
                    std::stringstream outStream;
                    if (regionSettings.isPrintFullResultSet()) {
                        regionCheckResult->writeToStream(outStream);
                    } else {
                        regionCheckResult->writeCondensedToStream(outStream);
                    }
                    outStream << std::endl;
                    if (!regionSettings.isPrintNoIllustrationSet()) {
                        auto const* regionRefinementCheckResult = dynamic_cast<storm::modelchecker::RegionRefinementCheckResult<ValueType> const*>(regionCheckResult);
                        if (regionRefinementCheckResult != nullptr) {
                            regionRefinementCheckResult->writeIllustrationToStream(outStream);
                        }
                    }
                    outStream << std::endl;
                    STORM_PRINT_AND_LOG(outStream.str());
                } else {
                    STORM_PRINT_AND_LOG(*result << std::endl);
                }
                if (watch) {
                    STORM_PRINT_AND_LOG("Time for model checking: " << *watch << "." << std::endl << std::endl);
                }
            } else {
                STORM_LOG_ERROR("Property is unsupported by selected engine/settings." << std::endl);
            }
        }

        template<typename ValueType>
        void verifyProperties(std::vector<storm::jani::Property> const& properties, std::function<std::unique_ptr<storm::modelchecker::CheckResult>(std::shared_ptr<storm::logic::Formula const> const& formula)> const& verificationCallback, std::function<void(std::unique_ptr<storm::modelchecker::CheckResult> const&)> const& postprocessingCallback) {
            for (auto const& property : properties) {
                storm::cli::printModelCheckingProperty(property);
                storm::utility::Stopwatch watch(true);
                std::unique_ptr<storm::modelchecker::CheckResult> result = verificationCallback(property.getRawFormula());
                watch.stop();
                printInitialStatesResult<ValueType>(result, property, &watch);
                postprocessingCallback(result);
            }
        }

        template<template<typename, typename> class ModelCheckerType, typename ModelType, typename ValueType, typename SolveValueType = double>
        void verifyPropertiesAtSamplePoints(ModelType const& model, SymbolicInput const& input, SampleInformation<ValueType> const& samples) {

            // When samples are provided, we create an instantiation model checker.
            ModelCheckerType<ModelType, SolveValueType> modelchecker(model);

            for (auto const& property : input.properties) {
                storm::cli::printModelCheckingProperty(property);

                modelchecker.specifyFormula(storm::api::createTask<ValueType>(property.getRawFormula(), true));
                modelchecker.setInstantiationsAreGraphPreserving(samples.graphPreserving);

                storm::utility::parametric::Valuation<ValueType> valuation;

                std::vector<typename utility::parametric::VariableType<ValueType>::type> parameters;
                std::vector<typename std::vector<typename utility::parametric::CoefficientType<ValueType>::type>::const_iterator> iterators;
                std::vector<typename std::vector<typename utility::parametric::CoefficientType<ValueType>::type>::const_iterator> iteratorEnds;

                storm::utility::Stopwatch watch(true);
                for (auto const& product : samples.cartesianProducts) {
                    parameters.clear();
                    iterators.clear();
                    iteratorEnds.clear();

                    for (auto const& entry : product) {
                        parameters.push_back(entry.first);
                        iterators.push_back(entry.second.cbegin());
                        iteratorEnds.push_back(entry.second.cend());
                    }

                    bool done = false;
                    while (!done) {
                        // Read off valuation.
                        for (uint64_t i = 0; i < parameters.size(); ++i) {
                            valuation[parameters[i]] = *iterators[i];
                        }

                        storm::utility::Stopwatch valuationWatch(true);
                        std::unique_ptr<storm::modelchecker::CheckResult> result = modelchecker.check(Environment(), valuation);
                        valuationWatch.stop();

                        if (result) {
                            result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model.getInitialStates()));
                        }
                        printInitialStatesResult<ValueType>(result, property, &valuationWatch, &valuation);

                        for (uint64_t i = 0; i < parameters.size(); ++i) {
                            ++iterators[i];
                            if (iterators[i] == iteratorEnds[i]) {
                                // Reset iterator and proceed to move next iterator.
                                iterators[i] = product.at(parameters[i]).cbegin();

                                // If the last iterator was removed, we are done.
                                if (i == parameters.size() - 1) {
                                    done = true;
                                }
                            } else {
                                // If an iterator was moved but not reset, we have another valuation to check.
                                break;
                            }
                        }

                    }
                }

                watch.stop();
                STORM_PRINT_AND_LOG("Overall time for sampling all instances: " << watch << std::endl << std::endl);
            }
        }

        template <typename ValueType, typename SolveValueType = double>
        void verifyPropertiesAtSamplePoints(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input, SampleInformation<ValueType> const& samples) {
            if (model->isOfType(storm::models::ModelType::Dtmc)) {
                verifyPropertiesAtSamplePoints<storm::modelchecker::SparseDtmcInstantiationModelChecker, storm::models::sparse::Dtmc<ValueType>, ValueType, SolveValueType>(*model->template as<storm::models::sparse::Dtmc<ValueType>>(), input, samples);
            } else if (model->isOfType(storm::models::ModelType::Ctmc)) {
                verifyPropertiesAtSamplePoints<storm::modelchecker::SparseCtmcInstantiationModelChecker, storm::models::sparse::Ctmc<ValueType>, ValueType, SolveValueType>(*model->template as<storm::models::sparse::Ctmc<ValueType>>(), input, samples);
            } else if (model->isOfType(storm::models::ModelType::Ctmc)) {
                verifyPropertiesAtSamplePoints<storm::modelchecker::SparseMdpInstantiationModelChecker, storm::models::sparse::Mdp<ValueType>, ValueType, SolveValueType>(*model->template as<storm::models::sparse::Mdp<ValueType>>(), input, samples);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Sampling is currently only supported for DTMCs, CTMCs and MDPs.");
            }
        }

        template <typename ValueType>
        void verifyPropertiesWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input, SampleInformation<ValueType> const& samples) {

            if (samples.empty()) {
                verifyProperties<ValueType>(input.properties,
                                            [&model] (std::shared_ptr<storm::logic::Formula const> const& formula) {
                                                std::unique_ptr<storm::modelchecker::CheckResult> result = storm::api::verifyWithSparseEngine<ValueType>(model, storm::api::createTask<ValueType>(formula, true));
                                                if (result) {
                                                    result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                                                }
                                                return result;
                                            },
                                            [&model] (std::unique_ptr<storm::modelchecker::CheckResult> const& result) {
                                                auto parametricSettings = storm::settings::getModule<storm::settings::modules::ParametricSettings>();
                                                if (parametricSettings.exportResultToFile() && model->isOfType(storm::models::ModelType::Dtmc)) {
                                                    auto dtmc = model->template as<storm::models::sparse::Dtmc<ValueType>>();
                                                    boost::optional<ValueType> rationalFunction = result->asExplicitQuantitativeCheckResult<ValueType>()[*model->getInitialStates().begin()];
                                                    storm::api::exportParametricResultToFile(rationalFunction, storm::analysis::ConstraintCollector<ValueType>(*dtmc), parametricSettings.exportResultPath());
                                                }
                                                else if (parametricSettings.exportResultToFile() && model->isOfType(storm::models::ModelType::Ctmc)) {
                                                    auto ctmc = model->template as<storm::models::sparse::Ctmc<ValueType>>();
                                                    boost::optional<ValueType> rationalFunction = result->asExplicitQuantitativeCheckResult<ValueType>()[*model->getInitialStates().begin()];
                                                    storm::api::exportParametricResultToFile(rationalFunction, storm::analysis::ConstraintCollector<ValueType>(*ctmc), parametricSettings.exportResultPath());
                                                }
                                            });
            } else {
                STORM_LOG_TRACE("Sampling the model at given points.");

                if (samples.exact) {
                    verifyPropertiesAtSamplePoints<ValueType, storm::RationalNumber>(model, input, samples);
                } else {
                    verifyPropertiesAtSamplePoints<ValueType, double>(model, input, samples);
                }
            }
        }

        template <typename ValueType>
        void performGradientDescent(std::shared_ptr<storm::models::sparse::Model<ValueType>> model, SymbolicInput const& input, boost::optional<std::set<RationalFunctionVariable>> omittedParameters) {
            STORM_LOG_THROW(model->isOfType(storm::models::ModelType::Dtmc), storm::exceptions::NotSupportedException, "Derivative currently only supported for DTMCs.");
            std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> dtmc = model->template as<storm::models::sparse::Dtmc<ValueType>>();

            std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(input.properties);
            auto derSettings = storm::settings::getModule<storm::settings::modules::DerivativeSettings>();
            auto formula = formulas[0];

            boost::optional<std::string> rewardModel;
            if (formula->isProbabilityOperatorFormula()) {
                rewardModel = boost::none;
            } else if (formula->isRewardOperatorFormula()) {
                if (formula->asRewardOperatorFormula().hasRewardModelName()) {
                    rewardModel = std::string(formula->asRewardOperatorFormula().getRewardModelName());
                } else {
                    rewardModel = std::string("");
                }
            } else {
                STORM_LOG_ERROR("Input formula needs to be either a probability operator formula or a reward operator formula.");
                return;
            }

            auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);
            if (rewardModel.is_initialized()) {
                for (auto const& rewardParameter : storm::models::sparse::getRewardParameters(*dtmc)) {
                    vars.insert(rewardParameter);
                }
            }

            std::cout << "Parameters: ";
            for (auto const& entry : vars) {
                std::cout << entry << " ";
            }
            std::cout << std::endl;

            if (omittedParameters && !omittedParameters->empty()) {
                std::cout << "Parameters ";
                for (auto const& entry : *omittedParameters) {
                    std::cout << entry << " ";
                }
                std::cout << "are inconsequential.";
                if (derSettings.areInconsequentialParametersOmitted()) {
                    std::cout << " They will be omitted in the found instantiation." << std::endl;
                } else {
                    std::cout << " They will be set to 0.5 in the found instantiation. To omit them, set the flag --omit-inconsequential-params." << std::endl;
                }
            }

            boost::optional<derivative::GradientDescentMethod> method = derSettings.getGradientDescentMethod();
            if (!method) {
                STORM_LOG_ERROR("Unknown Gradient Descent method: " << derSettings.getGradientDescentMethodAsString());
                return;
            }

            boost::optional<std::map<typename utility::parametric::VariableType<ValueType>::type, typename utility::parametric::CoefficientType<ValueType>::type>> startPoint;
            auto startPointAsString = derSettings.getStartPoint();
            if (startPointAsString) {
                auto samples = parseSamples<ValueType>(model, *startPointAsString, true);
                std::map<typename utility::parametric::VariableType<ValueType>::type, std::vector<typename utility::parametric::CoefficientType<ValueType>::type>> cartesianProduct = samples.cartesianProducts[0];
                std::map<typename utility::parametric::VariableType<ValueType>::type, typename utility::parametric::CoefficientType<ValueType>::type> point;
                for (auto const& entry : cartesianProduct) {
                    point[entry.first] = entry.second[0];
                }
                startPoint = point;
            }

            // Use the SparseDerivativeInstantiationModelChecker to retrieve the derivative at an instantiation that is input by the user
            if (auto instantiationString = derSettings.getDerivativeAtInstantiation()) {
                std::unordered_map<std::string, std::string> keyValue = storm::parser::parseKeyValueString(*instantiationString);
                std::map<typename utility::parametric::VariableType<ValueType>::type, typename utility::parametric::CoefficientType<ValueType>::type> instantiation;
                for (auto const& pair : keyValue) {
                    auto variable = carl::VariablePool::getInstance().findVariableWithName(pair.first);
                    auto value = storm::utility::convertNumber<typename utility::parametric::CoefficientType<ValueType>::type>(pair.second);
                    instantiation.emplace(variable, value);
                }

                derivative::SparseDerivativeInstantiationModelChecker<ValueType, storm::RationalNumber> modelChecker(*dtmc);

                // TODO Make Initial State flexible
                uint_fast64_t initialState;           
                const storm::storage::BitVector initialVector = dtmc->getStates("init");
                for (uint_fast64_t x : initialVector) {
                    initialState = x;
                    break;
                }
                
                modelchecker::CheckTask<storm::logic::Formula, storm::RationalNumber> referenceCheckTask(*formula);
                std::shared_ptr<storm::logic::Formula> formulaWithoutBound;
                if (!referenceCheckTask.isRewardModelSet()) {
                    formulaWithoutBound = std::make_shared<storm::logic::ProbabilityOperatorFormula>(
                        formulas[0]->asProbabilityOperatorFormula().getSubformula().asSharedPointer(), storm::logic::OperatorInformation(boost::none, boost::none));
                } else {
                    // No worries, this works as intended, the API is just weird.
                    formulaWithoutBound = std::make_shared<storm::logic::RewardOperatorFormula>(
                            formulas[0]->asRewardOperatorFormula().getSubformula().asSharedPointer());
                }
                const storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> checkTask
                    = storm::modelchecker::CheckTask<storm::logic::Formula, ValueType>(*formulaWithoutBound);
                modelChecker.specifyFormula(Environment(), checkTask);

                for (auto const& parameter : vars) {
                    std::cout << "Derivative w.r.t. " << parameter << ": ";
                    
                    auto result = modelChecker.check(Environment(), instantiation, parameter);
                    std::cout << *result << std::endl;
                }
                return;
            } else if (derSettings.isFeasibleInstantiationSearchSet()) {
                STORM_PRINT("Finding an extremum using Gradient Descent" << std::endl);
                storm::utility::Stopwatch derivativeWatch(true);
                storm::derivative::GradientDescentInstantiationSearcher<storm::RationalFunction, double> derivativeChecker(*dtmc, *method, derSettings.getLearningRate(), derSettings.getAverageDecay(), derSettings.getSquaredAverageDecay(), derSettings.getMiniBatchSize(), derSettings.getTerminationEpsilon(), startPoint, derSettings.isPrintJsonSet());
                storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> checkTask(*formula);
                derivativeChecker.specifyFormula(Environment(), checkTask);
                auto instantiationAndValue = derivativeChecker.gradientDescent(Environment());
                if (!derSettings.areInconsequentialParametersOmitted() && omittedParameters) {
                    for (RationalFunctionVariable const& param : *omittedParameters) {
                        if (startPoint) {
                            instantiationAndValue.first[param] = startPoint->at(param);
                        } else {
                            instantiationAndValue.first[param] = utility::convertNumber<RationalFunction::CoeffType>(0.5);
                        }
                    }
                }
                derivativeWatch.stop();
                if (derSettings.isPrintJsonSet()) {
                    derivativeChecker.printRunAsJson();
                } else {
                    std::cout << "Found value " << instantiationAndValue.second << " at instantiation " << std::endl;
                    bool isFirstLoop = true;
                    for (auto const& p : instantiationAndValue.first) {
                        if (!isFirstLoop) {
                            std::cout << ",";
                        }
                        isFirstLoop = false;
                        std::cout << p.first << "=" << p.second;
                    }
                    std::cout << std::endl;
                }
                std::cout << "Finished in " << derivativeWatch << std::endl;
                return;
            }
        }

        template <typename ValueType>
        void analyzeMonotonicity(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input, std::vector<storm::storage::ParameterRegion<ValueType>> const& regions) {
            std::ofstream outfile;
            auto monSettings = storm::settings::getModule<storm::settings::modules::MonotonicitySettings>();

            if (monSettings.isExportMonotonicitySet()) {
                utility::openFile(monSettings.getExportMonotonicityFilename(), outfile);
            }
            std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(input.properties);
            storm::utility::Stopwatch monotonicityWatch(true);
            STORM_LOG_THROW(regions.size() <= 1, storm::exceptions::InvalidArgumentException, "Monotonicity analysis only allowed on single region");
            if (!monSettings.isMonSolutionSet()) {
                auto monotonicityHelper = storm::analysis::MonotonicityHelper<ValueType, double>(model, formulas, regions, monSettings.getNumberOfSamples(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision(), monSettings.isDotOutputSet());
                if (monSettings.isExportMonotonicitySet()) {
                    monotonicityHelper.checkMonotonicityInBuild(outfile, monSettings.isUsePLABoundsSet(), monSettings.getDotOutputFilename());
                } else {
                    monotonicityHelper.checkMonotonicityInBuild(std::cout, monSettings.isUsePLABoundsSet(), monSettings.getDotOutputFilename());
                }
            } else {
                // Checking monotonicity based on solution function

                auto parametricSettings = storm::settings::getModule<storm::settings::modules::ParametricSettings>();
                auto regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();

                std::function<std::unique_ptr<storm::modelchecker::CheckResult>(std::shared_ptr<storm::logic::Formula const> const& formula)> verificationCallback;
                std::function<void(std::unique_ptr<storm::modelchecker::CheckResult> const&)> postprocessingCallback;

                // Check the given set of regions with or without refinement
                verificationCallback = [&] (std::shared_ptr<storm::logic::Formula const> const& formula) {
                    std::unique_ptr<storm::modelchecker::CheckResult> result = storm::api::verifyWithSparseEngine<ValueType>(model, storm::api::createTask<ValueType>(formula, true));
                    return result;
                };

                for (auto & property : input.properties) {
                    auto result = verificationCallback(property.getRawFormula())->asExplicitQuantitativeCheckResult<ValueType>().getValueVector();
                    ValueType valuation;

                    auto states= model->getInitialStates();
                    for (auto state : states) {
                        valuation += result[state];
                    }

                    storm::analysis::MonotonicityResult<storm::RationalFunctionVariable> monRes;
                    for (auto & var : storm::models::sparse::getProbabilityParameters(*model)) {
                        auto res = storm::analysis::MonotonicityChecker<ValueType>::checkDerivative(valuation.derivative(var), regions[0]);

                        if (res.first && res.second) {
                            monRes.addMonotonicityResult(var, analysis::MonotonicityResult<storm::RationalFunctionVariable>::Monotonicity::Constant);
                        } else if (res.first) {
                            monRes.addMonotonicityResult(var, analysis::MonotonicityResult<storm::RationalFunctionVariable>::Monotonicity::Incr);
                        } else if (res.second) {
                            monRes.addMonotonicityResult(var, analysis::MonotonicityResult<storm::RationalFunctionVariable>::Monotonicity::Decr);
                        } else {
                            monRes.addMonotonicityResult(var, analysis::MonotonicityResult<storm::RationalFunctionVariable>::Monotonicity::Not);
                        }
                    }
                    if (monSettings.isExportMonotonicitySet()) {
                        outfile << monRes.toString();
                    } else {
                        STORM_PRINT(monRes.toString());
                    }
                }
            }

            if (monSettings.isExportMonotonicitySet()) {
                utility::closeFile(outfile);
            }

            monotonicityWatch.stop();
            STORM_PRINT(std::endl << "Total time for monotonicity checking: " << monotonicityWatch << "." << std::endl << std::endl);
            return;
        }

        template <typename ValueType>
        void computeRegionExtremumWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input, std::vector<storm::storage::ParameterRegion<ValueType>> const& regions, storm::api::MonotonicitySetting monotonicitySettings = storm::api::MonotonicitySetting(), boost::optional<std::pair<std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>, std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>>>& monotoneParameters = boost::none) {
            STORM_LOG_ASSERT(!regions.empty(), "Can not analyze an empty set of regions.");
            auto regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
            auto monSettings = storm::settings::getModule<storm::settings::modules::MonotonicitySettings>();
            auto engine = regionSettings.getRegionCheckEngine();
            storm::solver::OptimizationDirection direction = regionSettings.getExtremumDirection();
            ValueType precision = storm::utility::convertNumber<ValueType>(regionSettings.getExtremumValuePrecision());
            bool generateSplitEstimates = regionSettings.isSplittingThresholdSet();
            for (auto const& property : input.properties) {
                for (auto const& region : regions) {
                    if (monotonicitySettings.useMonotonicity) {
                        STORM_PRINT_AND_LOG("Computing extremal value for property " << property.getName() << ": "
                                                                                     << *property.getRawFormula()
                                                                                     << " within region " << region
                                                                                     << " and using monotonicity ..." << std::endl);
                    } else {
                        STORM_PRINT_AND_LOG("Computing extremal value for property " << property.getName() << ": "
                                                                                     << *property.getRawFormula()
                                                                                     << " within region " << region
                                                                                     << "..." << std::endl);
                    }
                    storm::utility::Stopwatch watch(true);
                    // TODO: hier eventueel checkExtremalValue van maken
                    if (regionSettings.isExtremumSuggestionSet()) {
                        ValueType suggestion = storm::utility::convertNumber<ValueType>(regionSettings.getExtremumSuggestion());
                        if (storm::api::checkExtremalValue<ValueType>(model, storm::api::createTask<ValueType>(property.getRawFormula(), true), region, engine, direction, precision, suggestion, monotonicitySettings, generateSplitEstimates, monotoneParameters)) {
                            STORM_PRINT_AND_LOG(suggestion << " is the extremum ");
                        } else {
                            STORM_PRINT_AND_LOG(suggestion << " is NOT the extremum ");
                        }

                    } else {
                        auto valueValuation = storm::api::computeExtremalValue<ValueType>(model, storm::api::createTask<ValueType>(property.getRawFormula(), true), region, engine, direction, precision, monotonicitySettings, generateSplitEstimates, monotoneParameters);
                        watch.stop();
                        std::stringstream valuationStr;
                        bool first = true;
                        for (auto const& v : valueValuation.second) {
                            if (first) {
                                first = false;
                            } else {
                                valuationStr << ", ";
                            }
                            valuationStr << v.first << "=" << v.second;
                        }
                        STORM_PRINT_AND_LOG("Result at initial state: " << valueValuation.first << " ( approx. " << storm::utility::convertNumber<double>(valueValuation.first) << ") at [" << valuationStr.str() << "]." << std::endl)
                        STORM_PRINT_AND_LOG("Time for model checking: " << watch << "." << std::endl);
                    }
                }
            }
        }
        
        template <typename ValueType>
        void verifyRegionsWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input, std::vector<storm::storage::ParameterRegion<ValueType>> const& regions, storm::api::MonotonicitySetting monotonicitySettings = storm::api::MonotonicitySetting(), uint64_t monThresh = 0) {
            STORM_LOG_ASSERT(!regions.empty(), "Can not analyze an empty set of regions.");

            auto parametricSettings = storm::settings::getModule<storm::settings::modules::ParametricSettings>();
            auto regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();

            std::function<std::unique_ptr<storm::modelchecker::CheckResult>(std::shared_ptr<storm::logic::Formula const> const& formula)> verificationCallback;
            std::function<void(std::unique_ptr<storm::modelchecker::CheckResult> const&)> postprocessingCallback;

            STORM_PRINT_AND_LOG(std::endl);
            if (regionSettings.isHypothesisSet()) {
                STORM_PRINT_AND_LOG("Checking hypothesis " << regionSettings.getHypothesis() << " on ");
            } else {
                STORM_PRINT_AND_LOG("Analyzing ");
            }
            if (regions.size() == 1) {
                STORM_PRINT_AND_LOG("parameter region " << regions.front());
            } else {
                STORM_PRINT_AND_LOG(regions.size() << " parameter regions");
            }
            auto engine = regionSettings.getRegionCheckEngine();
            STORM_PRINT_AND_LOG(" using " << engine);
            if (monotonicitySettings.useMonotonicity) {
                STORM_PRINT_AND_LOG(" with local monotonicity and");
            }

            // Check the given set of regions with or without refinement
            if (regionSettings.isRefineSet()) {
                STORM_LOG_THROW(regions.size() == 1, storm::exceptions::NotSupportedException, "Region refinement is not supported for multiple initial regions.");
                STORM_PRINT_AND_LOG(" with iterative refinement until " << (1.0 - regionSettings.getCoverageThreshold()) * 100.0 << "% is covered." << (regionSettings.isDepthLimitSet() ? " Depth limit is " + std::to_string(regionSettings.getDepthLimit()) + "." : "") << std::endl);
                verificationCallback = [&] (std::shared_ptr<storm::logic::Formula const> const& formula) {
                    ValueType refinementThreshold = storm::utility::convertNumber<ValueType>(regionSettings.getCoverageThreshold());
                    boost::optional<uint64_t> optionalDepthLimit;
                    if (regionSettings.isDepthLimitSet()) {
                        optionalDepthLimit = regionSettings.getDepthLimit();
                    }
                    // TODO @Jip: change allow model simplification when not using monotonicity, for benchmarking purposes simplification is moved forward.
                    std::unique_ptr<storm::modelchecker::RegionRefinementCheckResult<ValueType>> result = storm::api::checkAndRefineRegionWithSparseEngine<ValueType>(model, storm::api::createTask<ValueType>(formula, true), regions.front(), engine, refinementThreshold, optionalDepthLimit, regionSettings.getHypothesis(), false, monotonicitySettings, monThresh);
                    return result;
                };
            } else {
                STORM_PRINT_AND_LOG("." << std::endl);
                verificationCallback = [&] (std::shared_ptr<storm::logic::Formula const> const& formula) {
                    std::unique_ptr<storm::modelchecker::CheckResult> result = storm::api::checkRegionsWithSparseEngine<ValueType>(model, storm::api::createTask<ValueType>(formula, true), regions, engine, regionSettings.getHypothesis());
                    return result;
                };
            }

            postprocessingCallback = [&] (std::unique_ptr<storm::modelchecker::CheckResult> const& result) {
                if (parametricSettings.exportResultToFile()) {
                    storm::api::exportRegionCheckResultToFile<ValueType>(result, parametricSettings.exportResultPath());
                }
            };

            verifyProperties<ValueType>(input.properties, verificationCallback, postprocessingCallback);
        }

        template <typename ValueType>
        void verifyWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input, std::vector<storm::storage::ParameterRegion<ValueType>> const& regions, SampleInformation<ValueType> const& samples, storm::api::MonotonicitySetting monotonicitySettings = storm::api::MonotonicitySetting(), boost::optional<std::pair<std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>, std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>>>& monotoneParameters = boost::none, uint64_t monThresh = 0, boost::optional<std::set<RationalFunctionVariable>> omittedParameters = boost::none) {
            auto derSettings = storm::settings::getModule<storm::settings::modules::DerivativeSettings>();
            if (derSettings.isFeasibleInstantiationSearchSet() || derSettings.getDerivativeAtInstantiation()) {
                    storm::pars::performGradientDescent<ValueType>(model, input, omittedParameters);
                    return;
            }

            if (regions.empty()) {
                storm::pars::verifyPropertiesWithSparseEngine(model, input, samples);
            } else {
                auto regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
                auto monSettings = storm::settings::getModule<storm::settings::modules::MonotonicitySettings>();
                if (monSettings.isMonotonicityAnalysisSet()) {
                    storm::pars::analyzeMonotonicity(model, input, regions);
                } else if (regionSettings.isExtremumSet()) {
                    storm::pars::computeRegionExtremumWithSparseEngine(model, input, regions, monotonicitySettings, monotoneParameters);
                } else {
                    assert (monotoneParameters == boost::none);
                    assert (!monotonicitySettings.useOnlyGlobalMonotonicity);
                    assert (!monotonicitySettings.useBoundsFromPLA);
                    storm::pars::verifyRegionsWithSparseEngine(model, input, regions, monotonicitySettings, monThresh);
                }
            }
        }

        template <storm::dd::DdType DdType, typename ValueType>
        void verifyPropertiesWithSymbolicEngine(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model, SymbolicInput const& input, SampleInformation<ValueType> const& samples) {
            if (samples.empty()) {
                verifyProperties<ValueType>(input.properties,
                                            [&model] (std::shared_ptr<storm::logic::Formula const> const& formula) {
                                                std::unique_ptr<storm::modelchecker::CheckResult> result = storm::api::verifyWithDdEngine<DdType, ValueType>(model, storm::api::createTask<ValueType>(formula, true));
                                                if (result) {
                                                    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<DdType>(model->getReachableStates(), model->getInitialStates()));
                                                }
                                                return result;
                                            },
                                            [&model] (std::unique_ptr<storm::modelchecker::CheckResult> const& result) {
                                                auto parametricSettings = storm::settings::getModule<storm::settings::modules::ParametricSettings>();
                                                if (parametricSettings.exportResultToFile() && model->isOfType(storm::models::ModelType::Dtmc)) {
                                                    //auto dtmc = model->template as<storm::models::symbolic::Dtmc<DdType, ValueType>>();
                                                    //boost::optional<ValueType> rationalFunction = result->asSymbolicQuantitativeCheckResult<DdType, ValueType>().sum();
                                                    //storm::api::exportParametricResultToFile(rationalFunction, storm::analysis::ConstraintCollector<ValueType>(*dtmc), parametricSettings.exportResultPath());
                                                }
                                            });
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Sampling is not supported in the symbolic engine.");
            }
        }

        template <storm::dd::DdType DdType, typename ValueType>
        void verifyWithDdEngine(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model, SymbolicInput const& input, std::vector<storm::storage::ParameterRegion<ValueType>> const& regions, SampleInformation<ValueType> const& samples) {
            if (regions.empty()) {
                storm::pars::verifyPropertiesWithSymbolicEngine(model, input, samples);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Region verification is not supported in the symbolic engine.");
            }
        }

        template <storm::dd::DdType DdType, typename ValueType>
        void verifyParametricModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input, std::vector<storm::storage::ParameterRegion<ValueType>> const& regions, SampleInformation<ValueType> const& samples, storm::api::MonotonicitySetting monotonicitySettings = storm::api::MonotonicitySetting(), boost::optional<std::pair<std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>, std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>>>& monotoneParameters = boost::none, uint64_t monThresh = 0, boost::optional<std::set<RationalFunctionVariable>> omittedParameters = boost::none) {
            if (model->isSparseModel()) {
                storm::pars::verifyWithSparseEngine<ValueType>(model->as<storm::models::sparse::Model<ValueType>>(), input, regions, samples, monotonicitySettings, monotoneParameters, monThresh, omittedParameters);
            } else {
                assert (!monotonicitySettings.useMonotonicity);
                assert (monotoneParameters == boost::none);
                storm::pars::verifyWithDdEngine<DdType, ValueType>(model->as<storm::models::symbolic::Model<DdType, ValueType>>(), input, regions, samples);
            }
        }

        template <storm::dd::DdType DdType, typename ValueType>
        void processInputWithValueTypeAndDdlib(SymbolicInput& input, storm::cli::ModelProcessingInformation const& mpi) {
            auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
            auto buildSettings = storm::settings::getModule<storm::settings::modules::BuildSettings>();
            auto parSettings = storm::settings::getModule<storm::settings::modules::ParametricSettings>();
            auto monSettings = storm::settings::getModule<storm::settings::modules::MonotonicitySettings>();

            STORM_LOG_THROW(mpi.engine == storm::utility::Engine::Sparse || mpi.engine == storm::utility::Engine::Hybrid || mpi.engine == storm::utility::Engine::Dd, storm::exceptions::InvalidSettingsException, "The selected engine is not supported for parametric models.");

            std::shared_ptr<storm::models::ModelBase> model;
            if (!buildSettings.isNoBuildModelSet()) {
                model = storm::cli::buildModel<DdType, ValueType>(input, ioSettings, mpi);
            }

            if (model) {
                model->printModelInformationToStream(std::cout);
            }

            STORM_LOG_THROW(model || input.properties.empty(), storm::exceptions::InvalidSettingsException, "No input model.");


            // If minimization is active and the model is parametric, parameters might be minimized away because they are inconsequential.
            // This is the set of all such inconsequential parameters.
            std::set<RationalFunctionVariable> omittedParameters;

            if (model) {
                auto preprocessingResult = storm::pars::preprocessModel<DdType, ValueType>(model, input, mpi);
                if (preprocessingResult.changed) {
                    if (model->isOfType(models::ModelType::Dtmc) || model->isOfType(models::ModelType::Mdp)) {
                        auto const previousParams = storm::models::sparse::getAllParameters(*model->template as<storm::models::sparse::Model<ValueType>>());
                        auto const currentParams = storm::models::sparse::getAllParameters(*(preprocessingResult.model)->template as<storm::models::sparse::Model<ValueType>>());
                        for (auto const& variable : previousParams) {
                            if (!currentParams.count(variable)) {
                                omittedParameters.insert(variable);
                            }
                        }
                    }
                    model = preprocessingResult.model;

                    if (preprocessingResult.formulas) {
                        std::vector<storm::jani::Property> newProperties;
                        for (size_t i = 0; i < preprocessingResult.formulas.get().size(); ++i) {
                            auto formula = preprocessingResult.formulas.get().at(i);
                            STORM_LOG_ASSERT(i < input.properties.size(), "Index " << i << " greater than number of properties.");
                            storm::jani::Property property = input.properties.at(i);
                            newProperties.push_back(storm::jani::Property(property.getName(), formula, property.getUndefinedConstants(), property.getComment()));
                        }
                        input.properties = newProperties;
                    }

                    model->printModelInformationToStream(std::cout);
                }
            }

            std::vector<storm::storage::ParameterRegion<ValueType>> regions = parseRegions<ValueType>(model);

            std::string samplesAsString = parSettings.getSamples();
            SampleInformation<ValueType> samples;
            if (!samplesAsString.empty()) {
                samples = parseSamples<ValueType>(model, samplesAsString,
                                                  parSettings.isSamplesAreGraphPreservingSet());
                samples.exact = parSettings.isSampleExactSet();
            }

            if (model) {
                storm::cli::exportModel<DdType, ValueType>(model, input);
            }

            if (parSettings.onlyObtainConstraints()) {
                STORM_LOG_THROW(parSettings.exportResultToFile(), storm::exceptions::InvalidSettingsException,
                                "When computing constraints, export path has to be specified.");
                storm::api::exportParametricResultToFile<ValueType>(boost::none,
                                                                    storm::analysis::ConstraintCollector<ValueType>(
                                                                            *(model->as<storm::models::sparse::Model<ValueType>>())),
                                                                    parSettings.exportResultPath());
                return;
            }

            if (model) {
                boost::optional<std::pair<std::set<storm::RationalFunctionVariable>, std::set<storm::RationalFunctionVariable>>> monotoneParameters;
                if (monSettings.isMonotoneParametersSet()) {
                    monotoneParameters = std::move(
                            storm::api::parseMonotoneParameters<ValueType>(monSettings.getMonotoneParameterFilename(),
                                    model->as<storm::models::sparse::Model<ValueType>>()));
                }
// TODO: is onlyGlobalSet was used here
                verifyParametricModel<DdType, ValueType>(model, input, regions, samples, storm::api::MonotonicitySetting(parSettings.isUseMonotonicitySet(), false, monSettings.isUsePLABoundsSet()), monotoneParameters, monSettings.getMonotonicityThreshold(), omittedParameters);
            }
        }

        void processOptions() {
            // Start by setting some urgent options (log levels, resources, etc.)
            storm::cli::setUrgentOptions();
            
            auto coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
            auto engine = coreSettings.getEngine();
            STORM_LOG_WARN_COND(engine != storm::utility::Engine::Dd || engine != storm::utility::Engine::Hybrid || coreSettings.getDdLibraryType() == storm::dd::DdType::Sylvan, "The selected DD library does not support parametric models. Switching to Sylvan...");
            
            // Parse and preprocess symbolic input (PRISM, JANI, properties, etc.)
            auto symbolicInput = storm::cli::parseSymbolicInput();
            storm::cli::ModelProcessingInformation mpi;
            std::tie(symbolicInput, mpi) = storm::cli::preprocessSymbolicInput(symbolicInput);
            processInputWithValueTypeAndDdlib<storm::dd::DdType::Sylvan, storm::RationalFunction>(symbolicInput, mpi);
        }

    }
}


/*!
 * Main entry point of the executable storm-pars.
 */
int main(const int argc, const char** argv) {

    try {
        storm::utility::setUp();
        storm::cli::printHeader("Storm-pars", argc, argv);
        storm::settings::initializeParsSettings("Storm-pars", "storm-pars");

        storm::utility::Stopwatch totalTimer(true);
        if (!storm::cli::parseOptions(argc, argv)) {
            return -1;
        }

        storm::pars::processOptions();

        totalTimer.stop();
        if (storm::settings::getModule<storm::settings::modules::ResourceSettings>().isPrintTimeAndMemorySet()) {
            storm::cli::printTimeAndMemoryStatistics(totalTimer.getTimeInMilliseconds());
        }

        storm::utility::cleanUp();
        return 0;
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused Storm-pars to terminate. The message of the exception is: " << exception.what());
        return 1;
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused Storm-pars to terminate. The message of this exception is: " << exception.what());
        return 2;
    }
}
