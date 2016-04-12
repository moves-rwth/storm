#include "src/settings/modules/ExplorationSettings.h"
#include "src/settings/modules/GeneralSettings.h"
#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/Argument.h"
#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string ExplorationSettings::moduleName = "exploration";
            const std::string ExplorationSettings::precomputationTypeOptionName = "precomp";
            const std::string ExplorationSettings::numberOfExplorationStepsUntilPrecomputationOptionName = "stepsprecomp";
            const std::string ExplorationSettings::numberOfSampledPathsUntilPrecomputationOptionName = "pathsprecomp";
            const std::string ExplorationSettings::nextStateHeuristicOptionName = "nextstate";
            
            ExplorationSettings::ExplorationSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                std::vector<std::string> types = { "local", "global" };
                    this->addOption(storm::settings::OptionBuilder(moduleName, precomputationTypeOptionName, true, "Sets the kind of precomputation used. Available are: { local, global }.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the type to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(types)).setDefaultValueString("global").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, numberOfExplorationStepsUntilPrecomputationOptionName, false, "Sets the number of exploration steps to perform until a precomputation is triggered.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of exploration steps to perform.").setDefaultValueUnsignedInteger(100000).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, numberOfSampledPathsUntilPrecomputationOptionName, false, "If set, a precomputation is perfomed periodically after the given number of paths has been sampled.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of paths to sample until a precomputation is triggered.").setDefaultValueUnsignedInteger(100000).build()).build());
                
                std::vector<std::string> nextStateHeuristics = { "probdiff", "prob" };
                this->addOption(storm::settings::OptionBuilder(moduleName, nextStateHeuristicOptionName, true, "Sets the next-state heuristic to use. Available are: { probdiff, prob } where 'prob' samples according to the probabilities in the system and 'probdiff' weights the probabilities with the differences between the current bounds.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the heuristic to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(nextStateHeuristics)).setDefaultValueString("probdiff").build()).build());
            }
            
            bool ExplorationSettings::isLocalPrecomputationSet() const {
                if (this->getOption(precomputationTypeOptionName).getArgumentByName("name").getValueAsString() == "local") {
                    return true;
                }
                return false;
            }
            
            bool ExplorationSettings::isGlobalPrecomputationSet() const {
                if (this->getOption(precomputationTypeOptionName).getArgumentByName("name").getValueAsString() == "global") {
                    return true;
                }
                return false;
            }
            
            ExplorationSettings::PrecomputationType ExplorationSettings::getPrecomputationType() const {
                std::string typeAsString = this->getOption(precomputationTypeOptionName).getArgumentByName("name").getValueAsString();
                if (typeAsString == "local") {
                    return ExplorationSettings::PrecomputationType::Local;
                } else if (typeAsString == "global") {
                    return ExplorationSettings::PrecomputationType::Global;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown precomputation type '" << typeAsString << "'.");
            }
            
            uint_fast64_t ExplorationSettings::getNumberOfExplorationStepsUntilPrecomputation() const {
                return this->getOption(numberOfExplorationStepsUntilPrecomputationOptionName).getArgumentByName("count").getValueAsUnsignedInteger();
            }
            
            bool ExplorationSettings::isNumberOfSampledPathsUntilPrecomputationSet() const {
                return this->getOption(numberOfSampledPathsUntilPrecomputationOptionName).getHasOptionBeenSet();
            }
            
            uint_fast64_t ExplorationSettings::getNumberOfSampledPathsUntilPrecomputation() const {
                return this->getOption(numberOfSampledPathsUntilPrecomputationOptionName).getArgumentByName("count").getValueAsUnsignedInteger();
            }
            
            ExplorationSettings::NextStateHeuristic ExplorationSettings::getNextStateHeuristic() const {
                std::string nextStateHeuristicAsString = this->getOption(nextStateHeuristicOptionName).getArgumentByName("name").getValueAsString();
                if (nextStateHeuristicAsString == "probdiff") {
                    return ExplorationSettings::NextStateHeuristic::DifferenceWeightedProbability;
                } else if (nextStateHeuristicAsString == "prob") {
                    return ExplorationSettings::NextStateHeuristic::Probability;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown next-state heuristic '" << nextStateHeuristicAsString << "'.");
            }
            
            bool ExplorationSettings::check() const {
                bool optionsSet = this->getOption(precomputationTypeOptionName).getHasOptionBeenSet() ||
                                    this->getOption(numberOfExplorationStepsUntilPrecomputationOptionName).getHasOptionBeenSet() ||
                                    this->getOption(numberOfSampledPathsUntilPrecomputationOptionName).getHasOptionBeenSet() ||
                                    this->getOption(nextStateHeuristicOptionName).getHasOptionBeenSet();
                STORM_LOG_WARN_COND(storm::settings::generalSettings().getEngine() == storm::settings::modules::GeneralSettings::Engine::Exploration || !optionsSet, "Exploration engine is not selected, so setting options for it has no effect.");
                return true;
            }
        } // namespace modules
    } // namespace settings
} // namespace storm