#pragma once

#include <map>

namespace storm {
namespace jani {
class Model;
}

namespace utility {
namespace jani {

void requireNoUndefinedConstants(storm::jani::Model const& model);
}
}  // namespace utility
}  // namespace storm
