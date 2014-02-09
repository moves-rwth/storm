#include "src/storage/StronglyConnectedComponentDecomposition.h"
#include "src/models/AbstractModel.h"
#include "src/models/PseudoModel.h"

namespace storm {
    namespace storage {
        template<typename ValueType>
        StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition() : Decomposition() {
            // Intentionally left empty.
        }

        template <typename ValueType>
        StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(storm::models::AbstractModel<ValueType> const& model, bool dropNaiveSccs, bool onlyBottomSccs) : Decomposition() {
			performSccDecomposition(storm::models::ModelBasedPseudoModel<ValueType>(model), dropNaiveSccs, onlyBottomSccs);
        }

		template <typename ValueType>
		StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(storm::models::AbstractPseudoModel<ValueType> const& pseudoModel, bool dropNaiveSccs, bool onlyBottomSccs) : Decomposition() {
			performSccDecomposition(pseudoModel, dropNaiveSccs, onlyBottomSccs);
		}
        
		template <typename ValueType>
        StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(storm::models::AbstractModel<ValueType> const& model, StateBlock const& block, bool dropNaiveSccs, bool onlyBottomSccs) {
			storm::models::ModelBasedPseudoModel<ValueType> encapsulatedModel(model);
			storm::storage::BitVector subsystem(encapsulatedModel.getNumberOfStates(), block.begin(), block.end());
			performSccDecomposition(*static_cast<storm::models::AbstractPseudoModel<ValueType>*>(&encapsulatedModel), subsystem, dropNaiveSccs, onlyBottomSccs);
        }

		template <typename ValueType>
		StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(storm::models::AbstractPseudoModel<ValueType> const& pseudoModel, StateBlock const& block, bool dropNaiveSccs, bool onlyBottomSccs) {
			storm::storage::BitVector subsystem(pseudoModel.getNumberOfStates(), block.begin(), block.end());
			performSccDecomposition(pseudoModel, subsystem, dropNaiveSccs, onlyBottomSccs);
		}
        
        template <typename ValueType>
        StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(storm::models::AbstractModel<ValueType> const& model, storm::storage::BitVector const& subsystem, bool dropNaiveSccs, bool onlyBottomSccs) {
			storm::models::ModelBasedPseudoModel<ValueType> encapsulatedModel(model);
			performSccDecomposition(*static_cast<storm::models::AbstractPseudoModel<ValueType>*>(&encapsulatedModel), subsystem, dropNaiveSccs, onlyBottomSccs);
        }

