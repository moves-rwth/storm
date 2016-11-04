#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAPREPROCESSOR_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAPREPROCESSOR_H_

#include <memory>

#include "src/logic/Formulas.h"
#include "src/storage/BitVector.h"
#include "src/modelchecker/multiobjective/pcaa/SparsePCAAPreprocessorReturnType.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            /*
             * This class invokes the necessary preprocessing for the Pareto Curve Approximation Algorithm (PCAA)
             */
            template <class SparseModelType>
            class SparsePCAAPreprocessor {
            public:
                typedef typename SparseModelType::ValueType ValueType;
                typedef typename SparseModelType::RewardModelType RewardModelType;
                typedef SparsePCAAPreprocessorReturnType<SparseModelType> ReturnType;
                
                /*!
                 * Preprocesses the given model w.r.t. the given formulas.
                 * @param originalModel The considered model
                 * @param originalFormula the considered formula. The subformulas should only contain one OperatorFormula at top level, i.e., the formula is simple.
                 */
                static ReturnType preprocess(storm::logic::MultiObjectiveFormula const& originalFormula, SparseModelType const& originalModel);
                
            private:
                /*!
                 * Initializes the returned Information
                 * @param originalModel The considered model
                 * @param originalFormula the considered formula
                 */
                static ReturnType initializeResult(storm::logic::MultiObjectiveFormula const& originalFormula, SparseModelType const& originalModel);
                
                /*!
                 * Updates the preprocessed model stored in the given result to the given model.
                 * The given newToOldStateIndexMapping should give for each state in the newPreprocessedModel
                 * the index of the state in the current result.preprocessedModel.
                 */
                static void updatePreprocessedModel(ReturnType& result, SparseModelType& newPreprocessedModel, std::vector<uint_fast64_t>& newToOldStateIndexMapping);
                
                /*!
                 * Apply the neccessary preprocessing for the given formula.
                 * @param formula the current (sub)formula
                 * @param result the information collected so far
                 * @param currentObjective the currently considered objective. The given formula should be a a (sub)formula of this objective
                 * @param optionalRewardModelName the reward model name that is considered for the formula (if available)
                 */
                static void preprocessFormula(storm::logic::OperatorFormula const& formula, ReturnType& result, PCAAObjective<ValueType>& currentObjective);
                static void preprocessFormula(storm::logic::ProbabilityOperatorFormula const& formula, ReturnType& result, PCAAObjective<ValueType>& currentObjective);
                static void preprocessFormula(storm::logic::RewardOperatorFormula const& formula, ReturnType& result, PCAAObjective<ValueType>& currentObjective);
                static void preprocessFormula(storm::logic::TimeOperatorFormula const& formula, ReturnType& result, PCAAObjective<ValueType>& currentObjective);
                static void preprocessFormula(storm::logic::UntilFormula const& formula, ReturnType& result, PCAAObjective<ValueType>& currentObjective);
                static void preprocessFormula(storm::logic::BoundedUntilFormula const& formula, ReturnType& result, PCAAObjective<ValueType>& currentObjective);
                static void preprocessFormula(storm::logic::GloballyFormula const& formula, ReturnType& result, PCAAObjective<ValueType>& currentObjective);
                static void preprocessFormula(storm::logic::EventuallyFormula const& formula, ReturnType& result, PCAAObjective<ValueType>& currentObjective, boost::optional<std::string> const& optionalRewardModelName = boost::none);
                static void preprocessFormula(storm::logic::CumulativeRewardFormula const& formula, ReturnType& result, PCAAObjective<ValueType>& currentObjective, boost::optional<std::string> const& optionalRewardModelName = boost::none);
                static void preprocessFormula(storm::logic::TotalRewardFormula const& formula, ReturnType& result, PCAAObjective<ValueType>& currentObjective, boost::optional<std::string> const& optionalRewardModelName = boost::none);
                
                /*!
                 * Checks whether the occurring reward properties are guaranteed to be finite for all states.
                 * if not, the input is rejected.
                 * Also applies further preprocessing steps regarding End Component Elimination
                 */
                static void ensureRewardFiniteness(ReturnType& result);
                
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAPREPROCESSOR_H_ */
