#include "storm/modelchecker/multiobjective/deterministicScheds/DeterministicSchedsLpChecker.h"

#include "storm/modelchecker/prctl/helper/BaierUpperRewardBoundsComputer.h"

#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/storage/Scheduler.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/utility/graph.h"
#include "storm/utility/solver.h"

#include "storm/environment/modelchecker/MultiObjectiveModelCheckerEnvironment.h"

#include <set>
#include <storm/exceptions/UnexpectedException.h>
#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
namespace modelchecker {
namespace multiobjective {

template<typename ModelType, typename GeometryValueType>
DeterministicSchedsLpChecker<ModelType, GeometryValueType>::DeterministicSchedsLpChecker(
    ModelType const& model, std::vector<DeterministicSchedsObjectiveHelper<ModelType>> const& objectiveHelper)
    : model(model), objectiveHelper(objectiveHelper), numLpQueries(0) {
    // intentionally left empty
}

template<typename ModelType, typename GeometryValueType>
void DeterministicSchedsLpChecker<ModelType, GeometryValueType>::initialize(Environment const& env) {
    if (!lpModel) {
        swInit.start();
        initializeLpModel(env);
        swInit.stop();
    }
}

template<typename ModelType, typename GeometryValueType>
std::string DeterministicSchedsLpChecker<ModelType, GeometryValueType>::getStatistics(std::string const& prefix) const {
    std::stringstream out;
    out << prefix << swAll << " seconds for LP Checker including... \n";
    out << prefix << "  " << swInit << " seconds for LP initialization\n";
    out << prefix << "  " << swCheckWeightVectors << " seconds for checking weight vectors\n";
    out << prefix << "  " << swCheckAreas << " seconds for checking areas\n";
    out << prefix << "  " << swValidate << " seconds for validating LP solutions\n";
    out << prefix << "  " << numLpQueries << " calls to LP optimization\n";
    return out.str();
}

template<typename ModelType, typename GeometryValueType>
void DeterministicSchedsLpChecker<ModelType, GeometryValueType>::setCurrentWeightVector(Environment const& env,
                                                                                        std::vector<GeometryValueType> const& weightVector) {
    swAll.start();
    initialize(env);
    STORM_LOG_ASSERT(weightVector.size() == objectiveHelper.size(), "Setting a weight vector with invalid number of entries.");
    if (!currentWeightVector.empty()) {
        // Pop information of the current weight vector.
        lpModel->pop();
        lpModel->update();
        currentObjectiveVariables.clear();
    }

    currentWeightVector = weightVector;

    lpModel->push();
    // set up objective function for the given weight vector
    for (uint64_t objIndex = 0; objIndex < initialStateResults.size(); ++objIndex) {
        currentObjectiveVariables.push_back(
            lpModel->addUnboundedContinuousVariable("w_" + std::to_string(objIndex), storm::utility::convertNumber<ValueType>(weightVector[objIndex])));
        if (objectiveHelper[objIndex].minimizing() && flowEncoding) {
            lpModel->addConstraint("", currentObjectiveVariables.back().getExpression() == -initialStateResults[objIndex]);
        } else {
            lpModel->addConstraint("", currentObjectiveVariables.back().getExpression() == initialStateResults[objIndex]);
        }
    }
    lpModel->update();
    swAll.stop();
}

template<typename ModelType, typename GeometryValueType>
boost::optional<std::vector<GeometryValueType>> DeterministicSchedsLpChecker<ModelType, GeometryValueType>::check(storm::Environment const& env,
                                                                                                                  Polytope overapproximation) {
    swAll.start();
    initialize(env);
    STORM_LOG_ASSERT(!currentWeightVector.empty(), "Checking invoked before specifying a weight vector.");
    STORM_LOG_TRACE("Checking a vertex...");
    lpModel->push();
    auto areaConstraints = overapproximation->getConstraints(lpModel->getManager(), currentObjectiveVariables);
    for (auto const& c : areaConstraints) {
        lpModel->addConstraint("", c);
    }
    lpModel->update();
    swCheckWeightVectors.start();
    lpModel->optimize();
    swCheckWeightVectors.stop();
    ++numLpQueries;
    // STORM_PRINT_AND_LOG("Writing model to file '" << std::to_string(numLpQueries) << ".lp'\n";);
    // lpModel->writeModelToFile(std::to_string(numLpQueries) + ".lp");
    boost::optional<Point> result;
    if (!lpModel->isInfeasible()) {
        STORM_LOG_ASSERT(!lpModel->isUnbounded(), "LP result is unbounded.");
        swValidate.start();
        result = validateCurrentModel(env);
        swValidate.stop();
    }
    lpModel->pop();
    STORM_LOG_TRACE("\t Done checking a vertex...");
    swAll.stop();
    return result;
}

template<typename ModelType, typename GeometryValueType>
std::pair<std::vector<std::vector<GeometryValueType>>, std::vector<std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>>>>
DeterministicSchedsLpChecker<ModelType, GeometryValueType>::check(storm::Environment const& env,
                                                                  storm::storage::geometry::PolytopeTree<GeometryValueType>& polytopeTree, Point const& eps) {
    swAll.start();
    initialize(env);
    STORM_LOG_INFO("Checking " << polytopeTree.toString());
    STORM_LOG_ASSERT(!currentWeightVector.empty(), "Checking invoked before specifying a weight vector.");
    if (polytopeTree.isEmpty()) {
        return {{}, {}};
    }

    // Specify a gap between the obtained lower/upper objective bounds.
    // Let p be the found solution point, q be the optimal (unknown) solution point, and w be the current weight vector.
    // The gap between the solution p and q is |w*p - w*q| = |w*(p-q)|
    GeometryValueType milpGap = storm::utility::vector::dotProduct(currentWeightVector, eps);
    lpModel->setMaximalMILPGap(storm::utility::convertNumber<ValueType>(milpGap), false);
    lpModel->update();

    std::vector<Point> foundPoints;
    std::vector<Polytope> infeasableAreas;
    checkRecursive(env, polytopeTree, eps, foundPoints, infeasableAreas, 0);
    swAll.stop();
    return {foundPoints, infeasableAreas};
}

template<typename ValueType>
std::map<storm::storage::BitVector, storm::storage::BitVector> getSubEndComponents(storm::storage::SparseMatrix<ValueType> const& mecTransitions) {
    auto backwardTransitions = mecTransitions.transpose(true);
    std::map<storm::storage::BitVector, storm::storage::BitVector> unprocessed, processed;
    storm::storage::BitVector allStates(mecTransitions.getRowGroupCount(), true);
    storm::storage::BitVector allChoices(mecTransitions.getRowCount(), true);
    unprocessed[allStates] = allChoices;
    while (!unprocessed.empty()) {
        auto currentIt = unprocessed.begin();
        storm::storage::BitVector currentStates = currentIt->first;
        storm::storage::BitVector currentChoices = currentIt->second;
        unprocessed.erase(currentIt);

        bool hasSubEc = false;
        for (auto removedState : currentStates) {
            storm::storage::BitVector subset = currentStates;
            subset.set(removedState, false);
            storm::storage::MaximalEndComponentDecomposition<ValueType> subMecs(mecTransitions, backwardTransitions, subset);
            for (auto const& subMec : subMecs) {
                hasSubEc = true;
                // Convert to bitvector
                storm::storage::BitVector newEcStates(currentStates.size(), false), newEcChoices(currentChoices.size(), false);
                for (auto const& stateChoices : subMec) {
                    newEcStates.set(stateChoices.first, true);
                    for (auto const& choice : stateChoices.second) {
                        newEcChoices.set(choice, true);
                    }
                }
                if (processed.count(newEcStates) == 0) {
                    unprocessed.emplace(std::move(newEcStates), std::move(newEcChoices));
                }
            }
        }
        processed.emplace(std::move(currentStates), std::move(currentChoices));
    }

    return processed;
}

template<typename ValueType>
std::map<uint64_t, storm::expressions::Expression> processEc(storm::storage::MaximalEndComponent const& ec,
                                                             storm::storage::SparseMatrix<ValueType> const& transitions, std::string const& varNameSuffix,
                                                             std::vector<storm::expressions::Expression> const& choiceVars,
                                                             storm::solver::LpSolver<ValueType>& lpModel) {
    std::map<uint64_t, storm::expressions::Expression> ecStateVars, ecChoiceVars, ecFlowChoiceVars;
    // Compute an upper bound on the expected number of visits of the states in this ec.
    // First get a lower bound l on the probability of a path that leaves this MEC. 1-l is an upper bound on Pr_s(X F s).
    // The desired upper bound is thus 1/(1-(1-l)) = 1/l. See Baier et al., CAV'17

    // To compute l, we multiply the smallest transition probabilities occurring at each state and MEC-Choice
    // as well as the smallest 'exit' probability
    // Observe that the actual transition probabilities do not matter for this encoding.
    // Hence, we assume that all distributions are uniform to achieve a better (numerically more stable) bound
    ValueType lpath = storm::utility::one<ValueType>();
    for (auto const& stateChoices : ec) {
        uint64_t maxEntryCount = transitions.getColumnCount();
        // Choices that leave the EC are not considered in the in[s] below. Hence, also do not need to consider them here.
        for (auto const& choice : stateChoices.second) {
            maxEntryCount = std::max(maxEntryCount, transitions.getRow(choice).getNumberOfEntries());
        }
        lpath *= storm::utility::one<ValueType>() / storm::utility::convertNumber<ValueType>(maxEntryCount);
    }
    ValueType expVisitsUpperBound = storm::utility::one<ValueType>() / lpath;
    STORM_LOG_WARN_COND(expVisitsUpperBound <= storm::utility::convertNumber<ValueType>(1000.0),
                        "Large upper bound for expected visiting times: " << expVisitsUpperBound);
    // create variables
    for (auto const& stateChoices : ec) {
        ecStateVars.emplace(stateChoices.first, lpModel
                                                    .addBoundedIntegerVariable("e" + std::to_string(stateChoices.first) + varNameSuffix,
                                                                               storm::utility::zero<ValueType>(), storm::utility::one<ValueType>())
                                                    .getExpression());
        for (auto const& choice : stateChoices.second) {
            ecChoiceVars.emplace(choice, lpModel
                                             .addBoundedIntegerVariable("ec" + std::to_string(choice) + varNameSuffix, storm::utility::zero<ValueType>(),
                                                                        storm::utility::one<ValueType>())
                                             .getExpression());
            ecFlowChoiceVars.emplace(
                choice,
                lpModel.addBoundedContinuousVariable("f" + std::to_string(choice) + varNameSuffix, storm::utility::zero<ValueType>(), expVisitsUpperBound)
                    .getExpression());
        }
    }

    // create constraints
    std::map<uint64_t, std::vector<storm::expressions::Expression>> ins, outs, ecIns;
    for (auto const& stateChoices : ec) {
        std::vector<storm::expressions::Expression> ecChoiceVarsAtState;
        std::vector<storm::expressions::Expression> out;
        for (auto const& choice : stateChoices.second) {
            if (choiceVars[choice].isInitialized()) {
                lpModel.addConstraint("", ecChoiceVars[choice] <= choiceVars[choice]);
                lpModel.addConstraint("", ecFlowChoiceVars[choice] <= lpModel.getConstant(expVisitsUpperBound) * choiceVars[choice]);
            }
            ecChoiceVarsAtState.push_back(ecChoiceVars[choice]);
            out.push_back(ecFlowChoiceVars[choice]);
            ValueType fakeProbability =
                storm::utility::one<ValueType>() / storm::utility::convertNumber<ValueType, uint64_t>(transitions.getRow(choice).getNumberOfEntries());
            for (auto const& transition : transitions.getRow(choice)) {
                lpModel.addConstraint("", ecChoiceVars[choice] <= ecStateVars[transition.getColumn()]);
                ins[transition.getColumn()].push_back(lpModel.getConstant(fakeProbability) * ecFlowChoiceVars[choice]);
                ecIns[transition.getColumn()].push_back(ecChoiceVars[choice]);
            }
        }
        lpModel.addConstraint("", ecStateVars[stateChoices.first] == storm::expressions::sum(ecChoiceVarsAtState));
        out.push_back(lpModel.getConstant(expVisitsUpperBound) * ecStateVars[stateChoices.first]);
        // Iterate over choices that leave the ec
        for (uint64_t choice = transitions.getRowGroupIndices()[stateChoices.first]; choice < transitions.getRowGroupIndices()[stateChoices.first + 1];
             ++choice) {
            if (stateChoices.second.find(choice) == stateChoices.second.end()) {
                assert(choiceVars[choice].isInitialized());
                out.push_back(lpModel.getConstant(expVisitsUpperBound) * choiceVars[choice]);
            }
        }
        outs.emplace(stateChoices.first, out);
    }
    uint64_t numStates = std::distance(ec.begin(), ec.end());
    for (auto const& stateChoices : ec) {
        auto in = ins.find(stateChoices.first);
        STORM_LOG_ASSERT(in != ins.end(), "ec state does not seem to have an incoming transition.");
        // Assume a uniform initial distribution
        in->second.push_back(lpModel.getConstant(storm::utility::one<ValueType>() / storm::utility::convertNumber<ValueType>(numStates)));
        auto out = outs.find(stateChoices.first);
        STORM_LOG_ASSERT(out != outs.end(), "out flow of ec state was not set.");
        lpModel.addConstraint("", storm::expressions::sum(in->second) <= storm::expressions::sum(out->second));
        auto ecIn = ecIns.find(stateChoices.first);
        STORM_LOG_ASSERT(ecIn != ecIns.end(), "ec state does not seem to have an incoming transition.");
        lpModel.addConstraint("", storm::expressions::sum(ecIn->second) >= ecStateVars[stateChoices.first]);
    }

    return ecStateVars;
}

template<typename ModelType, typename GeometryValueType>
bool DeterministicSchedsLpChecker<ModelType, GeometryValueType>::processEndComponents(std::vector<std::vector<storm::expressions::Expression>>& ecVars) {
    uint64_t ecCounter = 0;
    auto backwardTransitions = model.getBackwardTransitions();

    // Get the choices that do not induce a value (i.e. reward) for all objectives.
    // Only MECS consisting of these choices are relevant
    storm::storage::BitVector choicesWithValueZero(model.getNumberOfChoices(), true);
    for (auto const& objHelper : objectiveHelper) {
        for (auto const& value : objHelper.getChoiceValueOffsets()) {
            STORM_LOG_ASSERT(!storm::utility::isZero(value.second), "Expected non-zero choice-value offset.");
            choicesWithValueZero.set(value.first, false);
        }
    }
    storm::storage::MaximalEndComponentDecomposition<ValueType> mecs(model.getTransitionMatrix(), backwardTransitions,
                                                                     storm::storage::BitVector(model.getNumberOfStates(), true), choicesWithValueZero);
    for (auto const& mec : mecs) {
        // For each objective we might need to split this mec into several subECs, if the objective yields a non-zero scheduler-independent state value for some
        // states of this ec. However, note that this split happens objective-wise which is why we can not consider a subsystem in the mec-decomposition above
        std::map<std::set<uint64_t>, std::vector<uint64_t>> excludedStatesToObjIndex;
        bool mecContainsSchedulerDependentValue = false;
        for (uint64_t objIndex = 0; objIndex < objectiveHelper.size(); ++objIndex) {
            std::set<uint64_t> excludedStates;
            for (auto const& stateChoices : mec) {
                auto schedIndValueIt = objectiveHelper[objIndex].getSchedulerIndependentStateValues().find(stateChoices.first);
                if (schedIndValueIt == objectiveHelper[objIndex].getSchedulerIndependentStateValues().end()) {
                    mecContainsSchedulerDependentValue = true;
                } else if (!storm::utility::isZero(schedIndValueIt->second)) {
                    excludedStates.insert(stateChoices.first);
                }
            }
            excludedStatesToObjIndex[excludedStates].push_back(objIndex);
        }

        // Skip this mec if all state values are independent of the scheduler (e.g. no reward is reachable from here).
        if (mecContainsSchedulerDependentValue) {
            for (auto const& exclStates : excludedStatesToObjIndex) {
                if (exclStates.first.empty()) {
                    auto varsForMec = processEc(mec, model.getTransitionMatrix(), "", choiceVariables, *lpModel);
                    ++ecCounter;
                    for (auto const& stateVar : varsForMec) {
                        for (auto const& objIndex : exclStates.second) {
                            ecVars[objIndex][stateVar.first] = stateVar.second;
                        }
                    }
                } else {
                    // Compute sub-end components
                    storm::storage::BitVector subEcStates(model.getNumberOfStates(), false), subEcChoices(model.getNumberOfChoices(), false);
                    for (auto const& stateChoices : mec) {
                        if (exclStates.first.count(stateChoices.first) == 0) {
                            subEcStates.set(stateChoices.first, true);
                            for (auto const& choice : stateChoices.second) {
                                subEcChoices.set(choice, true);
                            }
                        }
                    }
                    storm::storage::MaximalEndComponentDecomposition<ValueType> subEcs(model.getTransitionMatrix(), backwardTransitions, subEcStates,
                                                                                       subEcChoices);
                    for (auto const& subEc : subEcs) {
                        auto varsForSubEc =
                            processEc(subEc, model.getTransitionMatrix(), "o" + std::to_string(*exclStates.second.begin()), choiceVariables, *lpModel);
                        ++ecCounter;
                        for (auto const& stateVar : varsForSubEc) {
                            for (auto const& objIndex : exclStates.second) {
                                ecVars[objIndex][stateVar.first] = stateVar.second;
                            }
                        }
                    }
                }
            }
        }
    }
    bool hasEndComponents =
        ecCounter > 0 || storm::utility::graph::checkIfECWithChoiceExists(model.getTransitionMatrix(), backwardTransitions,
                                                                          storm::storage::BitVector(model.getNumberOfStates(), true), ~choicesWithValueZero);
    STORM_LOG_WARN_COND(!hasEndComponents, "Processed " << ecCounter << " End components.");
    return hasEndComponents;
}

template<typename ModelType, typename GeometryValueType>
void DeterministicSchedsLpChecker<ModelType, GeometryValueType>::initializeLpModel(Environment const& env) {
    STORM_LOG_INFO("Initializing LP model with " << model.getNumberOfStates() << " states.");
    if (env.modelchecker().multi().getEncodingType() == storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Auto) {
        flowEncoding = true;
        for (auto const& helper : objectiveHelper) {
            if (!helper.isTotalRewardObjective()) {
                flowEncoding = false;
            }
        }
    } else {
        flowEncoding = env.modelchecker().multi().getEncodingType() == storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Flow;
    }
    STORM_LOG_INFO("Using " << (flowEncoding ? "flow" : "classical") << " encoding.\n");

    uint64_t numStates = model.getNumberOfStates();
    uint64_t initialState = *model.getInitialStates().begin();
    STORM_LOG_WARN_COND(!storm::settings::getModule<storm::settings::modules::CoreSettings>().isLpSolverSetFromDefaultValue() ||
                            storm::settings::getModule<storm::settings::modules::CoreSettings>().getLpSolver() == storm::solver::LpSolverType::Gurobi,
                        "The selected MILP solver might not perform well. Consider installing / using Gurobi.");
    lpModel = storm::utility::solver::getLpSolver<ValueType>("model");

    lpModel->setOptimizationDirection(storm::solver::OptimizationDirection::Maximize);
    initialStateResults.clear();

    auto zero = lpModel->getConstant(storm::utility::zero<ValueType>());
    auto one = lpModel->getConstant(storm::utility::one<ValueType>());
    auto const& groups = model.getTransitionMatrix().getRowGroupIndices();
    // Create choice variables.
    choiceVariables.reserve(model.getNumberOfChoices());
    for (uint64_t state = 0; state < numStates; ++state) {
        uint64_t numChoices = model.getNumberOfChoices(state);
        if (numChoices == 1) {
            choiceVariables.emplace_back();
        } else {
            std::vector<storm::expressions::Expression> localChoices;
            for (uint64_t choice = 0; choice < numChoices; ++choice) {
                localChoices.push_back(lpModel->addBoundedIntegerVariable("c" + std::to_string(state) + "_" + std::to_string(choice), 0, 1).getExpression());
                choiceVariables.push_back(localChoices.back());
            }
            lpModel->addConstraint("", storm::expressions::sum(localChoices) == one);
        }
    }
    // Create ec Variables for each state/objective
    std::vector<std::vector<storm::expressions::Expression>> ecVars(objectiveHelper.size(),
                                                                    std::vector<storm::expressions::Expression>(model.getNumberOfStates()));
    bool hasEndComponents = processEndComponents(ecVars);

    if (flowEncoding) {
        storm::storage::BitVector bottomStates(model.getNumberOfStates(), true);
        for (auto const& helper : objectiveHelper) {
            STORM_LOG_THROW(helper.isTotalRewardObjective(), storm::exceptions::InvalidOperationException,
                            "The given type of encoding is only supported if the objectives can be reduced to total reward objectives.");
            storm::storage::BitVector objBottomStates(model.getNumberOfStates(), false);
            for (auto const& stateVal : helper.getSchedulerIndependentStateValues()) {
                STORM_LOG_THROW(storm::utility::isZero(stateVal.second), storm::exceptions::InvalidOperationException,
                                "Non-zero constant state-values not allowed for flow encoding.");
                objBottomStates.set(stateVal.first, true);
            }
            bottomStates &= objBottomStates;
        }
        storm::storage::BitVector nonBottomStates = ~bottomStates;
        STORM_LOG_TRACE("Found " << bottomStates.getNumberOfSetBits() << " bottom states.");

        // Compute upper bounds for each state
        std::vector<ValueType> visitingTimesUpperBounds = DeterministicSchedsObjectiveHelper<ModelType>::computeUpperBoundOnExpectedVisitingTimes(
            model.getTransitionMatrix(), bottomStates, nonBottomStates, hasEndComponents);
        ValueType largestUpperBound = *std::max_element(visitingTimesUpperBounds.begin(), visitingTimesUpperBounds.end());
        STORM_LOG_WARN_COND(largestUpperBound < storm::utility::convertNumber<ValueType>(1e5),
                            "Found a large upper bound '" << storm::utility::convertNumber<double>(largestUpperBound)
                                                          << "' in the LP encoding. This might trigger numerical instabilities.");
        // create choiceValue variables and assert deterministic ones.
        std::vector<storm::expressions::Expression> choiceValVars(model.getNumberOfChoices());
        for (auto state : nonBottomStates) {
            for (uint64_t globalChoice = groups[state]; globalChoice < groups[state + 1]; ++globalChoice) {
                choiceValVars[globalChoice] =
                    lpModel
                        ->addBoundedContinuousVariable("y" + std::to_string(globalChoice), storm::utility::zero<ValueType>(), visitingTimesUpperBounds[state])
                        .getExpression();
                if (model.getNumberOfChoices(state) > 1) {
                    ;
                    lpModel->addConstraint(
                        "", choiceValVars[globalChoice] <= lpModel->getConstant(visitingTimesUpperBounds[state]) * choiceVariables[globalChoice]);
                }
            }
        }
        // create EC 'slack' variables for states that lie in an ec
        std::vector<storm::expressions::Expression> ecValVars(model.getNumberOfStates());
        if (hasEndComponents) {
            for (auto state : nonBottomStates) {
                // For the in-out-encoding, all objectives have the same ECs (because there are no non-zero scheduler independend state values).
                // Hence, we only care for the variables of the first objective.
                if (ecVars.front()[state].isInitialized()) {
                    ecValVars[state] =
                        lpModel->addBoundedContinuousVariable("z" + std::to_string(state), storm::utility::zero<ValueType>(), visitingTimesUpperBounds[state])
                            .getExpression();
                    lpModel->addConstraint("", ecValVars[state] <= lpModel->getConstant(visitingTimesUpperBounds[state]) * ecVars.front()[state]);
                }
            }
        }
        // Get 'in' and 'out' expressions
        std::vector<storm::expressions::Expression> bottomStatesIn;
        std::vector<std::vector<storm::expressions::Expression>> ins(numStates), outs(numStates);
        ins[initialState].push_back(one);
        for (auto state : nonBottomStates) {
            for (uint64_t globalChoice = groups[state]; globalChoice < groups[state + 1]; ++globalChoice) {
                for (auto const& transition : model.getTransitionMatrix().getRow(globalChoice)) {
                    uint64_t successor = transition.getColumn();
                    storm::expressions::Expression exp = lpModel->getConstant(transition.getValue()) * choiceValVars[globalChoice];
                    if (bottomStates.get(successor)) {
                        bottomStatesIn.push_back(exp);
                    } else {
                        ins[successor].push_back(exp);
                    }
                }
                outs[state].push_back(choiceValVars[globalChoice]);
            }
            if (ecValVars[state].isInitialized()) {
                outs[state].push_back(ecValVars[state]);
                bottomStatesIn.push_back(ecValVars[state]);
            }
        }

        // Assert 'in == out' at each state
        for (auto state : nonBottomStates) {
            lpModel->addConstraint("", storm::expressions::sum(ins[state]) == storm::expressions::sum(outs[state]));
        }
        // Assert the sum for the bottom states
        lpModel->addConstraint("", storm::expressions::sum(bottomStatesIn) == one);

        // create initial state results for each objective
        for (uint64_t objIndex = 0; objIndex < objectiveHelper.size(); ++objIndex) {
            auto choiceValueOffsets = objectiveHelper[objIndex].getChoiceValueOffsets();
            std::vector<storm::expressions::Expression> objValue;
            for (auto state : nonBottomStates) {
                for (uint64_t globalChoice = groups[state]; globalChoice < groups[state + 1]; ++globalChoice) {
                    auto choiceValueIt = choiceValueOffsets.find(globalChoice);
                    if (choiceValueIt != choiceValueOffsets.end()) {
                        assert(!storm::utility::isZero(choiceValueIt->second));
                        objValue.push_back(lpModel->getConstant(choiceValueIt->second) * choiceValVars[globalChoice]);
                    }
                }
            }
            ValueType lowerBound = objectiveHelper[objIndex].getLowerValueBoundAtState(env, initialState);
            ValueType upperBound = objectiveHelper[objIndex].getUpperValueBoundAtState(env, initialState);
            storm::expressions::Expression objValueVariable;
            if (lowerBound == upperBound) {
                // GLPK does not like point-intervals......
                objValueVariable = lpModel->getConstant(lowerBound);
            } else {
                objValueVariable = lpModel->addBoundedContinuousVariable("x" + std::to_string(objIndex), lowerBound, upperBound).getExpression();
            }
            if (objValue.empty()) {
                lpModel->addConstraint("", objValueVariable == zero);
            } else {
                lpModel->addConstraint("", objValueVariable == storm::expressions::sum(objValue));
            }
            initialStateResults.push_back(objValueVariable);
        }
    } else {
        // 'classic' backward encoding.
        for (uint64_t objIndex = 0; objIndex < objectiveHelper.size(); ++objIndex) {
            STORM_LOG_WARN_COND(objectiveHelper[objIndex].getLargestUpperBound(env) < storm::utility::convertNumber<ValueType>(1e5),
                                "Found a large upper value bound '"
                                    << storm::utility::convertNumber<double>(objectiveHelper[objIndex].getLargestUpperBound(env))
                                    << "' in the LP encoding. This might trigger numerical instabilities.");
            auto const& schedulerIndependentStates = objectiveHelper[objIndex].getSchedulerIndependentStateValues();
            // Create state variables and store variables of ecs which contain a state with a scheduler independent value
            std::vector<storm::expressions::Expression> stateVars;
            stateVars.reserve(numStates);
            for (uint64_t state = 0; state < numStates; ++state) {
                auto valIt = schedulerIndependentStates.find(state);
                if (valIt == schedulerIndependentStates.end()) {
                    ValueType lowerBound = objectiveHelper[objIndex].getLowerValueBoundAtState(env, state);
                    ValueType upperBound = objectiveHelper[objIndex].getUpperValueBoundAtState(env, state);
                    if (lowerBound == upperBound) {
                        // glpk does not like variables with point-interval bounds...
                        if (objectiveHelper[objIndex].minimizing()) {
                            stateVars.push_back(lpModel->getConstant(-lowerBound));
                        } else {
                            stateVars.push_back(lpModel->getConstant(lowerBound));
                        }
                    } else {
                        if (objectiveHelper[objIndex].minimizing()) {
                            stateVars.push_back(
                                lpModel->addBoundedContinuousVariable("x" + std::to_string(objIndex) + "_" + std::to_string(state), -upperBound, -lowerBound)
                                    .getExpression());
                        } else {
                            stateVars.push_back(
                                lpModel->addBoundedContinuousVariable("x" + std::to_string(objIndex) + "_" + std::to_string(state), lowerBound, upperBound)
                                    .getExpression());
                        }
                    }
                } else {
                    ValueType value = valIt->second;
                    if (objectiveHelper[objIndex].minimizing()) {
                        value = -value;
                    }
                    stateVars.push_back(lpModel->getConstant(value));
                }
                if (state == initialState) {
                    initialStateResults.push_back(stateVars.back());
                }
            }

            // Create and assert choice values
            auto const& choiceValueOffsets = objectiveHelper[objIndex].getChoiceValueOffsets();
            for (uint64_t state = 0; state < numStates; ++state) {
                if (schedulerIndependentStates.find(state) != schedulerIndependentStates.end()) {
                    continue;
                }
                storm::expressions::Expression stateValue;
                uint64_t numChoices = model.getNumberOfChoices(state);
                uint64_t choiceOffset = groups[state];
                storm::expressions::Expression upperValueBoundAtState = lpModel->getConstant(objectiveHelper[objIndex].getUpperValueBoundAtState(env, state));
                for (uint64_t choice = 0; choice < numChoices; ++choice) {
                    storm::expressions::Expression choiceValue;
                    auto valIt = choiceValueOffsets.find(choiceOffset + choice);
                    if (valIt != choiceValueOffsets.end()) {
                        if (objectiveHelper[objIndex].minimizing()) {
                            choiceValue = lpModel->getConstant(-valIt->second);
                        } else {
                            choiceValue = lpModel->getConstant(valIt->second);
                        }
                    }
                    for (auto const& transition : model.getTransitionMatrix().getRow(state, choice)) {
                        if (!storm::utility::isZero(transition.getValue())) {
                            storm::expressions::Expression transitionValue = lpModel->getConstant(transition.getValue()) * stateVars[transition.getColumn()];
                            if (choiceValue.isInitialized()) {
                                choiceValue = choiceValue + transitionValue;
                            } else {
                                choiceValue = transitionValue;
                            }
                        }
                    }
                    choiceValue = choiceValue.simplify().reduceNesting();
                    if (numChoices == 1) {
                        stateValue = choiceValue;
                    } else {
                        uint64_t globalChoiceIndex = groups[state] + choice;
                        storm::expressions::Expression choiceValVar;
                        if (objectiveHelper[objIndex].minimizing()) {
                            choiceValVar = lpModel
                                               ->addBoundedContinuousVariable(
                                                   "x" + std::to_string(objIndex) + "_" + std::to_string(state) + "_" + std::to_string(choice),
                                                   -objectiveHelper[objIndex].getUpperValueBoundAtState(env, state), storm::utility::zero<ValueType>())
                                               .getExpression();
                        } else {
                            choiceValVar = lpModel
                                               ->addBoundedContinuousVariable(
                                                   "x" + std::to_string(objIndex) + "_" + std::to_string(state) + "_" + std::to_string(choice),
                                                   storm::utility::zero<ValueType>(), objectiveHelper[objIndex].getUpperValueBoundAtState(env, state))
                                               .getExpression();
                        }
                        lpModel->addConstraint("", choiceValVar <= choiceValue);
                        if (objectiveHelper[objIndex].minimizing()) {
                            lpModel->addConstraint("", choiceValVar <= -upperValueBoundAtState * (one - choiceVariables[globalChoiceIndex]));
                        } else {
                            lpModel->addConstraint("", choiceValVar <= upperValueBoundAtState * choiceVariables[globalChoiceIndex]);
                        }
                        if (choice == 0) {
                            stateValue = choiceValVar;
                        } else {
                            stateValue = stateValue + choiceValVar;
                        }
                    }
                }
                stateValue.simplify().reduceNesting();
                if (objectiveHelper[objIndex].minimizing()) {
                    lpModel->addConstraint(
                        "", stateVars[state] <=
                                stateValue + (lpModel->getConstant(storm::utility::convertNumber<ValueType>(numChoices - 1)) * upperValueBoundAtState));
                } else {
                    lpModel->addConstraint("", stateVars[state] <= stateValue);
                }
                if (numChoices > 1 && hasEndComponents) {
                    auto& ecVar = ecVars[objIndex][state];
                    if (ecVar.isInitialized()) {
                        // if this state is part of an ec, make sure to assign a value of zero.
                        if (objectiveHelper[objIndex].minimizing()) {
                            // This part is optional
                            lpModel->addConstraint(
                                "", stateVars[state] >= (ecVar - one) * lpModel->getConstant(objectiveHelper[objIndex].getUpperValueBoundAtState(env, state)));
                        } else {
                            lpModel->addConstraint(
                                "", stateVars[state] <= (one - ecVar) * lpModel->getConstant(objectiveHelper[objIndex].getUpperValueBoundAtState(env, state)));
                        }
                    }
                }
            }
        }
    }
    lpModel->update();
    STORM_LOG_INFO("Done initializing LP model.");
}

template<typename ModelType, typename GeometryValueType>
void DeterministicSchedsLpChecker<ModelType, GeometryValueType>::checkRecursive(Environment const& env,
                                                                                storm::storage::geometry::PolytopeTree<GeometryValueType>& polytopeTree,
                                                                                Point const& eps, std::vector<Point>& foundPoints,
                                                                                std::vector<Polytope>& infeasableAreas, uint64_t const& depth) {
    STORM_LOG_ASSERT(!polytopeTree.isEmpty(), "Tree node is empty");
    STORM_LOG_ASSERT(!polytopeTree.getPolytope()->isEmpty(), "Tree node is empty.");
    STORM_LOG_TRACE("Checking at depth " << depth << ": " << polytopeTree.toString());

    lpModel->push();
    // Assert the constraints of the current polytope
    auto nodeConstraints = polytopeTree.getPolytope()->getConstraints(lpModel->getManager(), currentObjectiveVariables);
    for (auto const& constr : nodeConstraints) {
        lpModel->addConstraint("", constr);
    }
    lpModel->update();

    if (polytopeTree.getChildren().empty()) {
        // At leaf nodes we need to perform the actual check.

        // Numerical instabilities might yield a point that is not actually inside the nodeConstraints.
        // If the downward closure is disjoint from this tree node, we will not make further progress.
        // In this case, we sharpen the violating constraints just a little bit.
        // This way, valid solutions might be excluded, so technically, this can yield false negatives.
        // However, since we apparently are dealing with numerical algorithms, we can't be sure about correctness anyway.
        uint64_t num_sharpen = 0;
        auto halfspaces = polytopeTree.getPolytope()->getHalfspaces();
        while (true) {
            STORM_LOG_TRACE("\tSolving MILP...");
            swCheckAreas.start();
            lpModel->optimize();
            swCheckAreas.stop();
            ++numLpQueries;
            STORM_LOG_TRACE("\tDone solving MILP...");

            // STORM_PRINT_AND_LOG("Writing model to file '" << polytopeTree.toId() << ".lp'\n";);
            // lpModel->writeModelToFile(polytopeTree.toId() + ".lp");

            if (lpModel->isInfeasible()) {
                infeasableAreas.push_back(polytopeTree.getPolytope());
                polytopeTree.clear();
                break;
            } else {
                STORM_LOG_ASSERT(!lpModel->isUnbounded(), "LP result is unbounded.");
                swValidate.start();
                Point newPoint = validateCurrentModel(env);
                swValidate.stop();
                // Check whether this new point yields any progress.
                // There is no progress if (due to numerical inaccuracies) the downwardclosure (including points that are epsilon close to it) contained in this
                // polytope. We multiply eps by 0.999 so that points that lie on the boundary of polytope and downw. do not count in the intersection.
                Point newPointPlusEps = newPoint;
                storm::utility::vector::addScaledVector(newPointPlusEps, eps, storm::utility::convertNumber<GeometryValueType>(0.999));
                if (polytopeTree.getPolytope()->contains(newPoint) ||
                    !polytopeTree.getPolytope()
                         ->intersection(storm::storage::geometry::Polytope<GeometryValueType>::createDownwardClosure({newPointPlusEps}))
                         ->isEmpty()) {
                    GeometryValueType offset = storm::utility::convertNumber<GeometryValueType>(lpModel->getObjectiveValue());
                    // Get the gap between the found solution and the known bound.
                    offset += storm::utility::convertNumber<GeometryValueType>(lpModel->getMILPGap(false));
                    // we might want to shift the halfspace to guarantee that our point is included.
                    offset = std::max(offset, storm::utility::vector::dotProduct(currentWeightVector, newPoint));
                    auto halfspace = storm::storage::geometry::Halfspace<GeometryValueType>(currentWeightVector, offset).invert();
                    infeasableAreas.push_back(polytopeTree.getPolytope()->intersection(halfspace));
                    if (infeasableAreas.back()->isEmpty()) {
                        infeasableAreas.pop_back();
                    }
                    polytopeTree.setMinus(storm::storage::geometry::Polytope<GeometryValueType>::create({halfspace}));
                    foundPoints.push_back(newPoint);
                    polytopeTree.substractDownwardClosure(newPoint, eps);
                    if (!polytopeTree.isEmpty()) {
                        checkRecursive(env, polytopeTree, eps, foundPoints, infeasableAreas, depth);
                    }
                    break;
                } else {
                    // If we end up here, we have to sharpen the violated constraints for this polytope
                    for (auto& h : halfspaces) {
                        GeometryValueType distance = h.distance(newPoint);
                        // Check if the found point is outside of this halfspace
                        if (!storm::utility::isZero(distance)) {
                            // The issue has to be for some normal vector with a negative entry. Otherwise, the intersection with the downward closure wouldn't
                            // be empty
                            bool normalVectorContainsNegative = false;
                            for (auto const& hi : h.normalVector()) {
                                if (hi < storm::utility::zero<GeometryValueType>()) {
                                    normalVectorContainsNegative = true;
                                    break;
                                }
                            }
                            if (normalVectorContainsNegative) {
                                h.offset() -= distance / storm::utility::convertNumber<GeometryValueType, uint64_t>(2);
                                if (num_sharpen == 0) {
                                    lpModel->push();
                                }
                                lpModel->addConstraint("", h.toExpression(lpModel->getManager(), currentObjectiveVariables));
                                ++num_sharpen;
                                break;
                            }
                        }
                    }
                    STORM_LOG_TRACE("\tSharpened LP");
                }
            }
        }
        if (num_sharpen > 0) {
            STORM_LOG_WARN(
                "Numerical instabilities detected: LP Solver found an achievable point outside of the search area. The search area had to be sharpened "
                << num_sharpen << " times.");
            // pop sharpened constraints
            lpModel->pop();
        }

    } else {
        // Traverse all the non-empty children.
        for (uint64_t childId = 0; childId < polytopeTree.getChildren().size(); ++childId) {
            if (polytopeTree.getChildren()[childId].isEmpty()) {
                continue;
            }
            uint64_t newPointIndex = foundPoints.size();
            checkRecursive(env, polytopeTree.getChildren()[childId], eps, foundPoints, infeasableAreas, depth + 1);
            STORM_LOG_ASSERT(polytopeTree.getChildren()[childId].isEmpty(), "expected empty children.");
            // Make the new points known to the right siblings
            for (; newPointIndex < foundPoints.size(); ++newPointIndex) {
                for (uint64_t siblingId = childId + 1; siblingId < polytopeTree.getChildren().size(); ++siblingId) {
                    polytopeTree.getChildren()[siblingId].substractDownwardClosure(foundPoints[newPointIndex], eps);
                }
            }
        }
        // All children are empty now, so this node becomes empty.
        polytopeTree.clear();
    }
    STORM_LOG_TRACE("Checking DONE at depth " << depth << " with node " << polytopeTree.toString());

    lpModel->pop();
    lpModel->update();
}

template<typename ModelType, typename GeometryValueType>
typename DeterministicSchedsLpChecker<ModelType, GeometryValueType>::Point DeterministicSchedsLpChecker<ModelType, GeometryValueType>::validateCurrentModel(
    Environment const& env) const {
    storm::storage::Scheduler<ValueType> scheduler(model.getNumberOfStates());
    for (uint64_t state = 0; state < model.getNumberOfStates(); ++state) {
        uint64_t numChoices = model.getNumberOfChoices(state);
        if (numChoices == 1) {
            scheduler.setChoice(0, state);
        } else {
            uint64_t globalChoiceOffset = model.getTransitionMatrix().getRowGroupIndices()[state];
            bool choiceFound = false;
            for (uint64_t localChoice = 0; localChoice < numChoices; ++localChoice) {
                bool localChoiceEnabled = false;
                localChoiceEnabled =
                    (lpModel->getIntegerValue(choiceVariables[globalChoiceOffset + localChoice].getBaseExpression().asVariableExpression().getVariable()) == 1);
                if (localChoiceEnabled) {
                    STORM_LOG_THROW(!choiceFound, storm::exceptions::UnexpectedException, "Multiple choices selected at state " << state);
                    scheduler.setChoice(localChoice, state);
                    choiceFound = true;
                }
            }
            STORM_LOG_THROW(choiceFound, storm::exceptions::UnexpectedException, "No choice selected at state " << state);
        }
    }
    bool dropUnreachableStates = true;
    bool preserveModelType = true;
    auto inducedModel = model.applyScheduler(scheduler, dropUnreachableStates, preserveModelType)->template as<ModelType>();
    Point inducedPoint;
    for (uint64_t objIndex = 0; objIndex < objectiveHelper.size(); ++objIndex) {
        ValueType inducedValue = objectiveHelper[objIndex].evaluateOnModel(env, *inducedModel);
        if (objectiveHelper[objIndex].minimizing()) {
            inducedValue = -inducedValue;
        }
        inducedPoint.push_back(storm::utility::convertNumber<GeometryValueType>(inducedValue));
        // If this objective has weight zero, the lp solution is not necessarily correct
        if (!storm::utility::isZero(currentWeightVector[objIndex])) {
            ValueType lpValue = lpModel->getContinuousValue(currentObjectiveVariables[objIndex]);
            double diff = storm::utility::convertNumber<double>(storm::utility::abs<ValueType>(inducedValue - lpValue));
            STORM_LOG_WARN_COND(diff <= 1e-4 * std::abs(storm::utility::convertNumber<double>(inducedValue)),
                                "Imprecise value for objective " << objIndex << ": LP says " << lpValue << " but scheduler induces " << inducedValue
                                                                 << " (difference is " << diff << ")");
        }
    }
    return inducedPoint;
}

template class DeterministicSchedsLpChecker<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
template class DeterministicSchedsLpChecker<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber>;
template class DeterministicSchedsLpChecker<storm::models::sparse::MarkovAutomaton<double>, storm::RationalNumber>;
template class DeterministicSchedsLpChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>, storm::RationalNumber>;
}  // namespace multiobjective
}  // namespace modelchecker
}  // namespace storm
