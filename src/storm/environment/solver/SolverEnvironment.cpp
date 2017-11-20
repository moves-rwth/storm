#include "storm/environment/solver/SolverEnvironment.h"

#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/environment/solver/EigenSolverEnvironment.h"
#include "storm/environment/solver/GmmxxSolverEnvironment.h"
#include "storm/environment/solver/NativeSolverEnvironment.h"
#include "storm/environment/solver/GameSolverEnvironment.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/InvalidEnvironmentException.h"


namespace storm {
    
    SolverEnvironment::SolverEnvironment() {
        forceSoundness = storm::settings::getModule<storm::settings::modules::GeneralSettings>().isSoundSet();
        linearEquationSolverType = storm::settings::getModule<storm::settings::modules::CoreSettings>().getEquationSolver();
        linearEquationSolverTypeSetFromDefault = storm::settings::getModule<storm::settings::modules::CoreSettings>().isEquationSolverSetFromDefaultValue();
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
    
    EigenSolverEnvironment& SolverEnvironment::eigen() {
        return eigenSolverEnvironment.get();
    }
    
    EigenSolverEnvironment const& SolverEnvironment::eigen() const {
        return eigenSolverEnvironment.get();
    }

    GmmxxSolverEnvironment& SolverEnvironment::gmmxx() {
        return gmmxxSolverEnvironment.get();
    }
    
    GmmxxSolverEnvironment const& SolverEnvironment::gmmxx() const {
        return gmmxxSolverEnvironment.get();
    }

    NativeSolverEnvironment& SolverEnvironment::native() {
        return nativeSolverEnvironment.get();
    }
    
    NativeSolverEnvironment const& SolverEnvironment::native() const {
        return nativeSolverEnvironment.get();
    }

    GameSolverEnvironment& SolverEnvironment::game() {
        return gameSolverEnvironment.get();
    }
    
    GameSolverEnvironment const& SolverEnvironment::game() const {
        return gameSolverEnvironment.get();
    }

    bool SolverEnvironment::isForceSoundness() const {
        return forceSoundness;
    }
    
    void SolverEnvironment::setForceSoundness(bool value) {
        SolverEnvironment::forceSoundness = value;
    }
    
    storm::solver::EquationSolverType const& SolverEnvironment::getLinearEquationSolverType() const {
        return linearEquationSolverType;
    }
    
    void SolverEnvironment::setLinearEquationSolverType(storm::solver::EquationSolverType const& value) {
        linearEquationSolverTypeSetFromDefault = false;
        linearEquationSolverType = value;
    }
    
    bool SolverEnvironment::isLinearEquationSolverTypeSetFromDefaultValue() const {
        return linearEquationSolverTypeSetFromDefault;
    }
    
    void SolverEnvironment::setLinearEquationSolverPrecision(storm::RationalNumber const& value) {
        STORM_LOG_ASSERT(getLinearEquationSolverType() == storm::solver::EquationSolverType::Native ||
                         getLinearEquationSolverType() == storm::solver::EquationSolverType::Gmmxx ||
                         getLinearEquationSolverType() == storm::solver::EquationSolverType::Eigen ||
                         getLinearEquationSolverType() == storm::solver::EquationSolverType::Elimination,
                        "The current solver type is not respected in this method.");
        native().setPrecision(value);
        gmmxx().setPrecision(value);
        eigen().setPrecision(value);
        // Elimination solver does not have a precision
    }
    
    void SolverEnvironment::setLinearEquationSolverRelativeTerminationCriterion(bool value) {
        STORM_LOG_ASSERT(getLinearEquationSolverType() == storm::solver::EquationSolverType::Native ||
                         getLinearEquationSolverType() == storm::solver::EquationSolverType::Gmmxx ||
                         getLinearEquationSolverType() == storm::solver::EquationSolverType::Eigen ||
                         getLinearEquationSolverType() == storm::solver::EquationSolverType::Elimination,
                        "The current solver type is not respected in this method.");
        native().setRelativeTerminationCriterion(value);
        // Elimination, gmm and eigen solver do not have an option for relative termination criterion
    }

    
}
    

