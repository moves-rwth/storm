#ifndef STORM_SOLVER_GAMESOLVER_H_
#define STORM_SOLVER_GAMESOLVER_H_

#include <vector>

#include "storm/solver/AbstractGameSolver.h"
#include "storm/solver/TerminationCondition.h"
#include "storm/solver/OptimizationDirection.h"

#include "storm/storage/sparse/StateType.h"

namespace storm {
    namespace storage {
        template<typename ValueType>
        class SparseMatrix;
    }

    namespace solver {
        template<typename ValueType>
        class GameSolver : public AbstractGameSolver<ValueType> {
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
            GameSolver(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix, storm::storage::SparseMatrix<ValueType> const& player2Matrix, ValueType precision, uint_fast64_t maximalNumberOfIterations, bool relative);

            /*!
             * Solves the equation system defined by the game matrices. Note that the game matrices have to be given upon
             * construction time of the solver object.
             *
             * @param player1Goal Sets whether player 1 wants to minimize or maximize.
             * @param player2Goal Sets whether player 2 wants to minimize or maximize.
             * @param x The initial guess of the solution. For correctness, the guess has to be less (or equal) to the final solution (unless both players minimize)
             * @param b The vector to add after matrix-vector multiplication.
             * @return The solution vector in the for of the vector x.
             */
            virtual void solveGame(OptimizationDirection player1Goal, OptimizationDirection player2Goal, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            
            /*!
             * Performs n multiplications of the form
             *  reduce(player1Matrix * reduce(player2Matrix * x + b))
             *
            * @param player1Goal Sets whether player 1 wants to minimize or maximize.
             * @param player2Goal Sets whether player 2 wants to minimize or maximize.
             * @param x The initial guess of the solution. For correctness, the guess has to be less (or equal) to the final solution (unless both players minimize)
             * @param b The vector to add after matrix-vector multiplication.
             * @param n The number of times we perform the multiplication
             */
            virtual void repeatedMultiply(OptimizationDirection player1Goal, OptimizationDirection player2Goal, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n) const;

            storm::storage::SparseMatrix<ValueType> const& getPlayer2Matrix() const;
            storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& getPlayer1Matrix() const;
            
         private:
            // The matrix defining the choices of player 1.
            storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix;

            // The matrix defining the choices of player 2.
            storm::storage::SparseMatrix<ValueType> const& player2Matrix;
        };
    }
}

#endif /* STORM_SOLVER_GAMESOLVER_H_ */
