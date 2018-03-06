#pragma once

#include <memory>

#include "storm/storage/dd/DdType.h"

#include "storm/models/symbolic/Model.h"
#include "storm/models/sparse/Model.h"

#include "storm/storage/dd/bisimulation/Partition.h"
#include "storm/storage/dd/bisimulation/PreservationInformation.h"

#include "storm/settings/modules/BisimulationSettings.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template<storm::dd::DdType DdType, typename ValueType>
            class QuotientExtractor {
            public:
                QuotientExtractor();
                
                std::shared_ptr<storm::models::Model<ValueType>> extract(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition, PreservationInformation<DdType, ValueType> const& preservationInformation);
                
            private:
                std::shared_ptr<storm::models::sparse::Model<ValueType>> extractSparseQuotient(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition, PreservationInformation<DdType, ValueType> const& preservationInformation);
                
                std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> extractDdQuotient(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition, PreservationInformation<DdType, ValueType> const& preservationInformation);
                std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> extractQuotientUsingBlockVariables(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition, PreservationInformation<DdType, ValueType> const& preservationInformation);
                std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> extractQuotientUsingOriginalVariables(storm::models::symbolic::Model<DdType, ValueType> const& model, Partition<DdType, ValueType> const& partition, PreservationInformation<DdType, ValueType> const& preservationInformation);

                bool useRepresentatives;
                bool useOriginalVariables;
                storm::settings::modules::BisimulationSettings::QuotientFormat quotientFormat;
            };
            
        }
    }
}
