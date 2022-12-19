#include "storm-synthesis/pomdp/PomdpManagerAposteriori.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/storage/sparse/ModelComponents.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
    namespace synthesis {

            
            template<typename ValueType>
            PomdpManagerAposteriori<ValueType>::PomdpManagerAposteriori(storm::models::sparse::Pomdp<ValueType> const& pomdp)
            : pomdp(pomdp) {
                
                auto num_observations = this->pomdp.getNrObservations();
                auto const& tm = this->pomdp.getTransitionMatrix();
                auto const& row_group_indices = tm.getRowGroupIndices();

                this->observation_actions.resize(num_observations,0);
                for(uint64_t state = 0; state < this->pomdp.getNumberOfStates(); state++) {
                    auto obs = this->pomdp.getObservation(state);
                    if(this->observation_actions[obs] == 0) {
                        this->observation_actions[obs] = tm.getRowGroupSize(state);
                    }
                }

                this->row_posteriors.resize(tm.getRowCount());
                this->observation_posteriors.resize(num_observations);
                for(uint64_t state = 0; state < this->pomdp.getNumberOfStates(); state++) {
                    auto prior = this->pomdp.getObservation(state);
                    for(auto row = row_group_indices[state]; row < row_group_indices[state+1]; row++) {
                        std::set<uint64_t> posteriors;
                        for(auto const &entry: tm.getRow(row)) {
                            auto successor_state = entry.getColumn();
                            auto posterior = this->pomdp.getObservation(successor_state);
                            posteriors.insert(posterior);
                            this->observation_posteriors[prior].insert(posterior);
                        }
                        this->row_posteriors[row] = std::vector<uint64_t>(posteriors.begin(),posteriors.end());
                    }
                }

                this->observation_memory_size.resize(num_observations, 1);
            }
            
            
            template<typename ValueType>
            void PomdpManagerAposteriori<ValueType>::setObservationMemorySize(uint64_t obs, uint64_t memory_size) {
                assert(obs < this->pomdp.getNrObservations());
                this->observation_memory_size[obs] = memory_size;
            }

            template<typename ValueType>
            void PomdpManagerAposteriori<ValueType>::setGlobalMemorySize(uint64_t memory_size) {
                for(uint64_t obs = 0; obs < this->pomdp.getNrObservations(); obs++) {
                    this->observation_memory_size[obs] = memory_size;
                }
            }




            template<typename ValueType>
            void PomdpManagerAposteriori<ValueType>::clear_before_unfolding() {
                
                this->num_states = 0;
                this->prototype_state_copies.clear();

                this->action_holes.clear();
                this->update_holes.clear();
                this->hole_num_options.clear();

                this->row_prototype.clear();
                this->coloring.clear();
            }

            template<typename ValueType>
            void PomdpManagerAposteriori<ValueType>::clear_after_unfolding() {
                
                this->num_states = 0;
                this->prototype_state_copies.clear();

                this->row_prototype.clear();
            }


            template<typename ValueType>
            void PomdpManagerAposteriori<ValueType>::buildStateSpace() {
                this->prototype_state_copies.resize(this->pomdp.getNumberOfStates());
                for(uint64_t prototype = 0; prototype < this->pomdp.getNumberOfStates(); prototype++) {
                    auto obs = this->pomdp.getObservation(prototype);
                    auto memory_size = this->observation_memory_size[obs];
                    this->prototype_state_copies[prototype].resize(memory_size);
                    for(uint64_t memory = 0; memory < memory_size; memory++) {
                        this->prototype_state_copies[prototype][memory] = this->num_states++;
                    }
                }
            }


            template<typename ValueType>
            void PomdpManagerAposteriori<ValueType>::buildDesignSpace() {
                uint64_t num_holes = 0;
                for(uint64_t prior = 0; prior < this->pomdp.getNrObservations(); prior++) {
                    auto num_actions = this->observation_actions[prior];
                    for(uint64_t mem = 0; mem < this->observation_memory_size[prior]; mem++) {
                        // gamma(n,z) in Act
                        this->action_holes[std::make_pair(mem,prior)] = num_holes++;
                        this->hole_num_options.push_back(num_actions);

                        for(auto posterior: this->observation_posteriors[prior]) {
                            // delta(n,z,z') in mu(z')
                            auto num_updates = this->observation_memory_size[posterior];
                            this->update_holes[std::make_tuple(mem,prior,posterior)] = num_holes++;
                            this->hole_num_options.push_back(num_updates);
                        }
                    }
                }
            }


            template<typename ValueType>
            storm::models::sparse::StateLabeling PomdpManagerAposteriori<ValueType>::constructStateLabeling() {
                storm::models::sparse::StateLabeling labeling(this->num_states);
                for (auto const& label : this->pomdp.getStateLabeling().getLabels()) {
                    storm::storage::BitVector label_flags(this->num_states, false);
                    for (auto const& prototype : this->pomdp.getStateLabeling().getStates(label)) {
                        for(auto state: this->prototype_state_copies[prototype]) {
                            label_flags.set(state);
                            if (label == "init") {
                                break;
                            }
                        }
                    }
                    labeling.addLabel(label, std::move(label_flags));
                }
                return labeling;
            }

            template<typename ValueType>
            void PomdpManagerAposteriori<ValueType>::unfoldRow(
                storm::storage::SparseMatrixBuilder<ValueType> & builder,
                uint64_t pomdp_state, uint64_t memory, uint64_t action
            ) {
                auto prior = this->pomdp.getObservation(pomdp_state);
                auto prototype_row = this->pomdp.getTransitionMatrix().getRowGroupIndices()[pomdp_state] + action;
                auto const& posteriors = this->row_posteriors[prototype_row];
                auto action_hole = this->action_holes[std::make_pair(memory,prior)];
                
                // iterate over all combinations of memory updates
                uint64_t num_combinations = 1;
                for(auto posterior: posteriors) {
                    num_combinations *= this->observation_memory_size[posterior];
                }
                std::map<uint64_t,uint64_t> combination;
                for(uint64_t c=0 ; c<num_combinations; c++) {
                    uint64_t index = c;
                    for(int64_t i=posteriors.size()-1; i>=0; i--) {
                        auto posterior = posteriors[i];
                        auto posterior_size = this->observation_memory_size[posterior];
                        combination[posterior] = index % posterior_size;
                        index = index / posterior_size;
                    }

                    // add row
                    for(auto const &entry: this->pomdp.getTransitionMatrix().getRow(prototype_row)) {
                        auto successor_pomdp_state = entry.getColumn();
                        auto posterior = this->pomdp.getObservation(successor_pomdp_state);
                        auto successor_memory = combination[posterior];
                        auto successor_state = this->prototype_state_copies[successor_pomdp_state][successor_memory];
                        builder.addNextValue(this->num_unfolded_rows(),successor_state,entry.getValue());
                    }

                    // add row coloring
                    std::map<uint64_t,uint64_t> coloring;
                    coloring[action_hole] = action;
                    for(uint64_t index = 0; index < posteriors.size(); index++) {
                        auto posterior = posteriors[index];
                        auto update_hole = this->update_holes[std::make_tuple(memory,prior,posterior)];
                        coloring[update_hole] = combination[posterior];
                    }
                    this->coloring.push_back(std::move(coloring));

                    // register prototype
                    this->row_prototype.push_back(prototype_row);
                }
            }


            template<typename ValueType>
            storm::storage::SparseMatrix<ValueType> PomdpManagerAposteriori<ValueType>::constructTransitionMatrix() {

                storm::storage::SparseMatrixBuilder<ValueType> builder(
                    0, this->num_states, 0, true, true, this->num_states
                );
                for(uint64_t pomdp_state = 0; pomdp_state < this->pomdp.getNumberOfStates(); pomdp_state++) {
                    auto prior = this->pomdp.getObservation(pomdp_state);
                    for(uint64_t memory = 0; memory < this->observation_memory_size[prior]; memory++) {
                        builder.newRowGroup(this->num_unfolded_rows());
                        for(uint64_t action = 0; action < this->pomdp.getTransitionMatrix().getRowGroupSize(pomdp_state); action++) {
                            this->unfoldRow(builder,pomdp_state,memory,action);
                        }
                    }
                }
                return builder.build();
            }


            template<typename ValueType>
            storm::models::sparse::StandardRewardModel<ValueType> PomdpManagerAposteriori<ValueType>::constructRewardModel(
                storm::models::sparse::StandardRewardModel<ValueType> const& reward_model
            ) {
                boost::optional<std::vector<ValueType>> state_rewards, action_rewards;
                STORM_LOG_THROW(!reward_model.hasStateRewards(), storm::exceptions::NotSupportedException, "state rewards are currently not supported.");
                STORM_LOG_THROW(!reward_model.hasTransitionRewards(), storm::exceptions::NotSupportedException, "transition rewards are currently not supported.");
                
                action_rewards = std::vector<ValueType>();
                for(uint64_t row = 0; row < this->row_prototype.size(); row++) {
                    auto prototype = this->row_prototype[row];
                    auto reward = reward_model.getStateActionReward(prototype);
                    action_rewards->push_back(reward);
                }
                return storm::models::sparse::StandardRewardModel<ValueType>(std::move(state_rewards), std::move(action_rewards));
            }
            
            template<typename ValueType>
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> PomdpManagerAposteriori<ValueType>::constructMdp() {
                
                this->clear_before_unfolding();
                
                this->buildStateSpace();
                this->buildDesignSpace();

                storm::storage::sparse::ModelComponents<ValueType> components;
                components.transitionMatrix = this->constructTransitionMatrix();
                assert(components.transitionMatrix.isProbabilistic());
                components.stateLabeling = this->constructStateLabeling();
                for (auto const& reward_model : this->pomdp.getRewardModels()) {
                    auto constructed = this->constructRewardModel(reward_model.second);
                    components.rewardModels.emplace(reward_model.first, constructed);
                }
                
                this->mdp = std::make_shared<storm::models::sparse::Mdp<ValueType>>(std::move(components));

                this->clear_after_unfolding();

                return this->mdp;

            }

            template class PomdpManagerAposteriori<double>;
    }
}