#include "src/settings/modules/ParametricSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string ParametricSettings::moduleName = "parametric";
            const std::string ParametricSettings::entryStatesLastOptionName = "entrylast";
            const std::string ParametricSettings::maximalSccSizeOptionName = "sccsize";
            const std::string ParametricSettings::sortTrivialSccOptionName = "sorttrivial";
            const std::string ParametricSettings::encodeSmt2StrategyOptionName = "smt2strategy";
            const std::string ParametricSettings::exportSmt2DestinationPathOptionName = "smt2path";
            const std::string ParametricSettings::exportResultDestinationPathOptionName = "resultfile";
            
            ParametricSettings::ParametricSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, entryStatesLastOptionName, true, "Sets whether the entry states are eliminated last.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, sortTrivialSccOptionName, true, "Sets whether the trivial SCCs are to be eliminated in descending order with respect to their distances from the initial state.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, maximalSccSizeOptionName, true, "Sets the maximal size of the SCCs for which state elimination is applied.")
                                .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("maxsize", "The maximal size of an SCC on which state elimination is applied.").setDefaultValueUnsignedInteger(20).setIsOptional(true).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, encodeSmt2StrategyOptionName, true, "Set the smt2 encoding strategy.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("strategy", "the used strategy").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, exportSmt2DestinationPathOptionName, true, "A path to a file where the result should be saved.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("path", "the location.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, exportResultDestinationPathOptionName, true, "A path to a file where the smt2 encoding should be saved.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("path", "the location.").build()).build());
            }
            
            bool ParametricSettings::isEliminateEntryStatesLastSet() const {
                return this->getOption(entryStatesLastOptionName).getHasOptionBeenSet();
            }
            
            uint_fast64_t ParametricSettings::getMaximalSccSize() const {
                return this->getOption(maximalSccSizeOptionName).getArgumentByName("maxsize").getValueAsUnsignedInteger();
            }
            
            bool ParametricSettings::isSortTrivialSccsSet() const {
                return this->getOption(sortTrivialSccOptionName).getHasOptionBeenSet();
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
            
            
            
            
        } // namespace modules
    } // namespace settings
} // namespace storm