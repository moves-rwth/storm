#pragma once

#include "storm/models/sparse/Pomdp.h"

namespace storm {
    namespace synthesis {

        template<typename ValueType=double>
        class SimulatedModel {

        public:

            /** Constructor (contains precomputations). */
            SimulatedModel(storm::models::sparse::Pomdp<ValueType> const& pomdp);
            
            /** Get random action index of the state. */
            uint_fast64_t sampleAction(uint_fast64_t state);
            
            /** Get random successor for the given action in the given state. */
            uint_fast64_t sampleSuccessor(uint_fast64_t state, uint_fast64_t action);
            
            /**
             * Simulate a path from the state and estimate the accumulated reward.
             * @param state the initial state
             * @param action action taken in the initial state
             * @param length length of the simulated path
             * @param reward_name name of the reward model to infer rewards from
             * @param discount_factor the discount factor
             * @return discounted reward accumulated on the simulated path
             */
            double stateActionRollout(
                uint_fast64_t state, uint_fast64_t action, uint_fast64_t length,
                std::string const& reward_name, double discount_factor
            );

        };
    
    } 
}
