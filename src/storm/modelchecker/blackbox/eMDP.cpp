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
}


template<class ValueType>
void eMDP<ValueType>::addState(index_type state, std::vector<index_type> avail_actions){
    // add state to explorationOrder -> mark it as known
    int knownStates = 0;
    explorationOrder.emplace(state, knownStates);
    
    // reserve avail_actions many new rows in visitsMatrix
    visitsMatrix.addRows(avail_actions.size());
    

    
}

template<class ValueType>
bool eMDP<ValueType>::isStateKnown(index_type state) {
    return explorationOrder.find(state) != explorationOrder.end();
}

template<class ValueType>
void eMDP<ValueType>::test() {
    print();
    addState(1, (std::vector<index_type>()));
    print();
}

template class eMDP<int>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
