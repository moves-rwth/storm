#include "storm/solver/MinMaxLinearEquationSolver.h"

#include <cstdint>

#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/IterativeMinMaxLinearEquationSolver.h"
#include "storm/solver/TopologicalMinMaxLinearEquationSolver.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/MinMaxEquationSolverSettings.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/IllegalFunctionCallException.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        MinMaxLinearEquationSolver<ValueType>::MinMaxLinearEquationSolver(OptimizationDirectionSetting direction) : direction(direction), trackScheduler(false), cachingEnabled(false) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        MinMaxLinearEquationSolver<ValueType>::~MinMaxLinearEquationSolver() {
            // Intentionally left empty.
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
        void MinMaxLinearEquationSolver<ValueType>::setLowerBound(ValueType const& value) {
            lowerBound = value;
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::setUpperBound(ValueType const& value) {
            upperBound = value;
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::setBounds(ValueType const& lower, ValueType const& upper) {
            setLowerBound(lower);
            setUpperBound(upper);
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::setSchedulerHint(std::vector<uint_fast64_t>&& choices) {
            choicesHint = std::move(choices);
        }
        
        template<typename ValueType>
        bool MinMaxLinearEquationSolver<ValueType>::hasSchedulerHint() const {
            return choicesHint.is_initialized();
        }
        
        template<typename ValueType>
        MinMaxLinearEquationSolverFactory<ValueType>::MinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method, bool trackScheduler) : trackScheduler(trackScheduler) {
            setMinMaxMethod(method);
        }
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> MinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType>&& matrix) const {
            return this->create(matrix);
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
        void MinMaxLinearEquationSolverFactory<ValueType>::setMinMaxMethod(MinMaxMethodSelection const& newMethod) {
            if (newMethod == MinMaxMethodSelection::FROMSETTINGS) {
                setMinMaxMethod(storm::settings::getModule<storm::settings::modules::MinMaxEquationSolverSettings>().getMinMaxEquationSolvingMethod());
            } else {
                setMinMaxMethod(convert(newMethod));
            }
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolverFactory<ValueType>::setMinMaxMethod(MinMaxMethod const& newMethod) {
            method = newMethod;
        }
        
        template<typename ValueType>
        MinMaxMethod const& MinMaxLinearEquationSolverFactory<ValueType>::getMinMaxMethod() const {
            return method;
        }

        template<typename ValueType>
        GeneralMinMaxLinearEquationSolverFactory<ValueType>::GeneralMinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method, bool trackScheduler) : MinMaxLinearEquationSolverFactory<ValueType>(method, trackScheduler) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> GeneralMinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
            return selectSolver(matrix);
        }
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> GeneralMinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType>&& matrix) const {
            return selectSolver(std::move(matrix));
        }
        
        template<typename ValueType>
        template<typename MatrixType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> GeneralMinMaxLinearEquationSolverFactory<ValueType>::selectSolver(MatrixType&& matrix) const {
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> result;
            auto method = this->getMinMaxMethod();
            if (method == MinMaxMethod::ValueIteration || method == MinMaxMethod::PolicyIteration || method == MinMaxMethod::Acyclic) {
                IterativeMinMaxLinearEquationSolverSettings<ValueType> iterativeSolverSettings;
                iterativeSolverSettings.setSolutionMethod(method);
                result = std::make_unique<IterativeMinMaxLinearEquationSolver<ValueType>>(std::forward<MatrixType>(matrix), std::make_unique<GeneralLinearEquationSolverFactory<ValueType>>(), iterativeSolverSettings);
            } else if (method == MinMaxMethod::Topological) {
                result = std::make_unique<TopologicalMinMaxLinearEquationSolver<ValueType>>(std::forward<MatrixType>(matrix));
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique.");
            }
            result->setTrackScheduler(this->isTrackSchedulerSet());
            return result;
        }

#ifdef STORM_HAVE_CARL
        template<>
        template<typename MatrixType>
        std::unique_ptr<MinMaxLinearEquationSolver<storm::RationalNumber>> GeneralMinMaxLinearEquationSolverFactory<storm::RationalNumber>::selectSolver(MatrixType&& matrix) const {
            std::unique_ptr<MinMaxLinearEquationSolver<storm::RationalNumber>> result;
            auto method = this->getMinMaxMethod();
            STORM_LOG_THROW(method == MinMaxMethod::ValueIteration || method == MinMaxMethod::PolicyIteration || method == MinMaxMethod::Acyclic, storm::exceptions::InvalidSettingsException, "For this data type only value iteration, policy iteration, and acyclic value iteration are available.");
            IterativeMinMaxLinearEquationSolverSettings<storm::RationalNumber> iterativeSolverSettings;
            iterativeSolverSettings.setSolutionMethod(method);
            return std::make_unique<IterativeMinMaxLinearEquationSolver<storm::RationalNumber>>(std::forward<MatrixType>(matrix), std::make_unique<GeneralLinearEquationSolverFactory<storm::RationalNumber>>(), iterativeSolverSettings);
        }
#endif
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
