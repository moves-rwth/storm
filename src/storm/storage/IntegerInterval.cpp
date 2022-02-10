#include "IntegerInterval.h"

#include <iostream>
#include <string>

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/utility/macros.h"

bool starts_with(const std::string& s1, const std::string& s2) {
    return s2.size() <= s1.size() && s1.compare(0, s2.size(), s2) == 0;
}

namespace storm {
namespace storage {
IntegerInterval parseIntegerInterval(std::string const& stringRepr) {
    if (starts_with(stringRepr, "[") && stringRepr.at(stringRepr.size() - 1) == ']') {
        auto split = stringRepr.find(",");
        std::string first = stringRepr.substr(1, split - 1);

        std::string second = stringRepr.substr(split + 1, stringRepr.size() - split - 2);
        return IntegerInterval(stoi(first), stoi(second));

    } else {
        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot parse " << stringRepr << " as integer interval");
    }
}
}  // namespace storage
}  // namespace storm
