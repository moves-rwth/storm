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


namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template <class SparseMdpModelType>
            SparseMdpRewardBoundedPcaaWeightVectorChecker<SparseMdpModelType>::SparseMdpRewardBoundedPcaaWeightVectorChecker(SparseMultiObjectivePreprocessorResult<SparseMdpModelType> const& preprocessorResult) : PcaaWeightVectorChecker<SparseMdpModelType>(preprocessorResult.objectives), rewardUnfolding(*preprocessorResult.preprocessedModel, this->objectives, storm::storage::BitVector(preprocessorResult.preprocessedModel->getNumberOfChoices(), true), preprocessorResult.reward0EStates) {
            
            }
            
            template <class SparseMdpModelType>
            void SparseMdpRewardBoundedPcaaWeightVectorChecker<SparseMdpModelType>::check(std::vector<ValueType> const& weightVector) {
                auto initEpoch = rewardUnfolding.getStartEpoch();
                auto epochOrder = rewardUnfolding.getEpochComputationOrder(initEpoch);
                for (auto const& epoch : epochOrder) {
                    computeEpochSolution(epoch, weightVector);
                }
                
                auto solution = rewardUnfolding.getInitialStateResult(initEpoch);
                // Todo: we currently assume precise results...
                underApproxResult = solution.objectiveValues;
                overApproxResult = solution.objectiveValues;
                
            }
            
            template <class SparseMdpModelType>
            void SparseMdpRewardBoundedPcaaWeightVectorChecker<SparseMdpModelType>::computeEpochSolution(typename MultiDimensionalRewardUnfolding<ValueType>::Epoch const& epoch, std::vector<ValueType> const& weightVector) {
                auto const& epochModel = rewardUnfolding.setCurrentEpoch(epoch);
                std::vector<typename MultiDimensionalRewardUnfolding<ValueType>::SolutionType> result(epochModel.epochMatrix.getRowGroupCount());
                
                
                // Formulate a min-max equation system max(A*x+b)=x for the weighted sum of the objectives
                std::vector<ValueType> b(epochModel.epochMatrix.getRowCount(), storm::utility::zero<ValueType>());
                for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    ValueType weight = storm::solver::minimize(this->objectives[objIndex].formula->getOptimalityType()) ? -weightVector[objIndex] : weightVector[objIndex];
                    if (!storm::utility::isZero(weight)) {
                        std::vector<ValueType> const& objectiveReward = epochModel.objectiveRewards[objIndex];
                        for (auto const& choice : epochModel.objectiveRewardFilter[objIndex]) {
                            b[choice] += weight * objectiveReward[choice];
                        }
                    }
                }
                auto stepSolutionIt = epochModel.stepSolutions.begin();
                for (auto const& choice : epochModel.stepChoices) {
                    b[choice] += stepSolutionIt->weightedValue;
                    ++stepSolutionIt;
                }
                
                // Invoke the min max solver
                storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> minMaxSolverFactory;
                auto minMaxSolver = minMaxSolverFactory.create(epochModel.epochMatrix);
                minMaxSolver->setOptimizationDirection(storm::solver::OptimizationDirection::Maximize);
                minMaxSolver->setTrackScheduler(true);
                //minMaxSolver->setCachingEnabled(true);
                std::vector<ValueType> x(result.size(), storm::utility::zero<ValueType>());
                minMaxSolver->solveEquations(x, b);
                for (uint64_t state = 0; state < x.size(); ++state) {
                    result[state].weightedValue = x[state];
                }
                
                // Formulate for each objective the linear equation system induced by the performed choices
                auto const& choices = minMaxSolver->getSchedulerChoices();
                storm::storage::SparseMatrix<ValueType> subMatrix = epochModel.epochMatrix.selectRowsFromRowGroups(choices, true);
                subMatrix.convertToEquationSystem();
                storm::solver::GeneralLinearEquationSolverFactory<ValueType> linEqSolverFactory;
                auto linEqSolver = linEqSolverFactory.create(std::move(subMatrix));
                b.resize(choices.size());
                // TODO: start with a better initial guess
                x.resize(choices.size());
                for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    std::vector<ValueType> const& objectiveReward = epochModel.objectiveRewards[objIndex];
                    for (uint64_t state = 0; state < choices.size(); ++state) {
                        uint64_t choice = epochModel.epochMatrix.getRowGroupIndices()[state] + choices[state];
                        if (epochModel.objectiveRewardFilter[objIndex].get(choice)) {
                            b[state] = objectiveReward[choice];
                        } else {
                            b[state] = storm::utility::zero<ValueType>();
                        }
                        if (epochModel.stepChoices.get(choice)) {
                            b[state] += epochModel.stepSolutions[epochModel.stepChoices.getNumberOfSetBitsBeforeIndex(choice)].objectiveValues[objIndex];
                        }
                    }
                    linEqSolver->solveEquations(x, b);
                    for (uint64_t state = 0; state < choices.size(); ++state) {
                        result[state].objectiveValues.push_back(x[state]);
                    }
                }
                
                rewardUnfolding.setSolutionForCurrentEpoch(result);
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
