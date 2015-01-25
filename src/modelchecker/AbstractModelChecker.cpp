#include "src/modelchecker/AbstractModelChecker.h"

#include "src/utility/ConstantsComparator.h"
#include "src/utility/macros.h"
#include "src/exceptions/NotImplementedException.h"

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
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
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
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
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
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
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
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkStateFormula(storm::logic::StateFormula const& stateFormula) {
            if (stateFormula.isBinaryBooleanStateFormula()) {
                return this->checkBinaryBooleanStateFormula(stateFormula.asBinaryBooleanStateFormula());
            } else if (stateFormula.isBooleanLiteralFormula()) {
                return this->checkBooleanLiteralFormula(stateFormula.asBooleanLiteralFormula());
            } else if (stateFormula.isProbabilityOperatorFormula()) {
                return this->checkProbabilityOperatorFormula(stateFormula.asProbabilityOperatorFormula());
            } else if (stateFormula.isRewardOperatorFormula()) {
                return this->checkRewardOperatorFormula(stateFormula.asRewardOperatorFormula());
            } else if (stateFormula.isSteadyStateOperatorFormula()) {
                return this->checkSteadyStateOperatorFormula(stateFormula.asSteadyStateOperatorFormula());
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
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << stateFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkAtomicLabelFormula(storm::logic::AtomicLabelFormula const& stateFormula) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << stateFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkBinaryBooleanStateFormula(storm::logic::BinaryBooleanStateFormula const& stateFormula) {
            STORM_LOG_THROW(stateFormula.getLeftSubformula().isStateFormula() && stateFormula.getRightSubformula().isStateFormula(), storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
            
            std::unique_ptr<CheckResult> leftResult = this->check(stateFormula.getLeftSubformula().asStateFormula());
            std::unique_ptr<CheckResult> rightResult = this->check(stateFormula.getRightSubformula().asStateFormula());
            
            *leftResult &= *rightResult;
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
            } else {
                result = this->computeProbabilities(stateFormula.getSubformula().asPathFormula(), qualitative);
            }
            
            if (stateFormula.hasBound()) {
                return result->compareAgainstBound(stateFormula.getComparisonType(), stateFormula.getBound());
            } else {
                return result;
            }
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkRewardOperatorFormula(storm::logic::RewardOperatorFormula const& stateFormula) {
            STORM_LOG_THROW(stateFormula.getSubformula().isRewardPathFormula(), storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
            
            // If the probability bound is 0, is suffices to do qualitative model checking.
            bool qualitative = false;
            if (stateFormula.hasBound()) {
                if (stateFormula.getBound() == storm::utility::zero<double>() || stateFormula.getBound() == storm::utility::one<double>()) {
                    qualitative = true;
                }
            }
            if (stateFormula.hasOptimalityType()) {
                return this->computeRewards(stateFormula.getSubformula().asRewardPathFormula(), qualitative, stateFormula.getOptimalityType());
            } else {
                return this->computeRewards(stateFormula.getSubformula().asRewardPathFormula(), qualitative);
            }
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkSteadyStateOperatorFormula(storm::logic::SteadyStateOperatorFormula const& stateFormula) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << stateFormula << ".");
        }
        
        std::unique_ptr<CheckResult> AbstractModelChecker::checkUnaryBooleanStateFormula(storm::logic::UnaryBooleanStateFormula const& stateFormula) {
            std::unique_ptr<CheckResult> subResult = this->check(stateFormula.getSubformula());
            subResult->complement();
            return subResult;
        }
    }
}