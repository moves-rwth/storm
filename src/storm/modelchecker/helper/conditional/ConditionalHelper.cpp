#include "storm/modelchecker/helper/conditional/ConditionalHelper.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/modelchecker/prctl/helper/SparseMdpPrctlHelper.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/solver/SolveGoal.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/transformer/EndComponentEliminator.h"
#include "storm/utility/Stopwatch.h"
#include "storm/utility/graph.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/ModelCheckerSettings.h"

namespace storm::modelchecker {

namespace internal {

template<typename ValueType, typename SolutionType = ValueType>
SolutionType solveMinMaxEquationSystem(storm::Environment const& env, storm::storage::SparseMatrix<ValueType>&& matrix, std::vector<ValueType>&& rowValues,
                                       storm::OptionalRef<storm::storage::BitVector> const& rowsWithSum1, storm::solver::OptimizationDirection const dir,
                                       uint64_t const initialState) {
    if (rowsWithSum1.has_value()) {
        storm::storage::BitVector const allRowGroups(matrix.getRowGroupCount(), true);
        storm::storage::BitVector const noRowGroups(matrix.getRowGroupCount(), false);
        storm::storage::MaximalEndComponentDecomposition<ValueType> mecs(matrix, matrix.transpose(true), allRowGroups, *rowsWithSum1);
        if (!mecs.empty()) {
            auto ecElimResult = storm::transformer::EndComponentEliminator<ValueType>::transform(matrix, mecs, allRowGroups, noRowGroups);
            std::vector<ValueType> newRowValues;
            newRowValues.reserve(ecElimResult.newToOldRowMapping.size());
            for (auto oldRowIndex : ecElimResult.newToOldRowMapping) {
                newRowValues.push_back(rowValues[oldRowIndex]);
            }
            return solveMinMaxEquationSystem<ValueType, SolutionType>(env, std::move(ecElimResult.matrix), std::move(newRowValues), storm::NullRef, dir,
                                                                      ecElimResult.oldToNewStateMapping[initialState]);
        }
    }

    // Initialize the solution vector.
    std::vector<SolutionType> x(matrix.getRowGroupCount(), storm::utility::zero<ValueType>());

    // Set up the solver.
    auto solver = storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType, SolutionType>().create(
        env, std::forward<storm::storage::SparseMatrix<ValueType>>(matrix));
    solver->setOptimizationDirection(dir);
    solver->setRequirementsChecked();
    solver->setHasUniqueSolution(true);
    solver->setHasNoEndComponents(true);
    solver->setLowerBound(storm::utility::zero<ValueType>());
    solver->setUpperBound(storm::utility::one<ValueType>());

    // Solve the corresponding system of equations.
    solver->solveEquations(env, x, rowValues);
    return x[initialState];
}

/*!
 * Computes the reachability probabilities for the given target states and inserts all non-zero values into the given map.
 * The assumption is that usually, not all states are reachable from the initial states.
 */
template<typename ValueType>
void computeReachabilityProbabilities(Environment const& env, std::map<uint64_t, ValueType>& nonZeroResults, storm::solver::OptimizationDirection const dir,
                                      storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& initialStates,
                                      storm::storage::BitVector const& allowedStates, storm::storage::BitVector const& targetStates) {
    if (initialStates.empty()) {  // nothing to do
        return;
    }
    auto const reachableStates = storm::utility::graph::getReachableStates(transitionMatrix, initialStates, allowedStates, targetStates);
    auto const subTargets = targetStates % reachableStates;
    // Catch the case where no target is reachable from an initial state. In this case, there is nothing to do since all probabilities are zero.
    if (subTargets.empty()) {
        return;
    }
    auto const subInits = initialStates % reachableStates;
    auto const submatrix = transitionMatrix.getSubmatrix(true, reachableStates, reachableStates);
    // TODO: adapt precision in sound mode
    auto const subResult = helper::SparseMdpPrctlHelper<ValueType, ValueType>::computeUntilProbabilities(
        env, storm::solver::SolveGoal<ValueType>(dir, subInits), submatrix, submatrix.transpose(true), storm::storage::BitVector(subTargets.size(), true),
        subTargets, false, false);
    auto origInitIt = initialStates.begin();
    for (auto subInit : subInits) {
        auto const& val = subResult.values[subInit];
        if (!storm::utility::isZero(val)) {
            nonZeroResults.emplace(*origInitIt, val);
        }
        ++origInitIt;
    }
}

template<typename ValueType>
struct NormalFormData {
    storm::storage::BitVector const
        definedStates;  // Those states where the property is defined, i.e., the condition states can be reached with positive probability
    storm::storage::BitVector const terminalStates;            // Those states where we already know the probability to reach the condition and the target value
    storm::storage::BitVector const conditionStates;           // Those states where the condition holds almost surely (under all schedulers)
    storm::storage::BitVector const observationFailureStates;  // Those states where the condition is not reachable (under all schedulers)
    std::map<uint64_t, ValueType> const nonZeroTargetStateValues;  // The known non-zero target values. (default is zero)
    // There are three cases of terminal states:
    // 1. conditionStates: The condition holds, so the target value is the optimal probability to reach target from there
    // 2. targetStates: The target is reached, so the target value is the optimal probability to reach a condition from there.
    //                  The remaining probability mass is the probability of an observation failure
    // 3. states that can not reach the condition under any scheduler. The target value is zero.

