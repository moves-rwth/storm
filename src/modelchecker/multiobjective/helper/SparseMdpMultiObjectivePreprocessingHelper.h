#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMDPMULTIOBJECTIVEPREPROCESSINGHELPER_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMDPMULTIOBJECTIVEPREPROCESSINGHELPER_H_

#include "src/logic/Formulas.h"
#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveModelCheckerInformation.h"
#include "src/storage/BitVector.h"

namespace storm {
    namespace modelchecker {
        
        
        namespace helper {
            
            /*
             * Helper Class to invoke the necessary preprocessing for multi objective model checking
             */
            template <class SparseMdpModelType>
            class SparseMdpMultiObjectivePreprocessingHelper {
            public:
                typedef typename SparseMdpModelType::ValueType ValueType;
                typedef typename SparseMdpModelType::RewardModelType RewardModelType;
                typedef SparseMultiObjectiveModelCheckerInformation<SparseMdpModelType> Information;
                
                /*!
                 * Preprocesses the given model w.r.t. the given formulas.
                 * @param originalModel The considered model
                 * @param originalFormula the considered formula. The subformulas should only contain one OperatorFormula at top level, i.e., the formula is simple.
                 */
                static Information preprocess(storm::logic::MultiObjectiveFormula const& originalFormula, SparseMdpModelType originalModel);
                
            private:
                
                static bool gatherObjectiveInformation(storm::logic::MultiObjectiveFormula const& formula, Information& info);
                static bool setStepBoundOfObjective(typename Information::ObjectiveInformation& currentObjective);
                static bool setWhetherNegativeRewardsAreConsidered(Information& info);
                
                /*!
                 * Apply the neccessary preprocessing for the given formula.
                 * @param formula the current (sub)formula
                 * @param info the current state of the preprocessing.
                 * @return true iff there was no error
                 */
                // State formulas (will transform the formula and the reward model)
                static bool preprocess(storm::logic::ProbabilityOperatorFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective);
                static bool preprocess(storm::logic::RewardOperatorFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective);
                
                // Path formulas (will transform the model)
                static bool preprocess(storm::logic::UntilFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective);
                static bool preprocess(storm::logic::BoundedUntilFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective);
                static bool preprocess(storm::logic::GloballyFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective);
                static bool preprocess(storm::logic::EventuallyFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName = boost::none);
                static bool preprocess(storm::logic::CumulativeRewardFormula const& formula, Information& info, typename Information::ObjectiveInformation& currentObjective, boost::optional<std::string> const& optionalRewardModelName = boost::none);
                
                static storm::storage::BitVector checkPropositionalFormula(storm::logic::Formula propFormula, SparseMdpModelType const& model);
                
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMDPMULTIOBJECTIVEPREPROCESSINGHELPER_H_ */
