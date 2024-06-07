#pragma once

#include <optional>

#include "RegionSplitEstimateKind.h"

namespace storm::modelchecker {
struct RegionSplittingStrategy {
   public:
    enum class Heuristic { EstimateBased, RoundRobin };
    Heuristic heuristic{Heuristic::EstimateBased};
    uint64_t maxSplitDimensions{std::numeric_limits<uint64_t>::max()};
    std::optional<RegionSplitEstimateKind> estimateKind;
};
}  // namespace storm::modelchecker
