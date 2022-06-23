#include "storm-synthesis/pomdp/PomdpManager.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/storage/sparse/ModelComponents.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
    namespace synthesis {

            
            template<typename ValueType>
            PomdpManager<ValueType>::PomdpManager(storm::models::sparse::Pomdp<ValueType> const& pomdp)
            : pomdp(pomdp) {
                STORM_LOG_THROW(pomdp.isCanonic(), storm::exceptions::InvalidArgumentException, "POMDP must be canonic");

                auto num_prototype_states = pomdp.getNumberOfStates();
                auto num_prototype_rows = pomdp.getNumberOfChoices();
                auto num_observations = pomdp.getNrObservations();
                this->observation_actions.resize(num_observations,0);
                this->observation_successors.resize(num_observations);
                this->prototype_row_index.resize(num_prototype_rows,0);

                std::vector<std::set<uint64_t>> observation_successor_sets;
                observation_successor_sets.resize(num_observations);
                
                for(uint64_t prototype_state = 0; prototype_state < num_prototype_states; prototype_state++) {
                    auto observation = pomdp.getObservation(prototype_state);
                    
                    auto const& row_group_indices = pomdp.getTransitionMatrix().getRowGroupIndices();
                    uint64_t row_index = 0;
                    for (
                        uint64_t prototype_row = row_group_indices[prototype_state];
                        prototype_row < row_group_indices[prototype_state + 1];
                        prototype_row++
                    ) {
                        this->prototype_row_index[prototype_row] = row_index;
                        row_index++;

                        for(auto const &entry: this->pomdp.getTransitionMatrix().getRow(prototype_row)) {
                            auto dst = entry.getColumn();
                            auto dst_obs = this->pomdp.getObservation(dst);
                            observation_successor_sets[observation].insert(dst_obs);
                        } 
                    }
                    
                    if(this->observation_actions[observation] != 0) {
                        continue;
                    }
                    this->observation_actions[observation] = pomdp.getTransitionMatrix().getRowGroupSize(prototype_state);
                }
                for(uint64_t obs = 0; obs < num_observations; obs++) {
                    this->observation_successors[obs] = std::vector<uint64_t>(
                        observation_successor_sets[obs].begin(),
                        observation_successor_sets[obs].end()
                        );
                }


                this->observation_memory_size.resize(num_observations, 1);
                this->prototype_duplicates.resize(num_prototype_states);
                
                this->max_successor_memory_size.resize(num_observations);
            }

 
            template<typename ValueType>
            void PomdpManager<ValueType>::buildStateSpace() {
                this->num_states = 0;
                this->state_prototype.clear();
                this->state_memory.clear();
                for(uint64_t prototype = 0; prototype < this->pomdp.getNumberOfStates(); prototype++) {
                    auto obs = this->pomdp.getObservation(prototype);
                    auto memory_size = this->observation_memory_size[obs];
                    this->prototype_duplicates[prototype].clear();
                    this->prototype_duplicates[prototype].reserve(memory_size);
                    for(uint64_t memory = 0; memory < memory_size; memory++) {
                        this->prototype_duplicates[prototype].push_back(this->num_states);
                        this->state_prototype.push_back(prototype);
                        this->state_memory.push_back(memory);
                        this->num_states++;
                    }
                }
            }

 
            template<typename ValueType>
            uint64_t PomdpManager<ValueType>::translateState(uint64_t prototype, uint64_t memory) {
                if(memory >= this->prototype_duplicates[prototype].size()) {
                    memory = 0;
                }
                return this->prototype_duplicates[prototype][memory];
            }

 
            template<typename ValueType>
            void PomdpManager<ValueType>::buildTransitionMatrixSpurious() {
                // for each observation, define the maximum successor memory size
                // this will define the number of copies we need to make of each row
                for(uint64_t obs = 0; obs < this->pomdp.getNrObservations(); obs++) {
                    uint64_t max_mem_size = 0;
                    for(auto dst_obs: this->observation_successors[obs]) {
                        if(max_mem_size < this->observation_memory_size[dst_obs]) {
                            max_mem_size = this->observation_memory_size[dst_obs];
                        }
                    }
                    this->max_successor_memory_size[obs] = max_mem_size;
                }

                this->row_groups.resize(this->num_states+1);
                this->row_prototype.clear();
                this->row_memory.clear();
                
                // TODO can simplify this: state (s,x) will have the same rows as state (s,0)
                for(uint64_t state = 0; state < this->num_states; state++) {
                    this->row_groups[state] = this->row_prototype.size();
                    auto prototype_state = this->state_prototype[state];
                    auto observation = this->pomdp.getObservation(prototype_state);
                    auto const& row_group_indices = this->pomdp.getTransitionMatrix().getRowGroupIndices();
                    for (
                        uint64_t prototype_row = row_group_indices[prototype_state];
                        prototype_row < row_group_indices[prototype_state + 1];
                        prototype_row++
                    ) {
                        // create the required number of copies of this row
                        // each transition will be associated with its own memory update
                        for(uint64_t dst_mem = 0; dst_mem < max_successor_memory_size[observation]; dst_mem++) {
                            this->row_prototype.push_back(prototype_row);
                            this->row_memory.push_back(dst_mem);
                        }
                    }
                }
                this->num_rows = this->row_prototype.size();
                this->row_groups[this->num_states] = this->num_rows;
            }


            template<typename ValueType>
            void PomdpManager<ValueType>::resetDesignSpace() {
                auto num_observations = this->pomdp.getNrObservations();
                this->num_holes = 0;
                this->action_holes.clear();
                this->action_holes.resize(num_observations);
                this->memory_holes.clear();
                this->memory_holes.resize(num_observations);
                this->hole_options.clear();

                this->row_action_hole.clear();
                this->row_action_hole.resize(this->num_rows);
                this->row_action_option.clear();
                this->row_action_option.resize(this->num_rows);
                this->row_memory_hole.clear();
                this->row_memory_hole.resize(this->num_rows);
                this->row_memory_option.clear();
                this->row_memory_option.resize(this->num_rows);
            }


            template<typename ValueType>
            void PomdpManager<ValueType>::buildDesignSpaceSpurious() {
                this->resetDesignSpace();
                
                // for each (z,n) create an action and a memory hole (if necessary)
                // store hole range
                // ? inverse mapping ?
                for(uint64_t obs = 0; obs < this->pomdp.getNrObservations(); obs++) {
                    if(this->observation_actions[obs] > 1) {
                        for(uint64_t mem = 0; mem < this->observation_memory_size[obs]; mem++) {
                            this->action_holes[obs].push_back(this->num_holes);
                            this->hole_options.push_back(this->observation_actions[obs]);
                            // std::cout << "created A(" << obs << "," << mem << ") = " << this->num_holes << " in {} of size " << this->observation_actions[obs] << std::endl;
                            this->num_holes++;
                        }
                    }
                    if(this->max_successor_memory_size[obs] > 1) {
                        for(uint64_t mem = 0; mem < this->observation_memory_size[obs]; mem++) {
                            this->memory_holes[obs].push_back(this->num_holes);
                            this->hole_options.push_back(this->max_successor_memory_size[obs]);
                            // std::cout << "created N(" << obs << "," << mem << ") = " << this->num_holes << " in {} of size " << this->max_successor_memory_size[obs] << std::endl;
                            this->num_holes++;
                        }
                    }
                }

                // map each row to some action (memory) hole (if applicable) and its value
                for(uint64_t state = 0; state < this->num_states; state++) {
                    auto prototype = this->state_prototype[state];
                    auto obs = this->pomdp.getObservation(prototype);
                    auto mem = this->state_memory[state];
                    for (uint64_t row = this->row_groups[state]; row < this->row_groups[state+1]; row++) {
                        auto prototype_row = this->row_prototype[row];
                        auto row_index = this->prototype_row_index[prototype_row];
                        auto row_mem = this->row_memory[row];
                        if(this->observation_actions[obs] > 1) {
                            // there is an action hole that corresponds to this state
                            auto action_hole = this->action_holes[obs][mem];
                            this->row_action_hole[row] = action_hole;
                            this->row_action_option[row] = row_index;
                        } else {
                            // no corresponding action hole
                            this->row_action_hole[row] = this->num_holes;
                        }
                        if(this->max_successor_memory_size[obs] > 1) {
                            // there is a memory hole that corresponds to this state
                            auto memory_hole = this->memory_holes[obs][mem];
                            this->row_memory_hole[row] = memory_hole;
                            this->row_memory_option[row] = row_mem;
                        } else {
                            this->row_memory_hole[row] = this->num_holes;
                        }
                        // std::cout << "row " << row << ": A[" << row_action_hole[row] << "]=" << row_action_option[row] << ", N[" << row_memory_hole[row] << "]=" << row_memory_option[row] << std::endl;
                    }   
                }
            }            

            
            template<typename ValueType>
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> PomdpManager<ValueType>::constructMdp() {
                this->buildStateSpace();
                this->buildTransitionMatrixSpurious();

                storm::storage::sparse::ModelComponents<ValueType> components;
                components.transitionMatrix = this->constructTransitionMatrix();
                // std::cout << "row groups: " << components.transitionMatrix.getRowGroupCount() << std::endl << std::flush;
                // std::cout << "rows: " << components.transitionMatrix.getRowCount() << std::endl << std::flush;
                // std::cout << "entries: " << components.transitionMatrix.getEntryCount() << std::endl << std::flush;
                // std::cout << "column: " << components.transitionMatrix.getColumnCount() << std::endl << std::flush;
                
                assert(components.transitionMatrix.isProbabilistic());
                components.stateLabeling = this->constructStateLabeling();
                // TODO remove unreachable states
                for (auto const& reward_model : pomdp.getRewardModels()) {
                    auto constructed = this->constructRewardModel(reward_model.second);
                    components.rewardModels.emplace(reward_model.first, constructed);
                }
                this->mdp = std::make_shared<storm::models::sparse::Mdp<ValueType>>(std::move(components));

                this->buildDesignSpaceSpurious();

                return this->mdp;
            }

            template<typename ValueType>
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> PomdpManager<ValueType>::constructMdpAposteriori() {
                // TODO
                return this->constructMdp();
            }

            template<typename ValueType>
            storm::models::sparse::StateLabeling PomdpManager<ValueType>::constructStateLabeling() {
                storm::models::sparse::StateLabeling labeling(this->num_states);
                for (auto const& label : pomdp.getStateLabeling().getLabels()) {
                    storm::storage::BitVector label_flags(this->num_states, false);
                    
                    if (label == "init") {
                        // init label is only assigned to states with the initial memory state
                        for (auto const& prototype : pomdp.getStateLabeling().getStates(label)) {
                            label_flags.set(translateState(prototype, 0));
                        }
                    } else {
                        for (auto const& prototype : pomdp.getStateLabeling().getStates(label)) {
                            for(auto duplicate: this->prototype_duplicates[prototype]) {
                                label_flags.set(duplicate);
                            }
                        }
                    }
                    labeling.addLabel(label, std::move(label_flags));
                }
                return labeling;
            }


            template<typename ValueType>
            storm::storage::SparseMatrix<ValueType> PomdpManager<ValueType>::constructTransitionMatrix() {
                storm::storage::SparseMatrixBuilder<ValueType> builder(
                    this->num_rows, this->num_states, 0, true, true, this->num_states
                );
                for(uint64_t state = 0; state < this->num_states; state++) {
                    builder.newRowGroup(this->row_groups[state]);
                    for (uint64_t row = this->row_groups[state]; row < this->row_groups[state+1]; row++) {
                        auto prototype_row = this->row_prototype[row];
                        auto dst_mem = this->row_memory[row];
                        for(auto const &entry: this->pomdp.getTransitionMatrix().getRow(prototype_row)) {
                            auto dst = this->translateState(entry.getColumn(),dst_mem);
                            builder.addNextValue(row, dst, entry.getValue());
                        }
                    }
                }

                return builder.build();
            }


            template<typename ValueType>
            storm::models::sparse::StandardRewardModel<ValueType> PomdpManager<ValueType>::constructRewardModel(
                storm::models::sparse::StandardRewardModel<ValueType> const& reward_model
                ) {
                boost::optional<std::vector<ValueType>> state_rewards, action_rewards;
                if (reward_model.hasStateRewards()) {
                    state_rewards = std::vector<ValueType>();
                    state_rewards->reserve(this->num_states);
                    for(uint64_t state = 0; state < this->num_states; state++) {
                        auto prototype = this->state_prototype[state];
                        auto reward = reward_model.getStateReward(prototype);
                        state_rewards->push_back(reward);
                    }
                }
                if (reward_model.hasStateActionRewards()) {
                    action_rewards = std::vector<ValueType>();
                    for(uint64_t row = 0; row < this->num_rows; row++) {
                        auto prototype = this->row_prototype[row];
                        auto reward = reward_model.getStateActionReward(prototype);
                        action_rewards->push_back(reward);
                    }
                }
                STORM_LOG_THROW(!reward_model.hasTransitionRewards(), storm::exceptions::NotSupportedException, "Transition rewards are currently not supported.");
                return storm::models::sparse::StandardRewardModel<ValueType>(std::move(state_rewards), std::move(action_rewards));
            }


            template<typename ValueType>
            void PomdpManager<ValueType>::setObservationMemorySize(uint64_t obs, uint64_t memory_size) {
                assert(obs < this->pomdp.getNrObservations());
                this->observation_memory_size[obs] = memory_size;
            }

            template<typename ValueType>
            void PomdpManager<ValueType>::setGlobalMemorySize(uint64_t memory_size) {
                for(uint64_t obs = 0; obs < this->pomdp.getNrObservations(); obs++) {
                    this->observation_memory_size[obs] = memory_size;
                }
            }


            template class PomdpManager<double>;

    }
}