    // TerminalStates is a superset of conditionStates and dom(nonZeroTargetStateValues).
    // For a terminalState that is not a conditionState, it is impossible to (reach the condition and not reach the target).

    ValueType targetValue(uint64_t state) const {
        STORM_LOG_ASSERT(terminalStates.get(state), "Tried to get target value for non-terminal state");
        auto const it = nonZeroTargetStateValues.find(state);
        return it == nonZeroTargetStateValues.end() ? storm::utility::zero<ValueType>() : it->second;
    }

    ValueType failProbability(uint64_t state) const {
        STORM_LOG_ASSERT(terminalStates.get(state), "Tried to get fail probability for non-terminal state");
        STORM_LOG_ASSERT(!conditionStates.get(state), "Tried to get fail probability for a condition state");
        // condition states have fail probability zero
        return storm::utility::one<ValueType>() - targetValue(state);
    }
};

template<typename ValueType>
NormalFormData<ValueType> obtainNormalForm(Environment const& env, storm::solver::OptimizationDirection const dir,
                                           storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                           storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& targetStates,
                                           storm::storage::BitVector const& conditionStates) {
    storm::storage::BitVector const allStates(transitionMatrix.getRowGroupCount(), true);
    auto extendedConditionStates =
        storm::utility::graph::performProb1A(transitionMatrix, transitionMatrix.getRowGroupIndices(), backwardTransitions, allStates, conditionStates);
    auto observationFailureStates = storm::utility::graph::performProb0A(backwardTransitions, allStates, extendedConditionStates);
    std::map<uint64_t, ValueType> nonZeroTargetStateValues;
    auto const extendedTargetStates =
        storm::utility::graph::performProb1A(transitionMatrix, transitionMatrix.getRowGroupIndices(), backwardTransitions, allStates, targetStates);
    computeReachabilityProbabilities(env, nonZeroTargetStateValues, dir, transitionMatrix, extendedConditionStates, allStates, extendedTargetStates);
    auto const targetAndNotCondFailStates = extendedTargetStates & ~(extendedConditionStates | observationFailureStates);
    computeReachabilityProbabilities(env, nonZeroTargetStateValues, dir, transitionMatrix, targetAndNotCondFailStates, allStates, extendedConditionStates);

    auto terminalStates = extendedConditionStates | extendedTargetStates | observationFailureStates;

    if (storm::solver::minimize(dir)) {
        // There can be targetAndNotCondFailStates from which only the *minimal* probability to reach a condition is zero.
        // For those states, the optimal policy is to enforce observation failure.
        // States that can only reach (target states with almost sure observation failure) or observation failure will be treated as terminal states with
        // targetValue zero and failProbability one.

        // We first determine states where the optimal policy reaches the condition with positive probability
        auto terminalStatesThatReachCondition = extendedConditionStates;
        for (auto state : targetAndNotCondFailStates) {
            if (nonZeroTargetStateValues.contains(state)) {
                terminalStatesThatReachCondition.set(state, true);
            }
        }
        // Then, we determine the non-terminal states that cannot reach such states.
        terminalStates |= storm::utility::graph::performProb0A(backwardTransitions, ~terminalStates, terminalStatesThatReachCondition);
    }

    return NormalFormData<ValueType>{.terminalStates = std::move(terminalStates),
                                     .conditionStates = std::move(extendedConditionStates),
                                     .observationFailureStates = std::move(observationFailureStates),
                                     .nonZeroTargetStateValues = std::move(nonZeroTargetStateValues)};
}

/*!
 * Uses the restart method by Baier et al.
// @see doi.org/10.1007/978-3-642-54862-8_43
 */
template<typename ValueType, typename SolutionType = ValueType>
SolutionType computeViaRestartMethod(Environment const& env, uint64_t const initialState, storm::solver::OptimizationDirection const dir,
                                     storm::storage::SparseMatrix<ValueType> const& transitionMatrix, NormalFormData<ValueType> const& normalForm) {
    auto const maybeStates = ~normalForm.terminalStates;
    auto const stateToMatrixIndexMap = maybeStates.getNumberOfSetBitsBeforeIndices();
    auto const numMaybeStates = maybeStates.getNumberOfSetBits();
    auto const numMaybeChoices = transitionMatrix.getNumRowsInRowGroups(maybeStates);

    // Build the transitions that include a backwards loop to the initial state
    storm::storage::SparseMatrixBuilder<ValueType> matrixBuilder(numMaybeChoices, numMaybeStates, 0, true, true, numMaybeStates);
    std::vector<ValueType> rowValues;
    storm::storage::BitVector rowsWithSum1(numMaybeChoices, true);
    rowValues.reserve(numMaybeChoices);
    uint64_t currentRow = 0;
    for (auto state : maybeStates) {
        matrixBuilder.newRowGroup(currentRow);
        for (auto origRowIndex : transitionMatrix.getRowGroupIndices(state)) {
            // We make two passes over the successors. First, we find out the reset probabilities and target probabilities
            // Then, we insert the matrix entries in the correct order
            // This two-phase approach is to avoid a costly out-of-order insertion into the matrix
            ValueType targetProbability = storm::utility::zero<ValueType>();
            ValueType restartProbability = storm::utility::zero<ValueType>();
            bool rowSumIsLess1 = false;
            for (auto const& entry : transitionMatrix.getRow(origRowIndex)) {
                if (normalForm.terminalStates.get(entry.getColumn())) {
                    ValueType const targetValue = normalForm.targetValue(entry.getColumn());
                    targetProbability += targetValue * entry.getValue();
                    if (normalForm.conditionStates.get(entry.getColumn())) {
                        rowSumIsLess1 = true;
                    } else {
                        if (!storm::utility::isZero(targetValue)) {
                            rowSumIsLess1 = true;
                        }
                        restartProbability += entry.getValue() * normalForm.failProbability(entry.getColumn());
                    }
                }
            }
            if (rowSumIsLess1) {
                rowsWithSum1.set(currentRow, false);
            }
            rowValues.push_back(targetProbability);
            bool addRestartTransition = !storm::utility::isZero(restartProbability);
            for (auto const& entry : transitionMatrix.getRow(origRowIndex)) {
                // Insert backloop probability if we haven't done so yet and are past the initial state index
                // This is to avoid a costly out-of-order insertion into the matrix
                if (addRestartTransition && entry.getColumn() > initialState) {
                    matrixBuilder.addNextValue(currentRow, stateToMatrixIndexMap[initialState], restartProbability);
                    addRestartTransition = false;
                }
                if (maybeStates.get(entry.getColumn())) {
                    matrixBuilder.addNextValue(currentRow, stateToMatrixIndexMap[entry.getColumn()], entry.getValue());
                }
            }
            // Add the backloop if we haven't done this already
            if (addRestartTransition) {
                matrixBuilder.addNextValue(currentRow, stateToMatrixIndexMap[initialState], restartProbability);
            }
            ++currentRow;
        }
    }

    return solveMinMaxEquationSystem(env, matrixBuilder.build(), std::move(rowValues), rowsWithSum1, dir, stateToMatrixIndexMap[initialState]);
}

}  // namespace internal

template<typename ValueType, typename SolutionType>
std::unique_ptr<CheckResult> computeConditionalProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType, SolutionType>&& goal,
                                                             storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                             storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
                                                             storm::storage::BitVector const& targetStates, storm::storage::BitVector const& conditionStates) {
    // We first translate the problem into a normal form.
    // @see doi.org/10.1007/978-3-642-54862-8_43
    STORM_LOG_THROW(goal.relevantValues().getNumberOfSetBits() == 1, storm::exceptions::NotSupportedException,
                    "Only one initial state is supported for conditional probabilities");
    storm::utility::Stopwatch sw(true);
    auto normalFormData = internal::obtainNormalForm(env, goal.direction(), transitionMatrix, backwardTransitions, targetStates, conditionStates);
    STORM_PRINT_AND_LOG("Time for obtaining the normal form:" << sw << ".\n");

    sw.restart();
    // Then, we solve the induced problem using the selected algorithm[p
    auto const initialState = *goal.relevantValues().begin();
    ValueType initialStateValue = -storm::utility::one<ValueType>();
    auto const algString = storm::settings::getModule<storm::settings::modules::ModelCheckerSettings>().getConditionalAlgorithm();
    if (normalFormData.terminalStates.get(initialState)) {
        if (normalFormData.conditionStates.get(initialState)) {
            initialStateValue = normalFormData.targetValue(initialState);  // The value is already known, nothing to do.
        } else {
            STORM_LOG_THROW(!normalFormData.observationFailureStates.get(initialState), storm::exceptions::NotSupportedException,
                            "Trying to compute undefined conditional probability: the condition has probability 0 under all policies.");
            // The last case for a terminal initial state is that it is already target and the condition is reachable with non-zero probability.
            // In this case, all schedulers induce a conditional probability of 1 (or do not reach the condition, i.e., have undefined value)
            initialStateValue = storm::utility::one<ValueType>();
        }
    } else if (algString == "restart" || algString == "default") {
        initialStateValue = internal::computeViaRestartMethod(env, *goal.relevantValues().begin(), goal.direction(), transitionMatrix, normalFormData);
    } else {
        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Unknown conditional probability algorithm: " + algString);
    }
    STORM_PRINT_AND_LOG("Time for analyzing the normal form:" << sw << ".\n");
    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(initialState, initialStateValue));
}

template std::unique_ptr<CheckResult> computeConditionalProbabilities(Environment const& env, storm::solver::SolveGoal<double>&& goal,
                                                                      storm::storage::SparseMatrix<double> const& transitionMatrix,
                                                                      storm::storage::SparseMatrix<double> const& backwardTransitions,
                                                                      storm::storage::BitVector const& targetStates,
                                                                      storm::storage::BitVector const& conditionStates);

template std::unique_ptr<CheckResult> computeConditionalProbabilities(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal,
                                                                      storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix,
                                                                      storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions,
                                                                      storm::storage::BitVector const& targetStates,
                                                                      storm::storage::BitVector const& conditionStates);

}  // namespace storm::modelchecker
