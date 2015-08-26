#include "src/modelchecker/prctl/helper/SymbolicMdpPrctlHelper.h"

#include "src/solver/SymbolicMinMaxLinearEquationSolver.h"

#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddAdd.h"
#include "src/storage/dd/CuddBdd.h"
#include "src/storage/dd/CuddOdd.h"

#include "src/utility/graph.h"

#include "src/models/symbolic/StandardRewardModel.h"

#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQuantitativeCheckResult.h"

#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> SymbolicMdpPrctlHelper<DdType, ValueType>::computeUntilProbabilities(bool minimize, storm::models::symbolic::NondeterministicModel<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory) {
                // We need to identify the states which have to be taken out of the matrix, i.e. all states that have
                // probability 0 and 1 of satisfying the until-formula.
                std::pair<storm::dd::Bdd<DdType>, storm::dd::Bdd<DdType>> statesWithProbability01;
                if (minimize) {
                    statesWithProbability01 = storm::utility::graph::performProb01Min(model, phiStates, psiStates);
                } else {
                    statesWithProbability01 = storm::utility::graph::performProb01Max(model, phiStates, psiStates);
                }
                
                storm::dd::Bdd<DdType> maybeStates = !statesWithProbability01.first && !statesWithProbability01.second && model.getReachableStates();
                
                // Perform some logging.
                STORM_LOG_INFO("Found " << statesWithProbability01.first.getNonZeroCount() << " 'no' states.");
                STORM_LOG_INFO("Found " << statesWithProbability01.second.getNonZeroCount() << " 'yes' states.");
                STORM_LOG_INFO("Found " << maybeStates.getNonZeroCount() << " 'maybe' states.");
                
                // Check whether we need to compute exact probabilities for some states.
                if (qualitative) {
                    // Set the values for all maybe-states to 0.5 to indicate that their probability values are neither 0 nor 1.
                    return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), statesWithProbability01.second.toAdd() + maybeStates.toAdd() * model.getManager().getConstant(0.5)));
                } else {
                    // If there are maybe states, we need to solve an equation system.
                    if (!maybeStates.isZero()) {
                        // Create the matrix and the vector for the equation system.
                        storm::dd::Add<DdType> maybeStatesAdd = maybeStates.toAdd();
                        
                        // Start by cutting away all rows that do not belong to maybe states. Note that this leaves columns targeting
                        // non-maybe states in the matrix.
                        storm::dd::Add<DdType> submatrix = transitionMatrix * maybeStatesAdd;
                        
                        // Then compute the vector that contains the one-step probabilities to a state with probability 1 for all
                        // maybe states.
                        storm::dd::Add<DdType> prob1StatesAsColumn = statesWithProbability01.second.toAdd();
                        prob1StatesAsColumn = prob1StatesAsColumn.swapVariables(model.getRowColumnMetaVariablePairs());
                        storm::dd::Add<DdType> subvector = submatrix * prob1StatesAsColumn;
                        subvector = subvector.sumAbstract(model.getColumnVariables());
                        
                        // Finally cut away all columns targeting non-maybe states.
                        submatrix *= maybeStatesAdd.swapVariables(model.getRowColumnMetaVariablePairs());
                        
                        // Now solve the resulting equation system.
                        std::unique_ptr<storm::solver::SymbolicMinMaxLinearEquationSolver<DdType, ValueType>> solver = linearEquationSolverFactory.create(submatrix, maybeStates, model.getIllegalMask() && maybeStates, model.getRowVariables(), model.getColumnVariables(), model.getNondeterminismVariables(), model.getRowColumnMetaVariablePairs());
                        storm::dd::Add<DdType> result = solver->solveEquationSystem(minimize, model.getManager().getAddZero(), subvector);
                        
                        return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), statesWithProbability01.second.toAdd() + result));
                    } else {
                        return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), statesWithProbability01.second.toAdd()));
                    }
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> SymbolicMdpPrctlHelper<DdType, ValueType>::computeNextProbabilities(bool minimize, storm::models::symbolic::NondeterministicModel<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Bdd<DdType> const& nextStates) {
                storm::dd::Add<DdType> result = transitionMatrix * nextStates.swapVariables(model.getRowColumnMetaVariablePairs()).toAdd();
                return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), result.sumAbstract(model.getColumnVariables())));
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> SymbolicMdpPrctlHelper<DdType, ValueType>::computeBoundedUntilProbabilities(bool minimize, storm::models::symbolic::NondeterministicModel<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, uint_fast64_t stepBound, storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory) {
                // We need to identify the states which have to be taken out of the matrix, i.e. all states that have
                // probability 0 or 1 of satisfying the until-formula.
                storm::dd::Bdd<DdType> statesWithProbabilityGreater0;
                if (minimize) {
                    statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0A(model, transitionMatrix.notZero(), phiStates, psiStates);
                } else {
                    statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0E(model, transitionMatrix.notZero(), phiStates, psiStates);
                }
                storm::dd::Bdd<DdType> maybeStates = statesWithProbabilityGreater0 && !psiStates && model.getReachableStates();
                
                // If there are maybe states, we need to perform matrix-vector multiplications.
                if (!maybeStates.isZero()) {
                    // Create the matrix and the vector for the equation system.
                    storm::dd::Add<DdType> maybeStatesAdd = maybeStates.toAdd();
                    
                    // Start by cutting away all rows that do not belong to maybe states. Note that this leaves columns targeting
                    // non-maybe states in the matrix.
                    storm::dd::Add<DdType> submatrix = transitionMatrix * maybeStatesAdd;
                    
                    // Then compute the vector that contains the one-step probabilities to a state with probability 1 for all
                    // maybe states.
                    storm::dd::Add<DdType> prob1StatesAsColumn = psiStates.toAdd().swapVariables(model.getRowColumnMetaVariablePairs());
                    storm::dd::Add<DdType> subvector = (submatrix * prob1StatesAsColumn).sumAbstract(model.getColumnVariables());
                    
                    // Finally cut away all columns targeting non-maybe states.
                    submatrix *= maybeStatesAdd.swapVariables(model.getRowColumnMetaVariablePairs());
                    
                    std::unique_ptr<storm::solver::SymbolicMinMaxLinearEquationSolver<DdType, ValueType>> solver = linearEquationSolverFactory.create(submatrix, maybeStates, model.getIllegalMask() && maybeStates, model.getRowVariables(), model.getColumnVariables(), model.getNondeterminismVariables(), model.getRowColumnMetaVariablePairs());
                    storm::dd::Add<DdType> result = solver->performMatrixVectorMultiplication(minimize, model.getManager().getAddZero(), &subvector, stepBound);
                    
                    return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), psiStates.toAdd() + result));
                } else {
                    return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), psiStates.toAdd()));
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> SymbolicMdpPrctlHelper<DdType, ValueType>::computeInstantaneousRewards(bool minimize, storm::models::symbolic::NondeterministicModel<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory) {
                // Only compute the result if the model has at least one reward this->getModel().
                STORM_LOG_THROW(rewardModel.hasStateRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                // Perform the matrix-vector multiplication.
                std::unique_ptr<storm::solver::SymbolicMinMaxLinearEquationSolver<DdType, ValueType>> solver = linearEquationSolverFactory.create(model.getTransitionMatrix(), model.getReachableStates(), model.getIllegalMask(), model.getRowVariables(), model.getColumnVariables(), model.getNondeterminismVariables(), model.getRowColumnMetaVariablePairs());
                storm::dd::Add<DdType> result = solver->performMatrixVectorMultiplication(minimize, rewardModel.getStateRewardVector(), nullptr, stepBound);

                return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), result));
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> SymbolicMdpPrctlHelper<DdType, ValueType>::computeCumulativeRewards(bool minimize, storm::models::symbolic::NondeterministicModel<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory) {
                // Only compute the result if the model has at least one reward this->getModel().
                STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                // Compute the reward vector to add in each step based on the available reward models.
                storm::dd::Add<DdType> totalRewardVector = rewardModel.getTotalRewardVector(transitionMatrix, model.getColumnVariables());
                
                // Perform the matrix-vector multiplication.
                std::unique_ptr<storm::solver::SymbolicMinMaxLinearEquationSolver<DdType, ValueType>> solver = linearEquationSolverFactory.create(model.getTransitionMatrix(), model.getReachableStates(), model.getIllegalMask(), model.getRowVariables(), model.getColumnVariables(), model.getNondeterminismVariables(), model.getRowColumnMetaVariablePairs());
                storm::dd::Add<DdType> result = solver->performMatrixVectorMultiplication(minimize, model.getManager().getAddZero(), &totalRewardVector, stepBound);
                
                return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), result));
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> SymbolicMdpPrctlHelper<DdType, ValueType>::computeReachabilityRewards(bool minimize, storm::models::symbolic::NondeterministicModel<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, RewardModelType const& rewardModel, storm::dd::Bdd<DdType> const& targetStates, bool qualitative, storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType> const& linearEquationSolverFactory) {
                
                // Only compute the result if there is at least one reward model.
                STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                // Determine which states have a reward of infinity by definition.
                storm::dd::Bdd<DdType> infinityStates;
                storm::dd::Bdd<DdType> transitionMatrixBdd = transitionMatrix.notZero();
                if (minimize) {
                    infinityStates = storm::utility::graph::performProb1E(model, transitionMatrixBdd, model.getReachableStates(), targetStates, storm::utility::graph::performProbGreater0E(model, transitionMatrixBdd, model.getReachableStates(), targetStates));
                } else {
                    infinityStates = storm::utility::graph::performProb1A(model, transitionMatrixBdd, model.getReachableStates(), targetStates, storm::utility::graph::performProbGreater0A(model, transitionMatrixBdd, model.getReachableStates(), targetStates));
                }
                infinityStates = !infinityStates && model.getReachableStates();
                
                storm::dd::Bdd<DdType> maybeStates = (!targetStates && !infinityStates) && model.getReachableStates();
                STORM_LOG_INFO("Found " << infinityStates.getNonZeroCount() << " 'infinity' states.");
                STORM_LOG_INFO("Found " << targetStates.getNonZeroCount() << " 'target' states.");
                STORM_LOG_INFO("Found " << maybeStates.getNonZeroCount() << " 'maybe' states.");
                
                // Check whether we need to compute exact rewards for some states.
                if (qualitative) {
                    // Set the values for all maybe-states to 1 to indicate that their reward values
                    // are neither 0 nor infinity.
                    return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), infinityStates.toAdd() * model.getManager().getConstant(storm::utility::infinity<ValueType>()) + maybeStates.toAdd() * model.getManager().getConstant(storm::utility::one<ValueType>())));
                } else {
                    // If there are maybe states, we need to solve an equation system.
                    if (!maybeStates.isZero()) {
                        // Create the matrix and the vector for the equation system.
                        storm::dd::Add<DdType> maybeStatesAdd = maybeStates.toAdd();
                        
                        // Start by cutting away all rows that do not belong to maybe states. Note that this leaves columns targeting
                        // non-maybe states in the matrix.
                        storm::dd::Add<DdType> submatrix = transitionMatrix * maybeStatesAdd;
                        
                        // Then compute the state reward vector to use in the computation.
                        storm::dd::Add<DdType> subvector = rewardModel.getTotalRewardVector(maybeStatesAdd, submatrix, model.getColumnVariables());
                        
                        // Finally cut away all columns targeting non-maybe states.
                        submatrix *= maybeStatesAdd.swapVariables(model.getRowColumnMetaVariablePairs());
                        
                        // Now solve the resulting equation system.
                        std::unique_ptr<storm::solver::SymbolicMinMaxLinearEquationSolver<DdType, ValueType>> solver = linearEquationSolverFactory.create(submatrix, maybeStates, model.getIllegalMask() && maybeStates, model.getRowVariables(), model.getColumnVariables(), model.getNondeterminismVariables(), model.getRowColumnMetaVariablePairs());
                        storm::dd::Add<DdType> result = solver->solveEquationSystem(minimize, model.getManager().getAddZero(), subvector);

                        return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), infinityStates.toAdd() * model.getManager().getConstant(storm::utility::infinity<ValueType>()) + result));
                    } else {
                        return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), infinityStates.toAdd() * model.getManager().getConstant(storm::utility::infinity<ValueType>())));
                    }
                }
            }
            
            template class SymbolicMdpPrctlHelper<storm::dd::DdType::CUDD, double>;
        }
    }
}