#pragma once
#include <optional>

namespace storm::pars::utility {
enum class ParametricMode { Feasibility, Verification, Monotonicity, SolutionFunction, Sampling, Partitioning };

std::optional<ParametricMode> getParametricModeFromString(std::string const&);
}  // namespace storm::pars::utility