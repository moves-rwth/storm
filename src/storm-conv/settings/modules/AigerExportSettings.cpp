#include "AigerExportSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"

#include <boost/algorithm/string.hpp>

namespace storm {
    namespace settings {
        namespace modules {
            const std::string AigerExportSettings::moduleName = "exportAiger";
            
            AigerExportSettings::AigerExportSettings() : ModuleSettings(moduleName) {

            }
            
            void AigerExportSettings::finalize() {
                
            }
            
            bool AigerExportSettings::check() const {
                return true;
            }
        }
    }
}
