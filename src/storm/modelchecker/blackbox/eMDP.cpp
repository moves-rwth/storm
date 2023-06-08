//
// Created by Maximilian Kamps on 25.05.23.
//

#include "eMDP.h"
#include "stdint.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<class ValueType>
eMDP<ValueType>::eMDP() : explorationOrder(), hashStorage(){
}

template<class ValueType>
void eMDP<ValueType>::addStateToExplorationOrder(index_type state) {
    if(explorationOrder.find(state) == explorationOrder.end())
        explorationOrder[state] = (explorationCount++);
}

template<class ValueType>
void eMDP<ValueType>::print() {

    std::cout << "exploration order [state, explorationTime]:\n";
    for (const auto& i: explorationOrder)
        std::cout << "[" << i.first << ", " << i.second << "] ";

    std::cout << "\nexplored eMDP:\n";
    hashStorage.print();
}

template<class ValueType>
void eMDP<ValueType>::addVisit(index_type state, index_type action, index_type succ) {
    addStateToExplorationOrder(succ);
    hashStorage.inc_trans(state, action, succ, 1);
}

template<class ValueType>
void eMDP<ValueType>::addVisits(index_type state, index_type action, index_type succ, ValueType visits) {
    addStateToExplorationOrder(succ);
    hashStorage.inc_trans(state, action, succ, visits);
}

template<class ValueType>
void eMDP<ValueType>::addState(index_type state, std::vector<index_type> avail_actions) {
    addStateToExplorationOrder(state);
    hashStorage.add_state_actions(state, avail_actions);
}

template<class ValueType>
bool eMDP<ValueType>::isStateKnown(index_type state) {
    return hashStorage.state_exists(state);
}

template<class ValueType>
ValueType eMDP<ValueType>::getSampleCount(index_type state, index_type action) {
    return hashStorage.get_total_samples(state, action);
}

template<class ValueType>
ValueType eMDP<ValueType>::getSampleCount(index_type state, index_type action, index_type succ) {
    return hashStorage.get_succ_samples(state, action, succ);
}

} //namespace blackbox
} //namespace modelchecker
} //namespace storm

/*
int main(int argc, char const *argv[]) {
    auto emdp = storm::modelchecker::blackbox::eMDP<uint_fast64_t>();
    emdp.addVisit(1,2,3);
    emdp.addVisits(1,2,4,10);
    emdp.addVisit(1,2,3);
    emdp.addVisits(2,2,3,16);
    emdp.print();
    return 0;
}
*/
