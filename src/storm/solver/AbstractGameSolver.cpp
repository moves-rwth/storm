#include "storm/solver/AbstractGameSolver.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/NativeEquationSolverSettings.h"
#include "storm/adapters/CarlAdapter.h"

#include "storm/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace solver {
        template <typename ValueType>
        AbstractGameSolver<ValueType>::AbstractGameSolver() {
            // Get the settings object to customize solving.
            storm::settings::modules::NativeEquationSolverSettings const& settings = storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>();

            // Get appropriate settings.
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = storm::utility::convertNumber<ValueType>(settings.getPrecision());
            relative = settings.getConvergenceCriterion() == storm::settings::modules::NativeEquationSolverSettings::ConvergenceCriterion::Relative;
        }

        template <typename ValueType>
        AbstractGameSolver<ValueType>::AbstractGameSolver(ValueType precision, uint_fast64_t maximalNumberOfIterations, bool relative) : precision(precision), maximalNumberOfIterations(maximalNumberOfIterations), relative(relative) {
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
        ValueType AbstractGameSolver<ValueType>::getPrecision() const {
            return precision;
        }
        
        template <typename ValueType>
        bool AbstractGameSolver<ValueType>::getRelative() const {
            return relative;
        }
    
        template <typename ValueType>
        void AbstractGameSolver<ValueType>::setSchedulerHint(storm::storage::TotalScheduler&& player1Scheduler, storm::storage::TotalScheduler&& player2Scheduler) {
            player1SchedulerHint = player1Scheduler;
            player2SchedulerHint = player2Scheduler;
        }
    
        template <typename ValueType>
        bool AbstractGameSolver<ValueType>::hasSchedulerHints() const {
            return player1SchedulerHint.is_initialized() && player2SchedulerHint.is_initialized();
        }
    
        template <typename ValueType>
        void AbstractGameSolver<ValueType>::setLowerBound(ValueType const& value) {
            this->lowerBound = value;
        }
    
        template <typename ValueType>
        void AbstractGameSolver<ValueType>::setUpperBound(ValueType const& value) {
            this->upperBound = value;
        }
    
        template class AbstractGameSolver<double>;
        template class AbstractGameSolver<storm::RationalNumber>;
        
    }
}
