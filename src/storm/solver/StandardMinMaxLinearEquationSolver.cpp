#include "storm/solver/StandardMinMaxLinearEquationSolver.h"

#include "storm/solver/IterativeMinMaxLinearEquationSolver.h"
#include "storm/solver/GmmxxLinearEquationSolver.h"
#include "storm/solver/EigenLinearEquationSolver.h"
#include "storm/solver/NativeLinearEquationSolver.h"
#include "storm/solver/EliminationLinearEquationSolver.h"
#include "storm/solver/TopologicalLinearEquationSolver.h"

#include "storm/environment/solver/MinMaxSolverEnvironment.h"

#include "storm/utility/vector.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/NotImplementedException.h"
namespace storm {
    namespace solver {
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolver<ValueType>::StandardMinMaxLinearEquationSolver(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory) : linearEquationSolverFactory(std::move(linearEquationSolverFactory)), A(nullptr) {
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
        void StandardMinMaxLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType> const& matrix) {
            this->localA = nullptr;
            this->A = &matrix;
        }
        
        template<typename ValueType>
        void StandardMinMaxLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType>&& matrix) {
            this->localA = std::make_unique<storm::storage::SparseMatrix<ValueType>>(std::move(matrix));
            this->A = this->localA.get();
        }
        
        template<typename ValueType>
        void StandardMinMaxLinearEquationSolver<ValueType>::repeatedMultiply(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n) const {
            if (!linEqSolverA) {
                linEqSolverA = linearEquationSolverFactory->create(env, *A, LinearEquationSolverTask::Multiply);
                linEqSolverA->setCachingEnabled(true);
            }
            
            if (!auxiliaryRowGroupVector) {
                auxiliaryRowGroupVector = std::make_unique<std::vector<ValueType>>(this->A->getRowGroupCount());
            }
            
            this->startMeasureProgress();
            for (uint64_t i = 0; i < n; ++i) {
                linEqSolverA->multiplyAndReduce(dir, this->A->getRowGroupIndices(), x, b, *auxiliaryRowGroupVector);
                std::swap(x, *auxiliaryRowGroupVector);

                // Potentially show progress.
                this->showProgressIterative(i, n);
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
        StandardMinMaxLinearEquationSolverFactory<ValueType>::StandardMinMaxLinearEquationSolverFactory() : MinMaxLinearEquationSolverFactory<ValueType>(), linearEquationSolverFactory(std::make_unique<GeneralLinearEquationSolverFactory<ValueType>>()) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverFactory<ValueType>::StandardMinMaxLinearEquationSolverFactory(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory) : MinMaxLinearEquationSolverFactory<ValueType>(), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverFactory<ValueType>::StandardMinMaxLinearEquationSolverFactory(EquationSolverType const& solverType) : MinMaxLinearEquationSolverFactory<ValueType>() {
            switch (solverType) {
                case EquationSolverType::Gmmxx: linearEquationSolverFactory = std::make_unique<GmmxxLinearEquationSolverFactory<ValueType>>(); break;
                case EquationSolverType::Eigen: linearEquationSolverFactory = std::make_unique<EigenLinearEquationSolverFactory<ValueType>>(); break;
                case EquationSolverType::Native: linearEquationSolverFactory = std::make_unique<NativeLinearEquationSolverFactory<ValueType>>(); break;
                case EquationSolverType::Elimination: linearEquationSolverFactory = std::make_unique<EliminationLinearEquationSolverFactory<ValueType>>(); break;
                case EquationSolverType::Topological: linearEquationSolverFactory = std::make_unique<TopologicalLinearEquationSolverFactory<ValueType>>(); break;
            }
        }

        template<>
        StandardMinMaxLinearEquationSolverFactory<storm::RationalNumber>::StandardMinMaxLinearEquationSolverFactory(EquationSolverType const& solverType) : MinMaxLinearEquationSolverFactory<storm::RationalNumber>() {
            switch (solverType) {
                case EquationSolverType::Eigen: linearEquationSolverFactory = std::make_unique<EigenLinearEquationSolverFactory<storm::RationalNumber>>(); break;
                case EquationSolverType::Elimination: linearEquationSolverFactory = std::make_unique<EliminationLinearEquationSolverFactory<storm::RationalNumber>>(); break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported equation solver for this data type.");
            }
        }

        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> StandardMinMaxLinearEquationSolverFactory<ValueType>::create(Environment const& env) const {
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> result;
            auto method = env.solver().minMax().getMethod();
            if (method == MinMaxMethod::ValueIteration || method == MinMaxMethod::PolicyIteration || method == MinMaxMethod::RationalSearch) {
                result = std::make_unique<IterativeMinMaxLinearEquationSolver<ValueType>>(this->linearEquationSolverFactory->clone());
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "The selected min max method is not supported by this solver.");
            }
            result->setRequirementsChecked(this->isRequirementsCheckedSet());
            return result;
        }
        
        template<typename ValueType>
        GmmxxMinMaxLinearEquationSolverFactory<ValueType>::GmmxxMinMaxLinearEquationSolverFactory() : StandardMinMaxLinearEquationSolverFactory<ValueType>(EquationSolverType::Gmmxx) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        EigenMinMaxLinearEquationSolverFactory<ValueType>::EigenMinMaxLinearEquationSolverFactory() : StandardMinMaxLinearEquationSolverFactory<ValueType>(EquationSolverType::Eigen) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        NativeMinMaxLinearEquationSolverFactory<ValueType>::NativeMinMaxLinearEquationSolverFactory() : StandardMinMaxLinearEquationSolverFactory<ValueType>(EquationSolverType::Native) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        EliminationMinMaxLinearEquationSolverFactory<ValueType>::EliminationMinMaxLinearEquationSolverFactory() : StandardMinMaxLinearEquationSolverFactory<ValueType>(EquationSolverType::Elimination) {
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
