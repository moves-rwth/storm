#include <unordered_map>
#include <stdint.h>
#include <vector>
#include <utility>
#include <iostream>


//TODO: Should we make HashStorage templated or is it fine if we use uint_fast64_t?

namespace storm {
namespace modelchecker {
namespace blackbox {
namespace storage {

/**
 * Class to store eMDPs
 * The underlying data structure uses consequitive hashmaps and NOT matrices
 */
class HashStorage {
   private:
    /*!
                * Helper function, returns the succ_map of a (state,action) Pair
                * 
                * @param state 
                * @param action 
                * @return std::unordered_map<uint_fast64_t, uint_fast64_t>
     */
    std::unordered_map<uint_fast64_t, uint_fast64_t> get_succ_map(uint_fast64_t state, uint_fast64_t action);

   public:
    /*!
                 * adds state do data 
                 * 
                 * @param state
     */
    void add_state(uint_fast64_t state);

    /*!
                 * Adds a vector of actions to the state 
                 * (only if the state does not have any actions yet!)
                 * 
                 * @param state 
                 * @param actions
     */
    void add_state_actions(uint_fast64_t state, std::vector<uint_fast64_t> actions);

    /*!
                 * Increments a transition of the form (state,action,succ) = samples
                 * If state or succ don't exists yet they get added to data 
                 * Increments the total samples of the action by samples 
                 * 
                 * @param state 
                 * @param action 
                 * @param succ 
                 * @param samples
     */
    void inc_trans(uint_fast64_t state, uint_fast64_t action, uint_fast64_t succ, uint_fast64_t samples);

    /*!
                 * Returns a vector of all states 
                 * 
                 * @return std::vector<uint_fast64_t>
     */
    std::vector<uint_fast64_t> get_state_vec();

    /*!
                 * Returns a vector of available actions for the state  
                 * 
                 * @param state 
                 * @return std::vector<uint_fast64_t>
     */
    std::vector<uint_fast64_t> get_state_actions_vec(uint_fast64_t state);

    /*!
                 * Returns a vector of all successors for a (state,action) pair 
                 * 
                 * @param state 
                 * @param action 
                 * @return std::vector<uint_fast64_t>
     */
    std::vector<uint_fast64_t> get_state_action_succ_vec(uint_fast64_t state, uint_fast64_t action);

    /*!
                 * Returns true if the state exists in data 
                 * 
                 * @param state 
                 * @return true 
                 * @return false
     */
    bool state_exists(uint_fast64_t state);

    /*!
                 * Returns the total samples for a given state and action 
                 * 
                 * @param state 
                 * @param action 
                 * @return uint_fast64_t
     */
    int_fast64_t get_total_samples(uint_fast64_t state, uint_fast64_t action);

    /*!
                 * Returns the samples for a (state,action,succ) triple
                 * 
                 * @param state 
                 * @param action 
                 * @param succ 
                 * @return uint_fast64_t
     */
    int_fast64_t get_succ_samples(uint_fast64_t state, uint_fast64_t action, uint_fast64_t succ);

    /*!
                 * Prints the data structure to std::cout
                 *
     */
    void print();
};
}
}
}
}
