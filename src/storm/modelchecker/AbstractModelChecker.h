#ifndef STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_
#define STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_

#include <boost/optional.hpp>

#include "src/storm/modelchecker/CheckTask.h"
#include "src/storm/logic/Formulas.h"
#include "src/storm/solver/OptimizationDirection.h"

namespace storm {
    namespace modelchecker {
        class CheckResult;
        
        enum class RewardType { Expectation, Variance };

        template<typename ModelType>
        class AbstractModelChecker {
        public:
            virtual ~AbstractModelChecker() {
                // Intentionally left empty.
            }

            typedef typename ModelType::ValueType ValueType;
            
            /*!
             * Determines whether the model checker can handle the given verification task. If this method returns
             * false, the task must not be checked using this model checker.
             *
             * @param checkTask The task for which to check whether the model checker can handle it.
             * @return True iff the model checker can check the given task.
             */
            virtual bool canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const = 0;
            
            /*!
             * Checks the provided formula.
             *
             * @param checkTask The verification task to pursue.
             * @return The verification result.
             */
            virtual std::unique_ptr<CheckResult> check(CheckTask<storm::logic::Formula, ValueType> const& checkTask);
                        
            // The methods to compute probabilities for path formulas.
            virtual std::unique_ptr<CheckResult> computeProbabilities(CheckTask<storm::logic::Formula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeConditionalProbabilities(CheckTask<storm::logic::ConditionalFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeGloballyProbabilities(CheckTask<storm::logic::GloballyFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(CheckTask<storm::logic::NextFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask);
            
            // The methods to compute the rewards for path formulas.
            virtual std::unique_ptr<CheckResult> computeRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::Formula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeConditionalRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::ConditionalFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeLongRunAverageRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask);
            
            // The methods to compute the long-run average probabilities and timing measures.
            virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeTimes(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::Formula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeReachabilityTimes(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask);
            
            // The methods to check state formulas.
            virtual std::unique_ptr<CheckResult> checkStateFormula(CheckTask<storm::logic::StateFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> checkAtomicExpressionFormula(CheckTask<storm::logic::AtomicExpressionFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> checkAtomicLabelFormula(CheckTask<storm::logic::AtomicLabelFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> checkBinaryBooleanStateFormula(CheckTask<storm::logic::BinaryBooleanStateFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> checkBooleanLiteralFormula(CheckTask<storm::logic::BooleanLiteralFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> checkProbabilityOperatorFormula(CheckTask<storm::logic::ProbabilityOperatorFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> checkRewardOperatorFormula(CheckTask<storm::logic::RewardOperatorFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> checkTimeOperatorFormula(CheckTask<storm::logic::TimeOperatorFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> checkLongRunAverageOperatorFormula(CheckTask<storm::logic::LongRunAverageOperatorFormula, ValueType> const& checkTask);
            virtual std::unique_ptr<CheckResult> checkUnaryBooleanStateFormula(CheckTask<storm::logic::UnaryBooleanStateFormula, ValueType> const& checkTask);
  
            // The methods to check multi-objective formulas.
            virtual std::unique_ptr<CheckResult> checkMultiObjectiveFormula(CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask);
                  
        };
    }
}

#endif /* STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_ */
