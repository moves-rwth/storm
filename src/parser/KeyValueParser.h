#pragma once

namespace storm {
    namespace parser {
        std::unordered_map<std::string, std::string> parseKeyValueString(std::string const& keyValueString);
    }
}