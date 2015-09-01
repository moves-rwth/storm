#ifndef STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_
#define STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_

#include <boost/optional.hpp>

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
             * @return The verification result.
             */
            virtual std::unique_ptr<CheckResult> check(storm::logic::Formula const& formula);
                        
            // The methods to compute probabilities for path formulas.
            virtual std::unique_ptr<CheckResult> computeProbabilities(storm::logic::PathFormula const& pathFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>());
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>());
            virtual std::unique_ptr<CheckResult> computeConditionalProbabilities(storm::logic::ConditionalPathFormula const& pathFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>());
            virtual std::unique_ptr<CheckResult> computeEventuallyProbabilities(storm::logic::EventuallyFormula const& pathFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>());
            virtual std::unique_ptr<CheckResult> computeGloballyProbabilities(storm::logic::GloballyFormula const& pathFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>());
            virtual std::unique_ptr<CheckResult> computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>());
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>());
            
            // The methods to compute the rewards for path formulas.
            virtual std::unique_ptr<CheckResult> computeRewards(storm::logic::RewardPathFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>());
            virtual std::unique_ptr<CheckResult> computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>());
            virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>());
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>());
            
            // The methods to compute the long-run average and expected time.
            virtual std::unique_ptr<CheckResult> computeLongRunAverage(storm::logic::StateFormula const& stateFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>());
            virtual std::unique_ptr<CheckResult> computeExpectedTimes(storm::logic::EventuallyFormula const& eventuallyFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>());
            
            // The methods to check state formulas.
            virtual std::unique_ptr<CheckResult> checkStateFormula(storm::logic::StateFormula const& stateFormula);
            virtual std::unique_ptr<CheckResult> checkAtomicExpressionFormula(storm::logic::AtomicExpressionFormula const& stateFormula);
            virtual std::unique_ptr<CheckResult> checkAtomicLabelFormula(storm::logic::AtomicLabelFormula const& stateFormula);
            virtual std::unique_ptr<CheckResult> checkBinaryBooleanStateFormula(storm::logic::BinaryBooleanStateFormula const& stateFormula);
            virtual std::unique_ptr<CheckResult> checkBooleanLiteralFormula(storm::logic::BooleanLiteralFormula const& stateFormula);
            virtual std::unique_ptr<CheckResult> checkProbabilityOperatorFormula(storm::logic::ProbabilityOperatorFormula const& stateFormula);
            virtual std::unique_ptr<CheckResult> checkRewardOperatorFormula(storm::logic::RewardOperatorFormula const& stateFormula);
            virtual std::unique_ptr<CheckResult> checkExpectedTimeOperatorFormula(storm::logic::ExpectedTimeOperatorFormula const& stateFormula);
            virtual std::unique_ptr<CheckResult> checkLongRunAverageOperatorFormula(storm::logic::LongRunAverageOperatorFormula const& stateFormula);
            virtual std::unique_ptr<CheckResult> checkUnaryBooleanStateFormula(storm::logic::UnaryBooleanStateFormula const& stateFormula);
        };
    }
}

#endif /* STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_ */