#include "MinMaxLinearEquationSolver.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"

#include "src/utility/macros.h"
#include "src/exceptions/NotImplementedException.h"
#include <cstdint>

namespace storm {
    namespace solver {
        template<typename ValueType>
        AbstractMinMaxLinearEquationSolver<ValueType>::AbstractMinMaxLinearEquationSolver(double precision, bool relativeError, uint_fast64_t maximalIterations, bool trackScheduler, MinMaxTechniqueSelection prefTech) : precision(precision), relative(relativeError), maximalNumberOfIterations(maximalIterations), trackScheduler(trackScheduler) {
            
            if(prefTech == MinMaxTechniqueSelection::FROMSETTINGS) {
                useValueIteration = (storm::settings::generalSettings().getMinMaxEquationSolvingTechnique() == storm::solver::MinMaxTechnique::ValueIteration);
            } else {
                useValueIteration = (prefTech == MinMaxTechniqueSelection::ValueIteration);
            }
        }
        
        template<typename ValueType>
        void AbstractMinMaxLinearEquationSolver<ValueType>::setTrackScheduler(bool trackScheduler) {
            this->trackScheduler = trackScheduler;
        }
        
        template<typename ValueType>
        bool AbstractMinMaxLinearEquationSolver<ValueType>::isTrackSchedulerSet() const {
            return this->trackScheduler;
        }
        
        template<typename ValueType>
        storm::storage::Scheduler const& AbstractMinMaxLinearEquationSolver<ValueType>::getScheduler() const {
            STORM_LOG_THROW(scheduler, storm::exceptions::InvalidSettingsException, "Cannot retrieve scheduler, because none was generated.");
            return *scheduler.get();
        }
        
        template<typename ValueType>
        void AbstractMinMaxLinearEquationSolver<ValueType>::setOptimizationDirection(OptimizationDirection d) {
            direction = convert(d);
        }
        
        template<typename ValueType>
        void AbstractMinMaxLinearEquationSolver<ValueType>::resetOptimizationDirection() {
            direction = OptimizationDirectionSetting::Unset;
        }
        
        template class AbstractMinMaxLinearEquationSolver<float>;
        template class AbstractMinMaxLinearEquationSolver<double>;

    }
}
