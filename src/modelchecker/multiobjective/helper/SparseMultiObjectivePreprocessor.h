#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSOR_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSOR_H_

#include "src/logic/Formulas.h"
#include "src/modelchecker/multiobjective/helper/SparseMultiObjectivePreprocessorData.h"

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
                 * @param currentObjective the currently considered objective. The given formula should be a a (sub)formula of this objective
                 * @param optionalRewardModelName the reward model name that is considered for the formula (if available)
                 */
                static void preprocessFormula(storm::logic::OperatorFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective);
                static void preprocessFormula(storm::logic::ProbabilityOperatorFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective);
                static void preprocessFormula(storm::logic::RewardOperatorFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective);
                static void preprocessFormula(storm::logic::UntilFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective);
                static void preprocessFormula(storm::logic::BoundedUntilFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective);
                static void preprocessFormula(storm::logic::GloballyFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective);
                static void preprocessFormula(storm::logic::EventuallyFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName = boost::none);
                static void preprocessFormula(storm::logic::CumulativeRewardFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName = boost::none);
                static void preprocessFormula(storm::logic::TotalRewardFormula const& formula, PreprocessorData& data, ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName = boost::none);
                
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEPREPROCESSOR_H_ */
