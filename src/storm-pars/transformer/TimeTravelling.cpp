#include "TimeTravelling.h"
#include <carl/core/FactorizedPolynomial.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/RationalFunction.h>
#include <carl/core/Variable.h>
#include <carl/core/VariablePool.h>
#include <carl/core/polynomialfunctions/Factorization.h>
#include <carl/core/rootfinder/RootFinder.h>
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

RationalFunction TimeTravelling::uniPolyToRationalFunction(UniPoly uniPoly) {
    auto multivariatePol = carl::MultivariatePolynomial<RationalFunctionCoefficient>(uniPoly);
    auto multiNominator = carl::FactorizedPolynomial(multivariatePol, rawPolynomialCache);
    return RationalFunction(multiNominator);
}

std::pair<std::map<uint64_t, std::set<uint64_t>>, std::set<uint64_t>> findSubgraph(const storm::storage::FlexibleSparseMatrix<RationalFunction>& transitionMatrix,
                                                               const uint64_t root,
                                                               const std::map<RationalFunctionVariable, std::map<uint64_t, std::set<uint64_t>>>& treeStates,
                                                               const boost::optional<std::vector<RationalFunction>>& stateRewardVector,
                                                               const RationalFunctionVariable parameter) {
    std::map<uint64_t, std::set<uint64_t>> subgraph;
    std::set<uint64_t> bottomStates;

    std::set<uint64_t> acyclicStates;

    std::vector<uint64_t> dfsStack = {root};
    while (!dfsStack.empty()) {
        uint64_t state = dfsStack.back();
        // Is it a new state that we see for the first time or one we've already visited?
        if (!subgraph.count(state)) {
            subgraph[state] = {};

            std::vector<uint64_t> tmpStack;
            bool isAcyclic = true;

            // First we find out whether the state is acyclic
            for (auto const& entry : transitionMatrix.getRow(state)) {
                if (!storm::utility::isZero(entry.getValue())) {
                    if (subgraph.count(entry.getColumn() && !acyclicStates.count(entry.getColumn()))) {
                        // The state has been visited before but is not known to be acyclic.
                        isAcyclic = false;
                        break;
                    }
                }
            }

            if (!isAcyclic) {
                bottomStates.emplace(state);
                continue;
            }

            for (auto const& entry : transitionMatrix.getRow(state)) {
                if (!storm::utility::isZero(entry.getValue())) {
                    STORM_LOG_ASSERT(entry.getValue().isConstant() ||
                                         (entry.getValue().gatherVariables().size() == 1 && *entry.getValue().gatherVariables().begin() == parameter),
                                     "Called findSubgraph with incorrect parameter.");
                    // Add this edge to the subgraph
                    subgraph.at(state).emplace(entry.getColumn());
                    // If we haven't explored the node we are going to, we will need to figure out if it is a leaf or not
                    if (!subgraph.count(entry.getColumn())) {
                        bool continueSearching = treeStates.at(parameter).count(entry.getColumn()) && !treeStates.at(parameter).at(entry.getColumn()).empty();

                        // Also continue searching if there is only a transition with a one coming up, we can skip that
                        // This is nice because we can possibly combine more transitions later
                        bool onlyHasOne = transitionMatrix.getRow(entry.getColumn()).size() == 1 &&
                                          transitionMatrix.getRow(entry.getColumn()).begin()->getValue() == utility::one<RationalFunction>();
                        continueSearching |= onlyHasOne;

                        // Don't mess with rewards
                        continueSearching &= !(stateRewardVector && !stateRewardVector->at(entry.getColumn()).isZero());

                        if (continueSearching) {
                            // We are setting this state to explored once we pop it from the stack, not yet
                            // Just push it to the stack
                            tmpStack.push_back(entry.getColumn());
                        } else {
                            // This state is a leaf
                            subgraph[entry.getColumn()] = {};
                            bottomStates.emplace(entry.getColumn());

                            acyclicStates.emplace(entry.getColumn());
                        }
                    }
                }
            }

            for (auto const& entry : tmpStack) {
                dfsStack.push_back(entry);
            }
        } else {
            // Go back over the states backwards - we know these are not acyclic
            acyclicStates.emplace(state);
            dfsStack.pop_back();
        }
    }
    return std::make_pair(subgraph, bottomStates);
}

