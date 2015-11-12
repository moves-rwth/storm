#include "src/solver/AbstractGameSolver.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/NativeEquationSolverSettings.h"

namespace storm {
    namespace solver {
        AbstractGameSolver::AbstractGameSolver() {
            // Get the settings object to customize solving.
            storm::settings::modules::NativeEquationSolverSettings const& settings = storm::settings::nativeEquationSolverSettings();

            // Get appropriate settings.
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = settings.getPrecision();
            relative = settings.getConvergenceCriterion() == storm::settings::modules::NativeEquationSolverSettings::ConvergenceCriterion::Relative;
        }

        AbstractGameSolver::AbstractGameSolver(double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : precision(precision), maximalNumberOfIterations(maximalNumberOfIterations), relative(relative) {
            // Intentionally left empty.
        }
        
        void AbstractGameSolver::setPolicyTracking(bool setToTrue) {
            this->trackPolicies = setToTrue;
        }
        
        std::vector<storm::storage::sparse::state_type> AbstractGameSolver::getPlayer1Policy() const {
            assert(!this->player1Policy.empty());
            return player1Policy;
        }
        
        std::vector<storm::storage::sparse::state_type> AbstractGameSolver::getPlayer2Policy() const {
            assert(!this->player2Policy.empty());
            return player2Policy;
        }
    }
}