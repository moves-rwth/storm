#include "storm/transformer/zeroWeight/ZeroWeightExitLawEnumerator.h"

#include <algorithm>
#include <type_traits>

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/storage/geometry/NativePolytope.h"
#include "storm/utility/ConstantsComparator.h"
#include "storm/utility/constants.h"
#include "storm/utility/macros.h"

namespace storm::transformer {

template<typename ValueType>
ZeroWeightExitLawEnumerator<ValueType>::ZeroWeightExitLawEnumerator(ZeroWeightComponentLp<ValueType>& componentLp) : componentLp(componentLp) {}

template<typename ValueType>
std::vector<std::vector<ValueType>> ZeroWeightExitLawEnumerator<ValueType>::enumerate(uint64_t entryState) {
    uint64_t const boundarySize = componentLp.getBoundaryStates().size();
    auto const zero = storm::utility::zero<ValueType>();
    auto const one = storm::utility::one<ValueType>();
    ValueType precision = zero;
    if constexpr (std::is_same_v<ValueType, double>) {
        precision = 1e-9;
    }
    storm::utility::ConstantsComparator<ValueType> comparator(precision);
    std::vector<ValueType> direction(boundarySize, zero);
    if (boundarySize == 1) {
        return {componentLp.maximize(entryState, direction).boundaryProbabilities};
    }

    uint64_t const dimension = boundarySize - 1;
    std::vector<std::vector<ValueType>> samples;
    std::vector<std::vector<ValueType>> projectedPoints;
    auto addSample = [&](std::vector<ValueType> const& distribution) {
        std::vector<ValueType> point(distribution.begin(), distribution.begin() + dimension);
        for (auto const& existing : projectedPoints) {
            bool equal = true;
            for (uint64_t coordinate = 0; coordinate < dimension; ++coordinate) {
                equal &= comparator.isEqual(existing[coordinate], point[coordinate]);
            }
            if (equal) {
                return false;
            }
        }
        projectedPoints.push_back(std::move(point));
        samples.push_back(distribution);
        return true;
    };

    for (uint64_t coordinate = 0; coordinate < dimension; ++coordinate) {
        direction[coordinate] = one;
        addSample(componentLp.maximize(entryState, direction).boundaryProbabilities);
        direction[coordinate] = -one;
        addSample(componentLp.maximize(entryState, direction).boundaryProbabilities);
        direction[coordinate] = zero;
    }

    while (true) {
        storm::storage::geometry::NativePolytope<ValueType> hull(projectedPoints);
        bool expanded = false;
        for (auto const& halfspace : hull.getHalfspaces()) {
            ValueType scale = zero;
            for (auto const& coefficient : halfspace.normalVector()) {
                ValueType const magnitude = coefficient < zero ? -coefficient : coefficient;
                if (scale < magnitude) {
                    scale = magnitude;
                }
            }
            STORM_LOG_THROW(!storm::utility::isZero(scale), storm::exceptions::UnexpectedException, "The exit-law hull has a zero normal vector.");
            for (uint64_t coordinate = 0; coordinate < dimension; ++coordinate) {
                direction[coordinate] = halfspace.normalVector()[coordinate] / scale;
            }
            auto const optimum = componentLp.maximize(entryState, direction);
            if (comparator.isLess(halfspace.offset() / scale, optimum.objectiveValue)) {
                STORM_LOG_THROW(addSample(optimum.boundaryProbabilities), storm::exceptions::UnexpectedException,
                                "A violated exit-law facet produced an existing point.");
                expanded = true;
                break;
            }
        }
        if (expanded) {
            continue;
        }

        std::vector<std::vector<ValueType>> vertices;
        for (auto const& vertex : hull.getVertices()) {
            bool found = false;
            for (uint64_t index = 0; index < projectedPoints.size(); ++index) {
                bool equal = true;
                for (uint64_t coordinate = 0; coordinate < dimension; ++coordinate) {
                    equal &= comparator.isEqual(vertex[coordinate], projectedPoints[index][coordinate]);
                }
                if (equal) {
                    vertices.push_back(samples[index]);
                    found = true;
                    break;
                }
            }
            STORM_LOG_THROW(found, storm::exceptions::UnexpectedException, "An exit-law vertex has no LP witness.");
        }
        std::sort(vertices.begin(), vertices.end());
        vertices.erase(std::unique(vertices.begin(), vertices.end()), vertices.end());
        return vertices;
    }
}

template class ZeroWeightExitLawEnumerator<double>;
template class ZeroWeightExitLawEnumerator<storm::RationalNumber>;

}  // namespace storm::transformer
