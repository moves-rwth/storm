// author: Roman Andriushchenko

#include "storm-synthesis/decpomdp/DecPomdp.h"

#include "storm-synthesis/decpomdp/madp/src/base/Globals.h"
#include "storm-synthesis/decpomdp/madp/src/base/E.h"
#include "storm-synthesis/decpomdp/madp/src/parser/MADPParser.h"

#include <stack>

namespace storm {
    namespace synthesis {

    
        uint_fast64_t DecPomdp::fresh_joint_action(std::string action_label) {
            std::vector<uint_fast64_t> action_tuple(this->num_agents);
            for(uint_fast64_t agent = 0; agent < this->num_agents; agent++) {
                action_tuple[agent] = this->agent_num_actions(agent);
                this->agent_observation_labels[agent].push_back(action_label);
            }
            uint_fast64_t joint_action = this->num_joint_actions();
            this->joint_actions.push_back(std::move(action_tuple));
            return joint_action;
        }

        
        void DecPomdp::collect_actions(DecPOMDPDiscrete *model) {
            
            // individual actions
            this->agent_action_labels.resize(this->num_agents);
            for(uint_fast64_t agent = 0; agent < this->num_agents; agent++) {
                uint_fast64_t num_actions = model->GetNrActions(agent);
                this->agent_action_labels[agent].resize(num_actions);
                std::vector<std::string> action_labels(num_actions);
                for(uint_fast64_t action = 0; action < num_actions; action++) {
                    this->agent_action_labels[agent][action] = model->GetAction(agent,action)->GetName();
                }
            }

            // joint actions
            this->joint_actions.resize(model->GetNrJointActions());
            for(uint_fast64_t joint_action_index = 0; joint_action_index < model->GetNrJointActions(); joint_action_index++) {
                for(auto action: model->JointToIndividualActionIndices(joint_action_index)) {
                    this->joint_actions[joint_action_index].push_back(action);
                }
            }
        }

        
        uint_fast64_t DecPomdp::fresh_joint_observation(std::string observation_label) {
            std::vector<uint_fast64_t> observation_tuple(this->num_agents);
            for(uint_fast64_t agent = 0; agent < this->num_agents; agent++) {
                observation_tuple[agent] = this->agent_num_observations(agent);
                this->agent_observation_labels[agent].push_back(observation_label);
            }
            uint_fast64_t joint_observation = this->num_joint_observations();
            this->joint_observations.push_back(std::move(observation_tuple));
            return joint_observation;
        }


        void DecPomdp::collect_observations(DecPOMDPDiscrete *model) {
            
            // individual observations
            this->agent_observation_labels.resize(this->num_agents);
            for(uint_fast64_t agent = 0; agent < this->num_agents; agent++) {
                for(uint_fast64_t obs = 0; obs < model->GetNrObservations(agent); obs++) {
                    this->agent_observation_labels[agent].push_back(model->GetObservation(agent,obs)->GetName());
                }
            }

            // joint observations
            uint_fast64_t num_joint_observations = model->GetNrJointObservations();
            this->joint_observations.resize(num_joint_observations);
            for(uint_fast64_t joint_observation_index = 0; joint_observation_index < num_joint_observations; joint_observation_index++) {
                for(auto observation: model->JointToIndividualObservationIndices(joint_observation_index)) {
                    this->joint_observations[joint_observation_index].push_back(observation);
                }
            }
        }
        
        
        bool DecPomdp::have_madp_state(MadpState madp_state) {
            return this->madp_to_storm_states.find(madp_state) != this->madp_to_storm_states.end();
        }
            
        
        uint_fast64_t DecPomdp::map_madp_state(MadpState madp_state) {
            uint_fast64_t new_state = this->num_states();
            auto const result = this->madp_to_storm_states.insert(std::make_pair(madp_state, new_state));
            if (result.second) {
                this->storm_to_madp_states.push_back(madp_state);
                this->storm_transition_matrix.resize(this->num_states());
                this->row_joint_action.resize(this->num_states());
            }
            return result.first->second;
        }


