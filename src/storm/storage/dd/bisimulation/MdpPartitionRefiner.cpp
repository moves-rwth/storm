#include "storm/storage/dd/bisimulation/MdpPartitionRefiner.h"

#include "storm/models/symbolic/Mdp.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template<storm::dd::DdType DdType, typename ValueType>
            MdpPartitionRefiner<DdType, ValueType>::MdpPartitionRefiner(storm::models::symbolic::Mdp<DdType, ValueType> const& mdp, Partition<DdType, ValueType> const& initialStatePartition) : PartitionRefiner<DdType, ValueType>(mdp, initialStatePartition), choicePartition(Partition<DdType, ValueType>::createTrivialChoicePartition(mdp, initialStatePartition.getBlockVariables())) {
                
                // Initialize the choice signature refiner.
                std::set<storm::expressions::Variable> choiceSignatureVariables;
                std::set_union(mdp.getRowVariables().begin(), mdp.getRowVariables().end(), mdp.getNondeterminismVariables().begin(), mdp.getNondeterminismVariables().end(), std::inserter(choiceSignatureVariables, choiceSignatureVariables.begin()));
                choiceSignatureRefiner = SignatureRefiner<DdType, ValueType>(mdp.getManager(), this->statePartition.getBlockVariable(), choiceSignatureVariables);
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            bool MdpPartitionRefiner<DdType, ValueType>::refine(bisimulation::SignatureMode const& mode) {
                Partition<DdType, ValueType> newChoicePartition = this->internalRefine(choiceSignatureRefiner, choicePartition, mode);
                
                if (newChoicePartition == choicePartition) {
                    this->status = Status::FixedPoint;
                    return false;
                } else {
                    return PartitionRefiner<DdType, ValueType>::refine(mode);
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> const& MdpPartitionRefiner<DdType, ValueType>::getChoicePartition() const {
                return choicePartition;
            }
            
            template class MdpPartitionRefiner<storm::dd::DdType::CUDD, double>;
            
            template class MdpPartitionRefiner<storm::dd::DdType::Sylvan, double>;
            template class MdpPartitionRefiner<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class MdpPartitionRefiner<storm::dd::DdType::Sylvan, storm::RationalFunction>;
            
        }
    }
}
