//
// Created by Maximilian Kamps on 25.05.23.
//

#include "eMDP.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<class ValueType>
eMDP<ValueType>::eMDP() : explorationOrder(), visitsMatrix(0){
}

template<class ValueType>
void eMDP<ValueType>::print() {
    std::cout << "exploration order:\n";
    for (const auto& i: explorationOrder)
        std::cout << "[" << i.first << ", " << i.second << "] ";
    
    std::cout << "\n";

    std::cout << "visitsMatrix:\n";
    for (int i = 0; i < visitsMatrix.getRowCount(); i++)
        visitsMatrix.printRow(std::cout, i);
}

template<class ValueType>
void eMDP<ValueType>::addVisit(index_type state, index_type action, index_type succ) {
    // TODO add checks if state and succ are already known
    //      -> throw errors or add them yourself?
    index_type state_index = explorationOrder[state];
    index_type succ_index = explorationOrder[succ];
    index_type row_group_index = visitsMatrix.getRowGroupIndices()[state_index];

    auto row = visitsMatrix.getRow(row_group_index, action);
    // the row eventually is not big enough
    if (row.size() <= succ_index) {
        row.resize(succ_index + 1);
    }
    row[succ_index].setValue(row[succ_index].getValue() + 1);
}


template<class ValueType>
void eMDP<ValueType>::addState(index_type state, index_type avail_actions){
    // add state to explorationOrder -> mark it as known
    int knownStates = 0;
    explorationOrder.emplace(state, knownStates);
    
    // reserve avail_actions many new rows in visitsMatrix
    visitsMatrix.addRows(avail_actions);    
}

template<class ValueType>
bool eMDP<ValueType>::isStateKnown(index_type state) {
    return explorationOrder.find(state) != explorationOrder.end();
}

template<class ValueType>
void eMDP<ValueType>::test() {
    std::vector<index_type> actions = {0, 1};
    print();
    addState(0, actions);
    addState(1, actions);
    print();
    addVisit(0, 0, 0);
    addVisit(0, 0, 0);
    print();
    addVisit(0, 1, 1);
    addVisit(1, 0, 1);
    print();
}

template class eMDP<int>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
