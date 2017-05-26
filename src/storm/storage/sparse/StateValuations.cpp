#include "storm/storage/sparse/StateValuations.h"

#include "storm/utility/vector.h"

namespace storm {
    namespace storage {
        namespace sparse {
            
            StateValuations::StateValuations(std::vector<storm::expressions::SimpleValuation> const& valuations) : valuations(valuations) {
                // Intentionally left empty.
            }
                        
            StateValuations::StateValuations(std::vector<storm::expressions::SimpleValuation>&& valuations) : valuations(std::move(valuations)) {
                // Intentionally left empty.
            }
            
            std::string StateValuations::getStateInfo(state_type const& state) const {
                return getStateValuation(state).toString();
            }
            
            storm::expressions::SimpleValuation const& StateValuations::getStateValuation(storm::storage::sparse::state_type const& state) const {
                return valuations[state];
            }

            uint_fast64_t StateValuations::getNumberOfStates() const {
                return valuations.size();
            }
            
            StateValuations StateValuations::selectStates(storm::storage::BitVector const& selectedStates) const {
                return StateValuations(storm::utility::vector::filterVector(valuations, selectedStates));
            }

            StateValuations StateValuations::selectStates(std::vector<storm::storage::sparse::state_type> const& selectedStates) const {
                std::vector<storm::expressions::SimpleValuation> selectedValuations;
                selectedValuations.reserve(selectedStates.size());
                for (auto const& selectedState : selectedStates){
                    if (selectedState < valuations.size()) {
                        selectedValuations.push_back(valuations[selectedState]);
                    } else {
                        selectedValuations.emplace_back();
                    }
                }
                return StateValuations(std::move(selectedValuations));
            }
        }
    }
}
