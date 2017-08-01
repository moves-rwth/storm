#pragma once

#include "storm/storage/dd/bisimulation/Status.h"
#include "storm/storage/dd/bisimulation/Partition.h"

#include "storm/storage/dd/bisimulation/SignatureComputer.h"
#include "storm/storage/dd/bisimulation/SignatureRefiner.h"

namespace storm {
    namespace models {
        namespace symbolic {
            template <storm::dd::DdType DdType, typename ValueType>
            class Model;
        }
    }
    
    namespace dd {
        namespace bisimulation {
            
            template <storm::dd::DdType DdType, typename ValueType>
            class PartitionRefiner {
            public:
                PartitionRefiner(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& initialStatePartition);
                
                /*!
                 * Refines the partition. 
                 *
                 * @param mode The signature mode to use.
                 * @return False iff the partition is stable and no refinement was actually performed.
                 */
                bool refine(bisimulation::SignatureMode const& mode = bisimulation::SignatureMode::Eager);
                
                /*!
                 * Retrieves the current state partition in the refinement process.
                 */
                Partition<DdType, ValueType> const& getStatePartition() const;
                
                /*!
                 * Retrieves the status of the refinement process.
                 */
                Status getStatus() const;
                
            protected:
                // The current status.
                Status status;
                
                // The number of refinements that were made.
                uint64_t refinements;
                
                // The state partition in the refinement process. Initially set to the initial partition.
                Partition<DdType, ValueType> statePartition;
                
                // The object used to compute the signatures.
                SignatureComputer<DdType, ValueType> signatureComputer;
                
                // The object used to refine the state partition based on the signatures.
                SignatureRefiner<DdType, ValueType> signatureRefiner;
                
                // Time measurements.
                std::chrono::high_resolution_clock::duration totalSignatureTime;
                std::chrono::high_resolution_clock::duration totalRefinementTime;
            };
            
        }
    }
}
