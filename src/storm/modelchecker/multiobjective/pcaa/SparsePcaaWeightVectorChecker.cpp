#include "storm/modelchecker/multiobjective/pcaa/SparsePcaaWeightVectorChecker.h"

#include <map>

#include "storm/adapters/CarlAdapter.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/transformer/EndComponentEliminator.h"
#include "storm/utility/graph.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"

#include "storm/exceptions/IllegalFunctionCallException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            

            template <class SparseModelType>
            SparsePcaaWeightVectorChecker<SparseModelType>::SparsePcaaWeightVectorChecker(SparseModelType const& model,
                                                                                          std::vector<Objective<typename SparseModelType::ValueType>> const& objectives,
                                                                                          storm::storage::BitVector const& possibleECActions,
                                                                                          storm::storage::BitVector const& possibleBottomStates) :
                    model(model),
                    objectives(objectives),
                    possibleECActions(possibleECActions),
                    actionsWithoutRewardInUnboundedPhase(model.getNumberOfChoices(), true),
                    possibleBottomStates(possibleBottomStates),
                    objectivesWithNoUpperTimeBound(objectives.size(), false),
                    discreteActionRewards(objectives.size()),
                    checkHasBeenCalled(false),
                    objectiveResults(objectives.size()),
                    offsetsToUnderApproximation(objectives.size()),
                    offsetsToOverApproximation(objectives.size()) {
                
                // set data for unbounded objectives
                for(uint_fast64_t objIndex = 0; objIndex < objectives.size(); ++objIndex) {
                    auto const& obj = objectives[objIndex];
                    if (!obj.upperTimeBound) {
                        objectivesWithNoUpperTimeBound.set(objIndex, true);
                        actionsWithoutRewardInUnboundedPhase &= model.getRewardModel(*obj.rewardModelName).getChoicesWithZeroReward(model.getTransitionMatrix());
                    }
                }
            }
            
            
            template <class SparseModelType>
            void SparsePcaaWeightVectorChecker<SparseModelType>::check(std::vector<ValueType> const& weightVector) {
                checkHasBeenCalled=true;
                STORM_LOG_INFO("Invoked WeightVectorChecker with weights " << std::endl << "\t" << storm::utility::vector::toString(storm::utility::vector::convertNumericVector<double>(weightVector)));
                std::vector<ValueType> weightedRewardVector(model.getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                boost::optional<ValueType> weightedLowerResultBound = storm::utility::zero<ValueType>();
                boost::optional<ValueType> weightedUpperResultBound = storm::utility::zero<ValueType>();
                for (auto objIndex : objectivesWithNoUpperTimeBound) {
                    if (storm::solver::minimize(objectives[objIndex].optimizationDirection)) {
                        if (objectives[objIndex].lowerResultBound && weightedUpperResultBound) {
                            weightedUpperResultBound.get() -= weightVector[objIndex] * objectives[objIndex].lowerResultBound.get();
                        } else {
                            weightedUpperResultBound = boost::none;
                        }
                        if (objectives[objIndex].upperResultBound && weightedLowerResultBound) {
                            weightedLowerResultBound.get() -= weightVector[objIndex] * objectives[objIndex].upperResultBound.get();
                        } else {
                            weightedLowerResultBound = boost::none;
                        }
                        storm::utility::vector::addScaledVector(weightedRewardVector, discreteActionRewards[objIndex], -weightVector[objIndex]);
                    } else {
                        if (objectives[objIndex].lowerResultBound && weightedLowerResultBound) {
                            weightedLowerResultBound.get() += weightVector[objIndex] * objectives[objIndex].lowerResultBound.get();
                        } else {
                            weightedLowerResultBound = boost::none;
                        }
                        if (objectives[objIndex].upperResultBound && weightedUpperResultBound) {
                            weightedUpperResultBound.get() += weightVector[objIndex] * objectives[objIndex].upperResultBound.get();
                        } else {
                            weightedUpperResultBound = boost::none;
                        }
                        storm::utility::vector::addScaledVector(weightedRewardVector, discreteActionRewards[objIndex], weightVector[objIndex]);
                    }
                }
                
                unboundedWeightedPhase(weightedRewardVector, weightedLowerResultBound, weightedUpperResultBound);
                unboundedIndividualPhase(weightVector);
                // Only invoke boundedPhase if necessarry, i.e., if there is at least one objective with a time bound
                for(auto const& obj : this->objectives) {
                    if(obj.lowerTimeBound || obj.upperTimeBound) {
                        boundedPhase(weightVector, weightedRewardVector);
                        break;
                    }
                }
                STORM_LOG_INFO("Weight vector check done. Lower bounds for results in initial state: " << storm::utility::vector::toString(storm::utility::vector::convertNumericVector<double>(getUnderApproximationOfInitialStateResults())));
                // Validate that the results are sufficiently precise
                ValueType resultingWeightedPrecision = storm::utility::abs<ValueType>(storm::utility::vector::dotProduct(getOverApproximationOfInitialStateResults(), weightVector) - storm::utility::vector::dotProduct(getUnderApproximationOfInitialStateResults(), weightVector));
                resultingWeightedPrecision /= storm::utility::sqrt(storm::utility::vector::dotProduct(weightVector, weightVector));
                STORM_LOG_THROW(resultingWeightedPrecision <= weightedPrecision, storm::exceptions::UnexpectedException, "The desired precision was not reached");
            }
            
            template <class SparseModelType>
            void SparsePcaaWeightVectorChecker<SparseModelType>::setWeightedPrecision(ValueType const& weightedPrecision) {
                this->weightedPrecision = weightedPrecision;
            }
            
            template <class SparseModelType>
            typename SparsePcaaWeightVectorChecker<SparseModelType>::ValueType const& SparsePcaaWeightVectorChecker<SparseModelType>::getWeightedPrecision() const {
                return this->weightedPrecision;
            }
            
            template <class SparseModelType>
            std::vector<typename SparsePcaaWeightVectorChecker<SparseModelType>::ValueType> SparsePcaaWeightVectorChecker<SparseModelType>::getUnderApproximationOfInitialStateResults() const {
                STORM_LOG_THROW(checkHasBeenCalled, storm::exceptions::IllegalFunctionCallException, "Tried to retrieve results but check(..) has not been called before.");
                uint_fast64_t initstate = *this->model.getInitialStates().begin();
                std::vector<ValueType> res;
                res.reserve(this->objectives.size());
                for(uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    res.push_back(this->objectiveResults[objIndex][initstate] + this->offsetsToUnderApproximation[objIndex]);
                }
                return res;
            }
            
            template <class SparseModelType>
            std::vector<typename SparsePcaaWeightVectorChecker<SparseModelType>::ValueType> SparsePcaaWeightVectorChecker<SparseModelType>::getOverApproximationOfInitialStateResults() const {
                STORM_LOG_THROW(checkHasBeenCalled, storm::exceptions::IllegalFunctionCallException, "Tried to retrieve results but check(..) has not been called before.");
                uint_fast64_t initstate = *this->model.getInitialStates().begin();
                std::vector<ValueType> res;
                res.reserve(this->objectives.size());
                for(uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    res.push_back(this->objectiveResults[objIndex][initstate] + this->offsetsToOverApproximation[objIndex]);
                }
                return res;
            }
            
            template <class SparseModelType>
            storm::storage::TotalScheduler const& SparsePcaaWeightVectorChecker<SparseModelType>::getScheduler() const {
                STORM_LOG_THROW(this->checkHasBeenCalled, storm::exceptions::IllegalFunctionCallException, "Tried to retrieve results but check(..) has not been called before.");
                for(auto const& obj : this->objectives) {
                    STORM_LOG_THROW(!obj.lowerTimeBound && !obj.upperTimeBound, storm::exceptions::NotImplementedException, "Scheduler retrival is not implemented for timeBounded objectives.");
                }
                return scheduler;
            }
            
            template <class SparseModelType>
            void SparsePcaaWeightVectorChecker<SparseModelType>::unboundedWeightedPhase(std::vector<ValueType> const& weightedRewardVector, boost::optional<ValueType> const& lowerResultBound, boost::optional<ValueType> const& upperResultBound) {
                if(this->objectivesWithNoUpperTimeBound.empty() || !storm::utility::vector::hasNonZeroEntry(weightedRewardVector)) {
                    this->weightedResult = std::vector<ValueType>(model.getNumberOfStates(), storm::utility::zero<ValueType>());
                    this->scheduler = storm::storage::TotalScheduler(model.getNumberOfStates());
                    return;
                }
                
                // Only consider the states from which a transition with non-zero reward is reachable. (The remaining states always have reward zero).
                storm::storage::BitVector zeroRewardActions = storm::utility::vector::filterZero(weightedRewardVector);
                storm::storage::BitVector nonZeroRewardActions = ~zeroRewardActions;
                storm::storage::BitVector nonZeroRewardStates(model.getNumberOfStates(), false);
                for(uint_fast64_t state = 0; state < model.getNumberOfStates(); ++state){
                    if(nonZeroRewardActions.getNextSetIndex(model.getTransitionMatrix().getRowGroupIndices()[state]) < model.getTransitionMatrix().getRowGroupIndices()[state+1]) {
                        nonZeroRewardStates.set(state);
                    }
                }
                storm::storage::BitVector subsystemStates = storm::utility::graph::performProbGreater0E(model.getTransitionMatrix().transpose(true), storm::storage::BitVector(model.getNumberOfStates(), true), nonZeroRewardStates);
                
                // Remove neutral end components, i.e., ECs in which no reward is earned.
                auto ecEliminatorResult = storm::transformer::EndComponentEliminator<ValueType>::transform(model.getTransitionMatrix(), subsystemStates, possibleECActions & zeroRewardActions, possibleBottomStates);
                
                std::vector<ValueType> subRewardVector(ecEliminatorResult.newToOldRowMapping.size());
                storm::utility::vector::selectVectorValues(subRewardVector, ecEliminatorResult.newToOldRowMapping, weightedRewardVector);
                std::vector<ValueType> subResult(ecEliminatorResult.matrix.getRowGroupCount());
                
                storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> solverFactory;
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = solverFactory.create(ecEliminatorResult.matrix);
                solver->setOptimizationDirection(storm::solver::OptimizationDirection::Maximize);
                solver->setTrackScheduler(true);
                if (lowerResultBound) {
                    solver->setLowerBound(*lowerResultBound);
                }
                if (upperResultBound) {
                    solver->setUpperBound(*upperResultBound);
                }
                solver->solveEquations(subResult, subRewardVector);

                this->weightedResult = std::vector<ValueType>(model.getNumberOfStates());
                std::vector<uint_fast64_t> optimalChoices(model.getNumberOfStates());
                
                transformReducedSolutionToOriginalModel(ecEliminatorResult.matrix, subResult, solver->getScheduler()->getChoices(), ecEliminatorResult.newToOldRowMapping, ecEliminatorResult.oldToNewStateMapping, this->weightedResult, optimalChoices);
                
                this->scheduler = storm::storage::TotalScheduler(std::move(optimalChoices));
            }
            
            template <class SparseModelType>
            void SparsePcaaWeightVectorChecker<SparseModelType>::unboundedIndividualPhase(std::vector<ValueType> const& weightVector) {
               if (objectivesWithNoUpperTimeBound.getNumberOfSetBits() == 1 && storm::utility::isOne(weightVector[*objectivesWithNoUpperTimeBound.begin()])) {
                   uint_fast64_t objIndex = *objectivesWithNoUpperTimeBound.begin();
                   objectiveResults[objIndex] = weightedResult;
                   if (storm::solver::minimize(objectives[objIndex].optimizationDirection)) {
                       storm::utility::vector::scaleVectorInPlace(objectiveResults[objIndex], -storm::utility::one<ValueType>());
                   }
                    for (uint_fast64_t objIndex2 = 0; objIndex2 < objectives.size(); ++objIndex2) {
			            if (objIndex != objIndex2) {
                            objectiveResults[objIndex2] = std::vector<ValueType>(model.getNumberOfStates(), storm::utility::zero<ValueType>());
                        }
                    }
               } else {
                   storm::storage::SparseMatrix<ValueType> deterministicMatrix = model.getTransitionMatrix().selectRowsFromRowGroups(this->scheduler.getChoices(), true);
                   storm::storage::SparseMatrix<ValueType> deterministicBackwardTransitions = deterministicMatrix.transpose();
                   std::vector<ValueType> deterministicStateRewards(deterministicMatrix.getRowCount());
                   storm::solver::GeneralLinearEquationSolverFactory<ValueType> linearEquationSolverFactory;

                   // We compute an estimate for the results of the individual objectives which is obtained from the weighted result and the result of the objectives computed so far.
                   // Note that weightedResult = Sum_{i=1}^{n} w_i * objectiveResult_i.
                   std::vector<ValueType> weightedSumOfUncheckedObjectives = weightedResult;
                   ValueType sumOfWeightsOfUncheckedObjectives = storm::utility::vector::sum_if(weightVector, objectivesWithNoUpperTimeBound);

                   for (uint_fast64_t const &objIndex : storm::utility::vector::getSortedIndices(weightVector)) {
                       auto const& obj = objectives[objIndex];
                       if (objectivesWithNoUpperTimeBound.get(objIndex)) {
                           offsetsToUnderApproximation[objIndex] = storm::utility::zero<ValueType>();
                           offsetsToOverApproximation[objIndex] = storm::utility::zero<ValueType>();
                           storm::utility::vector::selectVectorValues(deterministicStateRewards, this->scheduler.getChoices(), model.getTransitionMatrix().getRowGroupIndices(), discreteActionRewards[objIndex]);
                           storm::storage::BitVector statesWithRewards = ~storm::utility::vector::filterZero(deterministicStateRewards);
                           // As maybestates we pick the states from which a state with reward is reachable
                           storm::storage::BitVector maybeStates = storm::utility::graph::performProbGreater0(deterministicBackwardTransitions, storm::storage::BitVector(deterministicMatrix.getRowCount(), true), statesWithRewards);

                           // Compute the estimate for this objective
                           if (!storm::utility::isZero(weightVector[objIndex])) {
                               objectiveResults[objIndex] = weightedSumOfUncheckedObjectives;
                               ValueType scalingFactor = storm::utility::one<ValueType>() / sumOfWeightsOfUncheckedObjectives;
                               if (storm::solver::minimize(obj.optimizationDirection)) {
                                   scalingFactor *= -storm::utility::one<ValueType>();
                               }
                               storm::utility::vector::scaleVectorInPlace(objectiveResults[objIndex], scalingFactor);
                               storm::utility::vector::clip(objectiveResults[objIndex], obj.lowerResultBound, obj.upperResultBound);
                           }
                           // Make sure that the objectiveResult is initialized correctly
                           objectiveResults[objIndex].resize(model.getNumberOfStates(), storm::utility::zero<ValueType>());

                           if (!maybeStates.empty()) {
                               storm::storage::SparseMatrix<ValueType> submatrix = deterministicMatrix.getSubmatrix(
                                       true, maybeStates, maybeStates, true);
                               // Converting the matrix from the fixpoint notation to the form needed for the equation
                               // system. That is, we go from x = A*x + b to (I-A)x = b.
                               submatrix.convertToEquationSystem();

                               // Prepare solution vector and rhs of the equation system.
                               std::vector<ValueType> x = storm::utility::vector::filterVector(objectiveResults[objIndex], maybeStates);
                               std::vector<ValueType> b = storm::utility::vector::filterVector(deterministicStateRewards, maybeStates);

                               // Now solve the resulting equation system.
                               std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(std::move(submatrix));
                               if (obj.lowerResultBound) {
                                   solver->setLowerBound(*obj.lowerResultBound);
                               }
                               if (obj.upperResultBound) {
                                   solver->setUpperBound(*obj.upperResultBound);
                               }
                               solver->solveEquations(x, b);

                               // Set the result for this objective accordingly
                               storm::utility::vector::setVectorValues<ValueType>(objectiveResults[objIndex], maybeStates, x);
                               storm::utility::vector::setVectorValues<ValueType>(objectiveResults[objIndex], ~maybeStates, storm::utility::zero<ValueType>());
                           }

                           // Update the estimate for the next objectives.
                           if (!storm::utility::isZero(weightVector[objIndex])) {
                               storm::utility::vector::addScaledVector(weightedSumOfUncheckedObjectives, objectiveResults[objIndex], -weightVector[objIndex]);
                               sumOfWeightsOfUncheckedObjectives -= weightVector[objIndex];
                           }
                       } else {
                           objectiveResults[objIndex] = std::vector<ValueType>(model.getNumberOfStates(), storm::utility::zero<ValueType>());
                       }
                   }
               }
            }
            
            template <class SparseModelType>
            void SparsePcaaWeightVectorChecker<SparseModelType>::transformReducedSolutionToOriginalModel(storm::storage::SparseMatrix<ValueType> const& reducedMatrix,
                                                         std::vector<ValueType> const& reducedSolution,
                                                         std::vector<uint_fast64_t> const& reducedOptimalChoices,
                                                         std::vector<uint_fast64_t> const& reducedToOriginalChoiceMapping,
                                                         std::vector<uint_fast64_t> const& originalToReducedStateMapping,
                                                         std::vector<ValueType>& originalSolution,
                                                         std::vector<uint_fast64_t>& originalOptimalChoices) const {
                
                storm::storage::BitVector bottomStates(model.getTransitionMatrix().getRowGroupCount(), false);
                storm::storage::BitVector statesThatShouldStayInTheirEC(model.getTransitionMatrix().getRowGroupCount(), false);
                storm::storage::BitVector statesWithUndefSched(model.getTransitionMatrix().getRowGroupCount(), false);
                
                // Handle all the states for which the choice in the original model is uniquely given by the choice in the reduced model
                // Also store some information regarding the remaining states
                for(uint_fast64_t state = 0; state < model.getTransitionMatrix().getRowGroupCount(); ++state) {
                    // Check if the state exists in the reduced model, i.e., the mapping retrieves a valid index
                    uint_fast64_t stateInReducedModel = originalToReducedStateMapping[state];
                    if(stateInReducedModel < reducedMatrix.getRowGroupCount()) {
                        originalSolution[state] = reducedSolution[stateInReducedModel];
                        uint_fast64_t chosenRowInReducedModel = reducedMatrix.getRowGroupIndices()[stateInReducedModel] + reducedOptimalChoices[stateInReducedModel];
                        uint_fast64_t chosenRowInOriginalModel = reducedToOriginalChoiceMapping[chosenRowInReducedModel];
                        // Check if the state is a bottom state, i.e., the chosen row stays inside its EC.
                        bool stateIsBottom = possibleBottomStates.get(state);
                        for(auto const& entry : model.getTransitionMatrix().getRow(chosenRowInOriginalModel)) {
                            stateIsBottom &= originalToReducedStateMapping[entry.getColumn()] == stateInReducedModel;
                        }
                        if(stateIsBottom) {
                            bottomStates.set(state);
                            statesThatShouldStayInTheirEC.set(state);
                        } else {
                            // Check if the chosen row originaly belonged to the current state (and not to another state of the EC)
                            if(chosenRowInOriginalModel >= model.getTransitionMatrix().getRowGroupIndices()[state] &&
                               chosenRowInOriginalModel <  model.getTransitionMatrix().getRowGroupIndices()[state+1]) {
                                originalOptimalChoices[state] = chosenRowInOriginalModel - model.getTransitionMatrix().getRowGroupIndices()[state];
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
                        if(possibleBottomStates.get(state)) {
                            bottomStates.set(state);
                        } else {
                            statesWithUndefSched.set(state);
                        }
                    }
                }
                
                // Handle bottom states
                for(auto state : bottomStates) {
                    bool foundRowForState = false;
                    // Find a row with zero rewards that only leads to bottom states.
                    // If the state should stay in its EC, we also need to make sure that all successors map to the same state in the reduced model
                    uint_fast64_t stateInReducedModel = originalToReducedStateMapping[state];
                    for(uint_fast64_t row = model.getTransitionMatrix().getRowGroupIndices()[state]; row < model.getTransitionMatrix().getRowGroupIndices()[state+1]; ++row) {
                        bool rowOnlyLeadsToBottomStates = true;
                        bool rowStaysInEC = true;
                        for( auto const& entry : model.getTransitionMatrix().getRow(row)) {
                            rowOnlyLeadsToBottomStates &= bottomStates.get(entry.getColumn());
                            rowStaysInEC &= originalToReducedStateMapping[entry.getColumn()] == stateInReducedModel;
                        }
                        if(rowOnlyLeadsToBottomStates && (rowStaysInEC || !statesThatShouldStayInTheirEC.get(state)) && actionsWithoutRewardInUnboundedPhase.get(row)) {
                            foundRowForState = true;
                            originalOptimalChoices[state] = row - model.getTransitionMatrix().getRowGroupIndices()[state];
                            break;
                        }
                    }
                    STORM_LOG_ASSERT(foundRowForState, "Could not find a suitable choice for a bottom state.");
                }
                
                // Handle remaining states with still undef. scheduler (either EC states or non-subsystem states)
                while(!statesWithUndefSched.empty()) {
                    for(auto state : statesWithUndefSched) {
                        // Iteratively Try to find a choice such that at least one successor has a defined scheduler.
                        uint_fast64_t stateInReducedModel = originalToReducedStateMapping[state];
                        for(uint_fast64_t row = model.getTransitionMatrix().getRowGroupIndices()[state]; row < model.getTransitionMatrix().getRowGroupIndices()[state+1]; ++row) {
                            bool rowStaysInEC = true;
                            bool rowLeadsToDefinedScheduler = false;
                            for(auto const& entry : model.getTransitionMatrix().getRow(row)) {
                                rowStaysInEC &= ( stateInReducedModel == originalToReducedStateMapping[entry.getColumn()]);
                                rowLeadsToDefinedScheduler |= !statesWithUndefSched.get(entry.getColumn());
                            }
                            if(rowLeadsToDefinedScheduler && (rowStaysInEC || !statesThatShouldStayInTheirEC.get(state))) {
                                originalOptimalChoices[state] = row - model.getTransitionMatrix().getRowGroupIndices()[state];
                                statesWithUndefSched.set(state, false);
                            }
                        }
                    }
                }
            }
            
            
            
            template class SparsePcaaWeightVectorChecker<storm::models::sparse::Mdp<double>>;
            template class SparsePcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>;
#ifdef STORM_HAVE_CARL
            template class SparsePcaaWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
            template class SparsePcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
#endif
            
        }
    }
}
