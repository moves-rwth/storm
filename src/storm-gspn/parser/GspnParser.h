#pragma once

#include "storm-gspn/storage/gspn/GSPN.h"

namespace storm {
namespace parser {
class GspnParser {
   public:
    static storm::gspn::GSPN* parse(std::string const& filename, std::string const& constantDefinitions = "");
};
}  // namespace parser
}  // namespace storm
