#pragma once

#include "storm/models/sparse/Pomdp.h"
#include "storm/logic/Formula.h"

namespace storm {
    namespace synthesis {

        class SubPomdpBuilder {

        public:

            SubPomdpBuilder(
                storm::models::sparse::Pomdp<double> const& pomdp,
                storm::logic::Formula const& formula
            );

            std::set<uint64_t> collectHorizon(std::vector<bool> const& state_relevant);

            std::shared_ptr<storm::models::sparse::Pomdp<double>> restrictPomdp(
                std::map<uint64_t,double> const& initial_belief,
                std::vector<bool> const& state_relevant,
                std::map<uint64_t,double> const& horizon_values
            );

        private:

            // original POMDP
            storm::models::sparse::Pomdp<double> const& pomdp;
            // for each state, a list of immediate successors (excluding state itself)
            std::vector<std::set<uint64_t>> reachable_successors;
            // index of the new initial state
            uint64_t initial_state;
            // index of the new sink state
            uint64_t sink_state;

        };




    }
}