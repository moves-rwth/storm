#include "storm/settings/modules/AbstractionSettings.h"

#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"

namespace storm {
    namespace settings {
        namespace modules {
         
            const std::string AbstractionSettings::moduleName = "abstraction";
            const std::string AbstractionSettings::addAllGuardsOptionName = "allguards";
            const std::string AbstractionSettings::splitPredicatesOptionName = "split-preds";
            const std::string AbstractionSettings::splitInitialGuardsOptionName = "split-init-guards";
            const std::string AbstractionSettings::splitGuardsOptionName = "split-guards";
            const std::string AbstractionSettings::useInterpolationOptionName = "interpolation";
            const std::string AbstractionSettings::splitInterpolantsOptionName = "split-interpolants";
            const std::string AbstractionSettings::splitAllOptionName = "split-all";
            const std::string AbstractionSettings::precisionOptionName = "precision";

            AbstractionSettings::AbstractionSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, addAllGuardsOptionName, true, "Sets whether all guards are added as initial predicates.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, splitPredicatesOptionName, true, "Sets whether the predicates are split into atoms before they are added.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, splitInitialGuardsOptionName, true, "Sets whether the initial guards are split into atoms before they are added.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, splitGuardsOptionName, true, "Sets whether the guards are split into atoms before they are added.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, splitAllOptionName, true, "Sets whether all predicates are split into atoms before they are added.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, useInterpolationOptionName, true, "Sets whether interpolation is to be used to eliminate spurious pivot blocks.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, true, "The precision used for detecting convergence.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-03).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
            }
            
            bool AbstractionSettings::isAddAllGuardsSet() const {
                return this->getOption(addAllGuardsOptionName).getHasOptionBeenSet();
            }

            bool AbstractionSettings::isSplitPredicatesSet() const {
                return this->getOption(splitPredicatesOptionName).getHasOptionBeenSet();
            }
         
            bool AbstractionSettings::isSplitInitialGuardsSet() const {
                return this->getOption(splitInitialGuardsOptionName).getHasOptionBeenSet();
            }
            
            bool AbstractionSettings::isSplitGuardsSet() const {
                return this->getOption(splitGuardsOptionName).getHasOptionBeenSet();
            }
            
            bool AbstractionSettings::isSplitAllSet() const {
                return this->getOption(splitAllOptionName).getHasOptionBeenSet();
            }
            
            bool AbstractionSettings::isUseInterpolationSet() const {
                return this->getOption(useInterpolationOptionName).getHasOptionBeenSet();
            }
            
            double AbstractionSettings::getPrecision() const {
                return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsDouble();
            }
        }
    }
}
