#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSOR_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSOR_H_

#include <memory>

#include "src/logic/Formulas.h"
#include "src/modelchecker/multiobjective/helper/SparseMultiObjectivePreprocessorData.h"
#include "src/storage/BitVector.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            /*
             * Helper Class to invoke the necessary preprocessing for multi objective model checking
             */
            template <class SparseModelType>
            class SparseMultiObjectivePreprocessor {
            public:
                typedef typename SparseModelType::ValueType ValueType;
                typedef typename SparseModelType::RewardModelType RewardModelType;
                typedef SparseMultiObjectivePreprocessorData<SparseModelType> PreprocessorData;
                typedef SparseMultiObjectiveObjectiveInformation<ValueType> ObjectiveInformation;
                
                /*!
                 * Preprocesses the given model w.r.t. the given formulas.
                 * @param originalModel The considered model
                 * @param originalFormula the considered formula. The subformulas should only contain one OperatorFormula at top level, i.e., the formula is simple.
                 */
                static PreprocessorData preprocess(storm::logic::MultiObjectiveFormula const& originalFormula, SparseModelType const& originalModel);
                
            private:
                
                /*!
                 * Apply the neccessary preprocessing for the given formula.
                 * @param formula the current (sub)formula
                 * @param data the information collected so far
                 * @param isProb0Formula true iff the considered objective is of the form P<=0 [..]
                 * @param isProb1Formula true iff the considered objective is of the form P>=1 [..]
                 * @param currentObjective the currently considered objective. The given formula should be a a (sub)formula of this objective
                 * @param optionalRewardModelName the reward model name that is considered for the formula (if available)
                 */
                static void preprocessFormula(storm::logic::OperatorFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective);
                static void preprocessFormula(storm::logic::ProbabilityOperatorFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective);
                static void preprocessFormula(storm::logic::RewardOperatorFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective);
                static void preprocessFormula(storm::logic::UntilFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective, bool isProb0Formula, bool isProb1Formula);
                static void preprocessFormula(storm::logic::BoundedUntilFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective);
                static void preprocessFormula(storm::logic::GloballyFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective, bool isProb0Formula, bool isProb1Formula);
                static void preprocessFormula(storm::logic::EventuallyFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective, bool isProb0Formula, bool isProb1Formula, boost::optional<std::string> const& optionalRewardModelName = boost::none);
                static void preprocessFormula(storm::logic::CumulativeRewardFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName = boost::none);
                static void preprocessFormula(storm::logic::TotalRewardFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName = boost::none);
                
                /*!
                 * Checks whether the occurring reward properties are guaranteed to be finite for all states.
                 * if not, further preprocessing is applied.
                 *
                 * For reward properties that maximize, it is checked whether the reward is unbounded for some scheduler
                 * (i.e., an EC with (only positive) rewards is reachable).
                 * If yes, the objective is removed as we can combine any scheduler with the scheduler above
                 * to obtain arbitrary high values. Note that in case of achievability or numerical queries, this combination might
                 * (theoretically) violate thresholds by some small epsilon. This is ignored as we are working with numerical methods anyway...
                 *
                 * For reward properties that minimize, all states from which only infinite reward is possible are removed.
                 * Note that this excludes solutions of numerical queries where the minimum is infinity...
                 */
                static void assertRewardFiniteness(PreprocessorData& data);
                
                /*!
                 * Checks reward finiteness for the negative rewards and returns the set of actions in the
                 * preprocessedModel that give negative rewards for some objective
                 */
                static storm::storage::BitVector assertNegativeRewardFiniteness(PreprocessorData& data);
                
                /*!
                 * Checks reward finiteness for the positive rewards
                 */
                static void assertPositiveRewardFiniteness(PreprocessorData& data, storm::storage::BitVector const& actionsWithNegativeReward);
                
                /*!
                 * Updates the preprocessed model stored in the given data to the given model.
                 * The given newToOldStateIndexMapping should give for each state in the newPreprocessedModel
                 * the index of the state in the current data.preprocessedModel.
                 */
                static void updatePreprocessedModel(PreprocessorData& data, SparseModelType& newPreprocessedModel, std::vector<uint_fast64_t>& newToOldStateIndexMapping);
                
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSOR_H_ */
