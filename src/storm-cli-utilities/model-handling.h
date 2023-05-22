#pragma once

#include "storm/api/storm.h"

#include "storm-counterexamples/api/counterexamples.h"
#include "storm-parsers/api/storm-parsers.h"

#include "storm/io/file.h"
#include "storm/utility/AutomaticSettings.h"
#include "storm/utility/Engine.h"
#include "storm/utility/NumberTraits.h"
#include "storm/utility/SignalHandler.h"
#include "storm/utility/macros.h"

#include "storm/utility/Stopwatch.h"
#include "storm/utility/initialize.h"

#include <type_traits>

#include "storm/storage/SymbolicModelDescription.h"
#include "storm/storage/jani/Property.h"

#include "storm/builder/BuilderType.h"

#include "storm/models/ModelBase.h"

#include "storm/environment/Environment.h"

#include "storm/exceptions/OptionParserException.h"

#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"

#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/symbolic/MarkovAutomaton.h"
#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/AbstractionSettings.h"
#include "storm/settings/modules/BuildSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/DebugSettings.h"
#include "storm/settings/modules/HintSettings.h"
#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/modules/ModelCheckerSettings.h"
#include "storm/settings/modules/ResourceSettings.h"
#include "storm/settings/modules/TransformationSettings.h"
#include "storm/storage/Qvbs.h"
#include "storm/storage/jani/localeliminator/AutomaticAction.h"
#include "storm/storage/jani/localeliminator/JaniLocalEliminator.h"

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

void parseSymbolicModelDescription(storm::settings::modules::IOSettings const& ioSettings, SymbolicInput& input) {
    auto buildSettings = storm::settings::getModule<storm::settings::modules::BuildSettings>();
    if (ioSettings.isPrismOrJaniInputSet()) {
        storm::utility::Stopwatch modelParsingWatch(true);
        if (ioSettings.isPrismInputSet()) {
            input.model =
                storm::api::parseProgram(ioSettings.getPrismInputFilename(), buildSettings.isPrismCompatibilityEnabled(), !buildSettings.isNoSimplifySet());
        } else {
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
            auto janiInput = storm::api::parseJaniModel(ioSettings.getJaniInputFilename(), propertyFilter);
            input.model = std::move(janiInput.first);
            if (ioSettings.isJaniPropertiesSet()) {
                input.properties = std::move(janiInput.second);
            }
        }
        modelParsingWatch.stop();
        STORM_PRINT("Time for model input parsing: " << modelParsingWatch << ".\n\n");
    }
}

