#pragma once

#include <memory>

#include "storm/storage/dd/DdType.h"

#include "storm/models/symbolic/Model.h"

#include "storm/storage/dd/bisimulation/Partition.h"

#include "storm/settings/modules/BisimulationSettings.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template<storm::dd::DdType DdType, typename ValueType>
            class QuotientExtractor {
            public:
                QuotientExtractor();
                
                std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> extract(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition);
                
            private:
                std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> extractSparseQuotient(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition);
                
                std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> extractDdQuotient(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition);
                std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> extractQuotientUsingBlockVariables(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition);
                std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> extractQuotientUsingOriginalVariables(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition);
                
                bool useRepresentatives;
                storm::settings::modules::BisimulationSettings::QuotientFormat quotientFormat;
            };
            
        }
    }
}
