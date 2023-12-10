#ifndef STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_
#define STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_

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
    using SolutionType = typename std::conditional<std::is_same_v<ValueType, storm::Interval>, double, ValueType>::type;

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
    virtual bool canHandle(CheckTask<storm::logic::Formula, SolutionType> const& checkTask) const = 0;

    /*!
     * Checks the provided formula.
     *
     * @param checkTask The verification task to pursue.
     * @return The verification result.
     */
    virtual std::unique_ptr<CheckResult> check(Environment const& env, CheckTask<storm::logic::Formula, SolutionType> const& checkTask);

    /*!
     * Checks the provided formula with the default environment.
     * TODO This function is obsolete as soon as the Environment is fully integrated.
     */
    std::unique_ptr<CheckResult> check(CheckTask<storm::logic::Formula, SolutionType> const& checkTask);

    // The methods to compute probabilities for path formulas.
    virtual std::unique_ptr<CheckResult> computeProbabilities(Environment const& env, CheckTask<storm::logic::Formula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeConditionalProbabilities(Environment const& env,
                                                                         CheckTask<storm::logic::ConditionalFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(Environment const& env,
                                                                          CheckTask<storm::logic::BoundedUntilFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeReachabilityProbabilities(Environment const& env,
                                                                          CheckTask<storm::logic::EventuallyFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeGloballyProbabilities(Environment const& env,
                                                                      CheckTask<storm::logic::GloballyFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeNextProbabilities(Environment const& env, CheckTask<storm::logic::NextFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeUntilProbabilities(Environment const& env,
                                                                   CheckTask<storm::logic::UntilFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeHOAPathProbabilities(Environment const& env,
                                                                     CheckTask<storm::logic::HOAPathFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeLTLProbabilities(Environment const& env, CheckTask<storm::logic::PathFormula, SolutionType> const& checkTask);
    std::unique_ptr<CheckResult> computeStateFormulaProbabilities(Environment const& env, CheckTask<storm::logic::Formula, SolutionType> const& checkTask);

    // The methods to compute the rewards for path formulas.
    virtual std::unique_ptr<CheckResult> computeRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                        CheckTask<storm::logic::Formula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeConditionalRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                   CheckTask<storm::logic::ConditionalFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeCumulativeRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                  CheckTask<storm::logic::CumulativeRewardFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                     CheckTask<storm::logic::InstantaneousRewardFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeReachabilityRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                    CheckTask<storm::logic::EventuallyFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeTotalRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                             CheckTask<storm::logic::TotalRewardFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeLongRunAverageRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                      CheckTask<storm::logic::LongRunAverageRewardFormula, SolutionType> const& checkTask);

    // The methods to compute the long-run average probabilities and timing measures.
    virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(Environment const& env,
                                                                            CheckTask<storm::logic::StateFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeTimes(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                      CheckTask<storm::logic::Formula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> computeReachabilityTimes(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                  CheckTask<storm::logic::EventuallyFormula, SolutionType> const& checkTask);

    // The methods to check state formulas.
    virtual std::unique_ptr<CheckResult> checkStateFormula(Environment const& env, CheckTask<storm::logic::StateFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkAtomicExpressionFormula(Environment const& env,
                                                                      CheckTask<storm::logic::AtomicExpressionFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkAtomicLabelFormula(Environment const& env,
                                                                 CheckTask<storm::logic::AtomicLabelFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkBinaryBooleanStateFormula(Environment const& env,
                                                                        CheckTask<storm::logic::BinaryBooleanStateFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkBooleanLiteralFormula(Environment const& env,
                                                                    CheckTask<storm::logic::BooleanLiteralFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkProbabilityOperatorFormula(Environment const& env,
                                                                         CheckTask<storm::logic::ProbabilityOperatorFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkRewardOperatorFormula(Environment const& env,
                                                                    CheckTask<storm::logic::RewardOperatorFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkTimeOperatorFormula(Environment const& env,
                                                                  CheckTask<storm::logic::TimeOperatorFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkLongRunAverageOperatorFormula(
        Environment const& env, CheckTask<storm::logic::LongRunAverageOperatorFormula, SolutionType> const& checkTask);
    virtual std::unique_ptr<CheckResult> checkUnaryBooleanStateFormula(Environment const& env,
                                                                       CheckTask<storm::logic::UnaryBooleanStateFormula, SolutionType> const& checkTask);

    // The methods to check multi-objective formulas.
    virtual std::unique_ptr<CheckResult> checkMultiObjectiveFormula(Environment const& env,
                                                                    CheckTask<storm::logic::MultiObjectiveFormula, SolutionType> const& checkTask);

    // The methods to check quantile formulas.
    virtual std::unique_ptr<CheckResult> checkQuantileFormula(Environment const& env, CheckTask<storm::logic::QuantileFormula, SolutionType> const& checkTask);

    // The methods to check lexicographic LTL formulae
    virtual std::unique_ptr<CheckResult> checkLexObjectiveFormula(Environment const& env,
                                                                  CheckTask<storm::logic::MultiObjectiveFormula, SolutionType> const& checkTask);

    // The methods to check game formulas.
    virtual std::unique_ptr<CheckResult> checkGameFormula(Environment const& env, CheckTask<storm::logic::GameFormula, SolutionType> const& checkTask);
};
}  // namespace modelchecker
}  // namespace storm

#endif /* STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_ */
