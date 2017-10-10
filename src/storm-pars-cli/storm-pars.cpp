
#include "storm-pars/api/storm-pars.h"
#include "storm-pars/settings/ParsSettings.h"
#include "storm-pars/settings/modules/ParametricSettings.h"
#include "storm-pars/settings/modules/RegionSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/api/storm.h"
#include "storm-cli-utilities/cli.h"
#include "storm-cli-utilities/model-handling.h"
#include "storm/models/ModelBase.h"
#include "storm/storage/SymbolicModelDescription.h"
#include "storm/utility/file.h"
#include "storm/utility/initialize.h"
#include "storm/utility/Stopwatch.h"
#include "storm/utility/macros.h"

#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/modules/BisimulationSettings.h"

#include "storm/exceptions/BaseException.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace pars {
    
        typedef typename storm::cli::SymbolicInput SymbolicInput;

        template <typename ValueType>
        std::shared_ptr<storm::models::ModelBase> buildModelSparse(SymbolicInput const& input, storm::settings::modules::IOSettings const& ioSettings) {
            return storm::api::buildSparseModel<ValueType>(input.model.get(), storm::api::extractFormulasFromProperties(input.properties), ioSettings.isBuildChoiceLabelsSet());
        }

        template <storm::dd::DdType DdType, typename ValueType>
        std::shared_ptr<storm::models::ModelBase> buildModel(storm::settings::modules::CoreSettings::Engine const& engine, SymbolicInput const& input, storm::settings::modules::IOSettings const& ioSettings) {
            storm::utility::Stopwatch modelBuildingWatch(true);

            std::shared_ptr<storm::models::ModelBase> result;
            if (input.model) {
                if (engine == storm::settings::modules::CoreSettings::Engine::Dd || engine == storm::settings::modules::CoreSettings::Engine::Hybrid) {
                    result = storm::cli::buildModelDd<DdType, ValueType>(input);
                } else if (engine == storm::settings::modules::CoreSettings::Engine::Sparse) {
                    result = storm::pars::buildModelSparse<ValueType>(input, ioSettings);
                }
            } else if (ioSettings.isExplicitSet() || ioSettings.isExplicitDRNSet()) {
                STORM_LOG_THROW(engine == storm::settings::modules::CoreSettings::Engine::Sparse, storm::exceptions::InvalidSettingsException, "Can only use sparse engine with explicit input.");
                result = storm::cli::buildModelExplicit<ValueType>(ioSettings);
            }

            modelBuildingWatch.stop();
            if (result) {
                STORM_PRINT_AND_LOG("Time for model construction: " << modelBuildingWatch << "." << std::endl << std::endl);
            }

            return result;
        }

        
        template <typename ValueType>
        std::vector<storm::storage::ParameterRegion<ValueType>> parseRegions(std::shared_ptr<storm::models::ModelBase> const& model) {
            std::vector<storm::storage::ParameterRegion<ValueType>> result;
            auto regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
            if (regionSettings.isRegionSet()) {
                result = storm::api::parseRegions<ValueType>(regionSettings.getRegionString(), *model);
            }
            return result;
        }
        
        template <typename ValueType>
        std::pair<std::shared_ptr<storm::models::ModelBase>, bool> preprocessSparseModel(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input) {
            auto generalSettings = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
            auto bisimulationSettings = storm::settings::getModule<storm::settings::modules::BisimulationSettings>();
            auto parametricSettings = storm::settings::getModule<storm::settings::modules::ParametricSettings>();
            
            std::pair<std::shared_ptr<storm::models::ModelBase>, bool> result = std::make_pair(model, false);
            
            if (result.first->isOfType(storm::models::ModelType::MarkovAutomaton)) {
                result.first = storm::cli::preprocessSparseMarkovAutomaton(result.first->template as<storm::models::sparse::MarkovAutomaton<ValueType>>());
                result.second = true;
            }
            
            if (generalSettings.isBisimulationSet()) {
                result.first = storm::cli::preprocessSparseModelBisimulation(result.first->template as<storm::models::sparse::Model<ValueType>>(), input, bisimulationSettings);
                result.second = true;
            }
            
            if (parametricSettings.transformContinuousModel() && (result.first->isOfType(storm::models::ModelType::Ctmc) || result.first->isOfType(storm::models::ModelType::MarkovAutomaton))) {
                result.first = storm::api::transformContinuousToDiscreteTimeSparseModel(std::move(*result.first->template as<storm::models::sparse::Model<ValueType>>()), storm::api::extractFormulasFromProperties(input.properties));
                result.second = true;
            }
            
            return result;
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        std::pair<std::shared_ptr<storm::models::ModelBase>, bool> preprocessDdModel(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model, SymbolicInput const& input) {
            
            std::pair<std::shared_ptr<storm::models::ModelBase>, bool> result = std::make_pair(model, false);
        
            auto coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
            if (coreSettings.getEngine() == storm::settings::modules::CoreSettings::Engine::Hybrid) {
                // Currently, hybrid engine for parametric models just referrs to building the model symbolically.
                STORM_LOG_INFO("Translating symbolic model to sparse model...");
                result.first = storm::api::transformSymbolicToSparseModel(model);
                result.second = true;
                // Invoke preprocessing on the sparse model
                auto sparsePreprocessingResult = storm::pars::preprocessSparseModel<ValueType>(result.first->as<storm::models::sparse::Model<ValueType>>(), input);
                if (sparsePreprocessingResult.second) {
                    result.first = sparsePreprocessingResult.first;
                }
            }
            return result;
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        std::pair<std::shared_ptr<storm::models::ModelBase>, bool> preprocessModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            storm::utility::Stopwatch preprocessingWatch(true);
            
            std::pair<std::shared_ptr<storm::models::ModelBase>, bool> result = std::make_pair(model, false);
            if (model->isSparseModel()) {
                result = storm::pars::preprocessSparseModel<ValueType>(result.first->as<storm::models::sparse::Model<ValueType>>(), input);
            } else {
                STORM_LOG_ASSERT(model->isSymbolicModel(), "Unexpected model type.");
                result = storm::pars::preprocessDdModel<DdType, ValueType>(result.first->as<storm::models::symbolic::Model<DdType, ValueType>>(), input);
            }
            
            if (result.second) {
                STORM_PRINT_AND_LOG(std::endl << "Time for model preprocessing: " << preprocessingWatch << "." << std::endl << std::endl);
            }
            return result;
        }
        
        template<typename ValueType>
        void printInitialStatesResult(std::unique_ptr<storm::modelchecker::CheckResult> const& result, storm::jani::Property const& property, storm::utility::Stopwatch* watch = nullptr) {
            if (result) {
                STORM_PRINT_AND_LOG("Result (initial states): " << std::endl);
                
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
                    STORM_PRINT_AND_LOG("Time for model checking: " << *watch << "." << std::endl);
                }
            } else {
                STORM_PRINT_AND_LOG(" failed, property is unsupported by selected engine/settings." << std::endl);
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
        
        template <typename ValueType>
        void verifyPropertiesWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input) {
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
                                                storm::api::exportParametricResultToFile(boost::make_optional<ValueType>(result->asExplicitQuantitativeCheckResult<ValueType>()[*model->getInitialStates().begin()]),storm::analysis::ConstraintCollector<ValueType>(*dtmc), parametricSettings.exportResultPath());
                                            }
                                        });
        }
        
        template <typename ValueType>
        void verifyRegionsWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input, std::vector<storm::storage::ParameterRegion<ValueType>> const& regions) {
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
                                        std::unique_ptr<storm::modelchecker::RegionRefinementCheckResult<ValueType>> result = storm::api::checkAndRefineRegionWithSparseEngine<ValueType>(model, storm::api::createTask<ValueType>(formula, true), regions.front(), engine, refinementThreshold, optionalDepthLimit, regionSettings.getHypothesis());
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
        void verifyWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input, std::vector<storm::storage::ParameterRegion<ValueType>> const& regions) {
            if (regions.empty()) {
                storm::pars::verifyPropertiesWithSparseEngine(model, input);
            } else {
                storm::pars::verifyRegionsWithSparseEngine(model, input, regions);
            }
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        void verifyParametricModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input, std::vector<storm::storage::ParameterRegion<ValueType>> const& regions) {
            STORM_LOG_ASSERT(model->isSparseModel(), "Unexpected model type.");
            storm::pars::verifyWithSparseEngine<ValueType>(model->as<storm::models::sparse::Model<ValueType>>(), input, regions);
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        void processInputWithValueTypeAndDdlib(SymbolicInput& input) {
            auto coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
            auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
            auto parSettings = storm::settings::getModule<storm::settings::modules::ParametricSettings>();
            
            auto engine = coreSettings.getEngine();
            STORM_LOG_THROW(engine == storm::settings::modules::CoreSettings::Engine::Sparse || engine == storm::settings::modules::CoreSettings::Engine::Hybrid || engine == storm::settings::modules::CoreSettings::Engine::Dd, storm::exceptions::InvalidSettingsException, "The selected engine is not supported for parametric models.");
            
            std::shared_ptr<storm::models::ModelBase> model;
            if (!ioSettings.isNoBuildModelSet()) {
                model = storm::pars::buildModel<DdType, ValueType>(engine, input, ioSettings);
            }
            
            if (model) {
                model->printModelInformationToStream(std::cout);
            }
            
            STORM_LOG_THROW(model || input.properties.empty(), storm::exceptions::InvalidSettingsException, "No input model.");
            
            if (model) {
                auto preprocessingResult = storm::pars::preprocessModel<DdType, ValueType>(model, input);
                if (preprocessingResult.second) {
                    model = preprocessingResult.first;
                    model->printModelInformationToStream(std::cout);
                }
            }
            
            std::vector<storm::storage::ParameterRegion<ValueType>> regions = parseRegions<ValueType>(model);



            if (model) {
                storm::cli::exportModel<DdType, ValueType>(model, input);
            }

            if (parSettings.onlyObtainConstraints()) {
                STORM_LOG_THROW(parSettings.exportResultToFile(), storm::exceptions::InvalidSettingsException, "When computing constraints, export path has to be specified.");
                storm::api::exportParametricResultToFile<ValueType>(boost::none, storm::analysis::ConstraintCollector<ValueType>(*(model->as<storm::models::sparse::Model<ValueType>>())), parSettings.exportResultPath());
                return;
            }

            if (model) {
                verifyParametricModel<DdType, ValueType>(model, input, regions);
            }
        }
        
        void processOptions() {
            // Start by setting some urgent options (log levels, resources, etc.)
            storm::cli::setUrgentOptions();
            
            // Parse and preprocess symbolic input (PRISM, JANI, properties, etc.)
            SymbolicInput symbolicInput = storm::cli::parseAndPreprocessSymbolicInput();
            
            auto coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
            auto engine = coreSettings.getEngine();
            STORM_LOG_WARN_COND(engine != storm::settings::modules::CoreSettings::Engine::Dd || engine != storm::settings::modules::CoreSettings::Engine::Hybrid || coreSettings.getDdLibraryType() == storm::dd::DdType::Sylvan, "The selected DD library does not support parametric models. Switching to Sylvan...");
        
            processInputWithValueTypeAndDdlib<storm::dd::DdType::Sylvan, storm::RationalFunction>(symbolicInput);
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
