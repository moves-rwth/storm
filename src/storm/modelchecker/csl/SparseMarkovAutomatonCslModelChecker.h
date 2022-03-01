#ifndef STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_
#define STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_

#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"

#include "storm/models/sparse/MarkovAutomaton.h"

#include "storm/solver/MinMaxLinearEquationSolver.h"

namespace storm {

namespace modelchecker {

template<typename SparseMarkovAutomatonModelType>
class SparseMarkovAutomatonCslModelChecker : public SparsePropositionalModelChecker<SparseMarkovAutomatonModelType> {
   public:
    typedef typename SparseMarkovAutomatonModelType::ValueType ValueType;
    typedef typename SparseMarkovAutomatonModelType::RewardModelType RewardModelType;

    explicit SparseMarkovAutomatonCslModelChecker(SparseMarkovAutomatonModelType const& model);

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
    virtual std::unique_ptr<CheckResult> computeLTLProbabilities(Environment const& env,
                                                                 CheckTask<storm::logic::PathFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeHOAPathProbabilities(Environment const& env,
                                                                     CheckTask<storm::logic::HOAPathFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeReachabilityRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                    CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeTotalRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                             CheckTask<storm::logic::TotalRewardFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(Environment const& env,
                                                                            CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeLongRunAverageRewards(
        Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
        CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeReachabilityTimes(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                  CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> checkMultiObjectiveFormula(Environment const& env,
                                                                    CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask) override;
};
}  // namespace modelchecker
}  // namespace storm

#endif /* STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_ */
