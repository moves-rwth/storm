#pragma once

#include "storm/api/storm.h"

#include "storm-counterexamples/api/counterexamples.h"
#include "storm-parsers/api/storm-parsers.h"

#include "storm/utility/resources.h"
#include "storm/utility/file.h"
#include "storm/utility/storm-version.h"
#include "storm/utility/macros.h"
#include "storm/utility/NumberTraits.h"

#include "storm/utility/initialize.h"
#include "storm/utility/Stopwatch.h"

#include <type_traits>


#include "storm/storage/SymbolicModelDescription.h"
#include "storm/storage/jani/Property.h"

#include "storm/builder/BuilderType.h"

#include "storm/models/ModelBase.h"

#include "storm/exceptions/OptionParserException.h"

#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"

#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/symbolic/StandardRewardModel.h"
#include "storm/models/symbolic/MarkovAutomaton.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/ResourceSettings.h"
#include "storm/settings/modules/JitBuilderSettings.h"
#include "storm/settings/modules/BuildSettings.h"
#include "storm/settings/modules/DebugSettings.h"
#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/AbstractionSettings.h"
#include "storm/settings/modules/ResourceSettings.h"
#include "storm/settings/modules/ModelCheckerSettings.h"

#include "storm/utility/Stopwatch.h"

namespace storm {
    namespace cli {
        
        
        struct SymbolicInput {
            // The symbolic model description.
            boost::optional<storm::storage::SymbolicModelDescription> model;
            
            // The original properties to check.
            std::vector<storm::jani::Property> properties;
            
            // The preprocessed properties to check (in case they needed amendment).
            boost::optional<std::vector<storm::jani::Property>> preprocessedProperties;
        };
        
        void parseSymbolicModelDescription(storm::settings::modules::IOSettings const& ioSettings, SymbolicInput& input, storm::builder::BuilderType const& builderType) {
            if (ioSettings.isPrismOrJaniInputSet()) {
                storm::utility::Stopwatch modelParsingWatch(true);
                if (ioSettings.isPrismInputSet()) {
                    input.model = storm::api::parseProgram(ioSettings.getPrismInputFilename(), storm::settings::getModule<storm::settings::modules::BuildSettings>().isPrismCompatibilityEnabled());
                } else {
                    storm::jani::ModelFeatures supportedFeatures = storm::api::getSupportedJaniFeatures(builderType);
                    boost::optional<std::vector<std::string>> propertyFilter;
                    if (ioSettings.isJaniPropertiesSet()) {
                        if (ioSettings.areJaniPropertiesSelected()) {
                            propertyFilter = ioSettings.getSelectedJaniProperties();
                        } else {
                            propertyFilter = boost::none;
                        }
                    } else {
                        propertyFilter = std::vector<std::string>();
                    }
                    auto janiInput = storm::api::parseJaniModel(ioSettings.getJaniInputFilename(), supportedFeatures, propertyFilter);
                    input.model = std::move(janiInput.first);
                    if (ioSettings.isJaniPropertiesSet()) {
                        input.properties = std::move(janiInput.second);
                    }
                }
                modelParsingWatch.stop();
                STORM_PRINT("Time for model input parsing: " << modelParsingWatch << "." << std::endl << std::endl);
            }
        }
        
        void parseProperties(storm::settings::modules::IOSettings const& ioSettings, SymbolicInput& input, boost::optional<std::set<std::string>> const& propertyFilter) {
            if (ioSettings.isPropertySet()) {
                std::vector<storm::jani::Property> newProperties;
                if (input.model) {
                    newProperties = storm::api::parsePropertiesForSymbolicModelDescription(ioSettings.getProperty(), input.model.get(), propertyFilter);
                } else {
                    newProperties = storm::api::parseProperties(ioSettings.getProperty(), propertyFilter);
                }
                
                input.properties.insert(input.properties.end(), newProperties.begin(), newProperties.end());
            }
        }
        
        SymbolicInput parseSymbolicInput(storm::builder::BuilderType const& builderType) {
            auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
            
            // Parse the property filter, if any is given.
            boost::optional<std::set<std::string>> propertyFilter = storm::api::parsePropertyFilter(ioSettings.getPropertyFilter());
            
            SymbolicInput input;
            parseSymbolicModelDescription(ioSettings, input, builderType);
            parseProperties(ioSettings, input, propertyFilter);
            
            return input;
        }
        
