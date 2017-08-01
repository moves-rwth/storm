#include "storm/storage/dd/bisimulation/MdpPartitionRefiner.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template<storm::dd::DdType DdType, typename ValueType>
            MdpPartitionRefiner<DdType, ValueType>::MdpPartitionRefiner(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& initialStatePartition) : PartitionRefiner<DdType, ValueType>(model, initialStatePartition) {
                // Start by initializing the choice signature refiner.
                std::set<storm::expressions::Variable> choiceSignatureVariables;
                std::set_union(model.getRowMetaVariables().begin(), model.getRowMetaVariables().end(), model.getNondeterminismVariables().begin(), model.getNondeterminismVariables().end(), std::inserter(choiceSignatureVariables, choiceSignatureVariables.begin()));
                choiceSignatureRefiner = SignatureRefiner<DdType, ValueType>(model.getManager(), this->statePartition.getBlockVariable(), choiceSignatureVariables);
                
                // Create dummy choice partition that is refined to the right result in the first call to refine.
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            bool MdpPartitionRefiner<DdType, ValueType>::refine(bisimulation::SignatureMode const& mode) {
                // Magic here.
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> const& MdpPartitionRefiner<DdType, ValueType>::getChoicePartition() const {
                return choicePartition;
            }
            
        }
    }
}
