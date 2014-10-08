#ifndef STORM_STORAGE_BISIMULATIONDECOMPOSITION_H_
#define STORM_STORAGE_BISIMULATIONDECOMPOSITION_H_

#include <queue>
#include <deque>

#include "src/storage/Decomposition.h"
#include "src/models/Dtmc.h"
#include "src/storage/Distribution.h"

namespace storm {
    namespace storage {
        
        /*!
         * This class represents the decomposition model into its bisimulation quotient.
         */
        template <typename ValueType>
        class BisimulationDecomposition : public Decomposition<StateBlock> {
        public:
            BisimulationDecomposition() = default;
            
            /*!
             * Decomposes the given DTMC into equivalence classes under weak or strong bisimulation.
             */
            BisimulationDecomposition(storm::models::Dtmc<ValueType> const& model, bool weak = false);
            
        private:
            void computeBisimulationEquivalenceClasses(storm::models::Dtmc<ValueType> const& model, bool weak);
            std::size_t splitPredecessorsGraphBased(storm::models::Dtmc<ValueType> const& dtmc, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::size_t const& block, std::vector<std::size_t>& stateToBlockMapping, std::vector<storm::storage::Distribution<ValueType>>& distributions, storm::storage::BitVector& blocksInRefinementQueue, std::deque<std::size_t>& refinementQueue);
            std::size_t splitBlock(storm::models::Dtmc<ValueType> const& dtmc, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::size_t const& block, std::vector<std::size_t>& stateToBlockMapping, std::vector<storm::storage::Distribution<ValueType>>& distributions, storm::storage::BitVector& blocksInRefinementQueue, std::deque<std::size_t>& refinementQueue);
        };
    }
}

#endif /* STORM_STORAGE_BISIMULATIONDECOMPOSITION_H_ */