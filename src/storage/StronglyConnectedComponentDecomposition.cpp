#include "src/storage/StronglyConnectedComponentDecomposition.h"
#include "src/models/AbstractModel.h"

namespace storm {
    namespace storage {
        template<typename T>
        StronglyConnectedComponentDecomposition<T>::StronglyConnectedComponentDecomposition() : Decomposition() {
            // Intentionally left empty.
        }

        template <typename T>
        StronglyConnectedComponentDecomposition<T>::StronglyConnectedComponentDecomposition(storm::models::AbstractModel<T> const& model) : Decomposition() {
            performSccDecomposition(model);
        }
        
        template <typename T>
        StronglyConnectedComponentDecomposition<T>::StronglyConnectedComponentDecomposition(StronglyConnectedComponentDecomposition const& other) : Decomposition(other) {
            // Intentionally left empty.
        }
        
        template <typename T>
        StronglyConnectedComponentDecomposition<T>& StronglyConnectedComponentDecomposition<T>::operator=(StronglyConnectedComponentDecomposition const& other) {
            this->blocks = other.blocks;
            return *this;
        }
        
        template <typename T>
        StronglyConnectedComponentDecomposition<T>::StronglyConnectedComponentDecomposition(StronglyConnectedComponentDecomposition&& other) : Decomposition(std::move(other)) {
            // Intentionally left empty.
        }
        
        template <typename T>
        StronglyConnectedComponentDecomposition<T>& StronglyConnectedComponentDecomposition<T>::operator=(StronglyConnectedComponentDecomposition&& other) {
            this->blocks = std::move(other.blocks);
            return *this;
        }
        
        template <typename T>
        void StronglyConnectedComponentDecomposition<T>::performSccDecomposition(storm::models::AbstractModel<T> const& model) {
            LOG4CPLUS_INFO(logger, "Computing SCC decomposition.");
            
            uint_fast64_t numberOfStates = model.getNumberOfStates();
            
            // Set up the environment of Tarjan's algorithm.
            std::vector<uint_fast64_t> tarjanStack;
            tarjanStack.reserve(numberOfStates);
            storm::storage::BitVector tarjanStackStates(numberOfStates);
            std::vector<uint_fast64_t> stateIndices(numberOfStates);
            std::vector<uint_fast64_t> lowlinks(numberOfStates);
            storm::storage::BitVector visitedStates(numberOfStates);
            
            // Start the search for SCCs from every vertex in the graph structure, because there is.
            uint_fast64_t currentIndex = 0;
            for (uint_fast64_t state = 0; state < numberOfStates; ++state) {
                if (!visitedStates.get(state)) {
                    performSccDecompositionHelper(model, state, currentIndex, stateIndices, lowlinks, tarjanStack, tarjanStackStates, visitedStates);
                }
            }
            
            LOG4CPLUS_INFO(logger, "Done computing SCC decomposition.");
        }
        
        template <typename T>
        void StronglyConnectedComponentDecomposition<T>::performSccDecompositionHelper(storm::models::AbstractModel<T> const& model, uint_fast64_t startState, uint_fast64_t& currentIndex, std::vector<uint_fast64_t>& stateIndices, std::vector<uint_fast64_t>& lowlinks, std::vector<uint_fast64_t>& tarjanStack, storm::storage::BitVector& tarjanStackStates, storm::storage::BitVector& visitedStates) {
            // Create the stacks needed for turning the recursive formulation of Tarjan's algorithm
            // into an iterative version. In particular, we keep one stack for states and one stack
            // for the iterators. The last one is not strictly needed, but reduces iteration work when
            // all successors of a particular state are considered.
            std::vector<uint_fast64_t> recursionStateStack;
            recursionStateStack.reserve(lowlinks.size());
            std::vector<typename storm::storage::SparseMatrix<T>::ConstIterator> recursionIteratorStack;
            recursionIteratorStack.reserve(lowlinks.size());
            std::vector<bool> statesInStack(lowlinks.size());
            
            // Initialize the recursion stacks with the given initial state (and its successor iterator).
            recursionStateStack.push_back(startState);
            recursionIteratorStack.push_back(model.getRows(startState).begin());
            
        recursionStepForward:
            while (!recursionStateStack.empty()) {
                uint_fast64_t currentState = recursionStateStack.back();
                typename storm::storage::SparseMatrix<T>::ConstIterator successorIt = recursionIteratorStack.back();
                
                // Perform the treatment of newly discovered state as defined by Tarjan's algorithm
                visitedStates.set(currentState, true);
                stateIndices[currentState] = currentIndex;
                lowlinks[currentState] = currentIndex;
                ++currentIndex;
                tarjanStack.push_back(currentState);
                tarjanStackStates.set(currentState, true);
                
                // Now, traverse all successors of the current state.
                for(; successorIt != model.getRows(currentState).end(); ++successorIt) {
                    // If we have not visited the successor already, we need to perform the procedure
                    // recursively on the newly found state.
                    if (!visitedStates.get(successorIt.column())) {
                        // Save current iterator position so we can continue where we left off later.
                        recursionIteratorStack.pop_back();
                        recursionIteratorStack.push_back(successorIt);
                        
                        // Put unvisited successor on top of our recursion stack and remember that.
                        recursionStateStack.push_back(successorIt.column());
                        statesInStack[successorIt.column()] = true;
                        
                        // Also, put initial value for iterator on corresponding recursion stack.
                        recursionIteratorStack.push_back(model.getRows(successorIt.column()).begin());
                        
                        // Perform the actual recursion step in an iterative way.
                        goto recursionStepForward;
                        
                    recursionStepBackward:
                        lowlinks[currentState] = std::min(lowlinks[currentState], lowlinks[successorIt.column()]);
                    } else if (tarjanStackStates.get(successorIt.column())) {
                        // Update the lowlink of the current state.
                        lowlinks[currentState] = std::min(lowlinks[currentState], stateIndices[successorIt.column()]);
                    }
                }
                
                // If the current state is the root of a SCC, we need to pop all states of the SCC from
                // the algorithm's stack.
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
                    this->blocks.emplace_back(std::move(scc));
                }
                
                // If we reach this point, we have completed the recursive descent for the current state.
                // That is, we need to pop it from the recursion stacks.
                recursionStateStack.pop_back();
                recursionIteratorStack.pop_back();
                
                // If there is at least one state under the current one in our recursion stack, we need
                // to restore the topmost state as the current state and jump to the part after the
                // original recursive call.
                if (recursionStateStack.size() > 0) {
                    currentState = recursionStateStack.back();
                    successorIt = recursionIteratorStack.back();
                    
                    goto recursionStepBackward;
                }
            }
        }
        
        template class StronglyConnectedComponentDecomposition<double>;
    } // namespace storage
} // namespace storm