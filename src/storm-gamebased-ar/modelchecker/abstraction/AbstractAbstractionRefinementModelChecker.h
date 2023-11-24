#pragma once

#include "storm/modelchecker/AbstractModelChecker.h"
#include "storm/storage/dd/DdType.h"

namespace storm {

class Environment;

namespace dd {
template<storm::dd::DdType DdType>
class Bdd;

template<storm::dd::DdType DdType, typename ValueType>
class Add;
}  // namespace dd

namespace models {
template<typename ValueType>
class Model;

namespace symbolic {
template<storm::dd::DdType DdType, typename ValueType>
class Model;

template<storm::dd::DdType DdType, typename ValueType>
class Dtmc;

template<storm::dd::DdType DdType, typename ValueType>
class Mdp;

template<storm::dd::DdType DdType, typename ValueType>
class StochasticTwoPlayerGame;
}  // namespace symbolic
}  // namespace models

namespace abstraction {
class QualitativeResultMinMax;

template<storm::dd::DdType DdType>
class SymbolicQualitativeResultMinMax;

template<storm::dd::DdType DdType>
class SymbolicQualitativeMdpResultMinMax;

template<storm::dd::DdType DdType>
class SymbolicQualitativeGameResultMinMax;

class StateSet;

template<storm::dd::DdType DdType>
class SymbolicStateSet;
}  // namespace abstraction

namespace modelchecker {
template<typename ModelType>
class SymbolicMdpPrctlModelChecker;

template<typename ValueType>
class QuantitativeCheckResult;

template<storm::dd::DdType DdType, typename ValueType>
class SymbolicQuantitativeCheckResult;

template<typename ModelType>
class AbstractAbstractionRefinementModelChecker : public AbstractModelChecker<ModelType> {
   public:
    typedef typename ModelType::ValueType ValueType;
    static const storm::dd::DdType DdType = ModelType::DdType;

    /*!
     * Constructs a model checker for the given model.
     */
    explicit AbstractAbstractionRefinementModelChecker();

    virtual ~AbstractAbstractionRefinementModelChecker();

    /// Overridden methods from super class.
    virtual bool canHandle(CheckTask<storm::logic::Formula> const& checkTask) const override;
    virtual std::unique_ptr<CheckResult> computeUntilProbabilities(Environment const& env,
                                                                   CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeReachabilityProbabilities(Environment const& env,
                                                                          CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) override;
    virtual std::unique_ptr<CheckResult> computeReachabilityRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType,
                                                                    CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) override;

   protected:
    /// -------- Methods that need to be overwritten/defined by implementations in subclasses.

    /// Determines whether the model checker can handle reachability rewards or only reachability probabilities.
    virtual bool supportsReachabilityRewards() const;

    /// Retrieves the name of the underlying method.
    virtual std::string const& getName() const = 0;

    /// Called before the abstraction refinement loop to give the implementation a time to set up auxiliary data.
    virtual void initializeAbstractionRefinement() = 0;

    /// Retrieves the abstract model.
    virtual std::shared_ptr<storm::models::Model<ValueType>> getAbstractModel() = 0;

    /// Retrieves the state sets corresponding to the constraint and target states.
    virtual std::pair<std::unique_ptr<storm::abstraction::StateSet>, std::unique_ptr<storm::abstraction::StateSet>> getConstraintAndTargetStates(
        storm::models::Model<ValueType> const& abstractModel) = 0;

    /// Retrieves the index of the abstraction player. Arbitrary for DTMCs, in {0, 1} for MDPs (0 = no player,
    /// 1 = the only player) and in {1, 2} for games.
    virtual uint64_t getAbstractionPlayer() const = 0;

    /// Retrieves whether schedulers need to be computed.
    virtual bool requiresSchedulerSynthesis() const = 0;

    /// Refines the abstract model so that the next iteration obtains bounds that are at least as precise as
    /// current ones.
    virtual void refineAbstractModel() = 0;

    /// -------- Methods used to implement the abstraction refinement procedure.

    /// Performs the actual abstraction refinement loop.
    std::unique_ptr<CheckResult> performAbstractionRefinement(Environment const& env);

