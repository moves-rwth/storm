#pragma once

#include <carl/interval/Interval.h>

#include "storm/adapters/IntervalForward.h"

namespace carl {
template<typename Number>
inline size_t hash_value(carl::Interval<Number> const& i) {
    std::hash<carl::Interval<Number>> h;
    return h(i);
}
}  // namespace carl