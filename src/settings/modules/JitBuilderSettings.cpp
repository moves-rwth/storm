#include "src/settings/modules/JitBuilderSettings.h"

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
            const std::string JitBuilderSettings::moduleName = "jitbuilder";
            
            const std::string JitBuilderSettings::noJitOptionName = "nojit";
            const std::string JitBuilderSettings::doctorOptionName = "doctor";
            
            JitBuilderSettings::JitBuilderSettings() : ModuleSettings(moduleName) {
//                this->addOption(storm::settings::OptionBuilder(moduleName, noJitOptionName, false, "Don't use the jit-based explicit model builder.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, doctorOptionName, false, "Show information on why the jit-based explicit model builder is not usable on the current system.").build());
            }
            
//            bool JitBuilderSettings::isNoJitSet() const {
//                return this->getOption(noJitOptionName).getHasOptionBeenSet();
//            }

            bool JitBuilderSettings::isDoctorSet() const {
                return this->getOption(doctorOptionName).getHasOptionBeenSet();
            }
                        
            void JitBuilderSettings::finalize() {
                // Intentionally left empty.
            }
            
            bool JitBuilderSettings::check() const {
                return true;
            }
        }
    }
}
