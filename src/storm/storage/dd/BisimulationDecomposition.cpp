#include "storm/storage/dd/BisimulationDecomposition.h"

#include "storm/storage/dd/bisimulation/Partition.h"
#include "storm/storage/dd/bisimulation/PartitionRefiner.h"
#include "storm/storage/dd/bisimulation/MdpPartitionRefiner.h"
#include "storm/storage/dd/bisimulation/QuotientExtractor.h"

#include "storm/models/symbolic/Model.h"
#include "storm/models/symbolic/Mdp.h"
#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace dd {
        
        using namespace bisimulation;

        template <storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<PartitionRefiner<DdType, ValueType>> createRefiner(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& initialPartition) {
            if (model.isOfType(storm::models::ModelType::Mdp)) {
                return std::make_unique<MdpPartitionRefiner<DdType, ValueType>>(*model.template as<storm::models::symbolic::Mdp<DdType, ValueType>>(), initialPartition);
            } else {
                return std::make_unique<PartitionRefiner<DdType, ValueType>>(model, initialPartition);
            }
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        BisimulationDecomposition<DdType, ValueType>::BisimulationDecomposition(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::storage::BisimulationType const& bisimulationType) : model(model), preservationInformation(model, bisimulationType), refiner(createRefiner(model, Partition<DdType, ValueType>::create(model, bisimulationType, preservationInformation))) {
            auto const& generalSettings = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
            showProgress = generalSettings.isVerboseSet();
            showProgressDelay = generalSettings.getShowProgressDelay();
            this->refineWrtRewardModels();
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        BisimulationDecomposition<DdType, ValueType>::BisimulationDecomposition(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, storm::storage::BisimulationType const& bisimulationType) : model(model), preservationInformation(model, formulas, bisimulationType), refiner(createRefiner(model, Partition<DdType, ValueType>::create(model, bisimulationType, preservationInformation))) {
            auto const& generalSettings = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
            showProgress = generalSettings.isVerboseSet();
            showProgressDelay = generalSettings.getShowProgressDelay();
            this->refineWrtRewardModels();
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        BisimulationDecomposition<DdType, ValueType>::BisimulationDecomposition(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& initialPartition, bisimulation::PreservationInformation<DdType, ValueType> const& preservationInformation) : model(model), preservationInformation(preservationInformation), refiner(createRefiner(model, initialPartition)) {
            auto const& generalSettings = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
            showProgress = generalSettings.isVerboseSet();
            showProgressDelay = generalSettings.getShowProgressDelay();
            this->refineWrtRewardModels();
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
            auto timeOfLastMessage = start;
            uint64_t iterations = 0;
            bool refined = true;
            while (refined) {
                refined = refiner->refine(mode);

                ++iterations;
                STORM_LOG_TRACE("After iteration " << iterations << " partition has " << refiner->getStatePartition().getNumberOfBlocks() << " blocks.");
                
                if (showProgress) {
                    auto now = std::chrono::high_resolution_clock::now();
                    auto durationSinceLastMessage = std::chrono::duration_cast<std::chrono::seconds>(now - timeOfLastMessage).count();
                    if (static_cast<uint64_t>(durationSinceLastMessage) >= showProgressDelay) {
                        auto durationSinceStart = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
                        STORM_LOG_INFO("State partition after " << iterations << " iterations (" << durationSinceStart << "s) has " << refiner->getStatePartition().getNumberOfBlocks() << " blocks.");
                        timeOfLastMessage = std::chrono::high_resolution_clock::now();
                    }
                }
            }
            auto end = std::chrono::high_resolution_clock::now();
            
            STORM_LOG_DEBUG("Partition refinement completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "s (" << iterations << " iterations).");
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        std::shared_ptr<storm::models::Model<ValueType>> BisimulationDecomposition<DdType, ValueType>::getQuotient() const {
            STORM_LOG_THROW(this->refiner->getStatus() == Status::FixedPoint, storm::exceptions::InvalidOperationException, "Cannot extract quotient, because bisimulation decomposition was not completed.");
            
            STORM_LOG_TRACE("Starting quotient extraction.");
            QuotientExtractor<DdType, ValueType> extractor;
            std::shared_ptr<storm::models::Model<ValueType>> quotient = extractor.extract(model, refiner->getStatePartition(), preservationInformation);
            STORM_LOG_TRACE("Quotient extraction done.");
            
            return quotient;
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        void BisimulationDecomposition<DdType, ValueType>::refineWrtRewardModels() {
            for (auto const& rewardModelName : this->preservationInformation.getRewardModelNames()) {
                auto const& rewardModel = this->model.getRewardModel(rewardModelName);
                refiner->refineWrtRewardModel(rewardModel);
            }
        }
        
        template class BisimulationDecomposition<storm::dd::DdType::CUDD, double>;

        template class BisimulationDecomposition<storm::dd::DdType::Sylvan, double>;
        template class BisimulationDecomposition<storm::dd::DdType::Sylvan, storm::RationalNumber>;
        template class BisimulationDecomposition<storm::dd::DdType::Sylvan, storm::RationalFunction>;
        
    }
}
