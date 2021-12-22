#pragma once

#include <iostream>

namespace storm {
namespace solver {

enum class MultiplicationStyle { GaussSeidel, Regular };

std::ostream& operator<<(std::ostream& out, MultiplicationStyle const& style);

}  // namespace solver
}  // namespace storm
