//
// Created by Maximilian Kamps on 25.05.23.
//

#include "eMDP.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<class ValueType>
eMDP<ValueType>::eMDP(): visitsMatrix(), explorationOrder() {
    // intentionally left empty
    print();
}

template<class ValueType>
void eMDP<ValueType>::print() {
    std::cout << 'exploration order:\n';
    for (const auto& i: explorationOrder)
        std::cout << i.second << ' ';
    
    std::cout << '\n';

    std::cout << 'visitsMatrix:\n';
    for (int i = 0; i < visitsMatrix.getRowCount(); i++)
        visitsMatrix.printRow(std::cout, i);
}

template<class ValueType>
void eMDP<ValueType>::addVisit(index_type state, index_type action, index_type succ) {

}


template<class ValueType>
void eMDP<ValueType>::addState(index_type state, std::vector<index_type> avail_actions){
    
}

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
