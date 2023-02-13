#include "ToJuliaSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"

namespace storm {
namespace settings {
namespace modules {
    const std::string ToJuliaSettings::moduleName = "tojulia";
    const std::string discountOption = "discount";

    ToJuliaSettings::ToJuliaSettings() : ModuleSettings(moduleName) {
        this->addOption(
            storm::settings::OptionBuilder(moduleName, discountOption, false, "Sets the discount factor considered for the Julia export").addArgument(
                storm::settings::ArgumentBuilder::createDoubleArgument("discount","the discount factor").setDefaultValueDouble(0.95).addValidatorDouble(storm::settings::ArgumentValidatorFactory::createDoubleRangeValidatorIncluding(0, 1)).build()).build());
    }

    bool ToJuliaSettings::isDiscountFactorSet() const {
        return this->getOption(discountOption).getHasOptionBeenSet();
    }

    double ToJuliaSettings::getDiscountFactor() const {
        return this->getOption(discountOption).getArgumentByName("discount").getValueAsDouble();
    }

    void ToJuliaSettings::finalize() {
    }

    bool ToJuliaSettings::check() const {
        return true;
    }
}
}
}
