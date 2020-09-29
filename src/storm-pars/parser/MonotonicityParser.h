#pragma once

#include <set>
#include <string>

namespace storm {
    namespace parser {
        template<typename VariableType>
        class MonotonicityParser{
        public:
            static std::pair<std::set<VariableType>, std::set<VariableType>> parseMonotoneVariablesFromFile(std::string const& fileName, std::set<VariableType> const& consideredVariables);
        };
    }
}


