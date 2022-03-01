#ifndef STORM_MODELCHECKER_SPARSEDTMCPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_SPARSEDTMCPRCTLMODELCHECKER_H_

#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/models/sparse/Dtmc.h"

namespace storm {
namespace modelchecker {

template<class SparseDtmcModelType>
class SparseDtmcPrctlModelChecker : public SparsePropositionalModelChecker<SparseDtmcModelType> {
   public:
    typedef typename SparseDtmcModelType::ValueType ValueType;
    typedef typename SparseDtmcModelType::RewardModelType RewardModelType;

    explicit SparseDtmcPrctlModelChecker(SparseDtmcModelType const& model);

    /*!
     * Returns false, if this task can certainly not be handled by this model checker (independent of the concrete model).
     * @param requiresSingleInitialState if not nullptr, this flag is set to true iff checking this formula requires a model with a single initial state
     */
    static bool canHandleStatic(CheckTask<storm::logic::Formula, ValueType> const& checkTask, bool* requiresSingleInitialState = nullptr);

    // The implemented methods of the AbstractModelChecker interface.
    virtual bool canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const override;
    virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(Environment const& env,
                                                                          CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeNextProbabilities(Environment const& env,
                                                                  CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeUntilProbabilities(Environment const& env,
                                                                   CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeGloballyProbabilities(Environment const& env,
                                                                      CheckTask<storm::logic::GloballyFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeHOAPathProbabilities(Environment const& env,
                                                                     CheckTask<storm::logic::HOAPathFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeConditionalProbabilities(Environment const& env,
                                                                         CheckTask<storm::logic::ConditionalFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(Environment const& env,
                                                                            CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeLTLProbabilities(Environment const& env,
                                                                 CheckTask<storm::logic::PathFormula, ValueType> const& checkTask) override;

    virtual std::unique_ptr<CheckResult> computeCumulativeRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                  CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeInstantaneousRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                     CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeReachabilityRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                    CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeTotalRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                             CheckTask<storm::logic::TotalRewardFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeConditionalRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                   CheckTask<storm::logic::ConditionalFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeLongRunAverageRewards(
        Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
        CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeReachabilityTimes(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                  CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> checkQuantileFormula(Environment const& env,
                                                              CheckTask<storm::logic::QuantileFormula, ValueType> const& checkTask) override;

    /*!
     * Computes the long run average (or: steady state) distribution over all states
     * Assumes a uniform distribution over initial states.
     */
    std::unique_ptr<CheckResult> computeSteadyStateDistribution(Environment const& env);

    /*!
     * Computes for each state the expected number of times we visit that state.
     * Assumes a uniform distribution over initial states.
     */
    std::unique_ptr<CheckResult> computeExpectedVisitingTimes(Environment const& env);
};

}  // namespace modelchecker
}  // namespace storm

#endif /* STORM_MODELCHECKER_SPARSEDTMCPRCTLMODELCHECKER_H_ */
