#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveWeightVectorChecker.h"

#include <map>

#include "src/adapters/CarlAdapter.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/MarkovAutomaton.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"
#include "src/solver/MinMaxLinearEquationSolver.h"
#include "src/transformer/EndComponentEliminator.h"
#include "src/utility/graph.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            

            template <class SparseModelType>
            SparseMultiObjectiveWeightVectorChecker<SparseModelType>::SparseMultiObjectiveWeightVectorChecker(PreprocessorData const& data) : data(data), objectivesWithNoUpperTimeBound(data.objectives.size()), discreteActionRewards(data.objectives.size()), checkHasBeenCalled(false), objectiveResults(data.objectives.size()), offsetsToLowerBound(data.objectives.size()), offsetsToUpperBound(data.objectives.size()) {
                
                // set the unbounded objectives
                for(uint_fast64_t objIndex = 0; objIndex < data.objectives.size(); ++objIndex) {
                    objectivesWithNoUpperTimeBound.set(objIndex, !data.objectives[objIndex].upperTimeBound);
                }
                // Enlarge the set of prob1 states to the states that are only reachable via prob1 states
                statesThatAreAllowedToBeVisitedInfinitelyOften = ~storm::utility::graph::getReachableStates(data.preprocessedModel.getTransitionMatrix(), data.preprocessedModel.getInitialStates(), ~data.preprocessedModel.getStates(data.prob1StatesLabel), storm::storage::BitVector(data.preprocessedModel.getNumberOfStates(), false));
            }
            
            
            template <class SparseModelType>
            void SparseMultiObjectiveWeightVectorChecker<SparseModelType>::check(std::vector<ValueType> const& weightVector) {
                checkHasBeenCalled=true;
                STORM_LOG_DEBUG("Invoked WeightVectorChecker with weights " << std::endl << "\t" << storm::utility::vector::toString(storm::utility::vector::convertNumericVector<double>(weightVector)));
                std::vector<ValueType> weightedRewardVector(data.preprocessedModel.getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                for(auto objIndex : objectivesWithNoUpperTimeBound) {
                    storm::utility::vector::addScaledVector(weightedRewardVector, discreteActionRewards[objIndex], weightVector[objIndex]);
                }
                unboundedWeightedPhase(weightedRewardVector);
                unboundedIndividualPhase(weightVector);
                // Only invoke boundedPhase if necessarry, i.e., if there is at least one objective with a time bound
                for(auto const& obj : this->data.objectives) {
                    if(obj.lowerTimeBound || obj.upperTimeBound) {
                        boundedPhase(weightVector, weightedRewardVector);
                        break;
                    }
                }
                STORM_LOG_DEBUG("Weight vector check done. Lower bounds for results in initial state: " << storm::utility::vector::toString(storm::utility::vector::convertNumericVector<double>(getLowerBoundsOfInitialStateResults())));
            }
            
            template <class SparseModelType>
            void SparseMultiObjectiveWeightVectorChecker<SparseModelType>::setMaximumLowerUpperBoundGap(ValueType const& value) {
                this->maximumLowerUpperBoundGap = value;
            }
            
            template <class SparseModelType>
            typename SparseMultiObjectiveWeightVectorChecker<SparseModelType>::ValueType const& SparseMultiObjectiveWeightVectorChecker<SparseModelType>::getMaximumLowerUpperBoundGap() const {
                return this->maximumLowerUpperBoundGap;
            }
            
            template <class SparseModelType>
            std::vector<typename SparseMultiObjectiveWeightVectorChecker<SparseModelType>::ValueType> SparseMultiObjectiveWeightVectorChecker<SparseModelType>::getLowerBoundsOfInitialStateResults() const {
                STORM_LOG_THROW(checkHasBeenCalled, storm::exceptions::IllegalFunctionCallException, "Tried to retrieve results but check(..) has not been called before.");
                uint_fast64_t initstate = *this->data.preprocessedModel.getInitialStates().begin();
                std::vector<ValueType> res;
                res.reserve(this->data.objectives.size());
                for(uint_fast64_t objIndex = 0; objIndex < this->data.objectives.size(); ++objIndex) {
                    res.push_back(this->objectiveResults[objIndex][initstate] + this->offsetsToLowerBound[objIndex]);
                }
                return res;
            }
            
            template <class SparseModelType>
            std::vector<typename SparseMultiObjectiveWeightVectorChecker<SparseModelType>::ValueType> SparseMultiObjectiveWeightVectorChecker<SparseModelType>::getUpperBoundsOfInitialStateResults() const {
                STORM_LOG_THROW(checkHasBeenCalled, storm::exceptions::IllegalFunctionCallException, "Tried to retrieve results but check(..) has not been called before.");
                uint_fast64_t initstate = *this->data.preprocessedModel.getInitialStates().begin();
                std::vector<ValueType> res;
                res.reserve(this->data.objectives.size());
                for(uint_fast64_t objIndex = 0; objIndex < this->data.objectives.size(); ++objIndex) {
                    res.push_back(this->objectiveResults[objIndex][initstate] + this->offsetsToUpperBound[objIndex]);
                }
                return res;
            }
            
            template <class SparseModelType>
            storm::storage::TotalScheduler const& SparseMultiObjectiveWeightVectorChecker<SparseModelType>::getScheduler() const {
                STORM_LOG_THROW(this->checkHasBeenCalled, storm::exceptions::IllegalFunctionCallException, "Tried to retrieve results but check(..) has not been called before.");
                for(auto const& obj : this->data.objectives) {
                    STORM_LOG_THROW(!obj.lowerTimeBound && !obj.upperTimeBound, storm::exceptions::NotImplementedException, "Scheduler retrival is not implemented for timeBounded objectives.");
                }
                return scheduler;
            }
            
            template <class SparseModelType>
            void SparseMultiObjectiveWeightVectorChecker<SparseModelType>::unboundedWeightedPhase(std::vector<ValueType> const& weightedRewardVector) {
                if(this->objectivesWithNoUpperTimeBound.empty()) {
                    this->weightedResult = std::vector<ValueType>(data.preprocessedModel.getNumberOfStates(), storm::utility::zero<ValueType>());
                    this->scheduler = storm::storage::TotalScheduler(data.preprocessedModel.getNumberOfStates());
                    return;
                }
                // Remove end components in which no reward is earned
                
                auto ecEliminatorResult = storm::transformer::EndComponentEliminator<ValueType>::transform(data.preprocessedModel.getTransitionMatrix(), storm::utility::vector::filterZero(weightedRewardVector), storm::storage::BitVector(data.preprocessedModel.getTransitionMatrix().getRowGroupCount(), true));
                
                std::vector<ValueType> subRewardVector(ecEliminatorResult.newToOldRowMapping.size());
                storm::utility::vector::selectVectorValues(subRewardVector, ecEliminatorResult.newToOldRowMapping, weightedRewardVector);
                std::vector<ValueType> subResult(ecEliminatorResult.matrix.getRowGroupCount());
                
                storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> solverFactory;
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = solverFactory.create(ecEliminatorResult.matrix);
                solver->setOptimizationDirection(storm::solver::OptimizationDirection::Maximize);
                solver->setTrackScheduler(true);
                solver->solveEquations(subResult, subRewardVector);
                
                this->weightedResult = std::vector<ValueType>(data.preprocessedModel.getNumberOfStates());
                std::vector<uint_fast64_t> optimalChoices(data.preprocessedModel.getNumberOfStates());
                
                transformReducedSolutionToOriginalModel(ecEliminatorResult.matrix, subResult, solver->getScheduler()->getChoices(), ecEliminatorResult.newToOldRowMapping, ecEliminatorResult.oldToNewStateMapping, this->data.preprocessedModel.getTransitionMatrix(), this->weightedResult, optimalChoices);
                
                this->scheduler = storm::storage::TotalScheduler(std::move(optimalChoices));
            }
            
            template <class SparseModelType>
            void SparseMultiObjectiveWeightVectorChecker<SparseModelType>::unboundedIndividualPhase(std::vector<ValueType> const& weightVector) {
                
                storm::storage::SparseMatrix<ValueType> deterministicMatrix = data.preprocessedModel.getTransitionMatrix().selectRowsFromRowGroups(this->scheduler.getChoices(), true);
                storm::storage::SparseMatrix<ValueType> deterministicBackwardTransitions = deterministicMatrix.transpose();
                std::vector<ValueType> deterministicStateRewards(deterministicMatrix.getRowCount());
                storm::solver::GeneralLinearEquationSolverFactory<ValueType> linearEquationSolverFactory;
                //TODO check if all but one entry of weightVector is zero
                //Also only compute values for objectives with weightVector != zero,
                //one check can be omitted as the result can be computed back from the weighed result and the results from the remaining objectives
                for(uint_fast64_t objIndex = 0; objIndex < data.objectives.size(); ++objIndex) {
                    if(objectivesWithNoUpperTimeBound.get(objIndex)){
                        offsetsToLowerBound[objIndex] = storm::utility::zero<ValueType>();
                        offsetsToUpperBound[objIndex] = storm::utility::zero<ValueType>();
                        storm::utility::vector::selectVectorValues(deterministicStateRewards, this->scheduler.getChoices(), data.preprocessedModel.getTransitionMatrix().getRowGroupIndices(), discreteActionRewards[objIndex]);
                        storm::storage::BitVector statesWithRewards =  ~storm::utility::vector::filterZero(deterministicStateRewards);
                        // As target states, we pick the states from which no reward is reachable.
                        storm::storage::BitVector targetStates = ~storm::utility::graph::performProbGreater0(deterministicBackwardTransitions, storm::storage::BitVector(deterministicMatrix.getRowCount(), true), statesWithRewards);
                        
                        //TODO: we could give the solver some hint for the result (e.g., weightVector[ObjIndex] * weightedResult or  (weightedResult - sum_i=0^objIndex-1 objectiveResult) * weightVector[objIndex]/ sum_i=objIndex^n weightVector[i] )
                        objectiveResults[objIndex] = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeReachabilityRewards(deterministicMatrix,
                                                                                                                                               deterministicBackwardTransitions,
                                                                                                                                               deterministicStateRewards,
                                                                                                                                               targetStates,
                                                                                                                                               false, //no qualitative checking,
                                                                                                                                               linearEquationSolverFactory);
                    } else {
                        objectiveResults[objIndex] = std::vector<ValueType>(data.preprocessedModel.getNumberOfStates(), storm::utility::zero<ValueType>());
                    }
                }
            }
            
            template <class SparseModelType>
            void SparseMultiObjectiveWeightVectorChecker<SparseModelType>::transformReducedSolutionToOriginalModel(storm::storage::SparseMatrix<ValueType> const& reducedMatrix,
                                                         std::vector<ValueType> const& reducedSolution,
                                                         std::vector<uint_fast64_t> const& reducedOptimalChoices,
                                                         std::vector<uint_fast64_t> const& reducedToOriginalChoiceMapping,
                                                         std::vector<uint_fast64_t> const& originalToReducedStateMapping,
                                                         storm::storage::SparseMatrix<ValueType> const& originalMatrix,
                                                         std::vector<ValueType>& originalSolution,
                                                         std::vector<uint_fast64_t>& originalOptimalChoices) const {
              
                storm::storage::BitVector statesWithUndefinedScheduler(originalMatrix.getRowGroupCount(), false);
                for(uint_fast64_t state = 0; state < originalMatrix.getRowGroupCount(); ++state) {
                    uint_fast64_t stateInReducedModel = originalToReducedStateMapping[state];
                    // Check if the state exists in the reduced model
                    if(stateInReducedModel < reducedMatrix.getRowGroupCount()) {
                        originalSolution[state] = reducedSolution[stateInReducedModel];
                        // Check if the chosen row originaly belonged to the current state
                        uint_fast64_t chosenRowInReducedModel = reducedMatrix.getRowGroupIndices()[stateInReducedModel] + reducedOptimalChoices[stateInReducedModel];
                        uint_fast64_t chosenRowInOriginalModel = reducedToOriginalChoiceMapping[chosenRowInReducedModel];
                        if(chosenRowInOriginalModel >= originalMatrix.getRowGroupIndices()[state] &&
                           chosenRowInOriginalModel <  originalMatrix.getRowGroupIndices()[state+1]) {
                            originalOptimalChoices[state] = chosenRowInOriginalModel - originalMatrix.getRowGroupIndices()[state];
                        } else {
                            statesWithUndefinedScheduler.set(state);
                        }
                    } else {
                        // if the state does not exist in the reduced model, it means that the result is always zero, independent of the scheduler.
                        // Hence, we don't have to set the scheduler explicitly
                        originalSolution[state] = storm::utility::zero<ValueType>();
                    }
                }
                while(!statesWithUndefinedScheduler.empty()) {
                    for(auto state : statesWithUndefinedScheduler) {
                        // Try to find a choice that stays inside the EC (i.e., for which all successors are represented by the same state in the reduced model)
                        // And at least one successor has a defined scheduler.
                        // This way, a scheduler is chosen that leads (with probability one) to the state of the EC for which the scheduler is defined
                        uint_fast64_t stateInReducedModel = originalToReducedStateMapping[state];
                        for(uint_fast64_t row = originalMatrix.getRowGroupIndices()[state]; row < originalMatrix.getRowGroupIndices()[state+1]; ++row) {
                            bool rowStaysInEC = true;
                            bool rowLeadsToDefinedScheduler = false;
                            for(auto const& entry : originalMatrix.getRow(row)) {
                                rowStaysInEC &= ( stateInReducedModel == originalToReducedStateMapping[entry.getColumn()]);
                                rowLeadsToDefinedScheduler |= !statesWithUndefinedScheduler.get(entry.getColumn());
                            }
                            if(rowStaysInEC && rowLeadsToDefinedScheduler) {
                                originalOptimalChoices[state] = row - originalMatrix.getRowGroupIndices()[state];
                                statesWithUndefinedScheduler.set(state, false);
                            }
                        }
                    }
                }
            }
            
            
            
            template class SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::Mdp<double>>;
            template class SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>;
#ifdef STORM_HAVE_CARL
            template class SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
            template class SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
#endif
            
        }
    }
}
