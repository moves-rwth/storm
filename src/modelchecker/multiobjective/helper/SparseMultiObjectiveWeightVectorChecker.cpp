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
            SparseMultiObjectiveWeightVectorChecker<SparseModelType>::SparseMultiObjectiveWeightVectorChecker(PreprocessorData const& data) : data(data), unboundedObjectives(data.objectives.size()), discreteActionRewards(data.objectives.size()), checkHasBeenCalled(false), objectiveResults(data.objectives.size()){
                
                // set the unbounded objectives
                for(uint_fast64_t objIndex = 0; objIndex < data.objectives.size(); ++objIndex) {
                    unboundedObjectives.set(objIndex, !data.objectives[objIndex].timeBounds);
                }
                // Enlarge the set of prob1 states to the states that are only reachable via prob1 states
                statesThatAreAllowedToBeVisitedInfinitelyOften = ~storm::utility::graph::getReachableStates(data.preprocessedModel.getTransitionMatrix(), data.preprocessedModel.getInitialStates(), ~data.preprocessedModel.getStates(data.prob1StatesLabel), storm::storage::BitVector(data.preprocessedModel.getNumberOfStates(), false));
            }
            
            
            template <class SparseModelType>
            void SparseMultiObjectiveWeightVectorChecker<SparseModelType>::check(std::vector<ValueType> const& weightVector) {
                checkHasBeenCalled=true;
                STORM_LOG_DEBUG("Invoked WeightVectorChecker with weights " << std::endl << "\t" << storm::utility::vector::convertNumericVector<double>(weightVector));
                std::vector<ValueType> weightedRewardVector(data.preprocessedModel.getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                for(auto objIndex : unboundedObjectives) {
                    storm::utility::vector::addScaledVector(weightedRewardVector, discreteActionRewards[objIndex], weightVector[objIndex]);
                }
                unboundedWeightedPhase(weightedRewardVector);
                STORM_LOG_DEBUG("Unbounded weighted phase result: " << weightedResult[data.preprocessedModel.getInitialStates().getNextSetIndex(0)] << " (value in initial state).");
                unboundedIndividualPhase(weightVector);
                STORM_LOG_DEBUG("Unbounded individual phase results in initial state: " << getInitialStateResultOfObjectives<double>());
                if(!this->unboundedObjectives.full()) {
                    boundedPhase(weightVector, weightedRewardVector);
                    STORM_LOG_DEBUG("Bounded individual phase results in initial state: " << getInitialStateResultOfObjectives<double>() << " ...WeightVectorChecker done.");
                }
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
                STORM_LOG_THROW(this->checkHasBeenCalled, storm::exceptions::IllegalFunctionCallException, "Tried to retrieve results but check(..) has not been called before.");
                STORM_LOG_THROW(this->unboundedObjectives.full(), storm::exceptions::NotImplementedException, "Scheduler retrival is not implemented for timeBounded objectives.");
                return scheduler;
            }
            
            template <class SparseModelType>
            void SparseMultiObjectiveWeightVectorChecker<SparseModelType>::unboundedWeightedPhase(std::vector<ValueType> const& weightedRewardVector) {
                if(this->unboundedObjectives.empty()) {
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
                    if(unboundedObjectives.get(objIndex)){
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
