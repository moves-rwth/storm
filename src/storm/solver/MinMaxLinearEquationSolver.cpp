#include "storm/solver/MinMaxLinearEquationSolver.h"

#include <cstdint>

#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/IterativeMinMaxLinearEquationSolver.h"
#include "storm/solver/TopologicalMinMaxLinearEquationSolver.h"
#include "storm/solver/LpMinMaxLinearEquationSolver.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/MinMaxEquationSolverSettings.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/IllegalFunctionCallException.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        MinMaxLinearEquationSolver<ValueType>::MinMaxLinearEquationSolver(OptimizationDirectionSetting direction) : direction(direction), trackScheduler(false), cachingEnabled(false), requirementsChecked(false) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        MinMaxLinearEquationSolver<ValueType>::~MinMaxLinearEquationSolver() {
            // Intentionally left empty.
        }

        template<typename ValueType>
        bool MinMaxLinearEquationSolver<ValueType>::solveEquations(OptimizationDirection d, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_WARN_COND_DEBUG(this->isRequirementsCheckedSet(), "The requirements of the solver have not been marked as checked. Please provide the appropriate check or mark the requirements as checked (if applicable).");
            return internalSolveEquations(d, x, b);
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::solveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_THROW(isSet(this->direction), storm::exceptions::IllegalFunctionCallException, "Optimization direction not set.");
            solveEquations(convert(this->direction), x, b);
        }

        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::repeatedMultiply(std::vector<ValueType>& x, std::vector<ValueType>* b, uint_fast64_t n) const {
            STORM_LOG_THROW(isSet(this->direction), storm::exceptions::IllegalFunctionCallException, "Optimization direction not set.");
            return repeatedMultiply(convert(this->direction), x, b, n);
        }

        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::setOptimizationDirection(OptimizationDirection d) {
            direction = convert(d);
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::unsetOptimizationDirection() {
            direction = OptimizationDirectionSetting::Unset;
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::setTrackScheduler(bool trackScheduler) {
            this->trackScheduler = trackScheduler;
            if (!this->trackScheduler) {
                schedulerChoices = boost::none;
            }
        }
        
        template<typename ValueType>
        bool MinMaxLinearEquationSolver<ValueType>::isTrackSchedulerSet() const {
            return this->trackScheduler;
        }
        
        template<typename ValueType>
        bool MinMaxLinearEquationSolver<ValueType>::hasScheduler() const {
            return static_cast<bool>(schedulerChoices);
        }
        
        template<typename ValueType>
        storm::storage::Scheduler<ValueType> MinMaxLinearEquationSolver<ValueType>::computeScheduler() const {
            STORM_LOG_THROW(hasScheduler(), storm::exceptions::IllegalFunctionCallException, "Cannot retrieve scheduler, because none was generated.");
            storm::storage::Scheduler<ValueType> result(schedulerChoices->size());
            uint_fast64_t state = 0;
            for (auto const& schedulerChoice : schedulerChoices.get()) {
                result.setChoice(schedulerChoice, state);
                ++state;
            }
            return result;
        }
        
        template<typename ValueType>
        std::vector<uint_fast64_t> const& MinMaxLinearEquationSolver<ValueType>::getSchedulerChoices() const {
            STORM_LOG_THROW(hasScheduler(), storm::exceptions::IllegalFunctionCallException, "Cannot retrieve scheduler choices, because they were not generated.");
            return schedulerChoices.get();
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::setCachingEnabled(bool value) {
            if(cachingEnabled && !value) {
                // caching will be turned off. Hence we clear the cache at this point
                clearCache();
            }
            cachingEnabled = value;
        }
        
        template<typename ValueType>
        bool MinMaxLinearEquationSolver<ValueType>::isCachingEnabled() const {
            return cachingEnabled;
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::clearCache() const {
            // Intentionally left empty.
        }
                
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::setInitialScheduler(std::vector<uint_fast64_t>&& choices) {
            initialScheduler = std::move(choices);
        }
        
        template<typename ValueType>
        bool MinMaxLinearEquationSolver<ValueType>::hasInitialScheduler() const {
            return static_cast<bool>(initialScheduler);
        }
        
        template<typename ValueType>
        std::vector<uint_fast64_t> const& MinMaxLinearEquationSolver<ValueType>::getInitialScheduler() const {
            return initialScheduler.get();
        }
        
        template<typename ValueType>
        MinMaxLinearEquationSolverRequirements MinMaxLinearEquationSolver<ValueType>::getRequirements(EquationSystemType const& equationSystemType, boost::optional<storm::solver::OptimizationDirection> const& direction) const {
            return MinMaxLinearEquationSolverRequirements();
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::setRequirementsChecked(bool value) {
            this->requirementsChecked = value;
        }
        
        template<typename ValueType>
        bool MinMaxLinearEquationSolver<ValueType>::isRequirementsCheckedSet() const {
            return requirementsChecked;
        }
        
        template<typename ValueType>
        MinMaxLinearEquationSolverFactory<ValueType>::MinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method, bool trackScheduler) : trackScheduler(trackScheduler) {
            setMinMaxMethod(method);
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolverFactory<ValueType>::setTrackScheduler(bool value) {
            this->trackScheduler = value;
        }
        
        template<typename ValueType>
        bool MinMaxLinearEquationSolverFactory<ValueType>::isTrackSchedulerSet() const {
            return trackScheduler;
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolverFactory<ValueType>::setRequirementsChecked(bool value) {
            this->requirementsChecked = value;
        }
        
        template<typename ValueType>
        bool MinMaxLinearEquationSolverFactory<ValueType>::isRequirementsCheckedSet() const {
            return this->requirementsChecked;
        }

        template<typename ValueType>
        void MinMaxLinearEquationSolverFactory<ValueType>::setMinMaxMethod(MinMaxMethodSelection const& newMethod) {
            if (newMethod == MinMaxMethodSelection::FROMSETTINGS) {
                bool wasSet = false;
                auto const& minMaxSettings = storm::settings::getModule<storm::settings::modules::MinMaxEquationSolverSettings>();
                if (std::is_same<ValueType, storm::RationalNumber>::value) {
                    if (minMaxSettings.isMinMaxEquationSolvingMethodSetFromDefaultValue() && minMaxSettings.getMinMaxEquationSolvingMethod() != MinMaxMethod::PolicyIteration) {
                        STORM_LOG_INFO("Selecting policy iteration as the solution method to guarantee exact results. If you want to override this, please explicitly specify a different method.");
                        this->setMinMaxMethod(MinMaxMethod::PolicyIteration);
                        wasSet = true;
                    }
                }
                
                if (!wasSet) {
                    setMinMaxMethod(minMaxSettings.getMinMaxEquationSolvingMethod());
                }
            } else {
                setMinMaxMethod(convert(newMethod));
            }
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolverFactory<ValueType>::setMinMaxMethod(MinMaxMethod const& newMethod) {
            STORM_LOG_WARN_COND(!(std::is_same<ValueType, storm::RationalNumber>::value) || newMethod == MinMaxMethod::PolicyIteration, "The selected solution method does not guarantee exact results. Consider using policy iteration.");
            method = newMethod;
        }
        
        template<typename ValueType>
        MinMaxMethod const& MinMaxLinearEquationSolverFactory<ValueType>::getMinMaxMethod() const {
            return method;
        }
        
        template<typename ValueType>
        MinMaxLinearEquationSolverRequirements MinMaxLinearEquationSolverFactory<ValueType>::getRequirements(EquationSystemType const& equationSystemType, boost::optional<storm::solver::OptimizationDirection> const& direction) const {
            // Create dummy solver and ask it for requirements.
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> solver = this->create();
            return solver->getRequirements(equationSystemType, direction);
        }
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> MinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> solver = this->create();
            solver->setMatrix(matrix);
            return solver;
        }
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> MinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType>&& matrix) const {
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> solver = this->create();
            solver->setMatrix(std::move(matrix));
            return solver;
        }
        
        template<typename ValueType>
        GeneralMinMaxLinearEquationSolverFactory<ValueType>::GeneralMinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method, bool trackScheduler) : MinMaxLinearEquationSolverFactory<ValueType>(method, trackScheduler) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> GeneralMinMaxLinearEquationSolverFactory<ValueType>::create() const {
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> result;
            auto method = this->getMinMaxMethod();
            if (method == MinMaxMethod::ValueIteration || method == MinMaxMethod::PolicyIteration || method == MinMaxMethod::Acyclic || method == MinMaxMethod::RationalSearch) {
                IterativeMinMaxLinearEquationSolverSettings<ValueType> iterativeSolverSettings;
                iterativeSolverSettings.setSolutionMethod(method);
                result = std::make_unique<IterativeMinMaxLinearEquationSolver<ValueType>>(std::make_unique<GeneralLinearEquationSolverFactory<ValueType>>(), iterativeSolverSettings);
            } else if (method == MinMaxMethod::Topological) {
                result = std::make_unique<TopologicalMinMaxLinearEquationSolver<ValueType>>();
            } else if (method == MinMaxMethod::LinearProgramming) {
                result = std::make_unique<LpMinMaxLinearEquationSolver<ValueType>>(std::make_unique<GeneralLinearEquationSolverFactory<ValueType>>(), std::make_unique<storm::utility::solver::LpSolverFactory<ValueType>>());
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique.");
            }
            result->setTrackScheduler(this->isTrackSchedulerSet());
            result->setRequirementsChecked(this->isRequirementsCheckedSet());
            return result;
        }

        template<>
        std::unique_ptr<MinMaxLinearEquationSolver<storm::RationalNumber>> GeneralMinMaxLinearEquationSolverFactory<storm::RationalNumber>::create() const {
            std::unique_ptr<MinMaxLinearEquationSolver<storm::RationalNumber>> result;
            auto method = this->getMinMaxMethod();
            if (method == MinMaxMethod::ValueIteration || method == MinMaxMethod::PolicyIteration || method == MinMaxMethod::Acyclic || method == MinMaxMethod::RationalSearch) {
                IterativeMinMaxLinearEquationSolverSettings<storm::RationalNumber> iterativeSolverSettings;
                iterativeSolverSettings.setSolutionMethod(method);
                result = std::make_unique<IterativeMinMaxLinearEquationSolver<storm::RationalNumber>>(std::make_unique<GeneralLinearEquationSolverFactory<storm::RationalNumber>>(), iterativeSolverSettings);
            } else if (method == MinMaxMethod::LinearProgramming) {
                result = std::make_unique<LpMinMaxLinearEquationSolver<storm::RationalNumber>>(std::make_unique<GeneralLinearEquationSolverFactory<storm::RationalNumber>>(), std::make_unique<storm::utility::solver::LpSolverFactory<storm::RationalNumber>>());
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique.");
            }
            result->setTrackScheduler(this->isTrackSchedulerSet());
            result->setRequirementsChecked(this->isRequirementsCheckedSet());
            return result;
        }

        template class MinMaxLinearEquationSolver<float>;
        template class MinMaxLinearEquationSolver<double>;
        
        template class MinMaxLinearEquationSolverFactory<double>;
        template class GeneralMinMaxLinearEquationSolverFactory<double>;

#ifdef STORM_HAVE_CARL
        template class MinMaxLinearEquationSolver<storm::RationalNumber>;
        template class MinMaxLinearEquationSolverFactory<storm::RationalNumber>;
        template class GeneralMinMaxLinearEquationSolverFactory<storm::RationalNumber>;
#endif
    }
}