std::pair<models::sparse::Dtmc<RationalFunction>, std::map<UniPoly, Annotation>> TimeTravelling::bigStep(
    models::sparse::Dtmc<RationalFunction> const& model, modelchecker::CheckTask<logic::Formula, RationalFunction> const& checkTask) {
    models::sparse::Dtmc<RationalFunction> dtmc(model);
    storage::SparseMatrix<RationalFunction> transitionMatrix = dtmc.getTransitionMatrix();

    STORM_LOG_ASSERT(transitionMatrix.isProbabilistic(), "Gave big-step a nonprobabilistic transition matrix.");

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
            if (!entry.getValue().isConstant()) {
                if (!this->rawPolynomialCache) {
                    // So we can create new FactorizedPolynomials later
                    this->rawPolynomialCache = entry.getValue().nominator().pCache();
                }
                for (auto const& parameter : entry.getValue().gatherVariables()) {
                    treeStatesNeedUpdate[parameter].emplace(row);
                    treeStates[parameter][row].emplace(row);
                }
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

    // We will return these stored annotations to help find the zeroes
    std::map<UniPoly, Annotation> storedAnnotations;

#if WRITE_DTMCS
    uint64_t writeDtmcCounter = 0;
#endif

    while (!topologicalOrderingStack.empty()) {
        auto state = topologicalOrderingStack.top();
        topologicalOrderingStack.pop();

        if (!reachableStates.get(state)) {
            continue;
        }

        std::set<RationalFunctionVariable> parametersInState;
        for (auto const& entry : flexibleMatrix.getRow(state)) {
            for (auto const& parameter : entry.getValue().gatherVariables()) {
                parametersInState.emplace(parameter);
            }
        }

        std::set<RationalFunctionVariable> bigStepParameters;
        for (auto const& parameter : allParameters) {
            if (treeStates[parameter].count(state)) {
                // Parallel parameters
                if (treeStates.at(parameter).at(state).size() > 1) {
                    bigStepParameters.emplace(parameter);
                    continue;
                } 
                // Sequential parameters
                if (parametersInState.count(parameter)) {
                    for (auto const& treeState : treeStates[parameter][state]) {
                        for (auto const& successor : flexibleMatrix.getRow(treeState)) {
                            if (treeStates[parameter].count(successor.getColumn())) {
                                bigStepParameters.emplace(parameter);
                                break;
                            }
                        }
                    }
                }
            }
        }

        // Do big-step lifting from here
        // Follow the treeStates and eliminate transitions
        for (auto const& parameter : bigStepParameters) {
            // Find the paths along which we eliminate the transitions into one transition along with their probabilities.
            auto const [bottomAnnotations, visitedStatesAndSubtree] = bigStepBFS(state, parameter, flexibleMatrix, backwardsTransitions, treeStates, stateRewardVector, storedAnnotations);
            auto const [visitedStates, subtree] = visitedStatesAndSubtree;

            // Check the following:
            // There exists a state s in visitedStates s.t. all predecessors of s are in the subtree
            // If not, we are not eliminating any states with this big-step which baaaad and leads to the world-famous "grid issue"
            bool existsEliminableState = false;
            for (auto const& s : visitedStates) {
                bool allPredecessorsInVisitedStates = true;
                for (auto const& predecessor : backwardsTransitions.getRow(s)) {
                    // is the predecessor not in the subtree? then this state won't get eliminated
                    // is the predcessor in the subtree but the edge isn't? then this state won't get eliminated
                    if (!subtree.count(predecessor.getColumn()) || !subtree.at(predecessor.getColumn()).count(s)) {
                        allPredecessorsInVisitedStates = false;
                        break;
                    }
                }
                if (allPredecessorsInVisitedStates) {
                    // std::cout << "Eliminable state " << s << std::endl;
                    existsEliminableState = true;
                    break;
                }
            }
            // If we will not eliminate any states, do not perfom big-step
            if (!existsEliminableState) {
                continue;
            }

            // for (auto const& [state, annotation] : bottomAnnotations) {
            //     std::cout << state << ": " << annotation << std::endl;
            // }

            uint64_t oldMatrixSize = flexibleMatrix.getRowCount();

            std::vector<std::pair<uint64_t, Annotation>> transitions = findTimeTravelling(
                bottomAnnotations, parameter, flexibleMatrix, backwardsTransitions, alreadyTimeTravelledToThis, treeStatesNeedUpdate, state, originalNumStates);

            // Put paths into matrix
            auto newStoredAnnotations =
                replaceWithNewTransitions(state, transitions, flexibleMatrix, backwardsTransitions, reachableStates, treeStatesNeedUpdate);
            for (auto const& entry : newStoredAnnotations) {
                storedAnnotations.emplace(entry);
            }

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
                    for (auto& [_parameter, updateStates] : treeStatesNeedUpdate) {
                        updateStates.emplace(i);
                    }
                    // New states have zero reward
                    if (stateRewardVector) {
                        stateRewardVector->push_back(storm::utility::zero<RationalFunction>());
                    }
                }
                updateTreeStates(treeStates, treeStatesNeedUpdate, flexibleMatrix, backwardsTransitions, allParameters, stateRewardVector,
                                 runningLabelingTreeStates);
            }
            // We continue the loop through the bigStepParameters if we don't do big-step.
            // If we reach here, then we did indeed to big-step, so we will break.
            break;
        }

        // STORM_LOG_ASSERT(flexibleMatrix.createSparseMatrix().transpose() == backwardsTransitions.createSparseMatrix(), "");

#if WRITE_DTMCS
        models::sparse::Dtmc<RationalFunction> newnewDTMC(flexibleMatrix.createSparseMatrix().getSubmatrix(false, reachableStates, reachableStates),
                                                          runningLabeling.getSubLabeling(reachableStates));
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

    lastSavedAnnotations.clear();
    for (auto const& entry : storedAnnotations) {
        lastSavedAnnotations.emplace(std::make_pair(uniPolyToRationalFunction(entry.first), entry.second));
    }

    return std::make_pair(newDTMC, storedAnnotations);
}

