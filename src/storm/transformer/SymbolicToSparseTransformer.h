#pragma once

#include "storm/models/sparse/Mdp.h"
#include "storm/models/symbolic/Mdp.h"

namespace storm {
    namespace transformer {

        template<storm::dd::DdType Type, typename ValueType>
        class SymbolicMdpToSparseMdpTransformer {
        public:
            static std::shared_ptr<storm::models::sparse::Mdp<ValueType>> translate(storm::models::symbolic::Mdp<Type, ValueType> const& symbolicMdp);
        };
    }
}
