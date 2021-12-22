#pragma once

namespace storm {
namespace utility {
template<class OutputIterator, class Size, class Assignable>
void iota_n(OutputIterator first, Size n, Assignable value) {
    std::generate_n(first, n, [&value]() { return value++; });
}
}  // namespace utility
}  // namespace storm
