// author: Roman Andriushchenko

#pragma once

#include "storm-synthesis/decpomdp/madp/src/base/POMDPDiscrete.h"
#include "storm-synthesis/decpomdp/madp/src/base/DecPOMDPDiscrete.h"

#include <string>

namespace storm {
    namespace synthesis {

        typedef std::pair<uint_fast64_t,uint_fast64_t> MadpState;
        typedef std::vector<std::pair<MadpState,double>> MadpRow;
        typedef std::vector<std::pair<uint_fast64_t,double>> StormRow;

        class DecPomdp {

        public:
            DecPomdp(DecPOMDPDiscrete *model);

            /** Number of agents. **/
            uint_fast64_t num_agents;
            
            /** For each agent, a list of its action labels. **/
            std::vector<std::vector<std::string>> agent_action_labels;
            /** A list of tuples of actions. **/
            std::vector<std::vector<uint_fast64_t>> joint_actions;


            /** For each agent, a list of its observation labels. */
            std::vector<std::vector<std::string>> agent_observation_labels;
            /** A list of tuples of observations. */
            std::vector<std::vector<uint_fast64_t>> joint_observations;

            /** The unique initial state. **/
            uint_fast64_t initial_state;
            /** Storm-esque transition matrix: for each state, a row group. */
            std::vector<std::vector<StormRow>> transition_matrix;
            /** For each state (row group), a mapping of a row to a joint action. */
            std::vector<std::vector<uint_fast64_t>> row_joint_action;
            /** State to joint observation map. */
            std::vector<uint_fast64_t> state_joint_observation;
            /** For each state (row group), a mapping of a row to its reward. */
            std::vector<std::vector<double>> row_reward;
            
            /** Discount factor. */
            double discount;
            /** Index of the sink state. */
            uint_fast64_t sink_state;
            /** If true, the rewards are interpreted as costs. */
            bool reward_minimizing;

            
            uint_fast64_t agent_num_actions(uint_fast64_t agent) {
                return this->agent_action_labels[agent].size();
            }
            uint_fast64_t num_joint_actions() {
                return this->joint_actions.size();
            }
            uint_fast64_t agent_num_observations(uint_fast64_t agent) {
                return this->agent_observation_labels[agent].size();
            }
            uint_fast64_t num_joint_observations() {
                return this->joint_observations.size();
            }

            uint_fast64_t num_states() {
                return this->storm_to_madp_states.size();
            }



        private:

            /** Madp to Storm state map. */
            std::map<MadpState, uint_fast64_t> madp_to_storm_states;
            /** Storm to Madp state map. */
            std::vector<MadpState> storm_to_madp_states;

            void collectActions(DecPOMDPDiscrete *model);
            void collectObservations(DecPOMDPDiscrete *model);
            
            uint_fast64_t freshJointAction(std::string action_label);
            uint_fast64_t freshJointObservation(std::string observation_label);

            bool haveMadpState(MadpState madp_state);
            uint_fast64_t mapMadpState(MadpState madp_state);

            void applyDiscountFactor();
        };

        
        /**
         * Parse MADP file and convert transition matrix as well as
         * probabilistic observations of the resulting dec-POMDP to a
         * Storm-friendly representation.
         * @return NULL on parsing error
         */
         std::unique_ptr<DecPomdp> parseDecPomdp(std::string filename);

    } // namespace synthesis
} // namespace storm

