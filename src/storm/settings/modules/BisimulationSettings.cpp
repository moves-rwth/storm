#include "storm/settings/modules/BisimulationSettings.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"
#include "storm/settings/SettingsManager.h"

#include "storm/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string BisimulationSettings::moduleName = "bisimulation";
            const std::string BisimulationSettings::typeOptionName = "type";
            const std::string BisimulationSettings::representativeOptionName = "repr";
            const std::string BisimulationSettings::quotientFormatOptionName = "quot";
            
            BisimulationSettings::BisimulationSettings() : ModuleSettings(moduleName) {
                std::vector<std::string> types = { "strong", "weak" };
                this->addOption(storm::settings::OptionBuilder(moduleName, typeOptionName, true, "Sets the kind of bisimulation quotienting used.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the type to use.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(types)).setDefaultValueString("strong").build()).build());
                
                std::vector<std::string> quotTypes = { "sparse", "dd" };
                this->addOption(storm::settings::OptionBuilder(moduleName, quotientFormatOptionName, true, "Sets the format in which the quotient is extracted (only applies to DD-based bisimulation).").addArgument(storm::settings::ArgumentBuilder::createStringArgument("format", "The format of the quotient.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(quotTypes)).setDefaultValueString("dd").build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, representativeOptionName, false, "Sets whether to use representatives in the quotient rather than block numbers.").build());
            }
            
            bool BisimulationSettings::isStrongBisimulationSet() const {
                if (this->getOption(typeOptionName).getArgumentByName("name").getValueAsString() == "strong") {
                    return true;
                }
                return false;
            }
            
            bool BisimulationSettings::isWeakBisimulationSet() const {
                if (this->getOption(typeOptionName).getArgumentByName("name").getValueAsString() == "weak") {
                    return true;
                }
                return false;
            }
            
            BisimulationSettings::QuotientFormat BisimulationSettings::getQuotientFormat() const {
                std::string quotientFormatAsString = this->getOption(quotientFormatOptionName).getArgumentByName("format").getValueAsString();
                if (quotientFormatAsString == "sparse") {
                    return BisimulationSettings::QuotientFormat::Sparse;
                }
                return BisimulationSettings::QuotientFormat::Dd;
            }
            
            bool BisimulationSettings::isUseRepresentativesSet() const {
                return this->getOption(representativeOptionName).getHasOptionBeenSet();
            }
            
            bool BisimulationSettings::check() const {
                bool optionsSet = this->getOption(typeOptionName).getHasOptionBeenSet();
                STORM_LOG_WARN_COND(storm::settings::getModule<storm::settings::modules::GeneralSettings>().isBisimulationSet() || !optionsSet, "Bisimulation minimization is not selected, so setting options for bisimulation has no effect.");
                return true;
            }
        } // namespace modules
    } // namespace settings
} // namespace storm
