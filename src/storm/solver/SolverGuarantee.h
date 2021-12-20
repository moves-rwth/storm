#pragma once

#include <ostream>

namespace storm {
namespace solver {

// The guarantees a solver can provide.
// GreaterOrEqual means that the provided solution is greater or equal than the actual solution.
// LessOrEqual means that the provided solution is less or equal than the actual solution.
// None means that the solver cannot provide any guarantees.
enum class SolverGuarantee { GreaterOrEqual, LessOrEqual, None };

std::ostream& operator<<(std::ostream& out, SolverGuarantee const& guarantee);

}  // namespace solver
}  // namespace storm
