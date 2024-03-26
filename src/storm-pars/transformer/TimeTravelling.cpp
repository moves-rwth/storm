#include "TimeTravelling.h"
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
#include <utility>
#include <vector>
#include "adapters/RationalFunctionAdapter.h"
#include "adapters/RationalFunctionForward.h"

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

        std::set<RationalFunctionVariable> timeTravelParameters;
        // TODO really slow!!! do we really want to do this??

        // // The parameters we can do time-travelling w.r.t. (perhaps without big-stepping. time-travelling is always done after big-stepping as well)
        // if (timeTravellingEnabled) {
        //     std::set<RationalFunction> occuringBranches;
        //     for (auto const& oneStep : flexibleMatrix.getRow(state)) {
        //         auto const variables = oneStep.getValue().gatherVariables();
        //         // TODO can only time-travel single-variate transitions
        //         if (variables.size() == 1) {
        //             auto const variable = *variables.begin();

        //             bool foundMatchingTransition = false;
        //             for (auto const& branch : occuringBranches) {
        //                 if (areTimeTravellable(oneStep.getValue(), branch)) {
        //                     timeTravelParameters.emplace(variable);
        //                     foundMatchingTransition = true;
        //                     break;
        //                 }
        //             }
        //             if (!foundMatchingTransition) {
        //                 occuringBranches.emplace(oneStep.getValue());
        //             }
        //         }
        //     }
        // }

        std::set<RationalFunctionVariable> doSomethingParameters;
        doSomethingParameters.insert(bigStepParameters.begin(), bigStepParameters.end());
        doSomethingParameters.insert(timeTravelParameters.begin(), timeTravelParameters.end());

        // Do big-step lifting from here
        // Follow the treeStates and eliminate transitions
        for (auto const& parameter : doSomethingParameters) {
            // Find the paths along which we eliminate the transitions into one transition along with their probabilities.
            auto const result = findBigStepPaths(state, parameter, horizon, flexibleMatrix, treeStates, stateRewardVector);

            auto paths = result.first;
            auto const visitedStates = result.second;

            bool doneTimeTravelling = false;
            uint64_t oldMatrixSize = flexibleMatrix.getRowCount();

            if (timeTravellingEnabled) {
                auto const timeTravelledPaths = findTimeTravelling(paths, parameter, flexibleMatrix, backwardsTransitions, alreadyTimeTravelledToThis,
                                                                   treeStatesNeedUpdate, originalNumStates);
                if (timeTravelledPaths) {
                    paths = *timeTravelledPaths;
                    doneTimeTravelling = true;
                }
            }

            // Put paths into matrix
            eliminateTransitionsAccordingToPaths(state, paths, flexibleMatrix, backwardsTransitions, reachableStates, treeStatesNeedUpdate);

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
    std::vector<std::shared_ptr<TimeTravelling::searchingPath>> searchingPaths;
    std::vector<std::shared_ptr<TimeTravelling::searchingPath>> donePaths;
    std::vector<uint64_t> visitedStates;

    searchingPaths.push_back(
        std::make_shared<TimeTravelling::searchingPath>(nullptr, start, utility::one<RationalFunction>(), utility::one<RationalFunction>()));

    while (!searchingPaths.empty()) {
        std::vector<std::shared_ptr<TimeTravelling::searchingPath>> newPaths;
        for (auto const& path : searchingPaths) {
            auto const bigStepState = path->state;
            auto const row = flexibleMatrix.getRow(bigStepState);

            bool bigStepThisRow = true;

            for (auto const& entry : row) {
                // Stop if degree gets too large
                auto const newProbability = path->probability * entry.getValue();
                auto const derivative = newProbability.nominator().derivative(parameter);

                if (newProbability.nominator().totalDegree() > horizon || !(derivative.isConstant() || derivative.isUnivariate()) ||
                    (!derivative.isConstant() && derivative.getSingleVariable() != parameter)) {
                    // We cannot do this big-step at all
                    donePaths.push_back(path);
                    bigStepThisRow = false;
                    break;
                }
            }

            if (!bigStepThisRow) {
                continue;
            }

            visitedStates.push_back(bigStepState);

            for (auto const& entry : row) {
                // Continue searching if we can still reach a parameter
                bool continueSearching = treeStates.at(parameter).count(entry.getColumn()) && !treeStates.at(parameter).at(entry.getColumn()).empty();

                // Also continue searching if there is only a transition with a one coming up, we can skip that
                // This is nice because we can possibly combine more transitions later
                bool onlyHasOne = flexibleMatrix.getRow(entry.getColumn()).size() == 1 &&
                                  flexibleMatrix.getRow(entry.getColumn()).begin()->getValue() == utility::one<RationalFunction>();
                continueSearching |= onlyHasOne;

                // Stop if we have encountered a loop
                continueSearching &= !path->stateInPath(entry.getColumn());
                // Don't mess with rewards for now (TODO for later)
                continueSearching &= !(stateRewardVector && !stateRewardVector->at(entry.getColumn()).isZero());

                auto const newPath =
                    std::make_shared<TimeTravelling::searchingPath>(path, entry.getColumn(), entry.getValue(), path->probability * entry.getValue());

                if (continueSearching) {
                    newPaths.push_back(newPath);
                } else {
                    donePaths.push_back(newPath);
                }
            }
        }
        searchingPaths = newPaths;
    }
    return std::make_pair(donePaths, visitedStates);
}

