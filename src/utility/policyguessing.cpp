/* 
 * File:   Regions.cpp
 * Author: Tim Quatmann
 * 
 * Created on November 16, 2015,
 */
#include <stdint.h>

#include "src/utility/policyguessing.h"

#include "src/storage/SparseMatrix.h"
#include "src/utility/macros.h"
#include "src/utility/solver.h"
#include "src/solver/LinearEquationSolver.h"
#include "src/solver/GameSolver.h"
#include "graph.h"

namespace storm {
    namespace utility{
        namespace policyguessing {
            
            template <typename ValueType>
            void solveGame( storm::solver::GameSolver<ValueType>& solver,
                            std::vector<ValueType>& x,
                            std::vector<ValueType> const& b,
                            OptimizationDirection player1Goal,
                            OptimizationDirection player2Goal,
                            std::vector<storm::storage::sparse::state_type>& player1Policy, 
                            std::vector<storm::storage::sparse::state_type>& player2Policy,
                            storm::storage::BitVector const& targetChoices,
                            ValueType const& prob0Value
                     ){
                
                solveInducedEquationSystem(solver, x, b, player1Policy, player2Policy, targetChoices, prob0Value);
                solver.setPolicyTracking();
                solver.solveGame(player1Goal, player2Goal, x, b);
                player1Policy = solver.getPlayer1Policy();
                player2Policy = solver.getPlayer2Policy();
            }
            
            
            template <typename ValueType>
            void solveInducedEquationSystem(storm::solver::GameSolver<ValueType> const& solver,
                                            std::vector<ValueType>& x,
                                            std::vector<ValueType> const& b,
                                            std::vector<storm::storage::sparse::state_type> const& player1Policy, 
                                            std::vector<storm::storage::sparse::state_type> const& player2Policy,
                                            storm::storage::BitVector const& targetChoices,
                                            ValueType const& prob0Value){
                uint_fast64_t numberOfPlayer1States = x.size();
                
                //Get the rows of the player2matrix that are selected by the policies
                //Note that rows can be selected more then once and in an arbitrary order.
                std::vector<storm::storage::sparse::state_type> selectedRows(numberOfPlayer1States);
                for (uint_fast64_t pl1State = 0; pl1State < numberOfPlayer1States; ++pl1State){
                    auto const& pl1Row = solver.getPlayer1Matrix().getRow(solver.getPlayer1Matrix().getRowGroupIndices()[pl1State] + player1Policy[pl1State]);
                    STORM_LOG_ASSERT(pl1Row.getNumberOfEntries()==1, "");
                    uint_fast64_t pl2State = pl1Row.begin()->getColumn();
                    selectedRows[pl1State] = solver.getPlayer2Matrix().getRowGroupIndices()[pl2State] + player2Policy[pl2State];
                }
                
                //Get the matrix A, vector b, and the targetStates induced by this selection
                storm::storage::SparseMatrix<ValueType> inducedA = solver.getPlayer2Matrix().selectRowsFromRowIndexSequence(selectedRows, false);
                std::vector<ValueType> inducedB(numberOfPlayer1States);
                storm::utility::vector::selectVectorValues<ValueType>(inducedB, selectedRows, b);
                storm::storage::BitVector inducedTarget(numberOfPlayer1States, false);
                for (uint_fast64_t pl1State = 0; pl1State < numberOfPlayer1States; ++pl1State){
                    if(targetChoices.get(selectedRows[pl1State])){
                        inducedTarget.set(pl1State);
                    }
                }
                
                //Find the states from which no target state is reachable.
                //Note that depending on the policies, qualitative properties might have changed which makes this step necessary.
                storm::storage::BitVector probGreater0States = storm::utility::graph::performProbGreater0(inducedA.transpose(), storm::storage::BitVector(numberOfPlayer1States, true), inducedTarget);
                
                //Get the final A,x, and b and invoke linear equation solver
                storm::storage::SparseMatrix<ValueType> subA = inducedA.getSubmatrix(true, probGreater0States, probGreater0States, true);
                subA.convertToEquationSystem();
                std::vector<ValueType> subX(probGreater0States.getNumberOfSetBits());
                storm::utility::vector::selectVectorValues(subX, probGreater0States, x);
                std::vector<ValueType> subB(probGreater0States.getNumberOfSetBits());
                storm::utility::vector::selectVectorValues(subB, probGreater0States, inducedB);
                std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> linEqSysSolver = storm::utility::solver::LinearEquationSolverFactory<ValueType>().create(subA);
                linEqSysSolver->solveEquationSystem(subX, subB);
            
                //fill in the result
                storm::utility::vector::setVectorValues(x, probGreater0States, subX);
                storm::utility::vector::setVectorValues(x, (~probGreater0States), prob0Value);
            }
            
            
            template <typename ValueType>
            void solveMinMaxLinearEquationSystem( storm::solver::MinMaxLinearEquationSolver<ValueType>& solver,
                                    std::vector<ValueType>& x,
                                    std::vector<ValueType> const& b,
                                    OptimizationDirection goal,
                                    std::vector<storm::storage::sparse::state_type>& policy,
                                    storm::storage::BitVector const& targetChoices,
                                    ValueType const& prob0Value
                                ){
                
                solveInducedEquationSystem(solver, x, b, policy, targetChoices, prob0Value);
                solver.setPolicyTracking();
                solver.solveEquationSystem(goal, x, b);
                policy = solver.getPolicy();
            }
            
            
            template <typename ValueType>
            void solveInducedEquationSystem(storm::solver::MinMaxLinearEquationSolver<ValueType> const& solver,
                                            std::vector<ValueType>& x,
                                            std::vector<ValueType> const& b,
                                            std::vector<storm::storage::sparse::state_type> const& policy,
                                            storm::storage::BitVector const& targetChoices,
                                            ValueType const& prob0Value
                                        ){
                uint_fast64_t numberOfStates = x.size();
                
                //Get the matrix A, vector b, and the targetStates induced by the policy
                storm::storage::SparseMatrix<ValueType> inducedA = solver.getMatrix().selectRowsFromRowGroups(policy, false);
                std::vector<ValueType> inducedB(numberOfStates);
                storm::utility::vector::selectVectorValues<ValueType>(inducedB, policy, solver.getMatrix().getRowGroupIndices(), b);
                storm::storage::BitVector inducedTarget(numberOfStates, false);
                for (uint_fast64_t state = 0; state < numberOfStates; ++state){
                    if(targetChoices.get(solver.getMatrix().getRowGroupIndices()[state] + policy[state])){
                        inducedTarget.set(state);
                    }
                }
                
                //Find the states from which no target state is reachable.
                //Note that depending on the policies, qualitative properties might have changed which makes this step necessary.
                storm::storage::BitVector probGreater0States = storm::utility::graph::performProbGreater0(inducedA.transpose(), storm::storage::BitVector(numberOfStates, true), inducedTarget);
                
                //Get the final A,x, and b and invoke linear equation solver
                storm::storage::SparseMatrix<ValueType> subA = inducedA.getSubmatrix(true, probGreater0States, probGreater0States, true);
                subA.convertToEquationSystem();
                std::vector<ValueType> subX(probGreater0States.getNumberOfSetBits());
                storm::utility::vector::selectVectorValues(subX, probGreater0States, x);
                std::vector<ValueType> subB(probGreater0States.getNumberOfSetBits());
                storm::utility::vector::selectVectorValues(subB, probGreater0States, inducedB);
                std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> linEqSysSolver = storm::utility::solver::LinearEquationSolverFactory<ValueType>().create(subA);
                linEqSysSolver->solveEquationSystem(subX, subB);
            
                //fill in the result
                storm::utility::vector::setVectorValues(x, probGreater0States, subX);
                storm::utility::vector::setVectorValues(x, (~probGreater0States), prob0Value);
            }
            
            
            template void solveGame<double>( storm::solver::GameSolver<double>& solver,
                                std::vector<double>& x,
                                std::vector<double> const& b,
                                OptimizationDirection player1Goal,
                                OptimizationDirection player2Goal,
                                std::vector<storm::storage::sparse::state_type>& player1Policy, 
                                std::vector<storm::storage::sparse::state_type>& player2Policy,
                                storm::storage::BitVector const& targetChoices,
                                double const& prob0Value
                            );
            
            template void solveInducedEquationSystem<double>(storm::solver::GameSolver<double> const& solver,
                                std::vector<double>& x,
                                std::vector<double> const& b,
                                std::vector<storm::storage::sparse::state_type> const& player1Policy, 
                                std::vector<storm::storage::sparse::state_type> const& player2Policy,
                                storm::storage::BitVector const& targetChoices,
                                double const& prob0Value
                            );
            
            template void solveMinMaxLinearEquationSystem<double>( storm::solver::MinMaxLinearEquationSolver<double>& solver,
                                std::vector<double>& x,
                                std::vector<double> const& b,
                                OptimizationDirection goal,
                                std::vector<storm::storage::sparse::state_type>& policy,
                                storm::storage::BitVector const& targetChoices,
                                double const& prob0Value
                            );
            
            template void solveInducedEquationSystem<double>(storm::solver::MinMaxLinearEquationSolver<double> const& solver,
                                std::vector<double>& x,
                                std::vector<double> const& b,
                                std::vector<storm::storage::sparse::state_type> const& policy,
                                storm::storage::BitVector const& targetChoices,
                                double const& prob0Value
                            );
                
        }
    }
}
