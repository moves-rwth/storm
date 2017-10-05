#include "storm/modelchecker/multiobjective/pcaa/SparseMdpRewardBoundedPcaaWeightVectorChecker.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/logic/Formulas.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/solver/LinearEquationSolver.h"


#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/IllegalArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/UncheckedRequirementException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template <class SparseMdpModelType>
            SparseMdpRewardBoundedPcaaWeightVectorChecker<SparseMdpModelType>::SparseMdpRewardBoundedPcaaWeightVectorChecker(SparseMultiObjectivePreprocessorResult<SparseMdpModelType> const& preprocessorResult) : PcaaWeightVectorChecker<SparseMdpModelType>(preprocessorResult.objectives), swAll(true), rewardUnfolding(*preprocessorResult.preprocessedModel, preprocessorResult.objectives) {
                
                STORM_LOG_THROW(preprocessorResult.rewardFinitenessType == SparseMultiObjectivePreprocessorResult<SparseMdpModelType>::RewardFinitenessType::AllFinite, storm::exceptions::NotSupportedException, "There is a scheduler that yields infinite reward for one  objective. This is not supported.");
                STORM_LOG_THROW(preprocessorResult.preprocessedModel->getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::NotSupportedException, "The model has multiple initial states.");
                
                numSchedChanges = 0;
                numCheckedEpochs = 0;
            }
            
            template <class SparseMdpModelType>
            void SparseMdpRewardBoundedPcaaWeightVectorChecker<SparseMdpModelType>::check(std::vector<ValueType> const& weightVector) {
                auto initEpoch = rewardUnfolding.getStartEpoch();
                auto epochOrder = rewardUnfolding.getEpochComputationOrder(initEpoch);
                
                EpochCheckingData cachedData;
                for (auto const& epoch : epochOrder) {
                    computeEpochSolution(epoch, weightVector, cachedData);
                }
                
                auto solution = rewardUnfolding.getInitialStateResult(initEpoch);
                // Todo: we currently assume precise results...
                auto solutionIt = solution.begin();
                ++solutionIt;
                underApproxResult = std::vector<ValueType>(solutionIt, solution.end());
                overApproxResult = underApproxResult;
                
            }
            
            template <class SparseMdpModelType>
            void SparseMdpRewardBoundedPcaaWeightVectorChecker<SparseMdpModelType>::computeEpochSolution(typename MultiDimensionalRewardUnfolding<ValueType, false>::Epoch const& epoch, std::vector<ValueType> const& weightVector, EpochCheckingData& cachedData) {
                auto& epochModel = rewardUnfolding.setCurrentEpoch(epoch);
                updateCachedData(epochModel, cachedData, weightVector);
                ++numCheckedEpochs;
                swEqBuilding.start();
                
                std::vector<typename MultiDimensionalRewardUnfolding<ValueType, false>::SolutionType> result;
                result.reserve(epochModel.epochInStates.getNumberOfSetBits());
                uint64_t solutionSize = this->objectives.size() + 1;
                
                // Formulate a min-max equation system max(A*x+b)=x for the weighted sum of the objectives
                swAux1.start();
                assert(cachedData.bMinMax.capacity() >= epochModel.epochMatrix.getRowCount());
                assert(cachedData.xMinMax.size() == epochModel.epochMatrix.getRowGroupCount());
                cachedData.bMinMax.assign(epochModel.epochMatrix.getRowCount(), storm::utility::zero<ValueType>());
                for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    ValueType weight = storm::solver::minimize(this->objectives[objIndex].formula->getOptimalityType()) ? -weightVector[objIndex] : weightVector[objIndex];
                    if (!storm::utility::isZero(weight)) {
                        std::vector<ValueType> const& objectiveReward = epochModel.objectiveRewards[objIndex];
                        for (auto const& choice : epochModel.objectiveRewardFilter[objIndex]) {
                            cachedData.bMinMax[choice] += weight * objectiveReward[choice];
                        }
                    }
                }
                auto stepSolutionIt = epochModel.stepSolutions.begin();
                for (auto const& choice : epochModel.stepChoices) {
                    cachedData.bMinMax[choice] += stepSolutionIt->front();
                    ++stepSolutionIt;
                }
                swAux1.stop();
                
                // Invoke the min max solver
                swEqBuilding.stop();
                swMinMaxSolving.start();
                cachedData.minMaxSolver->solveEquations(cachedData.xMinMax, cachedData.bMinMax);
                swMinMaxSolving.stop();
                swEqBuilding.start();
                for (auto const& state : epochModel.epochInStates) {
                    result.emplace_back();
                    result.back().reserve(solutionSize);
                    result.back().push_back(cachedData.xMinMax[state]);
                }
                
                // Check whether the linear equation solver needs to be updated
                auto const& choices = cachedData.minMaxSolver->getSchedulerChoices();
                if (cachedData.schedulerChoices != choices) {
                    std::vector<uint64_t> choicesTmp = choices;
                    cachedData.minMaxSolver->setInitialScheduler(std::move(choicesTmp));
                    swAux2.start();
                    ++numSchedChanges;
                    cachedData.schedulerChoices = choices;
                    storm::storage::SparseMatrix<ValueType> subMatrix = epochModel.epochMatrix.selectRowsFromRowGroups(choices, true);
                    subMatrix.convertToEquationSystem();
                    storm::solver::GeneralLinearEquationSolverFactory<ValueType> linEqSolverFactory;
                    cachedData.linEqSolver = linEqSolverFactory.create(std::move(subMatrix));
                    cachedData.linEqSolver->setCachingEnabled(true);
                    swAux2.stop();
                }
                
                // Formulate for each objective the linear equation system induced by the performed choices
                swAux3.start();
                assert(cachedData.bLinEq.size() == choices.size());
                for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    auto const& obj = this->objectives[objIndex];
                    std::vector<ValueType> const& objectiveReward = epochModel.objectiveRewards[objIndex];
                    auto rowGroupIndexIt = epochModel.epochMatrix.getRowGroupIndices().begin();
                    auto choiceIt = choices.begin();
                    auto stepChoiceIt = epochModel.stepChoices.begin();
                    auto stepSolutionIt = epochModel.stepSolutions.begin();
                    std::vector<ValueType>& x = cachedData.xLinEq[objIndex];
                    auto xIt = x.begin();
                    for (auto& b_i : cachedData.bLinEq) {
                        uint64_t i = *rowGroupIndexIt + *choiceIt;
                        if (epochModel.objectiveRewardFilter[objIndex].get(i)) {
                            b_i = objectiveReward[i];
                        } else {
                            b_i = storm::utility::zero<ValueType>();
                        }
                        while (*stepChoiceIt < i) {
                            ++stepChoiceIt;
                            ++stepSolutionIt;
                        }
                        if (i == *stepChoiceIt) {
                            b_i += (*stepSolutionIt)[objIndex + 1];
                            ++stepChoiceIt;
                            ++stepSolutionIt;
                        }
                        // We can already set x_i correctly if row i is empty.
                        // Appearingly, some linear equation solvers struggle to converge otherwise.
                        if (epochModel.epochMatrix.getRow(i).getNumberOfEntries() == 0) {
                            *xIt = b_i;
                        }
                        ++xIt;
                        ++rowGroupIndexIt;
                        ++choiceIt;
                    }
                    assert(x.size() == choices.size());
                    auto req = cachedData.linEqSolver->getRequirements();
                    if (obj.lowerResultBound) {
                        req.clearLowerBounds();
                        cachedData.linEqSolver->setLowerBound(*obj.lowerResultBound);
                    }
                    if (obj.upperResultBound) {
                        cachedData.linEqSolver->setUpperBound(*obj.upperResultBound);
                        req.clearUpperBounds();
                    }
                    STORM_LOG_THROW(req.empty(), storm::exceptions::UncheckedRequirementException, "At least one requirement of the LinearEquationSolver was not met.");
                    swEqBuilding.stop();
                    swLinEqSolving.start();
                    cachedData.linEqSolver->solveEquations(x, cachedData.bLinEq);
                    swLinEqSolving.stop();
                    swEqBuilding.start();
                    auto resultIt = result.begin();
                    for (auto const& state : epochModel.epochInStates) {
                        resultIt->push_back(x[state]);
                        ++resultIt;
                    }
                }
                swEqBuilding.stop();
                swAux3.stop();
                rewardUnfolding.setSolutionForCurrentEpoch(std::move(result));
            }

            template <class SparseMdpModelType>
            void SparseMdpRewardBoundedPcaaWeightVectorChecker<SparseMdpModelType>::updateCachedData(typename MultiDimensionalRewardUnfolding<ValueType, false>::EpochModel const& epochModel, EpochCheckingData& cachedData, std::vector<ValueType> const& weightVector) {
                if (epochModel.epochMatrixChanged) {
                    swDataUpdate.start();
                
                    // Update the cached MinMaxSolver data
                    cachedData.bMinMax.resize(epochModel.epochMatrix.getRowCount());
                    cachedData.xMinMax.assign(epochModel.epochMatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
                    storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> minMaxSolverFactory;
                    cachedData.minMaxSolver = minMaxSolverFactory.create(epochModel.epochMatrix);
                    cachedData.minMaxSolver->setTrackScheduler(true);
                    cachedData.minMaxSolver->setCachingEnabled(true);
                    auto req = cachedData.minMaxSolver->getRequirements(storm::solver::EquationSystemType::StochasticShortestPath);
                    req.clearNoEndComponents();
                    boost::optional<ValueType> lowerBound = this->computeWeightedResultBound(true, weightVector, storm::storage::BitVector(weightVector.size(), true));
                    if (lowerBound) {
                        cachedData.minMaxSolver->setLowerBound(lowerBound.get());
                        req.clearLowerBounds();
                    }
                    boost::optional<ValueType> upperBound = this->computeWeightedResultBound(false, weightVector, storm::storage::BitVector(weightVector.size(), true));
                    if (upperBound) {
                        cachedData.minMaxSolver->setUpperBound(upperBound.get());
                        req.clearUpperBounds();
                    }
                    STORM_LOG_THROW(req.empty(), storm::exceptions::UncheckedRequirementException, "At least one requirement of the MinMaxSolver was not met.");
                    cachedData.minMaxSolver->setRequirementsChecked(true);
                    cachedData.minMaxSolver->setOptimizationDirection(storm::solver::OptimizationDirection::Maximize);
                
                    // Clear the scheduler choices so that an update of the linEqSolver is enforced
                    cachedData.schedulerChoices.clear();
                    cachedData.schedulerChoices.reserve(epochModel.epochMatrix.getRowGroupCount());
                    
                    // Update data for linear equation solving
                    cachedData.bLinEq.resize(epochModel.epochMatrix.getRowGroupCount());
                    cachedData.xLinEq.resize(this->objectives.size());
                    for (auto& x_o : cachedData.xLinEq) {
                        x_o.assign(epochModel.epochMatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
                    }
                    swDataUpdate.stop();
                }
            }
            
            template <class SparseMdpModelType>
            std::vector<typename SparseMdpRewardBoundedPcaaWeightVectorChecker<SparseMdpModelType>::ValueType> SparseMdpRewardBoundedPcaaWeightVectorChecker<SparseMdpModelType>::getUnderApproximationOfInitialStateResults() const {
                STORM_LOG_THROW(underApproxResult, storm::exceptions::InvalidOperationException, "Tried to retrieve results but check(..) has not been called before.");
                return underApproxResult.get();
            }
            
            template <class SparseMdpModelType>
            std::vector<typename SparseMdpRewardBoundedPcaaWeightVectorChecker<SparseMdpModelType>::ValueType> SparseMdpRewardBoundedPcaaWeightVectorChecker<SparseMdpModelType>::getOverApproximationOfInitialStateResults() const {
                STORM_LOG_THROW(overApproxResult, storm::exceptions::InvalidOperationException, "Tried to retrieve results but check(..) has not been called before.");
                return overApproxResult.get();
            }
            
            template class SparseMdpRewardBoundedPcaaWeightVectorChecker<storm::models::sparse::Mdp<double>>;
#ifdef STORM_HAVE_CARL
            template class SparseMdpRewardBoundedPcaaWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
#endif
        
        }
    }
}
