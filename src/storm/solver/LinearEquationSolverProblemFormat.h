#pragma once

#include <iostream>

namespace storm {
namespace solver {

enum class LinearEquationSolverProblemFormat { EquationSystem, FixedPointSystem };

std::ostream& operator<<(std::ostream& out, LinearEquationSolverProblemFormat const& format);

}  // namespace solver
}  // namespace storm
