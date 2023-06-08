#include "HashStorage.h"

namespace storm {
namespace modelchecker {
namespace blackbox {
namespace storage {

/*!
 * eMDPs are saved as 3 consequitive hashmaps 
 * 1.) key = state      | value = 2.) hashmap 
 * 2.) key = action     | value = pair (#total samples, 3.) hashmap)
 * 3.) key = succ       | value = #samples of the (state,action,succ) triple
 */
typedef std::pair<uint_fast64_t, std::unordered_map<uint_fast64_t, uint_fast64_t> > count_sampleMap_pair;
std::unordered_map<uint_fast64_t, std::unordered_map<uint_fast64_t, count_sampleMap_pair> > data;

std::unordered_map<uint_fast64_t, uint_fast64_t> HashStorage::get_succ_map(uint_fast64_t state, uint_fast64_t action) {
    std::unordered_map<uint_fast64_t, uint_fast64_t> succ_map;
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

void HashStorage::add_state(uint_fast64_t state) {
    if (data.find(state) == data.end())
        data[state] = std::unordered_map<uint_fast64_t, count_sampleMap_pair>();
}

void HashStorage::add_state_actions(uint_fast64_t state, std::vector<uint_fast64_t> actions) {
    add_state(state);
    auto* act_map = &data.at(state);
    if (act_map->begin() == act_map->end()) {
        for (auto act : actions) (*act_map)[act] = count_sampleMap_pair();
    }
}

void HashStorage::inc_trans(uint_fast64_t state, uint_fast64_t action, uint_fast64_t succ, uint_fast64_t samples) {
    add_state(state);  // add state to data if it doesn't exist
    add_state(succ);   // add succ to data if it doesn't exist

    auto* act_map = &data.at(state);  // add action to data if it doesn't exist
    if (act_map->find(action) == act_map->end())
        (*act_map)[action] = count_sampleMap_pair();

    auto* sample_map = &(act_map->at(action).second);
    (*act_map).at(action).first += samples;  // Increment the total samples for the action
    (*sample_map)[succ] += samples;          // Increments the samples for the (state,action,succ) triple
}

std::vector<uint_fast64_t> HashStorage::get_state_vec() {
    std::vector<uint_fast64_t> state_vec;
    for (auto const& p : data) state_vec.push_back(p.first);
    return state_vec;
}

std::vector<uint_fast64_t> HashStorage::get_state_actions_vec(uint_fast64_t state) {
    std::vector<uint_fast64_t> action_vec;
    if (data.find(state) != data.end())
        for (auto const& p : data.at(state)) action_vec.push_back(p.first);
    return action_vec;
}

std::vector<uint_fast64_t> HashStorage::get_state_action_succ_vec(uint_fast64_t state, uint_fast64_t action) {
    std::vector<uint_fast64_t> succ_vec;
    for (auto const& p : get_succ_map(state, action)) succ_vec.push_back(p.first);
    return succ_vec;
}

bool HashStorage::state_exists(uint_fast64_t state) {
    return data.find(state) != data.end();
}

int_fast64_t HashStorage::get_total_samples(uint_fast64_t state, uint_fast64_t action) {
    if (data.find(state) == data.end()) {
        return -1;
    }

    auto* act_map = &data.at(state);
    if (act_map->find(action) == act_map->end()) {
        return -1;
    }
    return act_map->at(action).first;
}

int_fast64_t HashStorage::get_succ_samples(uint_fast64_t state, uint_fast64_t action, uint_fast64_t succ) {
    auto succ_map = get_succ_map(state, action);

    if (succ_map.find(succ) != succ_map.end())
        return succ_map.at(succ);
    return -1;
}

void HashStorage::print() {
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

/*
int main(int argc, char const *argv[])
{
        HashStorage x = HashStorage();
        x.inc_trans(1,2,3,10);
        x.inc_trans(1,2,3,5);
        x.inc_trans(1,2,4,3);
        x.inc_trans(5,6,9,3);

        std::vector<uint_fast64_t> vect;
        vect.push_back(100);
        vect.push_back(101);
        vect.push_back(102);

        x.add_state_actions(10, vect);

        x.print();
}
*/
}
}
}
}


