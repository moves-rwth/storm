#include "storm/solver/GameSolver.h"

#include "storm/solver/LinearEquationSolver.h"
#include "storm/utility/solver.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/utility/vector.h"
#include "storm/utility/graph.h"

namespace storm {
    namespace solver {
        template <typename ValueType>
        GameSolver<ValueType>::GameSolver(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix, storm::storage::SparseMatrix<ValueType> const& player2Matrix) : AbstractGameSolver<ValueType>(), player1Matrix(player1Matrix), player2Matrix(player2Matrix) {
            // Intentionally left empty.
        }

        template <typename ValueType>
        GameSolver<ValueType>::GameSolver(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix, storm::storage::SparseMatrix<ValueType> const& player2Matrix, double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : AbstractGameSolver<ValueType>(precision, maximalNumberOfIterations, relative), player1Matrix(player1Matrix), player2Matrix(player2Matrix) {
            // Intentionally left empty.
        }

        template <typename ValueType>
        void GameSolver<ValueType>::solveGame(OptimizationDirection player1Goal, OptimizationDirection player2Goal, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
                        
            // Set up the environment for value iteration.
            bool converged = false;
            uint_fast64_t numberOfPlayer1States = x.size();
            std::vector<ValueType> tmpResult(numberOfPlayer1States);
            std::vector<ValueType> nondetResult(player2Matrix.getRowCount());
            std::vector<ValueType> player2Result(player2Matrix.getRowGroupCount());
            
            // check if we have a scheduler hint to apply
            if(this->hasSchedulerHints()) {
                //Get the rows of the player2matrix that are selected by the schedulers
                //Note that rows can be selected more then once and in an arbitrary order.
                std::vector<storm::storage::sparse::state_type> selectedRows(numberOfPlayer1States);
                for (uint_fast64_t pl1State = 0; pl1State < numberOfPlayer1States; ++pl1State){
                    auto const& pl1Row = player1Matrix.getRow(pl1State, this->player1SchedulerHint->getChoice(pl1State));
                    STORM_LOG_ASSERT(pl1Row.getNumberOfEntries()==1, "We assume that rows of player one have one entry.");
                    uint_fast64_t pl2State = pl1Row.begin()->getColumn();
                    selectedRows[pl1State] = player2Matrix.getRowGroupIndices()[pl2State] + this->player2SchedulerHint->getChoice(pl1State);
                }
                //Get the matrix and the vector induced by this selection
                auto inducedMatrix = player2Matrix.selectRowsFromRowIndexSequence(selectedRows, true);
                inducedMatrix.convertToEquationSystem();
                storm::utility::vector::selectVectorValues<ValueType>(tmpResult, selectedRows, b);
                auto submatrixSolver = storm::solver::GeneralLinearEquationSolverFactory<ValueType>().create(std::move(inducedMatrix));
                if (this->lowerBound) { submatrixSolver->setLowerBound(this->lowerBound.get()); }
                if (this->upperBound) { submatrixSolver->setUpperBound(this->upperBound.get()); }
                submatrixSolver->solveEquations(x, tmpResult);
            }

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
                    storm::storage::SparseMatrix<storm::storage::sparse::state_type>::const_rows relevantRows = player1Matrix.getRowGroup(pl1State);
                    if (relevantRows.getNumberOfEntries() > 0) {
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
                    } else {
                        tmpResult[pl1State] = storm::utility::zero<ValueType>();
                    }
                }

                // Check if the process converged and set up the new iteration in case we are not done.
                converged = storm::utility::vector::equalModuloPrecision(x, tmpResult, this->precision, this->relative);
                std::swap(x, tmpResult);

                ++iterations;
            } while (!converged && iterations < this->maximalNumberOfIterations && !(this->hasCustomTerminationCondition() && this->getTerminationCondition().terminateNow(x)));
            
            STORM_LOG_WARN_COND(converged, "Iterative solver for stochastic two player games did not converge after " << iterations << " iterations.");
            
