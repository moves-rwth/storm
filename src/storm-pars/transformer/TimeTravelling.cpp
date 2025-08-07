#include "TimeTravelling.h"
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
#include <vector>
#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/logic/UntilFormula.h"
#include "storm/modelchecker/CheckTask.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/StateLabeling.h"
#include "storm/solver/stateelimination/StateEliminator.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/FlexibleSparseMatrix.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/utility/constants.h"
#include "storm/utility/graph.h"
#include "storm/utility/logging.h"
#include "storm/utility/macros.h"

#define WRITE_DTMCS 0

namespace storm {
namespace transformer {

models::sparse::Dtmc<RationalFunction> TimeTravelling::timeTravel(models::sparse::Dtmc<RationalFunction> const& model,
                                                                  modelchecker::CheckTask<logic::Formula, RationalFunction> const& checkTask) {
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

    auto const constantVariable = carl::VariablePool::getInstance().getFreshPersistentVariable();
    auto topologicalOrdering = utility::graph::getTopologicalSort<RationalFunction>(transitionMatrix, {initialState});
    auto flexibleMatrix = storage::FlexibleSparseMatrix<RationalFunction>(transitionMatrix);

    std::stack<uint64_t> topologicalOrderingStack;
    for (auto rit = topologicalOrdering.begin(); rit != topologicalOrdering.end(); ++rit) {
        topologicalOrderingStack.push(*rit);
    }

    // Initialize counting
    std::map<RationalFunctionVariable, std::map<uint64_t, std::set<uint64_t>>> treeStates;
    std::map<RationalFunctionVariable, std::set<uint64_t>> workingSets;

    auto backwardsTransitions = flexibleMatrix.createSparseMatrix().transpose(true);
    // Count number of parameter occurences per state
    for (uint64_t row = 0; row < flexibleMatrix.getRowCount(); row++) {
        for (auto const& entry : flexibleMatrix.getRow(row)) {
            if (entry.getValue().isConstant()) {
                continue;
            }
            STORM_LOG_ERROR_COND(entry.getValue().gatherVariables().size() == 1, "Flip minimization only supports transitions with a single parameter.");
            auto parameter = *entry.getValue().gatherVariables().begin();
            auto cache = entry.getValue().nominatorAsPolynomial().pCache();
            STORM_LOG_ERROR_COND(entry.getValue().denominator().isOne() && entry.getValue().nominator().isUnivariate() &&
                                     entry.getValue().nominator().getSingleVariable() == parameter && entry.getValue().nominator().factorization().size() == 1,
                                 "Flip minimization only supports simple pMCs.");
            STORM_LOG_ERROR_COND(flexibleMatrix.getRow(row).size() == 2, "Flip minimization only supports transitions with a single parameter.");
            workingSets[parameter].emplace(row);
            treeStates[parameter][row].emplace(row);
            if (utility::isOne(entry.getValue().derivative(entry.getValue().nominator().getSingleVariable()))) {
            } else if (utility::isOne(-entry.getValue().derivative(entry.getValue().nominator().getSingleVariable()))) {
            } else {
                STORM_LOG_ERROR_COND(false, "Flip minimization only supports transitions with a single parameter.");
            }
        }
    }

    // To prevent infinite unrolling of parametric loops
    // We have already reordered with these as root
    std::set<std::pair<RationalFunctionVariable, std::set<uint64_t>>> alreadyReorderedWrt;

    updateTreeStates(treeStates, workingSets, flexibleMatrix, allParameters, stateRewardVector, runningLabeling, labelsInFormula);
    while (!topologicalOrderingStack.empty()) {
        auto state = topologicalOrderingStack.top();
        topologicalOrderingStack.pop();
        // Check if we can reach more than one var from here (by the original matrix)
        bool performJipConvert = false;
        bool reorderingPossible = false;

        bool alreadyReorderedWrtThis = true;
        for (auto const& parameter : allParameters) {
            if (!treeStates[parameter].count(state)) {
                continue;
            }
            // If we can reach more than two equal parameters, we can reorder
            auto const entry = treeStates.at(parameter).at(state);
            if (entry.size() >= 2) {
                performJipConvert = true;
                reorderingPossible = true;
            }

            // For the duplicate checking, new states automatically count as duplicates. Thus, they are filtered out here.
            std::set<uint64_t> entriesInOldDtmc;
            const uint64_t statesOfDtmc = dtmc.getNumberOfStates();
            std::copy_if(entry.begin(), entry.end(), std::inserter(entriesInOldDtmc, entriesInOldDtmc.end()),
                         [statesOfDtmc](uint64_t value) { return value < statesOfDtmc; });

            // Check if we have already reordered w.r.t. this and enter it into the map
            if (alreadyReorderedWrt.count(std::make_pair(parameter, entriesInOldDtmc)) == 0) {
                alreadyReorderedWrtThis = false;
            }
            alreadyReorderedWrt.emplace(std::make_pair(parameter, entry));
        }

        if (!performJipConvert || alreadyReorderedWrtThis) {
            continue;
        }
        std::map<uint64_t, bool> alreadyVisited;
        collapseConstantTransitions(state, flexibleMatrix, alreadyVisited, treeStates, allParameters, stateRewardVector, runningLabeling, labelsInFormula);

#if WRITE_DTMCS
        models::sparse::Dtmc<RationalFunction> newnewDTMC(flexibleMatrix.createSparseMatrix(), runningLabeling);
        if (stateRewardVector) {
            models::sparse::StandardRewardModel<RationalFunction> newRewardModel(*stateRewardVector);
            newnewDTMC.addRewardModel(*stateRewardName, newRewardModel);
        }
        std::ofstream file;
        storm::io::openFile("dots/jipconvert_" + std::to_string(flexibleMatrix.getRowCount()) + ".dot", file);
        newnewDTMC.writeDotToStream(file);
        storm::io::closeFile(file);
        // newnewDTMC.writeDotToStream(std::cout);
        newnewDTMC.getTransitionMatrix().isProbabilistic();
#endif

        // Now our matrix is in Jip normal form. Now re-order if that is needed
        if (reorderingPossible) {
            std::map<storm::RationalFunctionVariable, std::set<uint64_t>> parameterBuckets;
            std::map<storm::RationalFunctionVariable, RationalFunction> cumulativeProbabilities;

            std::map<uint64_t, uint64_t> pTransitions;
            std::map<uint64_t, uint64_t> oneMinusPTransitions;

            std::map<uint64_t, RationalFunction> directProbs;

            std::map<storm::RationalFunctionVariable, RationalFunction> pRationalFunctions;
            std::map<storm::RationalFunctionVariable, RationalFunction> oneMinusPRationalFunctions;

            for (auto const& entry : flexibleMatrix.getRow(state)) {
                // Identify parameter of successor (or constant)
                if (stateRewardVector && !stateRewardVector->at(entry.getColumn()).isZero()) {
                    parameterBuckets[constantVariable].emplace(entry.getColumn());
                    cumulativeProbabilities[constantVariable] += entry.getValue();
                    directProbs[entry.getColumn()] = entry.getValue();
                    continue;
                }
                RationalFunctionVariable parameterOfSuccessor;
                for (auto const& entry2 : flexibleMatrix.getRow(entry.getColumn())) {
                    if (entry2.getValue().isZero()) {
                        continue;
                    }
                    if (entry2.getValue().isConstant()) {
                        parameterOfSuccessor = constantVariable;
                        break;
                    }

                    STORM_LOG_ERROR_COND(entry2.getValue().gatherVariables().size() == 1,
                                         "Flip minimization only supports transitions with a single parameter.");
                    parameterOfSuccessor = *entry2.getValue().gatherVariables().begin();
                    auto cache = entry2.getValue().nominatorAsPolynomial().pCache();
                    STORM_LOG_ERROR_COND(entry2.getValue().denominator().isOne() && entry2.getValue().nominator().isUnivariate() &&
                                             entry2.getValue().nominator().getSingleVariable() == parameterOfSuccessor &&
                                             entry2.getValue().nominator().factorization().size() == 1,
                                         "Flip minimization only supports simple pMCs.");
                    STORM_LOG_ERROR_COND(flexibleMatrix.getRow(entry.getColumn()).size() == 2,
                                         "Flip minimization only supports transitions with a single parameter.");
                    if (utility::isOne(entry2.getValue().derivative(entry2.getValue().nominator().getSingleVariable()))) {
                        pRationalFunctions[parameterOfSuccessor] = entry2.getValue();
                        pTransitions[entry.getColumn()] = entry2.getColumn();
                    } else if (utility::isOne(-entry2.getValue().derivative(entry2.getValue().nominator().getSingleVariable()))) {
                        oneMinusPRationalFunctions[parameterOfSuccessor] = entry2.getValue();
                        oneMinusPTransitions[entry.getColumn()] = entry2.getColumn();
                    } else {
                        STORM_LOG_ERROR_COND(false, "Flip minimization only supports transitions with a single parameter.");
                    }
                }
                parameterBuckets[parameterOfSuccessor].emplace(entry.getColumn());
                cumulativeProbabilities[parameterOfSuccessor] += entry.getValue();
                directProbs[entry.getColumn()] = entry.getValue();
            }

            // TODO slow could be done better if flexible matrix had ability to add states
            uint64_t newMatrixSize = flexibleMatrix.getRowCount() + 3 * parameterBuckets.size();
            if (parameterBuckets.count(constantVariable)) {
                newMatrixSize -= 2;
            }
            storage::SparseMatrixBuilder<RationalFunction> builder;
            storage::FlexibleSparseMatrix<RationalFunction> matrixWithAdditionalStates(builder.build(newMatrixSize, newMatrixSize, 0));
            for (uint64_t row = 0; row < flexibleMatrix.getRowCount(); row++) {
                matrixWithAdditionalStates.getRow(row) = flexibleMatrix.getRow(row);
            }

            workingSets.clear();

            uint64_t newStateIndex = flexibleMatrix.getRowCount();
            matrixWithAdditionalStates.getRow(state).clear();
            for (auto const& entry : parameterBuckets) {
                matrixWithAdditionalStates.getRow(state).push_back(
                    storage::MatrixEntry<uint64_t, RationalFunction>(newStateIndex, cumulativeProbabilities.at(entry.first)));
                STORM_LOG_INFO("Reorder: " << state << " -> " << newStateIndex);

                if (entry.first == constantVariable) {
                    for (auto const& successor : entry.second) {
                        matrixWithAdditionalStates.getRow(newStateIndex)
                            .push_back(storage::MatrixEntry<uint64_t, RationalFunction>(successor,
                                                                                        directProbs.at(successor) / cumulativeProbabilities.at(entry.first)));
                    }
                    // Issue: multiple transitions can go to a single state, not allowed
                    // Solution: Join them
                    matrixWithAdditionalStates.getRow(newStateIndex) = joinDuplicateTransitions(matrixWithAdditionalStates.getRow(newStateIndex));

                    workingSets[entry.first].emplace(newStateIndex);
                    for (auto const& entry : matrixWithAdditionalStates.getRow(newStateIndex)) {
                        for (auto const& parameter : allParameters) {
                            workingSets[parameter].emplace(entry.getColumn());
                        }
                    }

                    newStateIndex += 1;
                } else {
                    matrixWithAdditionalStates.getRow(newStateIndex)
                        .push_back(storage::MatrixEntry<uint64_t, RationalFunction>(newStateIndex + 1, pRationalFunctions.at(entry.first)));
                    matrixWithAdditionalStates.getRow(newStateIndex)
                        .push_back(storage::MatrixEntry<uint64_t, RationalFunction>(newStateIndex + 2, oneMinusPRationalFunctions.at(entry.first)));

                    for (auto const& successor : entry.second) {
                        // Remove transition from being counted (for now, we will re-add it below)
                        for (auto& state : treeStates.at(entry.first)) {
                            if (state.first != successor) {
                                state.second.erase(successor);
                            }
                        }
                        // If it's still needed, re-count it
                        workingSets[entry.first].emplace(successor);

                        matrixWithAdditionalStates.getRow(newStateIndex + 1)
                            .push_back(storage::MatrixEntry<uint64_t, RationalFunction>(pTransitions.at(successor),
                                                                                        directProbs.at(successor) / cumulativeProbabilities.at(entry.first)));
                        matrixWithAdditionalStates.getRow(newStateIndex + 2)
                            .push_back(storage::MatrixEntry<uint64_t, RationalFunction>(oneMinusPTransitions.at(successor),
                                                                                        directProbs.at(successor) / cumulativeProbabilities.at(entry.first)));
                    }
                    // Issue: multiple transitions can go to a single state, not allowed
                    // Solution: Join them
                    matrixWithAdditionalStates.getRow(newStateIndex + 1) = joinDuplicateTransitions(matrixWithAdditionalStates.getRow(newStateIndex + 1));
                    matrixWithAdditionalStates.getRow(newStateIndex + 2) = joinDuplicateTransitions(matrixWithAdditionalStates.getRow(newStateIndex + 2));

                    treeStates[entry.first][newStateIndex].emplace(newStateIndex);
                    workingSets[entry.first].emplace(newStateIndex);
                    workingSets[entry.first].emplace(newStateIndex + 1);
                    workingSets[entry.first].emplace(newStateIndex + 2);

                    for (auto const& entry : matrixWithAdditionalStates.getRow(newStateIndex + 1)) {
                        for (auto const& parameter : allParameters) {
                            workingSets[parameter].emplace(entry.getColumn());
                        }
                    }
                    for (auto const& entry : matrixWithAdditionalStates.getRow(newStateIndex + 2)) {
                        for (auto const& parameter : allParameters) {
                            workingSets[parameter].emplace(entry.getColumn());
                        }
                    }

                    newStateIndex += 3;
                }
            }

            // Extend labeling to more states
            models::sparse::StateLabeling nextNewLabels =
                extendStateLabeling(runningLabeling, flexibleMatrix.getRowCount(), newMatrixSize, state, labelsInFormula);

            for (uint64_t i = flexibleMatrix.getRowCount(); i < newMatrixSize; i++) {
                // Next consider the new states
                topologicalOrderingStack.push(i);
                // New states have zero reward
                if (stateRewardVector) {
                    stateRewardVector->push_back(storm::utility::zero<RationalFunction>());
                }
            }
            runningLabeling = nextNewLabels;

            updateTreeStates(treeStates, workingSets, matrixWithAdditionalStates, allParameters, stateRewardVector, runningLabeling, labelsInFormula);

            flexibleMatrix = matrixWithAdditionalStates;
            backwardsTransitions = flexibleMatrix.createSparseMatrix().transpose(true);
        }
    }

#if WRITE_DTMCS
    models::sparse::Dtmc<RationalFunction> newnewnewDTMC(flexibleMatrix.createSparseMatrix(), runningLabeling);
    if (stateRewardVector) {
        models::sparse::StandardRewardModel<RationalFunction> newRewardModel(*stateRewardVector);
        newnewnewDTMC.addRewardModel(*stateRewardName, newRewardModel);
    }
    std::ofstream file2;
    storm::io::openFile("dots/travel_" + std::to_string(flexibleMatrix.getRowCount()) + ".dot", file2);
    newnewnewDTMC.writeDotToStream(file2);
    storm::io::closeFile(file2);
    newnewnewDTMC.getTransitionMatrix().isProbabilistic();
#endif

    transitionMatrix = flexibleMatrix.createSparseMatrix();

    // Delete states (2)
    {
        storage::BitVector trueVector(transitionMatrix.getRowCount(), true);
        storage::BitVector falseVector(transitionMatrix.getRowCount(), false);
        storage::BitVector initialStates(transitionMatrix.getRowCount(), false);
        initialStates.set(initialState, true);
        storage::BitVector reachableStates = storm::utility::graph::getReachableStates(transitionMatrix, initialStates, trueVector, falseVector);
        transitionMatrix = transitionMatrix.getSubmatrix(false, reachableStates, reachableStates);
        runningLabeling = runningLabeling.getSubLabeling(reachableStates);
        uint64_t newInitialState = 0;
        for (uint64_t i = 0; i < initialState; i++) {
            if (reachableStates.get(i)) {
                newInitialState++;
            }
        }
        initialState = newInitialState;
        if (stateRewardVector) {
            std::vector<RationalFunction> newStateRewardVector;
            for (uint64_t i = 0; i < stateRewardVector->size(); i++) {
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
                                      storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix, const std::set<carl::Variable>& allParameters,
                                      const boost::optional<std::vector<RationalFunction>>& stateRewardVector,
                                      const models::sparse::StateLabeling stateLabeling, const std::set<std::string> labelsInFormula) {
    auto backwardsTransitions = flexibleMatrix.createSparseMatrix().transpose(true);
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

bool TimeTravelling::collapseConstantTransitions(uint64_t state, storage::FlexibleSparseMatrix<RationalFunction>& matrix,
                                                 std::map<uint64_t, bool>& alreadyVisited,
                                                 const std::map<RationalFunctionVariable, std::map<uint64_t, std::set<uint64_t>>>& treeStates,
                                                 const std::set<carl::Variable>& allParameters,
                                                 const boost::optional<std::vector<RationalFunction>>& stateRewardVector,
                                                 const models::sparse::StateLabeling stateLabeling, const std::set<std::string> labelsInFormula) {
    auto copiedRow = matrix.getRow(state);
    bool firstIteration = true;
    for (auto const& entry : copiedRow) {
        // ignore zero-entries
        if (entry.getValue().isZero()) {
            continue;
        }
        // if this is a parameteric transition, for now this means returning and ending our preprocessing
        if (!entry.getValue().isConstant()) {
            return false;
        }
        uint64_t nextState = entry.getColumn();
        bool continueConvertingHere;
        if (stateRewardVector && !stateRewardVector->at(entry.getColumn()).isZero()) {
            continueConvertingHere = false;
        } else if (!labelsIntersectedEqual(stateLabeling.getLabelsOfState(state), stateLabeling.getLabelsOfState(nextState), labelsInFormula)) {
            continueConvertingHere = false;
        } else {
            if (alreadyVisited.count(nextState)) {
                continueConvertingHere = alreadyVisited.at(nextState);
            } else {
                alreadyVisited[nextState] = false;
                continueConvertingHere = collapseConstantTransitions(nextState, matrix, alreadyVisited, treeStates, allParameters, stateRewardVector,
                                                                     stateLabeling, labelsInFormula);
                alreadyVisited[nextState] = continueConvertingHere;
            }
        }
        RationalFunction probability = entry.getValue();
        if (firstIteration) {
            matrix.getRow(state).clear();
            firstIteration = false;
        }
        if (continueConvertingHere) {
            for (auto const& successor : matrix.getRow(nextState)) {
                RationalFunction succProbability = successor.getValue();
                storm::storage::MatrixEntry<uint64_t, RationalFunction> newEntry(successor.getColumn(), probability * succProbability);
                STORM_LOG_INFO("JipConvert: " << state << " -> " << successor.getColumn() << " w/ " << probability * succProbability);
                matrix.getRow(state).push_back(newEntry);
            }
        } else {
            matrix.getRow(state).push_back(entry);
        }
    }
    std::sort(matrix.getRow(state).begin(), matrix.getRow(state).end(),
              [](const storage::MatrixEntry<uint64_t, RationalFunction>& a, const storage::MatrixEntry<uint64_t, RationalFunction>& b) -> bool {
                  return a.getColumn() > b.getColumn();
              });
    matrix.getRow(state) = joinDuplicateTransitions(matrix.getRow(state));
    return true;
}

class TimeTravelling;
}  // namespace transformer
}  // namespace storm
