#include "storm/storage/StronglyConnectedComponentDecomposition.h"
#include "storm/models/sparse/Model.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
    namespace storage {
        template <typename ValueType>
        StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition() : Decomposition() {
            // Intentionally left empty.
        }

        template <typename ValueType>
        template <typename RewardModelType>
        StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(storm::models::sparse::Model<ValueType, RewardModelType> const& model, bool dropNaiveSccs, bool onlyBottomSccs) : Decomposition() {
            performSccDecomposition(model, dropNaiveSccs, onlyBottomSccs);
        }
        
        template <typename ValueType>
        template <typename RewardModelType>
        StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(storm::models::sparse::Model<ValueType, RewardModelType> const& model, StateBlock const& block, bool dropNaiveSccs, bool onlyBottomSccs) {
            storm::storage::BitVector subsystem(model.getNumberOfStates(), block.begin(), block.end());
            performSccDecomposition(model.getTransitionMatrix(), subsystem, dropNaiveSccs, onlyBottomSccs);
        }
        
        template <typename ValueType>
        template <typename RewardModelType>
        StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(storm::models::sparse::Model<ValueType, RewardModelType> const& model, storm::storage::BitVector const& subsystem, bool dropNaiveSccs, bool onlyBottomSccs) {
            performSccDecomposition(model.getTransitionMatrix(), subsystem, dropNaiveSccs, onlyBottomSccs);
        }
        
        template <typename ValueType>
        StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, StateBlock const& block, bool dropNaiveSccs, bool onlyBottomSccs) {
            storm::storage::BitVector subsystem(transitionMatrix.getRowGroupCount(), block.begin(), block.end());
            performSccDecomposition(transitionMatrix, subsystem, dropNaiveSccs, onlyBottomSccs);
        }
        
        
        template <typename ValueType>
        StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, bool dropNaiveSccs, bool onlyBottomSccs) {
            performSccDecomposition(transitionMatrix, storm::storage::BitVector(transitionMatrix.getRowGroupCount(), true), dropNaiveSccs, onlyBottomSccs);
        }

        template <typename ValueType>
        StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& subsystem, bool dropNaiveSccs, bool onlyBottomSccs) {
            performSccDecomposition(transitionMatrix, subsystem, dropNaiveSccs, onlyBottomSccs);
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
        void StronglyConnectedComponentDecomposition<ValueType>::performSccDecomposition(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& subsystem, bool dropNaiveSccs, bool onlyBottomSccs) {
            uint_fast64_t numberOfStates = transitionMatrix.getRowGroupCount();

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
                    performSccDecompositionGCM(transitionMatrix, state, statesWithSelfLoop, subsystem, currentIndex, hasPreorderNumber, preorderNumbers, s, p, stateHasScc, stateToSccMapping, sccCount);
                }
            }

            // After we obtained the state-to-SCC mapping, we build the actual blocks.
            this->blocks.resize(sccCount);
            for (auto state : subsystem) {
                this->blocks[stateToSccMapping[state]].insert(state);
            }
            
            // Now flag all trivial SCCs as such.
            for (uint_fast64_t sccIndex = 0; sccIndex < sccCount; ++sccIndex) {
                if (this->blocks[sccIndex].size() == 1) {
                    uint_fast64_t onlyState = *this->blocks[sccIndex].begin();
                    
                    if (!statesWithSelfLoop.get(onlyState)) {
                        this->blocks[sccIndex].setIsTrivial(true);
                    }
                }
            }
            
            // If requested, we need to drop some SCCs.
            if (onlyBottomSccs || dropNaiveSccs) {
                storm::storage::BitVector blocksToDrop(sccCount);
                
                // If requested, we need to delete all naive SCCs.
                if (dropNaiveSccs) {
                    for (uint_fast64_t sccIndex = 0; sccIndex < sccCount; ++sccIndex) {
                        if (this->blocks[sccIndex].isTrivial()) {
                            blocksToDrop.set(sccIndex);
                        }
                    }
                }
                
                // If requested, we need to drop all non-bottom SCCs.
                if (onlyBottomSccs) {
                    for (uint_fast64_t state = 0; state < numberOfStates; ++state) {
                        // If the block of the state is already known to be dropped, we don't need to check the transitions.
                        if (!blocksToDrop.get(stateToSccMapping[state])) {
                            for (typename storm::storage::SparseMatrix<ValueType>::const_iterator successorIt = transitionMatrix.getRowGroup(state).begin(), successorIte = transitionMatrix.getRowGroup(state).end(); successorIt != successorIte; ++successorIt) {
                                if (subsystem.get(successorIt->getColumn()) && stateToSccMapping[state] != stateToSccMapping[successorIt->getColumn()]) {
                                    blocksToDrop.set(stateToSccMapping[state]);
                                    break;
                                }
                            }
                        }
                    }
                }
                
                // Create the new set of blocks by moving all the blocks we need to keep into it.
                std::vector<block_type> newBlocks((~blocksToDrop).getNumberOfSetBits());
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
        template <typename RewardModelType>
        void StronglyConnectedComponentDecomposition<ValueType>::performSccDecomposition(storm::models::sparse::Model<ValueType, RewardModelType> const& model, bool dropNaiveSccs, bool onlyBottomSccs) {
            // Prepare a block that contains all states for a call to the other overload of this function.
            storm::storage::BitVector fullSystem(model.getNumberOfStates(), true);
            
            // Call the overloaded function.
            performSccDecomposition(model.getTransitionMatrix(), fullSystem, dropNaiveSccs, onlyBottomSccs);
        }
        
        template <typename ValueType>
        void StronglyConnectedComponentDecomposition<ValueType>::performSccDecompositionGCM(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, uint_fast64_t startState, storm::storage::BitVector& statesWithSelfLoop, storm::storage::BitVector const& subsystem, uint_fast64_t& currentIndex, storm::storage::BitVector& hasPreorderNumber, std::vector<uint_fast64_t>& preorderNumbers, std::vector<uint_fast64_t>& s, std::vector<uint_fast64_t>& p, storm::storage::BitVector& stateHasScc, std::vector<uint_fast64_t>& stateToSccMapping, uint_fast64_t& sccCount) {
            
            // Prepare the stack used for turning the recursive procedure into an iterative one.
            std::vector<uint_fast64_t> recursionStateStack;
            recursionStateStack.reserve(transitionMatrix.getRowGroupCount());
            recursionStateStack.push_back(startState);

            while (!recursionStateStack.empty()) {
                // Peek at the topmost state in the stack, but leave it on there for now.
                uint_fast64_t currentState = recursionStateStack.back();

                // If the state has not yet been seen, we need to assign it a preorder number and iterate over its successors.
                if (!hasPreorderNumber.get(currentState)) {
                    preorderNumbers[currentState] = currentIndex++;
                    hasPreorderNumber.set(currentState, true);
                
                    s.push_back(currentState);
                    p.push_back(currentState);
                
                    for (auto const& successor : transitionMatrix.getRowGroup(currentState)) {
                        if (subsystem.get(successor.getColumn()) && successor.getValue() != storm::utility::zero<ValueType>()) {
                            if (currentState == successor.getColumn()) {
                                statesWithSelfLoop.set(currentState);
                            }
                            
                            if (!hasPreorderNumber.get(successor.getColumn())) {
                                // In this case, we must recursively visit the successor. We therefore push the state
                                // onto the recursion stack.
                                recursionStateStack.push_back(successor.getColumn());
                            } else {
                                if (!stateHasScc.get(successor.getColumn())) {
                                    while (preorderNumbers[p.back()] > preorderNumbers[successor.getColumn()]) {
                                        p.pop_back();
                                    }
                                }
                            }
                        }
                    }
                } else {
                    // In this case, we have searched all successors of the current state and can exit the "recursion"
                    // on the current state.
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
                }
            }
        }
        
        // Explicitly instantiate the SCC decomposition.
        template class StronglyConnectedComponentDecomposition<double>;
        template StronglyConnectedComponentDecomposition<double>::StronglyConnectedComponentDecomposition(storm::models::sparse::Model<double> const& model, bool dropNaiveSccs, bool onlyBottomSccs);
        template StronglyConnectedComponentDecomposition<double>::StronglyConnectedComponentDecomposition(storm::models::sparse::Model<double> const& model, StateBlock const& block, bool dropNaiveSccs, bool onlyBottomSccs);
        template StronglyConnectedComponentDecomposition<double>::StronglyConnectedComponentDecomposition(storm::models::sparse::Model<double> const& model, storm::storage::BitVector const& subsystem, bool dropNaiveSccs, bool onlyBottomSccs);
        
        template class StronglyConnectedComponentDecomposition<float>;
        template StronglyConnectedComponentDecomposition<float>::StronglyConnectedComponentDecomposition(storm::models::sparse::Model<float> const& model, bool dropNaiveSccs, bool onlyBottomSccs);
        template StronglyConnectedComponentDecomposition<float>::StronglyConnectedComponentDecomposition(storm::models::sparse::Model<float> const& model, StateBlock const& block, bool dropNaiveSccs, bool onlyBottomSccs);
        template StronglyConnectedComponentDecomposition<float>::StronglyConnectedComponentDecomposition(storm::models::sparse::Model<float> const& model, storm::storage::BitVector const& subsystem, bool dropNaiveSccs, bool onlyBottomSccs);

#ifdef STORM_HAVE_CARL
        template class StronglyConnectedComponentDecomposition<storm::RationalNumber>;
        template StronglyConnectedComponentDecomposition<storm::RationalNumber>::StronglyConnectedComponentDecomposition(storm::models::sparse::Model<storm::RationalNumber> const& model, bool dropNaiveSccs, bool onlyBottomSccs);
        template StronglyConnectedComponentDecomposition<storm::RationalNumber>::StronglyConnectedComponentDecomposition(storm::models::sparse::Model<storm::RationalNumber> const& model, StateBlock const& block, bool dropNaiveSccs, bool onlyBottomSccs);
        template StronglyConnectedComponentDecomposition<storm::RationalNumber>::StronglyConnectedComponentDecomposition(storm::models::sparse::Model<storm::RationalNumber> const& model, storm::storage::BitVector const& subsystem, bool dropNaiveSccs, bool onlyBottomSccs);
        
        template class StronglyConnectedComponentDecomposition<storm::RationalFunction>;
        template StronglyConnectedComponentDecomposition<storm::RationalFunction>::StronglyConnectedComponentDecomposition(storm::models::sparse::Model<storm::RationalFunction> const& model, bool dropNaiveSccs, bool onlyBottomSccs);
        template StronglyConnectedComponentDecomposition<storm::RationalFunction>::StronglyConnectedComponentDecomposition(storm::models::sparse::Model<storm::RationalFunction> const& model, StateBlock const& block, bool dropNaiveSccs, bool onlyBottomSccs);
        template StronglyConnectedComponentDecomposition<storm::RationalFunction>::StronglyConnectedComponentDecomposition(storm::models::sparse::Model<storm::RationalFunction> const& model, storm::storage::BitVector const& subsystem, bool dropNaiveSccs, bool onlyBottomSccs);
#endif
    } // namespace storage
} // namespace storm
