#include "storm-pomdp-cli/settings/modules/POMDPSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"

#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/IllegalArgumentValueException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string POMDPSettings::moduleName = "pomdp";
            const std::string exportAsParametricModelOption = "parametric-drn";


            POMDPSettings::POMDPSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, exportAsParametricModelOption, false, "Export the parametric file.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the file to which to write the model.").build()).build());

            }

            bool POMDPSettings::isExportToParametricSet() const {
                return this->getOption(exportAsParametricModelOption).getHasOptionBeenSet();
            }

            std::string POMDPSettings::getExportToParametricFilename() const {
                return this->getOption(exportAsParametricModelOption).getArgumentByName("filename").getValueAsString();
            }

            void POMDPSettings::finalize() {
            }

            bool POMDPSettings::check() const {
                // Ensure that at most one of min or max is set
                return true;
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm
