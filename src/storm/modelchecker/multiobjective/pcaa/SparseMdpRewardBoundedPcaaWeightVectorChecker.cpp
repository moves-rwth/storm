#include "storm/modelchecker/multiobjective/pcaa/SparseMdpRewardBoundedPcaaWeightVectorChecker.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/logic/Formulas.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/solver/LinearEquationSolver.h"

#include "storm/settings/SettingsManager.h"
#include "storm/utility/export.h"
#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/modules/GeneralSettings.h"

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
                numChecks = 0;
            }
            
            template <class SparseMdpModelType>
            void SparseMdpRewardBoundedPcaaWeightVectorChecker<SparseMdpModelType>::check(std::vector<ValueType> const& weightVector) {
                STORM_LOG_DEBUG("Analyzing weight vector " << storm::utility::vector::toString(weightVector));

                ++numChecks;
                
                // In case we want to export the cdf, we will collect the corresponding data
                std::vector<std::vector<ValueType>> cdfData;
                
                auto initEpoch = rewardUnfolding.getStartEpoch();
                auto epochOrder = rewardUnfolding.getEpochComputationOrder(initEpoch);
                EpochCheckingData cachedData;
                ValueType precision = storm::utility::convertNumber<ValueType>(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
                uint64_t epochCount = 0;
                for (uint64_t dim = 0; dim < rewardUnfolding.getEpochManager().getDimensionCount(); ++dim) {
                    epochCount += rewardUnfolding.getEpochManager().getDimensionOfEpoch(initEpoch, dim) + 1;
                }
                if (storm::settings::getModule<storm::settings::modules::GeneralSettings>().isSoundSet()) {
                    precision = precision / storm::utility::convertNumber<ValueType>(epochCount);
                }
                if (numChecks == 1) {
                    STORM_PRINT_AND_LOG("Objective/Dimension/Epoch count is: " << 1 << "/" << rewardUnfolding.getEpochManager().getDimensionCount() << "/" <<  epochCount << "."  << std::endl);
                }


                for (auto const& epoch : epochOrder) {
                    computeEpochSolution(epoch, weightVector, cachedData, precision);
                    if (storm::settings::getModule<storm::settings::modules::IOSettings>().isExportCdfSet() && !rewardUnfolding.getEpochManager().hasBottomDimension(epoch)) {
                        std::vector<ValueType> cdfEntry;
                        for (uint64_t i = 0; i < rewardUnfolding.getEpochManager().getDimensionCount(); ++i) {
                            uint64_t offset = rewardUnfolding.getDimension(i).isUpperBounded ? 0 : 1;
                            cdfEntry.push_back(storm::utility::convertNumber<ValueType>(rewardUnfolding.getEpochManager().getDimensionOfEpoch(epoch, i) + offset) * rewardUnfolding.getDimension(i).scalingFactor);
                        }
                        auto const& solution = rewardUnfolding.getInitialStateResult(epoch);
                        auto solutionIt = solution.begin();
                        ++solutionIt;
                        cdfEntry.insert(cdfEntry.end(), solutionIt, solution.end());
                        cdfData.push_back(std::move(cdfEntry));
                    }
                }
                
                if (storm::settings::getModule<storm::settings::modules::IOSettings>().isExportCdfSet()) {
                    std::vector<std::string> headers;
                    for (uint64_t i = 0; i < rewardUnfolding.getEpochManager().getDimensionCount(); ++i) {
                        headers.push_back("obj" + std::to_string(rewardUnfolding.getDimension(i).objectiveIndex) + ":" + rewardUnfolding.getDimension(i).formula->toString());
                    }
                    for (uint64_t i = 0; i < this->objectives.size(); ++i) {
                        headers.push_back("obj" + std::to_string(i));
                    }
                    storm::utility::exportDataToCSVFile<ValueType, ValueType, std::string>("cdf" + std::to_string(numChecks) + ".csv", cdfData, weightVector, headers);
                }
                auto solution = rewardUnfolding.getInitialStateResult(initEpoch);
                // Todo: we currently assume precise results...
                auto solutionIt = solution.begin();
                ++solutionIt;
                underApproxResult = std::vector<ValueType>(solutionIt, solution.end());
                overApproxResult = underApproxResult;
                
            }
            
            template <class SparseMdpModelType>
            void SparseMdpRewardBoundedPcaaWeightVectorChecker<SparseMdpModelType>::computeEpochSolution(typename MultiDimensionalRewardUnfolding<ValueType, false>::Epoch const& epoch, std::vector<ValueType> const& weightVector, EpochCheckingData& cachedData, ValueType const& precision) {
                
                ++numCheckedEpochs;
                swEpochModelBuild.start();
                auto& epochModel = rewardUnfolding.setCurrentEpoch(epoch);
                swEpochModelBuild.stop();
                swEpochModelAnalysis.start();
                std::vector<typename MultiDimensionalRewardUnfolding<ValueType, false>::SolutionType> result;
                result.reserve(epochModel.epochInStates.getNumberOfSetBits());
                uint64_t solutionSize = this->objectives.size() + 1;

                // If the epoch matrix is empty we do not need to solve linear equation systems
                if (epochModel.epochMatrix.getEntryCount() == 0) {
                    std::vector<ValueType> weights = weightVector;
                    for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                        if (storm::solver::minimize(this->objectives[objIndex].formula->getOptimalityType())) {
                            weights[objIndex] *= -storm::utility::one<ValueType>();
                        }
                    }

                    auto stepSolutionIt = epochModel.stepSolutions.begin();
                    auto stepChoiceIt = epochModel.stepChoices.begin();
                    for (auto const& state : epochModel.epochInStates) {
                        // Obtain the best choice for this state according to the weighted combination of objectives
                        ValueType bestValue;
                        uint64_t bestChoice = std::numeric_limits<uint64_t>::max();
                        auto bestChoiceStepSolutionIt = epochModel.stepSolutions.end();
                        uint64_t lastChoice = epochModel.epochMatrix.getRowGroupIndices()[state + 1];
                        bool firstChoice = true;
                        for (uint64_t choice = epochModel.epochMatrix.getRowGroupIndices()[state]; choice < lastChoice; ++choice) {
                            ValueType choiceValue = storm::utility::zero<ValueType>();
                            // Obtain the (weighted) objective rewards
                            for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                                if (epochModel.objectiveRewardFilter[objIndex].get(choice)) {
                                    choiceValue += weights[objIndex] * epochModel.objectiveRewards[objIndex][choice];
                                }
                            }
                            
                            // Obtain the step solution if this is a step choice
                            while (*stepChoiceIt < choice) {
                                ++stepChoiceIt;
                                ++stepSolutionIt;
                            }
                            if (*stepChoiceIt == choice) {
                                choiceValue += stepSolutionIt->front();
                                // Check if this choice is better
                                if (firstChoice || choiceValue > bestValue) {
                                    bestValue = std::move(choiceValue);
                                    bestChoice = choice;
                                    bestChoiceStepSolutionIt = stepSolutionIt;
                                }
                            } else if (firstChoice || choiceValue > bestValue) {
                                bestValue = std::move(choiceValue);
                                bestChoice = choice;
                                bestChoiceStepSolutionIt = epochModel.stepSolutions.end();
                            }
                            firstChoice = false;
                        }
                        
                        // Insert the solution w.r.t. this choice
                        result.emplace_back();
                        result.back().reserve(solutionSize);
                        result.back().push_back(std::move(bestValue));
                        
                        if (bestChoiceStepSolutionIt != epochModel.stepSolutions.end()) {
                            for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                                if (epochModel.objectiveRewardFilter[objIndex].get(bestChoice)) {
                                    result.back().push_back((epochModel.objectiveRewards[objIndex][bestChoice] + (*bestChoiceStepSolutionIt)[objIndex + 1]));
                                } else {
                                    result.back().push_back((*bestChoiceStepSolutionIt)[objIndex + 1]);
                                }
                            }
                        } else {
                            for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                                if (epochModel.objectiveRewardFilter[objIndex].get(bestChoice)) {
                                    result.back().push_back((epochModel.objectiveRewards[objIndex][bestChoice]));
                                } else {
                                    result.back().push_back(storm::utility::zero<ValueType>());
                                }
                            }
                        }
                    }
                } else {
                    
                    
                    updateCachedData(epochModel, cachedData, weightVector, precision);
                    
                    
                    // Formulate a min-max equation system max(A*x+b)=x for the weighted sum of the objectives
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
                    
                    // Invoke the min max solver
                    cachedData.minMaxSolver->solveEquations(cachedData.xMinMax, cachedData.bMinMax);
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
                        ++numSchedChanges;
                        cachedData.schedulerChoices = choices;
                        storm::solver::GeneralLinearEquationSolverFactory<ValueType> linEqSolverFactory;
                        bool needEquationSystem = linEqSolverFactory.getEquationProblemFormat() == storm::solver::LinearEquationSolverProblemFormat::EquationSystem;
                        storm::storage::SparseMatrix<ValueType> subMatrix = epochModel.epochMatrix.selectRowsFromRowGroups(choices, needEquationSystem);
                        if (needEquationSystem) {
                            subMatrix.convertToEquationSystem();
                        }
                        cachedData.linEqSolver = linEqSolverFactory.create(std::move(subMatrix));
                        cachedData.linEqSolver->setPrecision(precision);
                        cachedData.linEqSolver->setCachingEnabled(true);
                    }
                    
                    // Formulate for each objective the linear equation system induced by the performed choices
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
                        cachedData.linEqSolver->clearBounds();
                        if (obj.lowerResultBound) {
                            req.clearLowerBounds();
                            cachedData.linEqSolver->setLowerBound(*obj.lowerResultBound);
                        }
                        if (obj.upperResultBound) {
                            cachedData.linEqSolver->setUpperBound(*obj.upperResultBound);
                            req.clearUpperBounds();
                        }
                        if (!req.empty()) {
                            // Todo: currently we wrongly require lower bounds for plain value iteration even if the fixpoint is unique
                            STORM_LOG_DEBUG("At least one requirement of the LinearEquationSolver was not met.");
                        }
                        cachedData.linEqSolver->solveEquations(x, cachedData.bLinEq);
                        auto resultIt = result.begin();
                        for (auto const& state : epochModel.epochInStates) {
                            resultIt->push_back(x[state]);
                            ++resultIt;
                        }
                    }
                }
                rewardUnfolding.setSolutionForCurrentEpoch(std::move(result));
                swEpochModelAnalysis.stop();
            }

            template <class SparseMdpModelType>
            void SparseMdpRewardBoundedPcaaWeightVectorChecker<SparseMdpModelType>::updateCachedData(typename MultiDimensionalRewardUnfolding<ValueType, false>::EpochModel const& epochModel, EpochCheckingData& cachedData, std::vector<ValueType> const& weightVector, ValueType const& precision) {
                if (epochModel.epochMatrixChanged) {
                
                    // Update the cached MinMaxSolver data
                    cachedData.bMinMax.resize(epochModel.epochMatrix.getRowCount());
                    cachedData.xMinMax.assign(epochModel.epochMatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
                    storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> minMaxSolverFactory;
                    cachedData.minMaxSolver = minMaxSolverFactory.create(epochModel.epochMatrix);
                    cachedData.minMaxSolver->setPrecision(precision);
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
                    if (!req.empty()) {
                        // Todo: currently we wrongly require lower bounds for plain value iteration even if the fixpoint is unique
                        STORM_LOG_DEBUG("At least one requirement of the LinearEquationSolver was not met.");
                    }
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
