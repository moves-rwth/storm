#include "src/solver/GameSolver.h"

#include "src/storage/SparseMatrix.h"

#include "src/utility/vector.h"

namespace storm {
    namespace solver {
        template <typename ValueType>
        GameSolver<ValueType>::GameSolver(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix, storm::storage::SparseMatrix<ValueType> const& player2Matrix) : AbstractGameSolver(), player1Matrix(player1Matrix), player2Matrix(player2Matrix) {
            // Intentionally left empty.
        }

        template <typename ValueType>
        GameSolver<ValueType>::GameSolver(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix, storm::storage::SparseMatrix<ValueType> const& player2Matrix, double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : AbstractGameSolver(precision, maximalNumberOfIterations, relative), player1Matrix(player1Matrix), player2Matrix(player2Matrix) {
            // Intentionally left empty.
        }

        template <typename ValueType>
        void GameSolver<ValueType>::solveGame(OptimizationDirection player1Goal, OptimizationDirection player2Goal, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            // Set up the environment for value iteration.
            uint_fast64_t numberOfPlayer1States = x.size();

            bool converged = false;
            std::vector<ValueType> tmpResult(numberOfPlayer1States);
            std::vector<ValueType> nondetResult(player2Matrix.getRowCount());
            std::vector<ValueType> player2Result(player2Matrix.getRowGroupCount());

            // Now perform the actual value iteration.
            uint_fast64_t iterations = 0;
            do {
                player2Matrix.multiplyWithVector(x, nondetResult);
                storm::utility::vector::addVectors(b, nondetResult, nondetResult);

                if (player2Goal == OptimizationDirection::Minimize) {
                    storm::utility::vector::reduceVectorMin(nondetResult, player2Result, player2Matrix.getRowGroupIndices());
                } else {
                    storm::utility::vector::reduceVectorMax(nondetResult, player2Result, player2Matrix.getRowGroupIndices());
                }

                for (uint_fast64_t pl1State = 0; pl1State < numberOfPlayer1States; ++pl1State) {
                    uint_fast64_t startRow = player1Matrix.getRowGroupIndices()[pl1State];
                    uint_fast64_t endRow = player1Matrix.getRowGroupIndices()[pl1State + 1];

                    storm::storage::SparseMatrix<storm::storage::sparse::state_type>::const_rows relevantRows = player1Matrix.getRows(startRow, endRow - 1);
                    storm::storage::SparseMatrix<storm::storage::sparse::state_type>::const_iterator it = relevantRows.begin();
                    storm::storage::SparseMatrix<storm::storage::sparse::state_type>::const_iterator ite = relevantRows.end();

                    // Set the first value.
                    tmpResult[pl1State] = player2Result[it->getColumn()];
                    ++it;

                    // Now iterate through the different values and pick the extremal one.
                    if (player1Goal == OptimizationDirection::Minimize) {
                        for (; it != ite; ++it) {
                            tmpResult[pl1State] = std::min(tmpResult[pl1State], player2Result[it->getColumn()]);
                        }
                    } else {
                        for (; it != ite; ++it) {
                            tmpResult[pl1State] = std::max(tmpResult[pl1State], player2Result[it->getColumn()]);
                        }
                    }
                }

                // Check if the process converged and set up the new iteration in case we are not done.
                converged = storm::utility::vector::equalModuloPrecision(x, tmpResult, precision, relative);
                std::swap(x, tmpResult);

                ++iterations;
            } while (!converged);
        }

        template class GameSolver<double>;
    }
}