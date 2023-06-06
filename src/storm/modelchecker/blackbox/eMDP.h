#ifndef STORM_EMDP_H
#define STORM_EMDP_H

#include "storm/models/sparse/NondeterministicModel.h"
#include "storm/storage/FlexibleSparseMatrix.h"

namespace storm {
namespace modelchecker {
namespace blackbox {


/*!
* TODO
*   - don't use FlexibleSparseMatrix as visitsMatrix -> memory inefficient because each row has to be filled to last entry
*       -> Perhaps Vector of hashmaps is more reasonable
*   - Make class child class of NondeterministicModel
*       -> Problem: Nondeterministic model uses SparseMatrix
*   - eMDP for now expects to know all states, that are processed.
*       -> add checks or behavoir in case addVisit or getVisited was called with unknown states or actions
*   - getVisited rename to getSampleCount
*/
template<class ValueType>
class eMDP {
   public:
    typedef uint_fast64_t index_type;
    
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
    void addVisits(index_type state, index_type action, index_type succ, ValueType visits);

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
    ValueType getVisited(index_type state, index_type action);

    /*!
     * returns how often this state action successor triple was sampled
     * @param state   state index
     * @param action  action index
     * @param succ    successor state index
     */
    ValueType getVisited(index_type state, index_type action, index_type succ);

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