		template <typename ValueType>
		StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(storm::models::AbstractPseudoModel<ValueType> const& pseudoModel, storm::storage::BitVector const& subsystem, bool dropNaiveSccs, bool onlyBottomSccs) {
			performSccDecomposition(pseudoModel, subsystem, dropNaiveSccs, onlyBottomSccs);
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
		void StronglyConnectedComponentDecomposition<ValueType>::performSccDecomposition(storm::models::AbstractPseudoModel<ValueType> const& model, storm::storage::BitVector const& subsystem, bool dropNaiveSccs, bool onlyBottomSccs) {
            // Set up the environment of Tarjan's algorithm.
            uint_fast64_t numberOfStates = model.getNumberOfStates();
            std::vector<uint_fast64_t> tarjanStack;
            tarjanStack.reserve(numberOfStates);
            storm::storage::BitVector tarjanStackStates(numberOfStates);
            std::vector<uint_fast64_t> stateIndices(numberOfStates);
            std::vector<uint_fast64_t> lowlinks(numberOfStates);
            storm::storage::BitVector visitedStates(numberOfStates);
            
            // Start the search for SCCs from every state in the block.
            uint_fast64_t currentIndex = 0;
            for (auto state : subsystem) {
                if (!visitedStates.get(state)) {
                    performSccDecompositionHelper(model, state, subsystem, currentIndex, stateIndices, lowlinks, tarjanStack, tarjanStackStates, visitedStates, dropNaiveSccs, onlyBottomSccs);
                }
            }
        }

        
        template <typename ValueType>
		void StronglyConnectedComponentDecomposition<ValueType>::performSccDecomposition(storm::models::AbstractPseudoModel<ValueType> const& model, bool dropNaiveSccs, bool onlyBottomSccs) {
            // Prepare a block that contains all states for a call to the other overload of this function.
            storm::storage::BitVector fullSystem(model.getNumberOfStates(), true);
            
            // Call the overloaded function.
            performSccDecomposition(model, fullSystem, dropNaiveSccs, onlyBottomSccs);
        }
        
        template <typename ValueType>
		void StronglyConnectedComponentDecomposition<ValueType>::performSccDecompositionHelper(storm::models::AbstractPseudoModel<ValueType> const& model, uint_fast64_t startState, storm::storage::BitVector const& subsystem, uint_fast64_t& currentIndex, std::vector<uint_fast64_t>& stateIndices, std::vector<uint_fast64_t>& lowlinks, std::vector<uint_fast64_t>& tarjanStack, storm::storage::BitVector& tarjanStackStates, storm::storage::BitVector& visitedStates, bool dropNaiveSccs, bool onlyBottomSccs) {
            // Create the stacks needed for turning the recursive formulation of Tarjan's algorithm into an iterative
            // version. In particular, we keep one stack for states and one stack for the iterators. The last one is not
            // strictly needed, but reduces iteration work when all successors of a particular state are considered.
            std::vector<uint_fast64_t> recursionStateStack;
            recursionStateStack.reserve(lowlinks.size());
            std::vector<typename storm::storage::SparseMatrix<ValueType>::const_iterator> recursionIteratorStack;
            recursionIteratorStack.reserve(lowlinks.size());
            std::vector<bool> statesInStack(lowlinks.size());
            
            // Store a bit vector of all states with a self-loop to be able to detect non-trivial SCCs with only one
            // state.
            storm::storage::BitVector statesWithSelfloop;
            if (dropNaiveSccs) {
                statesWithSelfloop = storm::storage::BitVector(lowlinks.size());
            }
            
            // Initialize the recursion stacks with the given initial state (and its successor iterator).
            recursionStateStack.push_back(startState);
            recursionIteratorStack.push_back(model.getRows(startState).begin());
            
        recursionStepForward:
            while (!recursionStateStack.empty()) {
                uint_fast64_t currentState = recursionStateStack.back();
                typename storm::storage::SparseMatrix<ValueType>::const_iterator successorIt = recursionIteratorStack.back();
                
                // Perform the treatment of newly discovered state as defined by Tarjan's algorithm.
                visitedStates.set(currentState, true);
                stateIndices[currentState] = currentIndex;
                lowlinks[currentState] = currentIndex;
                ++currentIndex;
                tarjanStack.push_back(currentState);
                tarjanStackStates.set(currentState, true);
                
                // Now, traverse all successors of the current state.
                for(; successorIt != model.getRows(currentState).end(); ++successorIt) {
                    // Record if the current state has a self-loop if we are to drop naive SCCs later.
                    if (dropNaiveSccs && currentState == successorIt->first) {
                        statesWithSelfloop.set(currentState, true);
                    }
                    
                    // If we have not visited the successor already, we need to perform the procedure recursively on the
                    // newly found state, but only if it belongs to the subsystem in which we are interested.
                    if (subsystem.get(successorIt->first)) {
                        if (!visitedStates.get(successorIt->first)) {
                            // Save current iterator position so we can continue where we left off later.
                            recursionIteratorStack.pop_back();
                            recursionIteratorStack.push_back(successorIt);
                            
                            // Put unvisited successor on top of our recursion stack and remember that.
                            recursionStateStack.push_back(successorIt->first);
                            statesInStack[successorIt->first] = true;
                            
                            // Also, put initial value for iterator on corresponding recursion stack.
                            recursionIteratorStack.push_back(model.getRows(successorIt->first).begin());
                            
                            // Perform the actual recursion step in an iterative way.
                            goto recursionStepForward;
                            
                        recursionStepBackward:
                            lowlinks[currentState] = std::min(lowlinks[currentState], lowlinks[successorIt->first]);
                        } else if (tarjanStackStates.get(successorIt->first)) {
                            // Update the lowlink of the current state.
                            lowlinks[currentState] = std::min(lowlinks[currentState], stateIndices[successorIt->first]);
                        }
                    }
                }
                
                // If the current state is the root of a SCC, we need to pop all states of the SCC from the algorithm's
                // stack.
                if (lowlinks[currentState] == stateIndices[currentState]) {
                    Block scc;
                    
                    uint_fast64_t lastState = 0;
                    do {
                        // Pop topmost state from the algorithm's stack.
                        lastState = tarjanStack.back();
                        tarjanStack.pop_back();
                        tarjanStackStates.set(lastState, false);
                        
                        // Add the state to the current SCC.
                        scc.insert(lastState);
                    } while (lastState != currentState);

                    bool isBottomScc = true;
                    if (onlyBottomSccs) {
                        for (auto const& state : scc) {
                            for (auto const& successor : model.getRows(state)) {
                                if (scc.find(successor.first) == scc.end()) {
                                    isBottomScc = false;
                                    break;
                                }
                            }
                        }
                    }

                    // Now determine whether we want to keep this SCC in the decomposition.
                    // First, we need to check whether we should drop the SCC because of the requirement to not include
                    // naive SCCs.
                    bool keepScc = !dropNaiveSccs || (scc.size() > 1 || statesWithSelfloop.get(*scc.begin()));
                    // Then, we also need to make sure that it is a bottom SCC if we were required to.
                    keepScc &= !onlyBottomSccs || isBottomScc;
                    
                    // Only add the SCC if we determined to keep it, based on the given parameters.
                    if (keepScc) {
                        this->blocks.emplace_back(std::move(scc));
                    }
                }
                
                // If we reach this point, we have completed the recursive descent for the current state. That is, we
                // need to pop it from the recursion stacks.
                recursionStateStack.pop_back();
                recursionIteratorStack.pop_back();
                
                // If there is at least one state under the current one in our recursion stack, we need to restore the
                // topmost state as the current state and jump to the part after the original recursive call.
                if (recursionStateStack.size() > 0) {
                    currentState = recursionStateStack.back();
                    successorIt = recursionIteratorStack.back();
                    
                    goto recursionStepBackward;
                }
            }
        }

        // Explicitly instantiate the SCC decomposition.
        template class StronglyConnectedComponentDecomposition<double>;
    } // namespace storage
} // namespace storm