#pragma  once

#include <map>

#include "storm-dft/storage/dft/DFT.h"
#include "storm-dft/builder/DFTBuilder.h"
#include "storm/adapters/JsonAdapter.h"
namespace storm {
    namespace parser {

        template<typename ValueType>
        class DFTJsonParser {

            typedef typename storm::json<double> Json;
        public:

            static storm::storage::DFT<ValueType> parseJsonFromString(std::string const& jsonString);

            static storm::storage::DFT<ValueType> parseJsonFromFile(std::string const& filename);

        private:
            static storm::storage::DFT<ValueType> parseJson(Json const& jsonInput);

            static std::string generateUniqueName(std::string const& name);

            static std::string parseJsonNumber(Json number);
        };
    }
}
