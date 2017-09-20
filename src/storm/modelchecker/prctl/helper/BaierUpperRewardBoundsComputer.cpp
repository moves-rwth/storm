#include "storm/modelchecker/prctl/helper/BaierUpperRewardBoundsComputer.h"

#include "storm/adapters/RationalNumberAdapter.h"

#include "storm/storage/SparseMatrix.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/StronglyConnectedComponentDecomposition.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template<typename ValueType>
            BaierUpperRewardBoundsComputer<ValueType>::BaierUpperRewardBoundsComputer(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& rewards, std::vector<ValueType> const& oneStepTargetProbabilities) : transitionMatrix(transitionMatrix), rewards(rewards), oneStepTargetProbabilities(oneStepTargetProbabilities) {
                // Intentionally left empty.
            }

            template<typename ValueType>
            std::vector<ValueType> BaierUpperRewardBoundsComputer<ValueType>::computeUpperBounds() {
                std::vector<uint64_t> stateToScc(transitionMatrix.getRowGroupCount());
                {
                    // Start with an SCC decomposition of the system.
                    storm::storage::StronglyConnectedComponentDecomposition<ValueType> sccDecomposition(transitionMatrix);
                    
                    uint64_t sccIndex = 0;
                    for (auto const& block : sccDecomposition) {
                        for (auto const& state : block) {
                            stateToScc[state] = sccIndex;
                        }
                        ++sccIndex;
                    }
                }

                // The states that we still need to assign a value.
                storm::storage::BitVector remainingStates(transitionMatrix.getRowGroupCount(), true);
                
                // A choice is valid iff it goes to non-remaining states with non-zero probability.
                storm::storage::BitVector validChoices(transitionMatrix.getRowCount());
                
                // Initially, mark all choices as valid that have non-zero probability to go to the target states directly.
                uint64_t index = 0;
                for (auto const& e : oneStepTargetProbabilities) {
                    if (!storm::utility::isZero(e)) {
                        validChoices.set(index);
                    }
                    ++index;
                }
                
                // Process all states as long as there are remaining ones.
                storm::storage::BitVector newStates(remainingStates.size());
                while (!remainingStates.empty()) {
                    for (auto state : remainingStates) {
                        bool allChoicesValid = true;
                        for (auto row = transitionMatrix.getRowGroupIndices()[state], endRow = transitionMatrix.getRowGroupIndices()[state + 1]; row < endRow; ++row) {
                            if (validChoices.get(row)) {
                                continue;
                            }
                            for (auto const& entry : transitionMatrix.getRow(row)) {
                                if (storm::utility::isZero(entry.getValue())) {
                                    continue;
                                }
                                
                                if (remainingStates.get(entry.getColumn())) {
                                    allChoicesValid = false;
                                    break;
                                }
                            }
                            
                            if (allChoicesValid) {
                                validChoices.set(row);
                            }
                        }
                        
                        if (allChoicesValid) {
                            newStates.set(state);
                            remainingStates.set(state, false);
                        }
                    }
                    
                    // Compute d_t over the newly found states.
                    ValueType d_t = storm::utility::one<ValueType>();
                    for (auto state : newStates) {
                        for (auto row = transitionMatrix.getRowGroupIndices()[state], endRow = transitionMatrix.getRowGroupIndices()[state + 1]; row < endRow; ++row) {
                            ValueType value = storm::utility::zero<ValueType>();
                            for (auto const& entry : transitionMatrix.getRow(row)) {
                                if () {
                                    
                                }
                            }
                            d_t = std::min();
                        }
                    }
                    newStates.clear();
                }
                
                
            }
            
            template class BaierUpperRewardBoundsComputer<double>;
            
#ifdef STORM_HAVE_CARL
            template class BaierUpperRewardBoundsComputer<storm::RationalNumber>;
#endif
        }
    }
}
