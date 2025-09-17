#include <algorithm>
#include <iterator>

#include "storm/modelchecker/helper/conditional/ConditionalHelper.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/environment/modelchecker/ModelCheckerEnvironment.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/modelchecker/prctl/helper/SparseMdpPrctlHelper.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/solver/SolveGoal.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/transformer/EndComponentEliminator.h"
#include "storm/utility/Extremum.h"
#include "storm/utility/KwekMehlhorn.h"
#include "storm/utility/SignalHandler.h"
#include "storm/utility/graph.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm::modelchecker {

namespace internal {

template<typename ValueType>
void eliminateEndComponents(storm::storage::BitVector possibleEcStates, bool addRowAtRepresentativeState, std::optional<uint64_t> representativeRowEntry,
                            storm::storage::SparseMatrix<ValueType>& matrix, uint64_t& initialState, storm::storage::BitVector& rowsWithSum1,
                            std::vector<ValueType>& rowValues1, storm::OptionalRef<std::vector<ValueType>> rowValues2 = {}) {
    storm::storage::MaximalEndComponentDecomposition<ValueType> ecs(matrix, matrix.transpose(true), possibleEcStates, rowsWithSum1);
    if (ecs.empty()) {
        return;  // nothing to do
    }

    storm::storage::BitVector allRowGroups(matrix.getRowGroupCount(), true);
    auto ecElimResult = storm::transformer::EndComponentEliminator<ValueType>::transform(
        matrix, ecs, allRowGroups, addRowAtRepresentativeState ? allRowGroups : ~allRowGroups, representativeRowEntry.has_value());

    // Update matrix
    matrix = std::move(ecElimResult.matrix);
    if (addRowAtRepresentativeState && representativeRowEntry) {
        auto const columnIndex = ecElimResult.oldToNewStateMapping[*representativeRowEntry];
        for (auto representativeRowIndex : ecElimResult.sinkRows) {
            auto row = matrix.getRow(representativeRowIndex);
            STORM_LOG_ASSERT(row.getNumberOfEntries() == 1, "unexpected number of entries in representative row.");
            auto& entry = *row.begin();
            entry.setColumn(columnIndex);
        }
    }

    // update vectors
    auto updateRowValue = [&ecElimResult](std::vector<ValueType>& rowValues) {
        std::vector<ValueType> newRowValues;
        newRowValues.reserve(ecElimResult.newToOldRowMapping.size());
        for (auto oldRowIndex : ecElimResult.newToOldRowMapping) {
            newRowValues.push_back(rowValues[oldRowIndex]);
        }
        rowValues = std::move(newRowValues);
        STORM_LOG_ASSERT(
            std::all_of(ecElimResult.sinkRows.begin(), ecElimResult.sinkRows.end(), [&rowValues](auto i) { return storm::utility::isZero(rowValues[i]); }),
            "Sink rows are expected to have zero value");
    };
    updateRowValue(rowValues1);
    if (rowValues2) {
        updateRowValue(*rowValues2);
    }

    // update initial state
    initialState = ecElimResult.oldToNewStateMapping[initialState];

    // update bitvector
    storm::storage::BitVector newRowsWithSum1(ecElimResult.newToOldRowMapping.size(), true);
    uint64_t newRowIndex = 0;
    for (auto oldRowIndex : ecElimResult.newToOldRowMapping) {
        if ((addRowAtRepresentativeState && !representativeRowEntry.has_value() && ecElimResult.sinkRows.get(newRowIndex)) || !rowsWithSum1.get(oldRowIndex)) {
            newRowsWithSum1.set(newRowIndex, false);
        }
        ++newRowIndex;
    }
    rowsWithSum1 = std::move(newRowsWithSum1);
}

template<typename ValueType, typename SolutionType = ValueType>
SolutionType solveMinMaxEquationSystem(storm::Environment const& env, storm::storage::SparseMatrix<ValueType> const& matrix,
                                       std::vector<ValueType> const& rowValues, storm::storage::BitVector const& rowsWithSum1,
                                       storm::solver::OptimizationDirection const dir, uint64_t const initialState) {
    // Initialize the solution vector.
    std::vector<SolutionType> x(matrix.getRowGroupCount(), storm::utility::zero<ValueType>());

    // Set up the solver.
    auto solver = storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType, SolutionType>().create(env, matrix);
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
 * @note This code is optimized for cases where not all states are reachable from the initial states.
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
    storm::storage::BitVector const maybeStates;      // Those states that can be reached from initial without reaching a terminal state
    storm::storage::BitVector const terminalStates;   // Those states where we already know the probability to reach the condition and the target value
    storm::storage::BitVector const conditionStates;  // Those states where the condition holds almost surely (under all schedulers)
    storm::storage::BitVector const universalObservationFailureStates;    // Those states where the condition is not reachable (under all schedulers)
    storm::storage::BitVector const existentialObservationFailureStates;  // Those states s where a scheduler exists that (i) does not reach the condition from
                                                                          // s and (ii) acts optimal in all terminal states
    std::map<uint64_t, ValueType> const nonZeroTargetStateValues;         // The known non-zero target values. (default is zero)
    // There are three cases of terminal states:
    // 1. conditionStates: The condition holds, so the target value is the optimal probability to reach target from there
    // 2. targetStates: The target is reached, so the target value is the optimal probability to reach a condition from there.
    //                  The remaining probability mass is the probability of an observation failure
    // 3. states that can not reach the condition under any scheduler. The target value is zero.

    // TerminalStates is a superset of conditionStates and dom(nonZeroTargetStateValues).
    // For a terminalState that is not a conditionState, it is impossible to (reach the condition and not reach the target).

    ValueType getTargetValue(uint64_t state) const {
        STORM_LOG_ASSERT(terminalStates.get(state), "Tried to get target value for non-terminal state");
        auto const it = nonZeroTargetStateValues.find(state);
        return it == nonZeroTargetStateValues.end() ? storm::utility::zero<ValueType>() : it->second;
    }

    ValueType failProbability(uint64_t state) const {
        STORM_LOG_ASSERT(terminalStates.get(state), "Tried to get fail probability for non-terminal state");
        STORM_LOG_ASSERT(!conditionStates.get(state), "Tried to get fail probability for a condition state");
        // condition states have fail probability zero
        return storm::utility::one<ValueType>() - getTargetValue(state);
    }
};

template<typename ValueType>
NormalFormData<ValueType> obtainNormalForm(Environment const& env, storm::solver::OptimizationDirection const dir,
                                           storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                           storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& relevantStates,
                                           storm::storage::BitVector const& targetStates, storm::storage::BitVector const& conditionStates) {
    storm::storage::BitVector const allStates(transitionMatrix.getRowGroupCount(), true);
    auto extendedConditionStates =
        storm::utility::graph::performProb1A(transitionMatrix, transitionMatrix.getRowGroupIndices(), backwardTransitions, allStates, conditionStates);
    auto universalObservationFailureStates = storm::utility::graph::performProb0A(backwardTransitions, allStates, extendedConditionStates);
    std::map<uint64_t, ValueType> nonZeroTargetStateValues;
    auto const extendedTargetStates =
        storm::utility::graph::performProb1A(transitionMatrix, transitionMatrix.getRowGroupIndices(), backwardTransitions, allStates, targetStates);
    computeReachabilityProbabilities(env, nonZeroTargetStateValues, dir, transitionMatrix, extendedConditionStates, allStates, extendedTargetStates);
    auto const targetAndNotCondFailStates = extendedTargetStates & ~(extendedConditionStates | universalObservationFailureStates);
    computeReachabilityProbabilities(env, nonZeroTargetStateValues, dir, transitionMatrix, targetAndNotCondFailStates, allStates, extendedConditionStates);

    // get states where the optimal policy reaches the condition with positive probability
    auto terminalStatesThatReachCondition = extendedConditionStates;
    for (auto state : targetAndNotCondFailStates) {
        if (nonZeroTargetStateValues.contains(state)) {
            terminalStatesThatReachCondition.set(state, true);
        }
    }

    // get the terminal states following the three cases described above
    auto terminalStates = extendedConditionStates | extendedTargetStates | universalObservationFailureStates;
    if (storm::solver::minimize(dir)) {
        // There can be target states from which (only) the *minimal* probability to reach a condition is zero.
        // For those states, the optimal policy is to enforce observation failure.
        // States that can only reach (target states with almost sure observation failure) or observation failure will be treated as terminal states with
        // targetValue zero and failProbability one.
        terminalStates |= storm::utility::graph::performProb0A(backwardTransitions, ~terminalStates, terminalStatesThatReachCondition);
    }

    auto nonTerminalStates = ~terminalStates;

    auto existentialObservationFailureStates = storm::utility::graph::performProb0E(transitionMatrix, transitionMatrix.getRowGroupIndices(),
                                                                                    backwardTransitions, nonTerminalStates, terminalStatesThatReachCondition);

    // Restrict non-terminal states to those that are still relevant
    nonTerminalStates &= storm::utility::graph::getReachableStates(transitionMatrix, relevantStates, nonTerminalStates, terminalStates);

    return NormalFormData<ValueType>{.maybeStates = std::move(nonTerminalStates),
                                     .terminalStates = std::move(terminalStates),
                                     .conditionStates = std::move(extendedConditionStates),
                                     .universalObservationFailureStates = std::move(universalObservationFailureStates),
                                     .existentialObservationFailureStates = std::move(existentialObservationFailureStates),
                                     .nonZeroTargetStateValues = std::move(nonZeroTargetStateValues)};
}

/*!
 * Uses the restart method by Baier et al.
// @see doi.org/10.1007/978-3-642-54862-8_43
 */
template<typename ValueType, typename SolutionType = ValueType>
SolutionType computeViaRestartMethod(Environment const& env, uint64_t const initialState, storm::solver::OptimizationDirection const dir,
                                     storm::storage::SparseMatrix<ValueType> const& transitionMatrix, NormalFormData<ValueType> const& normalForm) {
    auto const& maybeStates = normalForm.maybeStates;
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
                    ValueType const targetValue = normalForm.getTargetValue(entry.getColumn());
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

    auto matrix = matrixBuilder.build();
    auto initStateInMatrix = stateToMatrixIndexMap[initialState];

    // Eliminate end components in two phases
    // First, we catch all end components that do not contain the initial state. It is possible to stay in those ECs forever
    // without reaching the condition. This is reflected by a backloop to the initial state.
    storm::storage::BitVector selectedStatesInMatrix(numMaybeStates, true);
    selectedStatesInMatrix.set(initStateInMatrix, false);
    eliminateEndComponents(selectedStatesInMatrix, true, initStateInMatrix, matrix, initStateInMatrix, rowsWithSum1, rowValues);
    // Second, eliminate the remaining ECs. These must involve the initial state and might have been introduced in the previous step.
    // A policy selecting such an EC must reach the condition with probability zero and is thus invalid.
    selectedStatesInMatrix.set(initStateInMatrix, true);
    eliminateEndComponents(selectedStatesInMatrix, false, std::nullopt, matrix, initStateInMatrix, rowsWithSum1, rowValues);

    STORM_LOG_INFO("Processed model has " << matrix.getRowGroupCount() << " states and " << matrix.getRowGroupCount() << " choices and "
                                          << matrix.getEntryCount() << " transitions.");
    // Finally, solve the equation system
    return solveMinMaxEquationSystem(env, matrix, rowValues, rowsWithSum1, dir, initStateInMatrix);
}

/*!
 * A helper class that computes (weighted) reachability probabilities for a given MDP in normal form.
 * @tparam ValueType
 * @tparam SolutionType
 */
template<typename ValueType, typename SolutionType = ValueType>
class WeightedReachabilityHelper {
   public:
    WeightedReachabilityHelper(uint64_t const initialState, storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                               NormalFormData<ValueType> const& normalForm) {
        // Determine rowgroups (states) and rows (choices) of the submatrix
        auto subMatrixRowGroups = normalForm.maybeStates;
        // Identify and eliminate the initial component to enforce that it is eventually exited
        // The initial component is the largest subset of maybestates C such that
        // (i) the initial state is contained in C
        // (ii) each state in C can be reached from the initial state while only playing actions that stay inside C or observation failure and
        // (iii) for each state in C except the initial state there is a policy that almost surely reaches an observation failure
        // An optimal scheduler can intuitively pick the best exiting action of C and enforce that all paths that satisfy the condition exit C through that
        // action. By eliminating the initial component, we ensure that only policies that actually exit C are considered. The remaining policies have
        // probability zero of satisfying the condition.
        storm::storage::BitVector initialComponentExitRows(transitionMatrix.getRowCount(), false);
        subMatrixRowGroups.set(initialState, false);  // temporarily unset initial state
        std::vector<uint64_t> dfsStack = {initialState};
        while (!dfsStack.empty()) {
            auto const state = dfsStack.back();
            dfsStack.pop_back();
            for (auto rowIndex : transitionMatrix.getRowGroupIndices(state)) {
                auto const row = transitionMatrix.getRow(rowIndex);
                if (std::all_of(row.begin(), row.end(),
                                [&normalForm](auto const& entry) { return normalForm.existentialObservationFailureStates.get(entry.getColumn()); })) {
                    for (auto const& entry : row) {
                        auto const& successor = entry.getColumn();
                        if (subMatrixRowGroups.get(successor)) {
                            subMatrixRowGroups.set(successor, false);
                            dfsStack.push_back(successor);
                        }
                    }
                } else {
                    initialComponentExitRows.set(rowIndex, true);
                }
            }
        }
        auto const numSubmatrixRows = transitionMatrix.getNumRowsInRowGroups(subMatrixRowGroups) + initialComponentExitRows.getNumberOfSetBits();
        subMatrixRowGroups.set(initialState, true);  // set initial state again, as single representative state for the initial component
        auto const numSubmatrixRowGroups = subMatrixRowGroups.getNumberOfSetBits();

        // state index mapping and initial state
        auto stateToMatrixIndexMap = subMatrixRowGroups.getNumberOfSetBitsBeforeIndices();
        initialStateInSubmatrix = stateToMatrixIndexMap[initialState];
        auto const eliminatedInitialComponentStates = normalForm.maybeStates & ~subMatrixRowGroups;
        for (auto state : eliminatedInitialComponentStates) {
            stateToMatrixIndexMap[state] = initialStateInSubmatrix;  // map all eliminated states to the initial state
        }

        // build matrix, rows that sum up to 1, target values, condition values
        storm::storage::SparseMatrixBuilder<ValueType> matrixBuilder(numSubmatrixRows, numSubmatrixRowGroups, 0, true, true, numSubmatrixRowGroups);
        rowsWithSum1 = storm::storage::BitVector(numSubmatrixRows, true);
        targetRowValues.reserve(numSubmatrixRows);
        conditionRowValues.reserve(numSubmatrixRows);
        uint64_t currentRow = 0;
        for (auto state : subMatrixRowGroups) {
            matrixBuilder.newRowGroup(currentRow);

            // Put the row processing into a lambda for avoiding code duplications
            auto processRow = [&](uint64_t origRowIndex) {
                // We make two passes. First, we find out the probability to reach an eliminated initial component state
                ValueType const eliminatedInitialComponentProbability = transitionMatrix.getConstrainedRowSum(origRowIndex, eliminatedInitialComponentStates);
                // Second, we insert the submatrix entries and find out the target and condition probabilities for this row
                ValueType targetProbability = storm::utility::zero<ValueType>();
                ValueType conditionProbability = storm::utility::zero<ValueType>();
                bool rowSumIsLess1 = false;
                bool initialStateEntryInserted = false;
                for (auto const& entry : transitionMatrix.getRow(origRowIndex)) {
                    if (normalForm.terminalStates.get(entry.getColumn())) {
                        STORM_LOG_ASSERT(!storm::utility::isZero(entry.getValue()), "Transition probability must be non-zero");
                        rowSumIsLess1 = true;
                        ValueType const scaledTargetValue = normalForm.getTargetValue(entry.getColumn()) * entry.getValue();
                        targetProbability += scaledTargetValue;
                        if (normalForm.conditionStates.get(entry.getColumn())) {
                            conditionProbability += entry.getValue();  // conditionValue of successor is 1
                        } else {
                            conditionProbability += scaledTargetValue;  // for terminal, non-condition states, the condition value equals the target value
                        }
                    } else if (!eliminatedInitialComponentStates.get(entry.getColumn())) {
                        auto const columnIndex = stateToMatrixIndexMap[entry.getColumn()];
                        if (!initialStateEntryInserted && columnIndex >= initialStateInSubmatrix) {
                            if (columnIndex == initialStateInSubmatrix) {
                                matrixBuilder.addNextValue(currentRow, initialStateInSubmatrix, eliminatedInitialComponentProbability + entry.getValue());
                            } else {
                                matrixBuilder.addNextValue(currentRow, initialStateInSubmatrix, eliminatedInitialComponentProbability);
                                matrixBuilder.addNextValue(currentRow, columnIndex, entry.getValue());
                            }
                            initialStateEntryInserted = true;
                        } else {
                            matrixBuilder.addNextValue(currentRow, columnIndex, entry.getValue());
                        }
                    }
                }
                if (rowSumIsLess1) {
                    rowsWithSum1.set(currentRow, false);
                }
                targetRowValues.push_back(targetProbability);
                conditionRowValues.push_back(conditionProbability);
                ++currentRow;
            };
            // invoke the lambda
            if (state == initialState) {
                for (auto origRowIndex : initialComponentExitRows) {
                    processRow(origRowIndex);
                }
            } else {
                for (auto origRowIndex : transitionMatrix.getRowGroupIndices(state)) {
                    processRow(origRowIndex);
                }
            }
        }
        submatrix = matrixBuilder.build();

        //  eliminate ECs if present. We already checked that the initial state can not yield observation failure, so it cannot be part of an EC.
        //  For all remaining ECs, staying in an EC forever is reflected by collecting a value of zero for both, target and condition
        storm::storage::BitVector allExceptInit(numSubmatrixRowGroups, true);
        allExceptInit.set(initialStateInSubmatrix, false);
        eliminateEndComponents<ValueType>(allExceptInit, true, std::nullopt, submatrix, initialStateInSubmatrix, rowsWithSum1, targetRowValues,
                                          conditionRowValues);
        STORM_LOG_INFO("Processed model has " << submatrix.getRowGroupCount() << " states and " << submatrix.getRowGroupCount() << " choices and "
                                              << submatrix.getEntryCount() << " transitions.");
    }

