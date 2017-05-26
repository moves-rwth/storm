#ifndef STORM_STORAGE_SPARSE_STATEVALUATIONS_H_
#define STORM_STORAGE_SPARSE_STATEVALUATIONS_H_

#include <cstdint>
#include <string>

#include "storm/storage/sparse/StateType.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/expressions/SimpleValuation.h"
#include "storm/models/sparse/StateAnnotation.h"

namespace storm {
    namespace storage {
        namespace sparse {
            
            // A structure holding information about the reachable state space that can be retrieved from the outside.
            class StateValuations : public storm::models::sparse::StateAnnotation {
            
            public:
                /*!
                 * Constructs a state information object for the given number of states.
                 */
                StateValuations(std::vector<storm::expressions::SimpleValuation> const& valuations);
                StateValuations(std::vector<storm::expressions::SimpleValuation>&& valuations);
                
                virtual ~StateValuations() = default;
                
                virtual std::string getStateInfo(storm::storage::sparse::state_type const& state) const override;
                
                storm::expressions::SimpleValuation const& getStateValuation(storm::storage::sparse::state_type const& state) const;
                
                // Returns the number of states that this object describes.
                uint_fast64_t getNumberOfStates() const;
                
                /*
                 * Derive new state valuations from this by selecting the given states.
                 */
                StateValuations selectStates(storm::storage::BitVector const& selectedStates) const;
                
                /*
                 * Derive new state valuations from this by selecting the given states.
                 * If an invalid state index is selected, the corresponding valuation will be empty.
                 */
                StateValuations selectStates(std::vector<storm::storage::sparse::state_type> const& selectedStates) const;
                
                
            private:
                
                // A mapping from state indices to their variable valuations.
                std::vector<storm::expressions::SimpleValuation> valuations;
                
            };
            
        }
    }
}

#endif /* STORM_STORAGE_SPARSE_STATEVALUATIONS_H_ */