std::pair<std::map<uint64_t, Annotation>, std::pair<std::vector<uint64_t>, std::map<uint64_t, std::set<uint64_t>>>> TimeTravelling::bigStepBFS(
    uint64_t start, const RationalFunctionVariable& parameter,
    const storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix,
    const storage::FlexibleSparseMatrix<RationalFunction>& backwardsFlexibleMatrix,
    const std::map<RationalFunctionVariable, std::map<uint64_t, std::set<uint64_t>>>& treeStates,
    const boost::optional<std::vector<RationalFunction>>& stateRewardVector,
    const std::map<UniPoly, Annotation>& storedAnnotations) {
    // Find the subgraph we will work on using DFS, following the treeStates, stopping before cycles
    auto const [subtree, bottomStates] = findSubgraph(flexibleMatrix, start, treeStates, stateRewardVector, parameter);

    // We need this to later determine which states are now unreachable
    std::vector<uint64_t> visitedStatesInBFSOrder;

    // We iterate over these annotations
    std::map<uint64_t, Annotation> annotations;

    // Set of active states in BFS
    std::set<uint64_t> activeStates = {start};

    annotations.emplace(start, Annotation(parameter, polynomialCache));
    // We go with probability one from the start to the start
    annotations.at(start)[std::vector<uint64_t>()] = utility::one<RationalFunctionCoefficient>();

    while (!activeStates.empty()) {
        std::set<uint64_t> nextActiveStates;
        for (auto const& state : activeStates) {
            visitedStatesInBFSOrder.push_back(state);
            for (auto const& entry : flexibleMatrix.getRow(state)) {
                auto const goToState = entry.getColumn();

                // Update the annotation of the target state
                if (annotations.count(goToState)) {
                    annotations.at(goToState).clear();
                } else {
                    annotations.emplace(goToState, std::move(Annotation(parameter, polynomialCache)));
                }

                // Value-iteration style
                for (auto const& backwardsEntry : backwardsFlexibleMatrix.getRow(goToState)) {
                    if (!subtree.count(backwardsEntry.getColumn()) || !subtree.at(backwardsEntry.getColumn()).count(goToState)) {
                        // We don't consider this edge for one of two reasons:
                        // (1) The node is not in the subtree.
                        // (2) The edge is not in the subtree. This can happen if to states are in the subtree for unrelated reasons
                        continue;
                    }
                    if (!annotations.count(backwardsEntry.getColumn())) {
                        // This edge is in the subtree but we just haven't gotten around to it yet. Treat it as having value zero.
                        continue;
                    }
                    auto const transition = backwardsEntry.getValue();

                    // std::cout << backwardsEntry.getColumn() << "--" << backwardsEntry.getValue() << "->" << goToState << ": ";

                    // We add stuff to this annotation
                    auto& targetAnnotation = annotations.at(goToState);

                    // std::cout << targetAnnotation << " + ";
                    // std::cout << "(" << transition << " * (" << annotations.at(backwardsEntry.getColumn()) << "))";

                    // The core of this big-step algorithm: "value-iterating" on our annotation.
                    if (transition.isConstant()) {
                        // std::cout << "(constant)";
                        targetAnnotation.addAnnotationTimesConstant(annotations.at(backwardsEntry.getColumn()), transition.constantPart());
                    } else {
                        // std::cout << "(pol)";
                        // Read transition from DTMC, convert to univariate polynomial
                        STORM_LOG_ERROR_COND(transition.denominator().isConstant(), "Only transitions with constant denominator supported but this has "
                                                                                        << transition.denominator() << " in transition " << transition);
                        auto nominator = transition.nominator();
                        UniPoly nominatorAsUnivariate = transition.nominator().toUnivariatePolynomial();
                        // Constant denominator is now distributed in the factors, not in the denominator of the rational function
                        nominatorAsUnivariate /= transition.denominator().coefficient();
                        if (storedAnnotations.count(nominatorAsUnivariate)) {
                            targetAnnotation.addAnnotationTimesAnnotation(annotations.at(backwardsEntry.getColumn()), storedAnnotations.at(nominatorAsUnivariate));
                        } else {
                            targetAnnotation.addAnnotationTimesPolynomial(annotations.at(backwardsEntry.getColumn()), std::move(nominatorAsUnivariate));
                        }
                    }

                    // std::cout << " == " << targetAnnotation << std::endl;
                }

                // Continue BFS
                if (subtree.count(goToState) && !bottomStates.count(goToState)) {
                    nextActiveStates.emplace(goToState);
                }
            }
        }
        activeStates = nextActiveStates;
    }
    // Delete annotations that are not bottom states
    for (auto const& [state, _successors] : subtree) {
        // std::cout << "Subtree of " << state << ": ";
        // for (auto const& entry : _successors) {
        //     std::cout << entry << " ";
        // }
        if (!bottomStates.count(state)) {
            annotations.erase(state);
        }
    }
    return std::make_pair(annotations, std::make_pair(visitedStatesInBFSOrder, subtree));
}

