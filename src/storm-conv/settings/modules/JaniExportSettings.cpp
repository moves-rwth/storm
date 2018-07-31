#include "JaniExportSettings.h"

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
            const std::string JaniExportSettings::moduleName = "exportJani";
            
            const std::string JaniExportSettings::standardCompliantOptionName = "standard-compliant";
            const std::string JaniExportSettings::standardCompliantOptionShortName = "standard";
            const std::string JaniExportSettings::exportFlattenOptionName = "flatten";
            const std::string JaniExportSettings::locationVariablesOptionName = "location-variables";
            const std::string JaniExportSettings::globalVariablesOptionName = "globalvars";

            
            JaniExportSettings::JaniExportSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, locationVariablesOptionName, true, "Variables to export in the location").addArgument(storm::settings::ArgumentBuilder::createStringArgument("variables", "A comma separated list with local variables.").setDefaultValueString("").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, standardCompliantOptionName, false, "Export in standard compliant variant.").setShortName(standardCompliantOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, exportFlattenOptionName, true, "Flattens the composition of Automata to obtain an equivalent model that contains exactly one automaton").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, globalVariablesOptionName, true, "If set, variables will preferably be made global, e.g., to guarantee the same variable order as in the input file.").build());
            }
            
            bool JaniExportSettings::isExportAsStandardJaniSet() const {
                return this->getOption(standardCompliantOptionName).getHasOptionBeenSet();
            }

            bool JaniExportSettings::isExportFlattenedSet() const {
                return this->getOption(exportFlattenOptionName).getHasOptionBeenSet();
            }

            bool JaniExportSettings::isLocationVariablesSet() const {
                return this->getOption(locationVariablesOptionName).getHasOptionBeenSet();
            }

            std::vector<std::pair<std::string, std::string>> JaniExportSettings::getLocationVariables() const {
                std::vector<std::pair<std::string, std::string>> result;
                if (isLocationVariablesSet()) {
                    std::string argument = this->getOption(locationVariablesOptionName).getArgumentByName("variables").getValueAsString();
                    std::vector<std::string> arguments;
                    boost::split( arguments, argument, boost::is_any_of(","));
                    for (auto const& pair : arguments) {
                        std::vector<std::string> keyvaluepair;
                        boost::split( keyvaluepair, pair, boost::is_any_of("."));
                        STORM_LOG_THROW(keyvaluepair.size() == 2, storm::exceptions::IllegalArgumentException, "Expected a name of the form AUTOMATON.VARIABLE (with no further dots) but got " << pair);
                        result.emplace_back(keyvaluepair.at(0), keyvaluepair.at(1));
                    }
                }
                return result;
            }

            bool JaniExportSettings::isGlobalVarsSet() const {
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
