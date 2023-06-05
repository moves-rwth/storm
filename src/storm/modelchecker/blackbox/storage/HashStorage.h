#include <unordered_map>
#include <stdint.h>
#include <vector>
#include <utility>
#include <iostream>

/**
 * Class to store eMDPs
 * The underlying data structure uses consequitive hashmaps and NOT matrices 
 */
class HashStorage {
        public:
                /*!
                 * Adds a transition of the form (state,action,succ) = samples
                 * Increments the total samples of the action by samples 
                 * 
                 * @param state 
                 * @param action 
                 * @param succ 
                 * @param samples 
                 */
                void add_trans(uint_fast64_t state, uint_fast64_t action, uint_fast64_t succ, uint_fast64_t samples);
                /*!
                 * Adds a vector of actions for the state 
                 * 
                 * @param state 
                 * @param actions 
                 */
                void add_state_actions(uint_fast64_t state, std::vector<uint_fast64_t> actions);

                /*!
                 * Returns a vector of available actions for the state  
                 * 
                 * @param state 
                 * @return std::vector<uint_fast64_t> 
                 */
                std::vector<uint_fast64_t> get_state_actions(uint_fast64_t state);

                /*!
                 * Returns the total samples for a given state and action 
                 * 
                 * @param state 
                 * @param action 
                 * @return uint_fast64_t 
                 */
                uint_fast64_t get_total_samples(uint_fast64_t state, uint_fast64_t action);

                /*!
                 * Returns the samples for a (state,action,succ) tripple 
                 * 
                 * @param state 
                 * @param action 
                 * @param succ 
                 * @return uint_fast64_t 
                 */
                uint_fast64_t get_succ_action_samples(uint_fast64_t state, uint_fast64_t action, uint_fast64_t succ);
};