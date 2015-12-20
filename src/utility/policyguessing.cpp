/* 
 * File:   Regions.cpp
 * Author: Tim Quatmann
 * 
 * Created on November 16, 2015,
 */
#include <stdint.h>

#include "src/utility/policyguessing.h"

#include "src/utility/macros.h"
#include "src/utility/solver.h"
#include "src/solver/LinearEquationSolver.h"
#include "graph.h"
#include "ConstantsComparator.h"

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
                
             //               std::vector<storm::storage::sparse::state_type> pl1Policy = player1Policy;
             //               std::vector<storm::storage::sparse::state_type> pl2Policy = player2Policy;
                storm::storage::SparseMatrix<ValueType> inducedA;
                std::vector<ValueType> inducedB;
                storm::storage::BitVector probGreater0States;
                getInducedEquationSystem(solver, b, player1Policy, player2Policy, targetChoices, inducedA, inducedB, probGreater0States);
                
                solveLinearEquationSystem(inducedA, x, inducedB, probGreater0States, prob0Value, solver.getPrecision(), solver.getRelative());
                
                solver.setPolicyTracking();
                bool resultCorrect = false;
                while(!resultCorrect){
                    solver.solveGame(player1Goal, player2Goal, x, b);
                    player1Policy = solver.getPlayer1Policy();
                    player2Policy = solver.getPlayer2Policy();
                    
                    //Check if the policies makes choices that lead to states from which no target state is reachable ("prob0"-states). 
                    getInducedEquationSystem(solver, b, player1Policy, player2Policy, targetChoices, inducedA, inducedB, probGreater0States);
                    resultCorrect = checkAndFixPolicy(solver, x, b, player1Policy, player2Policy, targetChoices, inducedA, inducedB, probGreater0States);
                    if(!resultCorrect){
                        //If the policy could not be fixed, it indicates that our guessed values were to high.
                        STORM_LOG_WARN("Policies could not be fixed. Restarting Gamesolver. ");
                        solveLinearEquationSystem(inducedA, x, inducedB, probGreater0States, prob0Value, solver.getPrecision(), solver.getRelative());
                        //x = std::vector<ValueType>(x.size(), storm::utility::zero<ValueType>());
                    }
                }
                /*
                        std::size_t p2Precount=0;
                        std::size_t p2Postcount=0;
                        std::size_t p1diff =0;
                        std::size_t p2diff =0;
                        std::size_t p2RelevantCount=0;
                        storm::storage::BitVector relevantP2Groups(pl2Policy.size(),false);
                        for(std::size_t i = 0; i<pl1Policy.size(); ++i){
                            if(pl1Policy[i] != player1Policy[i]){
                                ++p1diff;
                            }
                            std::size_t row = solver.getPlayer1Matrix().getRowGroupIndices()[i] + player1Policy[i];
                            auto rowObj = solver.getPlayer1Matrix().getRow(row);
                            relevantP2Groups.set(rowObj.begin()->getColumn());
                        }
                        for (std::size_t i : relevantP2Groups){
                            if(pl2Policy[i] != player2Policy[i]){
                                ++p2RelevantCount;
                            }
                        }
                        for(std::size_t i = 0; i<pl2Policy.size(); ++i){
                            if(pl2Policy[i] != player2Policy[i]){
                                ++p2diff;
                            }
                            p2Precount += pl2Policy[i];
                            p2Postcount += player2Policy[i];
                        }
                        std::cout << "P1: " << (player1Goal == OptimizationDirection::Minimize ? "MIN " : "MAX ");
                        std::cout << "P2: " << (player2Goal == OptimizationDirection::Minimize ? "MIN " : "MAX ");
                        std::cout << "Changes: P1: " << p1diff;
                        std::cout << " P2: " << p2diff << " (" << p2RelevantCount << " relevant)";
                        std::cout << " Counts P2: " << p2Precount << " and " << p2Postcount << ".";
                        std::cout << std::endl;
                  */      
                
                
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
                storm::storage::SparseMatrix<ValueType> inducedA;
                std::vector<ValueType> inducedB;
                storm::storage::BitVector probGreater0States;
                getInducedEquationSystem(solver, b, policy, targetChoices, inducedA, inducedB, probGreater0States);
                solveLinearEquationSystem(inducedA, x, inducedB, probGreater0States, prob0Value, solver.getPrecision(), solver.getRelative());
                
                solver.setPolicyTracking();
                bool resultCorrect = false;
                while(!resultCorrect){
                    solver.solveEquationSystem(goal, x, b);
                    policy = solver.getPolicy();
                    
                    //Check if the policy makes choices that lead to states from which no target state is reachable ("prob0"-states). 
                    getInducedEquationSystem(solver, b, policy, targetChoices, inducedA, inducedB, probGreater0States);
                    resultCorrect = checkAndFixPolicy(solver, x, b, policy, targetChoices, inducedA, inducedB, probGreater0States);
                    
                    if(!resultCorrect){
                        //If the policy could not be fixed, it indicates that our guessed values were to high.
                        STORM_LOG_WARN("Policy could not be fixed. Restarting MinMaxsolver." );
                        solveLinearEquationSystem(inducedA, x, inducedB, probGreater0States, prob0Value, solver.getPrecision(), solver.getRelative());
                    }
                }
                
                
            }
            
            template <typename ValueType>
            void getInducedEquationSystem(storm::solver::GameSolver<ValueType> const& solver,
                                            std::vector<ValueType> const& b,
                                            std::vector<storm::storage::sparse::state_type> const& player1Policy, 
                                            std::vector<storm::storage::sparse::state_type> const& player2Policy,
                                            storm::storage::BitVector const& targetChoices,
                                            storm::storage::SparseMatrix<ValueType>& inducedA,
                                            std::vector<ValueType>& inducedB,
                                            storm::storage::BitVector& probGreater0States
                                        ){
                uint_fast64_t numberOfPlayer1States = solver.getPlayer1Matrix().getRowGroupCount();
                
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
                inducedA = solver.getPlayer2Matrix().selectRowsFromRowIndexSequence(selectedRows, false);
                inducedB = std::vector<ValueType>(numberOfPlayer1States);
                storm::utility::vector::selectVectorValues<ValueType>(inducedB, selectedRows, b);
                storm::storage::BitVector inducedTarget(numberOfPlayer1States, false);
                for (uint_fast64_t pl1State = 0; pl1State < numberOfPlayer1States; ++pl1State){
                    if(targetChoices.get(selectedRows[pl1State])){
                        inducedTarget.set(pl1State);
                    }
                }
                //Find the states from which no target state is reachable.
                probGreater0States = storm::utility::graph::performProbGreater0(inducedA.transpose(), storm::storage::BitVector(numberOfPlayer1States, true), inducedTarget);
            }
            
            
            template <typename ValueType>
            void getInducedEquationSystem(storm::solver::MinMaxLinearEquationSolver<ValueType> const& solver,
                                            std::vector<ValueType> const& b,
                                            std::vector<storm::storage::sparse::state_type> const& policy,
                                            storm::storage::BitVector const& targetChoices,
                                            storm::storage::SparseMatrix<ValueType>& inducedA,
                                            std::vector<ValueType>& inducedB,
                                            storm::storage::BitVector& probGreater0States
                                        ){
                uint_fast64_t numberOfStates = solver.getMatrix().getRowGroupCount();
                
                //Get the matrix A, vector b, and the targetStates induced by the policy
                inducedA = solver.getMatrix().selectRowsFromRowGroups(policy, false);
                inducedB = std::vector<ValueType>(numberOfStates);
                storm::utility::vector::selectVectorValues<ValueType>(inducedB, policy, solver.getMatrix().getRowGroupIndices(), b);
                storm::storage::BitVector inducedTarget(numberOfStates, false);
                for (uint_fast64_t state = 0; state < numberOfStates; ++state){
                    if(targetChoices.get(solver.getMatrix().getRowGroupIndices()[state] + policy[state])){
                        inducedTarget.set(state);
                    }
                }
                //Find the states from which no target state is reachable.
                probGreater0States = storm::utility::graph::performProbGreater0(inducedA.transpose(), storm::storage::BitVector(numberOfStates, true), inducedTarget);
            }
            
            template<typename ValueType>
            void solveLinearEquationSystem(storm::storage::SparseMatrix<ValueType>const& A,
                                           std::vector<ValueType>& x,
                                           std::vector<ValueType> const& b,
                                           storm::storage::BitVector const& probGreater0States,
                                           ValueType const& prob0Value,
                                           ValueType const& precision,
                                           bool relative
                                        ){
                //Get the submatrix/subvector A,x, and b and invoke linear equation solver
                storm::storage::SparseMatrix<ValueType> subA = A.getSubmatrix(true, probGreater0States, probGreater0States, true);
                storm::storage::SparseMatrix<ValueType> eqSysA(subA);
                eqSysA.convertToEquationSystem();
                std::vector<ValueType> subX(probGreater0States.getNumberOfSetBits());
                storm::utility::vector::selectVectorValues(subX, probGreater0States, x);
                std::vector<ValueType> subB(probGreater0States.getNumberOfSetBits());
                storm::utility::vector::selectVectorValues(subB, probGreater0States, b);
                std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> linEqSysSolver = storm::utility::solver::LinearEquationSolverFactory<ValueType>().create(eqSysA);
                linEqSysSolver->setRelative(relative);
                
                std::size_t iterations = 0;
                std::vector<ValueType> copyX(subX.size());
                ValueType newPrecision = precision;
                do {
                    linEqSysSolver->setPrecision(newPrecision);
                    if(!linEqSysSolver->solveEquationSystem(subX, subB)){
                        break; //Solver did not converge.. so we have to go on with the current solution.
                    }
                    subA.multiplyWithVector(subX,copyX);
                    storm::utility::vector::addVectors(copyX, subB, copyX); // = Ax + b
                    ++iterations;
                    newPrecision *= 0.1;
                } while(!storm::utility::vector::equalModuloPrecision(subX, copyX, precision, relative) && iterations<30);
                
                STORM_LOG_DEBUG("Required to increase the precision " << iterations << " times in order to obtain a precise result");
                //fill in the result
                storm::utility::vector::setVectorValues(x, probGreater0States, subX);
                storm::utility::vector::setVectorValues(x, (~probGreater0States), prob0Value);
            }
            
            
            template <typename ValueType>
            bool checkAndFixPolicy(storm::solver::GameSolver<ValueType> const& solver,
                                                    std::vector<ValueType> const& x,
                                                    std::vector<ValueType> const& b,
                                                    std::vector<storm::storage::sparse::state_type>& player1Policy,
                                                    std::vector<storm::storage::sparse::state_type>& player2Policy,
                                                    storm::storage::BitVector const& targetChoices,
                                                    storm::storage::SparseMatrix<ValueType>& inducedA,
                                                    std::vector<ValueType>& inducedB,
                                                    storm::storage::BitVector& probGreater0States
                                        ){
                if(probGreater0States.getNumberOfSetBits() == probGreater0States.size()) return true;
                
                bool policyChanged = true;
                while(policyChanged){
                    /*
                     * Lets try to fix the issue by doing other choices that are equally good.
                     * We change the policy in a state if the following conditions apply:
                     * 1. The current choice does not lead to target
                     * 2. There is another choice that leads to target
                     * 3. The value of that choice is equal to the value of the choice given by the policy
                     * Note that the values of the result will not change this way.
                     * We do this until the policy does not change anymore
                     */
                    policyChanged = false;
                    //Player 1:
                    for(uint_fast64_t pl1State=0; pl1State < player1Policy.size(); ++pl1State){
                        uint_fast64_t pl1RowGroupIndex = solver.getPlayer1Matrix().getRowGroupIndices()[pl1State];
                        //Check 1.: The current choice does not lead to target
                        if(!probGreater0States.get(pl1State)){
                            //1. Is satisfied. Check 2.: There is another choice that leads to target
                            ValueType choiceValue = x[pl1State];
                            for(uint_fast64_t otherChoice = 0; otherChoice < solver.getPlayer1Matrix().getRowGroupSize(pl1State); ++otherChoice){
                                if(otherChoice == player1Policy[pl1State]) continue;
                                //the otherChoice selects a player2 state in which player2 makes his choice (according to the player2Policy).
                                uint_fast64_t pl2State = solver.getPlayer1Matrix().getRow(pl1RowGroupIndex + otherChoice).begin()->getColumn();
                                uint_fast64_t pl2Row = solver.getPlayer2Matrix().getRowGroupIndices()[pl2State] + player2Policy[pl2State];
                                if(rowLeadsToTarget(pl2Row, solver.getPlayer2Matrix(), targetChoices, probGreater0States)){
                                    //2. is satisfied. Check 3. The value of that choice is equal to the value of the choice given by the policy
                                    ValueType otherValue = solver.getPlayer2Matrix().multiplyRowWithVector(pl2Row, x) + b[pl2Row];
                                    if(storm::utility::vector::equalModuloPrecision(choiceValue, otherValue, solver.getPrecision(), !solver.getRelative())){
                                        //3. is satisfied.
                                        player1Policy[pl1State] = otherChoice;
                                        probGreater0States.set(pl1State);
                                        policyChanged = true;
                                        break; //no need to check other choices
                                    }
                                }
                            }
                        }
                    }
                    //update probGreater0States
                    probGreater0States = storm::utility::graph::performProbGreater0(inducedA.transpose(), storm::storage::BitVector(probGreater0States.size(), true), probGreater0States);
                    //Player 2:
                    for(uint_fast64_t pl2State=0; pl2State < player2Policy.size(); ++pl2State){
                        uint_fast64_t pl2RowGroupIndex = solver.getPlayer2Matrix().getRowGroupIndices()[pl2State];
                        //Check 1.: The current choice does not lead to target
                        if(!rowLeadsToTarget(pl2RowGroupIndex + player2Policy[pl2State], solver.getPlayer2Matrix(), targetChoices, probGreater0States)){
                            //1. Is satisfied. Check 2. There is another choice that leads to target
                            ValueType choiceValue = solver.getPlayer2Matrix().multiplyRowWithVector(pl2RowGroupIndex + player2Policy[pl2State], x) + b[pl2RowGroupIndex + player2Policy[pl2State]];
                            for(uint_fast64_t otherChoice = 0; otherChoice < solver.getPlayer2Matrix().getRowGroupSize(pl2State); ++otherChoice){
                                if(otherChoice == player2Policy[pl2State]) continue;
                                if(rowLeadsToTarget(pl2RowGroupIndex + otherChoice, solver.getPlayer2Matrix(), targetChoices, probGreater0States)){
                                    //2. is satisfied. Check 3. The value of that choice is equal to the value of the choice given by the policy
                                    ValueType otherValue = solver.getPlayer2Matrix().multiplyRowWithVector(pl2RowGroupIndex + otherChoice, x) + b[pl2RowGroupIndex + otherChoice];
                                    if(storm::utility::vector::equalModuloPrecision(choiceValue, otherValue, solver.getPrecision(), solver.getRelative())){
                                        //3. is satisfied.
                                        player2Policy[pl2State] = otherChoice;
                                        policyChanged = true;
                                        break; //no need to check other choices
                                    }
                                }
                            }
                        }
                    }
                    
                    //update probGreater0States
                    getInducedEquationSystem(solver, b, player1Policy, player2Policy, targetChoices, inducedA, inducedB, probGreater0States);
                    if(probGreater0States.getNumberOfSetBits() == probGreater0States.size()){
                        return true;
                    }
                }
                //Reaching this point means that the policy does not change anymore and we could not fix it.
                return false;
            }
            
            template <typename ValueType>
            bool checkAndFixPolicy(storm::solver::MinMaxLinearEquationSolver<ValueType> const& solver,
                                    std::vector<ValueType> const& x,
                                    std::vector<ValueType> const& b,
                                    std::vector<storm::storage::sparse::state_type>& policy,
                                    storm::storage::BitVector const& targetChoices,
                                    storm::storage::SparseMatrix<ValueType>& inducedA,
                                    std::vector<ValueType>& inducedB,
                                    storm::storage::BitVector& probGreater0States
                                ){
                if(probGreater0States.getNumberOfSetBits() == probGreater0States.size()) return true;
                
                bool policyChanged = true;
                while(policyChanged){
                    /*
                     * Lets try to fix the issue by doing other choices that are equally good.
                     * We change the policy in a state if the following conditions apply:
                     * 1. The current choice does not lead to target
                     * 2. There is another choice that leads to target
                     * 3. The value of that choice is equal to the value of the choice given by the policy
                     * Note that the values of the result will not change this way.
                     * We do this unil the policy does not change anymore
                     */
                    policyChanged = false;
                    for(uint_fast64_t state=0; state < policy.size(); ++state){
                        uint_fast64_t rowGroupIndex = solver.getMatrix().getRowGroupIndices()[state];
                        //Check 1.: The current choice does not lead to target
                        if(!probGreater0States.get(state)){
                            //1. Is satisfied. Check 2.: There is another choice that leads to target
                            ValueType choiceValue = x[state];
                            for(uint_fast64_t otherChoice = 0; otherChoice < solver.getMatrix().getRowGroupSize(state); ++otherChoice){
                                if(otherChoice == policy[state]) continue;
                                if(rowLeadsToTarget(rowGroupIndex + otherChoice, solver.getMatrix(), targetChoices, probGreater0States)){
                                    //2. is satisfied. Check 3. The value of that choice is equal to the value of the choice given by the policy
                                    ValueType otherValue = solver.getMatrix().multiplyRowWithVector(rowGroupIndex + otherChoice, x) + b[rowGroupIndex + otherChoice];
                                    if(storm::utility::vector::equalModuloPrecision(choiceValue, otherValue, solver.getPrecision(), !solver.getRelative())){
                                        //3. is satisfied.
                                        policy[state] = otherChoice;
                                        probGreater0States.set(state);
                                        policyChanged = true;
                                        break; //no need to check other choices
                                    }
                                }
                            }
                        }
                    }
                    
                    //update probGreater0States and equation system
                    getInducedEquationSystem(solver, b, policy, targetChoices, inducedA, inducedB, probGreater0States);
                    if(probGreater0States.getNumberOfSetBits() == probGreater0States.size()){
                        return true;
                    }
                }
                //Reaching this point means that the policy does not change anymore and we could not fix it.
                return false;
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
            
            template void solveMinMaxLinearEquationSystem<double>( storm::solver::MinMaxLinearEquationSolver<double>& solver,
                                std::vector<double>& x,
                                std::vector<double> const& b,
                                OptimizationDirection goal,
                                std::vector<storm::storage::sparse::state_type>& policy,
                                storm::storage::BitVector const& targetChoices,
                                double const& prob0Value
                            );
            
            template void getInducedEquationSystem<double>(storm::solver::GameSolver<double> const& solver,
                                                    std::vector<double> const& b,
                                                    std::vector<storm::storage::sparse::state_type> const& player1Policy,
                                                    std::vector<storm::storage::sparse::state_type> const& player2Policy,
                                                    storm::storage::BitVector const& targetChoices,
                                                    storm::storage::SparseMatrix<double>& inducedA,
                                                    std::vector<double>& inducedB,
                                                    storm::storage::BitVector& probGreater0States
                            );
            
            template void getInducedEquationSystem<double>(storm::solver::MinMaxLinearEquationSolver<double> const& solver,
                                            std::vector<double> const& b,
                                            std::vector<storm::storage::sparse::state_type> const& policy,
                                            storm::storage::BitVector const& targetChoices,
                                            storm::storage::SparseMatrix<double>& inducedA,
                                            std::vector<double>& inducedB,
                                            storm::storage::BitVector& probGreater0States
                            );
            
            template void solveLinearEquationSystem<double>(storm::storage::SparseMatrix<double>const& A,
                                           std::vector<double>& x,
                                           std::vector<double> const& b,
                                           storm::storage::BitVector const& probGreater0States,
                                           double const& prob0Value,
                                           double const& precision,
                                           bool relative
                            );
            
            template bool checkAndFixPolicy<double>(storm::solver::GameSolver<double> const& solver,
                                            std::vector<double> const& x,
                                            std::vector<double> const& b,
                                            std::vector<storm::storage::sparse::state_type>& player1Policy,
                                            std::vector<storm::storage::sparse::state_type>& player2Policy,
                                            storm::storage::BitVector const& targetChoices,
                                            storm::storage::SparseMatrix<double>& inducedA,
                                            std::vector<double>& inducedB,
                                            storm::storage::BitVector& probGreater0States
                                        );
            
            template bool checkAndFixPolicy<double>(storm::solver::MinMaxLinearEquationSolver<double> const& solver,
                                    std::vector<double> const& x,
                                    std::vector<double> const& b,
                                    std::vector<storm::storage::sparse::state_type>& policy,
                                    storm::storage::BitVector const& targetChoices,
                                    storm::storage::SparseMatrix<double>& inducedA,
                                    std::vector<double>& inducedB,
                                    storm::storage::BitVector& probGreater0States
                                );
                
        }
    }
}
