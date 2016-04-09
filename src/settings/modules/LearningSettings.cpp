#include "src/settings/modules/LearningSettings.h"
#include "src/settings/modules/GeneralSettings.h"
#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/Argument.h"
#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string LearningSettings::moduleName = "learning";
            const std::string LearningSettings::precomputationTypeOptionName = "precomp";
            const std::string LearningSettings::numberOfExplorationStepsUntilPrecomputationOptionName = "stepsprecomp";
            
            LearningSettings::LearningSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                std::vector<std::string> types = { "local", "global" };
                this->addOption(storm::settings::OptionBuilder(moduleName, precomputationTypeOptionName, true, "Sets the kind of precomputation used. Available are: { local, global }.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the type to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(types)).setDefaultValueString("local").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, numberOfExplorationStepsUntilPrecomputationOptionName, false, "Sets the number of exploration steps until a precomputation is triggered.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of exploration steps to perform.").setDefaultValueUnsignedInteger(100000).build()).build());
            }
            
            bool LearningSettings::isLocalPrecomputationSet() const {
                if (this->getOption(precomputationTypeOptionName).getArgumentByName("name").getValueAsString() == "local") {
                    return true;
                }
                return false;
            }
            
            bool LearningSettings::isGlobalPrecomputationSet() const {
                if (this->getOption(precomputationTypeOptionName).getArgumentByName("name").getValueAsString() == "global") {
                    return true;
                }
                return false;
            }
            
            LearningSettings::PrecomputationType LearningSettings::getPrecomputationType() const {
                std::string typeAsString = this->getOption(precomputationTypeOptionName).getArgumentByName("name").getValueAsString();
                if (typeAsString == "local") {
                    return LearningSettings::PrecomputationType::Local;
                } else if (typeAsString == "global") {
                    return LearningSettings::PrecomputationType::Global;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown EC-detection type '" << typeAsString << "'.");
            }
            
            uint_fast64_t LearningSettings::getNumberOfExplorationStepsUntilPrecomputation() const {
                return this->getOption(numberOfExplorationStepsUntilPrecomputationOptionName).getArgumentByName("count").getValueAsUnsignedInteger();
            }
            
            bool LearningSettings::check() const {
                bool optionsSet = this->getOption(precomputationTypeOptionName).getHasOptionBeenSet() || this->getOption(numberOfExplorationStepsUntilPrecomputationOptionName).getHasOptionBeenSet();
                STORM_LOG_WARN_COND(storm::settings::generalSettings().getEngine() == storm::settings::modules::GeneralSettings::Engine::Learning || !optionsSet, "Learning engine is not selected, so setting options for it has no effect.");
                return true;
            }
        } // namespace modules
    } // namespace settings
} // namespace storm