#pragma once
#include <set>
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/storage/SparseMatrix.h"

namespace storm::storage {
#ifdef STORM_HAVE_CARL
std::set<storm::RationalFunctionVariable> getVariables(SparseMatrix<storm::RationalFunction> const& matrix);
#endif
}  // namespace storm::storage