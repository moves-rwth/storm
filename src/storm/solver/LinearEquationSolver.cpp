#include <storm/exceptions/InvalidEnvironmentException.h>
#include "storm/solver/LinearEquationSolver.h"

#include "storm/solver/SolverSelectionOptions.h"

#include "storm/solver/GmmxxLinearEquationSolver.h"
#include "storm/solver/NativeLinearEquationSolver.h"
#include "storm/solver/EigenLinearEquationSolver.h"
#include "storm/solver/EliminationLinearEquationSolver.h"

#include "storm/utility/vector.h"

#include "storm/environment/solver/SolverEnvironment.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnmetRequirementException.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        LinearEquationSolver<ValueType>::LinearEquationSolver() : cachingEnabled(false) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool LinearEquationSolver<ValueType>::solveEquations(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            return this->internalSolveEquations(env, x, b);
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::repeatedMultiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n) const {
            if (!cachedRowVector) {
                cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
            }
            
            // We enable caching for this. But remember how the old setting was
            bool cachingWasEnabled = isCachingEnabled();
            setCachingEnabled(true);
            
            // Set up some temporary variables so that we can just swap pointers instead of copying the result after
            // each iteration.
            std::vector<ValueType>* currentX = &x;
            std::vector<ValueType>* nextX = cachedRowVector.get();
            
            // Now perform matrix-vector multiplication as long as we meet the bound.
            this->startMeasureProgress();
            for (uint_fast64_t i = 0; i < n; ++i) {
                this->multiply(*currentX, b, *nextX);
                std::swap(nextX, currentX);

                // Potentially show progress.
                this->showProgressIterative(i, n);
            }
            
            // If we performed an odd number of repetitions, we need to swap the contents of currentVector and x,
            // because the output is supposed to be stored in the input vector x.
            if (currentX == cachedRowVector.get()) {
                std::swap(x, *currentX);
            }
            
            // restore the old caching setting
            setCachingEnabled(cachingWasEnabled);
            
            if (!isCachingEnabled()) {
                clearCache();
            }
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::multiplyAndReduce(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint_fast64_t>* choices) const {
            if (!cachedRowVector) {
                cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
            }
            
            // We enable caching for this. But remember how the old setting was
            bool cachingWasEnabled = isCachingEnabled();
            setCachingEnabled(true);

            this->multiply(x, b, *cachedRowVector);
            vectorHelper.reduceVector(dir, *cachedRowVector, result, rowGroupIndices, choices);
            
            // restore the old caching setting
            setCachingEnabled(cachingWasEnabled);
            
            if (!isCachingEnabled()) {
                clearCache();
            }
        }
        
#ifdef STORM_HAVE_CARL
        template<>
        void LinearEquationSolver<storm::RationalFunction>::multiplyAndReduce(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<storm::RationalFunction>& x, std::vector<storm::RationalFunction> const* b, std::vector<storm::RationalFunction>& result, std::vector<uint_fast64_t>* choices ) const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Reducing rational function vector is not supported.");
        }
