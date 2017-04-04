#pragma once

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/symbolic/Mdp.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/symbolic/Ctmc.h"

#include "storm/storage/dd/Odd.h"

namespace storm {
    namespace transformer {

        template<storm::dd::DdType Type, typename ValueType>
        class SymbolicDtmcToSparseDtmcTransformer {
        public:
            std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> translate(storm::models::symbolic::Dtmc<Type, ValueType> const& symbolicDtmc);
            storm::dd::Odd const& getOdd() const;
            
        private:
            storm::dd::Odd odd;
        };
        
        template<storm::dd::DdType Type, typename ValueType>
        class SymbolicMdpToSparseMdpTransformer {
        public:
            static std::shared_ptr<storm::models::sparse::Mdp<ValueType>> translate(storm::models::symbolic::Mdp<Type, ValueType> const& symbolicMdp);
        };
        
        template<storm::dd::DdType Type, typename ValueType>
        class SymbolicCtmcToSparseCtmcTransformer {
        public:
            static std::shared_ptr<storm::models::sparse::Ctmc<ValueType>> translate(storm::models::symbolic::Ctmc<Type, ValueType> const& symbolicCtmc);
        };
    }
}
