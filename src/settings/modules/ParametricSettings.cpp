#include "src/settings/modules/ParametricSettings.h"

#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/Argument.h"


namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string ParametricSettings::moduleName = "parametric";
            const std::string ParametricSettings::encodeSmt2StrategyOptionName = "smt2strategy";
            const std::string ParametricSettings::exportSmt2DestinationPathOptionName = "smt2path";
            const std::string ParametricSettings::exportResultDestinationPathOptionName = "resultfile";
            const std::string ParametricSettings::derivativesOptionName = "derivatives";
            
            ParametricSettings::ParametricSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, encodeSmt2StrategyOptionName, true, "Set the smt2 encoding strategy.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("strategy", "the used strategy").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, exportSmt2DestinationPathOptionName, true, "A path to a file where the result should be saved.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("path", "the location.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, exportResultDestinationPathOptionName, true, "A path to a file where the smt2 encoding should be saved.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("path", "the location.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, derivativesOptionName, true, "Sets whether to generate the derivatives of the resulting rational function.").build());
            }
            
            bool ParametricSettings::exportResultToFile() const {
                return this->getOption(exportResultDestinationPathOptionName).getHasOptionBeenSet();
            }
            
            std::string ParametricSettings::exportResultPath() const {
                return this->getOption(exportResultDestinationPathOptionName).getArgumentByName("path").getValueAsString();
            }
            
            bool ParametricSettings::exportToSmt2File() const {
                return this->getOption(exportSmt2DestinationPathOptionName).getHasOptionBeenSet();
            }
            
            std::string ParametricSettings::exportSmt2Path() const {
                return this->getOption(exportSmt2DestinationPathOptionName).getArgumentByName("path").getValueAsString();
            }
            
            ParametricSettings::Smt2EncodingStrategy ParametricSettings::smt2EncodingStrategy() const {
                std::string strategy = this->getOption(encodeSmt2StrategyOptionName).getArgumentByName("strategy").getValueAsString();
                
                if(strategy == "fts") {
                    return Smt2EncodingStrategy::FULL_TRANSITION_SYSTEM;
                } else if(strategy == "rf") {
                    return Smt2EncodingStrategy::RATIONAL_FUNCTION;
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown smt2encoding strategy '" << strategy << "'.");
                }
            }
            
            bool ParametricSettings::isDerivativesSet() const {
                return this->getOption(derivativesOptionName).getHasOptionBeenSet();
            }

        } // namespace modules
    } // namespace settings
} // namespace storm