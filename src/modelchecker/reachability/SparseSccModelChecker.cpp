#include "src/modelchecker/reachability/SparseSccModelChecker.h"

namespace storm {
    namespace modelchecker {
        namespace reachability {
            
            template<typename ValueType>
            ValueType SparseSccModelChecker<ValueType>::computeReachabilityProbability(storm::models::Dtmc<ValueType> const& dtmc, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
                
                
                
                return 0;
            }
            
            template<typename ValueType>
            typename SparseSccModelChecker<ValueType>::FlexibleSparseMatrix SparseSccModelChecker<ValueType>::getFlexibleSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix) {
                FlexibleSparseMatrix flexibleMatrix(matrix.getRowCount());
                
                for (SparseSccModelChecker<ValueType>::FlexibleMatrix::index_type row = 0; row < matrix.getRowCount(); ++row) {
                    
                }
                
                return flexibleMatrix;
            }
            
            template class SparseSccModelChecker<double>;
            
        } // namespace reachability
    } // namespace modelchecker
} // namespace storm