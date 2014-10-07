#ifndef STORM_MODELCHECKER_REACHABILITY_SPARSESCCMODELCHECKER_H_
#define STORM_MODELCHECKER_REACHABILITY_SPARSESCCMODELCHECKER_H_

#include "src/storage/sparse/StateType.h"
#include "src/models/Dtmc.h"
#include "src/properties/prctl/PrctlFilter.h"

namespace storm {
    namespace modelchecker {
        namespace reachability {

            template<typename ValueType>
            class FlexibleSparseMatrix {
            public:
                typedef uint_fast64_t index_type;
                typedef ValueType value_type;
                typedef std::vector<storm::storage::MatrixEntry<index_type, value_type>> row_type;
                typedef typename row_type::iterator iterator;
                typedef typename row_type::const_iterator const_iterator;
                
                FlexibleSparseMatrix() = default;
                FlexibleSparseMatrix(index_type rows);
                
                void reserveInRow(index_type row, index_type numberOfElements);
                
                row_type& getRow(index_type);
                row_type const& getRow(index_type) const;
                
                index_type getNumberOfRows() const;
                
            private:
                std::vector<row_type> data;
            };
            
            template<typename ValueType>
            class SparseSccModelChecker {
            public:
                static ValueType computeReachabilityProbability(storm::models::Dtmc<ValueType> const& dtmc, std::shared_ptr<storm::properties::prctl::PrctlFilter<double>> const& filterFormula);
                
            private:
                static uint_fast64_t treatScc(storm::models::Dtmc<ValueType> const& dtmc, FlexibleSparseMatrix<ValueType>& matrix, std::vector<ValueType>& oneStepProbabilities, storm::storage::BitVector const& entryStates, storm::storage::BitVector const& scc, storm::storage::SparseMatrix<ValueType> const& forwardTransitions, FlexibleSparseMatrix<ValueType>& backwardTransitions, bool eliminateEntryStates, uint_fast64_t level, uint_fast64_t maximalSccSize, std::vector<storm::storage::sparse::state_type>& entryStateQueue, std::vector<std::size_t> const& distances);
                static FlexibleSparseMatrix<ValueType> getFlexibleSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix, bool setAllValuesToOne = false);
                static void eliminateState(FlexibleSparseMatrix<ValueType>& matrix, std::vector<ValueType>& oneStepProbabilities, uint_fast64_t state, FlexibleSparseMatrix<ValueType>& backwardTransitions);
                static bool eliminateStateInPlace(storm::storage::SparseMatrix<ValueType>& matrix, std::vector<ValueType>& oneStepProbabilities, uint_fast64_t state, storm::storage::SparseMatrix<ValueType>& backwardTransitions);
            };
            
        } // namespace reachability
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_REACHABILITY_SPARSESCCMODELCHECKER_H_ */
