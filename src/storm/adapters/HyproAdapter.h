#ifndef STORM_ADAPTERS_HYPROADAPTER_H_
#define STORM_ADAPTERS_HYPROADAPTER_H_

// Include config to know whether HyPro is available or not.
#include "storm-config.h"

#ifdef STORM_HAVE_HYPRO

#include <hypro/datastructures/Halfspace.h>
#include <hypro/representations/GeometricObject.h>
#include <hypro/representations/Polytopes/HPolytope/HPolytope.h>
#include <hypro/types.h>

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/storage/geometry/Halfspace.h"

namespace storm {
namespace adapters {

template<typename T>
std::vector<T> fromHypro(hypro::vector_t<T> const& v) {
    return std::vector<T>(v.data(), v.data() + v.rows());
}

template<typename T>
hypro::vector_t<T> toHypro(std::vector<T> const& v) {
    return hypro::vector_t<T>::Map(v.data(), v.size());
}

template<typename T>
hypro::Halfspace<T> toHypro(storm::storage::geometry::Halfspace<T> const& h) {
    T offset = h.offset();
    return hypro::Halfspace<T>(toHypro(h.normalVector()), std::move(offset));
}

template<typename T>
storm::storage::geometry::Halfspace<T> fromHypro(hypro::Halfspace<T> const& h) {
    T offset = h.offset();
    return storm::storage::geometry::Halfspace<T>(fromHypro(h.normal()), std::move(offset));
}

}  // namespace adapters
}  // namespace storm
#endif  // STORM_HAVE_HYPRO

#endif /* STORM_ADAPTERS_HYPROADAPTER_H_ */