        SymbolicInput preprocessSymbolicInput(SymbolicInput const& input, storm::builder::BuilderType const& builderType) {
            auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
            
            SymbolicInput output = input;
            
            // Substitute constant definitions in symbolic input.
            std::string constantDefinitionString = ioSettings.getConstantDefinitionString();
            std::map<storm::expressions::Variable, storm::expressions::Expression> constantDefinitions;
            if (output.model) {
                constantDefinitions = output.model.get().parseConstantDefinitions(constantDefinitionString);
                output.model = output.model.get().preprocess(constantDefinitions);
            }
            if (!output.properties.empty()) {
                output.properties = storm::api::substituteConstantsInProperties(output.properties, constantDefinitions);
            }
            
            // Make sure there are no undefined constants remaining in any property.
            for (auto const& property : output.properties) {
                std::set<storm::expressions::Variable> usedUndefinedConstants = property.getUndefinedConstants();
                if (!usedUndefinedConstants.empty()) {
                    std::vector<std::string> undefinedConstantsNames;
                    for (auto const& constant : usedUndefinedConstants) {
                        undefinedConstantsNames.emplace_back(constant.getName());
                    }

                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The property '" << property << " still refers to the undefined constants " << boost::algorithm::join(undefinedConstantsNames, ",") << ".");
                }
            }
            
            // Check whether conversion for PRISM to JANI is requested or necessary.
            if (input.model && input.model.get().isPrismProgram()) {
                bool transformToJani = ioSettings.isPrismToJaniSet();
                bool transformToJaniForJit = builderType == storm::builder::BuilderType::Jit;
                STORM_LOG_WARN_COND(transformToJani || !transformToJaniForJit, "The JIT-based model builder is only available for JANI models, automatically converting the PRISM input model.");
                bool transformToJaniForDdMA = (builderType == storm::builder::BuilderType::Dd) && (input.model->getModelType() == storm::storage::SymbolicModelDescription::ModelType::MA);
                STORM_LOG_WARN_COND(transformToJani || !transformToJaniForDdMA, "Dd-based model builder for Markov Automata is only available for JANI models, automatically converting the PRISM input model.");
                transformToJani |= (transformToJaniForJit || transformToJaniForDdMA);
                
                if (transformToJani) {
                    storm::prism::Program const& model = output.model.get().asPrismProgram();
                    auto modelAndProperties = model.toJani(output.properties);
                    
                    // Remove functions here
                    modelAndProperties.first.substituteFunctions();
                    
                    output.model = modelAndProperties.first;
                    
                    if (!modelAndProperties.second.empty()) {
                        output.preprocessedProperties = std::move(modelAndProperties.second);
                    }
                }
            }
            
            return output;
        }
        
        void exportSymbolicInput(SymbolicInput const& input) {
            auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
            if (input.model && input.model.get().isJaniModel()) {
                storm::storage::SymbolicModelDescription const& model = input.model.get();
                if (ioSettings.isExportJaniDotSet()) {
                    storm::api::exportJaniModelAsDot(model.asJaniModel(), ioSettings.getExportJaniDotFilename());
                }
            }
        }
        
