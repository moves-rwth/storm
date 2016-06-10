#include "src/modelchecker/multiobjective/helper/SparseWeightedObjectivesModelCheckerHelper.h"

#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"
#include "src/solver/LinearEquationSolver.h"
#include "src/solver/MinMaxLinearEquationSolver.h"
#include "src/transformer/EffectlessMECRemover.h"
#include "src/utility/graph.h"
#include "src/utility/macros.h"
#include "src/utility/solver.h"
#include "src/utility/vector.h"
#include "src/exceptions/IllegalFunctionCallException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            

            template <class SparseModelType>
            SparseWeightedObjectivesModelCheckerHelper<SparseModelType>::SparseWeightedObjectivesModelCheckerHelper(Information const& info) : info(info), checkHasBeenCalled(false) , objectiveResults(info.objectives.size()){
                //Intentionally left empty
            }
            
            template <class SparseModelType>
            void SparseWeightedObjectivesModelCheckerHelper<SparseModelType>::check(std::vector<ValueType> const& weightVector) {
                STORM_LOG_DEBUG("Checking weighted objectives with weight vector " << std::endl << "\t" << weightVector);
                unboundedWeightedPhase(weightVector);
                STORM_LOG_DEBUG("Unbounded weighted phase resulted in " << std::endl << "\t Result: " << weightedResult << std::endl << "\t Scheduler: " << scheduler);
                unboundedIndividualPhase(weightVector);
                STORM_LOG_DEBUG("Unbounded individual phase resulted in...");
                for(uint_fast64_t objIndex = 0; objIndex < info.objectives.size(); ++objIndex) {
                    STORM_LOG_DEBUG("\t objective " << objIndex << ":" << objectiveResults[objIndex]);
                }
                boundedPhase(weightVector);
                STORM_LOG_DEBUG("Bounded phase resulted in...");
                for(uint_fast64_t objIndex = 0; objIndex < info.objectives.size(); ++objIndex) {
                    STORM_LOG_DEBUG("\t objective " << objIndex << ":" << objectiveResults[objIndex]);
                }
                checkHasBeenCalled=true;
            }
            
            template <class SparseModelType>
            std::vector<typename SparseWeightedObjectivesModelCheckerHelper<SparseModelType>::ValueType> SparseWeightedObjectivesModelCheckerHelper<SparseModelType>::getInitialStateResultOfObjectives() const {
                STORM_LOG_THROW(checkHasBeenCalled, storm::exceptions::IllegalFunctionCallException, "Tried to retrieve results but check(..) has not been called before.");
                STORM_LOG_ASSERT(info.preprocessedModel.getInitialStates().getNumberOfSetBits()==1, "The considered model has multiple initial states");
                std::vector<ValueType> res;
                res.reserve(objectiveResults.size());
                for(auto const& objResult : objectiveResults) {
                    res.push_back(objResult[*info.preprocessedModel.getInitialStates().begin()]);
                }
                return res;
            }
            
            template <class SparseModelType>
            storm::storage::TotalScheduler const& SparseWeightedObjectivesModelCheckerHelper<SparseModelType>::getScheduler() const {
                STORM_LOG_THROW(checkHasBeenCalled, storm::exceptions::IllegalFunctionCallException, "Tried to retrieve results but check(..) has not been called before.");
                return scheduler;
            }
            
            template <class SparseModelType>
            void SparseWeightedObjectivesModelCheckerHelper<SparseModelType>::unboundedWeightedPhase(std::vector<ValueType> const& weightVector) {
                std::vector<ValueType> weightedRewardVector(info.preprocessedModel.getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                for(uint_fast64_t objIndex = 0; objIndex < weightVector.size(); ++objIndex) {
                    if(!info.objectives[objIndex].stepBound){
                        storm::utility::vector::addScaledVector(weightedRewardVector, info.preprocessedModel.getRewardModel(info.objectives[objIndex].rewardModelName).getStateActionRewardVector(), weightVector[objIndex]);
                    }
                }
                
                // TODO check for +/- infty reward...
                
                //std::cout << "weighted reward vector is " << storm::utility::vector::toString(weightedRewardVector) << std::endl;
                
                // Remove end components in which no reward is earned
                auto removerResult = storm::transformer::EffectlessMECRemover<ValueType>::transform(info.preprocessedModel.getTransitionMatrix(), weightedRewardVector, storm::storage::BitVector(info.preprocessedModel.getTransitionMatrix().getRowGroupCount(), true));
                
                std::vector<ValueType> subResult(removerResult.matrix.getRowGroupCount());
                
                storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> solverFactory;
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = solverFactory.create(removerResult.matrix, true);
                solver->setOptimizationDirection(storm::solver::OptimizationDirection::Maximize);
                solver->solveEquationSystem(subResult, removerResult.vector);
                
                this->weightedResult = std::vector<ValueType>(info.preprocessedModel.getNumberOfStates());
                this->scheduler = storm::storage::TotalScheduler(info.preprocessedModel.getNumberOfStates());
                storm::storage::BitVector statesWithUndefinedScheduler(info.preprocessedModel.getNumberOfStates(), false);
                for(uint_fast64_t state = 0; state < info.preprocessedModel.getNumberOfStates(); ++state) {
                    uint_fast64_t stateInReducedModel = removerResult.oldToNewStateMapping[state];
                    // Check if the state exists in the reduced model
                    if(stateInReducedModel < removerResult.matrix.getRowGroupCount()) {
                        this->weightedResult[state] = subResult[stateInReducedModel];
                        // Check if the chosen row originaly belonged to the current state
                        uint_fast64_t chosenRowInReducedModel = removerResult.matrix.getRowGroupIndices()[stateInReducedModel] + solver->getScheduler().getChoice(stateInReducedModel);
                        uint_fast64_t chosenRowInOriginalModel = removerResult.newToOldRowMapping[chosenRowInReducedModel];
                        if(chosenRowInOriginalModel >= info.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state] &&
                           chosenRowInOriginalModel <  info.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state+1]) {
                            this->scheduler.setChoice(state, chosenRowInOriginalModel - info.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state]);
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
                        // This way, a scheduler is chosen that leads (with probability one) to the state from which the EC can be left
                        uint_fast64_t stateInReducedModel = removerResult.oldToNewStateMapping[state];
                        for(uint_fast64_t row = info.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state]; row < info.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state+1]; ++row) {
                            bool rowStaysInEC = true;
                            bool rowLeadsToDefinedScheduler = false;
                            for(auto const& entry : info.preprocessedModel.getTransitionMatrix().getRow(row)) {
                                rowStaysInEC &= ( stateInReducedModel == removerResult.oldToNewStateMapping[entry.getColumn()]);
                                rowLeadsToDefinedScheduler |= !statesWithUndefinedScheduler.get(entry.getColumn());
                            }
                            if(rowStaysInEC && rowLeadsToDefinedScheduler) {
                                this->scheduler.setChoice(state, row - info.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state]);
                                statesWithUndefinedScheduler.set(state, false);
                            }
                        }
                    }
                }
            }
            
            template <class SparseModelType>
            void SparseWeightedObjectivesModelCheckerHelper<SparseModelType>::unboundedIndividualPhase(std::vector<ValueType> const& weightVector) {
                
                storm::storage::SparseMatrix<ValueType> deterministicMatrix = info.preprocessedModel.getTransitionMatrix().selectRowsFromRowGroups(this->scheduler.getChoices(), true);
                storm::storage::SparseMatrix<ValueType> deterministicBackwardTransitions = deterministicMatrix.transpose();
                std::vector<ValueType> deterministicStateRewards(deterministicMatrix.getRowCount());
                storm::utility::solver::LinearEquationSolverFactory<ValueType> linearEquationSolverFactory;
                //TODO check if all but one entry of weightVector is zero
                //Also only compute values for objectives with weightVector != zero,
                //one check can be omitted as the result can be computed back from the weighed result and the results from the remaining objectives
                for(uint_fast64_t objIndex = 0; objIndex < weightVector.size(); ++objIndex) {
                    if(!info.objectives[objIndex].stepBound){
                    
                        storm::utility::vector::selectVectorValues(deterministicStateRewards, this->scheduler.getChoices(), info.preprocessedModel.getTransitionMatrix().getRowGroupIndices(), info.preprocessedModel.getRewardModel(info.objectives[objIndex].rewardModelName).getStateActionRewardVector());
                        
                        //std::cout << "stateActionRewardVector for objective " << objIndex << " is " << storm::utility::vector::toString(info.preprocessedModel.getRewardModel(info.objectives[objIndex].rewardModelName).getStateActionRewardVector()) << std::endl;
                        //std::cout << "deterministic state rewards for objective " << objIndex << " are " << storm::utility::vector::toString(deterministicStateRewards) << std::endl;
                        
                        storm::storage::BitVector statesWithRewards =  storm::utility::vector::filter<ValueType>(deterministicStateRewards, [&] (ValueType const& value) -> bool {return !storm::utility::isZero(value);});
                        // As target states, we take the states from which no reward is reachable.
                        STORM_LOG_WARN("TODO: target state selection is currently only valid for reachability properties...");
                        //TODO: we should be able to give some hint to the solver..
                        storm::storage::BitVector targetStates = storm::utility::graph::performProbGreater0(deterministicBackwardTransitions, storm::storage::BitVector(deterministicMatrix.getRowCount(), true), statesWithRewards);
                        targetStates.complement();
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
            void SparseWeightedObjectivesModelCheckerHelper<SparseModelType>::boundedPhase(std::vector<ValueType> const& weightVector) {
                STORM_LOG_WARN("bounded properties not yet implemented");
            }
                
     
            
            template class SparseWeightedObjectivesModelCheckerHelper<storm::models::sparse::Mdp<double>>;

            
        }
    }
}
