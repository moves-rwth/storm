#include "src/settings/modules/PGCLSettings.h"

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
            const std::string PGCLSettings::moduleName = "pgcl";
            
            const std::string PGCLSettings::pgclFileOptionName = "pgclfile";
            const std::string PGCLSettings::pgclFileOptionShortName = "pgcl";
            
            PGCLSettings::PGCLSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, pgclFileOptionName, false, "Parses the pgcl program.").setShortName(pgclFileOptionShortName).addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "path to file").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
            }
            
            bool PGCLSettings::isPgclFileSet() const {
                return this->getOption(pgclFileOptionName).getHasOptionBeenSet();
            }
            
            std::string PGCLSettings::getPgclFilename() const {
                return this->getOption(pgclFileOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            void PGCLSettings::finalize() {
                
            }
            
            bool PGCLSettings::check() const {
                return true;
            }
        }
    }
}