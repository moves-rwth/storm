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
    visitsMatrix.updateDimensions();

    std::cout << "exploration order:\n";
    for (const auto& i: explorationOrder)
        std::cout << "[" << i.first << ", " << i.second << "] ";
    
    std::cout << "\n";

    std::cout << "visitsMatrix:\n";
    std::cout << "rowCount: " << visitsMatrix.getRowCount() << "\n";
    std::cout << "columnCount: " << visitsMatrix.getColumnCount() << "\n";
    for (int i = 0; i < visitsMatrix.getRowCount(); i++)
        visitsMatrix.printRow(std::cout, i);
        std::cout << "\n";
}

template<class ValueType>
void eMDP<ValueType>::addVisit(index_type state, index_type action, index_type succ) {
    this->addVisits(state, action, succ, 1);
}

template<class ValueType>
void eMDP<ValueType>::addVisits(index_type state, index_type action, index_type succ, index_type visits) {
    // TODO add checks if state and succ are already known
    //      -> throw errors or add them yourself?
    index_type state_index = explorationOrder[state];
    index_type succ_index = explorationOrder[succ];
    index_type row_group_index = visitsMatrix.getRowGroupIndices()[state_index];

    auto& row = visitsMatrix.getRow(row_group_index, action);
    index_type row_size = row.size(); 

    // the row potentially is not big enough
    if (row_size <= succ_index) {
        row.resize(succ_index + 1);
        for (index_type i = row_size; i < succ_index + 1; i++) {
            row[i].setColumn(i);
        }
    }
    
    row[succ_index].setValue(row[succ_index].getValue() + visits);
}

template<class ValueType>
void eMDP<ValueType>::addState(index_type state, index_type avail_actions){
    // add state to explorationOrder -> mark it as known
    int knownStates = explorationOrder.size();
    explorationOrder.emplace(state, knownStates);
    
    // reserve avail_actions many new rows in visitsMatrix
    visitsMatrix.addRows(avail_actions);    
}

template<class ValueType>
bool eMDP<ValueType>::isStateKnown(index_type state) {
    return explorationOrder.find(state) != explorationOrder.end();
}

template<class ValueType>
typename eMDP<ValueType>::index_type eMDP<ValueType>::getVisited(index_type state, index_type action) {
    index_type state_index = explorationOrder[state];
    index_type row_group_index = visitsMatrix.getRowGroupIndices()[state_index];
    return visitsMatrix.getRowSum(row_group_index + action);
}

template<class ValueType>
typename eMDP<ValueType>::index_type eMDP<ValueType>::getVisited(index_type state, index_type action, index_type succ) {
    index_type state_index = explorationOrder[state];
    index_type row_group_index = visitsMatrix.getRowGroupIndices()[state_index];
    return visitsMatrix.getRow(row_group_index, action)[succ].getValue();
}

template class eMDP<int>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
