#pragma once

#include <ostream>

namespace storm {
namespace parser {

enum class ConstantDataType { Bool, Integer, Rational };

std::ostream& operator<<(std::ostream& out, ConstantDataType const& constantDataType);
}  // namespace parser
}  // namespace storm
