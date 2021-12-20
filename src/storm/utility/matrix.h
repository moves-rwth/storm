#ifndef STORM_UTILITY_MATRIX_H_
#define STORM_UTILITY_MATRIX_H_

#include "storm/exceptions/InvalidStateException.h"
#include "storm/storage/Scheduler.h"
#include "storm/storage/SparseMatrix.h"

namespace storm {
namespace utility {
namespace matrix {

/*!
 * Applies the given scheduler to the given transition matrix. This means that all choices that are not taken by the scheduler are
 * dropped from the transition matrix. If a state has no choice enabled, it is equipped with a self-loop instead.
 *
 * @param transitionMatrix The transition matrix of the original system.
 * @param scheduler The scheduler to apply to the system.
 * @return A transition matrix that corresponds to all transitions of the given system that are selected by the given scheduler.
 */
template<typename T>
storm::storage::SparseMatrix<T> applyScheduler(storm::storage::SparseMatrix<T> const& transitionMatrix, storm::storage::Scheduler const& scheduler) {
    storm::storage::SparseMatrixBuilder<T> matrixBuilder(transitionMatrix.getRowGroupCount(), transitionMatrix.getColumnCount());

    for (uint_fast64_t state = 0; state < transitionMatrix.getRowGroupCount(); ++state) {
        if (scheduler.isChoiceDefined(state)) {
            // Check whether the choice is valid for this state.
            uint_fast64_t choice = transitionMatrix.getRowGroupIndices()[state] + scheduler.getChoice(state);
            if (choice >= transitionMatrix.getRowGroupIndices()[state + 1]) {
                throw storm::exceptions::InvalidStateException() << "Scheduler defines illegal choice " << choice << " for state " << state << ".";
            }

            // If a valid choice for this state is defined, we copy over the corresponding entries.
            typename storm::storage::SparseMatrix<T>::const_rows selectedRow = transitionMatrix.getRow(choice);
            for (auto const& entry : selectedRow) {
                matrixBuilder.addNextValue(state, entry.getColumn(), entry.getValue());
            }
        } else {
            // If no valid choice for the state is defined, we insert a self-loop.
            matrixBuilder.addNextValue(state, state, storm::utility::one<T>());
        }
    }

    return matrixBuilder.build();
}
}  // namespace matrix
}  // namespace utility
}  // namespace storm

#endif /* STORM_UTILITY_MATRIX_H_ */
