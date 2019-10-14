#pragma  once

#include <map>

#include "storm-dft/storage/dft/DFT.h"
#include "storm-dft/builder/DFTBuilder.h"

// JSON parser
#include "json.hpp"

using json = nlohmann::json;

namespace storm {
    namespace parser {

        template<typename ValueType>
        class DFTJsonParser {

        public:

            static storm::storage::DFT<ValueType> parseJsonFromString(std::string const& jsonString);

            static storm::storage::DFT<ValueType> parseJsonFromFile(std::string const& filename);

        private:
            static storm::storage::DFT<ValueType> parseJson(json const& jsonInput);

            static std::string generateUniqueName(std::string const& name);

            static std::string parseJsonNumber(json number);
        };
    }
}
