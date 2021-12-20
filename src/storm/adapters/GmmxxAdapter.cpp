#include "storm/adapters/GmmxxAdapter.h"

#include <algorithm>

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/adapters/RationalNumberAdapter.h"

#include "storm/utility/macros.h"

namespace storm {
namespace adapters {

template<typename T>
std::unique_ptr<gmm::csr_matrix<T>> GmmxxAdapter<T>::toGmmxxSparseMatrix(storm::storage::SparseMatrix<T> const& matrix) {
    uint_fast64_t realNonZeros = matrix.getEntryCount();
    STORM_LOG_TRACE("Converting " << matrix.getRowCount() << "x" << matrix.getColumnCount() << " matrix with " << realNonZeros
                                  << " non-zeros to gmm++ format.");

    // Prepare the resulting matrix.
    std::unique_ptr<gmm::csr_matrix<T>> result(new gmm::csr_matrix<T>(matrix.getRowCount(), matrix.getColumnCount()));

    // Copy Row Indications
    std::copy(matrix.rowIndications.begin(), matrix.rowIndications.end(), result->jc.begin());

    // Copy columns and values.
    std::vector<T> values;
    values.reserve(matrix.getEntryCount());

    // To match the correct vector type for gmm, we create the vector with the exact same type.
    decltype(result->ir) columns;
    columns.reserve(matrix.getEntryCount());

    for (auto const& entry : matrix) {
        columns.emplace_back(entry.getColumn());
        values.emplace_back(entry.getValue());
    }

    std::swap(result->ir, columns);
    std::swap(result->pr, values);

    STORM_LOG_TRACE("Done converting matrix to gmm++ format.");

    return result;
}

template class GmmxxAdapter<double>;

#ifdef STORM_HAVE_CARL
template class GmmxxAdapter<storm::RationalNumber>;
template class GmmxxAdapter<storm::RationalFunction>;
#endif

}  // namespace adapters
}  // namespace storm
