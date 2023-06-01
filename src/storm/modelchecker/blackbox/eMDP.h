#ifndef STORM_EMDP_H
#define STORM_EMDP_H

#include "storm/models/sparse/NondeterministicModel.h"
#include "storm/storage/FlexibleSparseMatrix.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<class ValueType>
class eMDP {
   public:
    typedef uint_fast64_t index_type;
    
    //TODO initial state has to be somehow set
    /*!
     * Constructs an empty eMDP
     */
    eMDP();

    /*!
     * increments the visits count of the given tripel by 1
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
    void addVisits(index_type state, index_type action, index_type succ, index_type visits);

    /*!
     * Add a new state to the eMDP
     * @param state  state index of new state 
     * @param avail_actions  number of available actions at state
     */
    void addState(index_type state, index_type avail_actions);

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
    index_type getVisited(index_type state, index_type action);

    /*!
     * returns how often this state action successor triple was sampled
     * @param state   state index
     * @param action  action index
     * @param succ    successor state index
     */
    index_type getVisited(index_type state, index_type action, index_type succ);

    //? Save to disk

   private:
   storm::storage::FlexibleSparseMatrix<ValueType> visitsMatrix;
   std::unordered_map<index_type, index_type> explorationOrder;  // maps state to its position of when its been found
    //flexSparseMatrix mit value integer

    //see bMDP
        //mapping von state zu index und zurück?
        //available actions
        // 0 5 9
};

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_EMDP_H
