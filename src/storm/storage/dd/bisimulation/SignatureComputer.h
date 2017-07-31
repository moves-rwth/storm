#pragma once

#include "storm/storage/dd/DdType.h"

#include "storm/storage/dd/bisimulation/Signature.h"
#include "storm/storage/dd/bisimulation/Partition.h"
#include "storm/storage/dd/bisimulation/SignatureMode.h"

#include "storm/models/symbolic/Model.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template<storm::dd::DdType DdType, typename ValueType>
            class SignatureComputer {
            public:
                SignatureComputer(storm::models::symbolic::Model<DdType, ValueType> const& model, SignatureMode const& mode = SignatureMode::Eager);
                
                Signature<DdType, ValueType> compute(Partition<DdType, ValueType> const& partition, uint64_t index = 0);

                uint64_t getNumberOfSignatures() const;
                
            private:
                Signature<DdType, ValueType> getFullSignature(Partition<DdType, ValueType> const& partition) const;

                Signature<DdType, ValueType> getQualitativeSignature(Partition<DdType, ValueType> const& partition) const;
                
                storm::models::symbolic::Model<DdType, ValueType> const& model;
                
                // The transition matrix to use for the signature computation.
                storm::dd::Add<DdType, ValueType> transitionMatrix;

                // The mode to use for signature computation.
                SignatureMode mode;
                
                // Only used when using lazy signatures is enabled.
                storm::dd::Add<DdType, ValueType> transitionMatrix01;
            };
            
        }
    }
}
