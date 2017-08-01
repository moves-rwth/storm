#include "storm/storage/dd/bisimulation/PartitionRefiner.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template <storm::dd::DdType DdType, typename ValueType>
            PartitionRefiner<DdType, ValueType>::PartitionRefiner(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& initialStatePartition) : status(Status::Initialized), refinements(0), statePartition(initialStatePartition), signatureComputer(model), signatureRefiner(model.getManager(), statePartition.getBlockVariable(), model.getRowVariables()) {
                // Intentionally left empty.
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            bool PartitionRefiner<DdType, ValueType>::refine(bisimulation::SignatureMode const& mode) {
                this->status = Status::InComputation;
                
                this->signatureComputer.setSignatureMode(mode);
                auto start = std::chrono::high_resolution_clock::now();

                std::chrono::milliseconds::rep signatureTime = 0;
                std::chrono::milliseconds::rep refinementTime = 0;
                
                bool alreadyRefined = false;
                uint64_t index = 0;
                Partition<DdType, ValueType> newStatePartition;
                auto signatureIterator = signatureComputer.compute(statePartition);
                while (signatureIterator.hasNext() && !alreadyRefined) {
                    auto signatureStart = std::chrono::high_resolution_clock::now();
                    auto signature = signatureIterator.next();
                    auto signatureEnd = std::chrono::high_resolution_clock::now();
                    totalSignatureTime += (signatureEnd - signatureStart);
                    STORM_LOG_DEBUG("Signature " << refinements << "[" << index << "] DD has " << signature.getSignatureAdd().getNodeCount() << " nodes.");
                    
                    auto refinementStart = std::chrono::high_resolution_clock::now();
                    newStatePartition = signatureRefiner.refine(statePartition, signature);
                    auto refinementEnd = std::chrono::high_resolution_clock::now();
                    totalRefinementTime += (refinementEnd - refinementStart);

                    signatureTime += std::chrono::duration_cast<std::chrono::milliseconds>(signatureEnd - signatureStart).count();
                    refinementTime = std::chrono::duration_cast<std::chrono::milliseconds>(refinementEnd - refinementStart).count();
                    
                    // Potentially exit early in case we have refined the partition already.
                    if (newStatePartition.getNumberOfBlocks() > statePartition.getNumberOfBlocks()) {
                        alreadyRefined = true;
                    }
                }
                
                auto totalTimeInRefinement = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
                ++refinements;
                STORM_LOG_DEBUG("Refinement " << refinements << " produced " << newStatePartition.getNumberOfBlocks() << " blocks and was completed in " << totalTimeInRefinement << "ms (signature: " << signatureTime << "ms, refinement: " << refinementTime << "ms).");

                if (statePartition == newStatePartition) {
                    this->status = Status::FixedPoint;
                    return false;
                } else {
                    this->statePartition = newStatePartition;
                    return true;
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