void parseProperties(storm::settings::modules::IOSettings const& ioSettings, SymbolicInput& input,
                     boost::optional<std::set<std::string>> const& propertyFilter) {
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

SymbolicInput parseSymbolicInputQvbs(storm::settings::modules::IOSettings const& ioSettings) {
    // Parse the model input
    SymbolicInput input;
    storm::storage::QvbsBenchmark benchmark(ioSettings.getQvbsModelName());
    STORM_PRINT_AND_LOG(benchmark.getInfo(ioSettings.getQvbsInstanceIndex(), ioSettings.getQvbsPropertyFilter()));
    storm::utility::Stopwatch modelParsingWatch(true);
    auto janiInput = storm::api::parseJaniModel(benchmark.getJaniFile(ioSettings.getQvbsInstanceIndex()), ioSettings.getQvbsPropertyFilter());
    input.model = std::move(janiInput.first);
    input.properties = std::move(janiInput.second);
    modelParsingWatch.stop();
    STORM_PRINT("Time for model input parsing: " << modelParsingWatch << ".\n\n");

    // Parse additional properties
    boost::optional<std::set<std::string>> propertyFilter = storm::api::parsePropertyFilter(ioSettings.getPropertyFilter());
    parseProperties(ioSettings, input, propertyFilter);

    // Substitute constant definitions
    auto constantDefinitions = input.model.get().parseConstantDefinitions(benchmark.getConstantDefinition(ioSettings.getQvbsInstanceIndex()));
    input.model = input.model.get().preprocess(constantDefinitions);
    if (!input.properties.empty()) {
        input.properties = storm::api::substituteConstantsInProperties(input.properties, constantDefinitions);
    }

    return input;
}

SymbolicInput parseSymbolicInput() {
    auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
    if (ioSettings.isQvbsInputSet()) {
        return parseSymbolicInputQvbs(ioSettings);
    } else {
        // Parse the property filter, if any is given.
        boost::optional<std::set<std::string>> propertyFilter = storm::api::parsePropertyFilter(ioSettings.getPropertyFilter());

        SymbolicInput input;
        parseSymbolicModelDescription(ioSettings, input);
        parseProperties(ioSettings, input, propertyFilter);
        return input;
    }
}

struct ModelProcessingInformation {
    // The engine to use
    storm::utility::Engine engine;

    // If set, bisimulation will be applied.
    bool applyBisimulation;

    // If set, a transformation to Jani will be enforced
    bool transformToJani;

    // Which data type is to be used for numbers ...
    enum class ValueType { FinitePrecision, Exact, Parametric };
    ValueType buildValueType;         // ... during model building
    ValueType verificationValueType;  // ... during model verification

    // The Dd library to be used
    storm::dd::DdType ddType;

    // The environment used during model checking
    storm::Environment env;

    // A flag which is set to true, if the settings were detected to be compatible.
    // If this is false, it could be that the query can not be handled.
    bool isCompatible;
};

void getModelProcessingInformationAutomatic(SymbolicInput const& input, ModelProcessingInformation& mpi) {
    auto hints = storm::settings::getModule<storm::settings::modules::HintSettings>();

    STORM_LOG_THROW(input.model.is_initialized(), storm::exceptions::InvalidArgumentException, "Automatic engine requires a JANI input model.");
    STORM_LOG_THROW(input.model->isJaniModel(), storm::exceptions::InvalidArgumentException, "Automatic engine requires a JANI input model.");
    std::vector<storm::jani::Property> const& properties =
        input.preprocessedProperties.is_initialized() ? input.preprocessedProperties.get() : input.properties;
    STORM_LOG_THROW(!properties.empty(), storm::exceptions::InvalidArgumentException, "Automatic engine requires a property.");
    STORM_LOG_WARN_COND(properties.size() == 1,
                        "Automatic engine does not support decisions based on multiple properties. Only the first property will be considered.");

    storm::utility::AutomaticSettings as;
    if (hints.isNumberStatesSet()) {
        as.predict(input.model->asJaniModel(), properties.front(), hints.getNumberStates());
    } else {
        as.predict(input.model->asJaniModel(), properties.front());
    }

    mpi.engine = as.getEngine();
    if (as.enableBisimulation()) {
        mpi.applyBisimulation = true;
    }
    if (as.enableExact() && mpi.verificationValueType == ModelProcessingInformation::ValueType::FinitePrecision) {
        mpi.verificationValueType = ModelProcessingInformation::ValueType::Exact;
    }
    STORM_PRINT_AND_LOG("Automatic engine picked the following settings: \n"
                        << "\tengine=" << mpi.engine << std::boolalpha << "\t bisimulation=" << mpi.applyBisimulation
                        << "\t exact=" << (mpi.verificationValueType != ModelProcessingInformation::ValueType::FinitePrecision) << std::noboolalpha << '\n');
}

/*!
 * Sets the model processing information based on the given input.
 * Finding the right model processing information might require a conversion to jani.
 * In this case, the jani conversion is stored in the transformedJaniInput pointer (unless it is null)
 */
ModelProcessingInformation getModelProcessingInformation(SymbolicInput const& input, std::shared_ptr<SymbolicInput> const& transformedJaniInput = nullptr) {
    ModelProcessingInformation mpi;
    auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
    auto coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
    auto generalSettings = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
    auto bisimulationSettings = storm::settings::getModule<storm::settings::modules::BisimulationSettings>();

    // Set the engine.
    mpi.engine = coreSettings.getEngine();

    // Set whether bisimulation is to be used.
    mpi.applyBisimulation = generalSettings.isBisimulationSet();

    // Set the value type used for numeric values
    if (generalSettings.isParametricSet()) {
        mpi.verificationValueType = ModelProcessingInformation::ValueType::Parametric;
    } else if (generalSettings.isExactSet()) {
        mpi.verificationValueType = ModelProcessingInformation::ValueType::Exact;
    } else {
        mpi.verificationValueType = ModelProcessingInformation::ValueType::FinitePrecision;
    }
    auto originalVerificationValueType = mpi.verificationValueType;

    // Since the remaining settings could depend on the ones above, we need apply the automatic engine now.
    bool useAutomatic = input.model.is_initialized() && mpi.engine == storm::utility::Engine::Automatic;
    if (useAutomatic) {
        if (input.model->isJaniModel()) {
            // This can potentially overwrite the settings above, but will not overwrite settings that were explicitly set by the user (e.g. we will not disable
            // bisimulation or disable exact arithmetic)
            getModelProcessingInformationAutomatic(input, mpi);
        } else {
            // Transform Prism to jani first
            STORM_LOG_ASSERT(input.model->isPrismProgram(), "Unexpected type of input.");
            SymbolicInput janiInput;
            janiInput.properties = input.properties;
            storm::prism::Program const& prog = input.model.get().asPrismProgram();
            auto modelAndProperties = prog.toJani(input.preprocessedProperties.is_initialized() ? input.preprocessedProperties.get() : input.properties);
            janiInput.model = modelAndProperties.first;
            if (!modelAndProperties.second.empty()) {
                janiInput.preprocessedProperties = std::move(modelAndProperties.second);
            }
            // This can potentially overwrite the settings above, but will not overwrite settings that were explicitly set by the user (e.g. we will not disable
            // bisimulation or disable exact arithmetic)
            getModelProcessingInformationAutomatic(janiInput, mpi);
            if (transformedJaniInput) {
                // We cache the transformation result.
                *transformedJaniInput = std::move(janiInput);
            }
        }
    }

    // Check whether these settings are compatible with the provided input.
    if (input.model) {
        auto checkCompatibleSettings = [&mpi, &input] {
            switch (mpi.verificationValueType) {
                case ModelProcessingInformation::ValueType::Parametric:
                    return storm::utility::canHandle<storm::RationalFunction>(
                        mpi.engine, input.preprocessedProperties.is_initialized() ? input.preprocessedProperties.get() : input.properties, input.model.get());
                case ModelProcessingInformation::ValueType::Exact:
                    return storm::utility::canHandle<storm::RationalNumber>(
                        mpi.engine, input.preprocessedProperties.is_initialized() ? input.preprocessedProperties.get() : input.properties, input.model.get());
                    break;
                case ModelProcessingInformation::ValueType::FinitePrecision:
                    return storm::utility::canHandle<double>(
                        mpi.engine, input.preprocessedProperties.is_initialized() ? input.preprocessedProperties.get() : input.properties, input.model.get());
            }
            return false;
        };
        mpi.isCompatible = checkCompatibleSettings();
        if (!mpi.isCompatible) {
            if (useAutomatic) {
                bool useExact = mpi.verificationValueType != ModelProcessingInformation::ValueType::FinitePrecision;
                STORM_LOG_WARN("The settings picked by the automatic engine (engine="
                               << mpi.engine << ", bisim=" << mpi.applyBisimulation << ", exact=" << useExact
                               << ") are incompatible with this model. Falling back to default settings.");
                mpi.engine = storm::utility::Engine::Sparse;
                mpi.applyBisimulation = false;
                mpi.verificationValueType = originalVerificationValueType;
                // Retry check with new settings
                mpi.isCompatible = checkCompatibleSettings();
            }
        }
    } else {
        // If there is no input model, nothing has to be done, actually
        mpi.isCompatible = true;
    }

    // Set whether a transformation to jani is required or necessary
    mpi.transformToJani = ioSettings.isPrismToJaniSet();
    if (input.model) {
        auto builderType = storm::utility::getBuilderType(mpi.engine);
        bool transformToJaniForDdMA = (builderType == storm::builder::BuilderType::Dd) &&
                                      (input.model->getModelType() == storm::storage::SymbolicModelDescription::ModelType::MA) && (!input.model->isJaniModel());
        STORM_LOG_WARN_COND(mpi.transformToJani || !transformToJaniForDdMA,
                            "Dd-based model builder for Markov Automata is only available for JANI models, automatically converting the input model.");
        mpi.transformToJani |= transformToJaniForDdMA;
    }

    // Set the Valuetype used during model building
    mpi.buildValueType = mpi.verificationValueType;
    if (bisimulationSettings.useExactArithmeticInDdBisimulation()) {
        if (storm::utility::getBuilderType(mpi.engine) == storm::builder::BuilderType::Dd && mpi.applyBisimulation) {
            if (mpi.buildValueType == ModelProcessingInformation::ValueType::FinitePrecision) {
                mpi.buildValueType = ModelProcessingInformation::ValueType::Exact;
            }
        } else {
            STORM_LOG_WARN("Requested using exact arithmetic in Dd bisimulation but no dd bisimulation is applied.");
        }
    }

    // Set the Dd library
    mpi.ddType = coreSettings.getDdLibraryType();
    if (mpi.ddType == storm::dd::DdType::CUDD && coreSettings.isDdLibraryTypeSetFromDefaultValue()) {
        if (!(mpi.buildValueType == ModelProcessingInformation::ValueType::FinitePrecision &&
              mpi.verificationValueType == ModelProcessingInformation::ValueType::FinitePrecision)) {
            STORM_LOG_INFO("Switching to DD library sylvan to allow for rational arithmetic.");
            mpi.ddType = storm::dd::DdType::Sylvan;
        }
    }
    return mpi;
}

void ensureNoUndefinedPropertyConstants(std::vector<storm::jani::Property> const& properties) {
    // Make sure there are no undefined constants remaining in any property.
    for (auto const& property : properties) {
        std::set<storm::expressions::Variable> usedUndefinedConstants = property.getUndefinedConstants();
        if (!usedUndefinedConstants.empty()) {
            std::vector<std::string> undefinedConstantsNames;
            for (auto const& constant : usedUndefinedConstants) {
                undefinedConstantsNames.emplace_back(constant.getName());
            }
            STORM_LOG_THROW(
                false, storm::exceptions::InvalidArgumentException,
                "The property '" << property << " still refers to the undefined constants " << boost::algorithm::join(undefinedConstantsNames, ",") << ".");
        }
    }
}

std::pair<SymbolicInput, ModelProcessingInformation> preprocessSymbolicInput(SymbolicInput const& input) {
    auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();

    SymbolicInput output = input;

    // Preprocess properties (if requested)
    if (ioSettings.isPropertiesAsMultiSet()) {
        STORM_LOG_THROW(!input.properties.empty(), storm::exceptions::InvalidArgumentException,
                        "Can not translate properties to multi-objective formula because no properties were specified.");
        output.properties = {storm::api::createMultiObjectiveProperty(output.properties)};
    }

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
    ensureNoUndefinedPropertyConstants(output.properties);
    auto transformedJani = std::make_shared<SymbolicInput>();
    ModelProcessingInformation mpi = getModelProcessingInformation(output, transformedJani);

    // Check whether conversion for PRISM to JANI is requested or necessary.
    if (output.model && output.model.get().isPrismProgram()) {
        if (mpi.transformToJani) {
            if (transformedJani->model) {
                // Use the cached transformation if possible
                output = std::move(*transformedJani);
            } else {
                storm::prism::Program const& model = output.model.get().asPrismProgram();
                auto modelAndProperties = model.toJani(output.properties);

                output.model = modelAndProperties.first;

                if (!modelAndProperties.second.empty()) {
                    output.preprocessedProperties = std::move(modelAndProperties.second);
                }
            }
        }
    }

    if (output.model && output.model.get().isJaniModel()) {
        storm::jani::ModelFeatures supportedFeatures = storm::api::getSupportedJaniFeatures(storm::utility::getBuilderType(mpi.engine));
        storm::api::simplifyJaniModel(output.model.get().asJaniModel(), output.properties, supportedFeatures);

        const auto& buildSettings = storm::settings::getModule<storm::settings::modules::BuildSettings>();
        if (buildSettings.isLocationEliminationSet()) {
            auto locationHeuristic = buildSettings.getLocationEliminationLocationHeuristic();
            auto edgesHeuristic = buildSettings.getLocationEliminationEdgesHeuristic();
            output.model->setModel(storm::jani::JaniLocalEliminator::eliminateAutomatically(output.model.get().asJaniModel(), output.properties,
                                                                                            locationHeuristic, edgesHeuristic));
        }
    }

    return {output, mpi};
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

std::vector<std::shared_ptr<storm::logic::Formula const>> createFormulasToRespect(std::vector<storm::jani::Property> const& properties) {
    std::vector<std::shared_ptr<storm::logic::Formula const>> result = storm::api::extractFormulasFromProperties(properties);

    for (auto const& property : properties) {
        if (!property.getFilter().getStatesFormula()->isInitialFormula()) {
            result.push_back(property.getFilter().getStatesFormula());
        }
    }

    return result;
}

template<storm::dd::DdType DdType, typename ValueType>
std::shared_ptr<storm::models::ModelBase> buildModelDd(SymbolicInput const& input) {
    auto buildSettings = storm::settings::getModule<storm::settings::modules::BuildSettings>();
    return storm::api::buildSymbolicModel<DdType, ValueType>(input.model.get(), createFormulasToRespect(input.properties), buildSettings.isBuildFullModelSet(),
                                                             !buildSettings.isApplyNoMaximumProgressAssumptionSet());
}

template<typename ValueType>
std::shared_ptr<storm::models::ModelBase> buildModelSparse(SymbolicInput const& input, storm::settings::modules::BuildSettings const& buildSettings) {
    storm::builder::BuilderOptions options(createFormulasToRespect(input.properties), input.model.get());
    options.setBuildChoiceLabels(options.isBuildChoiceLabelsSet() || buildSettings.isBuildChoiceLabelsSet());
    options.setBuildStateValuations(options.isBuildStateValuationsSet() || buildSettings.isBuildStateValuationsSet());
    options.setBuildAllLabels(options.isBuildAllLabelsSet() || buildSettings.isBuildAllLabelsSet());
    options.setBuildObservationValuations(options.isBuildObservationValuationsSet() || buildSettings.isBuildObservationValuationsSet());
    bool buildChoiceOrigins = options.isBuildChoiceOriginsSet() || buildSettings.isBuildChoiceOriginsSet();
    if (storm::settings::manager().hasModule(storm::settings::modules::CounterexampleGeneratorSettings::moduleName)) {
        auto counterexampleGeneratorSettings = storm::settings::getModule<storm::settings::modules::CounterexampleGeneratorSettings>();
        if (counterexampleGeneratorSettings.isCounterexampleSet()) {
            buildChoiceOrigins |= counterexampleGeneratorSettings.isMinimalCommandSetGenerationSet();
        }
    }
    options.setBuildChoiceOrigins(buildChoiceOrigins);

    if (buildSettings.isApplyNoMaximumProgressAssumptionSet()) {
        options.setApplyMaximalProgressAssumption(false);
    }

    if (buildSettings.isExplorationChecksSet()) {
        options.setExplorationChecks();
    }
    options.setReservedBitsForUnboundedVariables(buildSettings.getBitsForUnboundedVariables());

    options.setAddOutOfBoundsState(buildSettings.isBuildOutOfBoundsStateSet());
    if (buildSettings.isBuildFullModelSet()) {
        options.clearTerminalStates();
        options.setApplyMaximalProgressAssumption(false);
        options.setBuildAllLabels(true);
        options.setBuildAllRewardModels(true);
    }

    if (buildSettings.isAddOverlappingGuardsLabelSet()) {
        options.setAddOverlappingGuardsLabel(true);
    }

    return storm::api::buildSparseModel<ValueType>(input.model.get(), options);
}

template<typename ValueType>
std::shared_ptr<storm::models::ModelBase> buildModelExplicit(storm::settings::modules::IOSettings const& ioSettings,
                                                             storm::settings::modules::BuildSettings const& buildSettings) {
    std::shared_ptr<storm::models::ModelBase> result;
    if (ioSettings.isExplicitSet()) {
        result = storm::api::buildExplicitModel<ValueType>(
            ioSettings.getTransitionFilename(), ioSettings.getLabelingFilename(),
            ioSettings.isStateRewardsSet() ? boost::optional<std::string>(ioSettings.getStateRewardsFilename()) : boost::none,
            ioSettings.isTransitionRewardsSet() ? boost::optional<std::string>(ioSettings.getTransitionRewardsFilename()) : boost::none,
            ioSettings.isChoiceLabelingSet() ? boost::optional<std::string>(ioSettings.getChoiceLabelingFilename()) : boost::none);
    } else if (ioSettings.isExplicitDRNSet()) {
        storm::parser::DirectEncodingParserOptions options;
        options.buildChoiceLabeling = buildSettings.isBuildChoiceLabelsSet();
        result = storm::api::buildExplicitDRNModel<ValueType>(ioSettings.getExplicitDRNFilename(), options);
    } else {
        STORM_LOG_THROW(ioSettings.isExplicitIMCASet(), storm::exceptions::InvalidSettingsException, "Unexpected explicit model input type.");
        result = storm::api::buildExplicitIMCAModel<ValueType>(ioSettings.getExplicitIMCAFilename());
    }
    return result;
}

template<storm::dd::DdType DdType, typename ValueType>
std::shared_ptr<storm::models::ModelBase> buildModel(SymbolicInput const& input, storm::settings::modules::IOSettings const& ioSettings,
                                                     ModelProcessingInformation const& mpi) {
    storm::utility::Stopwatch modelBuildingWatch(true);

    auto buildSettings = storm::settings::getModule<storm::settings::modules::BuildSettings>();
    std::shared_ptr<storm::models::ModelBase> result;
    if (input.model) {
        auto builderType = storm::utility::getBuilderType(mpi.engine);
        if (builderType == storm::builder::BuilderType::Dd) {
            result = buildModelDd<DdType, ValueType>(input);
        } else if (builderType == storm::builder::BuilderType::Explicit) {
            result = buildModelSparse<ValueType>(input, buildSettings);
        }
    } else if (ioSettings.isExplicitSet() || ioSettings.isExplicitDRNSet() || ioSettings.isExplicitIMCASet()) {
        STORM_LOG_THROW(mpi.engine == storm::utility::Engine::Sparse, storm::exceptions::InvalidSettingsException,
                        "Can only use sparse engine with explicit input.");
        result = buildModelExplicit<ValueType>(ioSettings, buildSettings);
    }

    modelBuildingWatch.stop();
    if (result) {
        STORM_PRINT("Time for model construction: " << modelBuildingWatch << ".\n\n");
    }

    return result;
}

template<typename ValueType>
std::shared_ptr<storm::models::sparse::Model<ValueType>> preprocessSparseMarkovAutomaton(
    std::shared_ptr<storm::models::sparse::MarkovAutomaton<ValueType>> const& model) {
    auto transformationSettings = storm::settings::getModule<storm::settings::modules::TransformationSettings>();
    auto debugSettings = storm::settings::getModule<storm::settings::modules::DebugSettings>();

    std::shared_ptr<storm::models::sparse::Model<ValueType>> result = model;
    model->close();
    STORM_LOG_WARN_COND(!debugSettings.isAdditionalChecksSet() || !model->containsZenoCycle(),
                        "MA contains a Zeno cycle. Model checking results cannot be trusted.");

    if (model->isConvertibleToCtmc()) {
        STORM_LOG_WARN_COND(false, "MA is convertible to a CTMC, consider using a CTMC instead.");
        result = model->convertToCtmc();
    }

    if (transformationSettings.isChainEliminationSet()) {
        // TODO: we should also transform the properties at this point.
        result = storm::api::eliminateNonMarkovianChains(result->template as<storm::models::sparse::MarkovAutomaton<ValueType>>(), {},
                                                         transformationSettings.getLabelBehavior())
                     .first;
    }

    return result;
}

template<typename ValueType>
std::shared_ptr<storm::models::sparse::Model<ValueType>> preprocessSparseModelBisimulation(
    std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input,
    storm::settings::modules::BisimulationSettings const& bisimulationSettings) {
    storm::storage::BisimulationType bisimType = storm::storage::BisimulationType::Strong;
    if (bisimulationSettings.isWeakBisimulationSet()) {
        bisimType = storm::storage::BisimulationType::Weak;
    }

    STORM_LOG_INFO("Performing bisimulation minimization...");
    return storm::api::performBisimulationMinimization<ValueType>(model, createFormulasToRespect(input.properties), bisimType);
}

template<typename ValueType>
std::pair<std::shared_ptr<storm::models::sparse::Model<ValueType>>, bool> preprocessSparseModel(
    std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input, ModelProcessingInformation const& mpi) {
    auto bisimulationSettings = storm::settings::getModule<storm::settings::modules::BisimulationSettings>();
    auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
    auto transformationSettings = storm::settings::getModule<storm::settings::modules::TransformationSettings>();

    std::pair<std::shared_ptr<storm::models::sparse::Model<ValueType>>, bool> result = std::make_pair(model, false);

    if (result.first->isOfType(storm::models::ModelType::MarkovAutomaton)) {
        result.first = preprocessSparseMarkovAutomaton(result.first->template as<storm::models::sparse::MarkovAutomaton<ValueType>>());
        result.second = true;
    }

    if (mpi.applyBisimulation) {
        result.first = preprocessSparseModelBisimulation(result.first, input, bisimulationSettings);
        result.second = true;
    }

    if (transformationSettings.isToDiscreteTimeModelSet()) {
        // TODO: we should also transform the properties at this point.
        STORM_LOG_WARN_COND(!model->hasRewardModel("_time"),
                            "Scheduled transformation to discrete time model, but a reward model named '_time' is already present in this model. We might take "
                            "the wrong reward model later.");
        result.first =
            storm::api::transformContinuousToDiscreteTimeSparseModel(std::move(*result.first), storm::api::extractFormulasFromProperties(input.properties))
                .first;
        result.second = true;
    }

    if (transformationSettings.isToNondeterministicModelSet()) {
        result.first = storm::api::transformToNondeterministicModel<ValueType>(std::move(*result.first));
        result.second = true;
    }

    return result;
}

template<typename ValueType>
void exportSparseModel(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input) {
    auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();

    if (ioSettings.isExportBuildSet()) {
        switch (ioSettings.getExportBuildFormat()) {
            case storm::exporter::ModelExportFormat::Dot:
                storm::api::exportSparseModelAsDot(model, ioSettings.getExportBuildFilename(), ioSettings.getExportDotMaxWidth());
                break;
            case storm::exporter::ModelExportFormat::Drn:
                storm::api::exportSparseModelAsDrn(model, ioSettings.getExportBuildFilename(),
                                                   input.model ? input.model.get().getParameterNames() : std::vector<std::string>(),
                                                   !ioSettings.isExplicitExportPlaceholdersDisabled());
                break;
            case storm::exporter::ModelExportFormat::Json:
                storm::api::exportSparseModelAsJson(model, ioSettings.getExportBuildFilename());
                break;
            default:
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                                "Exporting sparse models in " << storm::exporter::toString(ioSettings.getExportBuildFormat()) << " format is not supported.");
        }
    }

    // TODO: The following options are depreciated and shall be removed at some point:

    if (ioSettings.isExportExplicitSet()) {
        storm::api::exportSparseModelAsDrn(model, ioSettings.getExportExplicitFilename(),
                                           input.model ? input.model.get().getParameterNames() : std::vector<std::string>(),
                                           !ioSettings.isExplicitExportPlaceholdersDisabled());
    }

    if (ioSettings.isExportDdSet()) {
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Exporting in drdd format is only supported for DDs.");
    }

    if (ioSettings.isExportDotSet()) {
        storm::api::exportSparseModelAsDot(model, ioSettings.getExportDotFilename(), ioSettings.getExportDotMaxWidth());
    }
}

template<storm::dd::DdType DdType, typename ValueType>
void exportDdModel(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model, SymbolicInput const& input) {
    auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();

    if (ioSettings.isExportBuildSet()) {
        switch (ioSettings.getExportBuildFormat()) {
            case storm::exporter::ModelExportFormat::Dot:
                storm::api::exportSymbolicModelAsDot(model, ioSettings.getExportBuildFilename());
                break;
            case storm::exporter::ModelExportFormat::Drdd:
                storm::api::exportSymbolicModelAsDrdd(model, ioSettings.getExportBuildFilename());
                break;
            default:
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                                "Exporting symbolic models in " << storm::exporter::toString(ioSettings.getExportBuildFormat()) << " format is not supported.");
        }
    }

    // TODO: The following options are depreciated and shall be removed at some point:

    if (ioSettings.isExportExplicitSet()) {
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Exporting in drn format is only supported for sparse models.");
    }

    if (ioSettings.isExportDdSet()) {
        storm::api::exportSymbolicModelAsDrdd(model, ioSettings.getExportDdFilename());
    }

    if (ioSettings.isExportDotSet()) {
        storm::api::exportSymbolicModelAsDot(model, ioSettings.getExportDotFilename());
    }
}

