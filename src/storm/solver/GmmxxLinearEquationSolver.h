#ifndef STORM_SOLVER_GMMXXLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_GMMXXLINEAREQUATIONSOLVER_H_

#include <ostream>

#include "storm/utility/gmm.h"

#include "storm/solver/GmmxxMultiplier.h"

#include "storm/solver/LinearEquationSolver.h"
#include "SolverSelectionOptions.h"

namespace storm {
    namespace solver {


        /*!
         * A class that uses the gmm++ library to implement the LinearEquationSolver interface.
         */
        template<typename ValueType>
        class GmmxxLinearEquationSolver : public LinearEquationSolver<ValueType> {
        public:
            GmmxxLinearEquationSolver();
            GmmxxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A);
            GmmxxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A);
            
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType> const& A) override;
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType>&& A) override;
            
            virtual void multiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const override;
            virtual void multiplyAndReduce(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint_fast64_t>* choices = nullptr) const override;
            virtual bool supportsGaussSeidelMultiplication() const override;
            virtual void multiplyGaussSeidel(std::vector<ValueType>& x, std::vector<ValueType> const* b) const override;
            virtual void multiplyAndReduceGaussSeidel(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<uint_fast64_t>* choices = nullptr) const override;

            virtual LinearEquationSolverProblemFormat getEquationProblemFormat(Environment const& env) const override;
            
            virtual void clearCache() const override;

        protected:
            virtual bool internalSolveEquations(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const override;
            
        private:
            
            GmmxxLinearEquationSolverMethod getMethod(Environment const& env) const;
            
            virtual uint64_t getMatrixRowCount() const override;
            virtual uint64_t getMatrixColumnCount() const override;

            // The matrix in gmm++ format.
            std::unique_ptr<gmm::csr_matrix<ValueType>> gmmxxA;
            
            // A multiplier object used to dispatch the multiplication calls.
            GmmxxMultiplier<ValueType> multiplier;
            
            // cached data obtained during solving
            mutable std::unique_ptr<gmm::ilu_precond<gmm::csr_matrix<ValueType>>> iluPreconditioner;
            mutable std::unique_ptr<gmm::diagonal_precond<gmm::csr_matrix<ValueType>>> diagonalPreconditioner;
        };
        
        template<typename ValueType>
        class GmmxxLinearEquationSolverFactory : public LinearEquationSolverFactory<ValueType> {
        public:
            using LinearEquationSolverFactory<ValueType>::create;
            
            virtual std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> create(Environment const& env, LinearEquationSolverTask const& task = LinearEquationSolverTask::Unspecified) const override;
            
            virtual std::unique_ptr<LinearEquationSolverFactory<ValueType>> clone() const override;

        };

    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_GMMXXLINEAREQUATIONSOLVER_H_ */
