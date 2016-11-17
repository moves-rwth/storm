#include "storm/parser/CSVParser.h"
#include <boost/any.hpp>

#include <boost/algorithm/string.hpp>
#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidArgumentException.h"


namespace storm {
    namespace parser {
        
        std::vector<std::string> parseCommaSeperatedValues(std::string const& input) {
            std::vector<std::string> values;
            std::vector<std::string> definitions;
            boost::split(definitions, input, boost::is_any_of(","));
            for (auto& definition : definitions) {
                boost::trim(definition);
                values.push_back(definition);
            }
            return values;
        }
    }
}
