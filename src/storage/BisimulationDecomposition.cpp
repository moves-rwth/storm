#include "src/storage/BisimulationDecomposition.h"

#include <queue>

namespace storm {
    namespace storage {
        
        template<typename ValueType>
        BisimulationDecomposition<ValueType>::BisimulationDecomposition(storm::models::Dtmc<ValueType> const& model, bool weak) {
            computeBisimulationEquivalenceClasses(model, weak);
        }
        
        template<typename ValueType>
        void BisimulationDecomposition<ValueType>::computeBisimulationEquivalenceClasses(storm::models::Dtmc<ValueType> const& model, bool weak) {
            // We start by computing the initial partition. In particular, we also keep a mapping of states to their blocks.
            std::vector<std::size_t> stateToBlockMapping(model.getNumberOfStates());
            storm::storage::BitVector labeledStates = model.getLabeledStates("one");
            this->blocks.emplace_back(labeledStates.begin(), labeledStates.end());
            std::for_each(labeledStates.begin(), labeledStates.end(), [&] (storm::storage::sparse::state_type const& state) { stateToBlockMapping[state] = 0; } );
            labeledStates.complement();
            this->blocks.emplace_back(labeledStates.begin(), labeledStates.end());
            std::for_each(labeledStates.begin(), labeledStates.end(), [&] (storm::storage::sparse::state_type const& state) { stateToBlockMapping[state] = 1; } );
            
            // Retrieve the backward transitions to allow for better checking of states that need to be re-examined.
            storm::storage::SparseMatrix<ValueType> const& backwardTransitions = model.getBackwardTransitions();
            
            // Initially, both blocks are potential splitters.
            std::queue<std::size_t> splitterQueue;
            splitterQueue.push(0);
            splitterQueue.push(1);

            // As long as there is a splitter, we keep refining the current partition.
            while (!splitterQueue.empty()) {
                
            }
            
            // While there is a splitter...
            //
            // check the predecessors of the splitter:
            // * if they still have the same signature as before, then they remain unsplit
            // * otherwise, split off the states that now behave differently
            //   and mark the smaller block as a splitter
            
            //
            
        }
        
        template class BisimulationDecomposition<double>;
    }
}