//
// Created by Maximilian Kamps on 25.05.23.
//

#include "eMDP.h"
#include "stdint.h"
#include <string.h>

namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename ValueType>
eMDP<ValueType>::eMDP() : explorationOrder(), hashStorage() {
}


template<typename ValueType>
void eMDP<ValueType>::addInitialState(index_type state) {
    if(init_state == -1) 
        hashStorage.add_state(state);
        init_state = state;
}

template<typename ValueType>
void eMDP<ValueType>::addStateToExplorationOrder(index_type state) {
    if(explorationOrder.find(state) == explorationOrder.end())
        explorationOrder[state] = (explorationCount++);
}

template<typename ValueType>
void eMDP<ValueType>::print() {

    std::cout << "exploration order [state, explorationTime]:\n";
    for (const auto& i: explorationOrder)
        std::cout << "[" << i.first << ", " << i.second << "] ";
    std::cout << "\nInitial State: " << init_state << "\n";
    std::cout << "explored eMDP:\n";
    hashStorage.print();
}



template<typename ValueType>
void eMDP<ValueType>::toDot() {
    std::unordered_map<int, std::string> color_map; //colors to distinguish actions 
    color_map[0] = "green";
    color_map[1] = "red";
    color_map[2] = "blue";
    color_map[3] = "yellow";
    color_map[4] = "pink";
    color_map[5] = "orange";

    std::cout << "digraph G {\n";
    std::cout << "node [shape=circle style=filled, fixedsize=true, width=2, height=2]\n"; //Node Attributes 
    
    std::cout << init_state << " [fillcolor=green]\n"; //Make the initial state a different color 

    for (auto state : hashStorage.get_state_vec()) {
        std::string action_str = ""; //build a string of all the actions and total Samples of state 
        int color_ctr = 0; //increment the state color per action 
        for (auto action : hashStorage.get_state_actions_vec(state)) {
            action_str += "\\n act: " + std::to_string(action) 
            + " | #tot_spl: " + std::to_string(hashStorage.get_total_samples(state, action));
            for (auto succ : hashStorage.get_state_action_succ_vec(state, action)) {
                std::cout << "  " << state << " -> " << succ // transition 
                << " [label=\"act: "  << action << "\\n #spl: " //label with action and samples 
                << hashStorage.get_succ_samples(state, action, succ) 
                << "\", color=" << color_map[color_ctr] << "]\n"; //color of trabsition 
            }
            color_ctr++;
        }
        std::cout << "  " << state << " [ label=\"state: " << state << action_str << "\"]\n"; // text in state 
    }

    std::cout << "}\n";
}

template<typename ValueType>
void eMDP<ValueType>::addVisit(index_type state, index_type action, index_type succ) {
    addStateToExplorationOrder(succ);
    hashStorage.inc_trans(state, action, succ, 1);
}

template<typename ValueType>
void eMDP<ValueType>::addVisits(index_type state, index_type action, index_type succ, ValueType visits) {
    addStateToExplorationOrder(succ);
    hashStorage.inc_trans(state, action, succ, visits);
}

template<typename ValueType>
void eMDP<ValueType>::addState(index_type state, std::vector<index_type> avail_actions) {
    addStateToExplorationOrder(state);
    hashStorage.add_state_actions(state, avail_actions);
}

template<typename ValueType>
bool eMDP<ValueType>::isStateKnown(index_type state) {
    return hashStorage.state_exists(state);
}

template<typename ValueType>
ValueType eMDP<ValueType>::getSampleCount(index_type state, index_type action) {
    return hashStorage.get_total_samples(state, action);
}

template<typename ValueType>
ValueType eMDP<ValueType>::getSampleCount(index_type state, index_type action, index_type succ) {
    return hashStorage.get_succ_samples(state, action, succ);
}

} //namespace blackbox
} //namespace modelchecker
} //namespace storm


int main(int argc, char const *argv[]) {
    auto emdp = storm::modelchecker::blackbox::eMDP<int_fast32_t>();
    emdp.addInitialState(1);
    emdp.addVisit(1,10,2);
    emdp.addVisit(2,11,3);
    emdp.addVisit(3,12,4);
    emdp.addVisit(3,12,5);
    emdp.toDot();
    return 0;
}

