#include "storm/modelchecker/AbstractModelChecker.h"

#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/QualitativeCheckResult.h"
#include "storm/modelchecker/results/QuantitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQuantitativeCheckResult.h"

#include "storm/exceptions/InternalTypeErrorException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/utility/constants.h"
#include "storm/utility/macros.h"

#include "storm/environment/Environment.h"
#include "storm/environment/modelchecker/MultiObjectiveModelCheckerEnvironment.h"

#include "storm/logic/FormulaInformation.h"
#include "storm/models/ModelRepresentation.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/models/sparse/Smg.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/symbolic/Ctmc.h"
#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/MarkovAutomaton.h"
#include "storm/models/symbolic/Mdp.h"
#include "storm/models/symbolic/StandardRewardModel.h"
#include "storm/models/symbolic/StochasticTwoPlayerGame.h"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

#include <boost/core/typeinfo.hpp>
#include <storm/models/symbolic/MarkovAutomaton.h>

namespace storm {
namespace modelchecker {

template<typename ModelType>
std::string AbstractModelChecker<ModelType>::getClassName() const {
    return std::string(boost::core::demangled_name(BOOST_CORE_TYPEID(*this)));
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::check(CheckTask<storm::logic::Formula, ValueType> const& checkTask) {
    Environment env;
    return this->check(env, checkTask);
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::check(Environment const& env, CheckTask<storm::logic::Formula, ValueType> const& checkTask) {
    storm::logic::Formula const& formula = checkTask.getFormula();
    STORM_LOG_THROW(this->canHandle(checkTask), storm::exceptions::InvalidArgumentException,
                    "The model checker (" << getClassName() << ") is not able to check the formula '" << formula << "'.");
    if (formula.isStateFormula()) {
        return this->checkStateFormula(env, checkTask.substituteFormula(formula.asStateFormula()));
    }
    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << formula << "' is invalid.");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeProbabilities(Environment const& env,
                                                                                   CheckTask<storm::logic::Formula, ValueType> const& checkTask) {
    storm::logic::Formula const& formula = checkTask.getFormula();

    if (formula.isStateFormula() || formula.hasQualitativeResult()) {
        return this->computeStateFormulaProbabilities(env, checkTask.substituteFormula(formula));
    }

    if (formula.isHOAPathFormula()) {
        return this->computeHOAPathProbabilities(env, checkTask.substituteFormula(formula.asHOAPathFormula()));
    } else if (formula.isConditionalProbabilityFormula()) {
        return this->computeConditionalProbabilities(env, checkTask.substituteFormula(formula.asConditionalFormula()));
    } else if (formula.info(false).containsComplexPathFormula()) {
        // we need to do LTL model checking
        return this->computeLTLProbabilities(env, checkTask.substituteFormula(formula.asPathFormula()));
    } else if (formula.isBoundedUntilFormula()) {
        return this->computeBoundedUntilProbabilities(env, checkTask.substituteFormula(formula.asBoundedUntilFormula()));
    } else if (formula.isReachabilityProbabilityFormula()) {
        return this->computeReachabilityProbabilities(env, checkTask.substituteFormula(formula.asReachabilityProbabilityFormula()));
    } else if (formula.isGloballyFormula()) {
        return this->computeGloballyProbabilities(env, checkTask.substituteFormula(formula.asGloballyFormula()));
    } else if (formula.isUntilFormula()) {
        return this->computeUntilProbabilities(env, checkTask.substituteFormula(formula.asUntilFormula()));
    } else if (formula.isNextFormula()) {
        return this->computeNextProbabilities(env, checkTask.substituteFormula(formula.asNextFormula()));
    }
    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << formula << "' is invalid.");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeBoundedUntilProbabilities(
    Environment const& env, CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeConditionalProbabilities(
    Environment const& env, CheckTask<storm::logic::ConditionalFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeReachabilityProbabilities(
    Environment const& env, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
    storm::logic::EventuallyFormula const& pathFormula = checkTask.getFormula();
    storm::logic::UntilFormula newFormula(storm::logic::Formula::getTrueFormula(), pathFormula.getSubformula().asSharedPointer());
    return this->computeUntilProbabilities(env, checkTask.substituteFormula(newFormula));
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeGloballyProbabilities(
    Environment const& env, CheckTask<storm::logic::GloballyFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeNextProbabilities(Environment const& env,
                                                                                       CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeUntilProbabilities(Environment const& env,
                                                                                        CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeHOAPathProbabilities(Environment const& env,
                                                                                          CheckTask<storm::logic::HOAPathFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeLTLProbabilities(Environment const& env,
                                                                                      CheckTask<storm::logic::PathFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This model checker does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeStateFormulaProbabilities(Environment const& env,
                                                                                               CheckTask<storm::logic::Formula, ValueType> const& checkTask) {
    // recurse
    std::unique_ptr<CheckResult> resultPointer = this->check(env, checkTask.getFormula());
    if (resultPointer->isExplicitQualitativeCheckResult()) {
        STORM_LOG_ASSERT(ModelType::Representation == storm::models::ModelRepresentation::Sparse, "Unexpected model type.");
        return std::make_unique<ExplicitQuantitativeCheckResult<ValueType>>(resultPointer->asExplicitQualitativeCheckResult());
    } else {
        STORM_LOG_ASSERT(resultPointer->isSymbolicQualitativeCheckResult(), "Unexpected result type.");
        STORM_LOG_ASSERT(ModelType::Representation != storm::models::ModelRepresentation::Sparse, "Unexpected model type.");
        auto const& qualRes = resultPointer->asSymbolicQualitativeCheckResult<storm::models::GetDdType<ModelType::Representation>::ddType>();
        return std::make_unique<SymbolicQuantitativeCheckResult<storm::models::GetDdType<ModelType::Representation>::ddType, ValueType>>(qualRes);
    }
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                             CheckTask<storm::logic::Formula, ValueType> const& checkTask) {
    storm::logic::Formula const& rewardFormula = checkTask.getFormula();
    if (rewardFormula.isCumulativeRewardFormula()) {
        return this->computeCumulativeRewards(env, rewardMeasureType, checkTask.substituteFormula(rewardFormula.asCumulativeRewardFormula()));
    } else if (rewardFormula.isInstantaneousRewardFormula()) {
        return this->computeInstantaneousRewards(env, rewardMeasureType, checkTask.substituteFormula(rewardFormula.asInstantaneousRewardFormula()));
    } else if (rewardFormula.isReachabilityRewardFormula()) {
        return this->computeReachabilityRewards(env, rewardMeasureType, checkTask.substituteFormula(rewardFormula.asReachabilityRewardFormula()));
    } else if (rewardFormula.isTotalRewardFormula()) {
        return this->computeTotalRewards(env, rewardMeasureType, checkTask.substituteFormula(rewardFormula.asTotalRewardFormula()));
    } else if (rewardFormula.isLongRunAverageRewardFormula()) {
        return this->computeLongRunAverageRewards(env, rewardMeasureType, checkTask.substituteFormula(rewardFormula.asLongRunAverageRewardFormula()));
    } else if (rewardFormula.isConditionalRewardFormula()) {
        return this->computeConditionalRewards(env, rewardMeasureType, checkTask.substituteFormula(rewardFormula.asConditionalFormula()));
    }
    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << rewardFormula << "' is invalid.");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeConditionalRewards(
    Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::ConditionalFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeCumulativeRewards(
    Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeInstantaneousRewards(
    Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeReachabilityRewards(
    Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeTotalRewards(Environment const& env, storm::logic::RewardMeasureType,
                                                                                  CheckTask<storm::logic::TotalRewardFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeLongRunAverageRewards(
    Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeLongRunAverageProbabilities(
    Environment const& env, CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeTimes(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                           CheckTask<storm::logic::Formula, ValueType> const& checkTask) {
    storm::logic::Formula const& timeFormula = checkTask.getFormula();
    if (timeFormula.isReachabilityTimeFormula()) {
        return this->computeReachabilityTimes(env, rewardMeasureType, checkTask.substituteFormula(timeFormula.asReachabilityTimeFormula()));
    }
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::computeReachabilityTimes(Environment const& env, storm::logic::RewardMeasureType,
                                                                                       CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkStateFormula(Environment const& env,
                                                                                CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) {
    storm::logic::StateFormula const& stateFormula = checkTask.getFormula();
    if (stateFormula.isBinaryBooleanStateFormula()) {
        return this->checkBinaryBooleanStateFormula(env, checkTask.substituteFormula(stateFormula.asBinaryBooleanStateFormula()));
    } else if (stateFormula.isUnaryBooleanStateFormula()) {
        return this->checkUnaryBooleanStateFormula(env, checkTask.substituteFormula(stateFormula.asUnaryBooleanStateFormula()));
    } else if (stateFormula.isBooleanLiteralFormula()) {
        return this->checkBooleanLiteralFormula(env, checkTask.substituteFormula(stateFormula.asBooleanLiteralFormula()));
    } else if (stateFormula.isProbabilityOperatorFormula()) {
        return this->checkProbabilityOperatorFormula(env, checkTask.substituteFormula(stateFormula.asProbabilityOperatorFormula()));
    } else if (stateFormula.isRewardOperatorFormula()) {
        return this->checkRewardOperatorFormula(env, checkTask.substituteFormula(stateFormula.asRewardOperatorFormula()));
    } else if (stateFormula.isTimeOperatorFormula()) {
        return this->checkTimeOperatorFormula(env, checkTask.substituteFormula(stateFormula.asTimeOperatorFormula()));
    } else if (stateFormula.isLongRunAverageOperatorFormula()) {
        return this->checkLongRunAverageOperatorFormula(env, checkTask.substituteFormula(stateFormula.asLongRunAverageOperatorFormula()));
    } else if (stateFormula.isAtomicExpressionFormula()) {
        return this->checkAtomicExpressionFormula(env, checkTask.substituteFormula(stateFormula.asAtomicExpressionFormula()));
    } else if (stateFormula.isAtomicLabelFormula()) {
        return this->checkAtomicLabelFormula(env, checkTask.substituteFormula(stateFormula.asAtomicLabelFormula()));
    } else if (stateFormula.isBooleanLiteralFormula()) {
        return this->checkBooleanLiteralFormula(env, checkTask.substituteFormula(stateFormula.asBooleanLiteralFormula()));
    } else if (stateFormula.isGameFormula()) {
        return this->checkGameFormula(env, checkTask.substituteFormula(stateFormula.asGameFormula()));
    } else if (stateFormula.isMultiObjectiveFormula()) {
        if (env.modelchecker().multi().isLexicographicModelCheckingSet()) {
            return this->checkLexObjectiveFormula(env, checkTask.substituteFormula(stateFormula.asMultiObjectiveFormula()));
        } else {
            return this->checkMultiObjectiveFormula(env, checkTask.substituteFormula(stateFormula.asMultiObjectiveFormula()));
        }
    } else if (stateFormula.isQuantileFormula()) {
        return this->checkQuantileFormula(env, checkTask.substituteFormula(stateFormula.asQuantileFormula()));
    }
    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << stateFormula << "' is invalid.");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkAtomicExpressionFormula(
    Environment const& env, CheckTask<storm::logic::AtomicExpressionFormula, ValueType> const& checkTask) {
    storm::logic::AtomicExpressionFormula const& stateFormula = checkTask.getFormula();
    std::stringstream stream;
    stream << stateFormula.getExpression();
    return this->checkAtomicLabelFormula(env, checkTask.substituteFormula(storm::logic::AtomicLabelFormula(stream.str())));
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkAtomicLabelFormula(Environment const& env,
                                                                                      CheckTask<storm::logic::AtomicLabelFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkBinaryBooleanStateFormula(
    Environment const& env, CheckTask<storm::logic::BinaryBooleanStateFormula, ValueType> const& checkTask) {
    storm::logic::BinaryBooleanStateFormula const& stateFormula = checkTask.getFormula();
    STORM_LOG_THROW(stateFormula.getLeftSubformula().isStateFormula() && stateFormula.getRightSubformula().isStateFormula(),
                    storm::exceptions::InvalidArgumentException, "The given formula is invalid.");

    std::unique_ptr<CheckResult> leftResult =
        this->check(env, checkTask.template substituteFormula<storm::logic::Formula>(stateFormula.getLeftSubformula().asStateFormula()));
    std::unique_ptr<CheckResult> rightResult =
        this->check(env, checkTask.template substituteFormula<storm::logic::Formula>(stateFormula.getRightSubformula().asStateFormula()));

    STORM_LOG_THROW(leftResult->isQualitative() && rightResult->isQualitative(), storm::exceptions::InternalTypeErrorException,
                    "Expected qualitative results.");

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
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkBooleanLiteralFormula(
    Environment const& env, CheckTask<storm::logic::BooleanLiteralFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkProbabilityOperatorFormula(
    Environment const& env, CheckTask<storm::logic::ProbabilityOperatorFormula, ValueType> const& checkTask) {
    storm::logic::ProbabilityOperatorFormula const& stateFormula = checkTask.getFormula();
    std::unique_ptr<CheckResult> result = this->computeProbabilities(env, checkTask.substituteFormula(stateFormula.getSubformula()));

    if (checkTask.isBoundSet()) {
        STORM_LOG_THROW(result->isQuantitative(), storm::exceptions::InvalidOperationException,
                        "Unable to perform comparison operation on non-quantitative result.");
        return result->asQuantitativeCheckResult<ValueType>().compareAgainstBound(checkTask.getBoundComparisonType(), checkTask.getBoundThreshold());
    } else {
        return result;
    }
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkRewardOperatorFormula(
    Environment const& env, CheckTask<storm::logic::RewardOperatorFormula, ValueType> const& checkTask) {
    storm::logic::RewardOperatorFormula const& stateFormula = checkTask.getFormula();
    std::unique_ptr<CheckResult> result = this->computeRewards(env, stateFormula.getMeasureType(), checkTask.substituteFormula(stateFormula.getSubformula()));

    if (checkTask.isBoundSet()) {
        STORM_LOG_THROW(result->isQuantitative(), storm::exceptions::InvalidOperationException,
                        "Unable to perform comparison operation on non-quantitative result.");
        return result->asQuantitativeCheckResult<ValueType>().compareAgainstBound(checkTask.getBoundComparisonType(), checkTask.getBoundThreshold());
    } else {
        return result;
    }
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkTimeOperatorFormula(
    Environment const& env, CheckTask<storm::logic::TimeOperatorFormula, ValueType> const& checkTask) {
    storm::logic::TimeOperatorFormula const& stateFormula = checkTask.getFormula();
    STORM_LOG_THROW(stateFormula.getSubformula().isReachabilityTimeFormula(), storm::exceptions::InvalidArgumentException, "The given formula is invalid.");

    std::unique_ptr<CheckResult> result = this->computeTimes(env, stateFormula.getMeasureType(), checkTask.substituteFormula(stateFormula.getSubformula()));

    if (checkTask.isBoundSet()) {
        STORM_LOG_THROW(result->isQuantitative(), storm::exceptions::InvalidOperationException,
                        "Unable to perform comparison operation on non-quantitative result.");
        return result->asQuantitativeCheckResult<ValueType>().compareAgainstBound(checkTask.getBoundComparisonType(), checkTask.getBoundThreshold());
    } else {
        return result;
    }
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkLongRunAverageOperatorFormula(
    Environment const& env, CheckTask<storm::logic::LongRunAverageOperatorFormula, ValueType> const& checkTask) {
    storm::logic::LongRunAverageOperatorFormula const& stateFormula = checkTask.getFormula();
    STORM_LOG_THROW(stateFormula.getSubformula().isStateFormula(), storm::exceptions::InvalidArgumentException, "The given formula is invalid.");

    std::unique_ptr<CheckResult> result =
        this->computeLongRunAverageProbabilities(env, checkTask.substituteFormula(stateFormula.getSubformula().asStateFormula()));

    if (checkTask.isBoundSet()) {
        STORM_LOG_THROW(result->isQuantitative(), storm::exceptions::InvalidOperationException,
                        "Unable to perform comparison operation on non-quantitative result.");
        return result->asQuantitativeCheckResult<ValueType>().compareAgainstBound(checkTask.getBoundComparisonType(), checkTask.getBoundThreshold());
    } else {
        return result;
    }
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkUnaryBooleanStateFormula(
    Environment const& env, CheckTask<storm::logic::UnaryBooleanStateFormula, ValueType> const& checkTask) {
    storm::logic::UnaryBooleanStateFormula const& stateFormula = checkTask.getFormula();
    std::unique_ptr<CheckResult> subResult = this->check(env, checkTask.template substituteFormula<storm::logic::Formula>(stateFormula.getSubformula()));
    STORM_LOG_THROW(subResult->isQualitative(), storm::exceptions::InternalTypeErrorException, "Expected qualitative result.");
    if (stateFormula.isNot()) {
        subResult->asQualitativeCheckResult().complement();
    } else {
        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given formula '" << stateFormula << "' is invalid.");
    }
    return subResult;
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkLexObjectiveFormula(
    Environment const& env, CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkMultiObjectiveFormula(
    Environment const& env, CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkQuantileFormula(Environment const& env,
                                                                                   CheckTask<storm::logic::QuantileFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

template<typename ModelType>
std::unique_ptr<CheckResult> AbstractModelChecker<ModelType>::checkGameFormula(Environment const& env,
                                                                               CheckTask<storm::logic::GameFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                    "This model checker (" << getClassName() << ") does not support the formula: " << checkTask.getFormula() << ".");
}

///////////////////////////////////////////////
// Explicitly instantiate the template class.
///////////////////////////////////////////////
// Sparse
template class AbstractModelChecker<storm::models::sparse::Model<double>>;
template class AbstractModelChecker<storm::models::sparse::Dtmc<double>>;
template class AbstractModelChecker<storm::models::sparse::Ctmc<double>>;
template class AbstractModelChecker<storm::models::sparse::Mdp<double>>;
template class AbstractModelChecker<storm::models::sparse::Pomdp<double>>;
template class AbstractModelChecker<storm::models::sparse::MarkovAutomaton<double>>;
template class AbstractModelChecker<storm::models::sparse::Smg<double>>;

#ifdef STORM_HAVE_CARL
template class AbstractModelChecker<storm::models::sparse::Mdp<double, storm::models::sparse::StandardRewardModel<storm::Interval>>>;
template class AbstractModelChecker<storm::models::sparse::Smg<double, storm::models::sparse::StandardRewardModel<storm::Interval>>>;

template class AbstractModelChecker<storm::models::sparse::Model<storm::RationalNumber>>;
template class AbstractModelChecker<storm::models::sparse::Dtmc<storm::RationalNumber>>;
template class AbstractModelChecker<storm::models::sparse::Ctmc<storm::RationalNumber>>;
template class AbstractModelChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
template class AbstractModelChecker<storm::models::sparse::Pomdp<storm::RationalNumber>>;
template class AbstractModelChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
template class AbstractModelChecker<storm::models::sparse::Smg<storm::RationalNumber>>;

template class AbstractModelChecker<storm::models::sparse::Model<storm::RationalFunction>>;
template class AbstractModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>>;
template class AbstractModelChecker<storm::models::sparse::Ctmc<storm::RationalFunction>>;
template class AbstractModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>>;
template class AbstractModelChecker<storm::models::sparse::MarkovAutomaton<storm::RationalFunction>>;
template class AbstractModelChecker<storm::models::sparse::Smg<storm::RationalFunction>>;
#endif
// DD
template class AbstractModelChecker<storm::models::symbolic::Model<storm::dd::DdType::CUDD, double>>;
template class AbstractModelChecker<storm::models::symbolic::Model<storm::dd::DdType::Sylvan, double>>;
template class AbstractModelChecker<storm::models::symbolic::Model<storm::dd::DdType::Sylvan, storm::RationalNumber>>;
template class AbstractModelChecker<storm::models::symbolic::Model<storm::dd::DdType::Sylvan, storm::RationalFunction>>;
template class AbstractModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double>>;
template class AbstractModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double>>;
template class AbstractModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, storm::RationalNumber>>;
template class AbstractModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, storm::RationalFunction>>;
template class AbstractModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD, double>>;
template class AbstractModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, double>>;
template class AbstractModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, storm::RationalNumber>>;
template class AbstractModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, storm::RationalFunction>>;
template class AbstractModelChecker<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD, double>>;
template class AbstractModelChecker<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan, double>>;
template class AbstractModelChecker<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan, storm::RationalNumber>>;
template class AbstractModelChecker<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan, storm::RationalFunction>>;
template class AbstractModelChecker<storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::CUDD, double>>;
template class AbstractModelChecker<storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::Sylvan, double>>;
template class AbstractModelChecker<storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::Sylvan, storm::RationalNumber>>;
template class AbstractModelChecker<storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::Sylvan, storm::RationalFunction>>;
template class AbstractModelChecker<storm::models::symbolic::StochasticTwoPlayerGame<storm::dd::DdType::CUDD, double>>;
template class AbstractModelChecker<storm::models::symbolic::StochasticTwoPlayerGame<storm::dd::DdType::Sylvan, double>>;
template class AbstractModelChecker<storm::models::symbolic::StochasticTwoPlayerGame<storm::dd::DdType::Sylvan, storm::RationalNumber>>;
template class AbstractModelChecker<storm::models::symbolic::StochasticTwoPlayerGame<storm::dd::DdType::Sylvan, storm::RationalFunction>>;
}  // namespace modelchecker
}  // namespace storm
