#include "storm/storage/dd/BisimulationDecomposition.h"

#include "storm/storage/dd/bisimulation/PartitionRefiner.h"

#include "storm/storage/dd/bisimulation/QuotientExtractor.h"

#include "storm/models/symbolic/Model.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace dd {
        
        using namespace bisimulation;
        
        template <storm::dd::DdType DdType, typename ValueType>
        BisimulationDecomposition<DdType, ValueType>::BisimulationDecomposition(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::storage::BisimulationType const& bisimulationType) : BisimulationDecomposition(model, bisimulation::Partition<DdType, ValueType>::create(model, bisimulationType)) {
            // Intentionally left empty.
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        BisimulationDecomposition<DdType, ValueType>::BisimulationDecomposition(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, storm::storage::BisimulationType const& bisimulationType) : BisimulationDecomposition(model, bisimulation::Partition<DdType, ValueType>::create(model, formulas, bisimulationType)) {
            // Intentionally left empty.
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        BisimulationDecomposition<DdType, ValueType>::BisimulationDecomposition(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& initialPartition) : model(model), refiner(std::make_unique<PartitionRefiner<DdType, ValueType>>(model, initialPartition)) {
            STORM_LOG_THROW(!model.hasRewardModel(), storm::exceptions::NotSupportedException, "Symbolic bisimulation currently does not support preserving rewards.");
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        BisimulationDecomposition<DdType, ValueType>::~BisimulationDecomposition() = default;
        
        template <storm::dd::DdType DdType, typename ValueType>
        void BisimulationDecomposition<DdType, ValueType>::compute(bisimulation::SignatureMode const& mode) {
            STORM_LOG_ASSERT(refiner, "No suitable refiner.");
            
            STORM_LOG_TRACE("Initial partition has " << refiner->getStatePartition().getNumberOfBlocks() << " blocks.");
#ifndef NDEBUG
            STORM_LOG_TRACE("Initial partition has " << refiner->getStatePartition().getNodeCount() << " nodes.");
#endif

            auto start = std::chrono::high_resolution_clock::now();
            uint64_t iterations = 0;
            bool refined = true;
            while (refined) {
                refined = refiner->refine(mode);
                ++iterations;
                STORM_LOG_TRACE("After iteration " << iterations << " partition has " << refiner->getStatePartition().getNumberOfBlocks() << " blocks.");
            }
            auto end = std::chrono::high_resolution_clock::now();
            
            STORM_LOG_DEBUG("Partition refinement completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms (" << iterations << " iterations).");
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        std::shared_ptr<storm::models::Model<ValueType>> BisimulationDecomposition<DdType, ValueType>::getQuotient() const {
            STORM_LOG_THROW(this->refiner->getStatus() == Status::FixedPoint, storm::exceptions::InvalidOperationException, "Cannot extract quotient, because bisimulation decomposition was not completed.");

            STORM_LOG_TRACE("Starting quotient extraction.");
            QuotientExtractor<DdType, ValueType> extractor;
            std::shared_ptr<storm::models::Model<ValueType>> quotient = extractor.extract(model, refiner->getStatePartition());
            STORM_LOG_TRACE("Quotient extraction done.");
            
            return quotient;
        }
        
        template class BisimulationDecomposition<storm::dd::DdType::CUDD, double>;

        template class BisimulationDecomposition<storm::dd::DdType::Sylvan, double>;
        template class BisimulationDecomposition<storm::dd::DdType::Sylvan, storm::RationalNumber>;
        template class BisimulationDecomposition<storm::dd::DdType::Sylvan, storm::RationalFunction>;
        
    }
}
