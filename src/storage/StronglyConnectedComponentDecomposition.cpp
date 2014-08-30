#include "src/storage/StronglyConnectedComponentDecomposition.h"
#include "src/models/AbstractModel.h"

namespace storm {
    namespace storage {
        template<typename ValueType>
        StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition() : Decomposition() {
            // Intentionally left empty.
        }

        template <typename ValueType>
        StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(storm::models::AbstractModel<ValueType> const& model, bool dropNaiveSccs, bool onlyBottomSccs) : Decomposition() {
            performSccDecomposition(model, dropNaiveSccs, onlyBottomSccs);
        }
        
        template <typename ValueType>
        StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(storm::models::AbstractModel<ValueType> const& model, StateBlock const& block, bool dropNaiveSccs, bool onlyBottomSccs) {
            storm::storage::BitVector subsystem(model.getNumberOfStates(), block.begin(), block.end());
            performSccDecomposition(model, subsystem, dropNaiveSccs, onlyBottomSccs);
        }
        
        template <typename ValueType>
        StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(storm::models::AbstractModel<ValueType> const& model, storm::storage::BitVector const& subsystem, bool dropNaiveSccs, bool onlyBottomSccs) {
            performSccDecomposition(model, subsystem, dropNaiveSccs, onlyBottomSccs);
        }
        
        template <typename ValueType>
        StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(StronglyConnectedComponentDecomposition const& other) : Decomposition(other) {
            // Intentionally left empty.
        }
        
        template <typename ValueType>
        StronglyConnectedComponentDecomposition<ValueType>& StronglyConnectedComponentDecomposition<ValueType>::operator=(StronglyConnectedComponentDecomposition const& other) {
            this->blocks = other.blocks;
            return *this;
        }
        
        template <typename ValueType>
        StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(StronglyConnectedComponentDecomposition&& other) : Decomposition(std::move(other)) {
            // Intentionally left empty.
        }
        
        template <typename ValueType>
        StronglyConnectedComponentDecomposition<ValueType>& StronglyConnectedComponentDecomposition<ValueType>::operator=(StronglyConnectedComponentDecomposition&& other) {
            this->blocks = std::move(other.blocks);
            return *this;
        }

        template <typename ValueType>
        void StronglyConnectedComponentDecomposition<ValueType>::performSccDecomposition(storm::models::AbstractModel<ValueType> const& model, storm::storage::BitVector const& subsystem, bool dropNaiveSccs, bool onlyBottomSccs) {
            uint_fast64_t numberOfStates = model.getNumberOfStates();

            // Set up the environment of the algorithm.
            // Start with the two stacks it maintains.
            std::vector<uint_fast64_t> s;
            s.reserve(numberOfStates);
            std::vector<uint_fast64_t> p;
            p.reserve(numberOfStates);
            
            // We also need to store the preorder numbers of states and which states have been assigned to which SCC.
            std::vector<uint_fast64_t> preorderNumbers(numberOfStates);
            storm::storage::BitVector hasPreorderNumber(numberOfStates);
            storm::storage::BitVector stateHasScc(numberOfStates);
            std::vector<uint_fast64_t> stateToSccMapping(numberOfStates);
            uint_fast64_t sccCount = 0;
            
            // Finally, we need to keep track of the states with a self-loop to identify naive SCCs.
            storm::storage::BitVector statesWithSelfLoop(numberOfStates);
            
            // Start the search for SCCs from every state in the block.
            uint_fast64_t currentIndex = 0;
            for (auto state : subsystem) {
                if (!hasPreorderNumber.get(state)) {
                    performSccDecompositionGCM(model, state, statesWithSelfLoop, subsystem, currentIndex, hasPreorderNumber, preorderNumbers, s, p, stateHasScc, stateToSccMapping, sccCount);
                }
            }

            // After we obtained the state-to-SCC mapping, we build the actual blocks.
            this->blocks.resize(sccCount);
            for (uint_fast64_t state = 0; state < numberOfStates; ++state) {
                this->blocks[stateToSccMapping[state]].insert(state);
            }
            
            // If requested, we need to drop some SCCs.
            if (onlyBottomSccs || dropNaiveSccs) {
                storm::storage::BitVector blocksToDrop(sccCount);
                
                // If requested, we need to delete all naive SCCs.
                if (dropNaiveSccs) {
                    for (uint_fast64_t sccIndex = 0; sccIndex < sccCount; ++sccIndex) {
                        if (this->blocks[sccIndex].size() == 1) {
                            uint_fast64_t onlyState = *this->blocks[sccIndex].begin();
                            
                            if (!statesWithSelfLoop.get(onlyState)) {
                                blocksToDrop.set(sccIndex);
                            }
                        }
                    }
                }
                
                // If requested, we need to drop all non-bottom SCCs.
                if (onlyBottomSccs) {
                    for (uint_fast64_t state = 0; state < numberOfStates; ++state) {
                        // If the block of the state is already known to be dropped, we don't need to check the transitions.
                        if (!blocksToDrop.get(stateToSccMapping[state])) {
                            for (typename storm::storage::SparseMatrix<ValueType>::const_iterator successorIt = model.getRows(state).begin(), successorIte = model.getRows(state).end(); successorIt != successorIte; ++successorIt) {
                                if (subsystem.get(successorIt->getColumn()) && stateToSccMapping[state] != stateToSccMapping[successorIt->getColumn()]) {
                                    blocksToDrop.set(stateToSccMapping[state]);
                                    break;
                                }
                            }
                        }
                    }
                }
                
                // Create the new set of blocks by moving all the blocks we need to keep into it.
                std::vector<Block> newBlocks((~blocksToDrop).getNumberOfSetBits());
                uint_fast64_t currentBlock = 0;
                for (uint_fast64_t blockIndex = 0; blockIndex < this->blocks.size(); ++blockIndex) {
                    if (!blocksToDrop.get(blockIndex)) {
                        newBlocks[currentBlock] = std::move(this->blocks[blockIndex]);
                        ++currentBlock;
                    }
                }
                
                // Now set this new set of blocks as the result of the decomposition.
                this->blocks = std::move(newBlocks);
            }
        }
        
