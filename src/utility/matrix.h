#ifndef STORM_UTILITY_MATRIX_H_
#define STORM_UTILITY_MATRIX_H_

#include "src/storage/SparseMatrix.h"
#include "src/storage/Scheduler.h"
#include "src/exceptions/InvalidStateException.h"

namespace storm {
    namespace utility {
        namespace matrix {
            
            /*!
             * Applies the given scheduler to the given transition matrix. This means that all choices that are not taken by the scheduler are
             * dropped from the transition matrix. If a state has no choice enabled, it is equipped with a self-loop instead.
             *
             * @param transitionMatrix The transition matrix of the original system.
             * @param nondeterministicChoiceIndices A vector indicating at which rows the choices for the states begin.
             * @param scheduler The scheduler to apply to the system.
             * @return A transition matrix that corresponds to all transitions of the given system that are selected by the given scheduler.
             */
            template <typename T>
            storm::storage::SparseMatrix<T> applyScheduler(storm::storage::SparseMatrix<T> const& transitionMatrix, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, storm::storage::Scheduler const& scheduler) {
                storm::storage::SparseMatrix<T> result(nondeterministicChoiceIndices.size() - 1, transitionMatrix.getColumnCount());
                
                for (uint_fast64_t state = 0; state < nondeterministicChoiceIndices.size() - 1; ++state) {
                    if (scheduler.isChoiceDefined(state)) {
                        // Check whether the choice is valid for this state.
                        uint_fast64_t choice = nondeterministicChoiceIndices[state] + scheduler.getChoice(state);
                        if (choice >= nondeterministicChoiceIndices[state + 1]) {
                            throw storm::exceptions::InvalidStateException() << "Scheduler defines illegal choice " << choice << " for state " << state << ".";
                        }
                        
                        // If a valid choice for this state is defined, we copy over the corresponding entries.
                        typename storm::storage::SparseMatrix<T>::Rows selectedRow = transitionMatrix.getRow(choice);
                        for (auto entry : selectedRow) {
                            result.insertNextValue(state, entry.column(), entry.value());
                        }
                    } else {
                        // If no valid choice for the state is defined, we insert a self-loop.
                        result.insertNextValue(state, state, storm::utility::constantOne<T>());
                    }
                }
                
                // Finalize the matrix creation and return result.
                result.finalize();
                return result;
            }
        }
    }
}

#endif /* STORM_UTILITY_MATRIX_H_ */
