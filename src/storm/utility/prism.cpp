#include "storm/utility/prism.h"

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/prism/Program.h"

#include "storm/utility/cli.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/InvalidArgumentException.h"

#include <boost/algorithm/string.hpp>

namespace storm {
namespace utility {
namespace prism {

storm::prism::Program preprocess(storm::prism::Program const& program,
                                 std::map<storm::expressions::Variable, storm::expressions::Expression> const& constantDefinitions) {
    storm::prism::Program result = program.defineUndefinedConstants(constantDefinitions);
    result = result.substituteConstantsFormulas();
    return result;
}

storm::prism::Program preprocess(storm::prism::Program const& program, std::string const& constantDefinitionString) {
    return preprocess(program, storm::utility::cli::parseConstantDefinitionString(program.getManager(), constantDefinitionString));
}

void requireNoUndefinedConstants(storm::prism::Program const& program) {
    if (program.hasUndefinedConstants()) {
        std::vector<std::reference_wrapper<storm::prism::Constant const>> undefinedConstants = program.getUndefinedConstants();
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
        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Program still contains these undefined constants: " + stream.str());
    }
}
}  // namespace prism
}  // namespace utility
}  // namespace storm
