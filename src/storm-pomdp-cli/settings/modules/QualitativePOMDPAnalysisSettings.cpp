#include "storm-pomdp-cli/settings/modules/QualitativePOMDPAnalysisSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"

#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace settings {
        namespace modules {

            const std::string QualitativePOMDPAnalysisSettings::moduleName = "pomdpQualitative";
            const std::string exportSATCallsOption = "exportSATCallsPath";
            const std::string lookaheadHorizonOption = "lookaheadHorizon";


            QualitativePOMDPAnalysisSettings::QualitativePOMDPAnalysisSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, exportSATCallsOption, false, "Export the SAT calls?.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("path", "The name of the file to which to write the model.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, lookaheadHorizonOption, false, "In reachability in combination with a discrete ranking function, a lookahead is necessary.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("bound", "The lookahead. Use 0 for the number of states.").setDefaultValueUnsignedInteger(0).build()).build());
            }

            uint64_t QualitativePOMDPAnalysisSettings::getLookahead() const {
                return this->getOption(lookaheadHorizonOption).getArgumentByName("bound").getValueAsUnsignedInteger();
            }
            bool QualitativePOMDPAnalysisSettings::isExportSATCallsSet() const {
                return this->getOption(exportSATCallsOption).getHasOptionBeenSet();
            }

            std::string QualitativePOMDPAnalysisSettings::getExportSATCallsPath() const {
                return this->getOption(exportSATCallsOption).getArgumentByName("path").getValueAsString();
            }



            void QualitativePOMDPAnalysisSettings::finalize() {
            }

            bool QualitativePOMDPAnalysisSettings::check() const {
                return true;
            }

        } // namespace modules
    } // namespace settings
} // namespace storm
