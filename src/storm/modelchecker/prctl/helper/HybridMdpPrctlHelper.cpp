#include "storm/modelchecker/prctl/helper/HybridMdpPrctlHelper.h"

#include "storm/modelchecker/prctl/helper/SymbolicMdpPrctlHelper.h"

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

#include "storm/solver/MinMaxLinearEquationSolver.h"

#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/UncheckedRequirementException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <typename ValueType>
            std::vector<uint64_t> computeValidInitialSchedulerForUntilProbabilities(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& b) {
                uint64_t numberOfMaybeStates = transitionMatrix.getRowGroupCount();
                std::vector<uint64_t> result(numberOfMaybeStates);
                storm::storage::BitVector targetStates(numberOfMaybeStates);
                
                for (uint64_t state = 0; state < numberOfMaybeStates; ++state) {
                    // Record all states with non-zero probability of moving directly to the target states.
                    for (uint64_t row = transitionMatrix.getRowGroupIndices()[state]; row < transitionMatrix.getRowGroupIndices()[state + 1]; ++row) {
                        if (!storm::utility::isZero(b[row])) {
                            targetStates.set(state);
                            result[state] = row - transitionMatrix.getRowGroupIndices()[state];
                        }
                    }
                }
                
                if (!targetStates.full()) {
                    storm::storage::Scheduler<ValueType> validScheduler(numberOfMaybeStates);
                    storm::storage::SparseMatrix<ValueType> backwardTransitions = transitionMatrix.transpose(true);
                    storm::utility::graph::computeSchedulerProbGreater0E(transitionMatrix, backwardTransitions, storm::storage::BitVector(numberOfMaybeStates, true), targetStates, validScheduler, boost::none);
                    
                    for (uint64_t state = 0; state < numberOfMaybeStates; ++state) {
                        if (!targetStates.get(state)) {
                            result[state] = validScheduler.getChoice(state).getDeterministicChoice();
                        }
                    }
                }
                
                return result;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> HybridMdpPrctlHelper<DdType, ValueType>::computeUntilProbabilities(OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                // We need to identify the states which have to be taken out of the matrix, i.e. all states that have
                // probability 0 and 1 of satisfying the until-formula.
                std::pair<storm::dd::Bdd<DdType>, storm::dd::Bdd<DdType>> statesWithProbability01;
                if (dir == OptimizationDirection::Minimize) {
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
                    return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), statesWithProbability01.second.template toAdd<ValueType>() + maybeStates.template toAdd<ValueType>() * model.getManager().getConstant(storm::utility::convertNumber<ValueType>(0.5))));
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
                        
                        // Before cutting the non-maybe columns, we need to compute the sizes of the row groups.
                        std::vector<uint_fast64_t> rowGroupSizes = submatrix.notZero().existsAbstract(model.getColumnVariables()).template toAdd<uint_fast64_t>().sumAbstract(model.getNondeterminismVariables()).toVector(odd);
                        
                        // Finally cut away all columns targeting non-maybe states.
                        submatrix *= maybeStatesAdd.swapVariables(model.getRowColumnMetaVariablePairs());
                        
                        // Translate the symbolic matrix/vector to their explicit representations and solve the equation system.
                        std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>> explicitRepresentation = submatrix.toMatrixVector(subvector, std::move(rowGroupSizes), model.getNondeterminismVariables(), odd, odd);
                        
                        // Create the solution vector.
                        std::vector<ValueType> x(explicitRepresentation.first.getRowGroupCount(), storm::utility::zero<ValueType>());

                        // Check for requirements of the solver.
                        storm::solver::MinMaxLinearEquationSolverRequirements requirements = linearEquationSolverFactory.getRequirements(storm::solver::EquationSystemType::UntilProbabilities, dir);
                        boost::optional<std::vector<uint64_t>> initialScheduler;
                        if (!requirements.empty()) {
                            if (requirements.requires(storm::solver::MinMaxLinearEquationSolverRequirements::Element::ValidInitialScheduler)) {
                                STORM_LOG_DEBUG("Computing valid scheduler hint, because the solver requires it.");
                                initialScheduler = computeValidInitialSchedulerForUntilProbabilities<ValueType>(explicitRepresentation.first, explicitRepresentation.second);

                                requirements.clearValidInitialScheduler();
                            }
                            STORM_LOG_THROW(requirements.empty(), storm::exceptions::UncheckedRequirementException, "Cannot establish requirements for solver.");
                        }
                        
                        std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(std::move(explicitRepresentation.first));
                        if (initialScheduler) {
                            solver->setInitialScheduler(std::move(initialScheduler.get()));
                        }
                        solver->setBounds(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>());
                        solver->setRequirementsChecked();
                        solver->solveEquations(dir, x, explicitRepresentation.second);
                        
                        // Return a hybrid check result that stores the numerical values explicitly.
                        return std::unique_ptr<CheckResult>(new storm::modelchecker::HybridQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), model.getReachableStates() && !maybeStates, statesWithProbability01.second.template toAdd<ValueType>(), maybeStates, odd, x));
                    } else {
                        return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), statesWithProbability01.second.template toAdd<ValueType>()));
                    }
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> HybridMdpPrctlHelper<DdType, ValueType>::computeGloballyProbabilities(OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                std::unique_ptr<CheckResult> result = computeUntilProbabilities(dir == OptimizationDirection::Minimize ? OptimizationDirection::Maximize : OptimizationDirection::Maximize, model, transitionMatrix, model.getReachableStates(), !psiStates && model.getReachableStates(), qualitative, linearEquationSolverFactory);
                result->asQuantitativeCheckResult<ValueType>().oneMinus();
                return result;
            }

            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> HybridMdpPrctlHelper<DdType, ValueType>::computeNextProbabilities(OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& nextStates) {
                return SymbolicMdpPrctlHelper<DdType, ValueType>::computeNextProbabilities(dir, model, transitionMatrix, nextStates);
            }

            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> HybridMdpPrctlHelper<DdType, ValueType>::computeBoundedUntilProbabilities(OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, uint_fast64_t stepBound, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                // We need to identify the states which have to be taken out of the matrix, i.e. all states that have
                // probability 0 or 1 of satisfying the until-formula.
                storm::dd::Bdd<DdType> statesWithProbabilityGreater0;
                if (dir == OptimizationDirection::Minimize) {
                    statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0A(model, transitionMatrix.notZero(), phiStates, psiStates);
                } else {
                    statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0E(model, transitionMatrix.notZero(), phiStates, psiStates);
                }
                storm::dd::Bdd<DdType> maybeStates = statesWithProbabilityGreater0 && !psiStates && model.getReachableStates();
                
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
                    
                    // Before cutting the non-maybe columns, we need to compute the sizes of the row groups.
                    std::vector<uint_fast64_t> rowGroupSizes = submatrix.notZero().existsAbstract(model.getColumnVariables()).template toAdd<uint_fast64_t>().sumAbstract(model.getNondeterminismVariables()).toVector(odd);
                    
                    // Finally cut away all columns targeting non-maybe states.
                    submatrix *= maybeStatesAdd.swapVariables(model.getRowColumnMetaVariablePairs());
                    
                    // Create the solution vector.
                    std::vector<ValueType> x(maybeStates.getNonZeroCount(), storm::utility::zero<ValueType>());
                    
                    // Translate the symbolic matrix/vector to their explicit representations.
                    std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>> explicitRepresentation = submatrix.toMatrixVector(subvector, std::move(rowGroupSizes), model.getNondeterminismVariables(), odd, odd);
                    
                    std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(std::move(explicitRepresentation.first));
                    solver->repeatedMultiply(dir, x, &explicitRepresentation.second, stepBound);
                    
                    // Return a hybrid check result that stores the numerical values explicitly.
                    return std::unique_ptr<CheckResult>(new storm::modelchecker::HybridQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), model.getReachableStates() && !maybeStates, psiStates.template toAdd<ValueType>(), maybeStates, odd, x));
                } else {
                    return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), psiStates.template toAdd<ValueType>()));
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> HybridMdpPrctlHelper<DdType, ValueType>::computeInstantaneousRewards(OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                // Only compute the result if the model has at least one reward this->getModel().
                STORM_LOG_THROW(rewardModel.hasStateRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                // Create the ODD for the translation between symbolic and explicit storage.
                storm::dd::Odd odd = model.getReachableStates().createOdd();
                
                // Translate the symbolic matrix to its explicit representations.
                storm::storage::SparseMatrix<ValueType> explicitMatrix = transitionMatrix.toMatrix(model.getNondeterminismVariables(), odd, odd);
                
                // Create the solution vector (and initialize it to the state rewards of the model).
                std::vector<ValueType> x = rewardModel.getStateRewardVector().toVector(odd);
                
                // Perform the matrix-vector multiplication.
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(std::move(explicitMatrix));
                solver->repeatedMultiply(dir, x, nullptr, stepBound);
                
                // Return a hybrid check result that stores the numerical values explicitly.
                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), model.getManager().getBddZero(), model.getManager().template getAddZero<ValueType>(), model.getReachableStates(), odd, x));
            }

            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> HybridMdpPrctlHelper<DdType, ValueType>::computeCumulativeRewards(OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                // Only compute the result if the model has at least one reward this->getModel().
                STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                // Compute the reward vector to add in each step based on the available reward models.
                storm::dd::Add<DdType, ValueType> totalRewardVector = rewardModel.getTotalRewardVector(transitionMatrix, model.getColumnVariables());
                
                // Create the ODD for the translation between symbolic and explicit storage.
                storm::dd::Odd odd = model.getReachableStates().createOdd();
                
                // Create the solution vector.
                std::vector<ValueType> x(model.getNumberOfStates(), storm::utility::zero<ValueType>());
                
                // Before cutting the non-maybe columns, we need to compute the sizes of the row groups.
                storm::dd::Add<DdType, uint_fast64_t> stateActionAdd = (transitionMatrix.notZero().existsAbstract(model.getColumnVariables()) || totalRewardVector.notZero()).template toAdd<uint_fast64_t>();
                std::vector<uint_fast64_t> rowGroupSizes = stateActionAdd.sumAbstract(model.getNondeterminismVariables()).toVector(odd);
                
                // Translate the symbolic matrix/vector to their explicit representations.
                std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>> explicitRepresentation = transitionMatrix.toMatrixVector(totalRewardVector, std::move(rowGroupSizes), model.getNondeterminismVariables(), odd, odd);
                
                // Perform the matrix-vector multiplication.
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(std::move(explicitRepresentation.first));
                solver->repeatedMultiply(dir, x, &explicitRepresentation.second, stepBound);
                
                // Return a hybrid check result that stores the numerical values explicitly.
                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), model.getManager().getBddZero(), model.getManager().template getAddZero<ValueType>(), model.getReachableStates(), odd, x));
            }

            template <typename ValueType>
            storm::storage::BitVector computeTargetStatesForReachabilityRewardsFromExplicitRepresentation(storm::storage::SparseMatrix<ValueType> const& transitionMatrix) {
                storm::storage::BitVector targetStates(transitionMatrix.getRowGroupCount());
                
                // A state is a target state if its row group is empty.
                for (uint64_t rowGroup = 0; rowGroup < transitionMatrix.getRowGroupCount(); ++rowGroup) {
                    if (transitionMatrix.getRowGroupIndices()[rowGroup] == transitionMatrix.getRowGroupIndices()[rowGroup + 1]) {
                        targetStates.set(rowGroup);
                    }
                }
                
                return targetStates;
            }
            
            template <typename ValueType>
            std::vector<uint64_t> computeValidInitialSchedulerForReachabilityRewards(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& properMaybeStates, storm::storage::BitVector const& targetStates) {
                uint64_t numberOfMaybeStates = transitionMatrix.getRowGroupCount();
                std::vector<uint64_t> result(numberOfMaybeStates);

                storm::storage::Scheduler<ValueType> validScheduler(numberOfMaybeStates);
                storm::storage::SparseMatrix<ValueType> backwardTransitions = transitionMatrix.transpose(true);
                storm::utility::graph::computeSchedulerProb1E(storm::storage::BitVector(numberOfMaybeStates, true), transitionMatrix, backwardTransitions, properMaybeStates, targetStates, validScheduler);
                
                for (uint64_t state = 0; state < numberOfMaybeStates; ++state) {
                    if (!targetStates.get(state)) {
                        result[state] = validScheduler.getChoice(state).getDeterministicChoice();
                    }
                }
                
                return result;
            }
            
            template <typename ValueType>
            void eliminateTargetStatesFromExplicitRepresentation(std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>>& explicitRepresentation, std::vector<uint64_t>& scheduler, storm::storage::BitVector const& properMaybeStates) {
                // Eliminate the superfluous entries from the rhs vector and the scheduler.
                uint64_t position = 0;
                for (auto state : properMaybeStates) {
                    for (uint64_t row = explicitRepresentation.first.getRowGroupIndices()[state]; row < explicitRepresentation.first.getRowGroupIndices()[state + 1]; ++row) {
                        explicitRepresentation.second[position] = explicitRepresentation.second[row];
                        position++;
                    }
                }
                explicitRepresentation.second.resize(position);
                explicitRepresentation.second.shrink_to_fit();
                
                position = 0;
                for (auto state : properMaybeStates) {
                    scheduler[position] = scheduler[state];
                    position++;
                }
                scheduler.resize(properMaybeStates.getNumberOfSetBits());
                scheduler.shrink_to_fit();

                // Treat the matrix.
                explicitRepresentation.first = explicitRepresentation.first.getSubmatrix(true, properMaybeStates, properMaybeStates);
            }
            
            template <typename ValueType>
            std::vector<ValueType> insertTargetStateValuesIntoExplicitRepresentation(std::vector<ValueType> const& x, storm::storage::BitVector const& properMaybeStates) {
                std::vector<ValueType> expandedResult(properMaybeStates.size(), storm::utility::zero<ValueType>());
                
                uint64_t position = 0;
                for (auto state : properMaybeStates) {
                    expandedResult[state] = x[position];
                    position++;
                }
                
                return expandedResult;
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<CheckResult> HybridMdpPrctlHelper<DdType, ValueType>::computeReachabilityRewards(OptimizationDirection dir, storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, RewardModelType const& rewardModel, storm::dd::Bdd<DdType> const& targetStates, bool qualitative, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                
                // Only compute the result if there is at least one reward model.
                STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                // Determine which states have a reward of infinity by definition.
                storm::dd::Bdd<DdType> infinityStates;
                storm::dd::Bdd<DdType> transitionMatrixBdd = transitionMatrix.notZero();
                if (dir == OptimizationDirection::Minimize) {
                    infinityStates = storm::utility::graph::performProb1E(model, transitionMatrixBdd, model.getReachableStates(), targetStates, storm::utility::graph::performProbGreater0E(model, transitionMatrixBdd, model.getReachableStates(), targetStates));
                } else {
                    infinityStates = storm::utility::graph::performProb1A(model, transitionMatrixBdd, targetStates, storm::utility::graph::performProbGreater0A(model, transitionMatrixBdd, model.getReachableStates(), targetStates));
                }
                infinityStates = !infinityStates && model.getReachableStates();
                storm::dd::Bdd<DdType> maybeStatesWithTargetStates = !infinityStates && model.getReachableStates();
                storm::dd::Bdd<DdType> maybeStates = !targetStates && maybeStatesWithTargetStates;
                STORM_LOG_INFO("Found " << infinityStates.getNonZeroCount() << " 'infinity' states.");
                STORM_LOG_INFO("Found " << targetStates.getNonZeroCount() << " 'target' states.");
                STORM_LOG_INFO("Found " << maybeStates.getNonZeroCount() << " 'maybe' states.");
                
                // Check whether we need to compute exact rewards for some states.
                if (qualitative) {
                    // Set the values for all maybe-states to 1 to indicate that their reward values
                    // are neither 0 nor infinity.
                    return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), infinityStates.ite(model.getManager().getConstant(storm::utility::infinity<ValueType>()), model.getManager().template getAddZero<ValueType>()) + maybeStates.template toAdd<ValueType>() * model.getManager().getConstant(storm::utility::one<ValueType>())));
                } else {
                    // If there are maybe states, we need to solve an equation system.
                    if (!maybeStates.isZero()) {
                        // Check for requirements of the solver this early so we can adapt the maybe states accordingly.
                        storm::solver::MinMaxLinearEquationSolverRequirements requirements = linearEquationSolverFactory.getRequirements(storm::solver::EquationSystemType::ReachabilityRewards, dir);
                        bool requireInitialScheduler = false;
                        if (!requirements.empty()) {
                            if (requirements.requires(storm::solver::MinMaxLinearEquationSolverRequirements::Element::ValidInitialScheduler)) {
                                STORM_LOG_DEBUG("Computing valid scheduler, because the solver requires it.");
                                requireInitialScheduler = true;
                                requirements.clearValidInitialScheduler();
                            }
                            STORM_LOG_THROW(requirements.empty(), storm::exceptions::UncheckedRequirementException, "Cannot establish requirements for solver.");
                        }

                        // Compute the set of maybe states that we are required to keep in the translation to explicit.
                        storm::dd::Bdd<DdType> requiredMaybeStates = requireInitialScheduler ? maybeStatesWithTargetStates : maybeStates;
                        
                        // Create the ODD for the translation between symbolic and explicit storage.
                        storm::dd::Odd odd = requiredMaybeStates.createOdd();
                        
                        // Create the matrix and the vector for the equation system.
                        storm::dd::Add<DdType, ValueType> maybeStatesAdd = maybeStates.template toAdd<ValueType>();
                        
                        // Start by getting rid of
                        // (a) transitions from non-maybe states, and
                        // (b) the choices in the transition matrix that lead to a state that is neither a maybe state
                        // nor a target state ('infinity choices').
                        storm::dd::Add<DdType, ValueType> choiceFilterAdd = (transitionMatrixBdd && maybeStatesWithTargetStates.renameVariables(model.getRowVariables(), model.getColumnVariables())).existsAbstract(model.getColumnVariables()).template toAdd<ValueType>();
                        storm::dd::Add<DdType, ValueType> submatrix = transitionMatrix * maybeStatesAdd * choiceFilterAdd;
                        
                        // Then compute the reward vector to use in the computation.
                        storm::dd::Add<DdType, ValueType> subvector = rewardModel.getTotalRewardVector(maybeStatesAdd, submatrix, model.getColumnVariables());
                        if (!rewardModel.hasStateActionRewards() && !rewardModel.hasTransitionRewards()) {
                            // If the reward model neither has state-action nor transition rewards, we need to multiply
                            // it with the legal nondetermism encodings in each state.
                            subvector *= choiceFilterAdd;
                        }
                        
                        // Before cutting the non-maybe columns, we need to compute the sizes of the row groups.
                        storm::dd::Add<DdType, uint_fast64_t> stateActionAdd = submatrix.notZero().existsAbstract(model.getColumnVariables()).template toAdd<uint_fast64_t>();
                        std::vector<uint_fast64_t> rowGroupSizes = stateActionAdd.sumAbstract(model.getNondeterminismVariables()).toVector(odd);

                        // Finally cut away all columns targeting non-maybe states (or non-(maybe or target) states, respectively).
                        submatrix *= requireInitialScheduler ? maybeStatesWithTargetStates.swapVariables(model.getRowColumnMetaVariablePairs()).template toAdd<ValueType>() : maybeStatesAdd.swapVariables(model.getRowColumnMetaVariablePairs());
                        
                        // Translate the symbolic matrix/vector to their explicit representations.
                        std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>> explicitRepresentation = submatrix.toMatrixVector(subvector, std::move(rowGroupSizes), model.getNondeterminismVariables(), odd, odd);
                        
                        // Compute a valid initial scheduler if necessary.
                        boost::optional<std::vector<uint64_t>> initialScheduler;
                        boost::optional<storm::storage::BitVector> properMaybeStates;
                        if (requireInitialScheduler) {
                            // Compute a valid initial scheduler.
                            storm::storage::BitVector targetStates = computeTargetStatesForReachabilityRewardsFromExplicitRepresentation(explicitRepresentation.first);
                            properMaybeStates = ~targetStates;
                            initialScheduler = computeValidInitialSchedulerForReachabilityRewards<ValueType>(explicitRepresentation.first, properMaybeStates.get(), targetStates);
                            
                            // Since we needed the transitions to target states to be translated as well for the computation
                            // of the scheduler, we have to get rid of them now.
                            eliminateTargetStatesFromExplicitRepresentation(explicitRepresentation, initialScheduler.get(), properMaybeStates.get());
                        }

                        // Create the solution vector.
                        std::vector<ValueType> x(explicitRepresentation.first.getRowGroupCount(), storm::utility::zero<ValueType>());
                        
                        // Now solve the resulting equation system.
                        std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(std::move(explicitRepresentation.first));
                        
                        // Move the scheduler to the solver.
                        if (initialScheduler) {
                            solver->setInitialScheduler(std::move(initialScheduler.get()));
                        }
                        
                        solver->setRequirementsChecked();
                        solver->solveEquations(dir, x, explicitRepresentation.second);
                        
                        // If we included the target states in the ODD, we need to expand the result from the solver.
                        if (requireInitialScheduler) {
                            x = insertTargetStateValuesIntoExplicitRepresentation(x, properMaybeStates.get());
                        }
                        
                        // Return a hybrid check result that stores the numerical values explicitly.
                        return std::unique_ptr<CheckResult>(new storm::modelchecker::HybridQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), model.getReachableStates() && !maybeStates, infinityStates.ite(model.getManager().getConstant(storm::utility::infinity<ValueType>()), model.getManager().template getAddZero<ValueType>()), maybeStates, odd, x));
                    } else {
                        return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), infinityStates.ite(model.getManager().getConstant(storm::utility::infinity<ValueType>()), model.getManager().template getAddZero<ValueType>())));
                    }
                }
            }
            
            template class HybridMdpPrctlHelper<storm::dd::DdType::CUDD, double>;
            template class HybridMdpPrctlHelper<storm::dd::DdType::Sylvan, double>;

            template class HybridMdpPrctlHelper<storm::dd::DdType::Sylvan, storm::RationalNumber>;

        }
    }
}