template<storm::dd::DdType DdType, typename ValueType>
void exportModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
    if (model->isSparseModel()) {
        exportSparseModel<ValueType>(model->as<storm::models::sparse::Model<ValueType>>(), input);
    } else {
        exportDdModel<DdType, ValueType>(model->as<storm::models::symbolic::Model<DdType, ValueType>>(), input);
    }
}

template<storm::dd::DdType DdType, typename ValueType>
typename std::enable_if<DdType != storm::dd::DdType::Sylvan && !std::is_same<ValueType, double>::value, std::shared_ptr<storm::models::Model<ValueType>>>::type
preprocessDdMarkovAutomaton(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model) {
    return model;
}

template<storm::dd::DdType DdType, typename ValueType>
typename std::enable_if<DdType == storm::dd::DdType::Sylvan || std::is_same<ValueType, double>::value, std::shared_ptr<storm::models::Model<ValueType>>>::type
preprocessDdMarkovAutomaton(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model) {
    auto ma = model->template as<storm::models::symbolic::MarkovAutomaton<DdType, ValueType>>();
    if (!ma->isClosed()) {
        return std::make_shared<storm::models::symbolic::MarkovAutomaton<DdType, ValueType>>(ma->close());
    } else {
        return model;
    }
}

template<storm::dd::DdType DdType, typename ValueType, typename ExportValueType = ValueType>
std::shared_ptr<storm::models::Model<ExportValueType>> preprocessDdModelBisimulation(
    std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model, SymbolicInput const& input,
    storm::settings::modules::BisimulationSettings const& bisimulationSettings, ModelProcessingInformation const& mpi) {
    STORM_LOG_WARN_COND(!bisimulationSettings.isWeakBisimulationSet(),
                        "Weak bisimulation is currently not supported on DDs. Falling back to strong bisimulation.");

    auto quotientFormat = bisimulationSettings.getQuotientFormat();
    if (mpi.engine == storm::utility::Engine::DdSparse && quotientFormat != storm::dd::bisimulation::QuotientFormat::Sparse &&
        bisimulationSettings.isQuotientFormatSetFromDefaultValue()) {
        STORM_LOG_INFO("Setting bisimulation quotient format to 'sparse'.");
        quotientFormat = storm::dd::bisimulation::QuotientFormat::Sparse;
    }
    STORM_LOG_INFO("Performing bisimulation minimization...");
    return storm::api::performBisimulationMinimization<DdType, ValueType, ExportValueType>(
        model, createFormulasToRespect(input.properties), storm::storage::BisimulationType::Strong, bisimulationSettings.getSignatureMode(), quotientFormat);
}