            if(this->trackScheduler){
                std::vector<uint_fast64_t> player2Choices(player2Matrix.getRowGroupCount());
                storm::utility::vector::reduceVectorMinOrMax(player2Goal, nondetResult, player2Result, player2Matrix.getRowGroupIndices(), &player2Choices);
                this->player2Scheduler = std::make_unique<storm::storage::TotalScheduler>(std::move(player2Choices));

                std::vector<uint_fast64_t> player1Choices(numberOfPlayer1States, 0);
                for (uint_fast64_t pl1State = 0; pl1State < numberOfPlayer1States; ++pl1State) {
                    storm::storage::SparseMatrix<storm::storage::sparse::state_type>::const_rows relevantRows = player1Matrix.getRowGroup(pl1State);
                    if (relevantRows.getNumberOfEntries() > 0) {
                        storm::storage::SparseMatrix<storm::storage::sparse::state_type>::const_iterator it = relevantRows.begin();
                        storm::storage::SparseMatrix<storm::storage::sparse::state_type>::const_iterator ite = relevantRows.end();
                        // Set the first value.
                        tmpResult[pl1State] = player2Result[it->getColumn()];
                        ++it;
                        storm::storage::sparse::state_type localChoice = 1;
                        // Now iterate through the different values and pick the extremal one.
                        if (player1Goal == OptimizationDirection::Minimize) {
                            for (; it != ite; ++it, ++localChoice) {
                                if(player2Result[it->getColumn()] < tmpResult[pl1State]){
                                    tmpResult[pl1State] = player2Result[it->getColumn()];
                                    player1Choices[pl1State] = localChoice;
                                }
                            }
                        } else {
                            for (; it != ite; ++it, ++localChoice) {
                                if(player2Result[it->getColumn()] > tmpResult[pl1State]){
                                    tmpResult[pl1State] = player2Result[it->getColumn()];
                                    player1Choices[pl1State] = localChoice;
                                }
                            }
                        }
                    } else {
                        STORM_LOG_ERROR("There is no choice for Player 1 at state " << pl1State << " in the stochastic two player game. This is not expected!");
                    }
                }
                this->player1Scheduler = std::make_unique<storm::storage::TotalScheduler>(std::move(player1Choices));
            }
        }
    
        template <typename ValueType>
        void GameSolver<ValueType>::repeatedMultiply(OptimizationDirection player1Goal, OptimizationDirection player2Goal, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n) const {
            // Set up the environment for iterations
            uint_fast64_t numberOfPlayer1States = x.size();
            std::vector<ValueType> tmpResult(numberOfPlayer1States);
            std::vector<ValueType> nondetResult(player2Matrix.getRowCount());
            std::vector<ValueType> player2Result(player2Matrix.getRowGroupCount());
            
            for (uint_fast64_t iteration = 0; iteration < n; ++iteration) {
                player2Matrix.multiplyWithVector(x, nondetResult);
                
                if(b != nullptr) {
                    storm::utility::vector::addVectors(*b, nondetResult, nondetResult);
                }
                
                storm::utility::vector::reduceVectorMinOrMax(player2Goal, nondetResult, player2Result, player2Matrix.getRowGroupIndices());

                for (uint_fast64_t pl1State = 0; pl1State < numberOfPlayer1States; ++pl1State) {
                    storm::storage::SparseMatrix<storm::storage::sparse::state_type>::const_rows relevantRows = player1Matrix.getRowGroup(pl1State);
                    if (relevantRows.getNumberOfEntries() > 0) {
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
                    } else {
                        tmpResult[pl1State] = storm::utility::zero<ValueType>();
                    }
                }
                std::swap(x, tmpResult);
            }
        }
        
        template <typename ValueType>
        storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& GameSolver<ValueType>::getPlayer1Matrix() const {
            return player1Matrix;
        }
        
        template <typename ValueType>
        storm::storage::SparseMatrix<ValueType> const& GameSolver<ValueType>::getPlayer2Matrix() const {
            return player2Matrix;
        }
    
        template class GameSolver<double>;
    }
}