    /// Computes lower and upper bounds on the abstract model and returns the bounds for the initial states.
    std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> computeBounds(Environment const& env,
                                                                                        storm::models::Model<ValueType> const& abstractModel);

    /// Solves the current check task qualitatively, i.e. computes all states with probability 0/1.
    std::unique_ptr<storm::abstraction::QualitativeResultMinMax> computeQualitativeResult(Environment const& env,
                                                                                          storm::models::Model<ValueType> const& abstractModel,
                                                                                          storm::abstraction::StateSet const& constraintStates,
                                                                                          storm::abstraction::StateSet const& targetStates);
    std::unique_ptr<storm::abstraction::QualitativeResultMinMax> computeQualitativeResult(
        Environment const& env, storm::models::symbolic::Model<DdType, ValueType> const& abstractModel,
        storm::abstraction::SymbolicStateSet<DdType> const& constraintStates, storm::abstraction::SymbolicStateSet<DdType> const& targetStates);
    std::unique_ptr<storm::abstraction::QualitativeResultMinMax> computeQualitativeResult(Environment const& env,
                                                                                          storm::models::symbolic::Dtmc<DdType, ValueType> const& abstractModel,
                                                                                          storm::abstraction::SymbolicStateSet<DdType> const& constraintStates,
                                                                                          storm::abstraction::SymbolicStateSet<DdType> const& targetStates);
    std::unique_ptr<storm::abstraction::QualitativeResultMinMax> computeQualitativeResult(Environment const& env,
                                                                                          storm::models::symbolic::Mdp<DdType, ValueType> const& abstractModel,
                                                                                          storm::abstraction::SymbolicStateSet<DdType> const& constraintStates,
                                                                                          storm::abstraction::SymbolicStateSet<DdType> const& targetStates);
    std::unique_ptr<storm::abstraction::QualitativeResultMinMax> computeQualitativeResult(
        Environment const& env, storm::models::symbolic::StochasticTwoPlayerGame<DdType, ValueType> const& abstractModel,
        storm::abstraction::SymbolicStateSet<DdType> const& constraintStates, storm::abstraction::SymbolicStateSet<DdType> const& targetStates);
    std::unique_ptr<storm::abstraction::SymbolicQualitativeGameResultMinMax<DdType>> computeQualitativeResultReuse(
        storm::models::symbolic::StochasticTwoPlayerGame<DdType, ValueType> const& abstractModel, storm::dd::Bdd<DdType> const& transitionMatrixBdd,
        storm::abstraction::SymbolicStateSet<DdType> const& constraintStates, storm::abstraction::SymbolicStateSet<DdType> const& targetStates,
        uint64_t abstractionPlayer, storm::OptimizationDirection const& modelNondeterminismDirection, bool requiresSchedulers);
    std::unique_ptr<CheckResult> checkForResultAfterQualitativeCheck(storm::models::Model<ValueType> const& abstractModel);
    std::unique_ptr<CheckResult> checkForResultAfterQualitativeCheck(storm::models::symbolic::Model<DdType, ValueType> const& abstractModel);

