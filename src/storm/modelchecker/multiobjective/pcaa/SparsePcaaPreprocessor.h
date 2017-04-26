#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAPREPROCESSOR_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAPREPROCESSOR_H_

#include <memory>

#include "storm/logic/Formulas.h"
#include "storm/storage/BitVector.h"
#include "storm/modelchecker/multiobjective/pcaa/SparsePcaaPreprocessorReturnType.h"
#include "storm/storage/memorystructure/MemoryStructure.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            /*
             * This class invokes the necessary preprocessing for the Pareto Curve Approximation Algorithm (PCAA)
             */
            template <class SparseModelType>
            class SparsePcaaPreprocessor {
            public:
                typedef typename SparseModelType::ValueType ValueType;
                typedef typename SparseModelType::RewardModelType RewardModelType;
                typedef SparsePcaaPreprocessorReturnType<SparseModelType> ReturnType;
                
                /*!
                 * Preprocesses the given model w.r.t. the given formulas.
                 * @param originalModel The considered model
                 * @param originalFormula the considered formula. The subformulas should only contain one OperatorFormula at top level, i.e., the formula is simple.
                 */
                static ReturnType preprocess(SparseModelType const& originalModel, storm::logic::MultiObjectiveFormula const& originalFormula);
                
            private:
                
                /*!
                 * Updates the preprocessed model stored in the given result to the given model.
                 * The given newToOldStateIndexMapping should give for each state in the newPreprocessedModel
                 * the index of the state in the current result.preprocessedModel.
                 */
                static void updatePreprocessedModel(ReturnType& result, SparseModelType& newPreprocessedModel, std::vector<uint_fast64_t>& newToOldStateIndexMapping);
                
                /*!
                 * Updates the preprocessed model stored in the given result to the product of the model and the given memory structure.
                 */
                static void addMemoryToPreprocessedModel(ReturnType& result, storm::storage::MemoryStructure& memory);
                
                /*!
                 * Apply the neccessary preprocessing for the given formula.
                 * @param formula the current (sub)formula
                 * @param result the information collected so far
                 * @param currentObjective the currently considered objective. The given formula should be a a (sub)formula of this objective
                 * @param optionalRewardModelName the reward model name that is considered for the formula (if available)
                 */
                static void preprocessOperatorFormula(storm::logic::OperatorFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective);
                static void preprocessProbabilityOperatorFormula(storm::logic::ProbabilityOperatorFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective);
                static void preprocessRewardOperatorFormula(storm::logic::RewardOperatorFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective);
                static void preprocessTimeOperatorFormula(storm::logic::TimeOperatorFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective);
                static void preprocessUntilFormula(storm::logic::UntilFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective);
                static void preprocessBoundedUntilFormula(storm::logic::BoundedUntilFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective);
                static void preprocessGloballyFormula(storm::logic::GloballyFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective);
                static void preprocessEventuallyFormula(storm::logic::EventuallyFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective, boost::optional<std::string> const& optionalRewardModelName = boost::none);
                static void preprocessCumulativeRewardFormula(storm::logic::CumulativeRewardFormula const& formula, ReturnType& result, PcaaObjective<ValueType>& currentObjective, boost::optional<std::string> const& optionalRewardModelName = boost::none);
                static void preprocessTotalRewardFormula(ReturnType& result, PcaaObjective<ValueType>& currentObjective, boost::optional<std::string> const& optionalRewardModelName = boost::none); // The total reward formula itself does not need to be provided as it is unique.
                
                /*!
                 * Analyzes the end components of the preprocessed model. That is:
                 * -get the set of actions that are part of an end component
                 * -Find the states that can be visited infinitely often without inducing infinite reward
                 */
                static void analyzeEndComponents(ReturnType& result, storm::storage::SparseMatrix<ValueType> const& backwardTransitions);
                
                /*!
                 * Checks whether the occurring expected rewards are finite. If not, the input is rejected.
                 * Also removes all states for which no finite reward wrt. all objectives is possible
                 */
                static void ensureRewardFiniteness(ReturnType& result, storm::storage::SparseMatrix<ValueType> const& backwardTransitions);
                
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAPREPROCESSOR_H_ */
