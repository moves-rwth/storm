#pragma once

#include <boost/optional.hpp>

#include "storm/storage/dd/DdType.h"

#include "storm/storage/dd/bisimulation/Signature.h"
#include "storm/storage/dd/bisimulation/Partition.h"
#include "storm/storage/dd/bisimulation/SignatureMode.h"

#include "storm/models/symbolic/Model.h"

namespace storm {
    namespace dd {
        namespace bisimulation {

            template<storm::dd::DdType DdType, typename ValueType>
            class SignatureComputer;
            
            template<storm::dd::DdType DdType, typename ValueType>
            class SignatureIterator {
            public:
                SignatureIterator(SignatureComputer<DdType, ValueType> const& signatureComputer, Partition<DdType, ValueType> const& partition);

                bool hasNext() const;
                
                Signature<DdType, ValueType> next();
                
            private:
                // The signature computer to use.
                SignatureComputer<DdType, ValueType> const& signatureComputer;
                
                // The current partition.
                Partition<DdType, ValueType> const& partition;
                
                // The position in the enumeration.
                uint64_t position;
            };

            template<storm::dd::DdType DdType, typename ValueType>
            class SignatureComputer {
            public:
                friend class SignatureIterator<DdType, ValueType>;
                
                SignatureComputer(storm::models::symbolic::Model<DdType, ValueType> const& model, SignatureMode const& mode = SignatureMode::Eager);
                SignatureComputer(storm::dd::Add<DdType, ValueType> const& transitionMatrix, std::set<storm::expressions::Variable> const& columnVariables, SignatureMode const& mode = SignatureMode::Eager);
                SignatureComputer(storm::dd::Bdd<DdType> const& qualitativeTransitionMatrix, std::set<storm::expressions::Variable> const& columnVariables, SignatureMode const& mode = SignatureMode::Eager);
                SignatureComputer(storm::dd::Add<DdType, ValueType> const& transitionMatrix, boost::optional<storm::dd::Bdd<DdType>> const& qualitativeTransitionMatrix, std::set<storm::expressions::Variable> const& columnVariables, SignatureMode const& mode = SignatureMode::Eager);

                void setSignatureMode(SignatureMode const& newMode);

                SignatureIterator<DdType, ValueType> compute(Partition<DdType, ValueType> const& partition);
                
            private:
                /// Methods to compute the signatures.
                Signature<DdType, ValueType> getFullSignature(Partition<DdType, ValueType> const& partition) const;
                Signature<DdType, ValueType> getQualitativeSignature(Partition<DdType, ValueType> const& partition) const;
                
                SignatureMode const& getSignatureMode() const;
                                
                /// The transition matrix to use for the signature computation.
                storm::dd::Add<DdType, ValueType> transitionMatrix;
                
                /// The set of variables from which to abstract when performing matrix-vector multiplication.
                std::set<storm::expressions::Variable> columnVariables;

                /// The mode to use for signature computation.
                SignatureMode mode;
                
                /// Only used when using lazy signatures is enabled.
                mutable boost::optional<storm::dd::Add<DdType, ValueType>> transitionMatrix01;
            };
            
        }
    }
}
