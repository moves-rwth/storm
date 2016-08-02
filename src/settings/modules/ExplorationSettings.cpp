#include "src/settings/modules/ExplorationSettings.h"
#include "src/settings/modules/CoreSettings.h"
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
            const std::string ExplorationSettings::precisionOptionName = "precision";
            const std::string ExplorationSettings::precisionOptionShortName = "eps";
            
            ExplorationSettings::ExplorationSettings() : ModuleSettings(moduleName) {
                std::vector<std::string> types = { "local", "global" };
                    this->addOption(storm::settings::OptionBuilder(moduleName, precomputationTypeOptionName, true, "Sets the kind of precomputation used. Available are: { local, global }.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the type to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(types)).setDefaultValueString("global").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, numberOfExplorationStepsUntilPrecomputationOptionName, true, "Sets the number of exploration steps to perform until a precomputation is triggered.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of exploration steps to perform.").setDefaultValueUnsignedInteger(100000).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, numberOfSampledPathsUntilPrecomputationOptionName, true, "If set, a precomputation is perfomed periodically after the given number of paths has been sampled.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of paths to sample until a precomputation is triggered.").setDefaultValueUnsignedInteger(100000).build()).build());
                
                std::vector<std::string> nextStateHeuristics = { "probdiffs", "prob", "unif" };
                this->addOption(storm::settings::OptionBuilder(moduleName, nextStateHeuristicOptionName, true, "Sets the next-state heuristic to use. Available are: { probdiffs, prob, unif } where 'prob' samples according to the probabilities in the system, 'probdiffs' takes into account probabilities and the differences between the current bounds and 'unif' samples uniformly.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the heuristic to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(nextStateHeuristics)).setDefaultValueString("probdiffs").build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, false, "The precision to achieve.").setShortName(precisionOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The value to use to determine convergence.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
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
                if (nextStateHeuristicAsString == "probdiffs") {
                    return ExplorationSettings::NextStateHeuristic::DifferenceProbabilitySum;
                } else if (nextStateHeuristicAsString == "prob") {
                    return ExplorationSettings::NextStateHeuristic::Probability;
                } else if (nextStateHeuristicAsString == "unif") {
                    return ExplorationSettings::NextStateHeuristic::Uniform;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown next-state heuristic '" << nextStateHeuristicAsString << "'.");
            }
            
            double ExplorationSettings::getPrecision() const {
                return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsDouble();
            }
            
            bool ExplorationSettings::check() const {
                bool optionsSet = this->getOption(precomputationTypeOptionName).getHasOptionBeenSet() ||
                                    this->getOption(numberOfExplorationStepsUntilPrecomputationOptionName).getHasOptionBeenSet() ||
                                    this->getOption(numberOfSampledPathsUntilPrecomputationOptionName).getHasOptionBeenSet() ||
                                    this->getOption(nextStateHeuristicOptionName).getHasOptionBeenSet();
                STORM_LOG_WARN_COND(storm::settings::getModule<storm::settings::modules::CoreSettings>().getEngine() == storm::settings::modules::CoreSettings::Engine::Exploration || !optionsSet, "Exploration engine is not selected, so setting options for it has no effect.");
                return true;
            }
        } // namespace modules
    } // namespace settings
} // namespace storm
