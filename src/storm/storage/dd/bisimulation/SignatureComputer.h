#pragma once

#include "storm/storage/dd/DdType.h"

#include "storm/storage/dd/bisimulation/Signature.h"
#include "storm/storage/dd/bisimulation/Partition.h"

#include "storm/models/symbolic/Model.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template<storm::dd::DdType DdType, typename ValueType>
            class SignatureComputer {
            public:
                SignatureComputer(storm::models::symbolic::Model<DdType, ValueType> const& model);
                
                Signature<DdType, ValueType> compute(Partition<DdType, ValueType> const& partition);
                
            private:
                storm::models::symbolic::Model<DdType, ValueType> const& model;
                
                storm::dd::Add<DdType, ValueType> transitionMatrix;
            };
            
        }
    }
}
