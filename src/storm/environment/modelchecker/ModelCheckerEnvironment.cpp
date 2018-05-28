#include "storm/environment/modelchecker/ModelCheckerEnvironment.h"

#include "storm/environment/modelchecker/MultiObjectiveModelCheckerEnvironment.h"

#include "storm/settings/SettingsManager.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/InvalidEnvironmentException.h"
#include "storm/exceptions/UnexpectedException.h"


namespace storm {
    
    ModelCheckerEnvironment::ModelCheckerEnvironment() {
        // Intentionally left empty
    }
    
    ModelCheckerEnvironment::~ModelCheckerEnvironment() {
        // Intentionally left empty
    }
    
    MultiObjectiveModelCheckerEnvironment& ModelCheckerEnvironment::multi() {
        return multiObjectiveModelCheckerEnvironment.get();
    }
    
    MultiObjectiveModelCheckerEnvironment const& ModelCheckerEnvironment::multi() const {
        return multiObjectiveModelCheckerEnvironment.get();
    }
}
    

