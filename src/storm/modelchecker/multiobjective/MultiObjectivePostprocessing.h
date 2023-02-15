#pragma once

#include <memory>
#include <vector>

#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/storage/Scheduler.h"
#include "storm/storage/geometry/Polytope.h"
#include "storm/storage/memorystructure/SparseModelMemoryProductReverseData.h"

namespace storm {
namespace modelchecker {
namespace multiobjective {

template<typename ValueType, typename GeometryValueType>
std::vector<GeometryValueType> transformObjectiveValuesToOriginal(std::vector<Objective<ValueType>> objectives, std::vector<GeometryValueType> const& point);

template<typename ValueType, typename GeometryValueType>
std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> transformObjectivePolytopeToOriginal(
    std::vector<Objective<ValueType>> objectives, std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> const& polytope);

template<typename ValueType, typename SparseModelType>
std::map<std::vector<ValueType>, std::shared_ptr<storm::storage::Scheduler<ValueType>>> transformObjectiveSchedulersToOriginal(
    storm::storage::SparseModelMemoryProductReverseData const& modelMemoryProduct, std::shared_ptr<SparseModelType> const& originalModel,
    std::map<std::vector<ValueType>, std::shared_ptr<storm::storage::Scheduler<ValueType>>> schedulers);

}  // namespace multiobjective
}  // namespace modelchecker
}  // namespace storm