    // Methods related to the quantitative solution.
    bool skipQuantitativeSolution(storm::models::Model<ValueType> const& abstractModel, storm::abstraction::QualitativeResultMinMax const& qualitativeResults);
    bool skipQuantitativeSolution(storm::models::symbolic::Model<DdType, ValueType> const& abstractModel,
                                  storm::abstraction::SymbolicQualitativeResultMinMax<DdType> const& qualitativeResults);
    std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> computeQuantitativeResult(
        Environment const& env, storm::models::Model<ValueType> const& abstractModel, storm::abstraction::StateSet const& constraintStates,
        storm::abstraction::StateSet const& targetStates, storm::abstraction::QualitativeResultMinMax const& qualitativeResults);
    std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> computeQuantitativeResult(
        Environment const& env, storm::models::symbolic::Model<DdType, ValueType> const& abstractModel,
        storm::abstraction::SymbolicStateSet<DdType> const& constraintStates, storm::abstraction::SymbolicStateSet<DdType> const& targetStates,
        storm::abstraction::SymbolicQualitativeResultMinMax<DdType> const& qualitativeResults);
    std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> computeQuantitativeResult(
        Environment const& env, storm::models::symbolic::Dtmc<DdType, ValueType> const& abstractModel,
        storm::abstraction::SymbolicStateSet<DdType> const& constraintStates, storm::abstraction::SymbolicStateSet<DdType> const& targetStates,
        storm::abstraction::SymbolicQualitativeResultMinMax<DdType> const& qualitativeResults);
    std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> computeQuantitativeResult(
        Environment const& env, storm::models::symbolic::Mdp<DdType, ValueType> const& abstractModel,
        storm::abstraction::SymbolicStateSet<DdType> const& constraintStates, storm::abstraction::SymbolicStateSet<DdType> const& targetStates,
        storm::abstraction::SymbolicQualitativeResultMinMax<DdType> const& qualitativeResults);
    std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> computeQuantitativeResult(
        Environment const& env, storm::models::symbolic::StochasticTwoPlayerGame<DdType, ValueType> const& abstractModel,
        storm::abstraction::SymbolicStateSet<DdType> const& constraintStates, storm::abstraction::SymbolicStateSet<DdType> const& targetStates,
        storm::abstraction::SymbolicQualitativeResultMinMax<DdType> const& qualitativeResults);
    void filterInitialStates(storm::models::Model<ValueType> const& abstractModel,
                             std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>>& bounds);
    void filterInitialStates(storm::models::symbolic::Model<DdType, ValueType> const& abstractModel,
                             std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>>& bounds);
    void printBoundsInformation(std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>>& bounds);
    void printBoundsInformation(SymbolicQuantitativeCheckResult<DdType, ValueType> const& lowerBounds,
                                SymbolicQuantitativeCheckResult<DdType, ValueType> const& upperBounds);
    bool checkForResultAfterQuantitativeCheck(storm::models::Model<ValueType> const& abstractModel, bool lowerBounds,
                                              QuantitativeCheckResult<ValueType> const& result);
    std::unique_ptr<CheckResult> computeReachabilityProbabilitiesHelper(
        Environment const& env, storm::models::symbolic::StochasticTwoPlayerGame<DdType, ValueType> const& abstractModel,
        storm::OptimizationDirection const& player1Direction, storm::OptimizationDirection const& player2Direction, storm::dd::Bdd<DdType> const& maybeStates,
        storm::dd::Bdd<DdType> const& prob1States, storm::dd::Add<DdType, ValueType> const& startValues);

    /// Tries to obtain the results from the bounds. If either of the two bounds is null, the result is assumed
    /// to be the non-null bound. If neither is null and the bounds are sufficiently close, the average of the
    /// bounds is returned.
    std::unique_ptr<CheckResult> tryToObtainResultFromBounds(storm::models::Model<ValueType> const& model,
                                                             std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>>& bounds);
    /// Checks whether the provided bounds are sufficiently close to terminate.
    bool boundsAreSufficientlyClose(std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> const& bounds);
    /// Retrieves the average of the two bounds. This should only be used to derive the overall result when the
    /// bounds are sufficiently close.
    std::unique_ptr<CheckResult> getAverageOfBounds(std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> const& bounds);

    /// Methods to set/get the check task that is currently handled.
    void setCheckTask(CheckTask<storm::logic::Formula, ValueType> const& checkTask);
    CheckTask<storm::logic::Formula, ValueType> const& getCheckTask() const;

    /// Methods that retrieve which results shall be reused.
    bool getReuseQualitativeResults() const;
    bool getReuseQuantitativeResults() const;

   private:
    /// The check task that is currently handled.
    std::unique_ptr<CheckTask<storm::logic::Formula, ValueType> const> checkTask;

    /// A flag indicating whether to reuse the qualitative results.
    bool reuseQualitativeResults;

    /// A flag indicating whether to reuse the quantitative results.
    bool reuseQuantitativeResults;

    /// The last qualitative results.
    std::unique_ptr<storm::abstraction::QualitativeResultMinMax> lastQualitativeResults;

    /// The last full results that were obtained. These results (if there are any) specify the lower and upper
    /// bounds for all states in the model.
    std::pair<std::unique_ptr<CheckResult>, std::unique_ptr<CheckResult>> lastBounds;
};
}  // namespace modelchecker
}  // namespace storm
