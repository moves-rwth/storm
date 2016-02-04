#ifndef STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_
#define STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_

#include <boost/optional.hpp>

#include "src/modelchecker/CheckTask.h"
#include "src/logic/Formulas.h"
#include "src/solver/OptimizationDirection.h"

namespace storm {
    namespace modelchecker {
        class CheckResult;
        
        class AbstractModelChecker {
        public:
            virtual ~AbstractModelChecker() {
                // Intentionally left empty.
            }
            
            /*!
             * Determines whether the model checker can handle the formula. If this method returns false, the formula
             * must not be checked using the model checker.
             *
             * @param formula The formula for which to check whether the model checker can handle it.
             * @return True iff the model checker can check the given formula.
             */
            virtual bool canHandle(storm::logic::Formula const& formula) const = 0;
            
            /*!
             * Checks the provided formula.
             *
             * @param formula The formula to check.
             * @param checkTask If provided, this object is used to customize the checking process.
             * @return The verification result.
             */
            virtual std::unique_ptr<CheckResult> check(CheckTask<storm::logic::Formula> const& checkTask);
                        
            // The methods to compute probabilities for path formulas.
            virtual std::unique_ptr<CheckResult> computeProbabilities(CheckTask<storm::logic::PathFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeConditionalProbabilities(CheckTask<storm::logic::ConditionalPathFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeEventuallyProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeGloballyProbabilities(CheckTask<storm::logic::GloballyFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(CheckTask<storm::logic::NextFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask);
            
            // The methods to compute the rewards for path formulas.
            virtual std::unique_ptr<CheckResult> computeRewards(CheckTask<storm::logic::RewardPathFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(CheckTask<storm::logic::CumulativeRewardFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(CheckTask<storm::logic::InstantaneousRewardFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(CheckTask<storm::logic::ReachabilityRewardFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeLongRunAverageRewards(CheckTask<storm::logic::LongRunAverageRewardFormula> const& checkTask);
            
            // The methods to compute the long-run average and expected time.
            virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> computeExpectedTimes(CheckTask<storm::logic::EventuallyFormula> const& checkTask);
            
            // The methods to check state formulas.
            virtual std::unique_ptr<CheckResult> checkStateFormula(CheckTask<storm::logic::StateFormula> const& stateFormula);
            virtual std::unique_ptr<CheckResult> checkAtomicExpressionFormula(CheckTask<storm::logic::AtomicExpressionFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> checkAtomicLabelFormula(CheckTask<storm::logic::AtomicLabelFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> checkBinaryBooleanStateFormula(CheckTask<storm::logic::BinaryBooleanStateFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> checkBooleanLiteralFormula(CheckTask<storm::logic::BooleanLiteralFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> checkProbabilityOperatorFormula(CheckTask<storm::logic::ProbabilityOperatorFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> checkRewardOperatorFormula(CheckTask<storm::logic::RewardOperatorFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> checkExpectedTimeOperatorFormula(CheckTask<storm::logic::ExpectedTimeOperatorFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> checkLongRunAverageOperatorFormula(CheckTask<storm::logic::LongRunAverageOperatorFormula> const& checkTask);
            virtual std::unique_ptr<CheckResult> checkUnaryBooleanStateFormula(CheckTask<storm::logic::UnaryBooleanStateFormula> const& checkTask);
        };
    }
}

#endif /* STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_ */