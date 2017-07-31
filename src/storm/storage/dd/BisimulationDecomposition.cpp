#include "storm/storage/dd/BisimulationDecomposition.h"

#include "storm/storage/dd/bisimulation/SignatureComputer.h"
#include "storm/storage/dd/bisimulation/SignatureRefiner.h"
#include "storm/storage/dd/bisimulation/QuotientExtractor.h"

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
        BisimulationDecomposition<DdType, ValueType>::BisimulationDecomposition(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& initialPartition) : model(model), currentPartition(initialPartition) {
            STORM_LOG_THROW(!model.hasRewardModel(), storm::exceptions::NotSupportedException, "Symbolic bisimulation currently does not support preserving rewards.");
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        void BisimulationDecomposition<DdType, ValueType>::compute(bisimulation::SignatureMode const& mode) {
            this->status = Status::InComputation;
            auto start = std::chrono::high_resolution_clock::now();
            std::chrono::high_resolution_clock::duration totalSignatureTime(0);
            std::chrono::high_resolution_clock::duration totalRefinementTime(0);
            
            STORM_LOG_TRACE("Initial partition has " << currentPartition.getNumberOfBlocks() << " blocks.");
#ifndef NDEBUG
            STORM_LOG_TRACE("Initial partition has " << currentPartition.getNodeCount() << " nodes.");
#endif
            
            SignatureRefiner<DdType, ValueType> refiner(model.getManager(), currentPartition.getBlockVariable(), model.getRowVariables());
            SignatureComputer<DdType, ValueType> signatureComputer(model, mode);
            bool done = false;
            uint64_t iterations = 0;
            while (!done) {
                ++iterations;
                auto iterationStart = std::chrono::high_resolution_clock::now();

                std::chrono::milliseconds::rep signatureTime = 0;
                std::chrono::milliseconds::rep refinementTime = 0;
                
                Partition<DdType, ValueType> newPartition;
                for (uint64_t index = 0, end = signatureComputer.getNumberOfSignatures(); index < end; ++index) {
                    auto signatureStart = std::chrono::high_resolution_clock::now();
                    Signature<DdType, ValueType> signature = signatureComputer.compute(currentPartition, index);
                    auto signatureEnd = std::chrono::high_resolution_clock::now();
                    totalSignatureTime += (signatureEnd - signatureStart);
                    STORM_LOG_DEBUG("Signature " << iterations << "[" << index << "] DD has " << signature.getSignatureAdd().getNodeCount() << " nodes and partition DD has " << currentPartition.getNodeCount() << " nodes.");
                    
                    auto refinementStart = std::chrono::high_resolution_clock::now();
                    newPartition = refiner.refine(currentPartition, signature);
                    auto refinementEnd = std::chrono::high_resolution_clock::now();
                    totalRefinementTime += (refinementEnd - refinementStart);

                    signatureTime += std::chrono::duration_cast<std::chrono::milliseconds>(signatureEnd - signatureStart).count();
                    refinementTime = std::chrono::duration_cast<std::chrono::milliseconds>(refinementEnd - refinementStart).count();
                    
                    // Potentially exit early in case we have refined the partition already. 
                    if (newPartition.getNumberOfBlocks() > currentPartition.getNumberOfBlocks()) {
                        break;
                    }
                }

                auto iterationTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - iterationStart).count();

                STORM_LOG_DEBUG("Iteration " << iterations << " produced " << newPartition.getNumberOfBlocks() << " blocks and was completed in " << iterationTime << "ms (signature: " << signatureTime << "ms, refinement: " << refinementTime << "ms).");

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
        std::shared_ptr<storm::models::Model<ValueType>> BisimulationDecomposition<DdType, ValueType>::getQuotient() const {
            STORM_LOG_THROW(this->status == Status::FixedPoint, storm::exceptions::InvalidOperationException, "Cannot extract quotient, because bisimulation decomposition was not completed.");

            STORM_LOG_TRACE("Starting quotient extraction.");
            QuotientExtractor<DdType, ValueType> extractor;
            std::shared_ptr<storm::models::Model<ValueType>> quotient = extractor.extract(model, currentPartition);
            STORM_LOG_TRACE("Quotient extraction done.");
            
            return quotient;
        }
        
        template class BisimulationDecomposition<storm::dd::DdType::CUDD, double>;

        template class BisimulationDecomposition<storm::dd::DdType::Sylvan, double>;
        template class BisimulationDecomposition<storm::dd::DdType::Sylvan, storm::RationalNumber>;
        template class BisimulationDecomposition<storm::dd::DdType::Sylvan, storm::RationalFunction>;
        
    }
}
