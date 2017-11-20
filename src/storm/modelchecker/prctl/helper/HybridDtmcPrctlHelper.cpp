#include "storm/modelchecker/prctl/helper/HybridDtmcPrctlHelper.h"

#include "storm/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"

#include "storm/solver/LinearEquationSolver.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"
#include "storm/storage/dd/Odd.h"

#include "storm/utility/graph.h"
#include "storm/utility/constants.h"

#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "storm/modelchecker/results/HybridQuantitativeCheckResult.h"

#include "storm/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {

            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> HybridDtmcPrctlHelper<DdType, ValueType>::computeUntilProbabilities(Environment const& env, storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                // We need to identify the states which have to be taken out of the matrix, i.e. all states that have
                // probability 0 and 1 of satisfying the until-formula.
                STORM_LOG_TRACE("Found " << phiStates.getNonZeroCount() << " phi states and " << psiStates.getNonZeroCount() << " psi states.");
                std::pair<storm::dd::Bdd<DdType>, storm::dd::Bdd<DdType>> statesWithProbability01 = storm::utility::graph::performProb01(model, transitionMatrix.notZero(), phiStates, psiStates);
                storm::dd::Bdd<DdType> maybeStates = !statesWithProbability01.first && !statesWithProbability01.second && model.getReachableStates();
                
                STORM_LOG_INFO("Preprocessing: " << statesWithProbability01.first.getNonZeroCount() << " states with probability 0, " << statesWithProbability01.second.getNonZeroCount() << " with probability 1 (" << maybeStates.getNonZeroCount() << " states remaining).");
                
                // Check whether we need to compute exact probabilities for some states.
                if (qualitative) {
                    // Set the values for all maybe-states to 0.5 to indicate that their probability values are neither 0 nor 1.
                    return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), statesWithProbability01.second.template toAdd<ValueType>() + maybeStates.template toAdd<ValueType>() * model.getManager().template getConstant<ValueType>(storm::utility::convertNumber<ValueType>(0.5))));
                } else {
                    // If there are maybe states, we need to solve an equation system.
                    if (!maybeStates.isZero()) {
                        // Create the ODD for the translation between symbolic and explicit storage.
                        storm::dd::Odd odd = maybeStates.createOdd();
                        
                        // Create the matrix and the vector for the equation system.
                        storm::dd::Add<DdType, ValueType> maybeStatesAdd = maybeStates.template toAdd<ValueType>();
                        
                        // Start by cutting away all rows that do not belong to maybe states. Note that this leaves columns targeting
                        // non-maybe states in the matrix.
                        storm::dd::Add<DdType, ValueType> submatrix = transitionMatrix * maybeStatesAdd;
                                                
                        // Then compute the vector that contains the one-step probabilities to a state with probability 1 for all
                        // maybe states.
                        storm::dd::Add<DdType, ValueType> prob1StatesAsColumn = statesWithProbability01.second.template toAdd<ValueType>();
                        prob1StatesAsColumn = prob1StatesAsColumn.swapVariables(model.getRowColumnMetaVariablePairs());
                        storm::dd::Add<DdType, ValueType> subvector = submatrix * prob1StatesAsColumn;
                        subvector = subvector.sumAbstract(model.getColumnVariables());

                        // Check whether we need to create an equation system.
                        bool convertToEquationSystem = linearEquationSolverFactory.getEquationProblemFormat(env) == storm::solver::LinearEquationSolverProblemFormat::EquationSystem;
                        
                        // Finally cut away all columns targeting non-maybe states and potentially convert the matrix
                        // into the matrix needed for solving the equation system (i.e. compute (I-A)).
                        submatrix *= maybeStatesAdd.swapVariables(model.getRowColumnMetaVariablePairs());
                        if (convertToEquationSystem) {
                            submatrix = (model.getRowColumnIdentity() * maybeStatesAdd) - submatrix;
                        }
                        
                        // Create the solution vector.
                        std::vector<ValueType> x(maybeStates.getNonZeroCount(), storm::utility::convertNumber<ValueType>(0.5));
                        
                        // Translate the symbolic matrix/vector to their explicit representations and solve the equation system.
                        storm::storage::SparseMatrix<ValueType> explicitSubmatrix = submatrix.toMatrix(odd, odd);
                        std::vector<ValueType> b = subvector.toVector(odd);
                            
                        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(env, std::move(explicitSubmatrix));
                        solver->setBounds(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>());
                        solver->solveEquations(env, x, b);
                        
                        // Return a hybrid check result that stores the numerical values explicitly.
                        return std::unique_ptr<CheckResult>(new storm::modelchecker::HybridQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), model.getReachableStates() && !maybeStates, statesWithProbability01.second.template toAdd<ValueType>(), maybeStates, odd, x));
                    } else {
                        return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), statesWithProbability01.second.template toAdd<ValueType>()));
                    }
                }
            }

            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> HybridDtmcPrctlHelper<DdType, ValueType>::computeGloballyProbabilities(Environment const& env, storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                std::unique_ptr<CheckResult> result = computeUntilProbabilities(env, model, transitionMatrix, model.getReachableStates(), !psiStates && model.getReachableStates(), qualitative, linearEquationSolverFactory);
                result->asQuantitativeCheckResult<ValueType>().oneMinus();
                return result;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> HybridDtmcPrctlHelper<DdType, ValueType>::computeNextProbabilities(Environment const& env, storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& nextStates) {
                storm::dd::Add<DdType, ValueType> result = transitionMatrix * nextStates.swapVariables(model.getRowColumnMetaVariablePairs()).template toAdd<ValueType>();
                return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), result.sumAbstract(model.getColumnVariables())));
            }

            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> HybridDtmcPrctlHelper<DdType, ValueType>::computeBoundedUntilProbabilities(Environment const& env, storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, uint_fast64_t stepBound, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                // We need to identify the states which have to be taken out of the matrix, i.e. all states that have
                // probability 0 or 1 of satisfying the until-formula.
                storm::dd::Bdd<DdType> statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0(model, transitionMatrix.notZero(), phiStates, psiStates, stepBound);
                storm::dd::Bdd<DdType> maybeStates = statesWithProbabilityGreater0 && !psiStates && model.getReachableStates();
                
                STORM_LOG_INFO("Preprocessing: " << statesWithProbabilityGreater0.getNonZeroCount() << " states with probability greater 0.");

                // If there are maybe states, we need to perform matrix-vector multiplications.
                if (!maybeStates.isZero()) {
                    // Create the ODD for the translation between symbolic and explicit storage.
                    storm::dd::Odd odd = maybeStates.createOdd();
                    
                    // Create the matrix and the vector for the equation system.
                    storm::dd::Add<DdType, ValueType> maybeStatesAdd = maybeStates.template toAdd<ValueType>();
                    
                    // Start by cutting away all rows that do not belong to maybe states. Note that this leaves columns targeting
                    // non-maybe states in the matrix.
                    storm::dd::Add<DdType, ValueType> submatrix = transitionMatrix * maybeStatesAdd;
                    
                    // Then compute the vector that contains the one-step probabilities to a state with probability 1 for all
                    // maybe states.
                    storm::dd::Add<DdType, ValueType> prob1StatesAsColumn = psiStates.template toAdd<ValueType>().swapVariables(model.getRowColumnMetaVariablePairs());
                    storm::dd::Add<DdType, ValueType> subvector = (submatrix * prob1StatesAsColumn).sumAbstract(model.getColumnVariables());
                    
                    // Finally cut away all columns targeting non-maybe states.
                    submatrix *= maybeStatesAdd.swapVariables(model.getRowColumnMetaVariablePairs());
                    
                    // Create the solution vector.
                    std::vector<ValueType> x(maybeStates.getNonZeroCount(), storm::utility::zero<ValueType>());
                    
                    // Translate the symbolic matrix/vector to their explicit representations.
                    storm::storage::SparseMatrix<ValueType> explicitSubmatrix = submatrix.toMatrix(odd, odd);
                    std::vector<ValueType> b = subvector.toVector(odd);
                    
                    std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(env, std::move(explicitSubmatrix));
                    solver->repeatedMultiply(x, &b, stepBound);
                    
                    // Return a hybrid check result that stores the numerical values explicitly.
                    return std::unique_ptr<CheckResult>(new storm::modelchecker::HybridQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), model.getReachableStates() && !maybeStates, psiStates.template toAdd<ValueType>(), maybeStates, odd, x));
                } else {
                    return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), psiStates.template toAdd<ValueType>()));
                }
            }

            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> HybridDtmcPrctlHelper<DdType, ValueType>::computeInstantaneousRewards(Environment const& env, storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                // Only compute the result if the model has at least one reward this->getModel().
                STORM_LOG_THROW(rewardModel.hasStateRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                // Create the ODD for the translation between symbolic and explicit storage.
                storm::dd::Odd odd = model.getReachableStates().createOdd();
                
                // Create the solution vector (and initialize it to the state rewards of the model).
                std::vector<ValueType> x = rewardModel.getStateRewardVector().toVector(odd);
                
                // Translate the symbolic matrix to its explicit representations.
                storm::storage::SparseMatrix<ValueType> explicitMatrix = transitionMatrix.toMatrix(odd, odd);
                
                // Perform the matrix-vector multiplication.
                std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(env, std::move(explicitMatrix));
                solver->repeatedMultiply(x, nullptr, stepBound);
                
                // Return a hybrid check result that stores the numerical values explicitly.
                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), model.getManager().getBddZero(), model.getManager().template getAddZero<ValueType>(), model.getReachableStates(), odd, x));
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> HybridDtmcPrctlHelper<DdType, ValueType>::computeCumulativeRewards(Environment const& env, storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                // Only compute the result if the model has at least one reward this->getModel().
                STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                // Compute the reward vector to add in each step based on the available reward models.
                storm::dd::Add<DdType, ValueType> totalRewardVector = rewardModel.getTotalRewardVector(transitionMatrix, model.getColumnVariables());
                
                // Create the ODD for the translation between symbolic and explicit storage.
                storm::dd::Odd odd = model.getReachableStates().createOdd();
                
                // Create the solution vector.
                std::vector<ValueType> x(model.getNumberOfStates(), storm::utility::zero<ValueType>());
                
                // Translate the symbolic matrix/vector to their explicit representations.
                storm::storage::SparseMatrix<ValueType> explicitMatrix = transitionMatrix.toMatrix(odd, odd);
                std::vector<ValueType> b = totalRewardVector.toVector(odd);
                
                // Perform the matrix-vector multiplication.
                std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(env, std::move(explicitMatrix));
                solver->repeatedMultiply(x, &b, stepBound);
                
                // Return a hybrid check result that stores the numerical values explicitly.
                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), model.getManager().getBddZero(), model.getManager().template getAddZero<ValueType>(), model.getReachableStates(), odd, x));
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> HybridDtmcPrctlHelper<DdType, ValueType>::computeReachabilityRewards(Environment const& env, storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, storm::dd::Bdd<DdType> const& targetStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                
                // Only compute the result if there is at least one reward model.
                STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                // Determine which states have a reward of infinity by definition.
                storm::dd::Bdd<DdType> infinityStates = storm::utility::graph::performProb1(model, transitionMatrix.notZero(), model.getReachableStates(), targetStates);
                infinityStates = !infinityStates && model.getReachableStates();
                storm::dd::Bdd<DdType> maybeStates = (!targetStates && !infinityStates) && model.getReachableStates();

                STORM_LOG_INFO("Preprocessing: " << infinityStates.getNonZeroCount() << " states with reward infinity, " << targetStates.getNonZeroCount() << " target states (" << maybeStates.getNonZeroCount() << " states remaining).");
                
                // Check whether we need to compute exact rewards for some states.
                if (qualitative) {
                    // Set the values for all maybe-states to 1 to indicate that their reward values
                    // are neither 0 nor infinity.
                    return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), infinityStates.ite(model.getManager().getConstant(storm::utility::infinity<ValueType>()), model.getManager().template getAddZero<ValueType>()) + maybeStates.template toAdd<ValueType>() * model.getManager().template getAddOne<ValueType>()));
                } else {
                    // If there are maybe states, we need to solve an equation system.
                    if (!maybeStates.isZero()) {
                        // Create the ODD for the translation between symbolic and explicit storage.
                        storm::dd::Odd odd = maybeStates.createOdd();
                        
                        // Create the matrix and the vector for the equation system.
                        storm::dd::Add<DdType, ValueType> maybeStatesAdd = maybeStates.template toAdd<ValueType>();
                        
                        // Start by cutting away all rows that do not belong to maybe states. Note that this leaves columns targeting
                        // non-maybe states in the matrix.
                        storm::dd::Add<DdType, ValueType> submatrix = transitionMatrix * maybeStatesAdd;
                        
                        // Then compute the state reward vector to use in the computation.
                        storm::dd::Add<DdType, ValueType> subvector = rewardModel.getTotalRewardVector(maybeStatesAdd, submatrix, model.getColumnVariables());

                        // Check whether we need to create an equation system.
                        bool convertToEquationSystem = linearEquationSolverFactory.getEquationProblemFormat(env) == storm::solver::LinearEquationSolverProblemFormat::EquationSystem;
                        
                        // Finally cut away all columns targeting non-maybe states and potentially convert the matrix
                        // into the matrix needed for solving the equation system (i.e. compute (I-A)).
                        submatrix *= maybeStatesAdd.swapVariables(model.getRowColumnMetaVariablePairs());
                        if (convertToEquationSystem) {
                            submatrix = (model.getRowColumnIdentity() * maybeStatesAdd) - submatrix;
                        }
                        
                        // Create the solution vector.
                        std::vector<ValueType> x(maybeStates.getNonZeroCount(), storm::utility::convertNumber<ValueType>(0.5));
                        
                        // Translate the symbolic matrix/vector to their explicit representations.
                        storm::storage::SparseMatrix<ValueType> explicitSubmatrix = submatrix.toMatrix(odd, odd);
                        std::vector<ValueType> b = subvector.toVector(odd);
                        
                        // Now solve the resulting equation system.
                        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(env, std::move(explicitSubmatrix));
                        solver->setLowerBound(storm::utility::zero<ValueType>());
                        solver->solveEquations(env, x, b);
                        
                        // Return a hybrid check result that stores the numerical values explicitly.
                        return std::unique_ptr<CheckResult>(new storm::modelchecker::HybridQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), model.getReachableStates() && !maybeStates, infinityStates.ite(model.getManager().getConstant(storm::utility::infinity<ValueType>()), model.getManager().template getAddZero<ValueType>()), maybeStates, odd, x));
                    } else {
                        return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), infinityStates.ite(model.getManager().getConstant(storm::utility::infinity<ValueType>()), model.getManager().template getAddZero<ValueType>())));
                    }
                }
            }

            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> HybridDtmcPrctlHelper<DdType, ValueType>::computeLongRunAverageProbabilities(Environment const& env, storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& targetStates, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                // Create ODD for the translation.
                storm::dd::Odd odd = model.getReachableStates().createOdd();
                storm::storage::SparseMatrix<ValueType> explicitProbabilityMatrix = model.getTransitionMatrix().toMatrix(odd, odd);

                std::vector<ValueType> result = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeLongRunAverageProbabilities(env, storm::solver::SolveGoal<ValueType>(), explicitProbabilityMatrix, targetStates.toVector(odd), linearEquationSolverFactory);
                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), model.getManager().getBddZero(), model.getManager().template getAddZero<ValueType>(), model.getReachableStates(), std::move(odd), std::move(result)));
            }

            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> HybridDtmcPrctlHelper<DdType, ValueType>::computeLongRunAverageRewards(Environment const& env, storm::models::symbolic::Model<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                // Create ODD for the translation.
                storm::dd::Odd odd = model.getReachableStates().createOdd();
                storm::storage::SparseMatrix<ValueType> explicitProbabilityMatrix = model.getTransitionMatrix().toMatrix(odd, odd);
                
                std::vector<ValueType> result = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeLongRunAverageRewards(env, storm::solver::SolveGoal<ValueType>(), explicitProbabilityMatrix, rewardModel.getTotalRewardVector(model.getTransitionMatrix(), model.getColumnVariables()).toVector(odd), linearEquationSolverFactory);
                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), model.getManager().getBddZero(), model.getManager().template getAddZero<ValueType>(), model.getReachableStates(), std::move(odd), std::move(result)));
            }
            
            template class HybridDtmcPrctlHelper<storm::dd::DdType::CUDD, double>;
            template class HybridDtmcPrctlHelper<storm::dd::DdType::Sylvan, double>;

            template class HybridDtmcPrctlHelper<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class HybridDtmcPrctlHelper<storm::dd::DdType::Sylvan, storm::RationalFunction>;
        }
    }
}
