#include "src/modelchecker/AbstractModelChecker.h"

#include "src/modelchecker/results/QualitativeCheckResult.h"
#include "src/modelchecker/results/QuantitativeCheckResult.h"
#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/InvalidOperationException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InternalTypeErrorException.h"

namespace storm {
    namespace modelchecker {
        std::unique_ptr<CheckResult> AbstractModelChecker::check(storm::logic::Formula const& formula, boost::optional<CheckSettings<double>> const& checkSettings) {
            STORM_LOG_THROW(this->canHandle(formula), storm::exceptions::InvalidArgumentException, "The model checker is not able to check the formula '" << formula << "'.");
            CheckSettings<double> settingsToUse = checkSettings ? checkSettings.get() : CheckSettings<double>::fromToplevelFormula(formula);
            if (formula.isStateFormula()) {
                return this->checkStateFormula(formula.asStateFormula(), settingsToUse);
            } else if (formula.isPathFormula()) {
                return this->computeProbabilities(formula.asPathFormula(), settingsToUse);
            } else if (formula.isRewardPathFormula()) {
                return this->computeRewards(formula.asRewardPathFormula(), settingsToUse);
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << formula << "' is invalid.");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeProbabilities(storm::logic::PathFormula const& pathFormula, CheckSettings<double> const& checkSettings) {
            if (pathFormula.isBoundedUntilFormula()) {
                return this->computeBoundedUntilProbabilities(pathFormula.asBoundedUntilFormula(), checkSettings);
            } else if (pathFormula.isConditionalPathFormula()) {
                return this->computeConditionalProbabilities(pathFormula.asConditionalPathFormula(), checkSettings);
            } else if (pathFormula.isEventuallyFormula()) {
                return this->computeEventuallyProbabilities(pathFormula.asEventuallyFormula(), checkSettings);
            } else if (pathFormula.isGloballyFormula()) {
                return this->computeGloballyProbabilities(pathFormula.asGloballyFormula(), checkSettings);
            } else if (pathFormula.isUntilFormula()) {
                return this->computeUntilProbabilities(pathFormula.asUntilFormula(), checkSettings);
            } else if (pathFormula.isNextFormula()) {
                return this->computeNextProbabilities(pathFormula.asNextFormula(), checkSettings);
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << pathFormula << "' is invalid.");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << pathFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeConditionalProbabilities(storm::logic::ConditionalPathFormula const& pathFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << pathFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeEventuallyProbabilities(storm::logic::EventuallyFormula const& pathFormula, CheckSettings<double> const& checkSettings) {
            storm::logic::UntilFormula newFormula(storm::logic::Formula::getTrueFormula(), pathFormula.getSubformula().asSharedPointer());
            return this->computeUntilProbabilities(newFormula, checkSettings);
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeGloballyProbabilities(storm::logic::GloballyFormula const& pathFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << pathFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeNextProbabilities(storm::logic::NextFormula const& pathFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << pathFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << pathFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeRewards(storm::logic::RewardPathFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) {
            if (rewardPathFormula.isCumulativeRewardFormula()) {
                return this->computeCumulativeRewards(rewardPathFormula.asCumulativeRewardFormula(), checkSettings);
            } else if (rewardPathFormula.isInstantaneousRewardFormula()) {
                return this->computeInstantaneousRewards(rewardPathFormula.asInstantaneousRewardFormula(), checkSettings);
            } else if (rewardPathFormula.isReachabilityRewardFormula()) {
                return this->computeReachabilityRewards(rewardPathFormula.asReachabilityRewardFormula(), checkSettings);
            } else if (rewardPathFormula.isLongRunAverageRewardFormula()) {
                return this->computeLongRunAverageRewards(rewardPathFormula.asLongRunAverageRewardFormula(), checkSettings);
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << rewardPathFormula << "' is invalid.");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << rewardPathFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << rewardPathFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << rewardPathFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeLongRunAverageRewards(storm::logic::LongRunAverageRewardFormula const& rewardPathFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << rewardPathFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeLongRunAverageProbabilities(storm::logic::StateFormula const& eventuallyFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the computation of long-run averages.");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeExpectedTimes(storm::logic::EventuallyFormula const& eventuallyFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the computation of expected times.");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkStateFormula(storm::logic::StateFormula const& stateFormula, CheckSettings<double> const& checkSettings) {
            if (stateFormula.isBinaryBooleanStateFormula()) {
                return this->checkBinaryBooleanStateFormula(stateFormula.asBinaryBooleanStateFormula(), checkSettings);
            } else if (stateFormula.isUnaryBooleanStateFormula()) {
                return this->checkUnaryBooleanStateFormula(stateFormula.asUnaryBooleanStateFormula(), checkSettings);
            } else if (stateFormula.isBooleanLiteralFormula()) {
                return this->checkBooleanLiteralFormula(stateFormula.asBooleanLiteralFormula(), checkSettings);
            } else if (stateFormula.isProbabilityOperatorFormula()) {
                return this->checkProbabilityOperatorFormula(stateFormula.asProbabilityOperatorFormula(), checkSettings);
            } else if (stateFormula.isRewardOperatorFormula()) {
                return this->checkRewardOperatorFormula(stateFormula.asRewardOperatorFormula(), checkSettings);
            } else if (stateFormula.isExpectedTimeOperatorFormula()) {
                return this->checkExpectedTimeOperatorFormula(stateFormula.asExpectedTimeOperatorFormula(), checkSettings);
            } else if (stateFormula.isLongRunAverageOperatorFormula()) {
                return this->checkLongRunAverageOperatorFormula(stateFormula.asLongRunAverageOperatorFormula(), checkSettings);
            } else if (stateFormula.isAtomicExpressionFormula()) {
                return this->checkAtomicExpressionFormula(stateFormula.asAtomicExpressionFormula(), checkSettings);
            } else if (stateFormula.isAtomicLabelFormula()) {
                return this->checkAtomicLabelFormula(stateFormula.asAtomicLabelFormula(), checkSettings);
            } else if (stateFormula.isBooleanLiteralFormula()) {
                return this->checkBooleanLiteralFormula(stateFormula.asBooleanLiteralFormula(), checkSettings);
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << stateFormula << "' is invalid.");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkAtomicExpressionFormula(storm::logic::AtomicExpressionFormula const& stateFormula, CheckSettings<double> const& checkSettings) {
            std::stringstream stream;
            stream << stateFormula.getExpression();
            return this->checkAtomicLabelFormula(storm::logic::AtomicLabelFormula(stream.str()), checkSettings);
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkAtomicLabelFormula(storm::logic::AtomicLabelFormula const& stateFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << stateFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkBinaryBooleanStateFormula(storm::logic::BinaryBooleanStateFormula const& stateFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(stateFormula.getLeftSubformula().isStateFormula() && stateFormula.getRightSubformula().isStateFormula(), storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
            
            std::unique_ptr<CheckResult> leftResult = this->check(stateFormula.getLeftSubformula().asStateFormula());
            std::unique_ptr<CheckResult> rightResult = this->check(stateFormula.getRightSubformula().asStateFormula());
            
            STORM_LOG_THROW(leftResult->isQualitative() && rightResult->isQualitative(), storm::exceptions::InternalTypeErrorException, "Expected qualitative results.");
            
            if (stateFormula.isAnd()) {
                leftResult->asQualitativeCheckResult() &= rightResult->asQualitativeCheckResult();
            } else if (stateFormula.isOr()) {
                leftResult->asQualitativeCheckResult() |= rightResult->asQualitativeCheckResult();
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << stateFormula << "' is invalid.");
            }
            
            return leftResult;
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkBooleanLiteralFormula(storm::logic::BooleanLiteralFormula const& stateFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << stateFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkProbabilityOperatorFormula(storm::logic::ProbabilityOperatorFormula const& stateFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(stateFormula.getSubformula().isPathFormula(), storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
            
            CheckSettings<double> newCheckSettings =
            
            // If the probability bound is 0 or 1, is suffices to do qualitative model checking.
            bool qualitative = false;
            if (stateFormula.hasBound()) {
                if (storm::utility::isZero(stateFormula.getBound()) || storm::utility::isOne(stateFormula.getBound())) {
                    qualitative = true;
                }
            }
            
            std::unique_ptr<CheckResult> result;
            if (stateFormula.hasOptimalityType()) {
                result = this->computeProbabilities(stateFormula.getSubformula().asPathFormula(), qualitative, stateFormula.getOptimalityType());
            } else if (stateFormula.hasBound()) {
                if (stateFormula.getComparisonType() == storm::logic::ComparisonType::Less || stateFormula.getComparisonType() == storm::logic::ComparisonType::LessEqual) {
                    result = this->computeProbabilities(stateFormula.getSubformula().asPathFormula(), qualitative, OptimizationDirection::Maximize);
                } else {
                    result = this->computeProbabilities(stateFormula.getSubformula().asPathFormula(), qualitative, OptimizationDirection::Minimize);
                }
            } else {
                result = this->computeProbabilities(stateFormula.getSubformula().asPathFormula(), qualitative);
            }
            
            if (stateFormula.hasBound()) {
                STORM_LOG_THROW(result->isQuantitative(), storm::exceptions::InvalidOperationException, "Unable to perform comparison operation on non-quantitative result.");
                return result->asQuantitativeCheckResult().compareAgainstBound(stateFormula.getComparisonType(), stateFormula.getBound());
            } else {
                return result;
            }
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkRewardOperatorFormula(storm::logic::RewardOperatorFormula const& stateFormula, CheckSettings<double> const& checkSettings) {
            STORM_LOG_THROW(stateFormula.getSubformula().isRewardPathFormula(), storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
            
            // If the reward bound is 0, is suffices to do qualitative model checking.
            bool qualitative = false;
            if (stateFormula.hasBound()) {
                if (storm::utility::isZero(stateFormula.getBound())) {
                    qualitative = true;
                }
            }
            
            std::unique_ptr<CheckResult> result;
            if (stateFormula.hasOptimalityType()) {
                result = this->computeRewards(stateFormula.getSubformula().asRewardPathFormula(), stateFormula.getOptionalRewardModelName(), qualitative, stateFormula.getOptimalityType());
            } else if (stateFormula.hasBound()) {
                if (stateFormula.getComparisonType() == storm::logic::ComparisonType::Less || stateFormula.getComparisonType() == storm::logic::ComparisonType::LessEqual) {
                    result = this->computeRewards(stateFormula.getSubformula().asRewardPathFormula(), stateFormula.getOptionalRewardModelName(), qualitative, OptimizationDirection::Maximize);
                } else {
                    result = this->computeRewards(stateFormula.getSubformula().asRewardPathFormula(), stateFormula.getOptionalRewardModelName(), qualitative, OptimizationDirection::Minimize);
                }
            } else {
                result = this->computeRewards(stateFormula.getSubformula().asRewardPathFormula(), stateFormula.getOptionalRewardModelName(), qualitative);
            }
            
            if (stateFormula.hasBound()) {
                STORM_LOG_THROW(result->isQuantitative(), storm::exceptions::InvalidOperationException, "Unable to perform comparison operation on non-quantitative result.");
                return result->asQuantitativeCheckResult().compareAgainstBound(stateFormula.getComparisonType(), stateFormula.getBound());
            } else {
                return result;
            }
        }
        
		std::unique_ptr<CheckResult> AbstractModelChecker::checkExpectedTimeOperatorFormula(storm::logic::ExpectedTimeOperatorFormula const& stateFormula, CheckSettings<double> const& checkSettings) {
			STORM_LOG_THROW(stateFormula.getSubformula().isEventuallyFormula(), storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
            
            // If the reward bound is 0, is suffices to do qualitative model checking.
            bool qualitative = false;
            if (stateFormula.hasBound()) {
                if (storm::utility::isZero(stateFormula.getBound())) {
                    qualitative = true;
                }
            }
            
            std::unique_ptr<CheckResult> result;
            if (stateFormula.hasOptimalityType()) {
                result = this->computeExpectedTimes(stateFormula.getSubformula().asEventuallyFormula(), qualitative, stateFormula.getOptimalityType());
            } else if (stateFormula.hasBound()) {
                if (stateFormula.getComparisonType() == storm::logic::ComparisonType::Less || stateFormula.getComparisonType() == storm::logic::ComparisonType::LessEqual) {
                    result = this->computeExpectedTimes(stateFormula.getSubformula().asEventuallyFormula(), qualitative, OptimizationDirection::Maximize);
                } else {
                    result = this->computeExpectedTimes(stateFormula.getSubformula().asEventuallyFormula(), qualitative, OptimizationDirection::Minimize);
                }
            } else {
                result = this->computeExpectedTimes(stateFormula.getSubformula().asEventuallyFormula(), qualitative);
            }
            
            if (stateFormula.hasBound()) {
                STORM_LOG_THROW(result->isQuantitative(), storm::exceptions::InvalidOperationException, "Unable to perform comparison operation on non-quantitative result.");
                return result->asQuantitativeCheckResult().compareAgainstBound(stateFormula.getComparisonType(), stateFormula.getBound());
            } else {
                return result;
            }
        }
        
		std::unique_ptr<CheckResult> AbstractModelChecker::checkLongRunAverageOperatorFormula(storm::logic::LongRunAverageOperatorFormula const& stateFormula, CheckSettings<double> const& checkSettings) {
			STORM_LOG_THROW(stateFormula.getSubformula().isStateFormula(), storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
            
            std::unique_ptr<CheckResult> result;
            if (stateFormula.hasOptimalityType()) {
                result = this->computeLongRunAverageProbabilities(stateFormula.getSubformula().asStateFormula(), false, stateFormula.getOptimalityType());
            } else if (stateFormula.hasBound()) {
                if (stateFormula.getComparisonType() == storm::logic::ComparisonType::Less || stateFormula.getComparisonType() == storm::logic::ComparisonType::LessEqual) {
                    result = this->computeLongRunAverageProbabilities(stateFormula.getSubformula().asStateFormula(), false, OptimizationDirection::Maximize);
                } else {
                    result = this->computeLongRunAverageProbabilities(stateFormula.getSubformula().asStateFormula(), false, OptimizationDirection::Minimize);
                }
            } else {
                result = this->computeLongRunAverageProbabilities(stateFormula.getSubformula().asStateFormula(), false);
            }
            
            if (stateFormula.hasBound()) {
                return result->asQuantitativeCheckResult().compareAgainstBound(stateFormula.getComparisonType(), stateFormula.getBound());
            } else {
                return result;
            }
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkUnaryBooleanStateFormula(storm::logic::UnaryBooleanStateFormula const& stateFormula, CheckSettings<double> const& checkSettings) {
            std::unique_ptr<CheckResult> subResult = this->check(stateFormula.getSubformula());
            STORM_LOG_THROW(subResult->isQualitative(), storm::exceptions::InternalTypeErrorException, "Expected qualitative result.");
            if (stateFormula.isNot()) {
                subResult->asQualitativeCheckResult().complement();
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << stateFormula << "' is invalid.");
            }
            return subResult;
        }
    }
}