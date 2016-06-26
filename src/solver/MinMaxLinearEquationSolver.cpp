#include "src/solver/MinMaxLinearEquationSolver.h"

#include <cstdint>

#include "src/solver/LinearEquationSolver.h"
#include "src/solver/StandardMinMaxLinearEquationSolver.h"
#include "src/solver/TopologicalMinMaxLinearEquationSolver.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/MinMaxEquationSolverSettings.h"

#include "src/utility/macros.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/InvalidSettingsException.h"
#include "src/exceptions/IllegalFunctionCallException.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        MinMaxLinearEquationSolver<ValueType>::MinMaxLinearEquationSolver(OptimizationDirectionSetting direction) : direction(direction), trackScheduler(false) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        MinMaxLinearEquationSolver<ValueType>::~MinMaxLinearEquationSolver() {
            // Intentionally left empty.
        }

        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::solveEquationSystem(std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult, std::vector<ValueType>* newX) const {
            STORM_LOG_THROW(isSet(this->direction), storm::exceptions::IllegalFunctionCallException, "Optimization direction not set.");
            solveEquationSystem(convert(this->direction), x, b, multiplyResult, newX);
        }

        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::performMatrixVectorMultiplication( std::vector<ValueType>& x, std::vector<ValueType>* b, uint_fast64_t n, std::vector<ValueType>* multiplyResult) const {
            STORM_LOG_THROW(isSet(this->direction), storm::exceptions::IllegalFunctionCallException, "Optimization direction not set.");
            return performMatrixVectorMultiplication(convert(this->direction), x, b, n, multiplyResult);
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
                scheduler = boost::none;
            }
        }
        
        template<typename ValueType>
        bool MinMaxLinearEquationSolver<ValueType>::isTrackSchedulerSet() const {
            return this->trackScheduler;
        }
        
        template<typename ValueType>
        bool MinMaxLinearEquationSolver<ValueType>::hasScheduler() const {
            return static_cast<bool>(scheduler);
        }
        
        template<typename ValueType>
        storm::storage::TotalScheduler const& MinMaxLinearEquationSolver<ValueType>::getScheduler() const {
            STORM_LOG_THROW(scheduler, storm::exceptions::IllegalFunctionCallException, "Cannot retrieve scheduler, because none was generated.");
            return *scheduler.get();
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::storage::TotalScheduler> MinMaxLinearEquationSolver<ValueType>::getScheduler() {
            STORM_LOG_THROW(scheduler, storm::exceptions::IllegalFunctionCallException, "Cannot retrieve scheduler, because none was generated.");
            return std::move(scheduler.get());
        }
        
        template<typename ValueType>
        MinMaxLinearEquationSolverFactory<ValueType>::MinMaxLinearEquationSolverFactory(bool trackScheduler) : trackScheduler(trackScheduler) {
            // Intentionally left empty.
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
        GeneralMinMaxLinearEquationSolverFactory<ValueType>::GeneralMinMaxLinearEquationSolverFactory(bool trackScheduler) : MinMaxLinearEquationSolverFactory<ValueType>(trackScheduler) {
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
            auto method = storm::settings::getModule<storm::settings::modules::MinMaxEquationSolverSettings>().getMinMaxEquationSolvingMethod();
            if (method == MinMaxMethod::ValueIteration || method == MinMaxMethod::PolicyIteration) {
                result = std::make_unique<StandardMinMaxLinearEquationSolver<ValueType>>(std::forward<MatrixType>(matrix), std::make_unique<GeneralLinearEquationSolverFactory<ValueType>>());
            } else if (method == MinMaxMethod::Topological) {
                result = std::make_unique<TopologicalMinMaxLinearEquationSolver<ValueType>>(std::forward<MatrixType>(matrix));
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique.");
            }
            return result;
        }
        
        template class MinMaxLinearEquationSolver<float>;
        template class MinMaxLinearEquationSolver<double>;
        
        template class MinMaxLinearEquationSolverFactory<double>;
        template class GeneralMinMaxLinearEquationSolverFactory<double>;
        
    }
}
