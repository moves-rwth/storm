#pragma once

#include <memory>
#include <vector>

#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/storage/geometry/Polytope.h"

namespace storm {
namespace modelchecker {
namespace multiobjective {

template<typename ValueType, typename GeometryValueType>
std::vector<GeometryValueType> transformObjectiveValuesToOriginal(std::vector<Objective<ValueType>> objectives, std::vector<GeometryValueType> const& point);

template<typename ValueType, typename GeometryValueType>
std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> transformObjectivePolytopeToOriginal(
    std::vector<Objective<ValueType>> objectives, std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> const& polytope);

}  // namespace multiobjective
}  // namespace modelchecker
}  // namespace storm