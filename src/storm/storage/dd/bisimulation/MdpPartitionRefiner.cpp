#include "storm/storage/dd/bisimulation/MdpPartitionRefiner.h"

#include "storm/models/symbolic/Mdp.h"
#include "storm/models/symbolic/StandardRewardModel.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template<storm::dd::DdType DdType, typename ValueType>
            MdpPartitionRefiner<DdType, ValueType>::MdpPartitionRefiner(storm::models::symbolic::Mdp<DdType, ValueType> const& mdp, Partition<DdType, ValueType> const& initialStatePartition) : PartitionRefiner<DdType, ValueType>(mdp, initialStatePartition), mdp(mdp), choicePartition(Partition<DdType, ValueType>::createTrivialChoicePartition(mdp, initialStatePartition.getBlockVariables())), stateSignatureRefiner(mdp, mdp.getManager(), this->statePartition.getBlockVariable(), mdp.getRowVariables(), mdp.getColumnVariables(), true) {
                // Intentionally left empty.
            }
            
            template<storm::dd::DdType DdType, typename ValueType>
            bool MdpPartitionRefiner<DdType, ValueType>::refine(bisimulation::SignatureMode const& mode) {
                // In this procedure, we will
                // (1) refine the partition of nondeterministic choices based on the state partition. For this, we use
                // the signature computer/refiner of the superclass. These objects use the full transition matrix.
                // (2) if the choice partition was in fact split, the state partition also needs to be refined.
                // For this, we use the signature computer/refiner of this class.
                
                STORM_LOG_TRACE("Refining choice partition.");
                Partition<DdType, ValueType> newChoicePartition = this->internalRefine(this->signatureComputer, this->signatureRefiner, this->choicePartition, this->statePartition, mode);
                
                if (newChoicePartition.getNumberOfBlocks() == choicePartition.getNumberOfBlocks()) {
                    this->status = Status::FixedPoint;
                    return false;
                } else {
                    this->choicePartition = newChoicePartition;
                    
                    // Compute state signatures.
                    storm::dd::Bdd<DdType> choicePartitionAsBdd;
                    if (this->choicePartition.storedAsBdd()) {
                        choicePartitionAsBdd = this->choicePartition.asBdd();
                    } else {
                        choicePartitionAsBdd = this->choicePartition.asAdd().notZero();
                    }
                    Signature<DdType, ValueType> stateSignature(choicePartitionAsBdd.existsAbstract(mdp.getNondeterminismVariables()).template toAdd<ValueType>());
                    
                    // If the choice partition changed, refine the state partition.
                    STORM_LOG_TRACE("Refining state partition.");
                    Partition<DdType, ValueType> newStatePartition = this->internalRefine(stateSignature, this->stateSignatureRefiner, this->statePartition);
                    
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
            
            template<storm::dd::DdType DdType, typename ValueType>
            bool MdpPartitionRefiner<DdType, ValueType>::refineWrtStateActionRewards(storm::dd::Add<DdType, ValueType> const& stateActionRewards) {
                STORM_LOG_TRACE("Refining with respect to state-action rewards.");
                Partition<DdType, ValueType> newChoicePartition = this->signatureRefiner.refine(this->choicePartition, Signature<DdType, ValueType>(stateActionRewards));
                if (newChoicePartition == this->choicePartition) {
                    return false;
                } else {
                    this->choicePartition = newChoicePartition;
                    return true;
                }
            }
            
            template class MdpPartitionRefiner<storm::dd::DdType::CUDD, double>;
            
            template class MdpPartitionRefiner<storm::dd::DdType::Sylvan, double>;
            template class MdpPartitionRefiner<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class MdpPartitionRefiner<storm::dd::DdType::Sylvan, storm::RationalFunction>;
            
        }
    }
}
