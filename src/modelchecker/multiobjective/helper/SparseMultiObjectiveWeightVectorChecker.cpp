#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveWeightVectorChecker.h"

#include "src/adapters/CarlAdapter.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"
#include "src/solver/MinMaxLinearEquationSolver.h"
#include "src/transformer/NeutralECRemover.h"
#include "src/utility/graph.h"
#include "src/utility/macros.h"
#include "src/utility/solver.h"
#include "src/utility/vector.h"

#include "src/exceptions/IllegalFunctionCallException.h"

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
                STORM_LOG_DEBUG("Invoked WeightVectorChecker with weights " << std::endl << "\t" << weightVector);
                unboundedWeightedPhase(weightVector);
                STORM_LOG_DEBUG("Unbounded weighted phase result: " << weightedResult[data.preprocessedModel.getInitialStates().getNextSetIndex(0)] << " (value in initial state).");
                unboundedIndividualPhase(weightVector);
                STORM_LOG_DEBUG("Unbounded individual phase results in initial state: " << getInitialStateResultOfObjectives());
                boundedPhase(weightVector);
                STORM_LOG_DEBUG("Bounded individual phase results in initial state: " << getInitialStateResultOfObjectives() << " ...WeightVectorChecker done.");
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
                return scheduler;
            }
            
            template <class SparseModelType>
            void SparseMultiObjectiveWeightVectorChecker<SparseModelType>::unboundedWeightedPhase(std::vector<ValueType> const& weightVector) {
                bool hasUnboundedObjective;
                std::vector<ValueType> weightedRewardVector(data.preprocessedModel.getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                for(uint_fast64_t objIndex = 0; objIndex < weightVector.size(); ++objIndex) {
                    if(!data.objectives[objIndex].stepBound){
                        hasUnboundedObjective = true;
                        storm::utility::vector::addScaledVector(weightedRewardVector, data.preprocessedModel.getRewardModel(data.objectives[objIndex].rewardModelName).getStateActionRewardVector(), weightVector[objIndex]);
                    }
                }
                if(!hasUnboundedObjective) {
                    this->weightedResult = std::vector<ValueType>(data.preprocessedModel.getNumberOfStates(), storm::utility::zero<ValueType>());
                    this->scheduler = storm::storage::TotalScheduler(data.preprocessedModel.getNumberOfStates());
                    return;
                }
                
                // TODO check for +/- infty reward...
                
                //std::cout << "weighted reward vector is " << storm::utility::vector::toString(weightedRewardVector) << std::endl;
                
                // Remove end components in which no reward is earned
                auto removerResult = storm::transformer::NeutralECRemover<ValueType>::transform(data.preprocessedModel.getTransitionMatrix(), weightedRewardVector, storm::storage::BitVector(data.preprocessedModel.getTransitionMatrix().getRowGroupCount(), true));
                
                std::vector<ValueType> subResult(removerResult.matrix.getRowGroupCount());
                
                storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> solverFactory;
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = solverFactory.create(removerResult.matrix, true);
                solver->setOptimizationDirection(storm::solver::OptimizationDirection::Maximize);
                solver->solveEquationSystem(subResult, removerResult.vector);
                
                this->weightedResult = std::vector<ValueType>(data.preprocessedModel.getNumberOfStates());
                this->scheduler = storm::storage::TotalScheduler(data.preprocessedModel.getNumberOfStates());
                storm::storage::BitVector statesWithUndefinedScheduler(data.preprocessedModel.getNumberOfStates(), false);
                for(uint_fast64_t state = 0; state < data.preprocessedModel.getNumberOfStates(); ++state) {
                    uint_fast64_t stateInReducedModel = removerResult.oldToNewStateMapping[state];
                    // Check if the state exists in the reduced model
                    if(stateInReducedModel < removerResult.matrix.getRowGroupCount()) {
                        this->weightedResult[state] = subResult[stateInReducedModel];
                        // Check if the chosen row originaly belonged to the current state
                        uint_fast64_t chosenRowInReducedModel = removerResult.matrix.getRowGroupIndices()[stateInReducedModel] + solver->getScheduler().getChoice(stateInReducedModel);
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
                storm::utility::solver::LinearEquationSolverFactory<ValueType> linearEquationSolverFactory;
                //TODO check if all but one entry of weightVector is zero
                //Also only compute values for objectives with weightVector != zero,
                //one check can be omitted as the result can be computed back from the weighed result and the results from the remaining objectives
                for(uint_fast64_t objIndex = 0; objIndex < weightVector.size(); ++objIndex) {
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
            void SparseMultiObjectiveWeightVectorChecker<SparseModelType>::boundedPhase(std::vector<ValueType> const& weightVector) {
                for(uint_fast64_t objIndex = 0; objIndex < weightVector.size(); ++objIndex) {
                    STORM_LOG_THROW(!data.objectives[objIndex].stepBound, storm::exceptions::IllegalFunctionCallException, "Bounded objectives not yet implemented.");
                }
            }
                
     
            
            template class SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::Mdp<double>>;
            template std::vector<double> SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::Mdp<double>>::getInitialStateResultOfObjectives<double>() const;
#ifdef STORM_HAVE_CARL
            template std::vector<storm::RationalNumber> SparseMultiObjectiveWeightVectorChecker<storm::models::sparse::Mdp<double>>::getInitialStateResultOfObjectives<storm::RationalNumber>() const;
#endif
            
        }
    }
}
