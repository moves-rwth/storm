#pragma once

#include "storm/storage/dd/bisimulation/PartitionRefiner.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template <storm::dd::DdType DdType, typename ValueType>
            class MdpPartitionRefiner : public PartitionRefiner<DdType, ValueType> {
            public:
                MdpPartitionRefiner(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& initialStatePartition);
                
                /*!
                 * Refines the partition.
                 *
                 * @param mode The signature mode to use.
                 * @return False iff the partition is stable and no refinement was actually performed.
                 */
                bool refine(bisimulation::SignatureMode const& mode = bisimulation::SignatureMode::Eager);
                
                /*!
                 * Retrieves the current choice partition in the refinement process.
                 */
                Partition<DdType, ValueType> const& getChoicePartition() const;
                
            private:
                // The choice partition in the refinement process.
                Partition<DdType, ValueType> choicePartition;
                
                // The object used to refine the choice partition based on the signatures.
                SignatureRefiner<DdType, ValueType> choiceSignatureRefiner;
            };
            
        }
    }
}
