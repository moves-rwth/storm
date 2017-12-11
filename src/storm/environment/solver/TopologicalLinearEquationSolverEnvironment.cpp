#include "storm/environment/solver/TopologicalLinearEquationSolverEnvironment.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/TopologicalEquationSolverSettings.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    
    TopologicalLinearEquationSolverEnvironment::TopologicalLinearEquationSolverEnvironment() {
        auto const& topologicalSettings = storm::settings::getModule<storm::settings::modules::TopologicalEquationSolverSettings>();
        underlyingSolverType = topologicalSettings.getUnderlyingEquationSolverType();
        underlyingSolverTypeSetFromDefault = topologicalSettings.isUnderlyingEquationSolverTypeSetFromDefaultValue();
    }

    TopologicalLinearEquationSolverEnvironment::~TopologicalLinearEquationSolverEnvironment() {
        // Intentionally left empty
    }
    
    storm::solver::EquationSolverType const& TopologicalLinearEquationSolverEnvironment::getUnderlyingSolverType() const {
        return underlyingSolverType;
    }
    
    bool const& TopologicalLinearEquationSolverEnvironment::isUnderlyingSolverTypeSetFromDefault() const {
        return underlyingSolverTypeSetFromDefault;
    }
    
    void TopologicalLinearEquationSolverEnvironment::setUnderlyingSolverType(storm::solver::EquationSolverType value) {
        STORM_LOG_THROW(value != storm::solver::EquationSolverType::Topological, storm::exceptions::InvalidArgumentException, "Can not use the topological solver as underlying solver of the topological solver.");
        underlyingSolverTypeSetFromDefault = false;
        underlyingSolverType = value;
    }
    


}
