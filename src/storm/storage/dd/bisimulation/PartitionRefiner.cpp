#include "storm/storage/dd/bisimulation/PartitionRefiner.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template <storm::dd::DdType DdType, typename ValueType>
            PartitionRefiner<DdType, ValueType>::PartitionRefiner(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& initialStatePartition) : status(Status::Initialized), refinements(0), statePartition(initialStatePartition), signatureComputer(model), signatureRefiner(model.getManager(), statePartition.getBlockVariable(), model.getRowAndNondeterminismVariables()) {
                // Intentionally left empty.
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            bool PartitionRefiner<DdType, ValueType>::refine(SignatureMode const& mode) {
                Partition<DdType, ValueType> newStatePartition = this->internalRefine(signatureComputer, signatureRefiner, statePartition, statePartition, mode);
                if (statePartition == newStatePartition) {
                    this->status = Status::FixedPoint;
                    return false;
                } else {
                    this->statePartition = newStatePartition;
                    return true;
                }
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> PartitionRefiner<DdType, ValueType>::internalRefine(SignatureComputer<DdType, ValueType>& signatureComputer, SignatureRefiner<DdType, ValueType>& signatureRefiner, Partition<DdType, ValueType> const& oldPartition, Partition<DdType, ValueType> const& targetPartition, SignatureMode const& mode) {
                auto start = std::chrono::high_resolution_clock::now();
                
                if (this->status != Status::FixedPoint) {
                    this->status = Status::InComputation;
                    
                    signatureComputer.setSignatureMode(mode);
                    
                    std::chrono::milliseconds::rep signatureTime = 0;
                    std::chrono::milliseconds::rep refinementTime = 0;
                    
                    bool refined = false;
                    uint64_t index = 0;
                    Partition<DdType, ValueType> newPartition;
                    auto signatureIterator = signatureComputer.compute(targetPartition);
                    while (signatureIterator.hasNext() && !refined) {
                        auto signatureStart = std::chrono::high_resolution_clock::now();
                        auto signature = signatureIterator.next();
                        auto signatureEnd = std::chrono::high_resolution_clock::now();
                        totalSignatureTime += (signatureEnd - signatureStart);
                        STORM_LOG_DEBUG("Signature " << refinements << "[" << index << "] DD has " << signature.getSignatureAdd().getNodeCount() << " nodes.");
                        
                        signature.getSignatureAdd().exportToDot("sig" + std::to_string(refinements) + ".dot");
//                        if (refinements == 1) {
//                            exit(-1);
//                        }
                        
                        auto refinementStart = std::chrono::high_resolution_clock::now();
                        newPartition = signatureRefiner.refine(statePartition, signature);
                        auto refinementEnd = std::chrono::high_resolution_clock::now();
                        totalRefinementTime += (refinementEnd - refinementStart);
                        
                        newPartition.asAdd().exportToDot("newpart" + std::to_string(refinements) + ".dot");
                        
                        signatureTime += std::chrono::duration_cast<std::chrono::milliseconds>(signatureEnd - signatureStart).count();
                        refinementTime = std::chrono::duration_cast<std::chrono::milliseconds>(refinementEnd - refinementStart).count();
                        
                        // Potentially exit early in case we have refined the partition already.
                        if (newPartition.getNumberOfBlocks() > oldPartition.getNumberOfBlocks()) {
                            refined = true;
                        }
                    }
                    
                    auto totalTimeInRefinement = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
                    ++refinements;
                    STORM_LOG_DEBUG("Refinement " << refinements << " produced " << newPartition.getNumberOfBlocks() << " blocks and was completed in " << totalTimeInRefinement << "ms (signature: " << signatureTime << "ms, refinement: " << refinementTime << "ms).");
                    return newPartition;
                } else {
                    return oldPartition;
                }
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            Partition<DdType, ValueType> const& PartitionRefiner<DdType, ValueType>::getStatePartition() const {
                return statePartition;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            Status PartitionRefiner<DdType, ValueType>::getStatus() const {
                return status;
            }
            
            template class PartitionRefiner<storm::dd::DdType::CUDD, double>;
            
            template class PartitionRefiner<storm::dd::DdType::Sylvan, double>;
            template class PartitionRefiner<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class PartitionRefiner<storm::dd::DdType::Sylvan, storm::RationalFunction>;
            
        }
    }
}
