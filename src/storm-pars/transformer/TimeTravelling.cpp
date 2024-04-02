#include "TimeTravelling.h"
#include <_types/_uint64_t.h>
#include <carl/core/Variable.h>
#include <carl/core/VariablePool.h>
#include <sys/types.h>
#include <algorithm>
#include <cstdint>

#include <functional>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "adapters/RationalFunctionAdapter.h"
#include "adapters/RationalFunctionForward.h"

#include "adapters/RationalNumberForward.h"
#include "modelchecker/CheckTask.h"
#include "models/sparse/Dtmc.h"
#include "models/sparse/StandardRewardModel.h"
#include "models/sparse/StateLabeling.h"
#include "storage/BitVector.h"
#include "storage/FlexibleSparseMatrix.h"
#include "storage/SparseMatrix.h"
#include "utility/Stopwatch.h"
#include "utility/constants.h"
#include "utility/graph.h"
#include "utility/logging.h"
#include "utility/macros.h"

#define WRITE_DTMCS 0

namespace storm {
namespace transformer {

std::pair<storage::BitVector, storage::BitVector> findSubgraph(
        const storm::storage::FlexibleSparseMatrix<RationalFunction>& transitionMatrix,
        const uint64_t root,
        const std::map<RationalFunctionVariable, std::map<uint64_t, std::set<uint64_t>>>& treeStates,
        const boost::optional<std::vector<RationalFunction>>& stateRewardVector,
        const RationalFunctionVariable parameter
    ) {
    storm::storage::BitVector exploredStates(transitionMatrix.getRowCount(), false);
    storm::storage::BitVector bottomStates(transitionMatrix.getRowCount(), false);

    storm::storage::BitVector acyclicStates(transitionMatrix.getRowCount(), false);

    std::vector<uint64_t> dfsStack = {root};
    while (!dfsStack.empty()) {
        uint64_t state = dfsStack.back();
        if (!exploredStates.get(state)) {
            exploredStates.set(state, true);

            std::vector<uint64_t> tmpStack;
            bool isAcyclic = true;
            for (auto const& entry : transitionMatrix.getRow(state)) {
                if (!storm::utility::isZero(entry.getValue())) {
                    STORM_LOG_ASSERT(entry.getValue().isConstant() || (entry.getValue().gatherVariables().size() == 1 && *entry.getValue().gatherVariables().begin() == parameter), "Called findSubgraph with incorrect parameter.");
                    if (!exploredStates.get(entry.getColumn())) {
                        bool continueSearching = treeStates.at(parameter).count(entry.getColumn()) && !treeStates.at(parameter).at(entry.getColumn()).empty();

                        // Also continue searching if there is only a transition with a one coming up, we can skip that
                        // This is nice because we can possibly combine more transitions later
                        bool onlyHasOne = transitionMatrix.getRow(entry.getColumn()).size() == 1 &&
                                            transitionMatrix.getRow(entry.getColumn()).begin()->getValue() == utility::one<RationalFunction>();
                        continueSearching |= onlyHasOne;

                        // Don't mess with rewards
                        continueSearching &= !(stateRewardVector && !stateRewardVector->at(state).isZero());

                        if (continueSearching) {
                            tmpStack.push_back(entry.getColumn());
                        } else {
                            exploredStates.set(entry.getColumn(), true);
                            bottomStates.set(entry.getColumn(), true);

                            acyclicStates.set(entry.getColumn(), true);
                        }
                    } else {
                        if (!acyclicStates.get(entry.getColumn())) {
                            // The state has been visited before but is not known to be acyclic.
                            isAcyclic = false;
                            break;
                        }
                    }
                }
            }
            if (isAcyclic) {
                for (auto const& entry : tmpStack) {
                    dfsStack.push_back(entry);
                }
            } else {
                bottomStates.set(state, true);
            }
        } else {
            acyclicStates.set(state, true);
            dfsStack.pop_back();
        }
    }
    return std::make_pair(exploredStates, bottomStates);
}

models::sparse::Dtmc<RationalFunction> TimeTravelling::bigStep(models::sparse::Dtmc<RationalFunction> const& model,
                                                               modelchecker::CheckTask<logic::Formula, RationalFunction> const& checkTask, uint64_t horizon,
                                                               bool timeTravellingEnabled) {
    models::sparse::Dtmc<RationalFunction> dtmc(model);
    storage::SparseMatrix<RationalFunction> transitionMatrix = dtmc.getTransitionMatrix();
    uint64_t initialState = dtmc.getInitialStates().getNextSetIndex(0);

    uint64_t originalNumStates = dtmc.getNumberOfStates();

    auto allParameters = storm::models::sparse::getAllParameters(dtmc);

    std::set<std::string> labelsInFormula;
    for (auto const& atomicLabelFormula : checkTask.getFormula().getAtomicLabelFormulas()) {
        labelsInFormula.emplace(atomicLabelFormula->getLabel());
    }

    models::sparse::StateLabeling runningLabeling(dtmc.getStateLabeling());
    models::sparse::StateLabeling runningLabelingTreeStates(dtmc.getStateLabeling());
    for (auto const& label : labelsInFormula) {
        runningLabelingTreeStates.removeLabel(label);
    }

    // Check the reward model - do not touch states with rewards
    boost::optional<std::vector<RationalFunction>> stateRewardVector;
    boost::optional<std::string> stateRewardName;
    if (checkTask.getFormula().isRewardOperatorFormula()) {
        if (checkTask.isRewardModelSet()) {
            dtmc.reduceToStateBasedRewards();
            stateRewardVector = dtmc.getRewardModel(checkTask.getRewardModel()).getStateRewardVector();
            stateRewardName = checkTask.getRewardModel();
        } else {
            dtmc.reduceToStateBasedRewards();
            stateRewardVector = dtmc.getRewardModel("").getStateRewardVector();
            stateRewardName = dtmc.getUniqueRewardModelName();
        }
    }

    auto topologicalOrdering = utility::graph::getTopologicalSort<RationalFunction>(transitionMatrix, {initialState});

    auto flexibleMatrix = storage::FlexibleSparseMatrix<RationalFunction>(transitionMatrix);
    auto backwardsTransitions = storage::FlexibleSparseMatrix<RationalFunction>(transitionMatrix.transpose());

    // Initialize counting
    // Tree states: parameter p -> state s -> set of reachable states from s by constant transition that have a p-transition
    std::map<RationalFunctionVariable, std::map<uint64_t, std::set<uint64_t>>> treeStates;
    // Tree states need updating for these sets and variables
    std::map<RationalFunctionVariable, std::set<uint64_t>> treeStatesNeedUpdate;

    // Initialize treeStates and treeStatesNeedUpdate
    for (uint64_t row = 0; row < flexibleMatrix.getRowCount(); row++) {
        for (auto const& entry : flexibleMatrix.getRow(row)) {
            if (entry.getValue().isConstant()) {
                continue;
            }
            for (auto const& parameter : entry.getValue().gatherVariables()) {
                treeStatesNeedUpdate[parameter].emplace(row);
                treeStates[parameter][row].emplace(row);
            }
        }
    }
    updateTreeStates(treeStates, treeStatesNeedUpdate, flexibleMatrix, backwardsTransitions, allParameters, stateRewardVector, runningLabelingTreeStates);

    // To prevent infinite unrolling of parametric loops:
    // We have already reordered with these as leaves, don't reorder with these as leaves again
    std::map<RationalFunctionVariable, std::set<std::set<uint64_t>>> alreadyTimeTravelledToThis;

    // We will traverse the model according to the topological ordering
    std::stack<uint64_t> topologicalOrderingStack;
    topologicalOrdering = utility::graph::getTopologicalSort<RationalFunction>(transitionMatrix, {initialState});
    for (auto rit = topologicalOrdering.begin(); rit != topologicalOrdering.end(); ++rit) {
        topologicalOrderingStack.push(*rit);
    }

    // Identify reachable states - not reachable states do not have do be big-stepped
    const storage::BitVector trueVector(transitionMatrix.getRowCount(), true);
    const storage::BitVector falseVector(transitionMatrix.getRowCount(), false);
    storage::BitVector initialStates(transitionMatrix.getRowCount(), false);
    initialStates.set(initialState, true);

    // We will compute the reachable states once in the beginning but update them dynamically
    storage::BitVector reachableStates = storm::utility::graph::getReachableStates(transitionMatrix, initialStates, trueVector, falseVector);

#if WRITE_DTMCS
    uint64_t writeDtmcCounter = 0;
#endif

    while (!topologicalOrderingStack.empty()) {
        auto state = topologicalOrderingStack.top();
        topologicalOrderingStack.pop();

        if (!reachableStates.get(state)) {
            continue;
        }

        // The parameters we can do big-step w.r.t. (if horizon > 1)
        std::set<RationalFunctionVariable> bigStepParameters;
        if (horizon > 1) {
            for (auto const& parameter : allParameters) {
                if (treeStates[parameter].count(state) && treeStates.at(parameter).at(state).size() >= 1) {
                    bigStepParameters.emplace(parameter);
                }
            }
        }

        // Do big-step lifting from here
        // Follow the treeStates and eliminate transitions
        for (auto const& parameter : bigStepParameters) {
            // Find the paths along which we eliminate the transitions into one transition along with their probabilities.
            auto const [bottomAnnotations, visitedStates] = bigStepBFS(state, parameter, horizon, flexibleMatrix, treeStates, stateRewardVector);

            bool doneTimeTravelling = false;
            uint64_t oldMatrixSize = flexibleMatrix.getRowCount();

            std::vector<std::pair<uint64_t, RationalFunction>> transitions;
            if (timeTravellingEnabled) {
                transitions = findTimeTravelling(bottomAnnotations, parameter, flexibleMatrix, backwardsTransitions, alreadyTimeTravelledToThis,
                                                                   treeStatesNeedUpdate, state, originalNumStates);
            } else {
                for (auto const& [state, annotation] : bottomAnnotations) {
                    transitions.emplace_back(state, annotation.getProbability(*this, parameter));
                }
            }

            // Put paths into matrix
            replaceWithNewTransitions(state, transitions, flexibleMatrix, backwardsTransitions, reachableStates, treeStatesNeedUpdate);

            // Dynamically update unreachable states
            updateUnreachableStates(reachableStates, visitedStates, backwardsTransitions, initialState);

            uint64_t newMatrixSize = flexibleMatrix.getRowCount();
            if (newMatrixSize > oldMatrixSize) {
                // Extend labeling to more states
                runningLabeling = extendStateLabeling(runningLabeling, oldMatrixSize, newMatrixSize, state, labelsInFormula);
                runningLabelingTreeStates = extendStateLabeling(runningLabelingTreeStates, oldMatrixSize, newMatrixSize, state, labelsInFormula);

                // Extend reachableStates
                reachableStates.resize(newMatrixSize, true);

                for (uint64_t i = oldMatrixSize; i < newMatrixSize; i++) {
                    topologicalOrderingStack.push(i);
                    // New states have zero reward
                    if (stateRewardVector) {
                        stateRewardVector->push_back(storm::utility::zero<RationalFunction>());
                    }
                }
                updateTreeStates(treeStates, treeStatesNeedUpdate, flexibleMatrix, backwardsTransitions, allParameters, stateRewardVector,
                                    runningLabelingTreeStates);
            }
        }

        // STORM_LOG_ASSERT(flexibleMatrix.createSparseMatrix().transpose() == backwardsTransitions.createSparseMatrix(), "");

#if WRITE_DTMCS
        models::sparse::Dtmc<RationalFunction> newnewDTMC(flexibleMatrix.createSparseMatrix().getSubmatrix(false, reachableStates, reachableStates), runningLabeling.getSubLabeling(reachableStates));
        if (stateRewardVector) {
            models::sparse::StandardRewardModel<RationalFunction> newRewardModel(*stateRewardVector);
        }
        std::ofstream file;
        file.open("dots/bigstep_" + std::to_string(writeDtmcCounter++) + ".dot");
        newnewDTMC.writeDotToStream(file);
        file.close();
        STORM_LOG_ASSERT(newnewDTMC.getTransitionMatrix().isProbabilistic(), "Written DTMC matrix not proababilistic.");
#endif
    }

    transitionMatrix = flexibleMatrix.createSparseMatrix();

    // Delete states
    {
        storage::BitVector trueVector(transitionMatrix.getRowCount(), true);
        storage::BitVector falseVector(transitionMatrix.getRowCount(), false);
        storage::BitVector initialStates(transitionMatrix.getRowCount(), false);
        initialStates.set(initialState, true);
        storage::BitVector reachableStates = storm::utility::graph::getReachableStates(transitionMatrix, initialStates, trueVector, falseVector);

        transitionMatrix = transitionMatrix.getSubmatrix(false, reachableStates, reachableStates);
        runningLabeling = runningLabeling.getSubLabeling(reachableStates);
        uint_fast64_t newInitialState = 0;
        for (uint_fast64_t i = 0; i < initialState; i++) {
            if (reachableStates.get(i)) {
                newInitialState++;
            }
        }
        initialState = newInitialState;
        if (stateRewardVector) {
            std::vector<RationalFunction> newStateRewardVector;
            for (uint_fast64_t i = 0; i < stateRewardVector->size(); i++) {
                if (reachableStates.get(i)) {
                    newStateRewardVector.push_back(stateRewardVector->at(i));
                } else {
                    STORM_LOG_ERROR_COND(stateRewardVector->at(i).isZero(), "Deleted non-zero reward.");
                }
            }
            stateRewardVector = newStateRewardVector;
        }
    }

    models::sparse::Dtmc<RationalFunction> newDTMC(transitionMatrix, runningLabeling);

    storage::BitVector newInitialStates(transitionMatrix.getRowCount());
    newInitialStates.set(initialState, true);
    newDTMC.setInitialStates(newInitialStates);

    if (stateRewardVector) {
        models::sparse::StandardRewardModel<RationalFunction> newRewardModel(*stateRewardVector);
        newDTMC.addRewardModel(*stateRewardName, newRewardModel);
    }

    STORM_LOG_ASSERT(newDTMC.getTransitionMatrix().isProbabilistic(), "Internal error: resulting matrix not probabilistic!");

    return newDTMC;
}

std::pair<std::map<uint64_t, TimeTravelling::stateAnnotation>, std::vector<uint64_t>> TimeTravelling::bigStepBFS(
    uint64_t start, const RationalFunctionVariable& parameter, uint64_t horizon, const storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix,
    const std::map<RationalFunctionVariable, std::map<uint64_t, std::set<uint64_t>>>& treeStates,
    const boost::optional<std::vector<RationalFunction>>& stateRewardVector) {
    
    // Find the subgraph we will work on using DFS, following the treeStates, stopping before cycles
    auto const [subtree, bottomStates] = findSubgraph(flexibleMatrix, start, treeStates, stateRewardVector, parameter);

    // We need this to later determine which states are now unreachable
    std::vector<uint64_t> visitedStatesInBFSOrder;

    std::cout << "Subtree starting at " << start << std::endl;
    for (auto const& state : subtree) {
        std::cout << state << " ";
    }
    std::cout << std::endl;

    std::map<uint64_t, TimeTravelling::stateAnnotation> annotations;
    std::set<uint64_t> activeStates = {start};

    annotations[start] = TimeTravelling::stateAnnotation();
    annotations[start].annotation[std::vector<uint64_t>()] = utility::one<RationalNumber>();

    while (!activeStates.empty()) {
        std::set<uint64_t> nextActiveStates;
        for (auto const& state : activeStates) {
            visitedStatesInBFSOrder.push_back(state);
            for (auto const& entry : flexibleMatrix.getRow(state)) {
                auto const goToState = entry.getColumn();
                auto const transition = entry.getValue();

                auto& targetAnnotation = annotations[goToState].annotation;
                for (auto const& [info, constant] : annotations[state].annotation) {
                    if (transition.isConstant()) {
                        // We've seen no more instances of any parametric transition, we just update the constant value
                        if (!targetAnnotation.count(info)) {
                            targetAnnotation[info] = utility::zero<RationalNumber>();
                        }
                        targetAnnotation[info] += constant * transition.constantPart();
                    } else {
                        // We've seen a parametric transition, add that into the counter
                        auto newCounter = info;
                        auto const cacheNum = lookUpInCache(transition, parameter);
                        while (newCounter.size() <= cacheNum) {
                            newCounter.push_back(0);
                        }
                        newCounter[cacheNum]++;
                        if (targetAnnotation.count(newCounter)) {
                            // We've seen one more instance of this transition
                            targetAnnotation.at(newCounter) += constant;
                        } else {
                            targetAnnotation[newCounter] = constant;
                        }
                    }
                    if (subtree.get(goToState) && !bottomStates.get(goToState)) {
                        nextActiveStates.emplace(goToState);
                    }
                }
            }
        }
        activeStates = nextActiveStates;
    }
    // Delete annotations that are not bottom states
    for (auto const& state : subtree) {
        if (!bottomStates.get(state)) {
            annotations.erase(state);
        }
    }
    return std::make_pair(annotations, visitedStatesInBFSOrder);
}

std::vector<std::pair<uint64_t, RationalFunction>> TimeTravelling::findTimeTravelling(
    const std::map<uint64_t, TimeTravelling::stateAnnotation> bigStepAnnotations, const RationalFunctionVariable& parameter,
    storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix, storage::FlexibleSparseMatrix<RationalFunction>& backwardsFlexibleMatrix,
    std::map<RationalFunctionVariable, std::set<std::set<uint64_t>>>& alreadyTimeTravelledToThis,
    std::map<RationalFunctionVariable, std::set<uint64_t>>& treeStatesNeedUpdate, uint64_t root, uint64_t originalNumStates) {
    bool doneTimeTravelling = false;

    // Time Travelling: For transitions that divide into constants, join them into one transition leading into new state
    std::map<std::vector<uint64_t>, std::map<uint64_t, RationalNumber>> parametricTransitions;

    for (auto const& [state, annotation] : bigStepAnnotations) {
        for (auto const& [info, constant] : annotation.annotation) {
            if (!parametricTransitions.count(info)) {
                parametricTransitions[info] = std::map<uint64_t, RationalNumber>();
            }
            STORM_LOG_ASSERT(!parametricTransitions.at(info).count(state), "State already exists");
            parametricTransitions.at(info)[state] = constant;
        }
    }

    // These are the transitions that we are actually going to insert (that the function will return).
    std::vector<std::pair<uint64_t, RationalFunction>> insertTransitions;
    
    // State affected by big-step
    std::unordered_set<uint64_t> affectedStates;

    for (auto const& [factors, transitions] : parametricTransitions) {
        const auto listOfConstants = transitions;

        utility::Stopwatch stopwatch;
        std::cout << "Computing polynomial from factorization" << std::endl;
        stopwatch.start();
        auto parametricPart = polynomialFromFactorization(factors, parameter);
        stopwatch.stop();
        std::cout << "Computed " << parametricPart << " in " << stopwatch << std::endl;

        if (transitions.size() > 1) {
            // The set of target states of the paths that we maybe want to time-travel
            std::set<uint64_t> targetStates;

            // All of these states are affected by time-travelling
            for (auto const& [state, info] : transitions) {
                affectedStates.emplace(state);
                if (state < originalNumStates) {
                    targetStates.emplace(state);
                }
            }

            if ((alreadyTimeTravelledToThis[parameter].count(targetStates) && root >= originalNumStates) || targetStates.size() == 1) {
                // We already reordered w.r.t. these target states. We're not going to time-travel again,
                // so just enter the paths into insertPaths.
                for (auto const& [toState, withConstant] : listOfConstants) {
                    insertTransitions.emplace_back(toState, parametricPart * withConstant);
                }
                continue;
            }
            alreadyTimeTravelledToThis[parameter].insert(targetStates);

            doneTimeTravelling = true;

            RationalFunction constantPart = utility::zero<RationalFunction>();
            for (auto const& [state, transition] : listOfConstants) {
                constantPart += transition;
            }
            RationalFunction sum = parametricPart * constantPart;

            STORM_LOG_INFO("Time travellable transitions with " << sum << std::endl);

            doneTimeTravelling = true;

            // Create the new state that our parametric transitions will start in
            uint64_t newRow = flexibleMatrix.insertNewRowsAtEnd(1);
            uint64_t newRowBackwards = backwardsFlexibleMatrix.insertNewRowsAtEnd(1);
            STORM_LOG_ASSERT(newRow == newRowBackwards, "Internal error: Drifting matrix and backwardsTransitions.");

            // Sum of parametric transitions goes to new row
            insertTransitions.emplace_back(newRow, sum);

            // Write outgoing transitions from new row directly into the flexible matrix
            for (auto const& [state, thisProb] : listOfConstants) {
                const RationalFunction probAsFunction = RationalFunction(thisProb) / constantPart;
                // Forward
                flexibleMatrix.getRow(newRow).push_back(storage::MatrixEntry<uint_fast64_t, RationalFunction>(state, probAsFunction));
                // Backward
                backwardsFlexibleMatrix.getRow(state).push_back(storage::MatrixEntry<uint_fast64_t, RationalFunction>(newRow, probAsFunction));
                // Update tree-states here
                for (auto& entry : treeStatesNeedUpdate) {
                    entry.second.emplace(state);
                }
                STORM_LOG_INFO("With: " << probAsFunction << " to " << state);
                // Join duplicate transitions backwards (need to do this for all rows we come from)
                backwardsFlexibleMatrix.getRow(state) = joinDuplicateTransitions(backwardsFlexibleMatrix.getRow(state));
            }
            // Join duplicate transitions forwards (only need to do this for row we go to)
            flexibleMatrix.getRow(newRow) = joinDuplicateTransitions(flexibleMatrix.getRow(newRow));
        } else {
            auto const [state, probability] = *listOfConstants.begin();

            RationalFunction sum = parametricPart * probability;
            insertTransitions.emplace_back(state, sum);
        }
    }

    return insertTransitions;
}

void TimeTravelling::replaceWithNewTransitions(uint64_t state, const std::vector<std::pair<uint64_t, RationalFunction>> transitions,
                                                          storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix,
                                                          storage::FlexibleSparseMatrix<RationalFunction>& backwardsFlexibleMatrix,
                                                          storage::BitVector& reachableStates,
                                                          std::map<RationalFunctionVariable, std::set<uint64_t>>& treeStatesNeedUpdate) {
    // STORM_LOG_ASSERT(flexibleMatrix.createSparseMatrix().transpose() == backwardsFlexibleMatrix.createSparseMatrix(), "");
    // Delete old transitions - backwards
    for (auto const& deletingTransition : flexibleMatrix.getRow(state)) {
        auto& row = backwardsFlexibleMatrix.getRow(deletingTransition.getColumn());
        auto it = row.begin();
        while (it != row.end()) {
            if (it->getColumn() == state) {
                it = row.erase(it);
            } else {
                it++;
            }
        }
    }
    // Delete old transitions - forwards
    flexibleMatrix.getRow(state) = std::vector<storage::MatrixEntry<uint_fast64_t, RationalFunction>>();
    // STORM_LOG_ASSERT(flexibleMatrix.createSparseMatrix().transpose() == backwardsFlexibleMatrix.createSparseMatrix().transpose().transpose(), "");

    // Insert new transitions
    std::map<uint64_t, RationalFunction> insertThese;
    for (auto const& [target, probability] : transitions) {
        for (auto& entry : treeStatesNeedUpdate) {
            entry.second.emplace(target);
        }
        if (insertThese.count(target)) {
            insertThese[target] += probability;
        } else {
            insertThese[target] = probability;
        }
    }
    for (auto const& entry : insertThese) {
        // We know that neither no transition state <-> entry.first exist because we've erased them
        flexibleMatrix.getRow(state).push_back(storm::storage::MatrixEntry(entry.first, entry.second));
        backwardsFlexibleMatrix.getRow(entry.first).push_back(storm::storage::MatrixEntry(state, entry.second));
    }
    // STORM_LOG_ASSERT(flexibleMatrix.createSparseMatrix().transpose() == backwardsFlexibleMatrix.createSparseMatrix(), "");
}

void TimeTravelling::updateUnreachableStates(storage::BitVector& reachableStates, std::vector<uint64_t> const& statesMaybeUnreachable,
                                             storage::FlexibleSparseMatrix<RationalFunction> const& backwardsFlexibleMatrix,
                                             uint64_t initialState) {
    // Look if one of our visitedStates has become unreachable
    // i.e. all of its predecessors are unreachable
    for (auto const& visitedState : statesMaybeUnreachable) {
        if (visitedState == initialState) {
            continue;
        }
        bool isUnreachable = true;
        for (auto const& entry : backwardsFlexibleMatrix.getRow(visitedState)) {
            if (reachableStates.get(entry.getColumn())) {
                isUnreachable = false;
                break;
            }
        }
        if (isUnreachable) {
            reachableStates.set(visitedState, false);
        }
    }
}

std::vector<storm::storage::MatrixEntry<uint64_t, RationalFunction>> TimeTravelling::joinDuplicateTransitions(
    std::vector<storm::storage::MatrixEntry<uint64_t, RationalFunction>> const& entries) {
    std::vector<uint64_t> keyOrder;
    std::map<uint64_t, storm::storage::MatrixEntry<uint64_t, RationalFunction>> existingEntries;
    for (auto const& entry : entries) {
        if (existingEntries.count(entry.getColumn())) {
            existingEntries.at(entry.getColumn()).setValue(existingEntries.at(entry.getColumn()).getValue() + entry.getValue());
        } else {
            existingEntries[entry.getColumn()] = entry;
            keyOrder.push_back(entry.getColumn());
        }
    }
    std::vector<storm::storage::MatrixEntry<uint64_t, RationalFunction>> newEntries;
    for (uint64_t key : keyOrder) {
        newEntries.push_back(existingEntries.at(key));
    }
    return newEntries;
}

models::sparse::StateLabeling TimeTravelling::extendStateLabeling(models::sparse::StateLabeling const& oldLabeling, uint64_t oldSize, uint64_t newSize,
                                                                  uint64_t stateWithLabels, const std::set<std::string>& labelsInFormula) {
    models::sparse::StateLabeling newLabels(newSize);
    for (auto const& label : oldLabeling.getLabels()) {
        newLabels.addLabel(label);
    }
    for (uint64_t state = 0; state < oldSize; state++) {
        for (auto const& label : oldLabeling.getLabelsOfState(state)) {
            newLabels.addLabelToState(label, state);
        }
    }
    for (uint64_t i = oldSize; i < newSize; i++) {
        // We assume that everything that we time-travel has the same labels for now.
        for (auto const& label : oldLabeling.getLabelsOfState(stateWithLabels)) {
            if (labelsInFormula.count(label)) {
                newLabels.addLabelToState(label, i);
            }
        }
    }
    return newLabels;
}

void TimeTravelling::updateTreeStates(std::map<RationalFunctionVariable, std::map<uint64_t, std::set<uint64_t>>>& treeStates,
                                      std::map<RationalFunctionVariable, std::set<uint64_t>>& workingSets,
                                      const storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix,
                                      const storage::FlexibleSparseMatrix<RationalFunction>& backwardsTransitions,
                                      const std::set<RationalFunctionVariable>& allParameters,
                                      const boost::optional<std::vector<RationalFunction>>& stateRewardVector,
                                      const models::sparse::StateLabeling stateLabeling) {
    for (auto const& parameter : allParameters) {
        std::set<uint64_t> workingSet = workingSets[parameter];
        while (!workingSet.empty()) {
            std::set<uint64_t> newWorkingSet;
            for (uint64_t row : workingSet) {
                if (stateRewardVector && !stateRewardVector->at(row).isZero()) {
                    continue;
                }
                for (auto const& entry : backwardsTransitions.getRow(row)) {
                    if (entry.getValue().isConstant() && stateLabeling.getLabelsOfState(entry.getColumn()) == stateLabeling.getLabelsOfState(row)) {
                        // If the set of tree states at the current position is a subset of the set of
                        // tree states of the parent state, we've reached some loop. Then we can stop.
                        bool isSubset = true;
                        for (auto const& state : treeStates.at(parameter)[row]) {
                            if (!treeStates.at(parameter)[entry.getColumn()].count(state)) {
                                isSubset = false;
                                break;
                            }
                        }
                        if (isSubset) {
                            continue;
                        }
                        for (auto const& state : treeStates.at(parameter).at(row)) {
                            treeStates.at(parameter).at(entry.getColumn()).emplace(state);
                        }
                        newWorkingSet.emplace(entry.getColumn());
                    }
                }
            }
            workingSet = newWorkingSet;
        }
    }
}

class TimeTravelling;
}  // namespace transformer
}  // namespace storm
