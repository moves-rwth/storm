#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
RationalFunctionVariable createRFVariable(std::string const& name) {
    return carl::freshRealVariable(name);
}
}  // namespace storm