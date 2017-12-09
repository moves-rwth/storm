#include "storm/environment/solver/SolverEnvironment.h"

#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/environment/solver/EigenSolverEnvironment.h"
#include "storm/environment/solver/GmmxxSolverEnvironment.h"
#include "storm/environment/solver/NativeSolverEnvironment.h"
#include "storm/environment/solver/GameSolverEnvironment.h"
#include "storm/environment/solver/TopologicalLinearEquationSolverEnvironment.h"

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

    TopologicalLinearEquationSolverEnvironment& SolverEnvironment::topological() {
        return topologicalSolverEnvironment.get();
    }
    
    TopologicalLinearEquationSolverEnvironment const& SolverEnvironment::topological() const {
        return topologicalSolverEnvironment.get();
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
    
    boost::optional<storm::RationalNumber> SolverEnvironment::getPrecisionOfCurrentLinearEquationSolver() const {
        switch (getLinearEquationSolverType()) {
            case storm::solver::EquationSolverType::Gmmxx:
                return gmmxx().getPrecision();
            case storm::solver::EquationSolverType::Eigen:
                return eigen().getPrecision();
            case storm::solver::EquationSolverType::Native:
                return native().getPrecision();
            case storm::solver::EquationSolverType::Elimination:
                return boost::none;
            default:
                STORM_LOG_ASSERT(false, "The selected solver type is unknown.");
        }
    }
    
    void SolverEnvironment::setLinearEquationSolverPrecision(storm::RationalNumber const& value) {
        STORM_LOG_ASSERT(getLinearEquationSolverType() == storm::solver::EquationSolverType::Native ||
                         getLinearEquationSolverType() == storm::solver::EquationSolverType::Gmmxx ||
                         getLinearEquationSolverType() == storm::solver::EquationSolverType::Eigen ||
                         getLinearEquationSolverType() == storm::solver::EquationSolverType::Elimination ||
                         getLinearEquationSolverType() == storm::solver::EquationSolverType::Topological,
                        "The current solver type is not respected in this method.");
            native().setPrecision(value);
            gmmxx().setPrecision(value);
            eigen().setPrecision(value);
        // Elimination and Topological solver do not have a precision
    }
    
    void SolverEnvironment::setLinearEquationSolverRelativeTerminationCriterion(bool value) {
        STORM_LOG_ASSERT(getLinearEquationSolverType() == storm::solver::EquationSolverType::Native ||
                         getLinearEquationSolverType() == storm::solver::EquationSolverType::Gmmxx ||
                         getLinearEquationSolverType() == storm::solver::EquationSolverType::Eigen ||
                         getLinearEquationSolverType() == storm::solver::EquationSolverType::Elimination ||
                         getLinearEquationSolverType() == storm::solver::EquationSolverType::Topological,
                        "The current solver type is not respected in this method.");
        native().setRelativeTerminationCriterion(value);
        // Elimination, gmm, eigen, and topological solver do not have an option for relative termination criterion
    }

    
}
    

