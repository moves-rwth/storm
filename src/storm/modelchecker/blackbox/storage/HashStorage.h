#include <unordered_map>
#include <stdint.h>
#include <vector>
#include <utility>
#include <iostream>



namespace storm {
namespace modelchecker {
namespace blackbox {
namespace storage {


template<typename IntValueType> 
class KeyIterator {
    private: 
     std::unordered_map<int, void*>::iterator cur;
     std::unordered_map<int, void*>::iterator end;
    public:
     KeyIterator(void* map);
     KeyIterator();

     IntValueType next();
     bool hasNext();
}; 

/*!
 * Class to store eMDPs
 * The underlying data structure uses consequitive hashmaps and NOT matrices
 */
template<typename IntValueType>
class HashStorage {
   private:
   typedef IntValueType index_type;
    /*!
     * Helper function, returns the succ_map of a (state,action) Pair
     * 
     * @param state 
     * @param action 
     * @return std::unordered_map<index_type, index_type>
     */

        /*!
    * eMDPs are saved as 3 consequitive hashmaps 
    * 1.) key = state      | value = 2.) hashmap 
    * 2.) key = action     | value = pair (#total samples, 3.) hashmap)
    * 3.) key = succ       | value = #samples of the (state,action,succ) triple
    */

    using count_sampleMap_pair = std::pair<index_type, std::unordered_map<index_type, index_type> >;
    std::unordered_map<index_type, std::unordered_map<index_type, count_sampleMap_pair > > data;

    std::unordered_map<index_type, index_type> get_succ_map(index_type state, index_type action);

   public:
    /**
     * Construct empty Hash Storage 
     * 
     */
    HashStorage();

    /*!
     * adds state do data 
     * sets the #actions available to the state to -1
     * 
     * @param state
     */
    void add_state(index_type state);

    /*!
     * Adds a vector of actions to the state 
     * (only if the state does not have any actions yet!)
     * 
     * @param state 
     * @param actions
     */
    void add_state_actions(index_type state, std::vector<index_type> actions);

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
    void inc_trans(index_type state, index_type action, index_type succ, index_type samples);

    /*!
     * Returns a vector of all states 
     * 
     * @return std::vector<index_type>
     */
    std::vector<index_type> get_state_vec();

    /*!
     * Returns a vector of available actions for the state  
     * 
     * @param state 
     * @return std::vector<index_type>
     */
    std::vector<index_type> get_state_actions_vec(index_type state);

    /*!
     * Return as vector of successors for a state action pair 
     * 
     * @param state 
     * @param action 
     * @return std::vector<index_type> 
     */

    std::vector<index_type> get_state_action_succ_vec(index_type state, index_type action);

    /*!
     * Returns a KeyIterator over the states 
     * 
     * @return KeyIterator<index_type> 
     */
    KeyIterator<index_type> get_state_itr();

    /*!
     * Returns a KeyIterator over the actions of a state
     * 
     * @param state 
     * @return KeyIterator<index_type> 
     */
    KeyIterator<index_type> get_state_actions_itr(index_type state);

    /*!
     * Returns a KeyIterator over the successor of a state for a given action 
     * 
     * @param state 
     * @param action 
     * @return KeyIterator<index_type> 
     */
    KeyIterator<index_type> get_state_action_succ_itr(index_type state, index_type action);

    /*!
     * Returns true if the passed state is in data 
     * 
     * @param state 
     * @return true 
     * @return false 
     */
    bool state_exists(index_type state);

    /*!
     * Returns the total samples for a given state and action 
     * 
     * @param state 
     * @param action 
     * @return index_type
     */
    index_type get_total_samples(index_type state, index_type action);

    /*!
     * Returns the samples for a (state,action,succ) triple
     * 
     * @param state 
     * @param action 
     * @param succ 
     * @return index_type
     */
    index_type get_succ_samples(index_type state, index_type action, index_type succ);

    /*!
     * Returns how many states there are in this HashStorage
     */
    index_type get_state_count();

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
