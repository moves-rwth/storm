#ifndef STORM_STORAGE_SPARSE_STATEVALUATIONS_H_
#define STORM_STORAGE_SPARSE_STATEVALUATIONS_H_

#include <cstdint>
#include <string>

#include "src/storage/sparse/StateType.h"
#include "src/storage/expressions/SimpleValuation.h"

#include "src/models/sparse/StateAnnotation.h"

namespace storm {
    namespace storage {
        namespace sparse {
            
            // A structure holding information about the reachable state space that can be retrieved from the outside.
            struct StateValuations : public storm::models::sparse::StateAnnotation {
                /*!
                 * Constructs a state information object for the given number of states.
                 */
                StateValuations(state_type const& numberOfStates);
                
                // A mapping from state indices to their variable valuations.
                std::vector<storm::expressions::SimpleValuation> valuations;
                
                virtual std::string stateInfo(state_type const& state) const override;
            };
            
        }
    }
}

#endif /* STORM_STORAGE_SPARSE_STATEVALUATIONS_H_ */