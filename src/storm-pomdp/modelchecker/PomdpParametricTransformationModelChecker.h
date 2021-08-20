#pragma once

#include "storm/api/verification.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm-pomdp/analysis/FormulaInformation.h"

#include "storm-pomdp/storage/PomdpMemory.h"

namespace storm {
    namespace pomdp {
        namespace modelchecker {

            template <typename ValueType>
            class PomdpParametricTransformationModelChecker {
            public:
                PomdpParametricTransformationModelChecker(storm::models::sparse::Pomdp<ValueType> const& pomdp);

                std::vector<ValueType> computeValuesForFMPolicy(storm::logic::Formula const& formula, storm::pomdp::analysis::FormulaInformation const& info, uint64_t memoryBound, storm::storage::PomdpMemoryPattern memoryPattern = storm::storage::PomdpMemoryPattern::Full, double precision = 1e-9);

            private:
                storm::models::sparse::Pomdp<ValueType> const& pomdp;
            };
        }
    }
}