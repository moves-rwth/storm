#ifndef STORM_STORAGE_STRONGLYCONNECTEDCOMPONENTDECOMPOSITION_H_
#define STORM_STORAGE_STRONGLYCONNECTEDCOMPONENTDECOMPOSITION_H_

#include "src/storage/Decomposition.h"
#include "src/storage/VectorSet.h"
#include "src/storage/BitVector.h"
#include "src/storage/SparseMatrix.h"

namespace storm {
    namespace models {
        // Forward declare the abstract model class.
        template <typename ValueType> class AbstractModel;
    }
    
    namespace storage {
        
        /*!
         * This class represents the decomposition of a graph-like structure into its strongly connected components.
         */
        template <typename ValueType>
        class StronglyConnectedComponentDecomposition : public Decomposition<StateBlock> {
        public:
            /*
             * Creates an empty SCC decomposition.
             */
            StronglyConnectedComponentDecomposition();
            
            /*
             * Creates an SCC decomposition of the given model.
             *
             * @param model The model to decompose into SCCs.
             * @param dropNaiveSccs A flag that indicates whether trivial SCCs (i.e. SCCs consisting of just one state
             * are to be kept in the decomposition.
             */
            StronglyConnectedComponentDecomposition(storm::models::AbstractModel<ValueType> const& model, bool dropNaiveSccs = false);

            /*
             * Creates an SCC decomposition of given block in the given model.
             *
             * @param model The model that contains the block.
             * @param block The block to decompose into SCCs.
             * @param dropNaiveSccs A flag that indicates whether trivial SCCs (i.e. SCCs consisting of just one state
             * are to be kept in the decomposition.
             */
            StronglyConnectedComponentDecomposition(storm::models::AbstractModel<ValueType> const& model, StateBlock const& block, bool dropNaiveSccs = false);
            
            StronglyConnectedComponentDecomposition(StronglyConnectedComponentDecomposition const& other);
            
            StronglyConnectedComponentDecomposition& operator=(StronglyConnectedComponentDecomposition const& other);
            
            StronglyConnectedComponentDecomposition(StronglyConnectedComponentDecomposition&& other);
            
            StronglyConnectedComponentDecomposition& operator=(StronglyConnectedComponentDecomposition&& other);
            
        private:
            void performSccDecomposition(storm::models::AbstractModel<ValueType> const& model, bool dropNaiveSccs);
            
            void performSccDecomposition(storm::models::AbstractModel<ValueType> const& model, storm::storage::BitVector const& subsystem, bool dropNaiveSccs);
            
            void performSccDecompositionHelper(storm::models::AbstractModel<ValueType> const& model, uint_fast64_t startState, storm::storage::BitVector const& subsystem, uint_fast64_t& currentIndex, std::vector<uint_fast64_t>& stateIndices, std::vector<uint_fast64_t>& lowlinks, std::vector<uint_fast64_t>& tarjanStack, storm::storage::BitVector& tarjanStackStates, storm::storage::BitVector& visitedStates, bool dropNaiveSccs);
        };
    }
}

#endif /* STORM_STORAGE_STRONGLYCONNECTEDCOMPONENTDECOMPOSITION_H_ */
