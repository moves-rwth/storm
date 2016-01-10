#include "src/settings/modules/SparseDtmcEliminationModelCheckerSettings.h"

#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/Argument.h"


namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string SparseDtmcEliminationModelCheckerSettings::moduleName = "sparseelim";
            const std::string SparseDtmcEliminationModelCheckerSettings::eliminationMethodOptionName = "method";
            const std::string SparseDtmcEliminationModelCheckerSettings::eliminationOrderOptionName = "order";
            const std::string SparseDtmcEliminationModelCheckerSettings::entryStatesLastOptionName = "entrylast";
            const std::string SparseDtmcEliminationModelCheckerSettings::maximalSccSizeOptionName = "sccsize";
            
            SparseDtmcEliminationModelCheckerSettings::SparseDtmcEliminationModelCheckerSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                std::vector<std::string> orders = {"fw", "fwrev", "bw", "bwrev", "rand", "spen", "dpen", "regex"};
                this->addOption(storm::settings::OptionBuilder(moduleName, eliminationOrderOptionName, true, "The order that is to be used for the elimination techniques. Available are {fw, fwrev, bw, bwrev, rand, spen, dpen}.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the order in which states are chosen for elimination.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(orders)).setDefaultValueString("fwrev").build()).build());
                
                std::vector<std::string> methods = {"state", "hybrid"};
                this->addOption(storm::settings::OptionBuilder(moduleName, eliminationMethodOptionName, true, "The elimination technique to use. Available are {state, hybrid}.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the elimination technique to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(methods)).setDefaultValueString("state").build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, entryStatesLastOptionName, true, "Sets whether the entry states are eliminated last.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, maximalSccSizeOptionName, true, "Sets the maximal size of the SCCs for which state elimination is applied.")
                                .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("maxsize", "The maximal size of an SCC on which state elimination is applied.").setDefaultValueUnsignedInteger(20).setIsOptional(true).build()).build());
            }
            
            SparseDtmcEliminationModelCheckerSettings::EliminationMethod SparseDtmcEliminationModelCheckerSettings::getEliminationMethod() const {
                std::string eliminationMethodAsString = this->getOption(eliminationMethodOptionName).getArgumentByName("name").getValueAsString();
                if (eliminationMethodAsString == "state") {
                    return EliminationMethod::State;
                } else if (eliminationMethodAsString == "hybrid") {
                    return EliminationMethod::Hybrid;
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Illegal elimination method selected.");
                }
            }
            
            SparseDtmcEliminationModelCheckerSettings::EliminationOrder SparseDtmcEliminationModelCheckerSettings::getEliminationOrder() const {
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
                } else if (eliminationOrderAsString == "spen") {
                    return EliminationOrder::StaticPenalty;
                } else if (eliminationOrderAsString == "dpen") {
                    return EliminationOrder::DynamicPenalty;
                } else if (eliminationOrderAsString == "regex") {
                    return EliminationOrder::RegularExpression;
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Illegal elimination order selected.");
                }
            }
            
            bool SparseDtmcEliminationModelCheckerSettings::isEliminateEntryStatesLastSet() const {
                return this->getOption(entryStatesLastOptionName).getHasOptionBeenSet();
            }
            
            uint_fast64_t SparseDtmcEliminationModelCheckerSettings::getMaximalSccSize() const {
                return this->getOption(maximalSccSizeOptionName).getArgumentByName("maxsize").getValueAsUnsignedInteger();
            }
        } // namespace modules
    } // namespace settings
} // namespace storm