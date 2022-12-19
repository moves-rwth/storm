#pragma once

#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/Pomdp.h"

#include <stack>


namespace storm {
    namespace synthesis {

        
        template<typename ValueType>
        class PomdpManagerAposteriori {

        public:
            
            PomdpManagerAposteriori(storm::models::sparse::Pomdp<ValueType> const& pomdp);

            // unfold memory model (a aposteriori memory update) into the POMDP
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> constructMdp();            

            // for each observation contains the number of allocated memory states (initially 1)
            std::vector<uint64_t> observation_memory_size;
            // set memory size to a selected observation
            void setObservationMemorySize(uint64_t obs, uint64_t memory_size);
            // set memory size to all observations
            void setGlobalMemorySize(uint64_t memory_size);

            // MDP obtained after last unfolding
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> mdp;
            // for each unfolded row, its coloring
            std::vector<std::map<uint64_t,uint64_t>> coloring;
            // for each hole, the number of its options
            std::vector<uint64_t> hole_num_options;
            // hole identifier for each (memory,prior) combination
            std::map<std::pair<uint64_t,uint64_t>,uint64_t> action_holes;
            // hole identifier for each (memory,prior,posterior) combination
            std::map<std::tuple<uint64_t,uint64_t,uint64_t>,uint64_t> update_holes;

        private:

            // original POMDP
            storm::models::sparse::Pomdp<ValueType> const& pomdp;
            
            // for each observation, number of available actions
            std::vector<uint64_t> observation_actions;
            // for each POMDP row, a set of successor observations
            std::vector<std::vector<uint64_t>> row_posteriors;
            // for each observation, a set of successor observations
            std::vector<std::set<uint64_t>> observation_posteriors;

            // clear auxiliary data structures before unfolding
            void clear_before_unfolding();
            // clear auxiliary data structures after unfolding
            void clear_after_unfolding();

            // current number of unfolded states
            uint64_t num_states;
            // for each POMDP state, a list of identifiers of unfolded states
            std::vector<std::vector<uint64_t>> prototype_state_copies;
            // establish the state space: to each state s, create mu(O(s)) of its copies
            void buildStateSpace();

            // establish the design space: create action and update holes
            void buildDesignSpace();

            // for each unfolded row, its prototype
            std::vector<uint64_t> row_prototype;
            // get current number of unfolded rows
            uint64_t num_unfolded_rows() { return this->coloring.size(); }

            
            // unfold a given state-action pair
            void unfoldRow(
                storm::storage::SparseMatrixBuilder<ValueType> & builder,
                uint64_t pomdp_state, uint64_t memory, uint64_t action
            );
            storm::storage::SparseMatrix<ValueType> constructTransitionMatrix();

            // translate state labeling for the unfolded MDP
            storm::models::sparse::StateLabeling constructStateLabeling();
            // translate reward models for the unfolded MDP
            storm::models::sparse::StandardRewardModel<ValueType> constructRewardModel(
                storm::models::sparse::StandardRewardModel<ValueType> const& reward_model
            );

            
        };
    }
}