        DecPomdp::DecPomdp(DecPOMDPDiscrete *model) {
            
            // agents
            this->num_agents = model->GetNrAgents();
            this->discount = model->GetDiscount();
            this->reward_minimizing = model->GetRewardType() == COST;

            this->collect_actions(model);
            this->collect_observations(model);

            // multiply transition and observation probabilities
            std::vector<std::vector<std::vector<std::pair<MadpState,double>>>> madp_transition_matrix;
            for(uint_fast64_t src = 0; src < model->GetNrStates(); src++) {
                std::vector<std::vector<std::pair<MadpState,double>>> row_group;
                
                for(uint_fast64_t joint_action = 0; joint_action < model->GetNrJointActions(); joint_action++) {
                    std::vector<std::pair<MadpState,double>> row;
                    
                    for(uint_fast64_t dst = 0; dst < model->GetNrStates(); dst++) {
                        double transition_prob = model->GetTransitionProbability(src, joint_action, dst);
                        if(transition_prob == 0) {
                            continue;
                        }
                        
                        for(uint_fast64_t obs = 0; obs < model->GetNrJointObservations(); obs++) {
                            double observation_prob = model->GetObservationProbability(joint_action, dst, obs);
                            if(observation_prob == 0) {
                                continue;
                            }
                            row.push_back(std::make_pair(std::make_pair(dst,obs), transition_prob*observation_prob));
                        }
                    }
                    row_group.push_back(row);
                }
                madp_transition_matrix.push_back(row_group);
            }

            // TODO collect rewards/costs

            // create initial observation for the (unique) initial state
            uint_fast64_t init_joint_observation = this->fresh_joint_observation("init");
            // create action that corresponds to the execution of the initial distribution
            uint_fast64_t init_joint_action = this->fresh_joint_action("init");
            // create empty observation for states in the initial distribution
            uint_fast64_t empty_joint_observation = this->fresh_joint_observation("");

            // collect initial distribution
            std::vector<MadpRow> initial_distribution_row_group(1);
            uint_fast64_t state = 0;
            for(auto prob: model->GetISD()->ToVectorOfDoubles()) {
                if(prob > 0) {
                    initial_distribution_row_group[0].push_back(std::make_pair(std::make_pair(state,empty_joint_observation),prob));
                }
                state++;
            }
            
            // explore the reachable state space from the initial state
            uint_fast64_t initial_state = model->GetNrStates();
            std::stack<MadpState> reachable_states;
            MadpState madp_initial = std::make_pair(initial_state,init_joint_observation);
            uint_fast64_t storm_initial = this->map_madp_state(madp_initial);
            reachable_states.push(madp_initial);
            while(!reachable_states.empty()) {
                MadpState madp_src = reachable_states.top();
                reachable_states.pop();
                uint_fast64_t storm_src = this->map_madp_state(madp_src);
                
                std::vector<std::vector<std::pair<MadpState,double>>> *row_group;
                if(madp_src.first == initial_state) {
                    row_group = &initial_distribution_row_group;
                } else {
                    row_group = &madp_transition_matrix[madp_src.first];
                }

                std::vector<StormRow> storm_row_group;
                for(auto &row : *row_group) {
                    StormRow storm_row;
                    for(auto &madp_state_prob: row) {
                        MadpState madp_dst = madp_state_prob.first;
                        if(!this->have_madp_state(madp_dst)) {
                            reachable_states.push(madp_dst);
                        }
                        uint_fast64_t storm_dst = this->map_madp_state(madp_dst);
                        storm_row.push_back(std::make_pair(storm_dst, madp_state_prob.second));
                    }
                    storm_row_group.push_back(std::move(storm_row));
                }
                this->storm_transition_matrix[storm_src] = std::move(storm_row_group);
            }

            // map rows to joint actions
            std::vector<uint_fast64_t> madp_row_group;
            for(uint_fast64_t joint_action = 0; joint_action < model->GetNrJointActions(); joint_action++) {
                madp_row_group.push_back(joint_action);
            }
            this->row_joint_action.resize(this->num_states());
            for(uint_fast64_t storm_state = 0; storm_state < this->num_states(); storm_state++) {
                if(storm_state == storm_initial) {
                    std::vector<uint_fast64_t> initial_row_group;
                    initial_row_group.push_back(init_joint_action);
                    this->row_joint_action[storm_state] = initial_row_group;
                } else {
                    this->row_joint_action[storm_state] = madp_row_group;
                }
            }
            
            // map states to joint observations
            this->state_joint_observation.resize(this->num_states());
            for(uint_fast64_t state = 0; state < this->num_states(); state++) {
                MadpState madp_state = this->storm_to_madp_states[state];
                this->state_joint_observation[state] = madp_state.second;
            }


        }

        POMDPDiscrete *parse_as_pomdp(std::string filename) {
            try {
                POMDPDiscrete *model = new POMDPDiscrete("","",filename);
                model->SetSparse(true);
                MADPParser parser(model);
                return model;
            } catch(E& e) {
                e.Print();
                return NULL;
            }
        }

        DecPOMDPDiscrete *parse_as_decpomdp(std::string filename) {
            try {
                DecPOMDPDiscrete *model = new DecPOMDPDiscrete("","",filename);
                model->SetSparse(true);
                MADPParser parser(model);
                return model;
            } catch(E& e) {
                e.Print();
                return NULL;
            }
        }

        DecPOMDPDiscrete *parseMadp(std::string filename) {
            
            DecPOMDPDiscrete *model;
            
            std::cerr << "MADP: trying to parse as POMDP..." << std::endl;
            model = parse_as_pomdp(filename);
            if(model != NULL) {
                return model;
            }

            std::cerr << "MADP: trying to parse as dec-POMDP..." << std::endl;
            model = parse_as_decpomdp(filename);
            if(model != NULL) {
                return model;
            }

            if(model == NULL) {
                std::cerr << "MADP: parsing failed" << std::endl;
            }
            return model;
            
        }

        std::unique_ptr<DecPomdp> parseDecPomdp(std::string filename) {
            DecPOMDPDiscrete *madp_decpomdp = parseMadp(filename);
            if(madp_decpomdp == NULL) {
                return NULL;
            }
            // debug: MADP info
            std::cerr << madp_decpomdp->SoftPrint() << std::endl;
            std::unique_ptr<DecPomdp> decpomdp = std::make_unique<DecPomdp>(madp_decpomdp);
            free(madp_decpomdp);
            return decpomdp;
        }

    } // namespace synthesis
} // namespace storm

