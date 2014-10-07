#ifndef STORM_STORAGE_BISIMULATIONDECOMPOSITION_H_
#define STORM_STORAGE_BISIMULATIONDECOMPOSITION_H_

#include "src/storage/Decomposition.h"
#include "src/models/Dtmc.h"

namespace storm {
    namespace storage {
        
        /*!
         * This class represents the decomposition model into its bisimulation quotient.
         */
        template <typename ValueType>
        class BisimulationDecomposition : public Decomposition<StateBlock> {
        public:
            /*!
             * Decomposes the given DTMC into equivalence classes under weak or strong bisimulation.
             */
            BisimulationDecomposition(storm::models::Dtmc<ValueType> const& model, bool weak);
            
        private:
            void computeBisimulationEquivalenceClasses(storm::models::Dtmc<ValueType> const& model, bool weak);
        };
        
    }
}

#endif /* STORM_STORAGE_BISIMULATIONDECOMPOSITION_H_ */