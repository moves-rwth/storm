#include "storm-synthesis/pomdp/SubPomdpBuilder.h"

#include "storm/exceptions/InvalidArgumentException.h"

#include "storm/storage/sparse/ModelComponents.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
    namespace synthesis {

        
        SubPomdpBuilder::SubPomdpBuilder(
            storm::models::sparse::Pomdp<double> const& pomdp,
            storm::logic::Formula const& formula
        )
            : pomdp(pomdp) {
            
            STORM_LOG_THROW(pomdp.isCanonic(), storm::exceptions::InvalidArgumentException, "POMDP must be canonic");

            this->initial_state = pomdp.getNumberOfStates();
            this->sink_state = pomdp.getNumberOfStates()+1;

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

        std::set<uint64_t> SubPomdpBuilder::collectHorizon(std::vector<bool> const& state_relevant) {
            std::set<uint64_t> horizon;
            for(uint64_t state = 0; state < this->pomdp.getNumberOfStates(); state++) {
                if(!state_relevant[state]) {
                    continue;
                }
                for(uint64_t successor: this->reachable_successors[state]) {
                    if(!state_relevant[successor]) {
                        horizon.insert(successor);
                    }
                }
            }
            return horizon;
        }

        std::shared_ptr<storm::models::sparse::Pomdp<double>> SubPomdpBuilder::restrictPomdp(
            std::map<uint64_t,double> const& initial_belief,
            std::vector<bool> const& state_relevant,
            std::map<uint64_t,double> const& horizon_values
        ) {
            std::cout << "restricting POMDP ... " << std::endl;
            storm::storage::sparse::ModelComponents<double> components;

            // components.stateLabeling = this->constructStateLabeling();
            // components.choiceLabeling = this->constructChoiceLabeling();
            // components.transitionMatrix = this->constructTransitionMatrix();
            // components.rewardModels.emplace(this->reward_model_name, this->constructRewardModel());
            // components.observabilityClasses = this->constructObservabilityClasses();
            // return std::make_shared<storm::models::sparse::Pomdp<double>>(std::move(components));

            return NULL;
        }

    }
}