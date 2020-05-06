#include "storm/modelchecker/multiobjective/pcaa/StandardPcaaWeightVectorChecker.h"

#include <map>
#include <set>

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"
#include "storm/modelchecker/prctl/helper/DsMpiUpperRewardBoundsComputer.h"
#include "storm/modelchecker/prctl/helper/BaierUpperRewardBoundsComputer.h"
#include "storm/modelchecker/multiobjective/preprocessing/SparseMultiObjectiveRewardAnalysis.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/utility/graph.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/logic/Formulas.h"
#include "storm/transformer/GoalStateMerger.h"

#include "storm/exceptions/IllegalFunctionCallException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UncheckedRequirementException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            

            template <class SparseModelType>
            StandardPcaaWeightVectorChecker<SparseModelType>::StandardPcaaWeightVectorChecker(preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType> const& preprocessorResult) :
                    PcaaWeightVectorChecker<SparseModelType>(preprocessorResult.objectives) {
                // Intantionally left empty
            }
            
            template <class SparseModelType>
            void StandardPcaaWeightVectorChecker<SparseModelType>::initialize(preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType> const& preprocessorResult) {
                auto rewardAnalysis = preprocessing::SparseMultiObjectiveRewardAnalysis<SparseModelType>::analyze(preprocessorResult);
                STORM_LOG_THROW(rewardAnalysis.rewardFinitenessType != preprocessing::RewardFinitenessType::Infinite, storm::exceptions::NotSupportedException, "There is no Pareto optimal scheduler that yields finite reward for all objectives. This is not supported.");
                STORM_LOG_THROW(rewardAnalysis.rewardLessInfinityEStates, storm::exceptions::UnexpectedException, "The set of states with reward < infinity for some scheduler has not been computed during preprocessing.");
                STORM_LOG_THROW(preprocessorResult.containsOnlyTrivialObjectives(), storm::exceptions::NotSupportedException, "At least one objective was not reduced to an expected (total or cumulative) reward objective during preprocessing. This is not supported by the considered weight vector checker.");
                STORM_LOG_THROW(preprocessorResult.preprocessedModel->getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::NotSupportedException, "The model has multiple initial states.");

                // Build a subsystem of the preprocessor result model that discards states that yield infinite reward for all schedulers.
                // We can also merge the states that will have reward zero anyway.
                storm::storage::BitVector maybeStates = rewardAnalysis.rewardLessInfinityEStates.get() & ~rewardAnalysis.reward0AStates;
                storm::storage::BitVector finiteRewardChoices = preprocessorResult.preprocessedModel->getTransitionMatrix().getRowFilter(rewardAnalysis.rewardLessInfinityEStates.get(), rewardAnalysis.rewardLessInfinityEStates.get());
                std::set<std::string> relevantRewardModels;
                for (auto const& obj : this->objectives) {
                    obj.formula->gatherReferencedRewardModels(relevantRewardModels);
                }
                storm::transformer::GoalStateMerger<SparseModelType> merger(*preprocessorResult.preprocessedModel);
                auto mergerResult = merger.mergeTargetAndSinkStates(maybeStates, rewardAnalysis.reward0AStates, storm::storage::BitVector(maybeStates.size(), false), std::vector<std::string>(relevantRewardModels.begin(), relevantRewardModels.end()), finiteRewardChoices);
                
                // Initialize data specific for the considered model type
                initializeModelTypeSpecificData(*mergerResult.model);
                
                // Initilize general data of the model
                transitionMatrix = std::move(mergerResult.model->getTransitionMatrix());
                initialState = *mergerResult.model->getInitialStates().begin();
                reward0EStates = rewardAnalysis.reward0EStates % maybeStates;
                if (mergerResult.targetState) {
                    // There is an additional state in the result
                    reward0EStates.resize(reward0EStates.size() + 1, true);
                    
                    // The overapproximation for the possible ec choices consists of the states that can reach the target states with prob. 0 and the target state itself.
                    storm::storage::BitVector targetStateAsVector(transitionMatrix.getRowGroupCount(), false);
                    targetStateAsVector.set(*mergerResult.targetState, true);
                    ecChoicesHint = transitionMatrix.getRowFilter(storm::utility::graph::performProb0E(transitionMatrix, transitionMatrix.getRowGroupIndices(), transitionMatrix.transpose(true), storm::storage::BitVector(targetStateAsVector.size(), true), targetStateAsVector));
                    ecChoicesHint.set(transitionMatrix.getRowGroupIndices()[*mergerResult.targetState], true);
                } else {
                    ecChoicesHint = storm::storage::BitVector(transitionMatrix.getRowCount(), true);
                }
                
                // set data for unbounded objectives
                objectivesWithNoUpperTimeBound = storm::storage::BitVector(this->objectives.size(), false);
                actionsWithoutRewardInUnboundedPhase = storm::storage::BitVector(transitionMatrix.getRowCount(), true);
                for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    auto const& formula = *this->objectives[objIndex].formula;
                    if (formula.getSubformula().isTotalRewardFormula()) {
                        objectivesWithNoUpperTimeBound.set(objIndex, true);
                        actionsWithoutRewardInUnboundedPhase &= storm::utility::vector::filterZero(actionRewards[objIndex]);
                    }
                }
                
                // initialize data for the results
                checkHasBeenCalled = false;
                objectiveResults.resize(this->objectives.size());
                offsetsToUnderApproximation.resize(this->objectives.size(), storm::utility::zero<ValueType>());
                offsetsToOverApproximation.resize(this->objectives.size(), storm::utility::zero<ValueType>());
                optimalChoices.resize(transitionMatrix.getRowGroupCount(), 0);
            }

            
            template <class SparseModelType>
            void StandardPcaaWeightVectorChecker<SparseModelType>::check(Environment const& env, std::vector<ValueType> const& weightVector) {
                checkHasBeenCalled = true;
                STORM_LOG_INFO("Invoked WeightVectorChecker with weights " << std::endl << "\t" << storm::utility::vector::toString(storm::utility::vector::convertNumericVector<double>(weightVector)));
                
                std::vector<ValueType> weightedRewardVector(transitionMatrix.getRowCount(), storm::utility::zero<ValueType>());
                for (auto objIndex : objectivesWithNoUpperTimeBound) {
                    if (storm::solver::minimize(this->objectives[objIndex].formula->getOptimalityType())) {
                        storm::utility::vector::addScaledVector(weightedRewardVector, actionRewards[objIndex], -weightVector[objIndex]);
                    } else {
                        storm::utility::vector::addScaledVector(weightedRewardVector, actionRewards[objIndex], weightVector[objIndex]);
                    }
                }
                
                unboundedWeightedPhase(env, weightedRewardVector, weightVector);
                
                unboundedIndividualPhase(env, weightVector);
                // Only invoke boundedPhase if necessarry, i.e., if there is at least one objective with a time bound
                for (auto const& obj : this->objectives) {
                    if (!obj.formula->getSubformula().isTotalRewardFormula()) {
                        boundedPhase(env, weightVector, weightedRewardVector);
                        break;
                    }
                }
                STORM_LOG_INFO("Weight vector check done. Lower bounds for results in initial state: " << storm::utility::vector::toString(storm::utility::vector::convertNumericVector<double>(getUnderApproximationOfInitialStateResults())));
                // Validate that the results are sufficiently precise
                ValueType resultingWeightedPrecision = storm::utility::abs<ValueType>(storm::utility::vector::dotProduct(getOverApproximationOfInitialStateResults(), weightVector) - storm::utility::vector::dotProduct(getUnderApproximationOfInitialStateResults(), weightVector));
                resultingWeightedPrecision /= storm::utility::sqrt(storm::utility::vector::dotProduct(weightVector, weightVector));
                STORM_LOG_THROW(resultingWeightedPrecision <= this->getWeightedPrecision(), storm::exceptions::UnexpectedException, "The desired precision was not reached");
            }
            
            template <class SparseModelType>
            std::vector<typename StandardPcaaWeightVectorChecker<SparseModelType>::ValueType> StandardPcaaWeightVectorChecker<SparseModelType>::getUnderApproximationOfInitialStateResults() const {
                STORM_LOG_THROW(checkHasBeenCalled, storm::exceptions::IllegalFunctionCallException, "Tried to retrieve results but check(..) has not been called before.");
                std::vector<ValueType> res;
                res.reserve(this->objectives.size());
                for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    res.push_back(this->objectiveResults[objIndex][initialState] + this->offsetsToUnderApproximation[objIndex]);
                }
                return res;
            }
            
            template <class SparseModelType>
            std::vector<typename StandardPcaaWeightVectorChecker<SparseModelType>::ValueType> StandardPcaaWeightVectorChecker<SparseModelType>::getOverApproximationOfInitialStateResults() const {
                STORM_LOG_THROW(checkHasBeenCalled, storm::exceptions::IllegalFunctionCallException, "Tried to retrieve results but check(..) has not been called before.");
                std::vector<ValueType> res;
                res.reserve(this->objectives.size());
                for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    res.push_back(this->objectiveResults[objIndex][initialState] + this->offsetsToOverApproximation[objIndex]);
                }
                return res;
            }
            
            template <class SparseModelType>
            storm::storage::Scheduler<typename StandardPcaaWeightVectorChecker<SparseModelType>::ValueType> StandardPcaaWeightVectorChecker<SparseModelType>::computeScheduler() const {
                STORM_LOG_THROW(this->checkHasBeenCalled, storm::exceptions::IllegalFunctionCallException, "Tried to retrieve results but check(..) has not been called before.");
                for (auto const& obj : this->objectives) {
                    STORM_LOG_THROW(obj.formula->getSubformula().isTotalRewardFormula(), storm::exceptions::NotImplementedException, "Scheduler retrival is only implemented for objectives without time-bound.");
                }
                
                storm::storage::Scheduler<ValueType> result(this->optimalChoices.size());
                uint_fast64_t state = 0;
                for (auto const& choice : optimalChoices) {
                    result.setChoice(choice, state);
                    ++state;
                }
                return result;
            }
            
            template <typename ValueType>
            std::vector<uint64_t> computeValidInitialScheduler(storm::storage::SparseMatrix<ValueType> const& matrix, storm::storage::BitVector const& rowsWithSumLessOne) {
                std::vector<uint64_t> result(matrix.getRowGroupCount());
                auto const& groups = matrix.getRowGroupIndices();
                auto backwardsTransitions = matrix.transpose(true);
                storm::storage::BitVector processedStates(result.size(), false);
                for (uint64_t state = 0; state < result.size(); ++state) {
                    if (rowsWithSumLessOne.getNextSetIndex(groups[state]) < groups[state + 1]) {
                        result[state] = rowsWithSumLessOne.getNextSetIndex(groups[state]) - groups[state];
                        processedStates.set(state, true);
                    }
                }
                std::vector<uint64_t> stack(processedStates.begin(), processedStates.end());
                while (!stack.empty()) {
                    uint64_t current = stack.back();
                    stack.pop_back();
                    STORM_LOG_ASSERT(processedStates.get(current), "states on the stack shall be processed.");
                    for (auto const& entry : backwardsTransitions.getRow(current)) {
                        uint64_t pred = entry.getColumn();
                        if (!processedStates.get(pred)) {
                            // Find a choice that leads to a processed state
                            uint64_t predChoice = groups[pred];
                            bool foundSuccessor = false;
                            for (; predChoice < groups[pred + 1]; ++predChoice) {
                                for (auto const& predEntry : matrix.getRow(predChoice)) {
                                    if (processedStates.get(predEntry.getColumn())) {
                                        foundSuccessor = true;
                                        break;
                                    }
                                }
                                if (foundSuccessor) {
                                    break;
                                }
                            }
                            STORM_LOG_ASSERT(foundSuccessor && predChoice < groups[pred + 1], "Predecessor of a processed state should have a processed successor");
                            result[pred] = predChoice - groups[pred];
                            processedStates.set(pred, true);
                            stack.push_back(pred);
                        }
                    }
                }
                return result;
            }
            
            template <class SparseModelType>
            void StandardPcaaWeightVectorChecker<SparseModelType>::unboundedWeightedPhase(Environment const& env, std::vector<ValueType> const& weightedRewardVector, std::vector<ValueType> const& weightVector) {
                
                if (this->objectivesWithNoUpperTimeBound.empty() || !storm::utility::vector::hasNonZeroEntry(weightedRewardVector)) {
                    this->weightedResult = std::vector<ValueType>(transitionMatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
                    this->optimalChoices = std::vector<uint_fast64_t>(transitionMatrix.getRowGroupCount(), 0);
                    return;
                }
                
                updateEcQuotient(weightedRewardVector);
                
                storm::utility::vector::selectVectorValues(ecQuotient->auxChoiceValues, ecQuotient->ecqToOriginalChoiceMapping, weightedRewardVector);
                
                storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> solverFactory;
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = solverFactory.create(env, ecQuotient->matrix);
                solver->setTrackScheduler(true);
                solver->setHasUniqueSolution(true);
                solver->setOptimizationDirection(storm::solver::OptimizationDirection::Maximize);
                auto req = solver->getRequirements(env, storm::solver::OptimizationDirection::Maximize);
                setBoundsToSolver(*solver, req.lowerBounds(), req.upperBounds(), weightVector, objectivesWithNoUpperTimeBound, ecQuotient->matrix, ecQuotient->rowsWithSumLessOne, ecQuotient->auxChoiceValues);
                if (solver->hasLowerBound()) {
                    req.clearLowerBounds();
                }
                if (solver->hasUpperBound()) {
                    req.clearUpperBounds();
                }
                if (req.validInitialScheduler()) {
                    solver->setInitialScheduler(computeValidInitialScheduler(ecQuotient->matrix, ecQuotient->rowsWithSumLessOne));
                    req.clearValidInitialScheduler();
                }
                STORM_LOG_THROW(!req.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException, "Solver requirements " + req.getEnabledRequirementsAsString() + " not checked.");
                solver->setRequirementsChecked(true);
                
                // Use the (0...0) vector as initial guess for the solution.
                std::fill(ecQuotient->auxStateValues.begin(), ecQuotient->auxStateValues.end(), storm::utility::zero<ValueType>());
                
                solver->solveEquations(env, ecQuotient->auxStateValues, ecQuotient->auxChoiceValues);
                this->weightedResult = std::vector<ValueType>(transitionMatrix.getRowGroupCount());
                
                transformReducedSolutionToOriginalModel(ecQuotient->matrix, ecQuotient->auxStateValues, solver->getSchedulerChoices(), ecQuotient->ecqToOriginalChoiceMapping, ecQuotient->originalToEcqStateMapping, this->weightedResult, this->optimalChoices);
            }
            
            template <class SparseModelType>
            void StandardPcaaWeightVectorChecker<SparseModelType>::unboundedIndividualPhase(Environment const& env,std::vector<ValueType> const& weightVector) {
                if (objectivesWithNoUpperTimeBound.getNumberOfSetBits() == 1 && storm::utility::isOne(weightVector[*objectivesWithNoUpperTimeBound.begin()])) {
                   uint_fast64_t objIndex = *objectivesWithNoUpperTimeBound.begin();
                   objectiveResults[objIndex] = weightedResult;
                   if (storm::solver::minimize(this->objectives[objIndex].formula->getOptimalityType())) {
                       storm::utility::vector::scaleVectorInPlace(objectiveResults[objIndex], -storm::utility::one<ValueType>());
                   }
                   for (uint_fast64_t objIndex2 = 0; objIndex2 < this->objectives.size(); ++objIndex2) {
                       if (objIndex != objIndex2) {
                           objectiveResults[objIndex2] = std::vector<ValueType>(transitionMatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
                       }
                   }
                } else {
                   storm::storage::SparseMatrix<ValueType> deterministicMatrix = transitionMatrix.selectRowsFromRowGroups(this->optimalChoices, true);
                   storm::storage::SparseMatrix<ValueType> deterministicBackwardTransitions = deterministicMatrix.transpose();
                   std::vector<ValueType> deterministicStateRewards(deterministicMatrix.getRowCount());
                   storm::solver::GeneralLinearEquationSolverFactory<ValueType> linearEquationSolverFactory;

                   // We compute an estimate for the results of the individual objectives which is obtained from the weighted result and the result of the objectives computed so far.
                   // Note that weightedResult = Sum_{i=1}^{n} w_i * objectiveResult_i.
                   std::vector<ValueType> weightedSumOfUncheckedObjectives = weightedResult;
                   ValueType sumOfWeightsOfUncheckedObjectives = storm::utility::vector::sum_if(weightVector, objectivesWithNoUpperTimeBound);

                   for (uint_fast64_t const &objIndex : storm::utility::vector::getSortedIndices(weightVector)) {
                       auto const& obj = this->objectives[objIndex];
                       if (objectivesWithNoUpperTimeBound.get(objIndex)) {
                           offsetsToUnderApproximation[objIndex] = storm::utility::zero<ValueType>();
                           offsetsToOverApproximation[objIndex] = storm::utility::zero<ValueType>();
                           storm::utility::vector::selectVectorValues(deterministicStateRewards, this->optimalChoices, transitionMatrix.getRowGroupIndices(), actionRewards[objIndex]);
                           storm::storage::BitVector statesWithRewards = ~storm::utility::vector::filterZero(deterministicStateRewards);
                           // As maybestates we pick the states from which a state with reward is reachable
                           storm::storage::BitVector maybeStates = storm::utility::graph::performProbGreater0(deterministicBackwardTransitions, storm::storage::BitVector(deterministicMatrix.getRowCount(), true), statesWithRewards);

                           // Compute the estimate for this objective
                           if (!storm::utility::isZero(weightVector[objIndex])) {
                               objectiveResults[objIndex] = weightedSumOfUncheckedObjectives;
                               ValueType scalingFactor = storm::utility::one<ValueType>() / sumOfWeightsOfUncheckedObjectives;
                               if (storm::solver::minimize(obj.formula->getOptimalityType())) {
                                   scalingFactor *= -storm::utility::one<ValueType>();
                               }
                               storm::utility::vector::scaleVectorInPlace(objectiveResults[objIndex], scalingFactor);
                               storm::utility::vector::clip(objectiveResults[objIndex], obj.lowerResultBound, obj.upperResultBound);
                           }
                           // Make sure that the objectiveResult is initialized correctly
                           objectiveResults[objIndex].resize(transitionMatrix.getRowGroupCount(), storm::utility::zero<ValueType>());

                           if (!maybeStates.empty()) {
                               bool needEquationSystem = linearEquationSolverFactory.getEquationProblemFormat(env) == storm::solver::LinearEquationSolverProblemFormat::EquationSystem;
                               storm::storage::SparseMatrix<ValueType> submatrix = deterministicMatrix.getSubmatrix(
                                       true, maybeStates, maybeStates, needEquationSystem);
                               if (needEquationSystem) {
                                   // Converting the matrix from the fixpoint notation to the form needed for the equation
                                   // system. That is, we go from x = A*x + b to (I-A)x = b.
                                   submatrix.convertToEquationSystem();
                               }

                               // Prepare solution vector and rhs of the equation system.
                               std::vector<ValueType> x = storm::utility::vector::filterVector(objectiveResults[objIndex], maybeStates);
                               std::vector<ValueType> b = storm::utility::vector::filterVector(deterministicStateRewards, maybeStates);

                               // Now solve the resulting equation system.
                               std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(env, submatrix);
                               auto req = solver->getRequirements(env);
                               solver->clearBounds();
                               storm::storage::BitVector submatrixRowsWithSumLessOne = deterministicMatrix.getRowFilter(maybeStates, maybeStates) % maybeStates;
                               submatrixRowsWithSumLessOne.complement();
                               this->setBoundsToSolver(*solver, req.lowerBounds(), req.upperBounds(), objIndex, submatrix, submatrixRowsWithSumLessOne, b);
                               if (solver->hasLowerBound()) {
                                   req.clearLowerBounds();
                               }
                               if (solver->hasUpperBound()) {
                                   req.clearUpperBounds();
                               }
                               STORM_LOG_THROW(!req.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException, "Solver requirements " + req.getEnabledRequirementsAsString() + " not checked.");
                               solver->solveEquations(env, x, b);
                               // Set the result for this objective accordingly
                               storm::utility::vector::setVectorValues<ValueType>(objectiveResults[objIndex], maybeStates, x);
                           }
                           storm::utility::vector::setVectorValues<ValueType>(objectiveResults[objIndex], ~maybeStates, storm::utility::zero<ValueType>());

                           // Update the estimate for the next objectives.
                           if (!storm::utility::isZero(weightVector[objIndex])) {
                               storm::utility::vector::addScaledVector(weightedSumOfUncheckedObjectives, objectiveResults[objIndex], -weightVector[objIndex]);
                               sumOfWeightsOfUncheckedObjectives -= weightVector[objIndex];
                           }
                       } else {
                           objectiveResults[objIndex] = std::vector<ValueType>(transitionMatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
                       }
                   }
               }
            }
            
            template <class SparseModelType>
            void StandardPcaaWeightVectorChecker<SparseModelType>::updateEcQuotient(std::vector<ValueType> const& weightedRewardVector) {
                // Check whether we need to update the currently cached ecElimResult
                storm::storage::BitVector newReward0Choices = storm::utility::vector::filterZero(weightedRewardVector);
                if (!ecQuotient || ecQuotient->origReward0Choices != newReward0Choices) {
                    
                    // It is sufficient to consider the states from which a transition with non-zero reward is reachable. (The remaining states always have reward zero).
                    storm::storage::BitVector nonZeroRewardStates(transitionMatrix.getRowGroupCount(), false);
                    for (uint_fast64_t state = 0; state < transitionMatrix.getRowGroupCount(); ++state){
                        if (newReward0Choices.getNextUnsetIndex(transitionMatrix.getRowGroupIndices()[state]) < transitionMatrix.getRowGroupIndices()[state+1]) {
                            nonZeroRewardStates.set(state);
                        }
                    }
                    storm::storage::BitVector subsystemStates = storm::utility::graph::performProbGreater0E(transitionMatrix.transpose(true), storm::storage::BitVector(transitionMatrix.getRowGroupCount(), true), nonZeroRewardStates);
                
                    // Remove neutral end components, i.e., ECs in which no reward is earned.
                    auto ecElimResult = storm::transformer::EndComponentEliminator<ValueType>::transform(transitionMatrix, subsystemStates, ecChoicesHint & newReward0Choices, reward0EStates);
                    
                    storm::storage::BitVector rowsWithSumLessOne(ecElimResult.matrix.getRowCount(), false);
                    for (uint64_t row = 0; row < rowsWithSumLessOne.size(); ++row) {
                        if (ecElimResult.matrix.getRow(row).getNumberOfEntries() == 0) {
                            rowsWithSumLessOne.set(row, true);
                        } else {
                            for (auto const& entry : transitionMatrix.getRow(ecElimResult.newToOldRowMapping[row])) {
                                if (!subsystemStates.get(entry.getColumn())) {
                                    rowsWithSumLessOne.set(row, true);
                                    break;
                                }
                            }
                        }
                    }
                    
                    ecQuotient = EcQuotient();
                    ecQuotient->matrix = std::move(ecElimResult.matrix);
                    ecQuotient->ecqToOriginalChoiceMapping = std::move(ecElimResult.newToOldRowMapping);
                    ecQuotient->originalToEcqStateMapping = std::move(ecElimResult.oldToNewStateMapping);
                    ecQuotient->origReward0Choices = std::move(newReward0Choices);
                    ecQuotient->rowsWithSumLessOne = std::move(rowsWithSumLessOne);
                    ecQuotient->auxStateValues.reserve(transitionMatrix.getRowGroupCount());
                    ecQuotient->auxStateValues.resize(ecQuotient->matrix.getRowGroupCount());
                    ecQuotient->auxChoiceValues.reserve(transitionMatrix.getRowCount());
                    ecQuotient->auxChoiceValues.resize(ecQuotient->matrix.getRowCount());
                }
            }

            template <class SparseModelType>
            void StandardPcaaWeightVectorChecker<SparseModelType>::setBoundsToSolver(storm::solver::AbstractEquationSolver<ValueType>& solver, bool requiresLower, bool requiresUpper, uint64_t objIndex, storm::storage::SparseMatrix<ValueType> const& transitions, storm::storage::BitVector const& rowsWithSumLessOne, std::vector<ValueType> const& rewards) const {
                
                // Check whether bounds are already available
                if (this->objectives[objIndex].lowerResultBound) {
                    solver.setLowerBound(this->objectives[objIndex].lowerResultBound.get());
                }
                if (this->objectives[objIndex].upperResultBound) {
                    solver.setUpperBound(this->objectives[objIndex].upperResultBound.get());
                }
                
                if ((requiresLower && !solver.hasLowerBound()) || (requiresUpper && !solver.hasUpperBound())) {
                    computeAndSetBoundsToSolver(solver, requiresLower, requiresUpper, transitions, rowsWithSumLessOne, rewards);
                }
            }
            
            template <class SparseModelType>
            void StandardPcaaWeightVectorChecker<SparseModelType>::setBoundsToSolver(storm::solver::AbstractEquationSolver<ValueType>& solver, bool requiresLower, bool requiresUpper, std::vector<ValueType> const& weightVector, storm::storage::BitVector const& objectiveFilter, storm::storage::SparseMatrix<ValueType> const& transitions, storm::storage::BitVector const& rowsWithSumLessOne, std::vector<ValueType> const& rewards) const {
                
                // Check whether bounds are already available
                boost::optional<ValueType> lowerBound = this->computeWeightedResultBound(true, weightVector, objectiveFilter);
                if (lowerBound) {
                    solver.setLowerBound(lowerBound.get());
                }
                boost::optional<ValueType> upperBound = this->computeWeightedResultBound(false, weightVector, objectiveFilter);
                if (upperBound) {
                    solver.setUpperBound(upperBound.get());
                }
                
                if ((requiresLower && !solver.hasLowerBound()) || (requiresUpper && !solver.hasUpperBound())) {
                    computeAndSetBoundsToSolver(solver, requiresLower, requiresUpper, transitions, rowsWithSumLessOne, rewards);
                }
            }
            
            template <class SparseModelType>
            void StandardPcaaWeightVectorChecker<SparseModelType>::computeAndSetBoundsToSolver(storm::solver::AbstractEquationSolver<ValueType>& solver, bool requiresLower, bool requiresUpper, storm::storage::SparseMatrix<ValueType> const& transitions, storm::storage::BitVector const& rowsWithSumLessOne, std::vector<ValueType> const& rewards) const {

                // Compute the one step target probs
                std::vector<ValueType> oneStepTargetProbs(transitions.getRowCount(), storm::utility::zero<ValueType>());
                for (auto const& row : rowsWithSumLessOne) {
                    oneStepTargetProbs[row] = storm::utility::one<ValueType>() - transitions.getRowSum(row);
                }
                
                if (requiresLower && !solver.hasLowerBound()) {
                    // Compute lower bounds
                    std::vector<ValueType> negativeRewards;
                    negativeRewards.reserve(transitions.getRowCount());
                    uint64_t row = 0;
                    for (auto const& rew : rewards) {
                        if (rew < storm::utility::zero<ValueType>()) {
                            negativeRewards.resize(row, storm::utility::zero<ValueType>());
                            negativeRewards.push_back(-rew);
                        }
                        ++row;
                    }
                    if (!negativeRewards.empty()) {
                        negativeRewards.resize(row, storm::utility::zero<ValueType>());
                        std::vector<ValueType> lowerBounds = storm::modelchecker::helper::DsMpiMdpUpperRewardBoundsComputer<ValueType>(transitions, negativeRewards, oneStepTargetProbs).computeUpperBounds();
                        storm::utility::vector::scaleVectorInPlace(lowerBounds, -storm::utility::one<ValueType>());
                        solver.setLowerBounds(std::move(lowerBounds));
                    } else {
                        solver.setLowerBound(storm::utility::zero<ValueType>());
                    }
                }
                
                // Compute upper bounds
                if (requiresUpper && !solver.hasUpperBound()) {
                    std::vector<ValueType> positiveRewards;
                    positiveRewards.reserve(transitions.getRowCount());
                    uint64_t row = 0;
                    for (auto const& rew : rewards) {
                        if (rew > storm::utility::zero<ValueType>()) {
                            positiveRewards.resize(row, storm::utility::zero<ValueType>());
                            positiveRewards.push_back(rew);
                        }
                        ++row;
                    }
                    if (!positiveRewards.empty()) {
                        positiveRewards.resize(row, storm::utility::zero<ValueType>());
                        solver.setUpperBound(storm::modelchecker::helper::BaierUpperRewardBoundsComputer<ValueType>(transitions, positiveRewards, oneStepTargetProbs).computeUpperBound());
                    } else {
                        solver.setUpperBound(storm::utility::zero<ValueType>());
                    }
                }
            }
            
            template <class SparseModelType>
            void StandardPcaaWeightVectorChecker<SparseModelType>::transformReducedSolutionToOriginalModel(storm::storage::SparseMatrix<ValueType> const& reducedMatrix,
                                                         std::vector<ValueType> const& reducedSolution,
                                                         std::vector<uint_fast64_t> const& reducedOptimalChoices,
                                                         std::vector<uint_fast64_t> const& reducedToOriginalChoiceMapping,
                                                         std::vector<uint_fast64_t> const& originalToReducedStateMapping,
                                                         std::vector<ValueType>& originalSolution,
                                                         std::vector<uint_fast64_t>& originalOptimalChoices) const {
                
                storm::storage::BitVector bottomStates(transitionMatrix.getRowGroupCount(), false);
                storm::storage::BitVector statesThatShouldStayInTheirEC(transitionMatrix.getRowGroupCount(), false);
                storm::storage::BitVector statesWithUndefSched(transitionMatrix.getRowGroupCount(), false);
                
                // Handle all the states for which the choice in the original model is uniquely given by the choice in the reduced model
                // Also store some information regarding the remaining states
                for (uint_fast64_t state = 0; state < transitionMatrix.getRowGroupCount(); ++state) {
                    // Check if the state exists in the reduced model, i.e., the mapping retrieves a valid index
                    uint_fast64_t stateInReducedModel = originalToReducedStateMapping[state];
                    if (stateInReducedModel < reducedMatrix.getRowGroupCount()) {
                        originalSolution[state] = reducedSolution[stateInReducedModel];
                        uint_fast64_t chosenRowInReducedModel = reducedMatrix.getRowGroupIndices()[stateInReducedModel] + reducedOptimalChoices[stateInReducedModel];
                        uint_fast64_t chosenRowInOriginalModel = reducedToOriginalChoiceMapping[chosenRowInReducedModel];
                        // Check if the state is a bottom state, i.e., the chosen row stays inside its EC.
                        bool stateIsBottom = reward0EStates.get(state);
                        for (auto const& entry : transitionMatrix.getRow(chosenRowInOriginalModel)) {
                            stateIsBottom &= originalToReducedStateMapping[entry.getColumn()] == stateInReducedModel;
                        }
                        if (stateIsBottom) {
                            bottomStates.set(state);
                            statesThatShouldStayInTheirEC.set(state);
                        } else {
                            // Check if the chosen row originaly belonged to the current state (and not to another state of the EC)
                            if (chosenRowInOriginalModel >= transitionMatrix.getRowGroupIndices()[state] &&
                               chosenRowInOriginalModel <  transitionMatrix.getRowGroupIndices()[state+1]) {
                                originalOptimalChoices[state] = chosenRowInOriginalModel - transitionMatrix.getRowGroupIndices()[state];
                            } else {
                                statesWithUndefSched.set(state);
                                statesThatShouldStayInTheirEC.set(state);
                            }
                        }
                    } else {
                        // if the state does not exist in the reduced model, it means that the (weighted) result is always zero, independent of the scheduler.
                        originalSolution[state] = storm::utility::zero<ValueType>();
                        // However, it might be the case that infinite reward is induced for an objective with weight 0.
                        // To avoid this, all possible bottom states are made bottom and the remaining states have to reach a bottom state with prob. one
                        if (reward0EStates.get(state)) {
                            bottomStates.set(state);
                        } else {
                            statesWithUndefSched.set(state);
                        }
                    }
                }
                
                // Handle bottom states
                for (auto state : bottomStates) {
                    bool foundRowForState = false;
                    // Find a row with zero rewards that only leads to bottom states.
                    // If the state should stay in its EC, we also need to make sure that all successors map to the same state in the reduced model
                    uint_fast64_t stateInReducedModel = originalToReducedStateMapping[state];
                    for (uint_fast64_t row = transitionMatrix.getRowGroupIndices()[state]; row < transitionMatrix.getRowGroupIndices()[state+1]; ++row) {
                        bool rowOnlyLeadsToBottomStates = true;
                        bool rowStaysInEC = true;
                        for ( auto const& entry : transitionMatrix.getRow(row)) {
                            rowOnlyLeadsToBottomStates &= bottomStates.get(entry.getColumn());
                            rowStaysInEC &= originalToReducedStateMapping[entry.getColumn()] == stateInReducedModel;
                        }
                        if (rowOnlyLeadsToBottomStates && (rowStaysInEC || !statesThatShouldStayInTheirEC.get(state)) && actionsWithoutRewardInUnboundedPhase.get(row)) {
                            foundRowForState = true;
                            originalOptimalChoices[state] = row - transitionMatrix.getRowGroupIndices()[state];
                            break;
                        }
                    }
                    STORM_LOG_ASSERT(foundRowForState, "Could not find a suitable choice for a bottom state.");
                }
                
                // Handle remaining states with still undef. scheduler (either EC states or non-subsystem states)
                while(!statesWithUndefSched.empty()) {
                    for (auto state : statesWithUndefSched) {
                        // Iteratively Try to find a choice such that at least one successor has a defined scheduler.
                        uint_fast64_t stateInReducedModel = originalToReducedStateMapping[state];
                        for (uint_fast64_t row = transitionMatrix.getRowGroupIndices()[state]; row < transitionMatrix.getRowGroupIndices()[state+1]; ++row) {
                            bool rowStaysInEC = true;
                            bool rowLeadsToDefinedScheduler = false;
                            for (auto const& entry : transitionMatrix.getRow(row)) {
                                rowStaysInEC &= ( stateInReducedModel == originalToReducedStateMapping[entry.getColumn()]);
                                rowLeadsToDefinedScheduler |= !statesWithUndefSched.get(entry.getColumn());
                            }
                            if (rowLeadsToDefinedScheduler && (rowStaysInEC || !statesThatShouldStayInTheirEC.get(state))) {
                                originalOptimalChoices[state] = row - transitionMatrix.getRowGroupIndices()[state];
                                statesWithUndefSched.set(state, false);
                                break;
                            }
                        }
                    }
                }
            }
            
            
            
            template class StandardPcaaWeightVectorChecker<storm::models::sparse::Mdp<double>>;
            template class StandardPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>;
#ifdef STORM_HAVE_CARL
            template class StandardPcaaWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
            template class StandardPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
#endif
            
        }
    }
}
