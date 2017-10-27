#pragma once

#include <vector>

#include "storm/modelchecker/multiobjective/pcaa/PcaaWeightVectorChecker.h"
#include "storm/modelchecker/prctl/helper/rewardbounded/MultiDimensionalRewardUnfolding.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/solver/LinearEquationSolver.h"

#include "storm/utility/Stopwatch.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            /*!
             * Helper Class that takes preprocessed Pcaa data and a weight vector and ...
             * - computes the maximal expected reward w.r.t. the weighted sum of the rewards of the individual objectives
             * - extracts the scheduler that induces this maximum
             * - computes for each objective the value induced by this scheduler
             */
            template <class SparseMdpModelType>
            class SparseMdpRewardBoundedPcaaWeightVectorChecker : public PcaaWeightVectorChecker<SparseMdpModelType> {
            public:
                typedef typename SparseMdpModelType::ValueType ValueType;
                typedef typename SparseMdpModelType::RewardModelType RewardModelType;
            
                SparseMdpRewardBoundedPcaaWeightVectorChecker(SparseMultiObjectivePreprocessorResult<SparseMdpModelType> const& preprocessorResult);

                virtual ~SparseMdpRewardBoundedPcaaWeightVectorChecker() {
                    swAll.stop();
                    std::cout << "WVC statistics: " << std::endl;
                    std::cout << "                overall Time: " <<  swAll << "." << std::endl;
                    std::cout << "---------------------------------------------" << std::endl;
                    std::cout << "     #checked weight vectors: " << numChecks << "." << std::endl;
                    std::cout << "             #checked epochs: " << numCheckedEpochs << "." << std::endl;
                    std::cout << "#Sched reused from prev. ep.: " << (numCheckedEpochs - numSchedChanges) << "." << std::endl;
                    std::cout << "   Epoch Model building time: "  << swEpochModelBuild << "." << std::endl;
                    std::cout << "   Epoch Model checking time: "  << swEpochModelAnalysis << "." << std::endl;
                }
                

                /*!
                 * - computes the optimal expected reward w.r.t. the weighted sum of the rewards of the individual objectives
                 * - extracts the scheduler that induces this optimum
                 * - computes for each objective the value induced by this scheduler
                 */
                virtual void check(std::vector<ValueType> const& weightVector) override;
                
                /*!
                 * Retrieves the results of the individual objectives at the initial state of the given model.
                 * Note that check(..) has to be called before retrieving results. Otherwise, an exception is thrown.
                 * Also note that there is no guarantee that the under/over approximation is in fact correct
                 * as long as the underlying solution methods are unsound (e.g., standard value iteration).
                 */
                virtual std::vector<ValueType> getUnderApproximationOfInitialStateResults() const override;
                virtual std::vector<ValueType> getOverApproximationOfInitialStateResults() const override;
                
            private:
                
                struct EpochCheckingData {
                
                    std::vector<ValueType> bMinMax;
                    std::vector<ValueType> xMinMax;
                    std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> minMaxSolver;
                    
                    std::vector<uint64_t> schedulerChoices;
                    
                    std::vector<ValueType> bLinEq;
                    std::vector<std::vector<ValueType>> xLinEq;
                    std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> linEqSolver;
                    
                    std::vector<typename helper::rewardbounded::MultiDimensionalRewardUnfolding<ValueType, false>::SolutionType> solutions;
                };
                
                void computeEpochSolution(typename helper::rewardbounded::MultiDimensionalRewardUnfolding<ValueType, false>::Epoch const& epoch, std::vector<ValueType> const& weightVector, EpochCheckingData& cachedData, ValueType const& precision);
                
                void updateCachedData(typename helper::rewardbounded::MultiDimensionalRewardUnfolding<ValueType, false>::EpochModel const& epochModel, EpochCheckingData& cachedData, std::vector<ValueType> const& weightVector, ValueType const& precision);
                
                storm::utility::Stopwatch swAll, swEpochModelBuild, swEpochModelAnalysis;
                uint64_t numSchedChanges, numCheckedEpochs, numChecks;
                
                helper::rewardbounded::MultiDimensionalRewardUnfolding<ValueType, false> rewardUnfolding;
                
                boost::optional<std::vector<ValueType>> underApproxResult;
                boost::optional<std::vector<ValueType>> overApproxResult;

                
            };
            
        }
    }
}