    SolutionType computeWeightedDiff(storm::Environment const& env, storm::OptimizationDirection const dir, ValueType const& targetWeight,
                                     ValueType const& conditionWeight) const {
        auto rowValues = createScaledVector(targetWeight, targetRowValues, conditionWeight, conditionRowValues);

        // Initialize the solution vector.
        std::vector<SolutionType> x(submatrix.getRowGroupCount(), storm::utility::zero<ValueType>());

        // Set up the solver.
        auto solver = storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType, SolutionType>().create(env, submatrix);
        solver->setOptimizationDirection(dir);
        solver->setRequirementsChecked();
        solver->setHasUniqueSolution(true);
        solver->setHasNoEndComponents(true);
        solver->setLowerBound(-storm::utility::one<ValueType>());
        solver->setUpperBound(storm::utility::one<ValueType>());

        solver->solveEquations(env, x, rowValues);
        return x[initialStateInSubmatrix];
    }

    auto getInternalInitialState() const {
        return initialStateInSubmatrix;
    }

    void evaluateScheduler(storm::Environment const& env, std::vector<uint64_t>& scheduler, std::vector<SolutionType>& targetResults,
                           std::vector<SolutionType>& conditionResults) const {
        if (scheduler.empty()) {
            scheduler.resize(submatrix.getRowGroupCount(), 0);
        }
        if (targetResults.empty()) {
            targetResults.resize(submatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
        }
        if (conditionResults.empty()) {
            conditionResults.resize(submatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
        }
        // apply the scheduler
        storm::solver::GeneralLinearEquationSolverFactory<ValueType> factory;
        bool const convertToEquationSystem = factory.getEquationProblemFormat(env) == storm::solver::LinearEquationSolverProblemFormat::EquationSystem;
        auto scheduledMatrix = submatrix.selectRowsFromRowGroups(scheduler, convertToEquationSystem);
        if (convertToEquationSystem) {
            scheduledMatrix.convertToEquationSystem();
        }
        auto solver = factory.create(env, std::move(scheduledMatrix));
        solver->setBounds(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>());
        solver->setCachingEnabled(true);

        std::vector<ValueType> subB(submatrix.getRowGroupCount());
        storm::utility::vector::selectVectorValues<ValueType>(subB, scheduler, submatrix.getRowGroupIndices(), targetRowValues);
        solver->solveEquations(env, targetResults, subB);

        storm::utility::vector::selectVectorValues<ValueType>(subB, scheduler, submatrix.getRowGroupIndices(), conditionRowValues);
        solver->solveEquations(env, conditionResults, subB);
    }

    template<OptimizationDirection Dir>
    bool improveScheduler(std::vector<uint64_t>& scheduler, ValueType const& lambda, std::vector<SolutionType> const& targetResults,
                          std::vector<SolutionType> const& conditionResults) {
        bool improved = false;
        for (uint64_t rowGroupIndex = 0; rowGroupIndex < scheduler.size(); ++rowGroupIndex) {
            storm::utility::Extremum<Dir, ValueType> groupValue;
            uint64_t optimalRowIndex{0};
            ValueType scheduledValue;
            for (auto rowIndex : submatrix.getRowGroupIndices(rowGroupIndex)) {
                ValueType rowValue = targetRowValues[rowIndex] - lambda * conditionRowValues[rowIndex];
                for (auto const& entry : submatrix.getRow(rowIndex)) {
                    rowValue += entry.getValue() * (targetResults[entry.getColumn()] - lambda * conditionResults[entry.getColumn()]);
                }
                if (rowIndex == scheduler[rowGroupIndex] + submatrix.getRowGroupIndices()[rowGroupIndex]) {
                    scheduledValue = rowValue;
                }
                if (groupValue &= rowValue) {
                    optimalRowIndex = rowIndex;
                }
            }
            if (scheduledValue != *groupValue) {
                scheduler[rowGroupIndex] = optimalRowIndex - submatrix.getRowGroupIndices()[rowGroupIndex];
                improved = true;
            }
        }
        return improved;
    }

   private:
    std::vector<ValueType> createScaledVector(ValueType const& w1, std::vector<ValueType> const& v1, ValueType const& w2,
                                              std::vector<ValueType> const& v2) const {
        STORM_LOG_ASSERT(v1.size() == v2.size(), "Vector sizes must match");
        std::vector<ValueType> result;
        result.reserve(v1.size());
        for (size_t i = 0; i < v1.size(); ++i) {
            result.push_back(w1 * v1[i] + w2 * v2[i]);
        }
        return result;
    }

    storm::storage::SparseMatrix<ValueType> submatrix;
    storm::storage::BitVector rowsWithSum1;
    std::vector<ValueType> targetRowValues;
    std::vector<ValueType> conditionRowValues;
    uint64_t initialStateInSubmatrix;
};

enum class BisectionMethodBounds { Simple, Advanced };
template<typename ValueType, typename SolutionType = ValueType>
SolutionType computeViaBisection(Environment const& env, BisectionMethodBounds boundOption, uint64_t const initialState,
                                 storm::solver::OptimizationDirection const dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                 NormalFormData<ValueType> const& normalForm) {
    // We currently handle sound model checking incorrectly: we would need the actual lower/upper bounds of the weightedReachabilityHelper
    STORM_LOG_WARN_COND(!env.solver().isForceSoundness(),
                        "Bisection method does not adequately handle propagation of errors. Result is not necessarily sound.");
    SolutionType const precision = [&env, boundOption]() {
        if (storm::NumberTraits<SolutionType>::IsExact || env.solver().isForceExact()) {
            STORM_LOG_WARN_COND(storm::NumberTraits<SolutionType>::IsExact && boundOption == BisectionMethodBounds::Advanced,
                                "Selected bisection method with exact precision in a setting that might not terminate.");
            return storm::utility::zero<SolutionType>();
        } else {
            return storm::utility::convertNumber<SolutionType>(env.solver().minMax().getPrecision());
        }
    }();
    bool const relative = env.solver().minMax().getRelativeTerminationCriterion();

    WeightedReachabilityHelper wrh(initialState, transitionMatrix, normalForm);
    SolutionType pMin{storm::utility::zero<SolutionType>()};
    SolutionType pMax{storm::utility::one<SolutionType>()};

    if (boundOption == BisectionMethodBounds::Advanced) {
        pMin = wrh.computeWeightedDiff(env, storm::OptimizationDirection::Minimize, storm::utility::zero<ValueType>(), storm::utility::one<ValueType>());
        pMax = wrh.computeWeightedDiff(env, storm::OptimizationDirection::Maximize, storm::utility::zero<ValueType>(), storm::utility::one<ValueType>());
        STORM_LOG_TRACE("Conditioning event bounds:\n\t Lower bound: " << storm::utility::convertNumber<double>(pMin)
                                                                       << ",\n\t Upper bound: " << storm::utility::convertNumber<double>(pMax));
    }
    storm::utility::Extremum<storm::OptimizationDirection::Maximize, SolutionType> lowerBound = storm::utility::zero<ValueType>();
    storm::utility::Extremum<storm::OptimizationDirection::Minimize, SolutionType> upperBound = storm::utility::one<ValueType>();
    SolutionType middle = (*lowerBound + *upperBound) / 2;
    for (uint64_t iterationCount = 1; true; ++iterationCount) {
        // evaluate the current middle
        SolutionType const middleValue = wrh.computeWeightedDiff(env, dir, storm::utility::one<ValueType>(), -middle);
        // update the bounds and new middle value according to the bisection method
        if (boundOption == BisectionMethodBounds::Simple) {
            if (middleValue >= storm::utility::zero<ValueType>()) {
                lowerBound &= middle;
            }
            if (middleValue <= storm::utility::zero<ValueType>()) {
                upperBound &= middle;
            }
            middle = (*lowerBound + *upperBound) / 2;  // update middle to the average of the bounds
        } else {
            STORM_LOG_ASSERT(boundOption == BisectionMethodBounds::Advanced, "Unknown bisection method bounds");
            if (middleValue >= storm::utility::zero<ValueType>()) {
                lowerBound &= middle + (middleValue / pMax);
                upperBound &= middle + (middleValue / pMin);
            }
            if (middleValue <= storm::utility::zero<ValueType>()) {
                lowerBound &= middle + (middleValue / pMin);
                upperBound &= middle + (middleValue / pMax);
            }
            // update middle to the average of the bounds, but scale it according to the middle value (which is in [-1,1])
            middle = *lowerBound + (storm::utility::one<SolutionType>() + middleValue) * (*upperBound - *lowerBound) / 2;

            if (!storm::NumberTraits<SolutionType>::IsExact && storm::utility::isAlmostZero(*upperBound - *lowerBound)) {
                if (*lowerBound > *upperBound) {
                    std::swap(*lowerBound, *upperBound);
                }
                STORM_LOG_WARN("Precision of non-exact type reached during bisection method. Result might be inaccurate.");
            } else {
                STORM_LOG_ASSERT(middle >= *lowerBound && middle <= *upperBound, "Bisection method bounds are inconsistent.");
            }
        }
        // check for convergence
        SolutionType const boundDiff = *upperBound - *lowerBound;
        STORM_LOG_TRACE("Iteration #" << iterationCount << ":\n\t Lower bound:      " << storm::utility::convertNumber<double>(*lowerBound)
                                      << ",\n\t Upper bound:      " << storm::utility::convertNumber<double>(*upperBound)
                                      << ",\n\t Difference:       " << storm::utility::convertNumber<double>(boundDiff)
                                      << ",\n\t Middle val:       " << storm::utility::convertNumber<double>(middleValue) << ",\n\t Difference bound: "
                                      << storm::utility::convertNumber<double>((relative ? (precision * *lowerBound) : precision)) << ".");
        if (boundDiff <= (relative ? (precision * *lowerBound) : precision)) {
            STORM_LOG_INFO("Bisection method converged after " << iterationCount << " iterations. Difference is "
                                                               << std::setprecision(std::numeric_limits<double>::digits10)
                                                               << storm::utility::convertNumber<double>(boundDiff) << ".");
            break;
        }
        // check for early termination
        if (storm::utility::resources::isTerminate()) {
            STORM_LOG_WARN("Bisection solver aborted after " << iterationCount << "iterations. Bound difference is "
                                                             << storm::utility::convertNumber<double>(boundDiff) << ".");
            break;
        }
        // process the middle value for the next iteration
        if constexpr (storm::NumberTraits<SolutionType>::IsExact) {
            // find a rational number with a concise representation close to middle and within the bounds
            auto const exactMiddle = middle;

            // Find number of digits - 1. Method using log10 does not work since that uses doubles internally.
            auto numDigits = storm::utility::numDigits<SolutionType>(*upperBound - *lowerBound) - 1;

            do {
                ++numDigits;
                middle = storm::utility::kwek_mehlhorn::sharpen<SolutionType, SolutionType>(numDigits, exactMiddle);
            } while (middle <= *lowerBound || middle >= *upperBound);
        }
        // Since above code never sets 'middle' to exactly zero or one, we check if that could be necessary after a couple of iterations
        if (iterationCount == 8) {  // 8 is just a heuristic value, it could be any number
            if (storm::utility::isZero(*lowerBound)) {
                middle = storm::utility::zero<SolutionType>();
            } else if (storm::utility::isOne(*upperBound)) {
                middle = storm::utility::one<SolutionType>();
            }
        }
    }
    return (*lowerBound + *upperBound) / 2;
}

template<typename ValueType, typename SolutionType = ValueType>
SolutionType computeViaPolicyIteration(Environment const& env, uint64_t const initialState, storm::solver::OptimizationDirection const dir,
                                       storm::storage::SparseMatrix<ValueType> const& transitionMatrix, NormalFormData<ValueType> const& normalForm) {
    WeightedReachabilityHelper wrh(initialState, transitionMatrix, normalForm);

    std::vector<uint64_t> scheduler;
    std::vector<SolutionType> targetResults, conditionResults;
    for (uint64_t iterationCount = 1; true; ++iterationCount) {
        wrh.evaluateScheduler(env, scheduler, targetResults, conditionResults);
        STORM_LOG_WARN_COND(
            targetResults[wrh.getInternalInitialState()] <= conditionResults[wrh.getInternalInitialState()],
            "Potential numerical issues: the probability to reach the target is greater than the probability to reach the condition. Difference is "
                << (storm::utility::convertNumber<double, ValueType>(targetResults[wrh.getInternalInitialState()] -
                                                                     conditionResults[wrh.getInternalInitialState()]))
                << ".");
        ValueType const lambda = storm::utility::isZero(conditionResults[wrh.getInternalInitialState()])
                                     ? storm::utility::zero<ValueType>()
                                     : ValueType(targetResults[wrh.getInternalInitialState()] / conditionResults[wrh.getInternalInitialState()]);
        bool schedulerChanged{false};
        if (storm::solver::minimize(dir)) {
            schedulerChanged = wrh.template improveScheduler<storm::OptimizationDirection::Minimize>(scheduler, lambda, targetResults, conditionResults);
        } else {
            schedulerChanged = wrh.template improveScheduler<storm::OptimizationDirection::Maximize>(scheduler, lambda, targetResults, conditionResults);
        }
        if (!schedulerChanged) {
            STORM_LOG_INFO("Policy iteration for conditional probabilities converged after " << iterationCount << " iterations.");
            return lambda;
        }
        if (storm::utility::resources::isTerminate()) {
            STORM_LOG_WARN("Policy iteration for conditional probabilities converged aborted after " << iterationCount << "iterations.");
            return lambda;
        }
    }
}

template<typename ValueType, typename SolutionType = ValueType>
std::optional<SolutionType> handleTrivialCases(uint64_t const initialState, NormalFormData<ValueType> const& normalForm) {
    if (normalForm.terminalStates.get(initialState)) {
        STORM_LOG_DEBUG("Initial state is terminal.");
        if (normalForm.conditionStates.get(initialState)) {
            return normalForm.getTargetValue(initialState);  // The value is already known, nothing to do.
        } else {
            STORM_LOG_THROW(!normalForm.universalObservationFailureStates.get(initialState), storm::exceptions::NotSupportedException,
                            "Trying to compute undefined conditional probability: the condition has probability 0 under all policies.");
            // The last case for a terminal initial state is that it is already target and the condition is reachable with non-zero probability.
            // In this case, all schedulers induce a conditional probability of 1 (or do not reach the condition, i.e., have undefined value)
            return storm::utility::one<SolutionType>();
        }
    } else {
        // Catch the case where all terminal states have value zero
        if (normalForm.nonZeroTargetStateValues.empty()) {
            return storm::utility::zero<SolutionType>();
        };
    }
    return std::nullopt;  // No trivial case applies, we need to compute the value.
}

}  // namespace internal

template<typename ValueType, typename SolutionType>
std::unique_ptr<CheckResult> computeConditionalProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType, SolutionType>&& goal,
                                                             storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                             storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
                                                             storm::storage::BitVector const& targetStates, storm::storage::BitVector const& conditionStates) {
    // We might require adapting the precision of the solver to counter error propagation (e.g. when computing the normal form).
    auto normalFormConstructionEnv = env;
    auto analysisEnv = env;
    if (env.solver().isForceSoundness()) {
        // We intuitively have to divide the precision into two parts, one for computations when constructing the normal form and one for the actual analysis.
        // As the former is usually less numerically challenging, we use a factor of 1/10 for the normal form construction and 9/10 for the analysis.
        auto const normalFormPrecisionFactor = storm::utility::convertNumber<storm::RationalNumber, std::string>("1/10");
        normalFormConstructionEnv.solver().minMax().setPrecision(env.solver().minMax().getPrecision() * normalFormPrecisionFactor);
        analysisEnv.solver().minMax().setPrecision(env.solver().minMax().getPrecision() *
                                                   (storm::utility::one<storm::RationalNumber>() - normalFormPrecisionFactor));
    }

    // We first translate the problem into a normal form.
    // @see doi.org/10.1007/978-3-642-54862-8_43
    STORM_LOG_THROW(goal.hasRelevantValues(), storm::exceptions::NotSupportedException,
                    "No initial state given. Conditional probabilities can only be computed for models with a single initial state.");
    STORM_LOG_THROW(goal.relevantValues().hasUniqueSetBit(), storm::exceptions::NotSupportedException,
                    "Only one initial state is supported for conditional probabilities");
    STORM_LOG_TRACE("Computing conditional probabilities for a model with " << transitionMatrix.getRowGroupCount() << " states and "
                                                                            << transitionMatrix.getEntryCount() << " transitions.");
    // storm::utility::Stopwatch sw(true);
    auto normalFormData = internal::obtainNormalForm(normalFormConstructionEnv, goal.direction(), transitionMatrix, backwardTransitions, goal.relevantValues(),
                                                     targetStates, conditionStates);
    // sw.stop();
    // STORM_PRINT_AND_LOG("Time for obtaining the normal form: " << sw << ".\n");
    // Then, we solve the induced problem using the selected algorithm
    auto const initialState = *goal.relevantValues().begin();
    ValueType initialStateValue = -storm::utility::one<ValueType>();
    if (auto trivialValue = internal::handleTrivialCases<ValueType, SolutionType>(initialState, normalFormData); trivialValue.has_value()) {
        initialStateValue = *trivialValue;
        STORM_LOG_DEBUG("Initial state has trivial value " << initialStateValue);
    } else {
        STORM_LOG_ASSERT(normalFormData.maybeStates.get(initialState), "Initial state must be a maybe state if it is not a terminal state");
        auto alg = analysisEnv.modelchecker().getConditionalAlgorithmSetting();
        if (alg == ConditionalAlgorithmSetting::Default) {
            alg = ConditionalAlgorithmSetting::Restart;
        }
        STORM_LOG_INFO("Analyzing normal form with " << normalFormData.maybeStates.getNumberOfSetBits() << " maybe states using algorithm '" << alg << ".");
        // sw.restart();
        switch (alg) {
            case ConditionalAlgorithmSetting::Restart:
                initialStateValue = internal::computeViaRestartMethod(analysisEnv, initialState, goal.direction(), transitionMatrix, normalFormData);
                break;
            case ConditionalAlgorithmSetting::Bisection:
                initialStateValue = internal::computeViaBisection(analysisEnv, internal::BisectionMethodBounds::Simple, initialState, goal.direction(),
                                                                  transitionMatrix, normalFormData);
                break;
            case ConditionalAlgorithmSetting::BisectionAdvanced:
                initialStateValue = internal::computeViaBisection(analysisEnv, internal::BisectionMethodBounds::Advanced, initialState, goal.direction(),
                                                                  transitionMatrix, normalFormData);
                break;
            case ConditionalAlgorithmSetting::PolicyIteration:
                initialStateValue = internal::computeViaPolicyIteration(analysisEnv, initialState, goal.direction(), transitionMatrix, normalFormData);
                break;
            default:
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Unknown conditional probability algorithm: " << alg);
        }
        // sw.stop();
        // STORM_PRINT_AND_LOG("Time for analyzing the normal form: " << sw << ".\n");
    }
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
