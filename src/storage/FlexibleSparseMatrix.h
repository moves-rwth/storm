#ifndef STORM_STORAGE_FLEXIBLESPARSEMATRIX_H_
#define	STORM_STORAGE_FLEXIBLESPARSEMATRIX_H_

#include <boost/optional.hpp>

#include "src/storage/SparseMatrix.h"
#include "src/storage/sparse/StateType.h"
#include "src/storage/BitVector.h"

namespace storm {
    namespace storage {
        
        template<typename ValueType>
        class FlexibleSparseMatrix {
        public:
            typedef uint_fast64_t index_type;
            typedef std::vector<storm::storage::MatrixEntry<index_type, ValueType>> row_type;
            typedef typename row_type::iterator iterator;
            typedef typename row_type::const_iterator const_iterator;

            FlexibleSparseMatrix() = default;
            FlexibleSparseMatrix(index_type rows);
            FlexibleSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix, bool setAllValuesToOne = false);

            void reserveInRow(index_type row, index_type numberOfElements);

            row_type& getRow(index_type);
            row_type const& getRow(index_type) const;

            index_type getNumberOfRows() const;

            void print() const;

            /*!
             * Checks whether the given state has a self-loop with an arbitrary probability in the given probability matrix.
             *
             * @param state The state for which to check whether it possesses a self-loop.
             * @param matrix The matrix in which to look for the loop.
             * @return True iff the given state has a self-loop with an arbitrary probability in the given probability matrix.
             */
            bool hasSelfLoop(storm::storage::sparse::state_type state);
            
            static void eliminateState(FlexibleSparseMatrix& matrix, std::vector<ValueType>& oneStepProbabilities, uint_fast64_t state, uint_fast64_t row, FlexibleSparseMatrix& backwardTransitions, boost::optional<std::vector<ValueType>>& stateRewards, bool removeForwardTransitions = true, bool constrained = false, storm::storage::BitVector const& predecessorConstraint = storm::storage::BitVector());

        private:
            std::vector<row_type> data;
        };
    }
}
#endif	/* STORM_STORAGE_FLEXIBLESPARSEMATRIX_H_ */

