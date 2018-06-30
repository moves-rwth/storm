#include "JaniExportSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"

namespace storm {
    namespace settings {
        namespace modules {
            const std::string JaniExportSettings::moduleName = "exportJani";
            
            const std::string JaniExportSettings::janiFileOptionName = "jani-output";
            const std::string JaniExportSettings::janiFileOptionShortName = "output";
            const std::string JaniExportSettings::standardCompliantOptionName = "standard-compliant";
            const std::string JaniExportSettings::standardCompliantOptionShortName = "standard";
            const std::string JaniExportSettings::exportFlattenOptionName = "flatten";

            
            JaniExportSettings::JaniExportSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, janiFileOptionName, false, "Destination for the jani model.").setShortName(janiFileOptionShortName).addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, standardCompliantOptionName, false, "Export in standard compliant variant.").setShortName(standardCompliantOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, exportFlattenOptionName, false, "Export in standard compliant variant.").build());
            }
            
            bool JaniExportSettings::isJaniFileSet() const {
                return this->getOption(janiFileOptionName).getHasOptionBeenSet();
            }
            
            std::string JaniExportSettings::getJaniFilename() const {
                return this->getOption(janiFileOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool JaniExportSettings::isExportAsStandardJaniSet() const {
                return this->getOption(standardCompliantOptionName).getHasOptionBeenSet();
            }

            bool JaniExportSettings::isExportFlattenedSet() const {
                return this->getOption(exportFlattenOptionName).getHasOptionBeenSet();
            }
            
            void JaniExportSettings::finalize() {
                
            }
            
            bool JaniExportSettings::check() const {
                return true;
            }
        }
    }
}