template<storm::dd::DdType DdType, typename ValueType, typename ExportValueType = ValueType>
std::pair<std::shared_ptr<storm::models::ModelBase>, bool> preprocessDdModel(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model,
                                                                             SymbolicInput const& input, ModelProcessingInformation const& mpi) {
    auto bisimulationSettings = storm::settings::getModule<storm::settings::modules::BisimulationSettings>();
    std::pair<std::shared_ptr<storm::models::Model<ValueType>>, bool> intermediateResult = std::make_pair(model, false);

    if (model->isOfType(storm::models::ModelType::MarkovAutomaton)) {
        intermediateResult.first = preprocessDdMarkovAutomaton(intermediateResult.first->template as<storm::models::symbolic::Model<DdType, ValueType>>());
        intermediateResult.second = true;
    }

    std::unique_ptr<std::pair<std::shared_ptr<storm::models::Model<ExportValueType>>, bool>> result;
    auto symbolicModel = intermediateResult.first->template as<storm::models::symbolic::Model<DdType, ValueType>>();
    if (mpi.applyBisimulation) {
        std::shared_ptr<storm::models::Model<ExportValueType>> newModel =
            preprocessDdModelBisimulation<DdType, ValueType, ExportValueType>(symbolicModel, input, bisimulationSettings, mpi);
        result = std::make_unique<std::pair<std::shared_ptr<storm::models::Model<ExportValueType>>, bool>>(newModel, true);
    } else {
        result = std::make_unique<std::pair<std::shared_ptr<storm::models::Model<ExportValueType>>, bool>>(
            symbolicModel->template toValueType<ExportValueType>(), !std::is_same<ValueType, ExportValueType>::value);
    }

    if (result && result->first->isSymbolicModel() && mpi.engine == storm::utility::Engine::DdSparse) {
        // Mark as changed.
        result->second = true;

        std::shared_ptr<storm::models::symbolic::Model<DdType, ExportValueType>> symbolicModel =
            result->first->template as<storm::models::symbolic::Model<DdType, ExportValueType>>();
        std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
        for (auto const& property : input.properties) {
            formulas.emplace_back(property.getRawFormula());
        }
        result->first = storm::api::transformSymbolicToSparseModel(symbolicModel, formulas);
        STORM_LOG_THROW(result, storm::exceptions::NotSupportedException, "The translation to a sparse model is not supported for the given model type.");
    }

    return *result;
}

