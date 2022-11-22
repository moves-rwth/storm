#include "storm-synthesis/pomdp/SubPomdpBuilder.h"

#include "storm/exceptions/InvalidArgumentException.h"

#include "storm/storage/sparse/ModelComponents.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
    namespace synthesis {

        
        SubPomdpBuilder::SubPomdpBuilder(
            storm::models::sparse::Pomdp<double> const& pomdp,
            std::string const& reward_name,
            std::string const& target_label
        )
            : pomdp(pomdp), reward_name(reward_name), target_label(target_label) {
            
            STORM_LOG_THROW(pomdp.isCanonic(), storm::exceptions::InvalidArgumentException, "POMDP must be canonic");

            auto const& tm = pomdp.getTransitionMatrix();
            reachable_successors.resize(pomdp.getNumberOfStates());
            for(uint64_t state = 0; state < pomdp.getNumberOfStates(); state++) {
                reachable_successors[state] = std::set<uint64_t>();
                for(auto const& entry: tm.getRowGroup(state)) {
                    if(entry.getColumn() != state) {
                        reachable_successors[state].insert(entry.getColumn());
                    }
                }
            }

        }

        void SubPomdpBuilder::setRelevantStates(storm::storage::BitVector const& relevant_states) {
            this->relevant_states = relevant_states;

            // compute horizon states
            this->horizon_states = storm::storage::BitVector(this->pomdp.getNumberOfStates(),false);
            for(auto state: relevant_states) {
                for(uint64_t successor: this->reachable_successors[state]) {
                    if(!relevant_states[successor]) {
                        this->horizon_states.set(successor,true);
                    }
                }
            }

            this->num_states = 2 + this->relevant_states.getNumberOfSetBits() + this->horizon_states.getNumberOfSetBits();

            // create state map
            this->state_map = std::vector<uint64_t>(this->pomdp.getNumberOfStates(),0);
            // indices 0 and 1 are reserved for the initial and the sink state respectively
            uint64_t state_index = 2;
            for(auto state: relevant_states) {
                this->state_map[state] = state_index++;
            }
            for(auto state: horizon_states) {
                this->state_map[state] = state_index++;
            }
        }

        
        storm::storage::BitVector const& SubPomdpBuilder::getHorizonStates() {
            return this->horizon_states;
        }


        
        storm::storage::SparseMatrix<double> SubPomdpBuilder::constructTransitionMatrix(
            std::map<uint64_t,double> const& initial_belief
        ) {
            // num_rows = initial state + sink state + 1 for each horizon state + rows of relevant states
            uint64_t num_rows = 1+1+this->horizon_states.getNumberOfSetBits();
            for(auto state: this->relevant_states) {
                num_rows += this->pomdp.getTransitionMatrix().getRowGroupSize(state);
            }

            // building the transition matrix
            storm::storage::SparseMatrixBuilder<double> builder(
                    num_rows, this->num_states, 0, true, true, this->num_states
            );
            uint64_t current_row = 0;

            // initial state distribution
            builder.newRowGroup(current_row);
            for(const auto &entry : initial_belief) {
                auto dst = this->state_map[entry.first];
                builder.addNextValue(current_row, dst, entry.second);
            }
            current_row++;

            // sink state self-loop
            builder.newRowGroup(current_row);
            builder.addNextValue(current_row, current_row, 1);
            current_row++;

            // relevant states
            auto const& tm = pomdp.getTransitionMatrix();
            auto const& row_groups = tm.getRowGroupIndices();
            for(auto state: this->relevant_states) {
                builder.newRowGroup(current_row);
                for(uint64_t row = row_groups[state]; row < row_groups[state+1]; row++) {
                    for(auto const& entry: tm.getRow(row)) {
                        auto dst = this->state_map[entry.getColumn()];
                        builder.addNextValue(current_row, dst, entry.getValue());
                    }
                    current_row++;
                }
            }

            // horizon states
            for(const auto state: this->horizon_states) {
                (void) state;
                builder.newRowGroup(current_row);
                builder.addNextValue(current_row, this->sink_state, 1);
                current_row++;
            }

            // transition matrix finalized
            return builder.build();
        }

        storm::models::sparse::StateLabeling SubPomdpBuilder::constructStateLabeling() {
            storm::models::sparse::StateLabeling labeling(this->num_states);
            storm::storage::BitVector label_init(this->num_states, false);
            label_init.set(this->initial_state);
            labeling.addLabel("init", std::move(label_init));

            std::cout << "(1) ok" << std::endl;
            
            storm::storage::BitVector label_target(this->num_states, false);
            std::cout << "(2) ok" << std::endl;
            auto const& pomdp_labeling = this->pomdp.getStateLabeling();
            std::cout << "(2.1) ok" << std::endl;
            auto const& pomdp_target_states = pomdp_labeling.getStates(this->target_label);
            std::cout << "(3) ok" << std::endl;
            for(auto state: pomdp_target_states) {
                label_target.set(this->state_map[state]);
            }
            std::cout << "(4) ok" << std::endl;
            label_target.set(this->sink_state);
            labeling.addLabel(this->target_label, std::move(label_target));
            
            std::cout << "(5) ok" << std::endl;

            return labeling;
        }

        storm::models::sparse::ChoiceLabeling SubPomdpBuilder::constructChoiceLabeling(uint64_t num_rows) {
            storm::models::sparse::ChoiceLabeling labeling(num_rows);
            auto const& pomdp_labeling = this->pomdp.getChoiceLabeling();
            labeling.addLabel(this->empty_label, storm::storage::BitVector(num_rows,false));
            for (auto const& label : pomdp_labeling.getLabels()) {
                labeling.addLabel(label, storm::storage::BitVector(num_rows,false));
            }
            uint64_t current_row = 0;

            // initial state, sink state
            labeling.addLabelToChoice(this->empty_label, current_row++);
            labeling.addLabelToChoice(this->empty_label, current_row++);

            // relevant states
            auto const& tm = this->pomdp.getTransitionMatrix();
            auto const& row_groups = tm.getRowGroupIndices();
            for(auto state: this->relevant_states) {
                for(uint64_t row = row_groups[state]; row < row_groups[state+1]; row++) {
                    for(auto label: pomdp_labeling.getLabelsOfChoice(row)) {
                        labeling.addLabelToChoice(label, current_row);
                    }
                    current_row++;
                }
            }

            // horizon states
            for(const auto state: this->horizon_states) {
                labeling.addLabelToChoice(this->empty_label, current_row++);
            }

            return labeling;
        }

        std::vector<uint32_t> SubPomdpBuilder::constructObservabilityClasses() {
            std::vector<uint32_t> observation_classes(this->num_states);
            uint32_t fresh_observation = this->pomdp.getNrObservations();
            observation_classes[this->initial_state] = fresh_observation;
            observation_classes[this->sink_state] = fresh_observation;
            for(auto state: this->relevant_states) {
                observation_classes[this->state_map[state]] = this->pomdp.getObservation(state);
            }
            for(auto state: this->horizon_states) {
                observation_classes[this->state_map[state]] = fresh_observation;
            }
            return observation_classes;
        }

        storm::models::sparse::StandardRewardModel<double> SubPomdpBuilder::constructRewardModel(
            uint64_t num_rows,
            std::map<uint64_t,double> const& horizon_values
        ) {
            auto const& reward_model = this->pomdp.getRewardModel(this->reward_name);
            boost::optional<std::vector<double>> state_rewards;
            std::vector<double> action_rewards(num_rows,0);

            uint64_t current_row = 0;

            // skip initial state, sink state
            current_row += 2;

            // relevant states
            auto const& row_groups = this->pomdp.getTransitionMatrix().getRowGroupIndices();
            for(auto state: this->relevant_states) {
                for(uint64_t row = row_groups[state]; row < row_groups[state+1]; row++) {
                    action_rewards[current_row] = reward_model.getStateActionReward(row);
                    current_row++;
                }
            }

            // horizon states
            for(const auto state: this->horizon_states) {
                action_rewards[current_row] = horizon_values.find(state)->second;
                current_row++;
            }

            return storm::models::sparse::StandardRewardModel<double>(std::move(state_rewards), std::move(action_rewards));
        }

        std::shared_ptr<storm::models::sparse::Pomdp<double>> SubPomdpBuilder::restrictPomdp(
            std::map<uint64_t,double> const& initial_belief,
            std::map<uint64_t,double> const& horizon_values
        ) {
            storm::storage::sparse::ModelComponents<double> components;
            std::cout << "transition matrix" << std::endl;
            components.transitionMatrix = this->constructTransitionMatrix(initial_belief);
            uint64_t num_rows = components.transitionMatrix.getRowCount();
            std::cout << "state labeling" << std::endl;
            components.stateLabeling = this->constructStateLabeling();
            std::cout << "choice labeling" << std::endl;
            components.choiceLabeling = this->constructChoiceLabeling(num_rows);
            std::cout << "observability classes" << std::endl;
            components.observabilityClasses = this->constructObservabilityClasses();
            std::cout << "reward model" << std::endl;
            components.rewardModels.emplace(this->reward_name, this->constructRewardModel(num_rows,horizon_values));
            std::cout << "done" << std::endl;
            return std::make_shared<storm::models::sparse::Pomdp<double>>(std::move(components));
        }

    }
}