        template <typename ValueType>
        void StronglyConnectedComponentDecomposition<ValueType>::performSccDecomposition(storm::models::AbstractModel<ValueType> const& model, bool dropNaiveSccs, bool onlyBottomSccs) {
            // Prepare a block that contains all states for a call to the other overload of this function.
            storm::storage::BitVector fullSystem(model.getNumberOfStates(), true);
            
            // Call the overloaded function.
            performSccDecomposition(model, fullSystem, dropNaiveSccs, onlyBottomSccs);
        }
        
        template <typename ValueType>
        void StronglyConnectedComponentDecomposition<ValueType>::performSccDecompositionGCM(storm::models::AbstractModel<ValueType> const& model, uint_fast64_t startState, storm::storage::BitVector& statesWithSelfLoop, storm::storage::BitVector const& subsystem, uint_fast64_t& currentIndex, storm::storage::BitVector& hasPreorderNumber, std::vector<uint_fast64_t>& preorderNumbers, std::vector<uint_fast64_t>& s, std::vector<uint_fast64_t>& p, storm::storage::BitVector& stateHasScc, std::vector<uint_fast64_t>& stateToSccMapping, uint_fast64_t& sccCount) {
            
            // Prepare the stack used for turning the recursive procedure into an iterative one.
            std::vector<uint_fast64_t> recursionStateStack;
            recursionStateStack.reserve(model.getNumberOfStates());
            recursionStateStack.push_back(startState);
            std::vector<typename storm::storage::SparseMatrix<ValueType>::const_iterator> recursionIteratorStack;
            recursionIteratorStack.push_back(model.getRows(startState).begin());

            while (!recursionStateStack.empty()) {
                uint_fast64_t currentState = recursionStateStack.back();
                typename storm::storage::SparseMatrix<ValueType>::const_iterator& successorIt = recursionIteratorStack.back();

                if (!hasPreorderNumber.get(currentState)) {
                    preorderNumbers[currentState] = currentIndex++;
                    hasPreorderNumber.set(currentState, true);
                
                    s.push_back(currentState);
                    p.push_back(currentState);
                }
                
                bool recursionStepIn = false;
                for (; successorIt != model.getRows(currentState).end(); ++successorIt) {
                    if (subsystem.get(successorIt->getColumn())) {
                        if (currentState == successorIt->getColumn()) {
                            statesWithSelfLoop.set(currentState);
                        }
                        
                        if (!hasPreorderNumber.get(successorIt->getColumn())) {
                            // In this case, we must recursively visit the successor. We therefore push the state onto the recursion stack an break the for-loop.
                            recursionStateStack.push_back(successorIt->getColumn());
                            recursionStepIn = true;
                            
                            recursionIteratorStack.push_back(model.getRows(successorIt->getColumn()).begin());
                            break;
                        } else {
                            if (!stateHasScc.get(successorIt->getColumn())) {
                                while (preorderNumbers[p.back()] > preorderNumbers[successorIt->getColumn()]) {
                                    p.pop_back();
                                }
                            }
                        }
                    }
                }
                
                if (!recursionStepIn) {
                    if (currentState == p.back()) {
                        p.pop_back();
                        uint_fast64_t poppedState = 0;
                        do {
                            poppedState = s.back();
                            s.pop_back();
                            stateToSccMapping[poppedState] = sccCount;
                            stateHasScc.set(poppedState);
                        } while (poppedState != currentState);
                        ++sccCount;
                    }
                    
                    recursionStateStack.pop_back();
                    recursionIteratorStack.pop_back();
                }
                recursionStepIn = false;
            }
        }
        
        // Explicitly instantiate the SCC decomposition.
        template class StronglyConnectedComponentDecomposition<double>;
    } // namespace storage
} // namespace storm