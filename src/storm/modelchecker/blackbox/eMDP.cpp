#include <algorithm>

#include "eMDP.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename ValueType>
eMDP<ValueType>::eMDP() : explorationOrder(), hashStorage(), stateLabeling() {
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
std::string eMDP<ValueType>::toDotString() {
    std::unordered_map<int, std::string> color_map; //colors to distinguish actions 
    color_map[0] = "green";
    color_map[1] = "red";
    color_map[2] = "blue";
    color_map[3] = "yellow";
    color_map[4] = "pink";
    color_map[5] = "orange";

    std::string dot_str = "digraph G {\n";
    dot_str += "node [shape=circle style=filled, fixedsize=true, width=2, height=2]\n"; //Node Attributes 
    
    dot_str += std::to_string(init_state) + " [fillcolor=green]\n"; //Make the initial state a different color 

    for (auto state : hashStorage.get_state_vec()) {
        std::string action_str = ""; //build a string of all the actions and total Samples of state 
        int color_ctr = 0; //increment the state color per action 
        for (auto action : hashStorage.get_state_actions_vec(state)) {
            action_str += "\\n act: " + std::to_string(action) 
            + " | #tot_spl: " + std::to_string(hashStorage.get_total_samples(state, action));
            for (auto succ : hashStorage.get_state_action_succ_vec(state, action)) {
                dot_str += "  " + std::to_string(state) + " -> " + std::to_string(succ) // transition 
                + " [label=\"act: "  + std::to_string(action) + "\\n #spl: " //label with action and samples 
                + std::to_string(hashStorage.get_succ_samples(state, action, succ)) 
                + "\", color=" + color_map[color_ctr] + "]\n"; //color of trabsition 
            }
            color_ctr++;
        }
        dot_str += "  " + std::to_string(state) + " [ label=\"state: " + std::to_string(state) + action_str + "\"]\n"; // text in state 
    }

    dot_str += "}\n";
    return dot_str;
}

template<typename ValueType>
void eMDP<ValueType>::writeDotFile(std::string filename) {
    std::ofstream MyFile(filename);
    MyFile << toDotString();
    MyFile.close();
}

template<typename ValueType>
void eMDP<ValueType>::printDot() {
    std::cout(toDotString());
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
void eMDP<ValueType>::addStateLabel(std::string label, index_type state) {
    auto* labelVec = &stateLabeling[state];
    auto it = find(labelVec->begin(), labelVec->end(), label);
    if(it == labelVec->end())
        labelVec->push_back(label);
}

template<typename ValueType>
void eMDP<ValueType>::removeStateLabel(std::string label, index_type state) {
    auto* labelVec = &stateLabeling[state];

    auto it = find(labelVec->begin(), labelVec->end(), label);
    if(it != labelVec->end())
        labelVec->erase(it);
}

template<typename ValueType>
std::vector<std::string> eMDP<ValueType>::getStateLabels(index_type state) {
    return stateLabeling[state];
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

template<typename ValueType>
storage::KeyIterator<ValueType> eMDP<ValueType>::get_state_itr() {
    return hashStorage.get_state_itr();
}

template<typename ValueType>
storage::KeyIterator<ValueType> eMDP<ValueType>::get_state_actions_itr(index_type state) {
    return hashStorage.get_state_actions_itr(state);
}

template<typename ValueType>
storage::KeyIterator<ValueType> eMDP<ValueType>::get_state_action_succ_itr(index_type state, index_type action) {
    return hashStorage.get_state_action_succ_itr(state, action);
}

} //namespace blackbox
} //namespace modelchecker
} //namespace storm


int main(int argc, char const *argv[]) {
    auto emdp = storm::modelchecker::blackbox::eMDP<int_fast32_t>();
    emdp.addInitialState(1);
    
    emdp.addStateLabel("label1", 1);
    emdp.addStateLabel("label2", 1);
    emdp.addStateLabel("label2", 1);
    emdp.addStateLabel("label3", 1);
    
    for(auto x : emdp.getStateLabels(1)) {
        std::cout << x << "\n";
    }

    std::cout << "\n";
    emdp.removeStateLabel("label1", 1);
    emdp.removeStateLabel("label2", 1);

    for(auto x : emdp.getStateLabels(1)) {
        std::cout << x << "\n";
    }
    
    
    emdp.writeDotFile("name.txt");
    return 0;
}

