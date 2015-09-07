#include "src/solver/GameSolver.h"

namespace storm {
    namespace solver {
        GameSolver::GameSolver(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix, storm::storage::SparseMatrix<ValueType> const& player2Matrix) : AbstractGameSolver(), player1Matrix(player1Matrix), player2Matrix(player2Matrix) {
            // Intentionally left empty.
        }

        GameSolver::GameSolver(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix, storm::storage::SparseMatrix<ValueType> const& player2Matrix, double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : AbstractGameSolver(precision, maximalNumberOfIterations, relative), player1Matrix(player1Matrix), player2Matrix(player2Matrix) {
            // Intentionally left empty.
        }

        void GameSolver::solveGame(OptimizationDirection player1Goal, OptimizationDirection player2Goal, std::vector<ValueType>& x, storm::dd::Add<Type> const& b) const {
            // TODO
        }
    }
}