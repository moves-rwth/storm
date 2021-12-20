#pragma once

#include <string>
#include <unordered_map>

namespace storm {
namespace parser {
std::unordered_map<std::string, std::string> parseKeyValueString(std::string const& keyValueString);
}
}  // namespace storm
