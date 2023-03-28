#include "storm/modelchecker/helper/finitehorizon/SparseDeterministicStepBoundedHorizonHelper.h"
#include "storm/modelchecker/hints/ExplicitModelCheckerHint.h"
#include "storm/modelchecker/prctl/helper/DsMpiUpperRewardBoundsComputer.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/utility/graph.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"

#include "storm/solver/multiplier/Multiplier.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/utility/SignalHandler.h"

#include "storm/environment/solver/MinMaxSolverEnvironment.h"

namespace storm {
namespace modelchecker {
namespace helper {

template<typename ValueType>
SparseDeterministicStepBoundedHorizonHelper<ValueType>::SparseDeterministicStepBoundedHorizonHelper() {
    // Intentionally left empty.
}

template<typename ValueType>
std::vector<ValueType> SparseDeterministicStepBoundedHorizonHelper<ValueType>::compute(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal,
                                                                                       storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                                                       storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
                                                                                       storm::storage::BitVector const& phiStates,
                                                                                       storm::storage::BitVector const& psiStates, uint64_t lowerBound,
                                                                                       uint64_t upperBound, ModelCheckerHint const& hint) {
    std::vector<ValueType> result(transitionMatrix.getRowCount(), storm::utility::zero<ValueType>());

    // If we identify the states that have probability 0 of reaching the target states, we can exclude them in the further analysis.
    storm::storage::BitVector maybeStates;
    storm::storage::BitVector makeZeroColumns;

    if (hint.isExplicitModelCheckerHint() && hint.template asExplicitModelCheckerHint<ValueType>().getComputeOnlyMaybeStates()) {
        maybeStates = hint.template asExplicitModelCheckerHint<ValueType>().getMaybeStates();
    } else {
        maybeStates = storm::utility::graph::performProbGreater0(backwardTransitions, phiStates, psiStates, true, upperBound);
        if (lowerBound == 0) {
            maybeStates &= ~psiStates;
        } else {
            makeZeroColumns = psiStates;
        }
    }

    STORM_LOG_INFO("Preprocessing: " << maybeStates.getNumberOfSetBits() << " non-target states with probability greater 0.");
    if (lowerBound == 0) {
        storm::utility::vector::setVectorValues<ValueType>(result, psiStates, storm::utility::one<ValueType>());
    }

    if (!maybeStates.empty()) {
        // We can eliminate the rows and columns from the original transition probability matrix that have probability 0.
        storm::storage::SparseMatrix<ValueType> submatrix = transitionMatrix.getSubmatrix(true, maybeStates, maybeStates, true, makeZeroColumns);

        // Create the vector of one-step probabilities to go to target states.
        std::vector<ValueType> b = transitionMatrix.getConstrainedRowSumVector(maybeStates, psiStates);

        // Create the vector with which to multiply.
        std::vector<ValueType> subresult(maybeStates.getNumberOfSetBits());

        // Perform the matrix vector multiplication
        auto multiplier = storm::solver::MultiplierFactory<ValueType>().create(env, submatrix);
        if (lowerBound == 0) {
            multiplier->repeatedMultiply(env, subresult, &b, upperBound);
        } else {
            multiplier->repeatedMultiply(env, subresult, &b, upperBound - lowerBound + 1);
            submatrix = transitionMatrix.getSubmatrix(true, maybeStates, maybeStates, true);
            multiplier = storm::solver::MultiplierFactory<ValueType>().create(env, submatrix);
            b = std::vector<ValueType>(b.size(), storm::utility::zero<ValueType>());
            multiplier->repeatedMultiply(env, subresult, &b, lowerBound - 1);
        }

        // Set the values of the resulting vector accordingly.
        storm::utility::vector::setVectorValues(result, maybeStates, subresult);
    }

    return result;
}

template class SparseDeterministicStepBoundedHorizonHelper<double>;
template class SparseDeterministicStepBoundedHorizonHelper<storm::RationalNumber>;
template class SparseDeterministicStepBoundedHorizonHelper<storm::RationalFunction>;
}  // namespace helper
}  // namespace modelchecker
}  // namespace storm
// Created by Sebastian Junges on 8/20/20.
//
