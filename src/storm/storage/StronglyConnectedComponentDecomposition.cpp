#include "storm/storage/StronglyConnectedComponentDecomposition.h"
#include <storm/utility/vector.h>
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/models/sparse/Model.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/Stopwatch.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/UnexpectedException.h"

namespace storm {
namespace storage {
template<typename ValueType>
StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition() : Decomposition() {
    // Intentionally left empty.
}

template<typename ValueType>
StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                                                            StronglyConnectedComponentDecompositionOptions const& options) {
    performSccDecomposition(transitionMatrix, options);
}

template<typename ValueType>
StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(StronglyConnectedComponentDecomposition const& other)
    : Decomposition(other) {
    // Intentionally left empty.
}

template<typename ValueType>
StronglyConnectedComponentDecomposition<ValueType>& StronglyConnectedComponentDecomposition<ValueType>::operator=(
    StronglyConnectedComponentDecomposition const& other) {
    this->blocks = other.blocks;
    return *this;
}

template<typename ValueType>
StronglyConnectedComponentDecomposition<ValueType>::StronglyConnectedComponentDecomposition(StronglyConnectedComponentDecomposition&& other)
    : Decomposition(std::move(other)) {
    // Intentionally left empty.
}

template<typename ValueType>
StronglyConnectedComponentDecomposition<ValueType>& StronglyConnectedComponentDecomposition<ValueType>::operator=(
    StronglyConnectedComponentDecomposition&& other) {
    this->blocks = std::move(other.blocks);
    return *this;
}

/*!
 * Uses the algorithm by Gabow/Cheriyan/Mehlhorn ("Path-based strongly connected component algorithm") to
 * compute a mapping of states to their SCCs. All arguments given by (non-const) reference are modified by
 * the function as a side-effect.
 *
 * @param transitionMatrix The transition matrix of the system to decompose.
 * @param startState The starting state for the search of Tarjan's algorithm.
 * @param nonTrivialStates A bit vector where entries for non-trivial states (states that either have a selfloop or whose SCC is not a singleton) will be set to
 * true
 * @param subsystem An optional bit vector indicating which subsystem to consider.
 * @param choices An optional bit vector indicating which choices belong to the subsystem.
 * @param currentIndex The next free index that can be assigned to states.
 * @param hasPreorderNumber A bit that is used to keep track of the states that already have a preorder number.
 * @param preorderNumbers A vector storing the preorder number for each state.
 * @param recursionStateStack memory used for the recursion stack.
 * @param s The stack S used by the algorithm.
 * @param p The stack S used by the algorithm.
 * @param stateHasScc A bit vector containing all states that have already been assigned to an SCC.
 * @param stateToSccMapping A mapping from states to the SCC indices they belong to. As a side effect of this
 * function this mapping is filled (for all states reachable from the starting state).
 * @param sccCount The number of SCCs that have been computed. As a side effect of this function, this count
 * is increased.
 */
template<typename ValueType>
void performSccDecompositionGCM(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, uint_fast64_t startState,
                                storm::storage::BitVector& nonTrivialStates, storm::storage::BitVector const* subsystem,
                                storm::storage::BitVector const* choices, uint_fast64_t& currentIndex, storm::storage::BitVector& hasPreorderNumber,
                                std::vector<uint_fast64_t>& preorderNumbers, std::vector<uint_fast64_t>& recursionStateStack, std::vector<uint_fast64_t>& s,
                                std::vector<uint_fast64_t>& p, storm::storage::BitVector& stateHasScc, std::vector<uint_fast64_t>& stateToSccMapping,
                                uint_fast64_t& sccCount, bool /*forceTopologicalSort*/, std::vector<uint_fast64_t>* sccDepths) {
    // The forceTopologicalSort flag can be ignored as this method always generates a topological sort.

    // Prepare the stack used for turning the recursive procedure into an iterative one.
    STORM_LOG_ASSERT(recursionStateStack.empty(), "Expected an empty recursion stack.");
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

            for (uint64_t row = transitionMatrix.getRowGroupIndices()[currentState], rowEnd = transitionMatrix.getRowGroupIndices()[currentState + 1];
                 row != rowEnd; ++row) {
                if (choices && !choices->get(row)) {
                    continue;
                }

                for (auto const& successor : transitionMatrix.getRow(row)) {
                    if ((!subsystem || subsystem->get(successor.getColumn())) && successor.getValue() != storm::utility::zero<ValueType>()) {
                        if (currentState == successor.getColumn()) {
                            nonTrivialStates.set(currentState, true);
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
            }
        } else {
            // In this case, we have searched all successors of the current state and can exit the "recursion"
            // on the current state.
            if (currentState == p.back()) {
                p.pop_back();
                if (sccDepths) {
                    uint_fast64_t sccDepth = 0;
                    // Find the largest depth over successor SCCs.
                    auto stateIt = s.end();
                    do {
                        --stateIt;
                        for (uint64_t row = transitionMatrix.getRowGroupIndices()[*stateIt], rowEnd = transitionMatrix.getRowGroupIndices()[*stateIt + 1];
                             row != rowEnd; ++row) {
                            if (choices && !choices->get(row)) {
                                continue;
                            }
                            for (auto const& successor : transitionMatrix.getRow(row)) {
                                if ((!subsystem || subsystem->get(successor.getColumn())) && successor.getValue() != storm::utility::zero<ValueType>() &&
                                    stateHasScc.get(successor.getColumn())) {
                                    sccDepth = std::max(sccDepth, (*sccDepths)[stateToSccMapping[successor.getColumn()]] + 1);
                                }
                            }
                        }
                    } while (*stateIt != currentState);
                    sccDepths->push_back(sccDepth);
                }
                bool nonSingletonScc = s.back() != currentState;
                uint_fast64_t poppedState = 0;
                do {
                    poppedState = s.back();
                    s.pop_back();
                    stateToSccMapping[poppedState] = sccCount;
                    stateHasScc.set(poppedState);
                    if (nonSingletonScc) {
                        nonTrivialStates.set(poppedState, true);
                    }
                } while (poppedState != currentState);
                ++sccCount;
            }

            recursionStateStack.pop_back();
        }
    }
}

template<typename ValueType>
void StronglyConnectedComponentDecomposition<ValueType>::performSccDecomposition(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                                                 StronglyConnectedComponentDecompositionOptions const& options) {
    STORM_LOG_ASSERT(!options.choicesPtr || options.subsystemPtr, "Expecting subsystem if choices are given.");

    uint_fast64_t numberOfStates = transitionMatrix.getRowGroupCount();
    uint_fast64_t sccCount = 0;

    // We need to keep of trivial states (singleton SCCs without selfloop).
    storm::storage::BitVector nonTrivialStates(numberOfStates, false);

    // Obtain a mapping from states to the SCC it belongs to
    std::vector<uint_fast64_t> stateToSccMapping(numberOfStates);
    {
        // Set up the environment of the algorithm.
        // Start with the two stacks it maintains.
        // This is to reduce memory (re-)allocations
        std::vector<uint_fast64_t> s;
        s.reserve(numberOfStates);
        std::vector<uint_fast64_t> p;
        p.reserve(numberOfStates);
        std::vector<uint_fast64_t> recursionStateStack;
        recursionStateStack.reserve(numberOfStates);

        // We also need to store the preorder numbers of states and which states have been assigned to which SCC.
        std::vector<uint_fast64_t> preorderNumbers(numberOfStates);
        storm::storage::BitVector hasPreorderNumber(numberOfStates);
        storm::storage::BitVector stateHasScc(numberOfStates);

        // Store scc depths if requested
        std::vector<uint_fast64_t>* sccDepthsPtr = nullptr;
        sccDepths = boost::none;
        if (options.isComputeSccDepthsSet || options.areOnlyBottomSccsConsidered) {
            sccDepths = std::vector<uint_fast64_t>();
            sccDepthsPtr = &sccDepths.get();
        }

        // Start the search for SCCs from every state in the block.
        uint_fast64_t currentIndex = 0;
        if (options.subsystemPtr) {
            for (auto state : *options.subsystemPtr) {
                if (!hasPreorderNumber.get(state)) {
                    performSccDecompositionGCM(transitionMatrix, state, nonTrivialStates, options.subsystemPtr, options.choicesPtr, currentIndex,
                                               hasPreorderNumber, preorderNumbers, recursionStateStack, s, p, stateHasScc, stateToSccMapping, sccCount,
                                               options.isTopologicalSortForced, sccDepthsPtr);
                }
            }
        } else {
            for (uint64_t state = 0; state < transitionMatrix.getRowGroupCount(); ++state) {
                if (!hasPreorderNumber.get(state)) {
                    performSccDecompositionGCM(transitionMatrix, state, nonTrivialStates, options.subsystemPtr, options.choicesPtr, currentIndex,
                                               hasPreorderNumber, preorderNumbers, recursionStateStack, s, p, stateHasScc, stateToSccMapping, sccCount,
                                               options.isTopologicalSortForced, sccDepthsPtr);
                }
            }
        }
    }
    // After we obtained the state-to-SCC mapping, we build the actual blocks.
    this->blocks.resize(sccCount);
    for (uint64_t state = 0; state < transitionMatrix.getRowGroupCount(); ++state) {
        // Check if this state (and is SCC) should be considered in this decomposition.
        if ((!options.subsystemPtr || options.subsystemPtr->get(state))) {
            if (!options.areNaiveSccsDropped || nonTrivialStates.get(state)) {
                uint_fast64_t sccIndex = stateToSccMapping[state];
                if (!options.areOnlyBottomSccsConsidered || sccDepths.get()[sccIndex] == 0) {
                    this->blocks[sccIndex].insert(state);
                    if (!nonTrivialStates.get(state)) {
                        this->blocks[sccIndex].setIsTrivial(true);
                        STORM_LOG_ASSERT(this->blocks[sccIndex].size() == 1, "Unexpected number of states in a trivial SCC.");
                    }
                }
            }
        }
    }

    // If requested, we need to delete all naive SCCs.
    if (options.areOnlyBottomSccsConsidered || options.areNaiveSccsDropped) {
        storm::storage::BitVector blocksToDrop(sccCount);
        for (uint_fast64_t sccIndex = 0; sccIndex < sccCount; ++sccIndex) {
            if (this->blocks[sccIndex].empty()) {
                blocksToDrop.set(sccIndex, true);
            }
        }
        // Create the new set of blocks by moving all the blocks we need to keep into it.
        storm::utility::vector::filterVectorInPlace(this->blocks, ~blocksToDrop);
        if (this->sccDepths) {
            storm::utility::vector::filterVectorInPlace(this->sccDepths.get(), ~blocksToDrop);
        }
    }
}

template<typename ValueType>
bool StronglyConnectedComponentDecomposition<ValueType>::hasSccDepth() const {
    return sccDepths.is_initialized();
}

template<typename ValueType>
uint_fast64_t StronglyConnectedComponentDecomposition<ValueType>::getSccDepth(uint_fast64_t const& sccIndex) const {
    STORM_LOG_THROW(sccDepths.is_initialized(), storm::exceptions::InvalidOperationException,
                    "Tried to get the SCC depth but SCC depths were not computed upon construction.");
    STORM_LOG_ASSERT(sccIndex < sccDepths->size(), "SCC index " << sccIndex << " is out of range (" << sccDepths->size() << ")");
    return sccDepths.get()[sccIndex];
}

template<typename ValueType>
uint_fast64_t StronglyConnectedComponentDecomposition<ValueType>::getMaxSccDepth() const {
    STORM_LOG_THROW(sccDepths.is_initialized(), storm::exceptions::InvalidOperationException,
                    "Tried to get the maximum SCC depth but SCC depths were not computed upon construction.");
    STORM_LOG_THROW(!sccDepths->empty(), storm::exceptions::InvalidOperationException,
                    "Tried to get the maximum SCC depth but this SCC decomposition seems to be empty.");
    return *std::max_element(sccDepths->begin(), sccDepths->end());
}

// Explicitly instantiate the SCC decomposition.
template class StronglyConnectedComponentDecomposition<double>;
template class StronglyConnectedComponentDecomposition<storm::RationalNumber>;
template class StronglyConnectedComponentDecomposition<storm::RationalFunction>;
}  // namespace storage
}  // namespace storm
