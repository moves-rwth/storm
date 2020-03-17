#pragma once

// Modernjson JSON parser
#include "json.hpp"

namespace storm {
    template <typename ValueType>
    using json = nlohmann::basic_json<std::map, std::vector, std::string, bool, int64_t, uint64_t, ValueType>;
}
