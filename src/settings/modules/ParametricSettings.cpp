#include "src/settings/modules/ParametricSettings.h"

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string ParametricSettings::moduleName = "parametric";
            const std::string ParametricSettings::eliminationMethodOptionName = "method";
            const std::string ParametricSettings::eliminationOrderOptionName = "order";
            const std::string ParametricSettings::entryStatesLastOptionName = "entrylast";
            const std::string ParametricSettings::maximalSccSizeOptionName = "sccsize";
            const std::string ParametricSettings::encodeSmt2StrategyOptionName = "smt2strategy";
            const std::string ParametricSettings::exportSmt2DestinationPathOptionName = "smt2path";
            const std::string ParametricSettings::exportResultDestinationPathOptionName = "resultfile";
            
            ParametricSettings::ParametricSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                std::vector<std::string> orders = {"fw", "fwrev", "bw", "bwrev", "rand"};
                this->addOption(storm::settings::OptionBuilder(moduleName, eliminationOrderOptionName, true, "The order that is to be used for the elimination techniques. Available are {fw, fwrev, bw, bwrev, rand}.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the order in which states are chosen for elimination.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(orders)).setDefaultValueString("bw").build()).build());

                std::vector<std::string> methods = {"state", "hybrid"};
                this->addOption(storm::settings::OptionBuilder(moduleName, eliminationMethodOptionName, true, "The elimination technique to use. Available are {state, hybrid}.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the elimination technique to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(methods)).setDefaultValueString("hybrid").build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, entryStatesLastOptionName, true, "Sets whether the entry states are eliminated last.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, maximalSccSizeOptionName, true, "Sets the maximal size of the SCCs for which state elimination is applied.")
                                .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("maxsize", "The maximal size of an SCC on which state elimination is applied.").setDefaultValueUnsignedInteger(20).setIsOptional(true).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, encodeSmt2StrategyOptionName, true, "Set the smt2 encoding strategy.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("strategy", "the used strategy").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, exportSmt2DestinationPathOptionName, true, "A path to a file where the result should be saved.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("path", "the location.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, exportResultDestinationPathOptionName, true, "A path to a file where the smt2 encoding should be saved.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("path", "the location.").build()).build());
            }
            
            ParametricSettings::EliminationMethod ParametricSettings::getEliminationMethod() const {
                std::string eliminationMethodAsString = this->getOption(eliminationMethodOptionName).getArgumentByName("name").getValueAsString();
                if (eliminationMethodAsString == "state") {
                    return EliminationMethod::State;
                } else if (eliminationMethodAsString == "hybrid") {
                    return EliminationMethod::Hybrid;
                } else {
                    STORM_LOG_ASSERT(false, "Illegal elimination method selected.");
                }
            }
            
            ParametricSettings::EliminationOrder ParametricSettings::getEliminationOrder() const {
                std::string eliminationOrderAsString = this->getOption(eliminationOrderOptionName).getArgumentByName("name").getValueAsString();
                if (eliminationOrderAsString == "fw") {
                    return EliminationOrder::Forward;
                } else if (eliminationOrderAsString == "fwrev") {
                    return EliminationOrder::ForwardReversed;
                } else if (eliminationOrderAsString == "bw") {
                    return EliminationOrder::Backward;
                } else if (eliminationOrderAsString == "bwrev") {
                    return EliminationOrder::BackwardReversed;
                } else if (eliminationOrderAsString == "rand") {
                    return EliminationOrder::Random;
                } else {
                    STORM_LOG_ASSERT(false, "Illegal elimination order selected.");
                }
            }
            
            bool ParametricSettings::isEliminateEntryStatesLastSet() const {
                return this->getOption(entryStatesLastOptionName).getHasOptionBeenSet();
            }
            
            uint_fast64_t ParametricSettings::getMaximalSccSize() const {
                return this->getOption(maximalSccSizeOptionName).getArgumentByName("maxsize").getValueAsUnsignedInteger();
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