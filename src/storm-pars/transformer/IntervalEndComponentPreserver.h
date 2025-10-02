#pragma once

#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "storm/adapters/RationalFunctionForward.h"
#include "storm/adapters/RationalNumberForward.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "storm-pars/modelchecker/region/monotonicity/Order.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "storm-pars/utility/parametric.h"
#include "storm/solver/OptimizationDirection.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"

namespace storm {
namespace transformer {

class IntervalEndComponentPreserver {
   public:
    IntervalEndComponentPreserver();
    std::optional<storage::SparseMatrix<Interval>> eliminateMECs(storm::storage::SparseMatrix<Interval> const& matrix, std::vector<Interval> const& vector);
};
}  // namespace transformer
}  // namespace storm
