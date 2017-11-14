#include "storm/environment/solver/SolverEnvironment.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"

namespace storm {
    
    SolverEnvironment::SolverEnvironment() {
        forceSoundness = storm::settings::getModule<storm::settings::modules::GeneralSettings>().isSoundSet();
    }
    
    SolverEnvironment::~SolverEnvironment() {
        // Intentionally left empty
    }
    
    MinMaxSolverEnvironment& SolverEnvironment::minMax() {
        return minMaxSolverEnvironment.get();
    }
    
    MinMaxSolverEnvironment const& SolverEnvironment::minMax() const {
        return minMaxSolverEnvironment.get();
    }

    bool SolverEnvironment::isForceSoundness() const {
        return forceSoundness;
    }
    
    void SolverEnvironment::setForceSoundness(bool value) {
        SolverEnvironment::forceSoundness = value;
    }
}
    

