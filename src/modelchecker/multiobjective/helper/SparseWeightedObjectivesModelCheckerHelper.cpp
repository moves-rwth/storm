#include "src/modelchecker/multiobjective/helper/SparseWeightedObjectivesModelCheckerHelper.h"

#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"
#include "src/solver/LinearEquationSolver.h"
#include "src/solver/MinMaxLinearEquationSolver.h"
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
                
                
            }
            
            template <class SparseModelType>
            void SparseWeightedObjectivesModelCheckerHelper<SparseModelType>::check(std::vector<ValueType> const& weightVector) {
                
                unboundedWeightedPhase(weightVector);
                unboundedIndividualPhase(weightVector);
                boundedPhase(weightVector);
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
                

                //Testing..
                storm::storage::BitVector test(64);
                test.set(63);
                std::cout << "Test set index with 0: " << test.getNextSetIndex(0) << std::endl;
                std::cout << "Test set index with 63: " << test.getNextSetIndex(63) << std::endl;
                std::cout << "Test set index with 64: " << test.getNextSetIndex(64) << std::endl;
                
                storm::storage::BitVector actionsWithRewards = storm::utility::vector::filter<ValueType>(weightedRewardVector, [&] (ValueType const& value) -> bool {return !storm::utility::isZero(value);});
                storm::storage::BitVector statesWithRewards(info.preprocessedModel.getNumberOfStates(), false);
                uint_fast64_t currActionIndex = actionsWithRewards.getNextSetIndex(0);
                auto endOfRowGroup = info.preprocessedModel.getTransitionMatrix().getRowGroupIndices().begin() + 1;
                for(uint_fast64_t state = 0; state<info.preprocessedModel.getNumberOfStates(); ++state){
                    if(currActionIndex < *endOfRowGroup){
                        statesWithRewards.set(state);
                        currActionIndex = actionsWithRewards.getNextSetIndex(*endOfRowGroup);
                    }
                    ++endOfRowGroup;
                }
                
                STORM_LOG_WARN("REMOVE VALIDATION CODE");
                for(uint_fast64_t state = 0; state<info.preprocessedModel.getNumberOfStates(); ++state){
                    bool stateHasReward=false;
                    for(uint_fast64_t row = info.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state]; row< info.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[state+1]; ++row) {
                        stateHasReward |= actionsWithRewards.get(row);
                    }
                    STORM_LOG_ERROR_COND(stateHasReward == statesWithRewards.get(state), "statesWithRewardsVector is wrong!!!!");
                }
                
                //Get the states from which a state with reward can be reached.
                storm::storage::BitVector maybeStates = storm::utility::graph::performProbGreater0E(info.preprocessedModel.getTransitionMatrix(),
                                                                                                    info.preprocessedModel.getTransitionMatrix().getRowGroupIndices(),
                                                                                                    info.preprocessedModel.getBackwardTransitions(),
                                                                                                    storm::storage::BitVector(info.preprocessedModel.getNumberOfStates(), true),
                                                                                                    statesWithRewards);
                
                this->weightedResult = std::vector<ValueType>(info.preprocessedModel.getNumberOfStates());
                this->scheduler = storm::storage::TotalScheduler(info.preprocessedModel.getNumberOfStates());
                
                storm::storage::SparseMatrix<ValueType> submatrix = info.preprocessedModel.getTransitionMatrix().getSubmatrix(true, maybeStates, maybeStates, false);
                std::vector<ValueType> b(submatrix.getRowCount());
                storm::utility::vector::selectVectorValues(b, maybeStates, weightedRewardVector);
                std::vector<ValueType> x(submatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
                
                storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> solverFactory;
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = solverFactory.create(submatrix, true);
                solver->solveEquationSystem(x, b);
                
                storm::utility::vector::setVectorValues(this->weightedResult, maybeStates, x);
                storm::utility::vector::setVectorValues(this->weightedResult, ~maybeStates, storm::utility::zero<ValueType>());
                
                uint_fast64_t currentSubState = 0;
                for (auto maybeState : maybeStates) {
                    this->scheduler.setChoice(maybeState, solver->getScheduler().getChoice(currentSubState));
                    ++currentSubState;
                }
                // Note that the choices for the ~maybeStates are arbitrary as no states with rewards are reachable any way.

            }
            
            template <class SparseModelType>
            void SparseWeightedObjectivesModelCheckerHelper<SparseModelType>::unboundedIndividualPhase(std::vector<ValueType> const& weightVector) {
                
                storm::storage::SparseMatrix<ValueType> deterministicMatrix = info.preprocessedModel.getTransitionMatrix().selectRowsFromRowGroups(this->scheduler.getChoices(), true);
                storm::storage::SparseMatrix<ValueType> deterministicBackwardTransitions = deterministicMatrix.transpose();
                std::vector<ValueType> deterministicStateRewards(deterministicMatrix.getRowCount());
                storm::utility::solver::LinearEquationSolverFactory<ValueType> linearEquationSolverFactory;
                //TODO check if all but one entry of weightVector is zero
                for(uint_fast64_t objIndex = 0; objIndex < weightVector.size(); ++objIndex) {
                    if(!info.objectives[objIndex].stepBound){
                    
                        storm::utility::vector::selectVectorValues(deterministicStateRewards, this->scheduler.getChoices(), info.preprocessedModel.getTransitionMatrix().getRowGroupIndices(), info.preprocessedModel.getRewardModel(info.objectives[objIndex].rewardModelName).getStateActionRewardVector());
                        
                        storm::storage::BitVector statesWithRewards =  storm::utility::vector::filter<ValueType>(deterministicStateRewards, [&] (ValueType const& value) -> bool {return !storm::utility::isZero(value);});
                        // As target states, we take the states from which no reward is reachable.
                        STORM_LOG_WARN("TODO: target state selection is currently only valid for reachability properties...");
                        //TODO: we should be able to give some hint to the solver..
                        storm::storage::BitVector targetStates = storm::utility::graph::performProbGreater0(deterministicBackwardTransitions, storm::storage::BitVector(deterministicMatrix.getRowCount(), true), statesWithRewards);
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
