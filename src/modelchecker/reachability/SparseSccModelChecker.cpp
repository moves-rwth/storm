#include "src/modelchecker/reachability/SparseSccModelChecker.h"
#include "src/storage/StronglyConnectedComponentDecomposition.h"
#include "src/utility/graph.h"
#include "src/exceptions/ExceptionMacros.h"

namespace storm {
    namespace modelchecker {
        namespace reachability {
            
            template<typename ValueType>
            ValueType SparseSccModelChecker<ValueType>::computeReachabilityProbability(storm::models::Dtmc<ValueType> const& dtmc, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
                // First, do some sanity checks to establish some required properties.
                LOG_THROW(dtmc.getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::IllegalArgumentException, "Input model is required to have exactly one initial state.");
                typename FlexibleSparseMatrix<ValueType>::index_type initialStateIndex = *dtmc.getInitialStates().begin();
                
                storm::storage::BitVector relevantStates = storm::utility::graph::getReachableStates(dtmc.getTransitionMatrix(), dtmc.getInitialStates(), phiStates, psiStates);
                
                std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(dtmc, phiStates, psiStates);
                storm::storage::BitVector statesWithProbability0 = std::move(statesWithProbability01.first);
                storm::storage::BitVector statesWithProbability1 = std::move(statesWithProbability01.second);
                storm::storage::BitVector maybeStates = ~(statesWithProbability0 | statesWithProbability1);
                
                // If the initial state is known to have either probability 0 or 1, we can directly return the result.
                if (!maybeStates.get(initialStateIndex)) {
                    return statesWithProbability0.get(initialStateIndex) ? 0 : 1;
                }
                
                
                // Otherwise, we build the submatrix that only has maybe states
                
                submatrix = dtmc.getTransitionMatrix().getSubmatrix(false, maybeStates, maybeStates);
                
                // Then, we convert the reduced matrix to a more flexible format to be able to perform state elimination more easily.
                FlexibleSparseMatrix<ValueType> flexibleMatrix = getFlexibleSparseMatrix(dtmc.getTransitionMatrix());
                
                // Then, we recursively treat all SCCs.
                treatScc(dtmc, flexibleMatrix, storm::storage::BitVector(dtmc.getNumberOfStates(), true), 0);
                
                // Now, we return the value for the only initial state.
                return flexibleMatrix.getRow(initialStateIndex)[0].getValue();
            }
            
            template<typename ValueType>
            void SparseSccModelChecker<ValueType>::treatScc(storm::models::Dtmc<ValueType> const& dtmc, FlexibleSparseMatrix<ValueType>& matrix, storm::storage::BitVector const& scc, uint_fast64_t level) {
                if (level < 2) {
                    // Here, we further decompose the SCC into sub-SCCs.
                    storm::storage::StronglyConnectedComponentDecomposition<ValueType> decomposition(dtmc, scc, true, false);

                    // And then recursively treat all sub-SCCs.
                    for (auto const& newScc : decomposition) {
                        treatScc(dtmc, matrix, scc, level + 1);
                    }
                    
                    
                    
                } else {
                    // In this case, we perform simple state elimination in the current SCC.
                    
                }
            }
            
            template<typename ValueType>
            FlexibleSparseMatrix<ValueType>::FlexibleSparseMatrix(index_type rows) : data(rows) {
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            void FlexibleSparseMatrix<ValueType>::reserveInRow(index_type row, index_type numberOfElements) {
                this->data[row].reserve(numberOfElements);
            }
            
            template<typename ValueType>
            typename FlexibleSparseMatrix<ValueType>::row_type& FlexibleSparseMatrix<ValueType>::getRow(index_type index) {
                return this->data[index];
            }
            
            template<typename ValueType>
            FlexibleSparseMatrix<ValueType> SparseSccModelChecker<ValueType>::getFlexibleSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix) {
                FlexibleSparseMatrix<ValueType> flexibleMatrix(matrix.getRowCount());
                
                for (typename FlexibleSparseMatrix<ValueType>::index_type rowIndex = 0; rowIndex < matrix.getRowCount(); ++rowIndex) {
                    typename storm::storage::SparseMatrix<ValueType>::const_rows row = matrix.getRow(rowIndex);
                    flexibleMatrix.reserveInRow(rowIndex, row.getNumberOfEntries());
                    
                    for (auto const& element : row) {
                        flexibleMatrix.getRow(rowIndex).emplace_back(element);
                    }
                }
                
                return flexibleMatrix;
            }
            
            template class FlexibleSparseMatrix<double>;
            template class SparseSccModelChecker<double>;
            
        } // namespace reachability
    } // namespace modelchecker
} // namespace storm