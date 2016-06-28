#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveWeightVectorChecker.h"

#include <map>

#include "src/adapters/CarlAdapter.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/MarkovAutomaton.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"
#include "src/solver/MinMaxLinearEquationSolver.h"
#include "src/transformer/NeutralECRemover.h"
#include "src/utility/graph.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            

            template <class SparseModelType>
            SparseMultiObjectiveWeightVectorChecker<SparseModelType>::SparseMultiObjectiveWeightVectorChecker(PreprocessorData const& data) : data(data), checkHasBeenCalled(false) , objectiveResults(data.objectives.size()){
                
                // Enlarge the set of prob1 states to the states that are only reachable via prob1 states
                statesThatAreAllowedToBeVisitedInfinitelyOften = ~storm::utility::graph::getReachableStates(data.preprocessedModel.getTransitionMatrix(), data.preprocessedModel.getInitialStates(), ~data.preprocessedModel.getStates(data.prob1StatesLabel), storm::storage::BitVector(data.preprocessedModel.getNumberOfStates(), false));
            }
            
            template <class SparseModelType>
            void SparseMultiObjectiveWeightVectorChecker<SparseModelType>::check(std::vector<ValueType> const& weightVector) {
                checkHasBeenCalled=true;
                STORM_LOG_DEBUG("Invoked WeightVectorChecker with weights " << std::endl << "\t" << storm::utility::vector::convertNumericVector<double>(weightVector));
                storm::storage::BitVector unboundedObjectives(data.objectives.size(), false);
                std::vector<ValueType> weightedRewardVector(data.preprocessedModel.getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                for(uint_fast64_t objIndex = 0; objIndex < data.objectives.size(); ++objIndex) {
                    if(!data.objectives[objIndex].stepBound) {
                        unboundedObjectives.set(objIndex, true);
                        storm::utility::vector::addScaledVector(weightedRewardVector, data.preprocessedModel.getRewardModel(data.objectives[objIndex].rewardModelName).getStateActionRewardVector(), weightVector[objIndex]);
                    }
                }
                unboundedWeightedPhase(unboundedObjectives, weightedRewardVector);
                STORM_LOG_DEBUG("Unbounded weighted phase result: " << weightedResult[data.preprocessedModel.getInitialStates().getNextSetIndex(0)] << " (value in initial state).");
                unboundedIndividualPhase(weightVector);
                STORM_LOG_DEBUG("Unbounded individual phase results in initial state: " << getInitialStateResultOfObjectives<double>());
                boundedPhase(weightVector, ~unboundedObjectives, weightedRewardVector);
                STORM_LOG_DEBUG("Bounded individual phase results in initial state: " << getInitialStateResultOfObjectives<double>() << " ...WeightVectorChecker done.");
            }
            
            template <class SparseModelType>
            template<typename TargetValueType>
            std::vector<TargetValueType> SparseMultiObjectiveWeightVectorChecker<SparseModelType>::getInitialStateResultOfObjectives() const {
                STORM_LOG_THROW(checkHasBeenCalled, storm::exceptions::IllegalFunctionCallException, "Tried to retrieve results but check(..) has not been called before.");
                STORM_LOG_ASSERT(data.preprocessedModel.getInitialStates().getNumberOfSetBits()==1, "The considered model has multiple initial states");
                std::vector<TargetValueType> res;
                res.reserve(objectiveResults.size());
                for(auto const& objResult : objectiveResults) {
                    res.push_back(storm::utility::convertNumber<TargetValueType>(objResult[*data.preprocessedModel.getInitialStates().begin()]));
                }
                return res;
            }
            
            template <class SparseModelType>
            storm::storage::TotalScheduler const& SparseMultiObjectiveWeightVectorChecker<SparseModelType>::getScheduler() const {
                STORM_LOG_THROW(checkHasBeenCalled, storm::exceptions::IllegalFunctionCallException, "Tried to retrieve results but check(..) has not been called before.");
                for(auto const& obj : data.objectives) {
                    STORM_LOG_THROW(!obj.stepBound, storm::exceptions::NotImplementedException, "Scheduler retrival is not implemented for stepbounded objectives.");
                }
                return scheduler;
            }
            
            template <class SparseModelType>
            void SparseMultiObjectiveWeightVectorChecker<SparseModelType>::unboundedWeightedPhase(storm::storage::BitVector const& unboundedObjectives, std::vector<ValueType> const& weightedRewardVector) {
                if(unboundedObjectives.empty()) {
                    this->weightedResult = std::vector<ValueType>(data.preprocessedModel.getNumberOfStates(), storm::utility::zero<ValueType>());
                    this->scheduler = storm::storage::TotalScheduler(data.preprocessedModel.getNumberOfStates());
                    return;
                }
                // Remove end components in which no reward is earned
                auto removerResult = storm::transformer::NeutralECRemover<ValueType>::transform(data.preprocessedModel.getTransitionMatrix(), weightedRewardVector, storm::storage::BitVector(data.preprocessedModel.getTransitionMatrix().getRowGroupCount(), true));
                
                std::vector<ValueType> subResult(removerResult.matrix.getRowGroupCount());
                
                storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> solverFactory;
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = solverFactory.create(removerResult.matrix);
                solver->setOptimizationDirection(storm::solver::OptimizationDirection::Maximize);
                solver->setTrackScheduler(true);
                solver->solveEquations(subResult, removerResult.vector);
                
                this->weightedResult = std::vector<ValueType>(data.preprocessedModel.getNumberOfStates());
                this->scheduler = storm::storage::TotalScheduler(data.preprocessedModel.getNumberOfStates());
                std::unique_ptr<storm::storage::TotalScheduler>  solverScheduler = solver->getScheduler();
                storm::storage::BitVector statesWithUndefinedScheduler(data.preprocessedModel.getNumberOfStates(), false);
                for(uint_fast64_t state = 0; state < data.preprocessedModel.getNumberOfStates(); ++state) {
                    uint_fast64_t stateInReducedModel = removerResult.oldToNewStateMapping[state];
                    // Check if the state exists in the reduced model
                    if(stateInReducedModel < removerResult.matrix.getRowGroupCount()) {
                        this->weightedResult[state] = subResult[stateInReducedModel];
                        // Check if the chosen row originaly belonged to the current state
                        uint_fast64_t chosenRowInReducedModel = removerResult.matrix.getRowGroupIndices()[stateInReducedModel] + solverScheduler->getChoice(stateInReducedModel);
                        uint_fast64_t chosenRowInOriginalModel = removerResult.newToOldRowMapping[chosenRowInReducedModel];
                        if(chosenRowInOriginalModel >= data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state] &&
                           chosenRowInOriginalModel <  data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state+1]) {
                            this->scheduler.setChoice(state, chosenRowInOriginalModel - data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state]);
                        } else {
                            statesWithUndefinedScheduler.set(state);
                        }
                    } else {
                        // if the state does not exist in the reduced model, it means that the result is always zero, independent of the scheduler.
                        // Hence, we don't have to set the scheduler explicitly
                        this->weightedResult[state] = storm::utility::zero<ValueType>();
                    }
                }
                while(!statesWithUndefinedScheduler.empty()) {
                    for(auto state : statesWithUndefinedScheduler) {
                        // Try to find a choice that stays inside the EC (i.e., for which all successors are represented by the same state in the reduced model)
                        // And at least one successor has a defined scheduler.
                        // This way, a scheduler is chosen that leads (with probability one) to the state of the EC for which the scheduler is defined
                        uint_fast64_t stateInReducedModel = removerResult.oldToNewStateMapping[state];
                        for(uint_fast64_t row = data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state]; row < data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state+1]; ++row) {
                            bool rowStaysInEC = true;
                            bool rowLeadsToDefinedScheduler = false;
                            for(auto const& entry : data.preprocessedModel.getTransitionMatrix().getRow(row)) {
                                rowStaysInEC &= ( stateInReducedModel == removerResult.oldToNewStateMapping[entry.getColumn()]);
                                rowLeadsToDefinedScheduler |= !statesWithUndefinedScheduler.get(entry.getColumn());
                            }
                            if(rowStaysInEC && rowLeadsToDefinedScheduler) {
                                this->scheduler.setChoice(state, row - data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state]);
                                statesWithUndefinedScheduler.set(state, false);
                            }
                        }
                    }
                }
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
                    if(data.objectives[objIndex].stepBound){
                        objectiveResults[objIndex] = std::vector<ValueType>(data.preprocessedModel.getNumberOfStates(), storm::utility::zero<ValueType>());
                    } else {
                        storm::utility::vector::selectVectorValues(deterministicStateRewards, this->scheduler.getChoices(), data.preprocessedModel.getTransitionMatrix().getRowGroupIndices(), data.preprocessedModel.getRewardModel(data.objectives[objIndex].rewardModelName).getStateActionRewardVector());
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
                    }
                }
            }
            
            template <class SparseModelType>
            void SparseMultiObjectiveWeightVectorChecker<SparseModelType>::boundedPhase(std::vector<ValueType> const& weightVector, storm::storage::BitVector const& boundedObjectives, std::vector<ValueType>& weightedRewardVector) {
                if(boundedObjectives.empty()) {
                    return;
                }
                // Allocate some memory so this does not need to happen for each time epoch
                std::vector<uint_fast64_t> optimalChoicesInCurrentEpoch(data.preprocessedModel.getNumberOfStates());
                std::vector<ValueType> choiceValues(weightedRewardVector.size());
                std::vector<ValueType> temporaryResult(data.preprocessedModel.getNumberOfStates());
                // Get for each occurring stepBound the indices of the objectives with that bound.
                std::map<uint_fast64_t, storm::storage::BitVector, std::greater<uint_fast64_t>> stepBounds;
                for(auto objIndex : boundedObjectives) {
                    auto stepBoundIt = stepBounds.insert(std::make_pair(*data.objectives[objIndex].stepBound, storm::storage::BitVector(data.objectives.size(), false))).first;
                        stepBoundIt->second.set(objIndex);
                }
                storm::storage::BitVector objectivesAtCurrentEpoch = ~boundedObjectives;
                auto stepBoundIt = stepBounds.begin();
                for(uint_fast64_t currentEpoch = stepBoundIt->first; currentEpoch > 0; --currentEpoch) {
                    if(stepBoundIt != stepBounds.end() && currentEpoch == stepBoundIt->first) {
                        objectivesAtCurrentEpoch |= stepBoundIt->second;
                        for(auto objIndex : stepBoundIt->second) {
                            storm::utility::vector::addScaledVector(weightedRewardVector, data.preprocessedModel.getRewardModel(data.objectives[objIndex].rewardModelName).getStateActionRewardVector(), weightVector[objIndex]);
                        }
                        ++stepBoundIt;
                    }
                    
                    // Get values and scheduler for weighted sum of objectives
                    data.preprocessedModel.getTransitionMatrix().multiplyWithVector(weightedResult, choiceValues);
                    storm::utility::vector::addVectors(choiceValues, weightedRewardVector, choiceValues);
                    storm::utility::vector::reduceVectorMax(choiceValues, weightedResult, data.preprocessedModel.getTransitionMatrix().getRowGroupIndices(), &optimalChoicesInCurrentEpoch);
                    
                    // get values for individual objectives
                    // TODO we could compute the result for one of the objectives from the weighted result, the given weight vector, and the remaining objective results.
                    for(auto objIndex : objectivesAtCurrentEpoch) {
                        std::vector<ValueType>& objectiveResult = objectiveResults[objIndex];
                        std::vector<ValueType> const& objectiveRewards = data.preprocessedModel.getRewardModel(data.objectives[objIndex].rewardModelName).getStateActionRewardVector();
                        auto rowGroupIndexIt = data.preprocessedModel.getTransitionMatrix().getRowGroupIndices().begin();
                        auto optimalChoiceIt = optimalChoicesInCurrentEpoch.begin();
                        for(ValueType& stateValue : temporaryResult){
                            uint_fast64_t row = (*rowGroupIndexIt) + (*optimalChoiceIt);
                            ++rowGroupIndexIt;
                            ++optimalChoiceIt;
                            stateValue = objectiveRewards[row];
                            for(auto const& entry : data.preprocessedModel.getTransitionMatrix().getRow(row)) {
                                stateValue += entry.getValue() * objectiveResult[entry.getColumn()];
                            }
                        }
                        objectiveResult.swap(temporaryResult);
                    }
                }
            }
                
     
            
            template class SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::Mdp<double>>;
            template std::vector<double> SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::Mdp<double>>::getInitialStateResultOfObjectives<double>() const;
            template class SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>;
            template std::vector<double> SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::getInitialStateResultOfObjectives<double>() const;
#ifdef STORM_HAVE_CARL
            template std::vector<storm::RationalNumber> SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::Mdp<double>>::getInitialStateResultOfObjectives<storm::RationalNumber>() const;
            template std::vector<storm::RationalNumber> SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::getInitialStateResultOfObjectives<storm::RationalNumber>() const;
            
            template class SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
            template std::vector<double> SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>::getInitialStateResultOfObjectives<double>() const;
            template class SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
            template std::vector<double> SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::getInitialStateResultOfObjectives<double>() const;
            template std::vector<storm::RationalNumber> SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>::getInitialStateResultOfObjectives<storm::RationalNumber>() const;
            template std::vector<storm::RationalNumber> SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::getInitialStateResultOfObjectives<storm::RationalNumber>() const;
#endif
            
        }
    }
}
