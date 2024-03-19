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

/* From SparseDtmcParameterLiftingModelChecker.h


template<typename SparseModelType, typename ConstantType>
void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::splitSmart(
    storm::storage::ParameterRegion<ParametricType>& region, std::vector<storm::storage::ParameterRegion<ParametricType>>& regionVector,
    storm::analysis::MonotonicityResult<VariableType>& monRes, bool splitForExtremum) const {
    assert(regionVector.size() == 0);

std::multimap<double, VariableType> sortedOnValues;
std::set<VariableType> consideredVariables;
if (splitForExtremum) {
    if (isValueDeltaRegionSplitEstimates() && useRegionSplitEstimates) {
        STORM_LOG_INFO("Splitting based on region split estimates");
        for (auto& entry : regionSplitEstimates) {
            assert(!this->isUseMonotonicitySet() ||
                   (!monRes.isMonotone(entry.first) && this->possibleMonotoneParameters.find(entry.first) != this->possibleMonotoneParameters.end()));
            //                            sortedOnValues.insert({-(entry.second *
            //                            storm::utility::convertNumber<double>(region.getDifference(entry.first))*
            //                            storm::utility::convertNumber<double>(region.getDifference(entry.first))), entry.first});
            sortedOnValues.insert({-(entry.second), entry.first});
        }

        for (auto itr = sortedOnValues.begin(); itr != sortedOnValues.end() && consideredVariables.size() < maxSplitDimensions; ++itr) {
            consideredVariables.insert(itr->second);
        }
        assert(consideredVariables.size() > 0);
        region.split(region.getCenterPoint(), regionVector, std::move(consideredVariables));
    } else {
        STORM_LOG_INFO("Splitting based on sorting");

        auto& sortedOnDifference = region.getVariablesSorted();
        for (auto itr = sortedOnDifference.begin(); itr != sortedOnDifference.end() && consideredVariables.size() < maxSplitDimensions; ++itr) {
            if (!this->isUseMonotonicitySet() || !monRes.isMonotone(itr->second)) {
                consideredVariables.insert(itr->second);
            }
        }
        assert(consideredVariables.size() > 0 || (monRes.isDone() && monRes.isAllMonotonicity()));
        region.split(region.getCenterPoint(), regionVector, std::move(consideredVariables));
    }
} else {
    // split for pla
    if (isValueDeltaRegionSplitEstimates() && useRegionSplitEstimates) {
        STORM_LOG_INFO("Splitting based on region split estimates");
        ConstantType diff = this->lastValue - (this->currentCheckTask->getFormula().asOperatorFormula().template getThresholdAs<ConstantType>());
        for (auto& entry : regionSplitEstimates) {
            if ((!this->isUseMonotonicitySet() || !monRes.isMonotone(entry.first)) && storm::utility::convertNumber<ConstantType>(entry.second) > diff) {
                sortedOnValues.insert({-(entry.second * storm::utility::convertNumber<double>(region.getDifference(entry.first)) *
                                         storm::utility::convertNumber<double>(region.getDifference(entry.first))),
                                       entry.first});
            }
        }

        for (auto itr = sortedOnValues.begin(); itr != sortedOnValues.end() && consideredVariables.size() < maxSplitDimensions; ++itr) {
            consideredVariables.insert(itr->second);
        }
    }
    if (consideredVariables.size() == 0) {
        auto& sortedOnDifference = region.getVariablesSorted();
        for (auto itr = sortedOnDifference.begin(); itr != sortedOnDifference.end() && consideredVariables.size() < maxSplitDimensions; ++itr) {
            consideredVariables.insert(itr->second);
        }
    }
    assert(consideredVariables.size() > 0);
    region.split(region.getCenterPoint(), regionVector, std::move(consideredVariables));
}
}


  */