#pragma once

#include <memory>
#include <string>

#include "storm/logic/Formulas.h"
#include "storm/modelchecker/multiobjective/preprocessing/SparseMultiObjectivePreprocessorResult.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/memorystructure/MemoryStructure.h"

namespace storm {

class Environment;

namespace modelchecker {
namespace multiobjective {
namespace preprocessing {

/*
 * This class invokes the necessary preprocessing for the constraint based multi-objective model checking algorithm
 */
template<class SparseModelType>
class SparseMultiObjectivePreprocessor {
   public:
    typedef typename SparseModelType::ValueType ValueType;
    typedef typename SparseModelType::RewardModelType RewardModelType;
    typedef SparseMultiObjectivePreprocessorResult<SparseModelType> ReturnType;

    /*!
     * Preprocesses the given model w.r.t. the given formulas
     * @param originalModel The considered model
     * @param originalFormula the considered formula. The subformulas should only contain one OperatorFormula at top level.
     */
    static ReturnType preprocess(Environment const& env, SparseModelType const& originalModel, storm::logic::MultiObjectiveFormula const& originalFormula);

   private:
    struct PreprocessorData {
        std::shared_ptr<SparseModelType> model;
        std::vector<std::shared_ptr<Objective<ValueType>>> objectives;

        // Indices of the objectives that require a check for finite reward
        storm::storage::BitVector finiteRewardCheckObjectives;

        // Indices of the objectives for which we need to compute an upper bound for the result
        storm::storage::BitVector upperResultBoundObjectives;

        std::string rewardModelNamePrefix;

        // If set, some states have been merged to a deadlock state with this label.
        boost::optional<std::string> deadlockLabel;

        PreprocessorData(std::shared_ptr<SparseModelType> model);
    };

    /*!
     * Removes states that are irrelevant for all objectives, e.g., because they are only reachable via goal states.
     */
    static void removeIrrelevantStates(std::shared_ptr<SparseModelType>& model, boost::optional<std::string>& deadlockLabel,
                                       storm::logic::MultiObjectiveFormula const& originalFormula);

    /*!
     * Apply the neccessary preprocessing for the given formula.
     * @param formula the current (sub)formula
     * @param opInfo the information of the resulting operator formula
     * @param data the current data. The currently processed objective is located at data.objectives.back()
     * @param optionalRewardModelName the reward model name that is considered for the formula (if available)
     */
    static void preprocessOperatorFormula(storm::logic::OperatorFormula const& formula, PreprocessorData& data);
    static void preprocessProbabilityOperatorFormula(storm::logic::ProbabilityOperatorFormula const& formula, storm::logic::OperatorInformation const& opInfo,
                                                     PreprocessorData& data);
    static void preprocessRewardOperatorFormula(storm::logic::RewardOperatorFormula const& formula, storm::logic::OperatorInformation const& opInfo,
                                                PreprocessorData& data);
    static void preprocessTimeOperatorFormula(storm::logic::TimeOperatorFormula const& formula, storm::logic::OperatorInformation const& opInfo,
                                              PreprocessorData& data);
    static void preprocessLongRunAverageOperatorFormula(storm::logic::LongRunAverageOperatorFormula const& formula,
                                                        storm::logic::OperatorInformation const& opInfo, PreprocessorData& data);
    static void preprocessUntilFormula(storm::logic::UntilFormula const& formula, storm::logic::OperatorInformation const& opInfo, PreprocessorData& data,
                                       std::shared_ptr<storm::logic::Formula const> subformula = nullptr);
    static void preprocessBoundedUntilFormula(storm::logic::BoundedUntilFormula const& formula, storm::logic::OperatorInformation const& opInfo,
                                              PreprocessorData& data);
    static void preprocessGloballyFormula(storm::logic::GloballyFormula const& formula, storm::logic::OperatorInformation const& opInfo,
                                          PreprocessorData& data);
    static void preprocessEventuallyFormula(storm::logic::EventuallyFormula const& formula, storm::logic::OperatorInformation const& opInfo,
                                            PreprocessorData& data, boost::optional<std::string> const& optionalRewardModelName = boost::none);
    static void preprocessCumulativeRewardFormula(storm::logic::CumulativeRewardFormula const& formula, storm::logic::OperatorInformation const& opInfo,
                                                  PreprocessorData& data, boost::optional<std::string> const& optionalRewardModelName = boost::none);
    static void preprocessTotalRewardFormula(storm::logic::TotalRewardFormula const& formula, storm::logic::OperatorInformation const& opInfo,
                                             PreprocessorData& data, boost::optional<std::string> const& optionalRewardModelName = boost::none);
    static void preprocessLongRunAverageRewardFormula(storm::logic::LongRunAverageRewardFormula const& formula, storm::logic::OperatorInformation const& opInfo,
                                                      PreprocessorData& data, boost::optional<std::string> const& optionalRewardModelName = boost::none);

    /*!
     * Builds the result from preprocessing
     */
    static ReturnType buildResult(SparseModelType const& originalModel, storm::logic::MultiObjectiveFormula const& originalFormula, PreprocessorData& data);

    /*!
     * Returns the query type
     */
    static typename ReturnType::QueryType getQueryType(std::vector<Objective<ValueType>> const& objectives);
};

}  // namespace preprocessing
}  // namespace multiobjective
}  // namespace modelchecker
}  // namespace storm