template<storm::dd::DdType DdType, typename BuildValueType, typename ExportValueType = BuildValueType>
std::pair<std::shared_ptr<storm::models::ModelBase>, bool> preprocessModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input,
                                                                           ModelProcessingInformation const& mpi) {
    storm::utility::Stopwatch preprocessingWatch(true);

    std::pair<std::shared_ptr<storm::models::ModelBase>, bool> result = std::make_pair(model, false);
    if (model->isSparseModel()) {
        result = preprocessSparseModel<BuildValueType>(result.first->as<storm::models::sparse::Model<BuildValueType>>(), input, mpi);
    } else {
        STORM_LOG_ASSERT(model->isSymbolicModel(), "Unexpected model type.");
        result =
            preprocessDdModel<DdType, BuildValueType, ExportValueType>(result.first->as<storm::models::symbolic::Model<DdType, BuildValueType>>(), input, mpi);
    }

    preprocessingWatch.stop();

    if (result.second) {
        STORM_PRINT("\nTime for model preprocessing: " << preprocessingWatch << ".\n\n");
    }
    return result;
}

void printComputingCounterexample(storm::jani::Property const& property) {
    STORM_PRINT("Computing counterexample for property " << *property.getRawFormula() << " ...\n");
}

void printCounterexample(std::shared_ptr<storm::counterexamples::Counterexample> const& counterexample, storm::utility::Stopwatch* watch = nullptr) {
    if (counterexample) {
        STORM_PRINT(*counterexample << '\n');
        if (watch) {
            STORM_PRINT("Time for computation: " << *watch << ".\n");
        }
    } else {
        STORM_PRINT(" failed.\n");
    }
}

template<typename ValueType>
void generateCounterexamples(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Counterexample generation is not supported for this data-type.");
}

