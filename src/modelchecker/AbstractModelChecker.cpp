#include "src/modelchecker/AbstractModelChecker.h"

#include "src/modelchecker/results/QualitativeCheckResult.h"
#include "src/modelchecker/results/QuantitativeCheckResult.h"
#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/InvalidOperationException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InternalTypeErrorException.h"

#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Ctmc.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/MarkovAutomaton.h"
#include "src/models/sparse/StandardRewardModel.h"

namespace storm {
    namespace modelchecker {

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::check(CheckTask<storm::logic::Formula, ValueType> const& checkTask) {
            storm::logic::Formula const& formula = checkTask.getFormula();
            STORM_LOG_THROW(this->canHandle(checkTask), storm::exceptions::InvalidArgumentException, "The model checker is not able to check the formula '" << formula << "'.");
            if (formula.isStateFormula()) {
                return this->checkStateFormula(checkTask.substituteFormula(formula.asStateFormula()));
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << formula << "' is invalid.");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeProbabilities(CheckTask<storm::logic::Formula, ValueType> const& checkTask) {
            storm::logic::Formula const& formula = checkTask.getFormula();
            if (formula.isBoundedUntilFormula()) {
                return this->computeBoundedUntilProbabilities(checkTask.substituteFormula(formula.asBoundedUntilFormula()));
            } else if (formula.isReachabilityProbabilityFormula()) {
                return this->computeReachabilityProbabilities(checkTask.substituteFormula(formula.asReachabilityProbabilityFormula()));
            } else if (formula.isGloballyFormula()) {
                return this->computeGloballyProbabilities(checkTask.substituteFormula(formula.asGloballyFormula()));
            } else if (formula.isUntilFormula()) {
                return this->computeUntilProbabilities(checkTask.substituteFormula(formula.asUntilFormula()));
            } else if (formula.isNextFormula()) {
                return this->computeNextProbabilities(checkTask.substituteFormula(formula.asNextFormula()));
            } else if (formula.isConditionalProbabilityFormula()) {
                return this->computeConditionalProbabilities(checkTask.substituteFormula(formula.asConditionalFormula()));
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << formula << "' is invalid.");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << checkTask.getFormula() << ".");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeConditionalProbabilities(CheckTask<storm::logic::ConditionalFormula, ValueType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << checkTask.getFormula() << ".");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            storm::logic::EventuallyFormula const& pathFormula = checkTask.getFormula();
            storm::logic::UntilFormula newFormula(storm::logic::Formula::getTrueFormula(), pathFormula.getSubformula().asSharedPointer());
            return this->computeUntilProbabilities(checkTask.substituteFormula(newFormula));
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeGloballyProbabilities(CheckTask<storm::logic::GloballyFormula, ValueType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << checkTask.getFormula() << ".");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeNextProbabilities(CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << checkTask.getFormula() << ".");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeUntilProbabilities(CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << checkTask.getFormula() << ".");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::Formula, ValueType> const& checkTask) {
            storm::logic::Formula const& rewardFormula = checkTask.getFormula();
            if (rewardFormula.isCumulativeRewardFormula()) {
                return this->computeCumulativeRewards(rewardMeasureType, checkTask.substituteFormula(rewardFormula.asCumulativeRewardFormula()));
            } else if (rewardFormula.isInstantaneousRewardFormula()) {
                return this->computeInstantaneousRewards(rewardMeasureType, checkTask.substituteFormula(rewardFormula.asInstantaneousRewardFormula()));
            } else if (rewardFormula.isReachabilityRewardFormula()) {
                return this->computeReachabilityRewards(rewardMeasureType, checkTask.substituteFormula(rewardFormula.asReachabilityRewardFormula()));
            } else if (rewardFormula.isLongRunAverageRewardFormula()) {
                return this->computeLongRunAverageRewards(rewardMeasureType, checkTask.substituteFormula(rewardFormula.asLongRunAverageRewardFormula()));
            } else if (rewardFormula.isConditionalRewardFormula()) {
                return this->computeConditionalRewards(rewardMeasureType, checkTask.substituteFormula(rewardFormula.asConditionalFormula()));
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << rewardFormula << "' is invalid.");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeConditionalRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::ConditionalFormula, ValueType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << checkTask.getFormula() << ".");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeCumulativeRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << checkTask.getFormula() << ".");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeInstantaneousRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << checkTask.getFormula() << ".");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeReachabilityRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << checkTask.getFormula() << ".");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeLongRunAverageRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << checkTask.getFormula() << ".");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << checkTask.getFormula() << ".");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeTimes(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::Formula, ValueType> const& checkTask) {
            storm::logic::Formula const& timeFormula = checkTask.getFormula();
            if (timeFormula.isReachabilityTimeFormula()) {
                return this->computeReachabilityTimes(rewardMeasureType, checkTask.substituteFormula(timeFormula.asReachabilityTimeFormula()));
            }
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << checkTask.getFormula() << ".");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeReachabilityTimes(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << checkTask.getFormula() << ".");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkStateFormula(CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) {
            storm::logic::StateFormula const& stateFormula = checkTask.getFormula();
            if (stateFormula.isBinaryBooleanStateFormula()) {
                return this->checkBinaryBooleanStateFormula(checkTask.substituteFormula(stateFormula.asBinaryBooleanStateFormula()));
            } else if (stateFormula.isUnaryBooleanStateFormula()) {
                return this->checkUnaryBooleanStateFormula(checkTask.substituteFormula(stateFormula.asUnaryBooleanStateFormula()));
            } else if (stateFormula.isBooleanLiteralFormula()) {
                return this->checkBooleanLiteralFormula(checkTask.substituteFormula(stateFormula.asBooleanLiteralFormula()));
            } else if (stateFormula.isProbabilityOperatorFormula()) {
                return this->checkProbabilityOperatorFormula(checkTask.substituteFormula(stateFormula.asProbabilityOperatorFormula()));
            } else if (stateFormula.isRewardOperatorFormula()) {
                return this->checkRewardOperatorFormula(checkTask.substituteFormula(stateFormula.asRewardOperatorFormula()));
            } else if (stateFormula.isTimeOperatorFormula()) {
                return this->checkTimeOperatorFormula(checkTask.substituteFormula(stateFormula.asTimeOperatorFormula()));
            } else if (stateFormula.isLongRunAverageOperatorFormula()) {
                return this->checkLongRunAverageOperatorFormula(checkTask.substituteFormula(stateFormula.asLongRunAverageOperatorFormula()));
            } else if (stateFormula.isAtomicExpressionFormula()) {
                return this->checkAtomicExpressionFormula(checkTask.substituteFormula(stateFormula.asAtomicExpressionFormula()));
            } else if (stateFormula.isAtomicLabelFormula()) {
                return this->checkAtomicLabelFormula(checkTask.substituteFormula(stateFormula.asAtomicLabelFormula()));
            } else if (stateFormula.isBooleanLiteralFormula()) {
                return this->checkBooleanLiteralFormula(checkTask.substituteFormula(stateFormula.asBooleanLiteralFormula()));
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << stateFormula << "' is invalid.");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkAtomicExpressionFormula(CheckTask<storm::logic::AtomicExpressionFormula, ValueType> const& checkTask) {
            storm::logic::AtomicExpressionFormula const& stateFormula = checkTask.getFormula();
            std::stringstream stream;
            stream << stateFormula.getExpression();
            return this->checkAtomicLabelFormula(checkTask.substituteFormula(storm::logic::AtomicLabelFormula(stream.str())));
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkAtomicLabelFormula(CheckTask<storm::logic::AtomicLabelFormula, ValueType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << checkTask.getFormula() << ".");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkBinaryBooleanStateFormula(CheckTask<storm::logic::BinaryBooleanStateFormula, ValueType> const& checkTask) {
            storm::logic::BinaryBooleanStateFormula const& stateFormula = checkTask.getFormula();
            STORM_LOG_THROW(stateFormula.getLeftSubformula().isStateFormula() && stateFormula.getRightSubformula().isStateFormula(), storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
            
            std::unique_ptr<CheckResult> leftResult = this->check(checkTask.template substituteFormula<storm::logic::Formula>(stateFormula.getLeftSubformula().asStateFormula()));
            std::unique_ptr<CheckResult> rightResult = this->check(checkTask.template substituteFormula<storm::logic::Formula>(stateFormula.getRightSubformula().asStateFormula()));
            
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

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkBooleanLiteralFormula(CheckTask<storm::logic::BooleanLiteralFormula, ValueType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << checkTask.getFormula() << ".");
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkProbabilityOperatorFormula(CheckTask<storm::logic::ProbabilityOperatorFormula, ValueType> const& checkTask) {
            storm::logic::ProbabilityOperatorFormula const& stateFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> result = this->computeProbabilities(checkTask.substituteFormula(stateFormula.getSubformula()));
            
            if (stateFormula.hasBound()) {
                STORM_LOG_THROW(result->isQuantitative(), storm::exceptions::InvalidOperationException, "Unable to perform comparison operation on non-quantitative result.");
                return result->asQuantitativeCheckResult<ValueType>().compareAgainstBound(stateFormula.getComparisonType(), stateFormula.getThreshold());
            } else {
                return result;
            }
        }

        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkRewardOperatorFormula(CheckTask<storm::logic::RewardOperatorFormula, ValueType> const& checkTask) {
            storm::logic::RewardOperatorFormula const& stateFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> result = this->computeRewards(stateFormula.getMeasureType(), checkTask.substituteFormula(stateFormula.getSubformula()));
            
            if (checkTask.isBoundSet()) {
                STORM_LOG_THROW(result->isQuantitative(), storm::exceptions::InvalidOperationException, "Unable to perform comparison operation on non-quantitative result.");
                return result->asQuantitativeCheckResult<ValueType>().compareAgainstBound(checkTask.getBoundComparisonType(), checkTask.getBoundThreshold());
            } else {
                return result;
            }
        }

        template<typename ModelType>
		std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkTimeOperatorFormula(CheckTask<storm::logic::TimeOperatorFormula, ValueType> const& checkTask) {
            storm::logic::TimeOperatorFormula const& stateFormula = checkTask.getFormula();
			STORM_LOG_THROW(stateFormula.getSubformula().isReachabilityTimeFormula(), storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
            
            std::unique_ptr<CheckResult> result = this->computeTimes(stateFormula.getMeasureType(), checkTask.substituteFormula(stateFormula.getSubformula()));
            
            if (checkTask.isBoundSet()) {
                STORM_LOG_THROW(result->isQuantitative(), storm::exceptions::InvalidOperationException, "Unable to perform comparison operation on non-quantitative result.");
                return result->asQuantitativeCheckResult<ValueType>().compareAgainstBound(checkTask.getBoundComparisonType(), checkTask.getBoundThreshold());
            } else {
                return result;
            }
        }

        template<typename ModelType>
		std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkLongRunAverageOperatorFormula(CheckTask<storm::logic::LongRunAverageOperatorFormula, ValueType> const& checkTask) {
            storm::logic::LongRunAverageOperatorFormula const& stateFormula = checkTask.getFormula();
			STORM_LOG_THROW(stateFormula.getSubformula().isStateFormula(), storm::exceptions::InvalidArgumentException, "The given formula is invalid.");
            
            std::unique_ptr<CheckResult> result = this->computeLongRunAverageProbabilities(checkTask.substituteFormula(stateFormula.getSubformula().asStateFormula()));
            
            if (checkTask.isBoundSet()) {
                STORM_LOG_THROW(result->isQuantitative(), storm::exceptions::InvalidOperationException, "Unable to perform comparison operation on non-quantitative result.");
                return result->asQuantitativeCheckResult<ValueType>().compareAgainstBound(checkTask.getBoundComparisonType(), checkTask.getBoundThreshold());
            } else {
                return result;
            }
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkUnaryBooleanStateFormula(CheckTask<storm::logic::UnaryBooleanStateFormula, ValueType> const& checkTask) {
            storm::logic::UnaryBooleanStateFormula const& stateFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResult = this->check(checkTask.template substituteFormula<storm::logic::Formula>(stateFormula.getSubformula()));
            STORM_LOG_THROW(subResult->isQualitative(), storm::exceptions::InternalTypeErrorException, "Expected qualitative result.");
            if (stateFormula.isNot()) {
                subResult->asQualitativeCheckResult().complement();
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << stateFormula << "' is invalid.");
            }
            return subResult;
        }

        // Explicitly instantiate the template class.
        template class AbstractModelChecker<storm::models::sparse::Model<double>>;
        template class AbstractModelChecker<storm::models::sparse::Dtmc<double>>;
        template class AbstractModelChecker<storm::models::sparse::Ctmc<double>>;
        template class AbstractModelChecker<storm::models::sparse::Mdp<double>>;
        template class AbstractModelChecker<storm::models::sparse::MarkovAutomaton<double>>;

#ifdef STORM_HAVE_CARL
        template class AbstractModelChecker<storm::models::sparse::Mdp<double, storm::models::sparse::StandardRewardModel<storm::Interval>>>;

        template class AbstractModelChecker<storm::models::sparse::Model<storm::RationalNumber>>;
        template class AbstractModelChecker<storm::models::sparse::Dtmc<storm::RationalNumber>>;
        template class AbstractModelChecker<storm::models::sparse::Ctmc<storm::RationalNumber>>;
        template class AbstractModelChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
        template class AbstractModelChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;

        template class AbstractModelChecker<storm::models::sparse::Model<storm::RationalFunction>>;
        template class AbstractModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>>;
        template class AbstractModelChecker<storm::models::sparse::Ctmc<storm::RationalFunction>>;
        template class AbstractModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>>;
        template class AbstractModelChecker<storm::models::sparse::MarkovAutomaton<storm::RationalFunction>>;
#endif
    }
}
