#ifndef STORM_EMDP_H
#define STORM_EMDP_H

#include "storage/HashStorage.h"

namespace storm {
namespace modelchecker {
namespace blackbox {


template<class ValueType>

//TODO: How should initial state be saved and handled?
//TODO: Add Methods for access of explorationOrder

class eMDP {
   public:
    typedef uint_fast64_t index_type;
    
    /*!
     * Constructs an empty eMDP
     */
    eMDP();

    /*!
     * increments the visits count of the given triple by 1
     * @param state   state index in which action was taken 
     * @param action  action index of chosen action
     * @param succ    state index of successor state
     */
    void addVisit(index_type state, index_type action, index_type succ);

    /*!
     * increments the visits count of the given by visits
     * @param state   state index in which action was taken 
     * @param action  action index of chosen action
     * @param succ    state index of successor state
     * @param visits  visits to be added to counter
     */
    void addVisits(index_type state, index_type action, index_type succ, ValueType visits);

    /*!
     * Add a new state to the eMDP
     * @param state  state index of new state
     * @param avail_actions vector of available actions at state
     */
    void addState(index_type state, std::vector<index_type> avail_actions);

    /*!
     * print the eMDP to std::cout
     */
    void print();

    /*!
     * returns true. if the state was already added to this eMDP. false otherwise
     * @param state  state index of tested state
     */
    bool isStateKnown(index_type state);

    /*!
     * returns how often this state action pair was sampled
     * @param state   state index 
     * @param action  action index
     */
    ValueType getSampleCount(index_type state, index_type action);

    /*!
     * returns how often this state action successor triple was sampled
     * @param state   state index
     * @param action  action index
     * @param succ    successor state index
     */
    ValueType getSampleCount(index_type state, index_type action, index_type succ);

    //? Save to disk

   private:
    /*!
     * Adds the state and its corresponding exploration time to explorationOrder
     *
     * @param state
     */
    void addStateToExplorationOrder(index_type state);

   storage::HashStorage hashStorage;
   std::unordered_map<index_type, index_type> explorationOrder; // maps state to its position of when its been found
   index_type explorationCount = 0; //Number of explored states
};

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_EMDP_H
