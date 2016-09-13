#include "JaniExportSettings.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/SettingMemento.h"
#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/Argument.h"

#include "src/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace settings {
        namespace modules {
            const std::string JaniExportSettings::moduleName = "exportJani";
            
            const std::string JaniExportSettings::janiFileOptionName = "jani-output";
            const std::string JaniExportSettings::janiFileOptionShortName = "output";

            
            JaniExportSettings::JaniExportSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, janiFileOptionName, false, "Destination for the jani model.").setShortName(janiFileOptionShortName).addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").build()).build());
            }
            
            bool JaniExportSettings::isJaniFileSet() const {
                return this->getOption(janiFileOptionName).getHasOptionBeenSet();
            }
            
            std::string JaniExportSettings::getJaniFilename() const {
                return this->getOption(janiFileOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            void JaniExportSettings::finalize() {
                
            }
            
            bool JaniExportSettings::check() const {
                return true;
            }
        }
    }
}