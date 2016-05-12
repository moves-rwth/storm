#include "src/solver/AbstractGameSolver.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/NativeEquationSolverSettings.h"

#include "src/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace solver {
        template <typename ValueType>
        AbstractGameSolver<ValueType>::AbstractGameSolver() {
            // Get the settings object to customize solving.
            storm::settings::modules::NativeEquationSolverSettings const& settings = storm::settings::nativeEquationSolverSettings();

            // Get appropriate settings.
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = settings.getPrecision();
            relative = settings.getConvergenceCriterion() == storm::settings::modules::NativeEquationSolverSettings::ConvergenceCriterion::Relative;
        }

        template <typename ValueType>
        AbstractGameSolver<ValueType>::AbstractGameSolver(double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : precision(precision), maximalNumberOfIterations(maximalNumberOfIterations), relative(relative) {
            // Intentionally left empty.
        }
        
        template <typename ValueType>
        void AbstractGameSolver<ValueType>::setTrackScheduler(bool trackScheduler){
            this->trackScheduler = trackScheduler;
        }
        
        template<typename ValueType>
        bool AbstractGameSolver<ValueType>::hasScheduler() const {
            return (static_cast<bool>(player1Scheduler) && static_cast<bool>(player2Scheduler));
        }
        
        template<typename ValueType>
        bool AbstractGameSolver<ValueType>::isTrackSchedulerSet() const {
            return this->trackScheduler;
        }
        
        template<typename ValueType>
        storm::storage::TotalScheduler const& AbstractGameSolver<ValueType>::getPlayer1Scheduler() const {
            STORM_LOG_THROW(player1Scheduler, storm::exceptions::InvalidSettingsException, "Cannot retrieve scheduler, because none was generated.");
            return *player1Scheduler.get();
        }
        
        template<typename ValueType>
        storm::storage::TotalScheduler const& AbstractGameSolver<ValueType>::getPlayer2Scheduler() const {
            STORM_LOG_THROW(player2Scheduler, storm::exceptions::InvalidSettingsException, "Cannot retrieve scheduler, because none was generated.");
            return *player2Scheduler.get();
        }

        
        template <typename ValueType>
        double AbstractGameSolver<ValueType>::getPrecision() const {
            return precision;
        }
        
        template <typename ValueType>
        bool AbstractGameSolver<ValueType>::getRelative() const {
            return relative;
        }
        
        template class AbstractGameSolver<double>;
        
    }
}
