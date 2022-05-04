#ifndef STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_
#define STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_

#include <boost/optional.hpp>
#include <string>

#include "storm/logic/Formulas.h"
#include "storm/modelchecker/CheckTask.h"
#include "storm/solver/OptimizationDirection.h"

namespace storm {

class Environment;

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
     * Returns the name of the model checker class (e.g., for display in error messages).
     */
    virtual std::string getClassName() const;

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
    virtual std::unique_ptr<CheckResult> check(Environment const& env, CheckTask<storm::logic::Formula, ValueType> const& checkTask);

    /*!
     * Checks the provided formula with the default environment.
     * TODO This function is obsolete as soon as the Environment is fully integrated.
     */
    std::unique_ptr<CheckResult> check(CheckTask<storm::logic::Formula, ValueType> const& checkTask);

    // The methods to compute probabilities for path formulas.
    virtual std::unique_ptr<CheckResult> computeProbabilities(Environment const& env, CheckTask<storm::logic::Formula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeConditionalProbabilities(Environment const& env,
                                                                         CheckTask<storm::logic::ConditionalFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(Environment const& env,
                                                                          CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeReachabilityProbabilities(Environment const& env,
                                                                          CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeGloballyProbabilities(Environment const& env,
                                                                      CheckTask<storm::logic::GloballyFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeNextProbabilities(Environment const& env, CheckTask<storm::logic::NextFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeUntilProbabilities(Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeHOAPathProbabilities(Environment const& env,
                                                                     CheckTask<storm::logic::HOAPathFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeLTLProbabilities(Environment const& env, CheckTask<storm::logic::PathFormula, ValueType> const& checkTask);
    std::unique_ptr<CheckResult> computeStateFormulaProbabilities(Environment const& env, CheckTask<storm::logic::Formula, ValueType> const& checkTask);

    // The methods to compute the rewards for path formulas.
    virtual std::unique_ptr<CheckResult> computeRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                        CheckTask<storm::logic::Formula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeConditionalRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                   CheckTask<storm::logic::ConditionalFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeCumulativeRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                  CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                     CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeReachabilityRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                    CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeTotalRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                             CheckTask<storm::logic::TotalRewardFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeLongRunAverageRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                      CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask);

    // The methods to compute the long-run average probabilities and timing measures.
    virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(Environment const& env,
                                                                            CheckTask<storm::logic::StateFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeTimes(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                      CheckTask<storm::logic::Formula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeReachabilityTimes(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                  CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask);

    // The methods to check state formulas.
    virtual std::unique_ptr<CheckResult> checkStateFormula(Environment const& env, CheckTask<storm::logic::StateFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkAtomicExpressionFormula(Environment const& env,
                                                                      CheckTask<storm::logic::AtomicExpressionFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkAtomicLabelFormula(Environment const& env,
                                                                 CheckTask<storm::logic::AtomicLabelFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkBinaryBooleanStateFormula(Environment const& env,
                                                                        CheckTask<storm::logic::BinaryBooleanStateFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkBooleanLiteralFormula(Environment const& env,
                                                                    CheckTask<storm::logic::BooleanLiteralFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkProbabilityOperatorFormula(Environment const& env,
                                                                         CheckTask<storm::logic::ProbabilityOperatorFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkRewardOperatorFormula(Environment const& env,
                                                                    CheckTask<storm::logic::RewardOperatorFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkTimeOperatorFormula(Environment const& env,
                                                                  CheckTask<storm::logic::TimeOperatorFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkLongRunAverageOperatorFormula(Environment const& env,
                                                                            CheckTask<storm::logic::LongRunAverageOperatorFormula, ValueType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkUnaryBooleanStateFormula(Environment const& env,
                                                                       CheckTask<storm::logic::UnaryBooleanStateFormula, ValueType> const& checkTask);

    // The methods to check multi-objective formulas.
    virtual std::unique_ptr<CheckResult> checkMultiObjectiveFormula(Environment const& env,
                                                                    CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask);

    // The methods to check quantile formulas.
    virtual std::unique_ptr<CheckResult> checkQuantileFormula(Environment const& env, CheckTask<storm::logic::QuantileFormula, ValueType> const& checkTask);

    // The methods to check lexicographic LTL formulae
    virtual std::unique_ptr<CheckResult> checkLexObjectiveFormula(Environment const& env,
                                                                  CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask);

    // The methods to check game formulas.
    virtual std::unique_ptr<CheckResult> checkGameFormula(Environment const& env, CheckTask<storm::logic::GameFormula, ValueType> const& checkTask);
};
}  // namespace modelchecker
}  // namespace storm

#endif /* STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_ */
