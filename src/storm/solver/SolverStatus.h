#pragma once

#include <ostream>

namespace storm {
namespace solver {

enum class SolverStatus { Converged, TerminatedEarly, MaximalIterationsExceeded, InProgress, Aborted };

std::ostream& operator<<(std::ostream& out, SolverStatus const& status);

}  // namespace solver
}  // namespace storm
