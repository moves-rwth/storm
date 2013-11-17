#ifndef STORM_STORAGE_STRONGLYCONNECTEDCOMPONENTDECOMPOSITION_H_
#define STORM_STORAGE_STRONGLYCONNECTEDCOMPONENTDECOMPOSITION_H_

#include "src/storage/Decomposition.h"
#include "src/storage/VectorSet.h"
#include "src/storage/BitVector.h"
#include "src/storage/SparseMatrix.h"

namespace storm {
    namespace models {
        // Forward declare the abstract model class.
        template <typename T> class AbstractModel;
    }
    
    namespace storage {
        
        /*!
         * This class represents the decomposition of a graph-like structure into its strongly connected components.
         */
        template <typename T>
        class StronglyConnectedComponentDecomposition : public Decomposition {
        public:
            /*
             * Creates an empty SCC decomposition.
             */
            StronglyConnectedComponentDecomposition();
            
            /*
             * Creates an SCC decomposition of the given model.
             */
            StronglyConnectedComponentDecomposition(storm::models::AbstractModel<T> const& model);
            
            StronglyConnectedComponentDecomposition(StronglyConnectedComponentDecomposition const& other);
            
            StronglyConnectedComponentDecomposition& operator=(StronglyConnectedComponentDecomposition const& other);
            
            StronglyConnectedComponentDecomposition(StronglyConnectedComponentDecomposition&& other);
            
            StronglyConnectedComponentDecomposition& operator=(StronglyConnectedComponentDecomposition&& other);
            
        private:
            void performSccDecomposition(storm::models::AbstractModel<T> const& model);
            
            void performSccDecompositionHelper(storm::models::AbstractModel<T> const& model, uint_fast64_t startState, uint_fast64_t& currentIndex, std::vector<uint_fast64_t>& stateIndices, std::vector<uint_fast64_t>& lowlinks, std::vector<uint_fast64_t>& tarjanStack, storm::storage::BitVector& tarjanStackStates, storm::storage::BitVector& visitedStates);
        };
    }
}

#endif /* STORM_STORAGE_STRONGLYCONNECTEDCOMPONENTDECOMPOSITION_H_ */
