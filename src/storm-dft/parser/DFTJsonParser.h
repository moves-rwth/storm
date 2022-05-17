#pragma once

#include <map>

#include "storm-dft/builder/DFTBuilder.h"
#include "storm-dft/storage/DFT.h"
#include "storm/adapters/JsonAdapter.h"

namespace storm::dft {
namespace parser {

template<typename ValueType>
class DFTJsonParser {
    typedef typename storm::json<double> Json;

   public:
    static storm::dft::storage::DFT<ValueType> parseJsonFromString(std::string const& jsonString);

    static storm::dft::storage::DFT<ValueType> parseJsonFromFile(std::string const& filename);

   private:
    static storm::dft::storage::DFT<ValueType> parseJson(Json const& jsonInput);

    static std::string generateUniqueName(std::string const& name);

    static std::string parseJsonNumber(Json number);
};

}  // namespace parser
}  // namespace storm::dft