        storm::builder::BuilderType getBuilderType(storm::settings::modules::CoreSettings::Engine const& engine, bool useJit) {
            if (engine == storm::settings::modules::CoreSettings::Engine::Dd || engine == storm::settings::modules::CoreSettings::Engine::Hybrid || engine == storm::settings::modules::CoreSettings::Engine::DdSparse || engine == storm::settings::modules::CoreSettings::Engine::AbstractionRefinement) {
                return storm::builder::BuilderType::Dd;
            } else if (engine == storm::settings::modules::CoreSettings::Engine::Sparse) {
                if (useJit) {
                    return storm::builder::BuilderType::Jit;
                } else {
                    return storm::builder::BuilderType::Explicit;
                }
            } else if (engine == storm::settings::modules::CoreSettings::Engine::Exploration) {
                return storm::builder::BuilderType::Explicit;
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unable to determine the model builder type.");
        }
        
        SymbolicInput parseAndPreprocessSymbolicInput() {
            // Get the used builder type to handle cases where preprocessing depends on it
            auto buildSettings = storm::settings::getModule<storm::settings::modules::BuildSettings>();
            auto coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
            auto builderType = getBuilderType(coreSettings.getEngine(), buildSettings.isJitSet());
            
            SymbolicInput input = parseSymbolicInput(builderType);
            input = preprocessSymbolicInput(input, builderType);
            exportSymbolicInput(input);
            return input;
        }
        
        std::vector<std::shared_ptr<storm::logic::Formula const>> createFormulasToRespect(std::vector<storm::jani::Property> const& properties) {
            std::vector<std::shared_ptr<storm::logic::Formula const>> result = storm::api::extractFormulasFromProperties(properties);
            
            for (auto const& property : properties) {
                if (!property.getFilter().getStatesFormula()->isInitialFormula()) {
                    result.push_back(property.getFilter().getStatesFormula());
                }
            }
            
            return result;
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        std::shared_ptr<storm::models::ModelBase> buildModelDd(SymbolicInput const& input) {
            return storm::api::buildSymbolicModel<DdType, ValueType>(input.model.get(), createFormulasToRespect(input.properties), storm::settings::getModule<storm::settings::modules::BuildSettings>().isBuildFullModelSet());
        }
        
        template <typename ValueType>
        std::shared_ptr<storm::models::ModelBase> buildModelSparse(SymbolicInput const& input, storm::settings::modules::BuildSettings const& buildSettings) {
            storm::builder::BuilderOptions options(createFormulasToRespect(input.properties), input.model.get());
            options.setBuildChoiceLabels(buildSettings.isBuildChoiceLabelsSet());
            options.setBuildStateValuations(buildSettings.isBuildStateValuationsSet());
            if (storm::settings::manager().hasModule(storm::settings::modules::CounterexampleGeneratorSettings::moduleName)) {
                auto counterexampleGeneratorSettings = storm::settings::getModule<storm::settings::modules::CounterexampleGeneratorSettings>();
                options.setBuildChoiceOrigins(counterexampleGeneratorSettings.isMinimalCommandSetGenerationSet());
            } else {
                options.setBuildChoiceOrigins(false);
            }
            options.setAddOutOfBoundsState(buildSettings.isBuildOutOfBoundsStateSet());
            if (buildSettings.isBuildFullModelSet()) {
                options.clearTerminalStates();
                options.setApplyMaximalProgressAssumption(false);
                options.setBuildAllLabels(true);
                options.setBuildAllRewardModels(true);
            }
            return storm::api::buildSparseModel<ValueType>(input.model.get(), options, buildSettings.isJitSet(), storm::settings::getModule<storm::settings::modules::JitBuilderSettings>().isDoctorSet());
        }
        
        template <typename ValueType>
        std::shared_ptr<storm::models::ModelBase> buildModelExplicit(storm::settings::modules::IOSettings const& ioSettings) {
            std::shared_ptr<storm::models::ModelBase> result;
            if (ioSettings.isExplicitSet()) {
                result = storm::api::buildExplicitModel<ValueType>(ioSettings.getTransitionFilename(), ioSettings.getLabelingFilename(), ioSettings.isStateRewardsSet() ? boost::optional<std::string>(ioSettings.getStateRewardsFilename()) : boost::none, ioSettings.isTransitionRewardsSet() ? boost::optional<std::string>(ioSettings.getTransitionRewardsFilename()) : boost::none, ioSettings.isChoiceLabelingSet() ? boost::optional<std::string>(ioSettings.getChoiceLabelingFilename()) : boost::none);
            } else if (ioSettings.isExplicitDRNSet()) {
                result = storm::api::buildExplicitDRNModel<ValueType>(ioSettings.getExplicitDRNFilename());
            } else {
                STORM_LOG_THROW(ioSettings.isExplicitIMCASet(), storm::exceptions::InvalidSettingsException, "Unexpected explicit model input type.");
                result = storm::api::buildExplicitIMCAModel<ValueType>(ioSettings.getExplicitIMCAFilename());
            }
            return result;
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        std::shared_ptr<storm::models::ModelBase> buildModel(storm::settings::modules::CoreSettings::Engine const& engine, SymbolicInput const& input, storm::settings::modules::IOSettings const& ioSettings) {
            storm::utility::Stopwatch modelBuildingWatch(true);

            auto buildSettings = storm::settings::getModule<storm::settings::modules::BuildSettings>();
            std::shared_ptr<storm::models::ModelBase> result;
            if (input.model) {
                auto builderType = getBuilderType(engine, buildSettings.isJitSet());
                if (builderType == storm::builder::BuilderType::Dd) {
                    result = buildModelDd<DdType, ValueType>(input);
                } else if (builderType == storm::builder::BuilderType::Explicit || builderType == storm::builder::BuilderType::Jit) {
                    result = buildModelSparse<ValueType>(input, buildSettings);
                }
            } else if (ioSettings.isExplicitSet() || ioSettings.isExplicitDRNSet() || ioSettings.isExplicitIMCASet()) {
                STORM_LOG_THROW(engine == storm::settings::modules::CoreSettings::Engine::Sparse, storm::exceptions::InvalidSettingsException, "Can only use sparse engine with explicit input.");
                result = buildModelExplicit<ValueType>(ioSettings);
            }
            
            modelBuildingWatch.stop();
            if (result) {
                STORM_PRINT("Time for model construction: " << modelBuildingWatch << "." << std::endl << std::endl);
            }
            
            return result;
        }
        
        template <typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> preprocessSparseMarkovAutomaton(std::shared_ptr<storm::models::sparse::MarkovAutomaton<ValueType>> const& model) {
            std::shared_ptr<storm::models::sparse::Model<ValueType>> result = model;
            model->close();
            if (model->isConvertibleToCtmc()) {
                STORM_LOG_WARN_COND(false, "MA is convertible to a CTMC, consider using a CTMC instead.");
                result = model->convertToCtmc();
            }
            return result;
        }
        
        template <typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> preprocessSparseModelBisimulation(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input, storm::settings::modules::BisimulationSettings const& bisimulationSettings) {
            storm::storage::BisimulationType bisimType = storm::storage::BisimulationType::Strong;
            if (bisimulationSettings.isWeakBisimulationSet()) {
                bisimType = storm::storage::BisimulationType::Weak;
            }
            
            STORM_LOG_INFO("Performing bisimulation minimization...");
            return storm::api::performBisimulationMinimization<ValueType>(model, createFormulasToRespect(input.properties), bisimType);
        }
        
        template <typename ValueType>
        std::pair<std::shared_ptr<storm::models::sparse::Model<ValueType>>, bool> preprocessSparseModel(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input) {
            auto generalSettings = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
            auto bisimulationSettings = storm::settings::getModule<storm::settings::modules::BisimulationSettings>();
            auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
            
            std::pair<std::shared_ptr<storm::models::sparse::Model<ValueType>>, bool> result = std::make_pair(model, false);
            
            if (result.first->isOfType(storm::models::ModelType::MarkovAutomaton)) {
                result.first = preprocessSparseMarkovAutomaton(result.first->template as<storm::models::sparse::MarkovAutomaton<ValueType>>());
                result.second = true;
            }
            
            if (generalSettings.isBisimulationSet()) {
                result.first = preprocessSparseModelBisimulation(result.first, input, bisimulationSettings);
                result.second = true;
            }
            
            if (ioSettings.isToNondeterministicModelSet()) {
                result.first = storm::api::transformToNondeterministicModel<ValueType>(std::move(*result.first));
                result.second = true;
            }
            
            return result;
        }
        
        template <typename ValueType>
        void exportSparseModel(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input) {
            auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
            
            if (ioSettings.isExportExplicitSet()) {
                storm::api::exportSparseModelAsDrn(model, ioSettings.getExportExplicitFilename(), input.model ? input.model.get().getParameterNames() : std::vector<std::string>());
            }
            
            if (ioSettings.isExportDotSet()) {
                storm::api::exportSparseModelAsDot(model, ioSettings.getExportDotFilename());
            }
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        void exportDdModel(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model, SymbolicInput const& input) {
            // Intentionally left empty.
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        void exportModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            if (model->isSparseModel()) {
                exportSparseModel<ValueType>(model->as<storm::models::sparse::Model<ValueType>>(), input);
            } else {
                exportDdModel<DdType, ValueType>(model->as<storm::models::symbolic::Model<DdType, ValueType>>(), input);
            }
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        typename std::enable_if<DdType != storm::dd::DdType::Sylvan && !std::is_same<ValueType, double>::value, std::shared_ptr<storm::models::Model<ValueType>>>::type
        preprocessDdMarkovAutomaton(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model) {
            return model;
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        typename std::enable_if<DdType == storm::dd::DdType::Sylvan || std::is_same<ValueType, double>::value, std::shared_ptr<storm::models::Model<ValueType>>>::type
        preprocessDdMarkovAutomaton(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model) {
            auto ma = model->template as<storm::models::symbolic::MarkovAutomaton<DdType, ValueType>>();
            if (!ma->isClosed()) {
                return std::make_shared<storm::models::symbolic::MarkovAutomaton<DdType, ValueType>>(ma->close());
            } else {
                return model;
            }
        }
        
        template <storm::dd::DdType DdType, typename ValueType, typename ExportValueType = ValueType>
        std::shared_ptr<storm::models::Model<ExportValueType>> preprocessDdModelBisimulation(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model, SymbolicInput const& input, storm::settings::modules::BisimulationSettings const& bisimulationSettings) {
            STORM_LOG_WARN_COND(!bisimulationSettings.isWeakBisimulationSet(), "Weak bisimulation is currently not supported on DDs. Falling back to strong bisimulation.");
            
            STORM_LOG_INFO("Performing bisimulation minimization...");
            return storm::api::performBisimulationMinimization<DdType, ValueType, ExportValueType>(model, createFormulasToRespect(input.properties), storm::storage::BisimulationType::Strong, bisimulationSettings.getSignatureMode());
        }
        
        template <storm::dd::DdType DdType, typename ValueType, typename ExportValueType = ValueType>
        std::pair<std::shared_ptr<storm::models::ModelBase>, bool> preprocessDdModel(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model, SymbolicInput const& input) {
            auto bisimulationSettings = storm::settings::getModule<storm::settings::modules::BisimulationSettings>();
            auto generalSettings = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
            std::pair<std::shared_ptr<storm::models::Model<ValueType>>, bool> intermediateResult = std::make_pair(model, false);
            
            if (model->isOfType(storm::models::ModelType::MarkovAutomaton)) {
                intermediateResult.first = preprocessDdMarkovAutomaton(intermediateResult.first->template as<storm::models::symbolic::Model<DdType, ValueType>>());
                intermediateResult.second = true;
            }
            
            std::unique_ptr<std::pair<std::shared_ptr<storm::models::Model<ExportValueType>>, bool>> result;
            auto symbolicModel = intermediateResult.first->template as<storm::models::symbolic::Model<DdType, ValueType>>();
            if (generalSettings.isBisimulationSet()) {
                std::shared_ptr<storm::models::Model<ExportValueType>> newModel = preprocessDdModelBisimulation<DdType, ValueType, ExportValueType>(symbolicModel, input, bisimulationSettings);
                result = std::make_unique<std::pair<std::shared_ptr<storm::models::Model<ExportValueType>>, bool>>(newModel, true);
            } else {
                result = std::make_unique<std::pair<std::shared_ptr<storm::models::Model<ExportValueType>>, bool>>(symbolicModel->template toValueType<ExportValueType>(), !std::is_same<ValueType, ExportValueType>::value);
            }
            
            if (result && result->first->isSymbolicModel() && storm::settings::getModule<storm::settings::modules::CoreSettings>().getEngine() == storm::settings::modules::CoreSettings::Engine::DdSparse) {
                // Mark as changed.
                result->second = true;
                
                std::shared_ptr<storm::models::symbolic::Model<DdType, ExportValueType>> symbolicModel = result->first->template as<storm::models::symbolic::Model<DdType, ExportValueType>>();
                std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
                for (auto const& property : input.properties) {
                    formulas.emplace_back(property.getRawFormula());
                }
                result->first = storm::api::transformSymbolicToSparseModel(symbolicModel, formulas);
                STORM_LOG_THROW(result, storm::exceptions::NotSupportedException, "The translation to a sparse model is not supported for the given model type.");
            }
            
            return *result;
        }
        
        template <storm::dd::DdType DdType, typename BuildValueType, typename ExportValueType = BuildValueType>
        std::pair<std::shared_ptr<storm::models::ModelBase>, bool> preprocessModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            storm::utility::Stopwatch preprocessingWatch(true);
            
            std::pair<std::shared_ptr<storm::models::ModelBase>, bool> result = std::make_pair(model, false);
            if (model->isSparseModel()) {
                result = preprocessSparseModel<BuildValueType>(result.first->as<storm::models::sparse::Model<BuildValueType>>(), input);
            } else {
                STORM_LOG_ASSERT(model->isSymbolicModel(), "Unexpected model type.");
                result = preprocessDdModel<DdType, BuildValueType, ExportValueType>(result.first->as<storm::models::symbolic::Model<DdType, BuildValueType>>(), input);
            }
            
            preprocessingWatch.stop();
            
            if (result.second) {
                STORM_PRINT(std::endl << "Time for model preprocessing: " << preprocessingWatch << "." << std::endl << std::endl);
            }
            return result;
        }
        
        void printComputingCounterexample(storm::jani::Property const& property) {
            STORM_PRINT("Computing counterexample for property " << *property.getRawFormula() << " ..." << std::endl);
        }
        
        void printCounterexample(std::shared_ptr<storm::counterexamples::Counterexample> const& counterexample, storm::utility::Stopwatch* watch = nullptr) {
            if (counterexample) {
                STORM_PRINT(*counterexample << std::endl);
                if (watch) {
                    STORM_PRINT("Time for computation: " << *watch << "." << std::endl);
                }
            } else {
                STORM_PRINT(" failed." << std::endl);
            }
        }
        
        template <typename ValueType>
        void generateCounterexamples(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Counterexample generation is not supported for this data-type.");
        }
        
        template <>
        void generateCounterexamples<double>(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            typedef double ValueType;
            
            STORM_LOG_THROW(model->isSparseModel(), storm::exceptions::NotSupportedException, "Counterexample generation is currently only supported for sparse models.");
            auto sparseModel = model->as<storm::models::sparse::Model<ValueType>>();
            for (auto& rewModel : sparseModel->getRewardModels()) {
                rewModel.second.reduceToStateBasedRewards(sparseModel->getTransitionMatrix(), true);
            }

            STORM_LOG_THROW(sparseModel->isOfType(storm::models::ModelType::Dtmc) || sparseModel->isOfType(storm::models::ModelType::Mdp), storm::exceptions::NotSupportedException, "Counterexample is currently only supported for discrete-time models.");
            
            auto counterexampleSettings = storm::settings::getModule<storm::settings::modules::CounterexampleGeneratorSettings>();
            if (counterexampleSettings.isMinimalCommandSetGenerationSet()) {
                
                bool useMilp = counterexampleSettings.isUseMilpBasedMinimalCommandSetGenerationSet();
                for (auto const& property : input.properties) {
                    std::shared_ptr<storm::counterexamples::Counterexample> counterexample;
                    printComputingCounterexample(property);
                    storm::utility::Stopwatch watch(true);
                    if (useMilp) {
                        STORM_LOG_THROW(sparseModel->isOfType(storm::models::ModelType::Mdp), storm::exceptions::NotSupportedException, "Counterexample generation using MILP is currently only supported for MDPs.");
                        counterexample = storm::api::computeHighLevelCounterexampleMilp(input.model.get(), sparseModel->template as<storm::models::sparse::Mdp<ValueType>>(), property.getRawFormula());
                    } else {
                        STORM_LOG_THROW(sparseModel->isOfType(storm::models::ModelType::Dtmc) || sparseModel->isOfType(storm::models::ModelType::Mdp), storm::exceptions::NotSupportedException, "Counterexample generation using MaxSAT is currently only supported for discrete-time models.");

                        if (sparseModel->isOfType(storm::models::ModelType::Dtmc)) {
                            counterexample = storm::api::computeHighLevelCounterexampleMaxSmt(input.model.get(), sparseModel->template as<storm::models::sparse::Dtmc<ValueType>>(), property.getRawFormula());
                        } else {
                            counterexample = storm::api::computeHighLevelCounterexampleMaxSmt(input.model.get(), sparseModel->template as<storm::models::sparse::Mdp<ValueType>>(), property.getRawFormula());
                        }
                    }
                    watch.stop();
                    printCounterexample(counterexample, &watch);
                }
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The selected counterexample formalism is unsupported.");
            }
        }
        
        template<typename ValueType>
        void printFilteredResult(std::unique_ptr<storm::modelchecker::CheckResult> const& result, storm::modelchecker::FilterType ft) {
            if (result->isQuantitative()) {
                if (ft == storm::modelchecker::FilterType::VALUES) {
                    STORM_PRINT(*result);
                } else {
                    ValueType resultValue;
                    switch (ft) {
                        case storm::modelchecker::FilterType::SUM:
                            resultValue = result->asQuantitativeCheckResult<ValueType>().sum();
                            break;
                        case storm::modelchecker::FilterType::AVG:
                            resultValue = result->asQuantitativeCheckResult<ValueType>().average();
                            break;
                        case storm::modelchecker::FilterType::MIN:
                            resultValue = result->asQuantitativeCheckResult<ValueType>().getMin();
                            break;
                        case storm::modelchecker::FilterType::MAX:
                            resultValue = result->asQuantitativeCheckResult<ValueType>().getMax();
                            break;
                        case storm::modelchecker::FilterType::ARGMIN:
                        case storm::modelchecker::FilterType::ARGMAX:
                            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Outputting states is not supported.");
                        case storm::modelchecker::FilterType::EXISTS:
                        case storm::modelchecker::FilterType::FORALL:
                        case storm::modelchecker::FilterType::COUNT:
                            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Filter type only defined for qualitative results.");
                        default:
                            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Unhandled filter type.");
                    }
                    if (storm::NumberTraits<ValueType>::IsExact && storm::utility::isConstant(resultValue)) {
                        STORM_PRINT(resultValue << " (approx. " << storm::utility::convertNumber<double>(resultValue) << ")");
                    } else {
                        STORM_PRINT(resultValue);
                    }
                }
            } else {
                switch (ft) {
                    case storm::modelchecker::FilterType::VALUES:
                        STORM_PRINT(*result << std::endl);
                        break;
                    case storm::modelchecker::FilterType::EXISTS:
                        STORM_PRINT(result->asQualitativeCheckResult().existsTrue());
                        break;
                    case storm::modelchecker::FilterType::FORALL:
                        STORM_PRINT(result->asQualitativeCheckResult().forallTrue());
                        break;
                    case storm::modelchecker::FilterType::COUNT:
                        STORM_PRINT(result->asQualitativeCheckResult().count());
                        break;
                    case storm::modelchecker::FilterType::ARGMIN:
                    case storm::modelchecker::FilterType::ARGMAX:
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Outputting states is not supported.");
                    case storm::modelchecker::FilterType::SUM:
                    case storm::modelchecker::FilterType::AVG:
                    case storm::modelchecker::FilterType::MIN:
                    case storm::modelchecker::FilterType::MAX:
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Filter type only defined for quantitative results.");
                }
            }
            STORM_PRINT(std::endl);
        }
        
        void printModelCheckingProperty(storm::jani::Property const& property) {
            STORM_PRINT(std::endl << "Model checking property \"" << property.getName() << "\": " << *property.getRawFormula() << " ..." << std::endl);
        }
        
        template<typename ValueType>
        void printResult(std::unique_ptr<storm::modelchecker::CheckResult> const& result, storm::jani::Property const& property, storm::utility::Stopwatch* watch = nullptr) {
            if (result) {
                std::stringstream ss;
                ss << "'" << *property.getFilter().getStatesFormula() << "'";
                STORM_PRINT("Result (for " << (property.getFilter().getStatesFormula()->isInitialFormula() ? "initial" : ss.str()) << " states): ");
                printFilteredResult<ValueType>(result, property.getFilter().getFilterType());
                if (watch) {
                    STORM_PRINT("Time for model checking: " << *watch << "." << std::endl);
                }
            } else {
                STORM_LOG_ERROR("Property is unsupported by selected engine/settings." << std::endl);
            }
        }
        
        struct PostprocessingIdentity {
            void operator()(std::unique_ptr<storm::modelchecker::CheckResult> const&) {
                // Intentionally left empty.
            }
        };
        
        template<typename ValueType>
        void verifyProperties(SymbolicInput const& input, std::function<std::unique_ptr<storm::modelchecker::CheckResult>(std::shared_ptr<storm::logic::Formula const> const& formula, std::shared_ptr<storm::logic::Formula const> const& states)> const& verificationCallback, std::function<void(std::unique_ptr<storm::modelchecker::CheckResult> const&)> const& postprocessingCallback = PostprocessingIdentity()) {
            auto const& properties = input.preprocessedProperties ? input.preprocessedProperties.get() : input.properties;
            for (auto const& property : properties) {
                printModelCheckingProperty(property);
                storm::utility::Stopwatch watch(true);
                std::unique_ptr<storm::modelchecker::CheckResult> result = verificationCallback(property.getRawFormula(), property.getFilter().getStatesFormula());
                watch.stop();
                postprocessingCallback(result);
                printResult<ValueType>(result, property, &watch);
            }
        }
        
        std::vector<storm::expressions::Expression> parseConstraints(storm::expressions::ExpressionManager const& expressionManager, std::string const& constraintsString) {
            std::vector<storm::expressions::Expression> constraints;
            
            std::vector<std::string> constraintsAsStrings;
            boost::split(constraintsAsStrings, constraintsString, boost::is_any_of(","));
            
            storm::parser::ExpressionParser expressionParser(expressionManager);
            std::unordered_map<std::string, storm::expressions::Expression> variableMapping;
            for (auto const& variableTypePair : expressionManager) {
                variableMapping[variableTypePair.first.getName()] = variableTypePair.first;
            }
            expressionParser.setIdentifierMapping(variableMapping);
            
            for (auto const& constraintString : constraintsAsStrings) {
                if (constraintString.empty()) {
                    continue;
                }

                storm::expressions::Expression constraint = expressionParser.parseFromString(constraintString);
                STORM_LOG_TRACE("Adding special (user-provided) constraint " << constraint << ".");
                constraints.emplace_back(constraint);
            }
            
            return constraints;
        }
        
        std::vector<std::vector<storm::expressions::Expression>> parseInjectedRefinementPredicates(storm::expressions::ExpressionManager const& expressionManager, std::string const& refinementPredicatesString) {
            std::vector<std::vector<storm::expressions::Expression>> injectedRefinementPredicates;
            
            storm::parser::ExpressionParser expressionParser(expressionManager);
            std::unordered_map<std::string, storm::expressions::Expression> variableMapping;
            for (auto const& variableTypePair : expressionManager) {
                variableMapping[variableTypePair.first.getName()] = variableTypePair.first;
            }
            expressionParser.setIdentifierMapping(variableMapping);
            
            std::vector<std::string> predicateGroupsAsStrings;
            boost::split(predicateGroupsAsStrings, refinementPredicatesString, boost::is_any_of(";"));
            
            if (!predicateGroupsAsStrings.empty()) {
                for (auto const& predicateGroupString : predicateGroupsAsStrings) {
                    if (predicateGroupString.empty()) {
                        continue;
                    }
                    
                    std::vector<std::string> predicatesAsStrings;
                    boost::split(predicatesAsStrings, predicateGroupString, boost::is_any_of(":"));
                    
                    if (!predicatesAsStrings.empty()) {
                        injectedRefinementPredicates.emplace_back();
                        for (auto const& predicateString : predicatesAsStrings) {
                            storm::expressions::Expression predicate = expressionParser.parseFromString(predicateString);
                            STORM_LOG_TRACE("Adding special (user-provided) refinement predicate " << predicateString << ".");
                            injectedRefinementPredicates.back().emplace_back(predicate);
                        }
                        
                        STORM_LOG_THROW(!injectedRefinementPredicates.back().empty(), storm::exceptions::InvalidArgumentException, "Expecting non-empty list of predicates to inject for each (mentioned) refinement step.");
                        
                        // Finally reverse the list, because we take the predicates from the back.
                        std::reverse(injectedRefinementPredicates.back().begin(), injectedRefinementPredicates.back().end());
                    }
                }
                
                // Finally reverse the list, because we take the predicates from the back.
                std::reverse(injectedRefinementPredicates.begin(), injectedRefinementPredicates.end());
            }
            
            return injectedRefinementPredicates;
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        void verifyWithAbstractionRefinementEngine(SymbolicInput const& input) {
            STORM_LOG_ASSERT(input.model, "Expected symbolic model description.");
            storm::settings::modules::AbstractionSettings const& abstractionSettings = storm::settings::getModule<storm::settings::modules::AbstractionSettings>();
            storm::api::AbstractionRefinementOptions options(parseConstraints(input.model->getManager(), abstractionSettings.getConstraintString()), parseInjectedRefinementPredicates(input.model->getManager(), abstractionSettings.getInjectedRefinementPredicates()));

            verifyProperties<ValueType>(input, [&input,&options] (std::shared_ptr<storm::logic::Formula const> const& formula, std::shared_ptr<storm::logic::Formula const> const& states) {
                STORM_LOG_THROW(states->isInitialFormula(), storm::exceptions::NotSupportedException, "Abstraction-refinement can only filter initial states.");
                return storm::api::verifyWithAbstractionRefinementEngine<DdType, ValueType>(input.model.get(), storm::api::createTask<ValueType>(formula, true), options);
            });
        }
        
        template <typename ValueType>
        void verifyWithExplorationEngine(SymbolicInput const& input) {
            STORM_LOG_ASSERT(input.model, "Expected symbolic model description.");
            STORM_LOG_THROW((std::is_same<ValueType, double>::value), storm::exceptions::NotSupportedException, "Exploration does not support other data-types than floating points.");
            verifyProperties<ValueType>(input, [&input] (std::shared_ptr<storm::logic::Formula const> const& formula, std::shared_ptr<storm::logic::Formula const> const& states) {
                STORM_LOG_THROW(states->isInitialFormula(), storm::exceptions::NotSupportedException, "Exploration can only filter initial states.");
                return storm::api::verifyWithExplorationEngine<ValueType>(input.model.get(), storm::api::createTask<ValueType>(formula, true));
            });
        }
        
        template <typename ValueType>
        void verifyWithSparseEngine(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            auto sparseModel = model->as<storm::models::sparse::Model<ValueType>>();
            verifyProperties<ValueType>(input,
                                        [&sparseModel] (std::shared_ptr<storm::logic::Formula const> const& formula, std::shared_ptr<storm::logic::Formula const> const& states) {
                                            bool filterForInitialStates = states->isInitialFormula();
                                            auto task = storm::api::createTask<ValueType>(formula, filterForInitialStates);
                                            std::unique_ptr<storm::modelchecker::CheckResult> result = storm::api::verifyWithSparseEngine<ValueType>(sparseModel, task);
                                            
                                            std::unique_ptr<storm::modelchecker::CheckResult> filter;
                                            if (filterForInitialStates) {
                                                filter = std::make_unique<storm::modelchecker::ExplicitQualitativeCheckResult>(sparseModel->getInitialStates());
                                            } else {
                                                filter = storm::api::verifyWithSparseEngine<ValueType>(sparseModel, storm::api::createTask<ValueType>(states, false));
                                            }
                                            if (result && filter) {
                                                result->filter(filter->asQualitativeCheckResult());
                                            }
                                            return result;
                                        });
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        void verifyWithHybridEngine(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            verifyProperties<ValueType>(input, [&model] (std::shared_ptr<storm::logic::Formula const> const& formula, std::shared_ptr<storm::logic::Formula const> const& states) {
                bool filterForInitialStates = states->isInitialFormula();
                auto task = storm::api::createTask<ValueType>(formula, filterForInitialStates);
                
                auto symbolicModel = model->as<storm::models::symbolic::Model<DdType, ValueType>>();
                std::unique_ptr<storm::modelchecker::CheckResult> result = storm::api::verifyWithHybridEngine<DdType, ValueType>(symbolicModel, task);
                
                std::unique_ptr<storm::modelchecker::CheckResult> filter;
                if (filterForInitialStates) {
                    filter = std::make_unique<storm::modelchecker::SymbolicQualitativeCheckResult<DdType>>(symbolicModel->getReachableStates(), symbolicModel->getInitialStates());
                } else {
                    filter = storm::api::verifyWithHybridEngine<DdType, ValueType>(symbolicModel, storm::api::createTask<ValueType>(states, false));
                }
                if (result && filter) {
                    result->filter(filter->asQualitativeCheckResult());
                }
                return result;
            });
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        void verifyWithDdEngine(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            verifyProperties<ValueType>(input, [&model] (std::shared_ptr<storm::logic::Formula const> const& formula, std::shared_ptr<storm::logic::Formula const> const& states) {
                bool filterForInitialStates = states->isInitialFormula();
                auto task = storm::api::createTask<ValueType>(formula, filterForInitialStates);
                
                auto symbolicModel = model->as<storm::models::symbolic::Model<DdType, ValueType>>();
                std::unique_ptr<storm::modelchecker::CheckResult> result = storm::api::verifyWithDdEngine<DdType, ValueType>(symbolicModel, storm::api::createTask<ValueType>(formula, true));
                
                std::unique_ptr<storm::modelchecker::CheckResult> filter;
                if (filterForInitialStates) {
                    filter = std::make_unique<storm::modelchecker::SymbolicQualitativeCheckResult<DdType>>(symbolicModel->getReachableStates(), symbolicModel->getInitialStates());
                } else {
                    filter = storm::api::verifyWithDdEngine<DdType, ValueType>(symbolicModel, storm::api::createTask<ValueType>(states, false));
                }
                if (result && filter) {
                    result->filter(filter->asQualitativeCheckResult());
                }
                return result;
            });
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        void verifyWithAbstractionRefinementEngine(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            verifyProperties<ValueType>(input, [&model] (std::shared_ptr<storm::logic::Formula const> const& formula, std::shared_ptr<storm::logic::Formula const> const& states) {
                STORM_LOG_THROW(states->isInitialFormula(), storm::exceptions::NotSupportedException, "Abstraction-refinement can only filter initial states.");
                auto symbolicModel = model->as<storm::models::symbolic::Model<DdType, ValueType>>();
                return storm::api::verifyWithAbstractionRefinementEngine<DdType, ValueType>(symbolicModel, storm::api::createTask<ValueType>(formula, true));
            });
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        typename std::enable_if<DdType != storm::dd::DdType::CUDD || std::is_same<ValueType, double>::value, void>::type verifySymbolicModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input, storm::settings::modules::CoreSettings const& coreSettings) {
            storm::settings::modules::CoreSettings::Engine engine = coreSettings.getEngine();;
            if (engine == storm::settings::modules::CoreSettings::Engine::Hybrid) {
                verifyWithHybridEngine<DdType, ValueType>(model, input);
            } else if (engine == storm::settings::modules::CoreSettings::Engine::Dd) {
                verifyWithDdEngine<DdType, ValueType>(model, input);
            } else {
                verifyWithAbstractionRefinementEngine<DdType, ValueType>(model, input);
            }
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        typename std::enable_if<DdType == storm::dd::DdType::CUDD && !std::is_same<ValueType, double>::value, void>::type verifySymbolicModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input, storm::settings::modules::CoreSettings const& coreSettings) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "CUDD does not support the selected data-type.");
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        void verifyModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input, storm::settings::modules::CoreSettings const& coreSettings) {
            if (model->isSparseModel()) {
                verifyWithSparseEngine<ValueType>(model, input);
            } else {
                STORM_LOG_ASSERT(model->isSymbolicModel(), "Unexpected model type.");
                verifySymbolicModel<DdType, ValueType>(model, input, coreSettings);
            }
        }
        
        template <storm::dd::DdType DdType, typename BuildValueType, typename VerificationValueType = BuildValueType>
        std::shared_ptr<storm::models::ModelBase> buildPreprocessExportModelWithValueTypeAndDdlib(SymbolicInput const& input, storm::settings::modules::CoreSettings::Engine engine) {
            auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
            auto buildSettings = storm::settings::getModule<storm::settings::modules::BuildSettings>();
            std::shared_ptr<storm::models::ModelBase> model;
            if (!buildSettings.isNoBuildModelSet()) {
                model = buildModel<DdType, BuildValueType>(engine, input, ioSettings);
            }
            
            if (model) {
                model->printModelInformationToStream(std::cout);
            }
            
            STORM_LOG_THROW(model || input.properties.empty(), storm::exceptions::InvalidSettingsException, "No input model.");
            
            if (model) {
                auto preprocessingResult = preprocessModel<DdType, BuildValueType, VerificationValueType>(model, input);
                if (preprocessingResult.second) {
                    model = preprocessingResult.first;
                    model->printModelInformationToStream(std::cout);
                }
                exportModel<DdType, BuildValueType>(model, input);
            }
            return model;
        }
        
        template <storm::dd::DdType DdType, typename BuildValueType, typename VerificationValueType = BuildValueType>
        void processInputWithValueTypeAndDdlib(SymbolicInput const& input) {
            auto coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
            auto abstractionSettings = storm::settings::getModule<storm::settings::modules::AbstractionSettings>();

            // For several engines, no model building step is performed, but the verification is started right away.
            storm::settings::modules::CoreSettings::Engine engine = coreSettings.getEngine();
            
            if (engine == storm::settings::modules::CoreSettings::Engine::AbstractionRefinement && abstractionSettings.getAbstractionRefinementMethod() == storm::settings::modules::AbstractionSettings::Method::Games) {
                verifyWithAbstractionRefinementEngine<DdType, VerificationValueType>(input);
            } else if (engine == storm::settings::modules::CoreSettings::Engine::Exploration) {
                verifyWithExplorationEngine<VerificationValueType>(input);
            } else {
                std::shared_ptr<storm::models::ModelBase> model = buildPreprocessExportModelWithValueTypeAndDdlib<DdType, BuildValueType, VerificationValueType>(input, engine);

                if (model) {
                    if (coreSettings.isCounterexampleSet()) {
                        auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
                        generateCounterexamples<VerificationValueType>(model, input);
                    } else {
                        auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
                        verifyModel<DdType, VerificationValueType>(model, input, coreSettings);
                    }
                }
            }
        }
        
        template <typename ValueType>
        void processInputWithValueType(SymbolicInput const& input) {
            auto coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
            auto generalSettings = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
            auto bisimulationSettings = storm::settings::getModule<storm::settings::modules::BisimulationSettings>();
            
            if (coreSettings.getDdLibraryType() == storm::dd::DdType::CUDD && coreSettings.isDdLibraryTypeSetFromDefaultValue() && generalSettings.isExactSet()) {
                STORM_LOG_INFO("Switching to DD library sylvan to allow for rational arithmetic.");
                processInputWithValueTypeAndDdlib<storm::dd::DdType::Sylvan, storm::RationalNumber>(input);
            } else if (coreSettings.getDdLibraryType() == storm::dd::DdType::CUDD && coreSettings.isDdLibraryTypeSetFromDefaultValue() && std::is_same<ValueType, double>::value && generalSettings.isBisimulationSet() && bisimulationSettings.useExactArithmeticInDdBisimulation()) {
                STORM_LOG_INFO("Switching to DD library sylvan to allow for rational arithmetic.");
                processInputWithValueTypeAndDdlib<storm::dd::DdType::Sylvan, storm::RationalNumber, double>(input);
            } else if (coreSettings.getDdLibraryType() == storm::dd::DdType::CUDD) {
                processInputWithValueTypeAndDdlib<storm::dd::DdType::CUDD, double>(input);
            } else {
                STORM_LOG_ASSERT(coreSettings.getDdLibraryType() == storm::dd::DdType::Sylvan, "Unknown DD library.");
                processInputWithValueTypeAndDdlib<storm::dd::DdType::Sylvan, ValueType>(input);
            }
        }
        
    }
}
