#ifndef STORM_SOLVER_GAMESOLVER_H_
#define STORM_SOLVER_GAMESOLVER_H_

#include "src/solver/AbstractGameSolver.h"

namespace storm {
    namespace solver {
        template<typename ValueType>
        class GameSolver : public AbstractGameSolver {
        public:
            /*
             * Constructs a game solver with the given player 1 and player 2 matrices.
             *
             * @param player1Matrix The matrix defining the choices of player 1.
             * @param player2Matrix The matrix defining the choices of player 2.
             */
            GameSolver(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix, storm::storage::SparseMatrix<ValueType> const& player2Matrix);

            /*
             * Constructs a game solver with the given player 1 and player 2 matrices and options.
             *
             * @param player1Matrix The matrix defining the choices of player 1.
             * @param player2Matrix The matrix defining the choices of player 2.
             * @param precision The precision that is used to detect convergence.
             * @param maximalNumberOfIterations The maximal number of iterations.
             * @param relative Sets whether or not to detect convergence with a relative or absolute criterion.
             */
            GameSolver(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix, storm::storage::SparseMatrix<ValueType> const& player2Matrix, double precision, uint_fast64_t maximalNumberOfIterations, bool relative);

            /*!
             * Solves the equation system defined by the game matrix. Note that the game matrix has to be given upon
             * construction time of the solver object.
             *
             * @param player1Goal Sets whether player 1 wants to minimize or maximize.
             * @param player2Goal Sets whether player 2 wants to minimize or maximize.
             * @param x The initial guess of the solution.
             * @param b The vector to add after matrix-vector multiplication.
             * @return The solution vector in the for of the vector x.
             */
            virtual void solveGame(OptimizationDirection player1Goal, OptimizationDirection player2Goal, std::vector<ValueType>& x, storm::dd::Add<Type> const& b) const;

        private:
            // The matrix defining the choices of player 1.
            storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix;

            // The matrix defining the choices of player 2.
            storm::storage::SparseMatrix<ValueType> const& player2Matrix;
        };
    }
}

#endif /* STORM_SOLVER_GAMESOLVER_H_ */