template<>
void generateCounterexamples<double>(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
    typedef double ValueType;

    STORM_LOG_THROW(model->isSparseModel(), storm::exceptions::NotSupportedException,
                    "Counterexample generation is currently only supported for sparse models.");
    auto sparseModel = model->as<storm::models::sparse::Model<ValueType>>();
    for (auto& rewModel : sparseModel->getRewardModels()) {
        rewModel.second.reduceToStateBasedRewards(sparseModel->getTransitionMatrix(), true);
    }

    STORM_LOG_THROW(sparseModel->isOfType(storm::models::ModelType::Dtmc) || sparseModel->isOfType(storm::models::ModelType::Mdp),
                    storm::exceptions::NotSupportedException, "Counterexample is currently only supported for discrete-time models.");

    auto counterexampleSettings = storm::settings::getModule<storm::settings::modules::CounterexampleGeneratorSettings>();
    if (counterexampleSettings.isMinimalCommandSetGenerationSet()) {
        bool useMilp = counterexampleSettings.isUseMilpBasedMinimalCommandSetGenerationSet();
        for (auto const& property : input.properties) {
            std::shared_ptr<storm::counterexamples::Counterexample> counterexample;
            printComputingCounterexample(property);
            storm::utility::Stopwatch watch(true);
            if (useMilp) {
                STORM_LOG_THROW(sparseModel->isOfType(storm::models::ModelType::Mdp), storm::exceptions::NotSupportedException,
                                "Counterexample generation using MILP is currently only supported for MDPs.");
                counterexample = storm::api::computeHighLevelCounterexampleMilp(
                    input.model.get(), sparseModel->template as<storm::models::sparse::Mdp<ValueType>>(), property.getRawFormula());
            } else {
                STORM_LOG_THROW(sparseModel->isOfType(storm::models::ModelType::Dtmc) || sparseModel->isOfType(storm::models::ModelType::Mdp),
                                storm::exceptions::NotSupportedException,
                                "Counterexample generation using MaxSAT is currently only supported for discrete-time models.");

                if (sparseModel->isOfType(storm::models::ModelType::Dtmc)) {
                    counterexample = storm::api::computeHighLevelCounterexampleMaxSmt(
                        input.model.get(), sparseModel->template as<storm::models::sparse::Dtmc<ValueType>>(), property.getRawFormula());
                } else {
                    counterexample = storm::api::computeHighLevelCounterexampleMaxSmt(
                        input.model.get(), sparseModel->template as<storm::models::sparse::Mdp<ValueType>>(), property.getRawFormula());
                }
            }
            watch.stop();
            printCounterexample(counterexample, &watch);
        }
    } else if (counterexampleSettings.isShortestPathGenerationSet()) {
        for (auto const& property : input.properties) {
            std::shared_ptr<storm::counterexamples::Counterexample> counterexample;
            printComputingCounterexample(property);
            storm::utility::Stopwatch watch(true);
            STORM_LOG_THROW(sparseModel->isOfType(storm::models::ModelType::Dtmc), storm::exceptions::NotSupportedException,
                            "Counterexample generation using shortest paths is currently only supported for DTMCs.");
            counterexample = storm::api::computeKShortestPathCounterexample(sparseModel->template as<storm::models::sparse::Dtmc<ValueType>>(),
                                                                            property.getRawFormula(), counterexampleSettings.getShortestPathMaxK());
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
                STORM_PRINT(*result << '\n');
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
    STORM_PRINT('\n');
}

void printModelCheckingProperty(storm::jani::Property const& property) {
    STORM_PRINT("\nModel checking property \"" << property.getName() << "\": " << *property.getRawFormula() << " ...\n");
}

template<typename ValueType>
void printResult(std::unique_ptr<storm::modelchecker::CheckResult> const& result, storm::jani::Property const& property,
                 storm::utility::Stopwatch* watch = nullptr) {
    if (result) {
        std::stringstream ss;
        ss << "'" << *property.getFilter().getStatesFormula() << "'";
        STORM_PRINT((storm::utility::resources::isTerminate() ? "Result till abort" : "Result")
                    << " (for " << (property.getFilter().getStatesFormula()->isInitialFormula() ? "initial" : ss.str()) << " states): ");
        printFilteredResult<ValueType>(result, property.getFilter().getFilterType());
        if (watch) {
            STORM_PRINT("Time for model checking: " << *watch << ".\n");
        }
    } else {
        STORM_LOG_ERROR("Property is unsupported by selected engine/settings.\n");
    }
}

struct PostprocessingIdentity {
    void operator()(std::unique_ptr<storm::modelchecker::CheckResult> const&) {
        // Intentionally left empty.
    }
};

template<typename ValueType>
void verifyProperties(
    SymbolicInput const& input,
    std::function<std::unique_ptr<storm::modelchecker::CheckResult>(std::shared_ptr<storm::logic::Formula const> const& formula,
                                                                    std::shared_ptr<storm::logic::Formula const> const& states)> const& verificationCallback,
    std::function<void(std::unique_ptr<storm::modelchecker::CheckResult> const&)> const& postprocessingCallback = PostprocessingIdentity()) {
    auto transformationSettings = storm::settings::getModule<storm::settings::modules::TransformationSettings>();
    auto const& properties = input.preprocessedProperties ? input.preprocessedProperties.get() : input.properties;
    for (auto const& property : properties) {
        printModelCheckingProperty(property);
        bool ignored = false;
        storm::utility::Stopwatch watch(true);
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        try {
            auto rawFormula = property.getRawFormula();
            if (transformationSettings.isChainEliminationSet() && !storm::transformer::NonMarkovianChainTransformer<ValueType>::preservesFormula(*rawFormula)) {
                STORM_LOG_WARN("Property is not preserved by elimination of non-markovian states.");
                ignored = true;
            } else if (transformationSettings.isToDiscreteTimeModelSet()) {
                auto propertyFormula = storm::api::checkAndTransformContinuousToDiscreteTimeFormula<ValueType>(*property.getRawFormula());
                auto filterFormula = storm::api::checkAndTransformContinuousToDiscreteTimeFormula<ValueType>(*property.getFilter().getStatesFormula());
                if (propertyFormula && filterFormula) {
                    result = verificationCallback(propertyFormula, filterFormula);
                } else {
                    ignored = true;
                }
            } else {
                result = verificationCallback(property.getRawFormula(), property.getFilter().getStatesFormula());
            }
        } catch (storm::exceptions::BaseException const& ex) {
            STORM_LOG_WARN("Cannot handle property: " << ex.what());
        }
        watch.stop();
        if (!ignored) {
            postprocessingCallback(result);
            printResult<ValueType>(result, property, &watch);
        }
    }
}

std::vector<storm::expressions::Expression> parseConstraints(storm::expressions::ExpressionManager const& expressionManager,
                                                             std::string const& constraintsString) {
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

std::vector<std::vector<storm::expressions::Expression>> parseInjectedRefinementPredicates(storm::expressions::ExpressionManager const& expressionManager,
                                                                                           std::string const& refinementPredicatesString) {
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

                STORM_LOG_THROW(!injectedRefinementPredicates.back().empty(), storm::exceptions::InvalidArgumentException,
                                "Expecting non-empty list of predicates to inject for each (mentioned) refinement step.");

                // Finally reverse the list, because we take the predicates from the back.
                std::reverse(injectedRefinementPredicates.back().begin(), injectedRefinementPredicates.back().end());
            }
        }

        // Finally reverse the list, because we take the predicates from the back.
        std::reverse(injectedRefinementPredicates.begin(), injectedRefinementPredicates.end());
    }

    return injectedRefinementPredicates;
}

template<storm::dd::DdType DdType, typename ValueType>
void verifyWithAbstractionRefinementEngine(SymbolicInput const& input, ModelProcessingInformation const& mpi) {
    STORM_LOG_ASSERT(input.model, "Expected symbolic model description.");
    storm::settings::modules::AbstractionSettings const& abstractionSettings = storm::settings::getModule<storm::settings::modules::AbstractionSettings>();
    storm::api::AbstractionRefinementOptions options(
        parseConstraints(input.model->getManager(), abstractionSettings.getConstraintString()),
        parseInjectedRefinementPredicates(input.model->getManager(), abstractionSettings.getInjectedRefinementPredicates()));

    verifyProperties<ValueType>(input, [&input, &options, &mpi](std::shared_ptr<storm::logic::Formula const> const& formula,
                                                                std::shared_ptr<storm::logic::Formula const> const& states) {
        STORM_LOG_THROW(states->isInitialFormula(), storm::exceptions::NotSupportedException, "Abstraction-refinement can only filter initial states.");
        return storm::api::verifyWithAbstractionRefinementEngine<DdType, ValueType>(mpi.env, input.model.get(),
                                                                                    storm::api::createTask<ValueType>(formula, true), options);
    });
}

template<typename ValueType>
void verifyWithExplorationEngine(SymbolicInput const& input, ModelProcessingInformation const& mpi) {
    STORM_LOG_ASSERT(input.model, "Expected symbolic model description.");
    STORM_LOG_THROW((std::is_same<ValueType, double>::value), storm::exceptions::NotSupportedException,
                    "Exploration does not support other data-types than floating points.");
    verifyProperties<ValueType>(
        input, [&input, &mpi](std::shared_ptr<storm::logic::Formula const> const& formula, std::shared_ptr<storm::logic::Formula const> const& states) {
            STORM_LOG_THROW(states->isInitialFormula(), storm::exceptions::NotSupportedException, "Exploration can only filter initial states.");
            return storm::api::verifyWithExplorationEngine<ValueType>(mpi.env, input.model.get(), storm::api::createTask<ValueType>(formula, true));
        });
}

