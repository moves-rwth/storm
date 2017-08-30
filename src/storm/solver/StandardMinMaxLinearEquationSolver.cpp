#include "storm/solver/StandardMinMaxLinearEquationSolver.h"

#include "storm/solver/IterativeMinMaxLinearEquationSolver.h"
#include "storm/solver/GmmxxLinearEquationSolver.h"
#include "storm/solver/EigenLinearEquationSolver.h"
#include "storm/solver/NativeLinearEquationSolver.h"
#include "storm/solver/EliminationLinearEquationSolver.h"

#include "storm/utility/vector.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/NotImplementedException.h"
namespace storm {
    namespace solver {
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolver<ValueType>::StandardMinMaxLinearEquationSolver() : A(nullptr) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolver<ValueType>::StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory) : linearEquationSolverFactory(std::move(linearEquationSolverFactory)), localA(nullptr), A(&A) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolver<ValueType>::StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory) : linearEquationSolverFactory(std::move(linearEquationSolverFactory)), localA(std::make_unique<storm::storage::SparseMatrix<ValueType>>(std::move(A))), A(localA.get()) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        void StandardMinMaxLinearEquationSolver<ValueType>::repeatedMultiply(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n) const {
            if (!linEqSolverA) {
                linEqSolverA = linearEquationSolverFactory->create(*A);
                linEqSolverA->setCachingEnabled(true);
            }
            
            if (!auxiliaryRowVector) {
                auxiliaryRowVector = std::make_unique<std::vector<ValueType>>(A->getRowCount());
            }
            std::vector<ValueType>& multiplyResult = *auxiliaryRowVector;
            
            for (uint64_t i = 0; i < n; ++i) {
                linEqSolverA->multiply(x, b, multiplyResult);
                
                // Reduce the vector x' by applying min/max for all non-deterministic choices as given by the topmost
                // element of the min/max operator stack.
                storm::utility::vector::reduceVectorMinOrMax(dir, multiplyResult, x, this->A->getRowGroupIndices());
            }
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }
        }
        
        template<typename ValueType>
        void StandardMinMaxLinearEquationSolver<ValueType>::clearCache() const {
            linEqSolverA.reset();
            auxiliaryRowVector.reset();
            MinMaxLinearEquationSolver<ValueType>::clearCache();
        }

        template<typename ValueType>
        StandardMinMaxLinearEquationSolverFactory<ValueType>::StandardMinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method, bool trackScheduler) : MinMaxLinearEquationSolverFactory<ValueType>(method, trackScheduler), linearEquationSolverFactory(std::make_unique<GeneralLinearEquationSolverFactory<ValueType>>()) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverFactory<ValueType>::StandardMinMaxLinearEquationSolverFactory(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, MinMaxMethodSelection const& method, bool trackScheduler) : MinMaxLinearEquationSolverFactory<ValueType>(method, trackScheduler), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverFactory<ValueType>::StandardMinMaxLinearEquationSolverFactory(EquationSolverType const& solverType, MinMaxMethodSelection const& method, bool trackScheduler) : MinMaxLinearEquationSolverFactory<ValueType>(method, trackScheduler) {
            switch (solverType) {
                case EquationSolverType::Gmmxx: linearEquationSolverFactory = std::make_unique<GmmxxLinearEquationSolverFactory<ValueType>>(); break;
                case EquationSolverType::Eigen: linearEquationSolverFactory = std::make_unique<EigenLinearEquationSolverFactory<ValueType>>(); break;
                case EquationSolverType::Native: linearEquationSolverFactory = std::make_unique<NativeLinearEquationSolverFactory<ValueType>>(); break;
                case EquationSolverType::Elimination: linearEquationSolverFactory = std::make_unique<EliminationLinearEquationSolverFactory<ValueType>>(); break;
            }
        }
        
