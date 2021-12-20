#pragma once

#include "storm/adapters/eigen.h"
#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/SolverSelectionOptions.h"

namespace storm {
namespace solver {

/*!
 * A class that uses the Eigen library to implement the LinearEquationSolver interface.
 */
template<typename ValueType>
class EigenLinearEquationSolver : public LinearEquationSolver<ValueType> {
   public:
    EigenLinearEquationSolver();
    EigenLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A);
    EigenLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A);

    virtual void setMatrix(storm::storage::SparseMatrix<ValueType> const& A) override;
    virtual void setMatrix(storm::storage::SparseMatrix<ValueType>&& A) override;

    virtual LinearEquationSolverProblemFormat getEquationProblemFormat(Environment const& env) const override;

   protected:
    virtual bool internalSolveEquations(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const override;

   private:
    EigenLinearEquationSolverMethod getMethod(Environment const& env, bool isExactMode) const;

    virtual uint64_t getMatrixRowCount() const override;
    virtual uint64_t getMatrixColumnCount() const override;

    // The (eigen) matrix associated with this equation solver.
    std::unique_ptr<Eigen::SparseMatrix<ValueType>> eigenA;
};

template<typename ValueType>
class EigenLinearEquationSolverFactory : public LinearEquationSolverFactory<ValueType> {
   public:
    using LinearEquationSolverFactory<ValueType>::create;

    virtual std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> create(Environment const& env) const override;

    virtual std::unique_ptr<LinearEquationSolverFactory<ValueType>> clone() const override;
};
}  // namespace solver
}  // namespace storm
