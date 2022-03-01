#include "storm/utility/jani.h"

#include <boost/algorithm/string.hpp>

#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/jani/Model.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace utility {
namespace jani {

void requireNoUndefinedConstants(storm::jani::Model const& model) {
    if (model.hasUndefinedConstants()) {
        std::vector<std::reference_wrapper<storm::jani::Constant const>> undefinedConstants = model.getUndefinedConstants();
        std::stringstream stream;
        bool printComma = false;
        for (auto const& constant : undefinedConstants) {
            if (printComma) {
                stream << ", ";
            } else {
                printComma = true;
            }
            stream << constant.get().getName() << " (" << constant.get().getType() << ")";
        }
        stream << ".";
        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Model still contains these undefined constants: " + stream.str());
    }
}
}  // namespace jani
}  // namespace utility
}  // namespace storm
