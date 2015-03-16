#include "src/modelchecker/AbstractModelChecker.h"

#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {
        std::unique_ptr<CheckResult> AbstractModelChecker::check(storm::logic::Formula const& formula) {
            STORM_LOG_THROW(this->canHandle(formula), storm::exceptions::InvalidArgumentException, "The model checker is not able to check the formula '" << formula << "'.");
            if (formula.isStateFormula()) {
                return this->checkStateFormula(formula.asStateFormula());
            } else if (formula.isPathFormula()) {
                return this->computeProbabilities(formula.asPathFormula());
            } else if (formula.isRewardPathFormula()) {
                return this->computeRewards(formula.asRewardPathFormula());
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << formula << "' is invalid.");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeProbabilities(storm::logic::PathFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            if (pathFormula.isBoundedUntilFormula()) {
                return this->computeBoundedUntilProbabilities(pathFormula.asBoundedUntilFormula(), qualitative, optimalityType);
            } else if (pathFormula.isConditionalPathFormula()) {
                return this->computeConditionalProbabilities(pathFormula.asConditionalPathFormula(), qualitative, optimalityType);
            } else if (pathFormula.isEventuallyFormula()) {
                return this->computeEventuallyProbabilities(pathFormula.asEventuallyFormula(), qualitative, optimalityType);
            } else if (pathFormula.isGloballyFormula()) {
                return this->computeGloballyProbabilities(pathFormula.asGloballyFormula(), qualitative, optimalityType);
            } else if (pathFormula.isUntilFormula()) {
                return this->computeUntilProbabilities(pathFormula.asUntilFormula(), qualitative, optimalityType);
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << pathFormula << "' is invalid.");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << pathFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeConditionalProbabilities(storm::logic::ConditionalPathFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << pathFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeEventuallyProbabilities(storm::logic::EventuallyFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            storm::logic::UntilFormula newFormula(storm::logic::Formula::getTrueFormula(), pathFormula.getSubformula().asSharedPointer());
            return this->computeUntilProbabilities(newFormula, qualitative, optimalityType);
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeGloballyProbabilities(storm::logic::GloballyFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << pathFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << pathFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << pathFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeRewards(storm::logic::RewardPathFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            if (rewardPathFormula.isCumulativeRewardFormula()) {
                return this->computeCumulativeRewards(rewardPathFormula.asCumulativeRewardFormula(), qualitative, optimalityType);
            } else if (rewardPathFormula.isInstantaneousRewardFormula()) {
                return this->computeInstantaneousRewards(rewardPathFormula.asInstantaneousRewardFormula(), qualitative, optimalityType);
            } else if (rewardPathFormula.isReachabilityRewardFormula()) {
                return this->computeReachabilityRewards(rewardPathFormula.asReachabilityRewardFormula(), qualitative, optimalityType);
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << rewardPathFormula << "' is invalid.");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << rewardPathFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << rewardPathFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << rewardPathFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeLongRunAverage(storm::logic::StateFormula const& eventuallyFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the computation of long-run averages.");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::computeExpectedTimes(storm::logic::EventuallyFormula const& eventuallyFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the computation of expected times.");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkStateFormula(storm::logic::StateFormula const& stateFormula) {
            if (stateFormula.isBinaryBooleanStateFormula()) {
                return this->checkBinaryBooleanStateFormula(stateFormula.asBinaryBooleanStateFormula());
            } else if (stateFormula.isUnaryBooleanStateFormula()) {
                return this->checkUnaryBooleanStateFormula(stateFormula.asUnaryBooleanStateFormula());
            } else if (stateFormula.isBooleanLiteralFormula()) {
                return this->checkBooleanLiteralFormula(stateFormula.asBooleanLiteralFormula());
            } else if (stateFormula.isProbabilityOperatorFormula()) {
                return this->checkProbabilityOperatorFormula(stateFormula.asProbabilityOperatorFormula());
            } else if (stateFormula.isRewardOperatorFormula()) {
                return this->checkRewardOperatorFormula(stateFormula.asRewardOperatorFormula());
            } else if (stateFormula.isExpectedTimeOperatorFormula()) {
                return this->checkExpectedTimeOperatorFormula(stateFormula.asExpectedTimeOperatorFormula());
            } else if (stateFormula.isLongRunAverageOperatorFormula()) {
                return this->checkLongRunAverageOperatorFormula(stateFormula.asLongRunAverageOperatorFormula());
            } else if (stateFormula.isAtomicExpressionFormula()) {
                return this->checkAtomicExpressionFormula(stateFormula.asAtomicExpressionFormula());
            } else if (stateFormula.isAtomicLabelFormula()) {
                return this->checkAtomicLabelFormula(stateFormula.asAtomicLabelFormula());
            } else if (stateFormula.isBooleanLiteralFormula()) {
                return this->checkBooleanLiteralFormula(stateFormula.asBooleanLiteralFormula());
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << stateFormula << "' is invalid.");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkAtomicExpressionFormula(storm::logic::AtomicExpressionFormula const& stateFormula) {
            std::stringstream stream;
            stream << stateFormula.getExpression();
            return this->checkAtomicLabelFormula(storm::logic::AtomicLabelFormula(stream.str()));
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkAtomicLabelFormula(storm::logic::AtomicLabelFormula const& stateFormula) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << stateFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkBinaryBooleanStateFormula(storm::logic::BinaryBooleanStateFormula const& stateFormula) {
            STORM_LOG_THROW(stateFormula.getLeftSubformula().isStateFormula() && stateFormula.getRightSubformula().isStateFormula(), storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
            
            std::unique_ptr<CheckResult> leftResult = this->check(stateFormula.getLeftSubformula().asStateFormula());
            std::unique_ptr<CheckResult> rightResult = this->check(stateFormula.getRightSubformula().asStateFormula());
            
            if (stateFormula.isAnd()) {
                *leftResult &= *rightResult;
            } else if (stateFormula.isOr()) {
                *leftResult |= *rightResult;
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << stateFormula << "' is invalid.");
            }
            
            return leftResult;
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkBooleanLiteralFormula(storm::logic::BooleanLiteralFormula const& stateFormula) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << stateFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkProbabilityOperatorFormula(storm::logic::ProbabilityOperatorFormula const& stateFormula) {
            STORM_LOG_THROW(stateFormula.getSubformula().isPathFormula(), storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
            
            // If the probability bound is 0 or 1, is suffices to do qualitative model checking.
            bool qualitative = false;
            if (stateFormula.hasBound()) {
                if (stateFormula.getBound() == storm::utility::zero<double>() || stateFormula.getBound() == storm::utility::one<double>()) {
                    qualitative = true;
                }
            }
            
            std::unique_ptr<CheckResult> result;
            if (stateFormula.hasOptimalityType()) {
                result = this->computeProbabilities(stateFormula.getSubformula().asPathFormula(), qualitative, stateFormula.getOptimalityType());
            } else if (stateFormula.hasBound()) {
                if (stateFormula.getComparisonType() == storm::logic::ComparisonType::Less || storm::logic::ComparisonType::LessEqual) {
                    result = this->computeProbabilities(stateFormula.getSubformula().asPathFormula(), storm::logic::OptimalityType::Maximize);
                } else {
                    result = this->computeProbabilities(stateFormula.getSubformula().asPathFormula(), storm::logic::OptimalityType::Minimize);
                }
            } else {
                result = this->computeProbabilities(stateFormula.getSubformula().asPathFormula(), qualitative);
            }
            
            if (stateFormula.hasBound()) {
                STORM_LOG_THROW(result->isQuantitative(), storm::exceptions::InvalidOperationException, "Unable to perform comparison operation on non-quantitative result.");
                return result->compareAgainstBound(stateFormula.getComparisonType(), stateFormula.getBound());
            } else {
                return result;
            }
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkRewardOperatorFormula(storm::logic::RewardOperatorFormula const& stateFormula) {
            STORM_LOG_THROW(stateFormula.getSubformula().isRewardPathFormula(), storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
            
            // If the reward bound is 0, is suffices to do qualitative model checking.
            bool qualitative = false;
            if (stateFormula.hasBound()) {
                if (stateFormula.getBound() == storm::utility::zero<double>()) {
                    qualitative = true;
                }
            }
            
            std::unique_ptr<CheckResult> result;
            if (stateFormula.hasOptimalityType()) {
                result = this->computeRewards(stateFormula.getSubformula().asRewardPathFormula(), qualitative, stateFormula.getOptimalityType());
            } else if (stateFormula.hasBound()) {
                if (stateFormula.getComparisonType() == storm::logic::ComparisonType::Less || storm::logic::ComparisonType::LessEqual) {
                    result = this->computeRewards(stateFormula.getSubformula().asRewardPathFormula(), storm::logic::OptimalityType::Maximize);
                } else {
                    result = this->computeRewards(stateFormula.getSubformula().asRewardPathFormula(), storm::logic::OptimalityType::Minimize);
                }
            } else {
                result = this->computeRewards(stateFormula.getSubformula().asRewardPathFormula(), qualitative);
            }
            
            if (stateFormula.hasBound()) {
                STORM_LOG_THROW(result->isQuantitative(), storm::exceptions::InvalidOperationException, "Unable to perform comparison operation on non-quantitative result.");
                return result->compareAgainstBound(stateFormula.getComparisonType(), stateFormula.getBound());
            } else {
                return result;
            }
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkExpectedTimeOperatorFormula(storm::logic::ExpectedTimeOperatorFormula const& stateFormula) {
            STORM_LOG_THROW(stateFormula.getSubformula().isStateFormula(), storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
            
            // If the reward bound is 0, is suffices to do qualitative model checking.
            bool qualitative = false;
            if (stateFormula.hasBound()) {
                if (stateFormula.getBound() == storm::utility::zero<double>()) {
                    qualitative = true;
                }
            }
            
            std::unique_ptr<CheckResult> result;
            if (stateFormula.hasOptimalityType()) {
                result = this->computeExpectedTimes(stateFormula.getSubformula().asEventuallyFormula(), qualitative, stateFormula.getOptimalityType());
            } else if (stateFormula.hasBound()) {
                if (stateFormula.getComparisonType() == storm::logic::ComparisonType::Less || storm::logic::ComparisonType::LessEqual) {
                    result = this->computeExpectedTimes(stateFormula.getSubformula().asEventuallyFormula(), storm::logic::OptimalityType::Maximize);
                } else {
                    result = this->computeExpectedTimes(stateFormula.getSubformula().asEventuallyFormula(), storm::logic::OptimalityType::Minimize);
                }
            } else {
                result = this->computeExpectedTimes(stateFormula.getSubformula().asEventuallyFormula(), qualitative);
            }
            
            if (stateFormula.hasBound()) {
                STORM_LOG_THROW(result->isQuantitative(), storm::exceptions::InvalidOperationException, "Unable to perform comparison operation on non-quantitative result.");
                return result->compareAgainstBound(stateFormula.getComparisonType(), stateFormula.getBound());
            } else {
                return result;
            }
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkLongRunAverageOperatorFormula(storm::logic::LongRunAverageOperatorFormula const& stateFormula) {
            STORM_LOG_THROW(stateFormula.getSubformula().isEventuallyFormula(), storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
            
            std::unique_ptr<CheckResult> result;
            if (stateFormula.hasOptimalityType()) {
                result = this->computeLongRunAverage(stateFormula.getSubformula().asStateFormula(), false, stateFormula.getOptimalityType());
            } else if (stateFormula.hasBound()) {
                if (stateFormula.getComparisonType() == storm::logic::ComparisonType::Less || storm::logic::ComparisonType::LessEqual) {
                    result = this->computeLongRunAverage(stateFormula.getSubformula().asStateFormula(), storm::logic::OptimalityType::Maximize);
                } else {
                    result = this->computeLongRunAverage(stateFormula.getSubformula().asStateFormula(), storm::logic::OptimalityType::Minimize);
                }
            } else {
                result = this->computeLongRunAverage(stateFormula.getSubformula().asStateFormula(), false);
            }
            
            if (stateFormula.hasBound()) {
                return result->compareAgainstBound(stateFormula.getComparisonType(), stateFormula.getBound());
            } else {
                return result;
            }
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkUnaryBooleanStateFormula(storm::logic::UnaryBooleanStateFormula const& stateFormula) {
            std::unique_ptr<CheckResult> subResult = this->check(stateFormula.getSubformula());
            if (stateFormula.isNot()) {
                subResult->complement();
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << stateFormula << "' is invalid.");
            }
            return subResult;
        }
    }
}