#include "src/models/Dtmc.h"

namespace storm {
    namespace modelchecker {
        namespace reachability {

            template<typename ValueType>
            class FlexibleSparseMatrix {
            public:
                typedef uint_fast64_t index_type;
                typedef ValueType value_type;
                typedef std::vector<storm::storage::MatrixEntry<index_type, value_type>> row_type;
                
                FlexibleSparseMatrix() = default;
                FlexibleSparseMatrix(index_type rows);
                
                void reserveInRow(index_type row, index_type numberOfElements);
                
                row_type& getRow(index_type);
                
            private:
                std::vector<row_type> data;
            };
            
            template<typename ValueType>
            class SparseSccModelChecker {
            private:

                
            public:
                static ValueType computeReachabilityProbability(storm::models::Dtmc<ValueType> const& dtmc, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates);
                
            private:
                static void treatScc(storm::models::Dtmc<ValueType> const& dtmc, FlexibleSparseMatrix<ValueType>& matrix, storm::storage::BitVector const& scc, uint_fast64_t level);
                static FlexibleSparseMatrix<ValueType> getFlexibleSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix);
            };
        }
    }
}