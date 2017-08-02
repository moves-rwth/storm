#include "storm/storage/dd/bisimulation/MdpPartitionRefiner.h"

#include "storm/models/symbolic/Mdp.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template<storm::dd::DdType DdType, typename ValueType>
            MdpPartitionRefiner<DdType, ValueType>::MdpPartitionRefiner(storm::models::symbolic::Mdp<DdType, ValueType> const& mdp, Partition<DdType, ValueType> const& initialStatePartition) : PartitionRefiner<DdType, ValueType>(mdp, initialStatePartition), choicePartition(Partition<DdType, ValueType>::createTrivialChoicePartition(mdp, initialStatePartition.getBlockVariables())), stateSignatureComputer(mdp.getQualitativeTransitionMatrix(), mdp.getColumnAndNondeterminismVariables(), SignatureMode::Qualitative, true), stateSignatureRefiner(mdp.getManager(), this->statePartition.getBlockVariable(), mdp.getRowVariables()) {
                // Intentionally left empty.
                
                mdp.getTransitionMatrix().exportToDot("fulltrans.dot");
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            bool MdpPartitionRefiner<DdType, ValueType>::refine(bisimulation::SignatureMode const& mode) {
                // In this procedure, we will
                // (1) refine the partition of nondeterministic choices based on the state partition. For this, we use
                // the signature computer/refiner of the superclass. These objects use the full transition matrix.
                // (2) if the choice partition was in fact split, the state partition also needs to be refined.
                // For this, we use the signature computer/refiner of this class.
                
                STORM_LOG_TRACE("Refining choice partition.");
                Partition<DdType, ValueType> newChoicePartition = this->internalRefine(this->signatureComputer, this->signatureRefiner, choicePartition, this->statePartition, mode);
                
                if (newChoicePartition == choicePartition) {
                    this->status = Status::FixedPoint;
                    return false;
                } else {
                    this->choicePartition = newChoicePartition;
                    
                    // If the choice partition changed, refine the state partition. Use qualitative mode we must properly abstract from choice counts.
                    STORM_LOG_TRACE("Refining state partition.");
                    Partition<DdType, ValueType> newStatePartition = this->internalRefine(this->stateSignatureComputer, this->stateSignatureRefiner, this->statePartition, this->choicePartition, SignatureMode::Qualitative);
                    
                    if (newStatePartition == this->statePartition) {
                        this->status = Status::FixedPoint;
                        return false;
                    } else {
                        this->statePartition = newStatePartition;
                        return true;
                    }
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
