#include "storm-pars/settings/modules/ScpSettings.h"

#include "storm/settings/Argument.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/ArgumentValidators.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"

namespace storm::settings::modules {

const std::string ScpSettings::moduleName = "scp";
namespace {
const std::string trustRegionInitialOption = "trust-region-initial";
const std::string trustRegionFactorOption = "trust-region-factor";
const std::string trustRegionMaxOption = "trust-region-max";
const std::string trustRegionMinExcessOption = "trust-region-min-excess";
const std::string maxIterationsOption = "max-iterations";
const std::string sumViolationObjectiveOption = "sum-violation-objective";
}  // namespace

ScpSettings::ScpSettings() : ModuleSettings(moduleName) {
    this->addOption(storm::settings::OptionBuilder(moduleName, trustRegionInitialOption, false, "The initial trust region radius (delta_0)")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument(trustRegionInitialOption, "The initial radius")
                                         .setDefaultValueDouble(3.0)
                                         .addValidatorDouble(storm::settings::ArgumentValidatorFactory::createDoubleGreaterValidator(1.0))
                                         .build())
                        .build());
    this->addOption(storm::settings::OptionBuilder(moduleName, trustRegionFactorOption, false, "The trust region growth/shrink factor (gamma)")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument(trustRegionFactorOption, "The factor")
                                         .setDefaultValueDouble(1.5)
                                         .addValidatorDouble(storm::settings::ArgumentValidatorFactory::createDoubleGreaterValidator(1.0))
                                         .build())
                        .build());
    this->addOption(storm::settings::OptionBuilder(moduleName, trustRegionMaxOption, false, "The upper cap on the trust region radius")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument(trustRegionMaxOption, "The cap")
                                         .setDefaultValueDouble(10.0)
                                         .addValidatorDouble(storm::settings::ArgumentValidatorFactory::createDoubleGreaterValidator(1.0))
                                         .build())
                        .build());
    this->addOption(storm::settings::OptionBuilder(moduleName, trustRegionMinExcessOption, false,
                                                   "The termination threshold on (trust region radius - 1); the radius asymptotically approaches 1, never 0")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument(trustRegionMinExcessOption, "The threshold")
                                         .setDefaultValueDouble(1e-4)
                                         .addValidatorDouble(storm::settings::ArgumentValidatorFactory::createDoubleGreaterEqualValidator(0.0))
                                         .build())
                        .build());
    this->addOption(storm::settings::OptionBuilder(moduleName, maxIterationsOption, false, "The maximum number of SCP iterations")
                        .setIsAdvanced()
                        .addArgument(storm::settings::ArgumentBuilder::createIntegerArgument(maxIterationsOption, "The maximum number of iterations")
                                         .setDefaultValueInteger(1000)
                                         .addValidatorInteger(storm::settings::ArgumentValidatorFactory::createIntegerGreaterEqualValidator(1))
                                         .build())
                        .build());
    this->addOption(storm::settings::OptionBuilder(moduleName, sumViolationObjectiveOption, false,
                                                   "Minimize the sum of per-state constraint violations instead of the default maximum violation")
                        .setIsAdvanced()
                        .build());
}

double ScpSettings::getTrustRegionInitial() const {
    return this->getOption(trustRegionInitialOption).getArgumentByName(trustRegionInitialOption).getValueAsDouble();
}

double ScpSettings::getTrustRegionFactor() const {
    return this->getOption(trustRegionFactorOption).getArgumentByName(trustRegionFactorOption).getValueAsDouble();
}

double ScpSettings::getTrustRegionMax() const {
    return this->getOption(trustRegionMaxOption).getArgumentByName(trustRegionMaxOption).getValueAsDouble();
}

double ScpSettings::getTrustRegionMinExcess() const {
    return this->getOption(trustRegionMinExcessOption).getArgumentByName(trustRegionMinExcessOption).getValueAsDouble();
}

uint64_t ScpSettings::getMaxIterations() const {
    return this->getOption(maxIterationsOption).getArgumentByName(maxIterationsOption).getValueAsInteger();
}

bool ScpSettings::isSumViolationObjectiveSet() const {
    return this->getOption(sumViolationObjectiveOption).getHasOptionBeenSet();
}

}  // namespace storm::settings::modules
