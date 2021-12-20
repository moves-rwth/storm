#include "storm/solver/StandardMinMaxLinearEquationSolver.h"

#include "storm/solver/EigenLinearEquationSolver.h"
#include "storm/solver/EliminationLinearEquationSolver.h"
#include "storm/solver/GmmxxLinearEquationSolver.h"
#include "storm/solver/IterativeMinMaxLinearEquationSolver.h"
#include "storm/solver/NativeLinearEquationSolver.h"
#include "storm/solver/TopologicalLinearEquationSolver.h"

#include "storm/environment/solver/MinMaxSolverEnvironment.h"

#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
namespace storm {
namespace solver {

template<typename ValueType>
StandardMinMaxLinearEquationSolver<ValueType>::StandardMinMaxLinearEquationSolver() : A(nullptr) {
    // Intentionally left empty.
}

template<typename ValueType>
StandardMinMaxLinearEquationSolver<ValueType>::StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A) : localA(nullptr), A(&A) {
    // Intentionally left empty.
}

template<typename ValueType>
StandardMinMaxLinearEquationSolver<ValueType>::StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A)
    : localA(std::make_unique<storm::storage::SparseMatrix<ValueType>>(std::move(A))), A(localA.get()) {
    // Intentionally left empty.
}

template<typename ValueType>
void StandardMinMaxLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType> const& matrix) {
    this->localA = nullptr;
    this->A = &matrix;
    this->clearCache();
}

template<typename ValueType>
void StandardMinMaxLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType>&& matrix) {
    this->localA = std::make_unique<storm::storage::SparseMatrix<ValueType>>(std::move(matrix));
    this->A = this->localA.get();
    this->clearCache();
}

template class StandardMinMaxLinearEquationSolver<double>;

#ifdef STORM_HAVE_CARL
template class StandardMinMaxLinearEquationSolver<storm::RationalNumber>;
#endif
}  // namespace solver
}  // namespace storm
