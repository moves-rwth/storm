#include "storm/storage/dd/BisimulationDecomposition.h"

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
//            LACE_ME;
            
            auto partitionRefinementStart = std::chrono::high_resolution_clock::now();
            this->status = Status::InComputation;
            
            STORM_LOG_TRACE("Initial partition has " << currentPartition.getNumberOfBlocks() << " blocks.");
#ifndef NDEBUG
            STORM_LOG_TRACE("Initial partition ADD has " << currentPartition.getPartitionAdd().getNodeCount() << " nodes.");
#endif
            
            SignatureRefiner<DdType, ValueType> refiner(model.getManager(), currentPartition.getBlockVariable(), model.getRowVariables());
            bool done = false;
            uint64_t iterations = 0;
            while (!done) {
//                currentPartition.getPartitionAdd().exportToDot("part" + std::to_string(iterations) + ".dot");
                Signature<DdType, ValueType> signature(model.getTransitionMatrix().multiplyMatrix(currentPartition.getPartitionAdd(), model.getColumnVariables()));
//                signature.getSignatureAdd().exportToDot("sig" + std::to_string(iterations) + ".dot");
#ifndef NDEBUG
                STORM_LOG_TRACE("Computed signature ADD with " << signature.getSignatureAdd().getNodeCount() << " nodes.");
#endif
                
                Partition<DdType, ValueType> newPartition = refiner.refine(currentPartition, signature);

                STORM_LOG_TRACE("New partition has " << newPartition.getNumberOfBlocks() << " blocks.");
#ifndef NDEBUG
                STORM_LOG_TRACE("Computed new partition ADD with " << newPartition.getPartitionAdd().getNodeCount() << " nodes.");
#endif
//                STORM_LOG_TRACE("Current #nodes in table " << llmsset_count_marked(nodes) << " of " << llmsset_get_size(nodes) << " BDD nodes.");
                
                if (currentPartition == newPartition) {
                    done = true;
                } else {
                    currentPartition = newPartition;
                }
                ++iterations;
            }

            this->status = Status::FixedPoint;
            auto partitionRefinementEnd = std::chrono::high_resolution_clock::now();
            STORM_LOG_DEBUG("Partition refinement completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(partitionRefinementEnd - partitionRefinementStart).count() << "ms (" << iterations << " iterations).");
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
