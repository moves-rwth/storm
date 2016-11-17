#ifndef STORM_MODELCHECKER_SPARSECTMCCSLMODELCHECKER_H_
#define STORM_MODELCHECKER_SPARSECTMCCSLMODELCHECKER_H_

#include "src/storm/modelchecker/propositional/SparsePropositionalModelChecker.h"

#include "src/storm/models/sparse/Ctmc.h"

#include "src/storm/solver/LinearEquationSolver.h"

#include "src/storm/utility/NumberTraits.h"

namespace storm {
    namespace modelchecker {
        
        template<class SparseCtmcModelType>
        class SparseCtmcCslModelChecker : public SparsePropositionalModelChecker<SparseCtmcModelType> {
        public:
            typedef typename SparseCtmcModelType::ValueType ValueType;
            typedef typename SparseCtmcModelType::RewardModelType RewardModelType;
            
            explicit SparseCtmcCslModelChecker(SparseCtmcModelType const& model);
            explicit SparseCtmcCslModelChecker(SparseCtmcModelType const& model, std::unique_ptr<storm::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityTimes(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) override;

        private:
            template<typename CValueType = ValueType, typename std::enable_if<storm::NumberTraits<CValueType>::SupportsExponential, int>::type = 0>
            bool canHandleImplementation(CheckTask<storm::logic::Formula, CValueType> const& checkTask) const;

            template<typename CValueType = ValueType, typename std::enable_if<!storm::NumberTraits<CValueType>::SupportsExponential, int>::type = 0>
            bool canHandleImplementation(CheckTask<storm::logic::Formula, CValueType> const& checkTask) const;

            // An object that is used for solving linear equations and performing matrix-vector multiplication.
            std::unique_ptr<storm::solver::LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_SPARSECTMCCSLMODELCHECKER_H_ */