std::vector<std::pair<uint64_t, Annotation>> TimeTravelling::findTimeTravelling(
    const std::map<uint64_t, Annotation> bigStepAnnotations, const RationalFunctionVariable& parameter,
    storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix, storage::FlexibleSparseMatrix<RationalFunction>& backwardsFlexibleMatrix,
    std::map<RationalFunctionVariable, std::set<std::set<uint64_t>>>& alreadyTimeTravelledToThis,
    std::map<RationalFunctionVariable, std::set<uint64_t>>& treeStatesNeedUpdate, uint64_t root, uint64_t originalNumStates) {
    STORM_LOG_INFO("Find time travelling called with root " << root << " and parameter " << parameter);
    bool doneTimeTravelling = false;

    // Time Travelling: For transitions that divide into constants, join them into one transition leading into new state
    std::map<std::vector<uint64_t>, std::map<uint64_t, RationalFunctionCoefficient>> parametricTransitions;

    for (auto const& [state, annotation] : bigStepAnnotations) {
        for (auto const& [info, constant] : annotation) {
            if (!parametricTransitions.count(info)) {
                parametricTransitions[info] = std::map<uint64_t, RationalFunctionCoefficient>();
            }
            STORM_LOG_ASSERT(!parametricTransitions.at(info).count(state), "State already exists");
            parametricTransitions.at(info)[state] = constant;
        }
    }

    // These are the transitions that we are actually going to insert (that the function will return).
    std::vector<std::pair<uint64_t, Annotation>> insertTransitions;

    // State affected by big-step
    std::unordered_set<uint64_t> affectedStates;

    // for (auto const& [factors, transitions] : parametricTransitions) {
    //     std::cout << "Factors: ";
    //     for (uint64_t i = 0; i < factors.size(); i++) {
    //         std::cout << polynomialCache->at(parameter).second[i] << ": " << factors[i] << " ";
    //     }
    //     std::cout << std::endl;
    //     for (auto const& [state, info] : transitions) {
    //         std::cout << "State " << state << " with " << info << std::endl;
    //     }
    // }

    for (auto const& [factors, transitions] : parametricTransitions) {
        if (transitions.size() > 1) {
            // STORM_LOG_ERROR_COND(!factors.empty(), "Empty factors!");
            STORM_LOG_INFO("Time-travelling from root " << root);
            // The set of target states of the paths that we maybe want to time-travel
            std::set<uint64_t> targetStates;

            // All of these states are affected by time-travelling
            for (auto const& [state, info] : transitions) {
                affectedStates.emplace(state);
                if (state < originalNumStates) {
                    targetStates.emplace(state);
                }
            }

            Annotation newAnnotation(parameter, polynomialCache);

            RationalFunctionCoefficient constantPart = utility::zero<RationalFunctionCoefficient>();
            for (auto const& [state, transition] : transitions) {
                constantPart += transition;
            }
            newAnnotation[factors] = constantPart;

            STORM_LOG_INFO("Time travellable transitions with " << newAnnotation);

            doneTimeTravelling = true;

            // Create the new state that our parametric transitions will start in
            uint64_t newRow = flexibleMatrix.insertNewRowsAtEnd(1);
            uint64_t newRowBackwards = backwardsFlexibleMatrix.insertNewRowsAtEnd(1);
            STORM_LOG_ASSERT(newRow == newRowBackwards, "Internal error: Drifting matrix and backwardsTransitions.");

            // Sum of parametric transitions goes to new row
            insertTransitions.emplace_back(newRow, newAnnotation);

            // Write outgoing transitions from new row directly into the flexible matrix
            for (auto const& [state, thisProb] : transitions) {
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
            auto const [state, probability] = *transitions.begin();

            Annotation newAnnotation(parameter, polynomialCache);
            newAnnotation[factors] = probability;

            insertTransitions.emplace_back(state, newAnnotation);
        }
    }

    return insertTransitions;
}

std::map<UniPoly, Annotation> TimeTravelling::replaceWithNewTransitions(uint64_t state, const std::vector<std::pair<uint64_t, Annotation>> transitions,
                                                                        storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix,
                                                                        storage::FlexibleSparseMatrix<RationalFunction>& backwardsFlexibleMatrix,
                                                                        storage::BitVector& reachableStates,
                                                                        std::map<RationalFunctionVariable, std::set<uint64_t>>& treeStatesNeedUpdate) {
    std::map<UniPoly, Annotation> storedAnnotations;

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
    std::map<uint64_t, Annotation> insertThese;
    for (auto const& [target, probability] : transitions) {
        for (auto& entry : treeStatesNeedUpdate) {
            entry.second.emplace(target);
        }
        if (insertThese.count(target)) {
            insertThese.at(target) += probability;
        } else {
            insertThese.emplace(target, probability);
        }
    }
    for (auto const& [state2, annotation] : insertThese) {
        auto uniProbability = annotation.getProbability();
        storedAnnotations.emplace(uniProbability, std::move(annotation));
        auto probability = uniPolyToRationalFunction(uniProbability);

        // We know that neither no transition state <-> entry.first exist because we've erased them
        flexibleMatrix.getRow(state).push_back(storm::storage::MatrixEntry(state2, probability));
        backwardsFlexibleMatrix.getRow(state2).push_back(storm::storage::MatrixEntry(state, probability));
    }
    // STORM_LOG_ASSERT(flexibleMatrix.createSparseMatrix().transpose() == backwardsFlexibleMatrix.createSparseMatrix(), "");
    return storedAnnotations;
}

void TimeTravelling::updateUnreachableStates(storage::BitVector& reachableStates, std::vector<uint64_t> const& statesMaybeUnreachable,
                                             storage::FlexibleSparseMatrix<RationalFunction> const& backwardsFlexibleMatrix, uint64_t initialState) {
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
        std::set<uint64_t>& workingSet = workingSets[parameter];
        while (!workingSet.empty()) {
            std::set<uint64_t> newWorkingSet;
            for (uint64_t row : workingSet) {
                if (stateRewardVector && !stateRewardVector->at(row).isZero()) {
                    continue;
                }
                for (auto const& entry : backwardsTransitions.getRow(row)) {
                    if (entry.getValue().isConstant()) {
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
                        if (stateLabeling.getLabelsOfState(entry.getColumn()) == stateLabeling.getLabelsOfState(row)) {
                            newWorkingSet.emplace(entry.getColumn());
                        }
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
