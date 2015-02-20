#ifndef STORM_STORAGE_MAXIMALENDCOMPONENTDECOMPOSITION_H_
#define STORM_STORAGE_MAXIMALENDCOMPONENTDECOMPOSITION_H_

#include "src/storage/Decomposition.h"
#include "src/storage/MaximalEndComponent.h"
#include "src/models/sparse/NondeterministicModel.h"

namespace storm  {
    namespace storage {
        
        /*!
         * This class represents the decomposition of a nondeterministic model into its maximal end components.
         */
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
            MaximalEndComponentDecomposition(storm::models::sparse::NondeterministicModel<ValueType> const& model);
            
            /*!
             * Creates an MEC decomposition of the given subsystem in the given model.
             *
             * @param model The model whose subsystem to decompose into MECs.
             * @param subsystem The subsystem to decompose.
             */
            MaximalEndComponentDecomposition(storm::models::sparse::NondeterministicModel<ValueType> const& model, storm::storage::BitVector const& subsystem);
            
            /*!
             * Creates an MEC decomposition by copying the contents of the given MEC decomposition.
             *
             * @param other The MEC decomposition to copy.
             */
            MaximalEndComponentDecomposition(MaximalEndComponentDecomposition const& other);
            
            /*!
             * Assigns the contents of the given MEC decomposition to the current one by copying its contents.
             *
             * @param other The MEC decomposition from which to copy-assign.
             */
            MaximalEndComponentDecomposition& operator=(MaximalEndComponentDecomposition const& other);
            
            /*!
             * Creates an MEC decomposition by moving the contents of the given MEC decomposition.
             *
             * @param other The MEC decomposition to move.
             */
            MaximalEndComponentDecomposition(MaximalEndComponentDecomposition&& other);
            
            /*!
             * Assigns the contents of the given MEC decomposition to the current one by moving its contents.
             *
             * @param other The MEC decomposition from which to move-assign.
             */
            MaximalEndComponentDecomposition& operator=(MaximalEndComponentDecomposition&& other);
            
        private:
            /*!
             * Performs the actual decomposition of the given subsystem in the given model into MECs. As a side-effect
             * this stores the MECs found in the current decomposition.
             *
             * @param model The model whose subsystem to decompose into MECs.
             * @param subsystem The subsystem to decompose.
             */
            void performMaximalEndComponentDecomposition(storm::models::sparse::NondeterministicModel<ValueType> const& model, storm::storage::BitVector const& subsystem);
        };
    }
}

#endif /* STORM_STORAGE_MAXIMALENDCOMPONENTDECOMPOSITION_H_ */