#ifdef STORM_HAVE_CARL
        template<>
        StandardMinMaxLinearEquationSolverFactory<storm::RationalNumber>::StandardMinMaxLinearEquationSolverFactory(EquationSolverType const& solverType, MinMaxMethodSelection const& method, bool trackScheduler) : MinMaxLinearEquationSolverFactory<storm::RationalNumber>(method, trackScheduler) {
            switch (solverType) {
                case  EquationSolverType::Eigen: linearEquationSolverFactory = std::make_unique<EigenLinearEquationSolverFactory<storm::RationalNumber>>(); break;
                case  EquationSolverType::Elimination: linearEquationSolverFactory = std::make_unique<EliminationLinearEquationSolverFactory<storm::RationalNumber>>(); break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Cannot create the requested solver for this data type.");
            }
        }
#endif
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> StandardMinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
            STORM_LOG_ASSERT(linearEquationSolverFactory, "Linear equation solver factory not initialized.");
            
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> result;
            auto method = this->getMinMaxMethod();
            if (method == MinMaxMethod::ValueIteration || method == MinMaxMethod::PolicyIteration || method == MinMaxMethod::Acyclic) {
                IterativeMinMaxLinearEquationSolverSettings<ValueType> iterativeSolverSettings;
                iterativeSolverSettings.setSolutionMethod(method);
                result = std::make_unique<IterativeMinMaxLinearEquationSolver<ValueType>>(matrix, linearEquationSolverFactory->clone(), iterativeSolverSettings);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique.");
            }
            result->setTrackScheduler(this->isTrackSchedulerSet());
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> StandardMinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType>&& matrix) const {
           STORM_LOG_ASSERT(linearEquationSolverFactory, "Linear equation solver factory not initialized.");
            
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> result;
            auto method = this->getMinMaxMethod();
            if (method == MinMaxMethod::ValueIteration || method == MinMaxMethod::PolicyIteration || method == MinMaxMethod::Acyclic) {
                IterativeMinMaxLinearEquationSolverSettings<ValueType> iterativeSolverSettings;
                iterativeSolverSettings.setSolutionMethod(method);
                result = std::make_unique<IterativeMinMaxLinearEquationSolver<ValueType>>(std::move(matrix), linearEquationSolverFactory->clone(), iterativeSolverSettings);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique.");
            }
            result->setTrackScheduler(this->isTrackSchedulerSet());
            return result;
        }
        
        template<typename ValueType>
        GmmxxMinMaxLinearEquationSolverFactory<ValueType>::GmmxxMinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method, bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(EquationSolverType::Gmmxx, method, trackScheduler) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        EigenMinMaxLinearEquationSolverFactory<ValueType>::EigenMinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method, bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(EquationSolverType::Eigen, method, trackScheduler) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        NativeMinMaxLinearEquationSolverFactory<ValueType>::NativeMinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method, bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(EquationSolverType::Native, method, trackScheduler) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        EliminationMinMaxLinearEquationSolverFactory<ValueType>::EliminationMinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method, bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(EquationSolverType::Elimination, method, trackScheduler) {
            // Intentionally left empty.
        }
        
        template class StandardMinMaxLinearEquationSolver<double>;
        template class StandardMinMaxLinearEquationSolverFactory<double>;
        template class GmmxxMinMaxLinearEquationSolverFactory<double>;
        template class EigenMinMaxLinearEquationSolverFactory<double>;
        template class NativeMinMaxLinearEquationSolverFactory<double>;
        template class EliminationMinMaxLinearEquationSolverFactory<double>;
        
#ifdef STORM_HAVE_CARL
        template class StandardMinMaxLinearEquationSolver<storm::RationalNumber>;
        template class StandardMinMaxLinearEquationSolverFactory<storm::RationalNumber>;
        template class EigenMinMaxLinearEquationSolverFactory<storm::RationalNumber>;
        template class EliminationMinMaxLinearEquationSolverFactory<storm::RationalNumber>;
#endif
    }
}