template<typename ValueType>
void verifyWithSparseEngine(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input, ModelProcessingInformation const& mpi) {
    auto sparseModel = model->as<storm::models::sparse::Model<ValueType>>();
    auto const& ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
    auto verificationCallback = [&sparseModel, &ioSettings, &mpi](std::shared_ptr<storm::logic::Formula const> const& formula,
                                                                  std::shared_ptr<storm::logic::Formula const> const& states) {
        bool filterForInitialStates = states->isInitialFormula();
        auto task = storm::api::createTask<ValueType>(formula, filterForInitialStates);
        if (ioSettings.isExportSchedulerSet()) {
            task.setProduceSchedulers(true);
        }
        std::unique_ptr<storm::modelchecker::CheckResult> result = storm::api::verifyWithSparseEngine<ValueType>(mpi.env, sparseModel, task);

        std::unique_ptr<storm::modelchecker::CheckResult> filter;
        if (filterForInitialStates) {
            filter = std::make_unique<storm::modelchecker::ExplicitQualitativeCheckResult>(sparseModel->getInitialStates());
        } else {
            filter = storm::api::verifyWithSparseEngine<ValueType>(mpi.env, sparseModel, storm::api::createTask<ValueType>(states, false));
        }
        if (result && filter) {
            result->filter(filter->asQualitativeCheckResult());
        }
        return result;
    };
    uint64_t exportCount = 0;  // this number will be prepended to the export file name of schedulers and/or check results in case of multiple properties.
    auto postprocessingCallback = [&sparseModel, &ioSettings, &input, &exportCount](std::unique_ptr<storm::modelchecker::CheckResult> const& result) {
        if (ioSettings.isExportSchedulerSet()) {
            if (result->isExplicitQuantitativeCheckResult()) {
                if (result->template asExplicitQuantitativeCheckResult<ValueType>().hasScheduler()) {
                    auto const& scheduler = result->template asExplicitQuantitativeCheckResult<ValueType>().getScheduler();
                    STORM_PRINT_AND_LOG("Exporting scheduler ... ")
                    if (input.model) {
                        STORM_LOG_WARN_COND(sparseModel->hasStateValuations(),
                                            "No information of state valuations available. The scheduler output will use internal state ids. You might be "
                                            "interested in building the model with state valuations using --buildstateval.");
                        STORM_LOG_WARN_COND(
                            sparseModel->hasChoiceLabeling() || sparseModel->hasChoiceOrigins(),
                            "No symbolic choice information is available. The scheduler output will use internal choice ids. You might be interested in "
                            "building the model with choice labels or choice origins using --buildchoicelab or --buildchoiceorig.");
                        STORM_LOG_WARN_COND(sparseModel->hasChoiceLabeling() && !sparseModel->hasChoiceOrigins(),
                                            "Only partial choice information is available. You might want to build the model with choice origins using "
                                            "--buildchoicelab or --buildchoiceorig.");
                    }
                    STORM_LOG_WARN_COND(exportCount == 0,
                                        "Prepending " << exportCount << " to file name for this property because there are multiple properties.");
                    storm::api::exportScheduler(sparseModel, scheduler,
                                                (exportCount == 0 ? std::string("") : std::to_string(exportCount)) + ioSettings.getExportSchedulerFilename());
                } else {
                    STORM_LOG_ERROR("Scheduler requested but could not be generated.");
                }
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Scheduler export not supported for this property.");
            }
        }
        if (ioSettings.isExportCheckResultSet()) {
            STORM_LOG_WARN_COND(sparseModel->hasStateValuations(),
                                "No information of state valuations available. The result output will use internal state ids. You might be interested in "
                                "building the model with state valuations using --buildstateval.");
            STORM_LOG_WARN_COND(exportCount == 0, "Prepending " << exportCount << " to file name for this property because there are multiple properties.");
            storm::api::exportCheckResultToJson(sparseModel, result,
                                                (exportCount == 0 ? std::string("") : std::to_string(exportCount)) + ioSettings.getExportCheckResultFilename());
        }
        ++exportCount;
    };
    verifyProperties<ValueType>(input, verificationCallback, postprocessingCallback);
    if (ioSettings.isComputeSteadyStateDistributionSet()) {
        storm::utility::Stopwatch watch(true);
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        try {
            result = storm::api::computeSteadyStateDistributionWithSparseEngine<ValueType>(mpi.env, sparseModel);
        } catch (storm::exceptions::BaseException const& ex) {
            STORM_LOG_WARN("Cannot compute steady-state probabilities: " << ex.what());
        }
        watch.stop();
        postprocessingCallback(result);
        STORM_PRINT((storm::utility::resources::isTerminate() ? "Result till abort: " : "Result: ") << *result << '\n');
        STORM_PRINT("Time for model checking: " << watch << ".\n");
    }
    if (ioSettings.isComputeExpectedVisitingTimesSet()) {
        storm::utility::Stopwatch watch(true);
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        try {
            result = storm::api::computeExpectedVisitingTimesWithSparseEngine<ValueType>(mpi.env, sparseModel);
        } catch (storm::exceptions::BaseException const& ex) {
            STORM_LOG_WARN("Cannot compute expected visiting times: " << ex.what());
        }
        watch.stop();
        postprocessingCallback(result);
        STORM_PRINT((storm::utility::resources::isTerminate() ? "Result till abort: " : "Result: ") << *result << '\n');
        STORM_PRINT("Time for model checking: " << watch << ".\n");
    }
}

template<storm::dd::DdType DdType, typename ValueType>
void verifyWithHybridEngine(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input, ModelProcessingInformation const& mpi) {
    verifyProperties<ValueType>(
        input, [&model, &mpi](std::shared_ptr<storm::logic::Formula const> const& formula, std::shared_ptr<storm::logic::Formula const> const& states) {
            bool filterForInitialStates = states->isInitialFormula();
            auto task = storm::api::createTask<ValueType>(formula, filterForInitialStates);

            auto symbolicModel = model->as<storm::models::symbolic::Model<DdType, ValueType>>();
            std::unique_ptr<storm::modelchecker::CheckResult> result = storm::api::verifyWithHybridEngine<DdType, ValueType>(mpi.env, symbolicModel, task);

            std::unique_ptr<storm::modelchecker::CheckResult> filter;
            if (filterForInitialStates) {
                filter = std::make_unique<storm::modelchecker::SymbolicQualitativeCheckResult<DdType>>(symbolicModel->getReachableStates(),
                                                                                                       symbolicModel->getInitialStates());
            } else {
                filter = storm::api::verifyWithHybridEngine<DdType, ValueType>(mpi.env, symbolicModel, storm::api::createTask<ValueType>(states, false));
            }
            if (result && filter) {
                result->filter(filter->asQualitativeCheckResult());
            }
            return result;
        });
}

