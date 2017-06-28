#pragma once


#include "storm-pars/transformer/SparseParametricModelSimplifier.h"

namespace storm {
    namespace transformer {

        /*!
         * This class performs different steps to simplify the given (parametric) model.
         * Checking the obtained simplified formula on the simplified model yields the same result as checking the original formula on the original model (wrt. to the initial states of the two models)
         * End Components of nondeterministic models are removed whenever this is valid for the corresponding formula. This allows us to apply, e.g., value iteration that does not start from the 0,...,0 vector.
         */
        template<typename SparseModelType>
        class SparseParametricMdpSimplifier : public SparseParametricModelSimplifier<SparseModelType> {
        public:
            SparseParametricMdpSimplifier(SparseModelType const& model);
            
        protected:
            
            // Perform the simplification for the corresponding formula type
            virtual bool simplifyForUntilProbabilities(storm::logic::ProbabilityOperatorFormula const& formula) override;
            virtual bool simplifyForBoundedUntilProbabilities(storm::logic::ProbabilityOperatorFormula const& formula) override;
            virtual bool simplifyForReachabilityRewards(storm::logic::RewardOperatorFormula const& formula) override;
            virtual bool simplifyForCumulativeRewards(storm::logic::RewardOperatorFormula const& formula) override;
            
                        
            /*!
             * Eliminates all end components of the model satisfying
             * * ignoredStates is false for all states of the EC
             * * (if rewardModelName is given) there is no reward collected while staying inside the EC.
             *
             * Eliminating an EC means that it is replaced by a single state whose incoming and outgoing tansitions correspond to the incoming and outgoing transitions of the EC
             *
             * The resulting model will only have the rewardModel with the provided name (or no reward model at all if no name was given)
             */
            static std::shared_ptr<SparseModelType> eliminateNeutralEndComponents(SparseModelType const& model, storm::storage::BitVector const& ignoredStates, boost::optional<std::string> const& rewardModelName = boost::none);
            
        };
    }
}
