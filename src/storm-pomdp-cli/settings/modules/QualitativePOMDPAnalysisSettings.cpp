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
            const std::string exportSATCallsOption = "exportSATcallspath";
            const std::string lookaheadHorizonOption = "lookaheadhorizon";
            const std::string onlyDeterministicOption = "onlydeterministic";
            const std::string winningRegionOption = "winningregion";
            const std::string validationLevel = "validate";


            QualitativePOMDPAnalysisSettings::QualitativePOMDPAnalysisSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, exportSATCallsOption, false, "Export the SAT calls?.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("path", "The name of the file to which to write the model.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, lookaheadHorizonOption, false, "In reachability in combination with a discrete ranking function, a lookahead is necessary.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("bound", "The lookahead. Use 0 for the number of states.").setDefaultValueUnsignedInteger(0).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, onlyDeterministicOption, false, "Search only for deterministic schedulers").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, winningRegionOption, false, "Search for the winning region").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, validationLevel, false, "Validate algorithm during runtime (for debugging)").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("level", "how regular to apply this validation. Use 0 for never, 1 for the end, and >=2 within computation steps.").setDefaultValueUnsignedInteger(0).build()).build());
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

            bool QualitativePOMDPAnalysisSettings::isOnlyDeterministicSet() const {
                return this->getOption(onlyDeterministicOption).getHasOptionBeenSet();
            }

            bool QualitativePOMDPAnalysisSettings::isWinningRegionSet() const {
                return this->getOption(winningRegionOption).getHasOptionBeenSet();
            }

            bool QualitativePOMDPAnalysisSettings::validateIntermediateSteps() const {
                return this->getOption(validationLevel).getArgumentByName("level").getValueAsUnsignedInteger() >= 2;
            }
            bool QualitativePOMDPAnalysisSettings::validateFinalResult() const {
                return this->getOption(validationLevel).getArgumentByName("level").getValueAsUnsignedInteger() >= 1;
            }

            void QualitativePOMDPAnalysisSettings::finalize() {
            }

            bool QualitativePOMDPAnalysisSettings::check() const {
                return true;
            }

        } // namespace modules
    } // namespace settings
} // namespace storm