template<storm::dd::DdType DdType, typename ValueType>
void verifyWithDdEngine(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input, ModelProcessingInformation const& mpi) {
    verifyProperties<ValueType>(
        input, [&model, &mpi](std::shared_ptr<storm::logic::Formula const> const& formula, std::shared_ptr<storm::logic::Formula const> const& states) {
            bool filterForInitialStates = states->isInitialFormula();
            auto task = storm::api::createTask<ValueType>(formula, filterForInitialStates);

            auto symbolicModel = model->as<storm::models::symbolic::Model<DdType, ValueType>>();
            std::unique_ptr<storm::modelchecker::CheckResult> result =
                storm::api::verifyWithDdEngine<DdType, ValueType>(mpi.env, symbolicModel, storm::api::createTask<ValueType>(formula, true));

            std::unique_ptr<storm::modelchecker::CheckResult> filter;
            if (filterForInitialStates) {
                filter = std::make_unique<storm::modelchecker::SymbolicQualitativeCheckResult<DdType>>(symbolicModel->getReachableStates(),
                                                                                                       symbolicModel->getInitialStates());
            } else {
                filter = storm::api::verifyWithDdEngine<DdType, ValueType>(mpi.env, symbolicModel, storm::api::createTask<ValueType>(states, false));
            }
            if (result && filter) {
                result->filter(filter->asQualitativeCheckResult());
            }
            return result;
        });
}

template<storm::dd::DdType DdType, typename ValueType>
void verifyWithAbstractionRefinementEngine(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input,
                                           ModelProcessingInformation const& mpi) {
    verifyProperties<ValueType>(input, [&model, &mpi](std::shared_ptr<storm::logic::Formula const> const& formula,
                                                      std::shared_ptr<storm::logic::Formula const> const& states) {
        STORM_LOG_THROW(states->isInitialFormula(), storm::exceptions::NotSupportedException, "Abstraction-refinement can only filter initial states.");
        auto symbolicModel = model->as<storm::models::symbolic::Model<DdType, ValueType>>();
        return storm::api::verifyWithAbstractionRefinementEngine<DdType, ValueType>(mpi.env, symbolicModel, storm::api::createTask<ValueType>(formula, true));
    });
}

template<storm::dd::DdType DdType, typename ValueType>
typename std::enable_if<DdType != storm::dd::DdType::CUDD || std::is_same<ValueType, double>::value, void>::type verifySymbolicModel(
    std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input, ModelProcessingInformation const& mpi) {
    if (mpi.engine == storm::utility::Engine::Hybrid) {
        verifyWithHybridEngine<DdType, ValueType>(model, input, mpi);
    } else if (mpi.engine == storm::utility::Engine::Dd) {
        verifyWithDdEngine<DdType, ValueType>(model, input, mpi);
    } else {
        verifyWithAbstractionRefinementEngine<DdType, ValueType>(model, input, mpi);
    }
}

template<storm::dd::DdType DdType, typename ValueType>
typename std::enable_if<DdType == storm::dd::DdType::CUDD && !std::is_same<ValueType, double>::value, void>::type verifySymbolicModel(
    std::shared_ptr<storm::models::ModelBase> const&, SymbolicInput const&, ModelProcessingInformation const&) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "CUDD does not support the selected data-type.");
}

template<storm::dd::DdType DdType, typename ValueType>
void verifyModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input, ModelProcessingInformation const& mpi) {
    if (model->isSparseModel()) {
        verifyWithSparseEngine<ValueType>(model, input, mpi);
    } else {
        STORM_LOG_ASSERT(model->isSymbolicModel(), "Unexpected model type.");
        verifySymbolicModel<DdType, ValueType>(model, input, mpi);
    }
}

template<storm::dd::DdType DdType, typename BuildValueType, typename VerificationValueType = BuildValueType>
std::shared_ptr<storm::models::ModelBase> buildPreprocessModelWithValueTypeAndDdlib(SymbolicInput const& input, ModelProcessingInformation const& mpi) {
    auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
    auto buildSettings = storm::settings::getModule<storm::settings::modules::BuildSettings>();
    std::shared_ptr<storm::models::ModelBase> model;
    if (!buildSettings.isNoBuildModelSet()) {
        model = buildModel<DdType, BuildValueType>(input, ioSettings, mpi);
    }

    if (model) {
        model->printModelInformationToStream(std::cout);
    }

    STORM_LOG_THROW(model || input.properties.empty(), storm::exceptions::InvalidSettingsException, "No input model.");

    if (model) {
        auto preprocessingResult = preprocessModel<DdType, BuildValueType, VerificationValueType>(model, input, mpi);
        if (preprocessingResult.second) {
            model = preprocessingResult.first;
            model->printModelInformationToStream(std::cout);
        }
    }
    return model;
}

template<storm::dd::DdType DdType, typename BuildValueType, typename VerificationValueType = BuildValueType>
std::shared_ptr<storm::models::ModelBase> buildPreprocessExportModelWithValueTypeAndDdlib(SymbolicInput const& input, ModelProcessingInformation const& mpi) {
    auto model = buildPreprocessModelWithValueTypeAndDdlib<DdType, BuildValueType, VerificationValueType>(input, mpi);
    if (model) {
        exportModel<DdType, BuildValueType>(model, input);
    }
    return model;
}

template<storm::dd::DdType DdType, typename BuildValueType, typename VerificationValueType = BuildValueType>
void processInputWithValueTypeAndDdlib(SymbolicInput const& input, ModelProcessingInformation const& mpi) {
    auto abstractionSettings = storm::settings::getModule<storm::settings::modules::AbstractionSettings>();
    auto counterexampleSettings = storm::settings::getModule<storm::settings::modules::CounterexampleGeneratorSettings>();

    // For several engines, no model building step is performed, but the verification is started right away.
    if (mpi.engine == storm::utility::Engine::AbstractionRefinement &&
        abstractionSettings.getAbstractionRefinementMethod() == storm::settings::modules::AbstractionSettings::Method::Games) {
        verifyWithAbstractionRefinementEngine<DdType, VerificationValueType>(input, mpi);
    } else if (mpi.engine == storm::utility::Engine::Exploration) {
        verifyWithExplorationEngine<VerificationValueType>(input, mpi);
    } else {
        std::shared_ptr<storm::models::ModelBase> model =
            buildPreprocessExportModelWithValueTypeAndDdlib<DdType, BuildValueType, VerificationValueType>(input, mpi);
        if (model) {
            if (counterexampleSettings.isCounterexampleSet()) {
                generateCounterexamples<VerificationValueType>(model, input);
            } else {
                verifyModel<DdType, VerificationValueType>(model, input, mpi);
            }
        }
    }
}

template<typename ValueType>
void processInputWithValueType(SymbolicInput const& input, ModelProcessingInformation const& mpi) {
    if (mpi.ddType == storm::dd::DdType::CUDD) {
        STORM_LOG_ASSERT(mpi.verificationValueType == ModelProcessingInformation::ValueType::FinitePrecision &&
                             mpi.buildValueType == ModelProcessingInformation::ValueType::FinitePrecision && (std::is_same<ValueType, double>::value),
                         "Unexpected value type for Dd library cudd.");
        processInputWithValueTypeAndDdlib<storm::dd::DdType::CUDD, double>(input, mpi);
    } else {
        STORM_LOG_ASSERT(mpi.ddType == storm::dd::DdType::Sylvan, "Unknown DD library.");
        if (mpi.buildValueType == mpi.verificationValueType) {
            processInputWithValueTypeAndDdlib<storm::dd::DdType::Sylvan, ValueType>(input, mpi);
        } else {
            // Right now, we only require (buildType == Exact and verificationType == FinitePrecision).
            // We exclude all other combinations to safe a few template instantiations.
            STORM_LOG_THROW((std::is_same<ValueType, double>::value) && mpi.buildValueType == ModelProcessingInformation::ValueType::Exact,
                            storm::exceptions::InvalidArgumentException, "Unexpected combination of buildValueType and verificationValueType");
#ifdef STORM_HAVE_CARL
            processInputWithValueTypeAndDdlib<storm::dd::DdType::Sylvan, storm::RationalNumber, double>(input, mpi);
#else
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Unexpected buildValueType.");
#endif
        }
    }
}
}  // namespace cli
}  // namespace storm
