
#include "storm/storage/SparseMatrixOperations.h"
#include "storm/storage/SparseMatrix.h"

namespace storm::storage {
#ifdef STORM_HAVE_CARL
std::set<storm::RationalFunctionVariable> getVariables(SparseMatrix<storm::RationalFunction> const& matrix) {
    std::set<storm::RationalFunctionVariable> result;
    for (auto const& entry : matrix) {
        entry.getValue().gatherVariables(result);
    }
    return result;
}

#endif
}  // namespace storm::storage