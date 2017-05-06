#include "storm/storage/dd/BisimulationDecomposition.h"

#include "storm/storage/dd/bisimulation/SignatureComputer.h"
#include "storm/storage/dd/bisimulation/SignatureRefiner.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidOperationException.h"

#include <sylvan_table.h>

extern llmsset_t nodes;

namespace storm {
    namespace dd {
        
        using namespace bisimulation;
        
        template <storm::dd::DdType DdType, typename ValueType>
        BisimulationDecomposition<DdType, ValueType>::BisimulationDecomposition(storm::models::symbolic::Model<DdType, ValueType> const& model) : status(Status::Initialized), model(model), currentPartition(bisimulation::Partition<DdType, ValueType>::create(model)) {
            // Intentionally left empty.
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        BisimulationDecomposition<DdType, ValueType>::BisimulationDecomposition(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& initialPartition) : model(model), currentPartition(initialPartition) {
            // Intentionally left empty.
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        void BisimulationDecomposition<DdType, ValueType>::compute() {
            this->status = Status::InComputation;
            auto start = std::chrono::high_resolution_clock::now();
            std::chrono::high_resolution_clock::duration totalSignatureTime(0);
            std::chrono::high_resolution_clock::duration totalRefinementTime(0);
            
            STORM_LOG_TRACE("Initial partition has " << currentPartition.getNumberOfBlocks() << " blocks.");
#ifndef NDEBUG
            STORM_LOG_TRACE("Initial partition has " << currentPartition.getNodeCount() << " nodes.");
#endif
            
            SignatureRefiner<DdType, ValueType> refiner(model.getManager(), currentPartition.getBlockVariable(), model.getRowVariables());
            SignatureComputer<DdType, ValueType> signatureComputer(model);
            bool done = false;
            uint64_t iterations = 0;
            while (!done) {
                ++iterations;
                auto iterationStart = std::chrono::high_resolution_clock::now();

                auto signatureStart = std::chrono::high_resolution_clock::now();
                Signature<DdType, ValueType> signature = signatureComputer.compute(currentPartition);
                auto signatureEnd = std::chrono::high_resolution_clock::now();
                totalSignatureTime += (signatureEnd - signatureStart);
                
                auto refinementStart = std::chrono::high_resolution_clock::now();
                Partition<DdType, ValueType> newPartition = refiner.refine(currentPartition, signature);
//                newPartition.getPartitionAdd().exportToDot("part" + std::to_string(iterations) + ".dot");
                auto refinementEnd = std::chrono::high_resolution_clock::now();
                totalRefinementTime += (refinementEnd - refinementStart);
                
                auto signatureTime = std::chrono::duration_cast<std::chrono::milliseconds>(signatureEnd - signatureStart).count();
                auto refinementTime = std::chrono::duration_cast<std::chrono::milliseconds>(refinementEnd - refinementStart).count();
                auto iterationTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - iterationStart).count();
                STORM_LOG_DEBUG("Iteration " << iterations << " produced " << newPartition.getNumberOfBlocks() << " blocks and was completed in " << iterationTime << "ms (signature: " << signatureTime << "ms, refinement: " << refinementTime << "ms). Signature DD has " << signature.getSignatureAdd().getNodeCount() << " nodes and partition DD has " << currentPartition.getNodeCount() << " nodes.");

                if (currentPartition == newPartition) {
                    done = true;
                } else {
                    currentPartition = newPartition;
                }
            }

            this->status = Status::FixedPoint;
            auto end = std::chrono::high_resolution_clock::now();
            auto totalSignatureTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(totalSignatureTime).count();
            auto totalRefinementTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(totalRefinementTime).count();
            STORM_LOG_DEBUG("Partition refinement completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms (" << iterations << " iterations, signature: " << totalSignatureTimeInMilliseconds << "ms, refinement: " << totalRefinementTimeInMilliseconds << "ms).");
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> BisimulationDecomposition<DdType, ValueType>::getQuotient() const {
            STORM_LOG_THROW(this->status == Status::FixedPoint, storm::exceptions::InvalidOperationException, "Cannot extract quotient, because bisimulation decomposition was not completed.");
            return nullptr;
        }
        
        template class BisimulationDecomposition<storm::dd::DdType::CUDD, double>;

        template class BisimulationDecomposition<storm::dd::DdType::Sylvan, double>;
        template class BisimulationDecomposition<storm::dd::DdType::Sylvan, storm::RationalNumber>;
        template class BisimulationDecomposition<storm::dd::DdType::Sylvan, storm::RationalFunction>;
        
    }
}
