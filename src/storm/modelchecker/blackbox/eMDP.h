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

    void addVisit(index_type state, index_type action, index_type succ);

    void addVisits(index_type state, index_type action, index_type succ, index_type visits);

    void addState(index_type state, index_type avail_actions);

    void print();

    bool isStateKnown(index_type state);

    index_type getVisited(index_type state, index_type action);

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
