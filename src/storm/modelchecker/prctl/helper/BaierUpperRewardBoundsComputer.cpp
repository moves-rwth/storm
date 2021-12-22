#include "storm/modelchecker/prctl/helper/BaierUpperRewardBoundsComputer.h"

#include "storm/adapters/RationalNumberAdapter.h"

#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/StronglyConnectedComponentDecomposition.h"

#include "storm/utility/macros.h"

namespace storm {
namespace modelchecker {
namespace helper {

template<typename ValueType>
BaierUpperRewardBoundsComputer<ValueType>::BaierUpperRewardBoundsComputer(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                                          std::vector<ValueType> const& rewards,
                                                                          std::vector<ValueType> const& oneStepTargetProbabilities)
    : _transitionMatrix(transitionMatrix), _rewards(rewards), _oneStepTargetProbabilities(oneStepTargetProbabilities) {
    // Intentionally left empty.
}

template<typename ValueType>
std::vector<ValueType> BaierUpperRewardBoundsComputer<ValueType>::computeUpperBoundOnExpectedVisitingTimes(
    storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& oneStepTargetProbabilities) {
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

    // Vector that holds the result.
    std::vector<ValueType> result(transitionMatrix.getRowGroupCount());

    // Process all states as long as there are remaining ones.
    std::vector<uint64_t> newStates;
    while (!remainingStates.empty()) {
        for (auto state : remainingStates) {
            bool allChoicesValid = true;
            for (auto row = transitionMatrix.getRowGroupIndices()[state], endRow = transitionMatrix.getRowGroupIndices()[state + 1]; row < endRow; ++row) {
                if (validChoices.get(row)) {
                    continue;
                }

                bool choiceValid = false;
                for (auto const& entry : transitionMatrix.getRow(row)) {
                    if (storm::utility::isZero(entry.getValue())) {
                        continue;
                    }

                    if (!remainingStates.get(entry.getColumn())) {
                        choiceValid = true;
                        break;
                    }
                }

                if (choiceValid) {
                    validChoices.set(row);
                } else {
                    allChoicesValid = false;
                }
            }

            if (allChoicesValid) {
                newStates.push_back(state);
            }
        }

        // Compute d_t over the newly found states.
        for (auto state : newStates) {
            result[state] = storm::utility::one<ValueType>();
            for (auto row = transitionMatrix.getRowGroupIndices()[state], endRow = transitionMatrix.getRowGroupIndices()[state + 1]; row < endRow; ++row) {
                ValueType rowValue = oneStepTargetProbabilities[row];
                for (auto const& entry : transitionMatrix.getRow(row)) {
                    if (!remainingStates.get(entry.getColumn())) {
                        rowValue += entry.getValue() *
                                    (stateToScc[state] == stateToScc[entry.getColumn()] ? result[entry.getColumn()] : storm::utility::one<ValueType>());
                    }
                }
                STORM_LOG_ASSERT(rowValue > storm::utility::zero<ValueType>(), "Expected entry with value greater 0.");
                result[state] = std::min(result[state], rowValue);
            }
        }

        remainingStates.set(newStates.begin(), newStates.end(), false);
        newStates.clear();
    }
    // Transform the d_t to an upper bound for zeta(t)
    for (auto& r : result) {
        r = storm::utility::one<ValueType>() / r;
    }
    return result;
}

template<typename ValueType>
ValueType BaierUpperRewardBoundsComputer<ValueType>::computeUpperBound() {
    STORM_LOG_TRACE("Computing upper reward bounds using variant-2 of Baier et al.");
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    auto expVisits = computeUpperBoundOnExpectedVisitingTimes(_transitionMatrix, _oneStepTargetProbabilities);

    ValueType upperBound = storm::utility::zero<ValueType>();
    for (uint64_t state = 0; state < expVisits.size(); ++state) {
        ValueType maxReward = storm::utility::zero<ValueType>();
        for (auto row = _transitionMatrix.getRowGroupIndices()[state], endRow = _transitionMatrix.getRowGroupIndices()[state + 1]; row < endRow; ++row) {
            maxReward = std::max(maxReward, _rewards[row]);
        }
        upperBound += expVisits[state] * maxReward;
    }

    STORM_LOG_TRACE("Baier algorithm for reward bound computation (variant 2) computed bound " << upperBound << ".");
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    STORM_LOG_TRACE("Computed upper bounds on rewards in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.");
    return upperBound;
}

template class BaierUpperRewardBoundsComputer<double>;

#ifdef STORM_HAVE_CARL
template class BaierUpperRewardBoundsComputer<storm::RationalNumber>;
#endif
}  // namespace helper
}  // namespace modelchecker
}  // namespace storm
