#include "TimeTravelling.h"
#include <_types/_uint64_t.h>
#include <carl/core/Variable.h>
#include <carl/core/VariablePool.h>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>
#include "adapters/RationalFunctionAdapter.h"
#include "adapters/RationalFunctionForward.h"
#include "adapters/RationalNumberAdapter.h"
#include "logic/UntilFormula.h"
#include "modelchecker/CheckTask.h"
#include "models/sparse/Dtmc.h"
#include "models/sparse/StandardRewardModel.h"
#include "models/sparse/StateLabeling.h"
#include "solver/stateelimination/StateEliminator.h"
#include "storage/BitVector.h"
#include "storage/FlexibleSparseMatrix.h"
#include "storage/SparseMatrix.h"
#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"
#include "utility/constants.h"
#include "utility/graph.h"
#include "utility/logging.h"
#include "utility/macros.h"

#define WRITE_DTMCS 0

namespace storm {
namespace transformer {

models::sparse::Dtmc<RationalFunction> TimeTravelling::bigStep(models::sparse::Dtmc<RationalFunction> const& model,
                                                               modelchecker::CheckTask<logic::Formula, RationalFunction> const& checkTask, uint64_t horizon,
                                                               bool timeTravellingEnabled) {
    models::sparse::Dtmc<RationalFunction> dtmc(model);
    storage::SparseMatrix<RationalFunction> transitionMatrix = dtmc.getTransitionMatrix();
    uint64_t initialState = dtmc.getInitialStates().getNextSetIndex(0);

    auto allParameters = storm::models::sparse::getAllParameters(dtmc);

    std::set<std::string> labelsInFormula;
    for (auto const& atomicLabelFormula : checkTask.getFormula().getAtomicLabelFormulas()) {
        labelsInFormula.emplace(atomicLabelFormula->getLabel());
    }

    models::sparse::StateLabeling runningLabeling(dtmc.getStateLabeling());

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
    std::map<RationalFunctionVariable, std::map<uint64_t, std::set<uint64_t>>> treeStates;
    std::map<RationalFunctionVariable, std::set<uint64_t>> workingSets;

    // Count number of parameter occurences per state
    for (uint64_t row = 0; row < flexibleMatrix.getRowCount(); row++) {
        for (auto const& entry : flexibleMatrix.getRow(row)) {
            if (entry.getValue().isConstant()) {
                continue;
            }
            STORM_LOG_ERROR_COND(entry.getValue().gatherVariables().size() == 1, "Flip minimization only supports transitions with a single parameter.");
            auto parameter = *entry.getValue().gatherVariables().begin();
            auto cache = entry.getValue().nominatorAsPolynomial().pCache();
            workingSets[parameter].emplace(row);
            treeStates[parameter][row].emplace(row);
        }
    }

    // To prevent infinite unrolling of parametric loops:
    // We have already reordered with these as leaves, don't reorder with these as leaves again
    std::map<RationalFunctionVariable, std::set<std::set<uint64_t>>> alreadyReorderedWrt;

    std::stack<uint_fast64_t> topologicalOrderingStack;
    topologicalOrdering = utility::graph::getTopologicalSort<RationalFunction>(transitionMatrix, {initialState});
    for (auto rit = topologicalOrdering.begin(); rit != topologicalOrdering.end(); ++rit) {
        topologicalOrderingStack.push(*rit);
    }

    updateTreeStates(treeStates, workingSets, flexibleMatrix, backwardsTransitions, allParameters, stateRewardVector, runningLabeling, labelsInFormula);

    // Identify reachable states - not reachable states do not have do be big-stepped
    storage::BitVector trueVector(transitionMatrix.getRowCount(), true);
    storage::BitVector falseVector(transitionMatrix.getRowCount(), false);
    storage::BitVector initialStates(transitionMatrix.getRowCount(), false);
    initialStates.set(initialState, true);
    storage::BitVector reachableStates = storm::utility::graph::getReachableStates(transitionMatrix, initialStates, trueVector, falseVector);

    while (!topologicalOrderingStack.empty()) {
        auto state = topologicalOrderingStack.top();
        topologicalOrderingStack.pop();

        if (!reachableStates.get(state)) {
            continue;
        }

        std::set<carl::Variable> bigStepParameters;
        for (auto const& oneStep : flexibleMatrix.getRow(state)) {
            for (auto const& parameter : allParameters) {
                if (treeStates[parameter].count(oneStep.getColumn()) && treeStates.at(parameter).at(oneStep.getColumn()).size() >= 1) {
                    bigStepParameters.emplace(parameter);
                }
            }
        }

        // Do big-step lifting from here
        // Just follow the treeStates and eliminate transitions
        for (auto const& parameter : bigStepParameters) {
            auto parameterMap = treeStates.at(parameter);

            // std::cout << "BigStep " << state << std::endl;

            struct searchingPath {
                std::vector<uint_fast64_t> path;
                RationalFunction probability;
            };

            // We enumerate all paths starting in `state` and eventually reaching our goals
            std::vector<searchingPath> searchingPaths;
            std::vector<searchingPath> donePaths;
            std::vector<uint64_t> visitedStates;

            searchingPaths.push_back(searchingPath{{state}, utility::one<RationalFunction>()});

            while (!searchingPaths.empty()) {
                std::vector<searchingPath> newPaths;
                for (auto const& path : searchingPaths) {
                    auto const bigStepState = path.path.back();
                    auto const row = flexibleMatrix.getRow(bigStepState);

                    bool bigStepThisRow = true;
                    // Stop here if degree gets too large
                    for (auto const& entry : row) {
                        // Stop if degree gets too large
                        auto const newProbability = path.probability * entry.getValue();
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
                        bool continueSearching = !parameterMap[entry.getColumn()].empty();

                        // Also continue searching if there is only a transition with a one coming up, we can skip that
                        // This is nice because we can possibly combine more transitions later
                        bool onlyHasOne = flexibleMatrix.getRow(entry.getColumn()).size() == 1 &&
                                          flexibleMatrix.getRow(entry.getColumn()).begin()->getValue() == utility::one<RationalFunction>();
                        continueSearching |= onlyHasOne;

                        // Stop if we have encountered a loop
                        continueSearching &= std::find(path.path.begin(), path.path.end(), entry.getColumn()) == path.path.end();
                        // Don't mess with rewards for now (TODO for later)
                        continueSearching &= !(stateRewardVector && !stateRewardVector->at(entry.getColumn()).isZero());

                        auto const newProbability = path.probability * entry.getValue();
                        std::vector<uint_fast64_t> pathCopy = path.path;
                        pathCopy.push_back(entry.getColumn());
                        searchingPath newPath{pathCopy, newProbability};

                        if (continueSearching) {
                            newPaths.push_back(newPath);
                        } else {
                            donePaths.push_back(newPath);
                        }
                    }
                }
                searchingPaths = newPaths;
            }

            bool doneTimeTravelling = false;
            const uint64_t oldMatrixSize = flexibleMatrix.getRowCount();

            // Final map of paths, will already get filled up with all constant paths
            std::vector<searchingPath> insertPaths;
            if (!timeTravellingEnabled) {
                insertPaths = donePaths;
            } else {
                // Time Travelling: For transitions that have the same derivative w.r.t. their variable, join them into one transition leading into new state
                std::map<std::pair<RationalFunctionVariable, RationalFunction>, std::vector<searchingPath>> parametricTransitions;

                for (auto const& path : donePaths) {
                    auto const variables = path.probability.gatherVariables();
                    // TODO can only time-travel single-variate transitions
                    if (variables.size() == 1) {
                        auto const variable = *variables.begin();
                        auto pair = std::make_pair(variable, path.probability.derivative(variable));
                        if (parametricTransitions.count(pair)) {
                            parametricTransitions[pair].push_back(path);
                        } else {
                            parametricTransitions[pair] = {path};
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
                            targetStates.emplace(path.path.back());
                        }
                        if (alreadyReorderedWrt[parameter].count(targetStates) || targetStates.size() == 1) {
                            // We already reordered w.r.t. these target states. We're not going to time-travel again,
                            // so just enter the paths into insertPaths.
                            for (auto const& path : entry.second) {
                                insertPaths.push_back(path);
                            }
                            continue;
                        }
                        alreadyReorderedWrt[parameter].insert(targetStates);

                        doneTimeTravelling = true;

                        // std::cout << "Time travellable transitions with " << entry.first.first << " and " << entry.first.second << std::endl;
                        RationalFunction sum = utility::zero<RationalFunction>();
                        for (auto const& path : entry.second) {
                            sum += path.probability;
                        }

                        // std::cout << "Sum: " << sum << std::endl;

                        uint64_t newRow = flexibleMatrix.insertNewRowsAtEnd(1);
                        STORM_LOG_ASSERT(newRow == backwardsTransitions.insertNewRowsAtEnd(1), "Internal error: Drifting matrix and backwardsTransitions.");

                        insertPaths.push_back(searchingPath{{newRow}, sum});

                        for (auto const& path : entry.second) {
                            flexibleMatrix.getRow(newRow).push_back(
                                storage::MatrixEntry<uint_fast64_t, RationalFunction>(path.path.back(), path.probability / sum));
                            backwardsTransitions.getRow(path.path.back())
                                .push_back(storage::MatrixEntry<uint_fast64_t, RationalFunction>(newRow, path.probability / sum));
                            for (auto const& p : allParameters) {
                                workingSets[p].emplace(path.path.back());
                            }
                            // std::cout << "With: " << path.probability / sum << " to " << path.path.back() << std::endl;
                            backwardsTransitions.getRow(path.path.back()) = joinDuplicateTransitions(backwardsTransitions.getRow(path.path.back()));
                        }
                        flexibleMatrix.getRow(newRow) = joinDuplicateTransitions(flexibleMatrix.getRow(newRow));
                    } else {
                        for (auto const& path : entry.second) {
                            insertPaths.push_back(path);
                        }
                    }
                }
            }

            // Now make modifications concerning the root state of the big-step

            // Delete old transitions - backwards
            for (auto const& deletingTransition : flexibleMatrix.getRow(state)) {
                auto row = backwardsTransitions.getRow(deletingTransition.getColumn());
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
            for (auto const& path : insertPaths) {
                uint64_t target = path.path.back();
                for (auto const& p : allParameters) {
                    workingSets[p].emplace(target);
                }
                if (insertThese.count(target)) {
                    insertThese[target] += path.probability;
                } else {
                    insertThese[target] = path.probability;
                }
            }
            for (auto const& entry : insertThese) {
                // We know that neither no transition state <-> entry.first exist because we've erased them
                flexibleMatrix.getRow(state).push_back(storm::storage::MatrixEntry(entry.first, entry.second));
                backwardsTransitions.getRow(entry.first).push_back(storm::storage::MatrixEntry(state, entry.second));
            }

            // Look if one of our visitedStates has become unreachable
            // i.e. all of its predecessors are unreachable
            for (auto const& visitedState : visitedStates) {
                bool isUnreachable = true;
                for (auto const& entry : backwardsTransitions.getRow(visitedState)) {
                    if (reachableStates.get(entry.getColumn())) {
                        isUnreachable = false;
                        break;
                    }
                }
                if (isUnreachable) {
                    // std::cout << visitedState << " has become unreachable" << std::endl;
                    reachableStates.set(visitedState, false);
                }
            }

            // std::sort(flexibleMatrix.getRow(state).begin(), flexibleMatrix.getRow(state).end(),
            //           [](const storage::MatrixEntry<uint64_t, RationalFunction>& a, const storage::MatrixEntry<uint64_t, RationalFunction>& b) -> bool {
            //               return a.getColumn() > b.getColumn();
            //           });
            if (doneTimeTravelling) {
                uint64_t newMatrixSize = flexibleMatrix.getRowCount();
                if (newMatrixSize > oldMatrixSize) {
                    // Extend labeling to more states
                    models::sparse::StateLabeling nextNewLabels = extendStateLabeling(runningLabeling, oldMatrixSize, newMatrixSize, state, labelsInFormula);
                    runningLabeling = nextNewLabels;

                    // Extend reachableStates
                    reachableStates.resize(newMatrixSize, true);

                    for (uint64_t i = oldMatrixSize; i < newMatrixSize; i++) {
                        // Next consider the new states
                        topologicalOrderingStack.push(i);
                        // New states have zero reward
                        if (stateRewardVector) {
                            stateRewardVector->push_back(storm::utility::zero<RationalFunction>());
                        }
                    }
                }
                updateTreeStates(treeStates, workingSets, flexibleMatrix, backwardsTransitions, allParameters, stateRewardVector, runningLabeling,
                                 labelsInFormula);
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
                                                                  uint64_t stateWithLabels, const std::set<std::string> labelsInFormula) {
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

bool labelsIntersectedEqual(const std::set<std::string>& labels1, const std::set<std::string>& labels2, const std::set<std::string>& intersection) {
    for (auto const& label : intersection) {
        bool set1ContainsLabel = labels1.count(label) > 0;
        bool set2ContainsLabel = labels2.count(label) > 0;
        if (set1ContainsLabel != set2ContainsLabel) {
            return false;
        }
    }
    return true;
}

void TimeTravelling::updateTreeStates(std::map<RationalFunctionVariable, std::map<uint64_t, std::set<uint64_t>>>& treeStates,
                                      std::map<RationalFunctionVariable, std::set<uint64_t>>& workingSets,
                                      const storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix,
                                      const storage::FlexibleSparseMatrix<RationalFunction>& backwardsTransitions,
                                      const std::set<carl::Variable>& allParameters, const boost::optional<std::vector<RationalFunction>>& stateRewardVector,
                                      const models::sparse::StateLabeling stateLabeling, const std::set<std::string> labelsInFormula) {
    for (auto const& parameter : allParameters) {
        std::set<uint64_t> workingSet = workingSets[parameter];
        while (!workingSet.empty()) {
            std::set<uint64_t> newWorkingSet;
            for (uint64_t row : workingSet) {
                if (stateRewardVector && !stateRewardVector->at(row).isZero()) {
                    continue;
                }
                for (auto const& entry : backwardsTransitions.getRow(row)) {
                    if (entry.getValue().isConstant() &&
                        labelsIntersectedEqual(stateLabeling.getLabelsOfState(entry.getColumn()), stateLabeling.getLabelsOfState(row), labelsInFormula)) {
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
