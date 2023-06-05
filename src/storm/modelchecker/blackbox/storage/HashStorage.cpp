#include "HashStorage.h"

/*!
 * eMDPs are saved as 3 consequitive hashmaps 
 * 1.) key = state      | value = 2.) hashmap 
 * 2.) key = action     | value = pair (#total samples, 3.) hashmap)
 * 3.) key = succ       | value = #samples of the (state,action,succ) tripple
 */
typedef std::pair<uint_fast64_t, std::unordered_map<uint_fast64_t, uint_fast64_t> > count_sampleMap_pair;
std::unordered_map<uint_fast64_t, 
std::unordered_map<uint_fast64_t, count_sampleMap_pair> > data;

void HashStorage::add_trans(uint_fast64_t state, uint_fast64_t action, uint_fast64_t succ, uint_fast64_t samples) {
        if(data.find(state) == data.end()) { //add state to data if it doesn't exist 
                data[state] = std::unordered_map<uint_fast64_t, count_sampleMap_pair>();
        } 

        auto* act_map = &data.at(state); //add action to data if it doesn't exist 
        if(act_map->find(action) == act_map->end()) {
                (*act_map)[action] = count_sampleMap_pair();
        }

        auto* sample_map = &(act_map->at(action).second);
        if(sample_map->find(succ) == sample_map->end()) { //TODO: should samples be updatable?
                (*act_map).at(action).first += samples;
                (*sample_map)[succ] = samples;
        }
}

void HashStorage::add_state_actions(uint_fast64_t state, std::vector<uint_fast64_t> actions) {
        if(data.find(state) == data.end()) {
                std::unordered_map<uint_fast64_t, count_sampleMap_pair> tmp_map;
                for (auto act : actions) 
                        tmp_map[act] = count_sampleMap_pair();
                data[state] = tmp_map;
        } 
}

std::vector<uint_fast64_t> HashStorage::get_state_actions(uint_fast64_t state) {
        std::vector<uint_fast64_t> tmp;
        if(data.find(state) != data.end()) 
                for(auto const& p: data.at(state))
                tmp.push_back(p.first);
        return tmp;     
}

uint_fast64_t HashStorage::get_total_samples(uint_fast64_t state, uint_fast64_t action) {
        if(data.find(state) == data.end()) {
              return -1;  
        } 

        auto* act_map = &data.at(state);
        if(act_map->find(action) == act_map->end()) {
                return -1;
        }
        return act_map->at(action).first;
}


uint_fast64_t HashStorage::get_succ_action_samples(uint_fast64_t state, uint_fast64_t action, uint_fast64_t succ) {
        if(data.find(state) == data.end()) {
              return -1;  
        } 

        auto* act_map = &data.at(state);
        if(act_map->find(action) == act_map->end()) {
                return -1;
        }

        auto* sample_map = &(act_map->at(action).second);
        if(sample_map->find(succ) == sample_map->end()) {
                return -1;
        }
        //TODO
}
 
int main(int argc, char const *argv[])
{       
        HashStorage x = HashStorage();
        x.add_trans(1,2,3,6);
        x.add_trans(1,2,9,10);
        x.add_trans(1,3,3,6);
        x.add_trans(1,4,3,6);
        std::cout << x.get_total_samples(1,2) << "\n";
        x.add_trans(1,2,5,7);
        std::cout << x.get_total_samples(1,2) << "\n";
        for (auto x : x.get_state_actions(1))
                std::cout << "Act: " << x << "\n";
        
}
