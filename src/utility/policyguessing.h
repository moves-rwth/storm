/* 
 * File:   Regions.h
 * Author: Tim Quatmann
 *
 * Created on November 16, 2015,
 * 
 * This file provides functions to apply a solver on a nondeterministic model or a two player game.
 * However, policies are used to compute an initial guess which will (hopefully) speed up the value iteration techniques.
 */


#ifndef STORM_UTILITY_POLICYGUESSING_H
#define	STORM_UTILITY_POLICYGUESSING_H

#include "src/solver/GameSolver.h"
#include "src/solver/MinMaxLinearEquationSolver.h"
#include "src/solver/OptimizationDirection.h"
#include "src/utility/vector.h"
#include "src/storage/BitVector.h"
#include "src/storage/sparse/StateType.h"

namespace storm {
    namespace utility{
        namespace policyguessing {
            
            /*!
             * invokes the given game solver.
             * 
             * The given policies for player 1 and player 2 will serve as initial guess.
             * A linear equation system defined by the induced Matrix A and vector b is solved before
             * solving the actual game.
             * Note that, depending on the policies, the qualitative properties of the graph defined by A
             * might be different to the original graph of the game.
             * To ensure a unique solution, we need to filter out the "prob0"-states.
             * To identify these states and set the result for them correctly, it is necessary to know whether rewards or probabilities are to be computed
             *
             * @param solver the solver to be invoked
             * @param player1Goal Sets whether player 1 wants to minimize or maximize.
             * @param player2Goal Sets whether player 2 wants to minimize or maximize.
             * @param x The initial guess of the solution.
             * @param b The vector to add after matrix-vector multiplication.
             * @param player1Policy A policy that selects rows in every rowgroup of player1. This will be used as an initial guess
             * @param player2Policy A policy that selects rows in every rowgroup of player2. This will be used as an initial guess
             * @param targetChoices marks the choices in the player2 matrix that have a positive probability to lead to a target state
             * @param prob0Value the value that, after policy instantiation, is assigned to the states that have probability zero to reach a target
             * @return The solution vector in the form of the vector x as well as the two policies.
             */
            template<typename ValueType>
            void solveGame( storm::solver::GameSolver<ValueType>& solver,
                                    std::vector<ValueType>& x,
                                    std::vector<ValueType> const& b,
                                    OptimizationDirection player1Goal,
                                    OptimizationDirection player2Goal,
                                    std::vector<storm::storage::sparse::state_type>& player1Policy,
                                    std::vector<storm::storage::sparse::state_type>& player2Policy,
                                    storm::storage::BitVector const& targetChoices,
                                    ValueType const& prob0Value
                                );
            
            
            /*!
             * Solves the equation system defined by the matrix A and vector b' that result from applying
             * the given policies to the matrices of the two players and the given b.
             * 
             * Note that, depending on the policies, the qualitative properties of the graph defined by A
             * might be different to the original graph of the game.
             * To ensure a unique solution, we need to filter out the "prob0"-states.
             * (Notice that new "prob1"-states do not harm as actual target states are always excluded, independent of the choice)
             *
             * @param solver the solver that contains the two player matrices
             * @param x The initial guess of the solution.
             * @param b The vector in which to select the entries of the right hand side
             * @param player1Policy A policy that selects rows in every rowgroup of player1.
             * @param player2Policy A policy that selects rows in every rowgroup of player2.
             * @param targetChoices marks the choices in the player2 matrix that have a positive probability to lead to a target state
             * @param prob0Value the value that is assigned to the states that have probability zero to reach a target
             * @return The solution vector in the form of the vector x.
             */
            template<typename ValueType>
            void solveInducedEquationSystem(storm::solver::GameSolver<ValueType> const& solver,
                                                    std::vector<ValueType>& x,
                                                    std::vector<ValueType> const& b,
                                                    std::vector<storm::storage::sparse::state_type> const& player1Policy,
                                                    std::vector<storm::storage::sparse::state_type> const& player2Policy,
                                                    storm::storage::BitVector const& targetChoices,
                                                    ValueType const& prob0Value
                                                );
            
            
            
            /*!
             * invokes the given MinMaxLinearEquationSolver.
             * 
             * The given policy will serve as an initial guess.
             * A linear equation system defined by the induced Matrix A and vector b is solved before
             * solving the actual MinMax equation system.
             * Note that, depending on the policy, the qualitative properties of the graph defined by A
             * might be different to the original graph.
             * To ensure a unique solution, we need to filter out the "prob0"-states.
             * To identify these states and set the result for them correctly, it is necessary to know whether rewards or probabilities are to be computed
             *

             * @param solver the solver that contains the two player matrices
             * @param x The initial guess of the solution.
             * @param b The vector to add after matrix-vector multiplication.
             * @param goal Sets whether we want to minimize or maximize.
             * @param policy A policy that selects rows in every rowgroup.
             * @param targetChoices marks the rows in the matrix that have a positive probability to lead to a target state
             * @param prob0Value the value that is assigned to the states that have probability zero to reach a target
             * @return The solution vector in the form of the vector x.
             */
            template<typename ValueType>
            void solveMinMaxLinearEquationSystem( storm::solver::MinMaxLinearEquationSolver<ValueType>& solver,
                                    std::vector<ValueType>& x,
                                    std::vector<ValueType> const& b,
                                    OptimizationDirection goal,
                                    std::vector<storm::storage::sparse::state_type>& policy,
                                    storm::storage::BitVector const& targetChoices,
                                    ValueType const& prob0Value
                                );
            
            /*!
             * Solves the equation system defined by the matrix A and vector b' that result from applying
             * the given policy to the matrices of the two players and the given b.
             * 
             * Note that, depending on the policy, the qualitative properties of the graph defined by A
             * might be different to the original graph
             * To ensure a unique solution, we need to filter out the "prob0"-states.
             * (Notice that new "prob1"-states do not harm as actual target states are always excluded, independent of the choice)
             *
             * @param solverthe solver that contains the two player matrices
             * @param x The initial guess of the solution.
             * @param b The vector in which to select the entries of the right hand side
             * @param policy A policy that selects rows in every rowgroup.
             * @param targetChoices marks the rows in the matrix that have a positive probability to lead to a target state
             * @param prob0Value the value that is assigned to the states that have probability zero to reach a target
             * @return The solution vector in the form of the vector x.
             */
            template<typename ValueType>
            void solveInducedEquationSystem(storm::solver::MinMaxLinearEquationSolver<ValueType> const& solver,
                                            std::vector<ValueType>& x,
                                            std::vector<ValueType> const& b,
                                            std::vector<storm::storage::sparse::state_type> const& policy,
                                            storm::storage::BitVector const& targetChoices,
                                            ValueType const& prob0Value
                                        );
            
        }
    }
}


#endif	/* STORM_UTILITY_REGIONS_H */

