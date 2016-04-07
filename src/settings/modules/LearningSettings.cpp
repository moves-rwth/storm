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
            const std::string LearningSettings::ecDetectionTypeOptionName = "ecdetection";
            
            LearningSettings::LearningSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                std::vector<std::string> types = { "local", "global" };
                this->addOption(storm::settings::OptionBuilder(moduleName, ecDetectionTypeOptionName, true, "Sets the kind of EC-detection used. Available are: { local, global }.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the type to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(types)).setDefaultValueString("local").build()).build());
            }
            
            bool LearningSettings::isLocalECDetectionSet() const {
                if (this->getOption(ecDetectionTypeOptionName).getArgumentByName("name").getValueAsString() == "local") {
                    return true;
                }
                return false;
            }
            
            bool LearningSettings::isGlobalECDetectionSet() const {
                if (this->getOption(ecDetectionTypeOptionName).getArgumentByName("name").getValueAsString() == "global") {
                    return true;
                }
                return false;
            }
            
            LearningSettings::ECDetectionType LearningSettings::getECDetectionType() const {
                std::string typeAsString = this->getOption(ecDetectionTypeOptionName).getArgumentByName("name").getValueAsString();
                if (typeAsString == "local") {
                    return LearningSettings::ECDetectionType::Local;
                } else if (typeAsString == "global") {
                    return LearningSettings::ECDetectionType::Global;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown EC-detection type '" << typeAsString << "'.");
            }
            
            bool LearningSettings::check() const {
                bool optionsSet = this->getOption(ecDetectionTypeOptionName).getHasOptionBeenSet();
                STORM_LOG_WARN_COND(storm::settings::generalSettings().getEngine() == storm::settings::modules::GeneralSettings::Engine::Learning || !optionsSet, "Bisimulation minimization is not selected, so setting options for bisimulation has no effect.");
                return true;
            }
        } // namespace modules
    } // namespace settings
} // namespace storm