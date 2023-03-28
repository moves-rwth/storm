#pragma once
#include <map>
#include <string>
#include <vector>
#include "basic_json_forward.h"

namespace storm {

template<typename ValueType>
using json = nlohmann::basic_json<std::map, std::vector, std::string, bool, int64_t, uint64_t, ValueType>;
}