#include "storm/settings/modules/MarkovAutomatonSettings.h"

#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"

#include "storm/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string MarkovAutomatonSettings::moduleName = "ma";
            const std::string MarkovAutomatonSettings::techniqueOptionName = "technique";
            
            MarkovAutomatonSettings::MarkovAutomatonSettings() : ModuleSettings(moduleName) {
                std::vector<std::string> techniques = {"imca", "unifplus"};
                this->addOption(storm::settings::OptionBuilder(moduleName, techniqueOptionName, true, "The technique to use to solve bounded reachability queries.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the technique to use.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(techniques)).setDefaultValueString("imca").build()).build());
            }
            
            MarkovAutomatonSettings::BoundedReachabilityTechnique MarkovAutomatonSettings::getTechnique() const {
                std::string techniqueAsString = this->getOption(techniqueOptionName).getArgumentByName("name").getValueAsString();
                if (techniqueAsString == "imca") {
                    return MarkovAutomatonSettings::BoundedReachabilityTechnique::Imca;
                }
                return MarkovAutomatonSettings::BoundedReachabilityTechnique::UnifPlus;
            }
        
        }
    }
}
