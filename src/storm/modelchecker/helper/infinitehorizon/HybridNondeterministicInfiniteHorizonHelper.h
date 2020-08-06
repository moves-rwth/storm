#pragma once
#include "storm/modelchecker/helper/SingleValueModelCheckerHelper.h"

#include "storm/modelchecker/results/HybridQuantitativeCheckResult.h"

#include "storm/models/symbolic/NondeterministicModel.h"
#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

namespace storm {
    class Environment;
    
    namespace modelchecker {
        namespace helper {
        
            /*!
             * Helper class for model checking queries that depend on the long run behavior of the (nondeterministic) system.
             */
            template <typename ValueType, storm::dd::DdType DdType>
            class HybridNondeterministicInfiniteHorizonHelper : public SingleValueModelCheckerHelper<ValueType, DdType> {

            public:
                /*!
                 * Initializes the helper for a discrete time (i.e. MDP)
                 */
                HybridNondeterministicInfiniteHorizonHelper(storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix);
                
                /*!
                 * Initializes the helper for a continuous time (i.e. MA)
                 */
                HybridNondeterministicInfiniteHorizonHelper(storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& markovianStates, storm::dd::Add<DdType, ValueType> const& _exitRates);
                
                /*!
                 * Computes the long run average probabilities, i.e., the fraction of the time we are in a psiState
                 * @return a value for each state
                 */
                std::unique_ptr<HybridQuantitativeCheckResult<DdType, ValueType>> computeLongRunAverageProbabilities(Environment const& env, storm::dd::Bdd<DdType> const& psiStates);
                
                /*!
                 * Computes the long run average rewards, i.e., the average reward collected per time unit
                 * @return a value for each state
                 */
                std::unique_ptr<HybridQuantitativeCheckResult<DdType, ValueType>> computeLongRunAverageRewards(Environment const& env, storm::models::symbolic::StandardRewardModel<DdType, ValueType> const& rewardModel);
                
            protected:
                
                /*!
                 * @return true iff this is a computation on a continuous time model (i.e. MA)
                 */
                bool isContinuousTime() const;


            private:
                storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& _model;
                storm::dd::Add<DdType, ValueType> const& _transitionMatrix;
                storm::dd::Bdd<DdType> const* _markovianStates;
                storm::dd::Add<DdType, ValueType> const* _exitRates;
            };
        }
    }
}