std::optional<std::vector<std::shared_ptr<TimeTravelling::searchingPath>>> TimeTravelling::findTimeTravelling(
    const std::vector<std::shared_ptr<TimeTravelling::searchingPath>> bigStepPaths, const RationalFunctionVariable& parameter,
    storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix, storage::FlexibleSparseMatrix<RationalFunction>& backwardsFlexibleMatrix,
    std::map<RationalFunctionVariable, std::set<std::set<uint64_t>>>& alreadyTimeTravelledToThis,
    std::map<RationalFunctionVariable, std::set<uint64_t>>& treeStatesNeedUpdate, uint64_t originalNumStates) {
    bool doneTimeTravelling = false;
    std::vector<std::shared_ptr<TimeTravelling::searchingPath>> insertPaths;

    // TODO there are 2 ways to find out time travelling
    // - we big-stepped and have the list of parameteric factors. these we just put in

    // Time Travelling: For transitions that divide into constants, join them into one transition leading into new state
    std::map<std::multiset<RationalFunction>, std::vector<std::shared_ptr<TimeTravelling::searchingPath>>> parametricTransitions;

    for (auto const& path : bigStepPaths) {
        auto const variables = path->probability.gatherVariables();
        // TODO can only time-travel single-variate transitions
        if (variables.size() == 1) {
            auto const variable = *variables.begin();

            bool foundMatchingTransition = false;

            // Compute parametric factors of this path
            std::multiset<RationalFunction> parametricFactors;
            auto currentPointer = path;
            while (currentPointer != nullptr) {
                if (!currentPointer->transition.isConstant()) {
                    // TODO normalize?
                    parametricFactors.emplace(currentPointer->transition);
                }
                currentPointer = currentPointer->prefix;
            }

            for (auto& branch : parametricTransitions) {
                if (parametricFactors == branch.first) {
                    branch.second.push_back(path);
                    foundMatchingTransition = true;
                    break;
                }
            }
            if (!foundMatchingTransition) {
                parametricTransitions[parametricFactors] = {path};
            }
        } else {
            insertPaths.push_back(path);
        }
    }

    for (auto const& entry : parametricTransitions) {
        if (entry.second.size() > 1) {
            // The set of target states of the paths that we maybe want to time-travel
            std::set<uint64_t> targetStates;
            for (auto const& path : entry.second) {
                if (path->state < originalNumStates) {
                    targetStates.emplace(path->state);
                }
            }
            if (alreadyTimeTravelledToThis[parameter].count(targetStates) || targetStates.size() == 1) {
                // We already reordered w.r.t. these target states. We're not going to time-travel again,
                // so just enter the paths into insertPaths.
                for (auto const& path : entry.second) {
                    insertPaths.push_back(path);
                }
                continue;
            }
            alreadyTimeTravelledToThis[parameter].insert(targetStates);

            doneTimeTravelling = true;

            STORM_LOG_INFO("Time travellable transitions with "
                           << std::reduce(entry.first.begin(), entry.first.end(), utility::one<RationalFunction>(), std::multiplies<RationalFunction>()));

            RationalFunction sum = utility::zero<RationalFunction>();
            for (auto const& path : entry.second) {
                sum += path->probability;
            }

            doneTimeTravelling = true;

            // Create the new state that our parametric transitions will start in
            uint64_t newRow = flexibleMatrix.insertNewRowsAtEnd(1);
            uint64_t newRowBackwards = backwardsFlexibleMatrix.insertNewRowsAtEnd(1);
            STORM_LOG_ASSERT(newRow == newRowBackwards, "Internal error: Drifting matrix and backwardsTransitions.");

            // Sum of parametric transitions goes to new row
            insertPaths.push_back(std::make_shared<TimeTravelling::searchingPath>(nullptr, newRow, sum, sum));

            // Write outgoing transitions from new row directly into the flexible matrix
            for (auto const& path : entry.second) {
                auto const thisProb = path->probability / sum;

                // TODO: Should probably be handled.
                STORM_LOG_ASSERT(thisProb.isConstant(), "Non-constant probability in sum.");

                // Forward
                flexibleMatrix.getRow(newRow).push_back(storage::MatrixEntry<uint_fast64_t, RationalFunction>(path->state, thisProb));
                // Backward
                backwardsFlexibleMatrix.getRow(path->state).push_back(storage::MatrixEntry<uint_fast64_t, RationalFunction>(newRow, thisProb));
                // Update tree-states here
                for (auto& entry : treeStatesNeedUpdate) {
                    entry.second.emplace(path->state);
                }
                STORM_LOG_INFO("With: " << path->probability / sum << " to " << path->state);
                // Join duplicate transitions backwards (need to do this for all rows we come from)
                backwardsFlexibleMatrix.getRow(path->state) = joinDuplicateTransitions(backwardsFlexibleMatrix.getRow(path->state));
            }
            // Join duplicate transitions forwards (only need to do this for row we go to)
            flexibleMatrix.getRow(newRow) = joinDuplicateTransitions(flexibleMatrix.getRow(newRow));
        } else {
            for (auto const& path : entry.second) {
                insertPaths.push_back(path);
            }
        }
    }

    if (doneTimeTravelling) {
        return insertPaths;
    }
    return std::nullopt;
}

void TimeTravelling::eliminateTransitionsAccordingToPaths(uint64_t state, const std::vector<std::shared_ptr<TimeTravelling::searchingPath>> paths,
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
    for (auto const& path : paths) {
        uint64_t target = path->state;
        for (auto& entry : treeStatesNeedUpdate) {
            entry.second.emplace(target);
        }
        if (insertThese.count(target)) {
            insertThese[target] += path->probability;
        } else {
            insertThese[target] = path->probability;
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
