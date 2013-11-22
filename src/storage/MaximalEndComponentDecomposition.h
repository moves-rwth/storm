#ifndef STORM_STORAGE_MAXIMALENDCOMPONENTDECOMPOSITION_H_
#define STORM_STORAGE_MAXIMALENDCOMPONENTDECOMPOSITION_H_

#include "src/storage/Decomposition.h"
#include "src/storage/StronglyConnectedComponentDecomposition.h"
#include "src/storage/MaximalEndComponent.h"
#include "src/models/AbstractNondeterministicModel.h"

namespace storm  {
    namespace storage {
        
        template <typename ValueType>
        class MaximalEndComponentDecomposition : public Decomposition<MaximalEndComponent> {
        public:
            /*
             * Creates an empty MEC decomposition.
             */
            MaximalEndComponentDecomposition();
            
            /*
             * Creates an MEC decomposition of the given model.
             *
             * @param model The model to decompose into MECs.
             */
            MaximalEndComponentDecomposition(storm::models::AbstractNondeterministicModel<ValueType> const& model);
            
            MaximalEndComponentDecomposition(storm::models::AbstractNondeterministicModel<ValueType> const& model, storm::storage::BitVector const& subsystem);
            
            MaximalEndComponentDecomposition(MaximalEndComponentDecomposition const& other);
            
            MaximalEndComponentDecomposition& operator=(MaximalEndComponentDecomposition const& other);
            
            MaximalEndComponentDecomposition(MaximalEndComponentDecomposition&& other);
            
            MaximalEndComponentDecomposition& operator=(MaximalEndComponentDecomposition&& other);
            
        private:
            /*!
             * Performs the actual decomposition of the given model into MECs. Stores the MECs found in the current decomposition
             * as a side-effect.
             *
             * @param model The model to decompose.
             */
            void performMaximalEndComponentDecomposition(storm::models::AbstractNondeterministicModel<ValueType> const& model, storm::storage::BitVector const& subsystem);
        };
    }
}

#endif /* STORM_STORAGE_MAXIMALENDCOMPONENTDECOMPOSITION_H_ */
