#pragma once

#include "storm/models/sparse/Pomdp.h"

namespace storm {
    namespace transformer {

        template<typename ValueType>
        class GlobalPOMDPSelfLoopEliminator {

        public:
            GlobalPOMDPSelfLoopEliminator(storm::models::sparse::Pomdp<ValueType> const& pomdp) : pomdp(pomdp) {

            }

            std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> transform() const;
            storm::models::sparse::Pomdp<ValueType> const& pomdp;
        };
    }
}