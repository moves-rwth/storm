#include "HashStorage.h"

namespace storm {
namespace modelchecker {
namespace blackbox {
namespace storage {

template<typename IntValueType>
HashStorage<IntValueType>::HashStorage() {
    data = std::unordered_map<index_type, std::unordered_map<index_type, count_sampleMap_pair > >();
}

template<typename IntValueType>
std::unordered_map<IntValueType, IntValueType> HashStorage<IntValueType>::get_succ_map(IntValueType state, IntValueType action) {
    std::unordered_map<IntValueType, IntValueType> succ_map;
    if (data.find(state) == data.end()) {
        return succ_map;
    }

    auto act_map = &data.at(state);
    if (act_map->find(action) == act_map->end()) {
        return succ_map;
    }

    succ_map = act_map->at(action).second;
    return succ_map;
}

template<typename IntValueType>
void HashStorage<IntValueType>::add_state(IntValueType state) {
    if (data.find(state) == data.end())
        data[state] = std::unordered_map<IntValueType, count_sampleMap_pair>();
}

template<typename IntValueType>
void HashStorage<IntValueType>::add_state_actions(IntValueType state, std::vector<IntValueType> actions) {
    add_state(state);
    auto* act_map = &data.at(state);
    if (act_map->begin() == act_map->end()) {
        for (auto act : actions) (*act_map)[act] = count_sampleMap_pair();
    }
}

template<typename IntValueType>
void HashStorage<IntValueType>::inc_trans(IntValueType state, IntValueType action, IntValueType succ, IntValueType samples) {
    add_state(state);  // add state to data if it doesn't exist
    add_state(succ);   // add succ to data if it doesn't exist

    auto* act_map = &data.at(state);  // add action to data if it doesn't exist
    if (act_map->find(action) == act_map->end())
        (*act_map)[action] = count_sampleMap_pair();

    auto* sample_map = &(act_map->at(action).second);
    (*act_map).at(action).first += samples;  // Increment the total samples for the action
    (*sample_map)[succ] += samples;          // Increments the samples for the (state,action,succ) triple
}

template<typename IntValueType>
std::vector<IntValueType> HashStorage<IntValueType>::get_state_vec() {
    std::vector<IntValueType> state_vec;
    for (auto const& p : data) state_vec.push_back(p.first);
    return state_vec;
}

template<typename IntValueType>
std::vector<IntValueType> HashStorage<IntValueType>::get_state_actions_vec(IntValueType state) {
    std::vector<IntValueType> action_vec;
    if (data.find(state) != data.end())
        for (auto const& p : data.at(state)) action_vec.push_back(p.first);
    return action_vec;
}

template<typename IntValueType>
std::vector<IntValueType> HashStorage<IntValueType>::get_state_action_succ_vec(IntValueType state, IntValueType action) {
    std::vector<IntValueType> succ_vec;
    for (auto const& p : get_succ_map(state, action)) succ_vec.push_back(p.first);
    return succ_vec;
}

template<typename IntValueType>
bool HashStorage<IntValueType>::state_exists(IntValueType state) {
    return data.find(state) != data.end();
}

template<typename IntValueType>
IntValueType HashStorage<IntValueType>::get_total_samples(IntValueType state, IntValueType action) {
    if (data.find(state) == data.end()) {
        return -1;
    }

    auto* act_map = &data.at(state);
    if (act_map->find(action) == act_map->end()) {
        return -1;
    }
    return act_map->at(action).first;
}

template<typename IntValueType>
IntValueType HashStorage<IntValueType>::get_succ_samples(IntValueType state, IntValueType action, IntValueType succ) {
    auto succ_map = get_succ_map(state, action);

    if (succ_map.find(succ) != succ_map.end())
        return succ_map.at(succ);
    return -1;
}

template<typename IntValueType>
void HashStorage<IntValueType>::print() {
    for (auto state : get_state_vec()) {
        std::cout << "-------------------------\n";
        std::cout << "State: " << state << "\n";
        for (auto action : get_state_actions_vec(state)) {
            std::cout << " Action: " << action << " | Total Samples: " << get_total_samples(state, action) << "\n";
            for (auto succ : get_state_action_succ_vec(state, action)) {
                std::cout << "  * Succ: " << succ << " | Samples: " << get_succ_samples(state, action, succ) << "\n";
            }
        }
    }
}
template class HashStorage<int_fast32_t>; //Type for which class gets compiled 
template class HashStorage<int_fast64_t>; //Type for which class gets compiled 
}
}
}
}

/*
int main(int argc, char const *argv[])
{
        auto x = storm::modelchecker::blackbox::storage::HashStorage<int>();
        x.inc_trans(1,2,3,10);
        x.inc_trans(1,2,3,5);
        x.inc_trans(1,2,4,3);
        x.inc_trans(5,6,9,3);
        x.get_succ_samples(1,2,3);

        std::vector<int> vect;
        vect.push_back(100);
        vect.push_back(101);
        vect.push_back(102);

        x.add_state_actions(10, vect);

        x.print();
}
*/


