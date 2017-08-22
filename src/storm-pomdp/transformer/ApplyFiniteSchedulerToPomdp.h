#pragma once

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Pomdp.h"
#include <map>
#include <string>

namespace storm {
    namespace transformer {

        template<typename ValueType>
        class ApplyFiniteSchedulerToPomdp {

        public:
            ApplyFiniteSchedulerToPomdp(storm::models::sparse::Pomdp<ValueType> const& pomdp) : pomdp(pomdp) {

            }

            std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> transform() const;
            storm::models::sparse::Pomdp<ValueType> const& pomdp;
        };
    }
}