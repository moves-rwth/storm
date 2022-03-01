#include "storm/storage/dd/bisimulation/InternalSignatureRefiner.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/BisimulationSettings.h"

namespace storm {
namespace dd {
namespace bisimulation {

InternalSignatureRefinerOptions::InternalSignatureRefinerOptions() : InternalSignatureRefinerOptions(true) {
    // Intentionally left empty.
}

InternalSignatureRefinerOptions::InternalSignatureRefinerOptions(bool shiftStateVariables)
    : shiftStateVariables(shiftStateVariables), createChangedStates(true) {
    auto const& bisimulationSettings = storm::settings::getModule<storm::settings::modules::BisimulationSettings>();

    storm::settings::modules::BisimulationSettings::ReuseMode reuseMode = bisimulationSettings.getReuseMode();
    this->reuseBlockNumbers = reuseMode == storm::settings::modules::BisimulationSettings::ReuseMode::BlockNumbers;

    storm::settings::modules::BisimulationSettings::RefinementMode refinementMode = bisimulationSettings.getRefinementMode();
    this->createChangedStates = refinementMode == storm::settings::modules::BisimulationSettings::RefinementMode::ChangedStates;
}

ReuseWrapper::ReuseWrapper() : ReuseWrapper(false) {
    // Intentionally left empty.
}

ReuseWrapper::ReuseWrapper(bool value) : value(value) {
    // Intentionally left empty.
}

bool ReuseWrapper::isReused() const {
    return value;
}

void ReuseWrapper::setReused() {
    value = true;
}

}  // namespace bisimulation
}  // namespace dd
}  // namespace storm
