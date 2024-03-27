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
#include "utility/constants.h"
#include "utility/graph.h"
#include "utility/logging.h"
#include "utility/macros.h"

#define WRITE_DTMCS 0

namespace storm {
namespace transformer {

bool areTimeTravellable(RationalFunction transition1, RationalFunction transition2) {
    if (transition1.isConstant() || transition2.isConstant()) {
        return false;
    }
    // TODO "constant parts" are not filtered out here, because we don't know how to handle them.
    // so if a transition is 0.5(1-p) + c we have lost.
    auto const dividing = transition1 / transition2;
    return dividing.isConstant() && dividing.constantPart() > utility::zero<RationalFunctionCoefficient>();
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

    while (!topologicalOrderingStack.empty()) {
        auto state = topologicalOrderingStack.top();
        topologicalOrderingStack.pop();

        if (!reachableStates.get(state)) {
            continue;
        }

        // The parameters we can do big-step w.r.t. (if horizon > 1)
        std::set<RationalFunctionVariable> bigStepParameters;
        if (horizon > 1) {
            for (auto const& oneStep : flexibleMatrix.getRow(state)) {
                for (auto const& parameter : allParameters) {
                    if (treeStates[parameter].count(oneStep.getColumn()) && treeStates.at(parameter).at(oneStep.getColumn()).size() >= 1) {
                        bigStepParameters.emplace(parameter);
                    }
                }
            }
        }

        // Do big-step lifting from here
        // Follow the treeStates and eliminate transitions
        for (auto const& parameter : bigStepParameters) {
            // Find the paths along which we eliminate the transitions into one transition along with their probabilities.
            auto const [paths, visitedStates] = findBigStepPaths(state, parameter, horizon, flexibleMatrix, treeStates, stateRewardVector);

            bool doneTimeTravelling = false;
            uint64_t oldMatrixSize = flexibleMatrix.getRowCount();

            std::vector<std::pair<uint64_t, RationalFunction>> transitions;
            if (timeTravellingEnabled) {
                auto const timeTravelledTransitions = findTimeTravelling(paths, parameter, flexibleMatrix, backwardsTransitions, alreadyTimeTravelledToThis,
                                                                   treeStatesNeedUpdate, originalNumStates);
                if (timeTravelledTransitions) {
                    transitions = *timeTravelledTransitions;
                    doneTimeTravelling = true;
                }
            } else {
                // TODO just converting the format idk how nice this is
                for (auto const& path : paths) {
                    transitions.emplace_back(path->state, path->probability);
                }
            }

            // Put paths into matrix
            replaceWithNewTransitions(state, transitions, flexibleMatrix, backwardsTransitions, reachableStates, treeStatesNeedUpdate);

            // Dynamically update unreachable states
            updateUnreachableStates(reachableStates, visitedStates, backwardsTransitions);

            if (doneTimeTravelling) {
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
                }
                updateTreeStates(treeStates, treeStatesNeedUpdate, flexibleMatrix, backwardsTransitions, allParameters, stateRewardVector,
                                 runningLabelingTreeStates);
            }
        }

#if WRITE_DTMCS
        models::sparse::Dtmc<RationalFunction> newnewDTMC(flexibleMatrix.createSparseMatrix(), runningLabeling);
        if (stateRewardVector) {
            models::sparse::StandardRewardModel<RationalFunction> newRewardModel(*stateRewardVector);
            newnewDTMC.addRewardModel(*stateRewardName, newRewardModel);
        }
        std::ofstream file;
        file.open("dots/bigstep_" + std::to_string(flexibleMatrix.getRowCount()) + ".dot");
        newnewDTMC.writeDotToStream(file);
        file.close();
        newnewDTMC.getTransitionMatrix().isProbabilistic();
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

std::pair<std::vector<std::shared_ptr<TimeTravelling::searchingPath>>, std::vector<uint64_t>> TimeTravelling::findBigStepPaths(
    uint64_t start, const RationalFunctionVariable& parameter, uint64_t horizon, const storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix,
    const std::map<RationalFunctionVariable, std::map<uint64_t, std::set<uint64_t>>>& treeStates,
    const boost::optional<std::vector<RationalFunction>>& stateRewardVector) {
    // We enumerate all paths starting in `state` and eventually reaching our goals
    std::map<uint64_t, std::shared_ptr<TimeTravelling::searchingPath>> allPaths;
    std::map<uint64_t, std::shared_ptr<TimeTravelling::searchingPath>> searchingPaths;
    std::vector<uint64_t> visitedStates;

    // TODO put into method signature
    bool timeTravellingEnabled = true;

    auto firstPath= std::make_shared<TimeTravelling::searchingPath>(start);
    firstPath->probability = utility::one<RationalFunction>();
    if (timeTravellingEnabled) {
        firstPath->timeTravellingHints = ProbCounter<Counter<RationalFunction>>();
        (*firstPath->timeTravellingHints)[Counter<RationalFunction>()] = utility::one<RationalNumber>();
    }
    searchingPaths[start] = firstPath;
    allPaths[start] = firstPath;

    while (!searchingPaths.empty()) {
        std::map<uint64_t, std::shared_ptr<TimeTravelling::searchingPath>> newSearchingPaths;
        for (auto const& [state, path] : searchingPaths) {
            STORM_LOG_ASSERT(state == path->state, "Path placed incorrectly in newPaths map.");

            auto const row = flexibleMatrix.getRow(path->state);

            bool bigStepThisRow = true;

            for (auto const& entry : row) {
                // Stop if degree gets too large
                auto const newProbability = path->probability * entry.getValue();
                auto const derivative = newProbability.nominator().derivative(parameter);

                // TODO Only do this in degree-only big-step
                if (newProbability.nominator().totalDegree() > horizon || !(derivative.isConstant() || derivative.isUnivariate()) ||
                    (!derivative.isConstant() && derivative.getSingleVariable() != parameter)) {
                    // We cannot do this big-step at all
                    bigStepThisRow = false;
                    searchingPaths.erase(path->state);
                    break;
                }
            }

            if (!bigStepThisRow) {
                continue;
            }

            visitedStates.push_back(path->state);

            for (auto const& entry : row) {
                std::shared_ptr<searchingPath> newPath;
                if (allPaths.count(entry.getColumn())) {
                    // A path already goes to this state
                    newPath = allPaths.at(entry.getColumn());
                    STORM_LOG_ASSERT(newPath->state == entry.getColumn(), "Path emplaced incorrectly into map");
                } else {
                    // We make a new path
                    newPath = std::make_shared<TimeTravelling::searchingPath>(entry.getColumn());
                    allPaths[entry.getColumn()] = newPath;
                }
                newSearchingPaths[entry.getColumn()] = newPath;

                newPath->probability += path->probability * entry.getValue();
                newPath->prefixes.emplace_back(entry.getValue(), path);
                if (timeTravellingEnabled) {
                    for (auto const& hint : *path->timeTravellingHints) {
                        auto newPair(hint.first);
                        auto count = hint.second;
                        if (entry.getValue().isConstant()) {
                            count += entry.getValue().constantPart();
                        } else {
                            newPair[entry.getValue()] += 1;
                        }
                        (*newPath->timeTravellingHints)[newPair] += count;
                    }
                }
            }
        }

        for (auto const& [state, path] : newSearchingPaths) {
            STORM_LOG_ASSERT(state == path->state, "State incorrectly inserted into newSearchingPaths");
            // Continue searching if we can still reach a parameter
            bool continueSearching = treeStates.at(parameter).count(state) && !treeStates.at(parameter).at(state).empty();

            // Also continue searching if there is only a transition with a one coming up, we can skip that
            // This is nice because we can possibly combine more transitions later
            bool onlyHasOne = flexibleMatrix.getRow(state).size() == 1 &&
                                flexibleMatrix.getRow(state).begin()->getValue() == utility::one<RationalFunction>();
            continueSearching |= onlyHasOne;

            // Stop if we have encountered a loop
            continueSearching &= !path->stateInPath(state);
            // Don't mess with rewards for now (TODO for later)
            continueSearching &= !(stateRewardVector && !stateRewardVector->at(state).isZero());

            if (!continueSearching) {
                newSearchingPaths.erase(state);
            }
        }
        searchingPaths = newSearchingPaths;
    }
    std::vector<std::shared_ptr<searchingPath>> finalPaths;
    for (auto const& [_, path] : allPaths) {
        finalPaths.push_back(path);
    }
    return std::make_pair(finalPaths, visitedStates);
}

std::optional<std::vector<std::pair<uint64_t, RationalFunction>>> TimeTravelling::findTimeTravelling(
    const std::vector<std::shared_ptr<TimeTravelling::searchingPath>> bigStepPaths, const RationalFunctionVariable& parameter,
    storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix, storage::FlexibleSparseMatrix<RationalFunction>& backwardsFlexibleMatrix,
    std::map<RationalFunctionVariable, std::set<std::set<uint64_t>>>& alreadyTimeTravelledToThis,
    std::map<RationalFunctionVariable, std::set<uint64_t>>& treeStatesNeedUpdate, uint64_t originalNumStates) {
    bool doneTimeTravelling = false;

    // Time Travelling: For transitions that divide into constants, join them into one transition leading into new state
    std::map<Counter<RationalFunction>, std::map<uint64_t, RationalFunction>> parametricTransitions;

    for (auto const& path : bigStepPaths) {
        STORM_LOG_ASSERT(path->timeTravellingHints, "Time-travelling enabled, so time-travelling hints must have been generated.");
        for (auto const& [paramCounter, probability] : *path->timeTravellingHints) {
            parametricTransitions[paramCounter][path->state] = RationalFunction(probability);
        }
    }

    // Problem: If we do time-travelling to target states {s_1, ..., s_n}, then we need to also consider the other probabilities
    // going to {s_1, ..., s_n} that are not time-travelled, as they change
    // State that are entirely unaffacted by time travelling, i.e., they don't appear in any time-travelling,
    // we want to leave in peace and not re-calculate all of the transitions.

    std::unordered_set<uint64_t> affectedStates;

    for (auto const& [counter, constantTransitions] : parametricTransitions) {
        for (auto const& [_, state] : constantTransitions) {
            if (constantTransitions.size() > 1) {
                affectedStates.emplace(state);
            }
        }
    }

    // These are the transitions that we are actually going to insert (that the function will return).
    std::vector<std::pair<uint64_t, RationalFunction>> insertTransitions;

    // Unaffected states have unchanged probabilities
    for (auto const& path : bigStepPaths) {
        if (!affectedStates.count(path->state)) {
            insertTransitions.emplace_back(path->state, path->probability);
        }
    }

    for (auto const& [counter, constantTransitions] : parametricTransitions) {
        if (constantTransitions.size() > 1) {
            // The set of target states of the paths that we maybe want to time-travel
            std::set<uint64_t> targetStates;
            for (auto const& [_, state] : counter) {
                if (state < originalNumStates) {
                    targetStates.emplace(state);
                }
            }
            if (alreadyTimeTravelledToThis[parameter].count(targetStates) || targetStates.size() == 1) {
                // We already reordered w.r.t. these target states. We're not going to time-travel again,
                // so just enter the paths into insertPaths.
                for (auto const& transition : constantTransitions) {
                    insertTransitions.push_back(transition);
                }
                continue;
            }
            alreadyTimeTravelledToThis[parameter].insert(targetStates);

            doneTimeTravelling = true;

            RationalFunction parametricPart = utility::one<RationalFunction>();
            for (auto const& [p, exp] : counter) {
                for (uint64_t i = 0; i < exp; i++) {
                    parametricPart *= p;
                }
            }
            RationalFunction constantPart = utility::zero<RationalFunction>();
            for (auto const& [state, transition] : constantTransitions) {
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
            for (auto const& [state, thisProb] : constantTransitions) {
                STORM_LOG_ASSERT(thisProb.isConstant(), "Non-constant probability >:(");

                // Forward
                flexibleMatrix.getRow(newRow).push_back(storage::MatrixEntry<uint_fast64_t, RationalFunction>(state, thisProb));
                // Backward
                backwardsFlexibleMatrix.getRow(state).push_back(storage::MatrixEntry<uint_fast64_t, RationalFunction>(newRow, thisProb));
                // Update tree-states here
                for (auto& entry : treeStatesNeedUpdate) {
                    entry.second.emplace(state);
                }
                STORM_LOG_INFO("With: " << thisProb << " to " << state);
                // Join duplicate transitions backwards (need to do this for all rows we come from)
                backwardsFlexibleMatrix.getRow(state) = joinDuplicateTransitions(backwardsFlexibleMatrix.getRow(state));
            }
            // Join duplicate transitions forwards (only need to do this for row we go to)
            flexibleMatrix.getRow(newRow) = joinDuplicateTransitions(flexibleMatrix.getRow(newRow));
        } else {
            auto const [state, probability] = *constantTransitions.begin();

            if (affectedStates.count(state)) {
                RationalFunction parametricPart = utility::one<RationalFunction>();
                for (auto const& [p, exp] : counter) {
                    for (uint64_t i = 0; i < exp; i++) {
                        parametricPart *= p;
                    }
                }
                RationalFunction constantPart = utility::zero<RationalFunction>();
                for (auto const& [state, transition] : constantTransitions) {
                    constantPart += transition;
                }
                RationalFunction sum = parametricPart * constantPart;
                insertTransitions.push_back(std::make_pair(state, sum));
            }
        }
    }

    if (doneTimeTravelling) {
        return insertTransitions;
    }
    return std::nullopt;
}

void TimeTravelling::replaceWithNewTransitions(uint64_t state, const std::vector<std::pair<uint64_t, RationalFunction>> transitions,
                                                          storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix,
                                                          storage::FlexibleSparseMatrix<RationalFunction>& backwardsFlexibleMatrix,
                                                          storage::BitVector& reachableStates,
                                                          std::map<RationalFunctionVariable, std::set<uint64_t>>& treeStatesNeedUpdate) {
    // Delete old transitions - backwards
    for (auto const& deletingTransition : flexibleMatrix.getRow(state)) {
        auto row = backwardsFlexibleMatrix.getRow(deletingTransition.getColumn());
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
}

void TimeTravelling::updateUnreachableStates(storage::BitVector& reachableStates, std::vector<uint64_t> const& statesMaybeUnreachable,
                                             storage::FlexibleSparseMatrix<RationalFunction> const& backwardsFlexibleMatrix) {
    // Look if one of our visitedStates has become unreachable
    // i.e. all of its predecessors are unreachable
    for (auto const& visitedState : statesMaybeUnreachable) {
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
