#pragma once
#include <ostream>

#include "storm/exceptions/UnexpectedException.h"
#include "storm/utility/macros.h"

namespace storm {
enum class ConditionalAlgorithm { Default, Restart, Bisection, BisectionAdvanced, PolicyIteration };

std::ostream& operator<<(std::ostream& stream, ConditionalAlgorithm const& algorithm);
ConditionalAlgorithm conditionalAlgorithmFromString(std::string const& algorithm);

}  // namespace storm