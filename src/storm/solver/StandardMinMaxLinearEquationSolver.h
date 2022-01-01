#pragma once

#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"

namespace storm {

class Environment;

namespace solver {

template<typename ValueType>
class StandardMinMaxLinearEquationSolver : public MinMaxLinearEquationSolver<ValueType> {
   public:
    StandardMinMaxLinearEquationSolver();
    explicit StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A);
    explicit StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A);

    virtual void setMatrix(storm::storage::SparseMatrix<ValueType> const& matrix) override;
    virtual void setMatrix(storm::storage::SparseMatrix<ValueType>&& matrix) override;

    virtual ~StandardMinMaxLinearEquationSolver() = default;

   protected:
    // If the solver takes posession of the matrix, we store the moved matrix in this member, so it gets deleted
    // when the solver is destructed.
    std::unique_ptr<storm::storage::SparseMatrix<ValueType>> localA;

    // A reference to the original sparse matrix given to this solver. If the solver takes posession of the matrix
    // the reference refers to localA.
    storm::storage::SparseMatrix<ValueType> const* A;
};

}  // namespace solver
}  // namespace storm