#endif
        
        template<typename ValueType>
        bool LinearEquationSolver<ValueType>::supportsGaussSeidelMultiplication() const {
            return false;
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::multiplyGaussSeidel(std::vector<ValueType>& x, std::vector<ValueType> const* b) const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This solver does not support the function 'multiplyGaussSeidel'.");
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::multiplyAndReduceGaussSeidel(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<uint_fast64_t>* choices) const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This solver does not support the function 'multiplyAndReduceGaussSeidel'.");
        }
        
        template<typename ValueType>
        LinearEquationSolverRequirements LinearEquationSolver<ValueType>::getRequirements(Environment const& env) const {
            return LinearEquationSolverRequirements();
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::setCachingEnabled(bool value) const {
            if(cachingEnabled && !value) {
                // caching will be turned off. Hence we clear the cache at this point
                clearCache();
            }
            cachingEnabled = value;
        }
        
        template<typename ValueType>
        bool LinearEquationSolver<ValueType>::isCachingEnabled() const {
            return cachingEnabled;
        }
        
        template<typename ValueType>
        void LinearEquationSolver<ValueType>::clearCache() const {
            cachedRowVector.reset();
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolver<ValueType>> LinearEquationSolverFactory<ValueType>::create(Environment const& env, storm::storage::SparseMatrix<ValueType> const& matrix, LinearEquationSolverTask const& task) const {
            std::unique_ptr<LinearEquationSolver<ValueType>> solver = this->create(env, task);
            solver->setMatrix(matrix);
            return solver;
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolver<ValueType>> LinearEquationSolverFactory<ValueType>::create(Environment const& env, storm::storage::SparseMatrix<ValueType>&& matrix, LinearEquationSolverTask const& task) const {
            std::unique_ptr<LinearEquationSolver<ValueType>> solver = this->create(env, task);
            solver->setMatrix(std::move(matrix));
            return solver;
        }
        
        template<typename ValueType>
        LinearEquationSolverProblemFormat LinearEquationSolverFactory<ValueType>::getEquationProblemFormat(Environment const& env) const {
            return this->create(env)->getEquationProblemFormat(env);
        }
        
        template<typename ValueType>
        LinearEquationSolverRequirements LinearEquationSolverFactory<ValueType>::getRequirements(Environment const& env) const {
            return this->create(env)->getRequirements(env);
        }
        
        template<typename ValueType>
        GeneralLinearEquationSolverFactory<ValueType>::GeneralLinearEquationSolverFactory() {
            // Intentionally left empty.
        }
        
        
        template<>
        std::unique_ptr<LinearEquationSolver<storm::RationalNumber>> GeneralLinearEquationSolverFactory<storm::RationalNumber>::create(Environment const& env, LinearEquationSolverTask const& task) const {
            EquationSolverType type = env.solver().getLinearEquationSolverType();
            
             // Adjust the solver type if it is not supported by this value type
            if (type == EquationSolverType::Gmmxx) {
                    type = EquationSolverType::Eigen;
                    STORM_LOG_INFO("Selecting '" + toString(type) + "' as the linear equation solver since the selected one does not support exact computations.");
            }
           
            switch (type) {
                case EquationSolverType::Native: return std::make_unique<NativeLinearEquationSolver<storm::RationalNumber>>();
                case EquationSolverType::Eigen: return std::make_unique<EigenLinearEquationSolver<storm::RationalNumber>>();
                case EquationSolverType::Elimination: return std::make_unique<EliminationLinearEquationSolver<storm::RationalNumber>>();
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidEnvironmentException, "Unknown solver type.");
                    return nullptr;
            }
        }
        
        template<>
        std::unique_ptr<LinearEquationSolver<storm::RationalFunction>> GeneralLinearEquationSolverFactory<storm::RationalFunction>::create(Environment const& env, LinearEquationSolverTask const& task) const {
            EquationSolverType type = env.solver().getLinearEquationSolverType();
            
             // Adjust the solver type if it is not supported by this value type
            if (type == EquationSolverType::Gmmxx && type == EquationSolverType::Native) {
                type = EquationSolverType::Eigen;
                STORM_LOG_INFO("Selecting '" + toString(type) + "' as the linear equation solver since the selected one does not support parametric computations.");
            }
           
            switch (type) {
                case EquationSolverType::Eigen: return std::make_unique<EigenLinearEquationSolver<storm::RationalFunction>>();
                case EquationSolverType::Elimination: return std::make_unique<EliminationLinearEquationSolver<storm::RationalFunction>>();
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidEnvironmentException, "Unknown solver type.");
                    return nullptr;
            }
    
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolver<ValueType>> GeneralLinearEquationSolverFactory<ValueType>::create(Environment const& env, LinearEquationSolverTask const& task) const {
            EquationSolverType type = env.solver().getLinearEquationSolverType();
            
            // Adjust the solver type if none was specified and we want sound computations
            if (env.solver().isForceSoundness() && task != LinearEquationSolverTask::Multiply, type != EquationSolverType::Native && type != EquationSolverType::Eigen && type != EquationSolverType::Elimination) {
                if (env.solver().isLinearEquationSolverTypeSetFromDefaultValue()) {
                    type = EquationSolverType::Native;
                    STORM_LOG_INFO("Selecting '" + toString(type) + "' as the linear equation solver to guarantee sound results. If you want to override this, please explicitly specify a different solver.");
                } else {
                    STORM_LOG_WARN("The selected solver does not yield sound results.");
                }
            }
            
            switch (type) {
                case EquationSolverType::Gmmxx: return std::make_unique<GmmxxLinearEquationSolver<ValueType>>();
                case EquationSolverType::Native: return std::make_unique<NativeLinearEquationSolver<ValueType>>();
                case EquationSolverType::Eigen: return std::make_unique<EigenLinearEquationSolver<ValueType>>();
                case EquationSolverType::Elimination: return std::make_unique<EliminationLinearEquationSolver<ValueType>>();
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidEnvironmentException, "Unknown solver type.");
                    return nullptr;
            }
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolverFactory<ValueType>> GeneralLinearEquationSolverFactory<ValueType>::clone() const {
            return std::make_unique<GeneralLinearEquationSolverFactory<ValueType>>(*this);
        }

        template class LinearEquationSolver<double>;
        template class LinearEquationSolverFactory<double>;
        template class GeneralLinearEquationSolverFactory<double>;

        template class LinearEquationSolver<storm::RationalNumber>;
        template class LinearEquationSolverFactory<storm::RationalNumber>;
        template class GeneralLinearEquationSolverFactory<storm::RationalNumber>;

        template class LinearEquationSolver<storm::RationalFunction>;
        template class LinearEquationSolverFactory<storm::RationalFunction>;
        template class GeneralLinearEquationSolverFactory<storm::RationalFunction>;

    }
}
