#include "storm/modelchecker/multiobjective/deterministicScheds/DeterministicSchedsLpChecker.h"

#include "storm/modelchecker/prctl/helper/BaierUpperRewardBoundsComputer.h"

#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/MultiObjectiveSettings.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/storage/Scheduler.h"
#include "storm/utility/graph.h"
#include "storm/utility/solver.h"

#include "storm/environment/modelchecker/MultiObjectiveModelCheckerEnvironment.h"

#include "storm/exceptions/InvalidOperationException.h"
#include <set>
#include <storm/exceptions/UnexpectedException.h>

namespace storm {
    namespace modelchecker {
        namespace multiobjective {

            
            storm::storage::BitVector encodingSettings() {
                storm::storage::BitVector res(64, false);
                if (storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().isMaxStepsSet()) {
                    res.setFromInt(0, 64, storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getMaxSteps());
                }
                return res;
            }
            
            bool isMaxDiffEncoding() { // + 2
                bool result = encodingSettings().get(62);
                STORM_LOG_ERROR_COND(!result || !isMinNegativeEncoding(), "maxDiffEncoding only works without minnegative encoding.");
                return result;
            }
            
            bool choiceVarReduction() { // + 4
                return encodingSettings().get(61);
            }
            
            bool inOutEncoding() { // + 8
                bool result = encodingSettings().get(60);
                STORM_LOG_ERROR_COND(!result || !isMinNegativeEncoding(), "inout-encoding only works without minnegative encoding.");
                return result;
            }
            
            bool assertBottomStateSum() { // + 16
                bool result = encodingSettings().get(59);
                STORM_LOG_ERROR_COND(!result || inOutEncoding(), "Asserting bottom state sum is only relevant for in-out encoding.");
                return result;
            }
            
            bool useNonOptimalSolutions() { // + 32
                bool result = encodingSettings().get(58);
                return result;
            }
            
            template <typename ModelType, typename GeometryValueType>
            DeterministicSchedsLpChecker<ModelType, GeometryValueType>::DeterministicSchedsLpChecker(Environment const& env, ModelType const& model, std::vector<DeterministicSchedsObjectiveHelper<ModelType>> const& objectiveHelper) : model(model) , objectiveHelper(objectiveHelper), numLpQueries(0) {
                swAll.start();
                swInit.start();
                initializeLpModel(env);
                swInit.stop();
                swAll.stop();
            }
            
            template <typename ModelType, typename GeometryValueType>
            std::string  DeterministicSchedsLpChecker<ModelType, GeometryValueType>::getStatistics(std::string const& prefix) const {
                std::stringstream out;
                out << prefix << swAll << " seconds for LP Checker including... " << std::endl;
                out << prefix << "  " << swInit << " seconds for LP initialization" << std::endl;
                out << prefix << "  " << swCheckWeightVectors << " seconds for checking weight vectors" << std::endl;
                out << prefix << "  " << swCheckAreas << " seconds for checking areas" << std::endl;
                out << prefix << "  " << swValidate << " seconds for validating LP solutions" << std::endl;
                out << prefix << "  " << numLpQueries << " calls to LP optimization" << std::endl;
                return out.str();
            }
            
            template <typename ModelType, typename GeometryValueType>
            void DeterministicSchedsLpChecker<ModelType, GeometryValueType>::setCurrentWeightVector(std::vector<GeometryValueType> const& weightVector) {
                swAll.start();
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
                    currentObjectiveVariables.push_back(lpModel->addUnboundedContinuousVariable("w_" + std::to_string(objIndex), storm::utility::convertNumber<ValueType>(weightVector[objIndex])));
                    if (objectiveHelper[objIndex].minimizing() && flowEncoding) {
                        lpModel->addConstraint("", currentObjectiveVariables.back().getExpression() == -initialStateResults[objIndex]);
                    } else {
                        lpModel->addConstraint("", currentObjectiveVariables.back().getExpression() == initialStateResults[objIndex]);
                    }
                }
                lpModel->update();
                swAll.stop();
            }
            
            template <typename ModelType, typename GeometryValueType>
            boost::optional<std::vector<GeometryValueType>> DeterministicSchedsLpChecker<ModelType, GeometryValueType>::check(storm::Environment const& env, Polytope overapproximation) {
                swAll.start();
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
                STORM_LOG_TRACE("\t Done checking a vertex...");
                boost::optional<Point> result;
                if (!lpModel->isInfeasible()) {
                    STORM_LOG_ASSERT(!lpModel->isUnbounded(), "LP result is unbounded.");
                    swValidate.start();
                    result = validateCurrentModel(env);
                    swValidate.stop();
                }
                lpModel->pop();
                swAll.stop();
                return result;
            }

            template <typename ModelType, typename GeometryValueType>
            std::pair<std::vector<std::vector<GeometryValueType>>, std::vector<std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>>>> DeterministicSchedsLpChecker<ModelType, GeometryValueType>::check(storm::Environment const& env, storm::storage::geometry::PolytopeTree<GeometryValueType>& polytopeTree, Point const& eps) {
                swAll.start();
                STORM_LOG_INFO("Checking " << polytopeTree.toString());
                STORM_LOG_ASSERT(!currentWeightVector.empty(), "Checking invoked before specifying a weight vector.");
                if (polytopeTree.isEmpty()) {
                    return {{}, {}};
                }
                
                if (gurobiLpModel) {
                    // For gurobi, it is possible to specify a gap between the obtained lower/upper objective bounds.
                    // Let p be the found solution point, q be the optimal (unknown) solution point, and w be the current weight vector.
                    // The gap between the solution p and q is |w*p - w*q| = |w*(p-q)|
                    GeometryValueType milpGap = storm::utility::vector::dotProduct(currentWeightVector, eps);
                    gurobiLpModel->setMaximalMILPGap(storm::utility::convertNumber<ValueType>(milpGap), false);
                    gurobiLpModel->update();
                }
                
                std::vector<Point> foundPoints;
                std::vector<Polytope> infeasableAreas;
                checkRecursive(env, polytopeTree, eps, foundPoints, infeasableAreas, 0);
                swAll.stop();
                return {foundPoints, infeasableAreas};
            }
            
            
            template <typename ValueType>
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
                    for (auto const& removedState : currentStates) {
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
            
            template <typename ValueType>
            std::map<uint64_t, storm::expressions::Expression> processEc(storm::storage::MaximalEndComponent const& ec, storm::storage::SparseMatrix<ValueType> const& transitions, std::string const& varNameSuffix, std::vector<storm::expressions::Expression> const& choiceVars, storm::solver::LpSolver<ValueType>& lpModel) {
                std::map<uint64_t, storm::expressions::Expression> ecStateVars, ecChoiceVars, ecFlowChoiceVars;
                
                // Compute an upper bound on the expected number of visits of the states in this ec.
                // First get a lower bound l on the probability of a path that leaves this MEC. 1-l is an upper bound on Pr_s(X F s).
                // The desired upper bound is thus 1/(1-(1-l)) = 1/l. See Baier et al., CAV'17
                
                // To compute l, we multiply the smallest transition probabilities occurring at each state and MEC-Choice
                // as well as the smallest 'exit' probability
                ValueType lpath = storm::utility::one<ValueType>();
                ValueType minExitProbability = storm::utility::one<ValueType>();
                for (auto const& stateChoices : ec) {
                    auto state = stateChoices.first;
                    ValueType minProb = storm::utility::one<ValueType>();
                    for (uint64_t choice = transitions.getRowGroupIndices()[state]; choice < transitions.getRowGroupIndices()[state + 1]; ++choice) {
                        if (stateChoices.second.count(choice) == 0) {
                            // The choice leaves the EC, so we take the sum over the exiting probabilities
                            ValueType exitProbabilitySum = storm::utility::zero<ValueType>();
                            for (auto const& transition : transitions.getRow(choice)) {
                                if (!ec.containsState(transition.getColumn())) {
                                    exitProbabilitySum += transition.getValue();
                                }
                            }
                            minExitProbability = std::min(minExitProbability, exitProbabilitySum);
                        } else {
                            // Get the minimum over all transition probabilities
                            for (auto const& transition : transitions.getRow(choice)) {
                                if (!storm::utility::isZero(transition.getValue())) {
                                    minProb = std::min(minProb, transition.getValue());
                                }
                            }
                            
                        }
                    }
                    lpath *= minProb;
                }
                lpath *= minExitProbability;
                ValueType expVisitsUpperBound = storm::utility::one<ValueType>() / lpath;
                STORM_LOG_WARN_COND(expVisitsUpperBound <= storm::utility::convertNumber<ValueType>(1000.0), "Large upper bound for expected visiting times: " << expVisitsUpperBound);
                // create variables
                for (auto const& stateChoices : ec) {
                    ecStateVars.emplace(stateChoices.first, lpModel.addBoundedIntegerVariable("e" + std::to_string(stateChoices.first) + varNameSuffix, storm::utility::zero<ValueType>(), storm::utility::one<ValueType>()).getExpression());
                    for (auto const& choice : stateChoices.second) {
                        ecChoiceVars.emplace(choice, lpModel.addBoundedIntegerVariable("ec" + std::to_string(choice) + varNameSuffix, storm::utility::zero<ValueType>(), storm::utility::one<ValueType>()).getExpression());
                        ecFlowChoiceVars.emplace(choice, lpModel.addBoundedContinuousVariable("f" + std::to_string(choice) + varNameSuffix, storm::utility::zero<ValueType>(), expVisitsUpperBound).getExpression());
                    }
                }
                
                // create constraints
                std::map<uint64_t, std::vector<storm::expressions::Expression>> ins, outs;
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
                        for (auto const& transition : transitions.getRow(choice)) {
                            if (!storm::utility::isZero(transition.getValue())) {
                                lpModel.addConstraint("", ecChoiceVars[choice] <= ecStateVars[transition.getColumn()]);
                                ins[transition.getColumn()].push_back(lpModel.getConstant(transition.getValue()) * ecFlowChoiceVars[choice]);
                            }
                        }
                    }
                    lpModel.addConstraint("", ecStateVars[stateChoices.first] == storm::expressions::sum(ecChoiceVarsAtState));
                    out.push_back(lpModel.getConstant(expVisitsUpperBound) * ecStateVars[stateChoices.first]);
                    // Iterate over choices that leave the ec
                    for (uint64_t choice = transitions.getRowGroupIndices()[stateChoices.first]; choice < transitions.getRowGroupIndices()[stateChoices.first + 1]; ++choice) {
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
                    lpModel.addConstraint("", storm::expressions::sum(in->second)<= storm::expressions::sum(out->second));
                }

                return ecStateVars;
        }
            
            template <typename ModelType, typename GeometryValueType>
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
                storm::storage::MaximalEndComponentDecomposition<ValueType> mecs(model.getTransitionMatrix(), backwardTransitions, storm::storage::BitVector(model.getNumberOfStates(), true), choicesWithValueZero);
                for (auto const& mec : mecs) {
                    // For each objective we might need to split this mec into several subECs, if the objective yields a non-zero scheduler-independent state value for some states of this ec.
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
                                storm::storage::MaximalEndComponentDecomposition<ValueType> subEcs(model.getTransitionMatrix(), backwardTransitions, subEcStates, subEcChoices);
                                for (auto const& subEc : subEcs) {
                                    auto varsForSubEc = processEc(subEc, model.getTransitionMatrix(), "o" + std::to_string(*exclStates.second.begin()), choiceVariables, *lpModel);
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
                bool hasEndComponents = ecCounter > 0 || storm::utility::graph::checkIfECWithChoiceExists(model.getTransitionMatrix(), backwardTransitions, storm::storage::BitVector(model.getNumberOfStates(), true), ~choicesWithValueZero);
                STORM_LOG_WARN_COND(!hasEndComponents, "Processed " << ecCounter << " End components.");
                return hasEndComponents;
            }
            
            template <typename ModelType, typename GeometryValueType>
            void DeterministicSchedsLpChecker<ModelType, GeometryValueType>::initializeLpModel(Environment const& env) {
                STORM_LOG_INFO("Initializing LP model with " << model.getNumberOfStates() << " states.");
                uint64_t numStates = model.getNumberOfStates();
                uint64_t initialState = *model.getInitialStates().begin();

                lpModel = storm::utility::solver::getLpSolver<ValueType>("model");
                gurobiLpModel = dynamic_cast<storm::solver::GurobiLpSolver<ValueType>*>(lpModel.get());
                
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
                        if (choiceVarReduction()) {
                            --numChoices;
                        }
                        for (uint64_t choice = 0; choice < numChoices; ++choice) {
                            localChoices.push_back(lpModel->addBoundedIntegerVariable("c" + std::to_string(state) + "_" + std::to_string(choice), 0, 1).getExpression());
                            choiceVariables.push_back(localChoices.back());
                        }
                            storm::expressions::Expression localChoicesSum = storm::expressions::sum(localChoices);
                        if (choiceVarReduction()) {
                            lpModel->addConstraint("", localChoicesSum <= one);
                            choiceVariables.push_back(one - localChoicesSum);
                        } else {
                            lpModel->addConstraint("", localChoicesSum == one);
                        }
                    }
                }
                // Create ec Variables for each state/objective
                std::vector<std::vector<storm::expressions::Expression>> ecVars(objectiveHelper.size(), std::vector<storm::expressions::Expression>(model.getNumberOfStates()));
                bool hasEndComponents = processEndComponents(ecVars);
                // ECs are not supported with choiceVarReduction.
                STORM_LOG_THROW(!hasEndComponents || !choiceVarReduction(), storm::exceptions::InvalidOperationException, "Choice var reduction is not supported with end components.");
                
                if (inOutEncoding()) {
                    storm::storage::BitVector bottomStates(model.getNumberOfStates(), true);
                    for (auto const& helper : objectiveHelper) {
                        STORM_LOG_THROW(helper.isTotalRewardObjective(), storm::exceptions::InvalidOperationException, "The given type of encoding is only supported if the objectives can be reduced to total reward objectives.");
                        storm::storage::BitVector objBottomStates(model.getNumberOfStates(), false);
                        for (auto const& stateVal : helper.getSchedulerIndependentStateValues()) {
                            STORM_LOG_THROW(storm::utility::isZero(stateVal.second), storm::exceptions::InvalidOperationException, "Non-zero constant state-values not allowed for this type of encoding.");
                            objBottomStates.set(stateVal.first, true);
                        }
                        bottomStates &= objBottomStates;
                    }
                    storm::storage::BitVector nonBottomStates = ~bottomStates;
                    STORM_LOG_TRACE("Found " << bottomStates.getNumberOfSetBits() << " bottom states.");
                    
                    // Compute upper bounds for each state
                    std::vector<ValueType> visitingTimesUpperBounds = DeterministicSchedsObjectiveHelper<ModelType>::computeUpperBoundOnExpectedVisitingTimes(model.getTransitionMatrix(), bottomStates, nonBottomStates, hasEndComponents);
                    ValueType largestUpperBound = *std::max_element(visitingTimesUpperBounds.begin(), visitingTimesUpperBounds.end());
                    STORM_LOG_WARN_COND(largestUpperBound < storm::utility::convertNumber<ValueType>(1e5), "Found a large upper bound '" << storm::utility::convertNumber<double>(largestUpperBound) << "' in the LP encoding. This might trigger numerical instabilities.");
                    // create choiceValue variables and assert deterministic ones.
                    std::vector<storm::expressions::Expression> choiceValVars(model.getNumberOfChoices());
                    for (auto const& state : nonBottomStates) {
                        for (uint64_t globalChoice = groups[state]; globalChoice < groups[state + 1]; ++globalChoice) {
                            choiceValVars[globalChoice] = lpModel->addBoundedContinuousVariable("y" + std::to_string(globalChoice), storm::utility::zero<ValueType>(), visitingTimesUpperBounds[state]).getExpression();
                            if (model.getNumberOfChoices(state) > 1) {;
                                lpModel->addConstraint("", choiceValVars[globalChoice] <= lpModel->getConstant(visitingTimesUpperBounds[state]) * choiceVariables[globalChoice]);
                            }
                        }
                    }
                    // create EC 'slack' variables for states that lie in an ec
                    std::vector<storm::expressions::Expression> ecValVars(model.getNumberOfStates());
                    if (hasEndComponents) {
                        for (auto const& state : nonBottomStates) {
                            // For the in-out-encoding, all objectives have the same ECs (because there are no non-zero scheduler independend state values).
                            // Hence, we only care for the variables of the first objective.
                            if (ecVars.front()[state].isInitialized()) {
                                ecValVars[state] = lpModel->addBoundedContinuousVariable("z" + std::to_string(state), storm::utility::zero<ValueType>(), visitingTimesUpperBounds[state]).getExpression();
                                lpModel->addConstraint("", ecValVars[state] <= lpModel->getConstant(visitingTimesUpperBounds[state]) * ecVars.front()[state]);
                            }
                        }
                    }
                    // Get 'in' and 'out' expressions
                    std::vector<storm::expressions::Expression> bottomStatesIn;
                    std::vector<std::vector<storm::expressions::Expression>> ins(numStates), outs(numStates);
                    ins[initialState].push_back(one);
                    for (auto const& state : nonBottomStates) {
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
                    for (auto const& state : nonBottomStates) {
                        lpModel->addConstraint("", storm::expressions::sum(ins[state]) == storm::expressions::sum(outs[state]));
                        
                    }
                    if (assertBottomStateSum()) {
                        lpModel->addConstraint("", storm::expressions::sum(bottomStatesIn) == one);
                    }
                    
                    // create initial state results for each objective
                    for (uint64_t objIndex = 0; objIndex < objectiveHelper.size(); ++objIndex) {
                        auto choiceValueOffsets = objectiveHelper[objIndex].getChoiceValueOffsets();
                        std::vector<storm::expressions::Expression> objValue;
                        for (auto const& state : nonBottomStates) {
                            for (uint64_t globalChoice = groups[state]; globalChoice < groups[state + 1]; ++globalChoice) {
                                auto choiceValueIt = choiceValueOffsets.find(globalChoice);
                                if (choiceValueIt != choiceValueOffsets.end()) {
                                    assert(!storm::utility::isZero(choiceValueIt->second));
                                    objValue.push_back(lpModel->getConstant(choiceValueIt->second) * choiceValVars[globalChoice]);
                                }
                            }
                        }
                        auto objValueVariable = lpModel->addBoundedContinuousVariable("x" + std::to_string(objIndex), objectiveHelper[objIndex].getLowerValueBoundAtState(env, initialState), objectiveHelper[objIndex].getUpperValueBoundAtState(env, initialState));
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
                        STORM_LOG_WARN_COND(objectiveHelper[objIndex].getLargestUpperBound(env) < storm::utility::convertNumber<ValueType>(1e5), "Found a large upper value bound '" << storm::utility::convertNumber<double>(objectiveHelper[objIndex].getLargestUpperBound(env)) << "' in the LP encoding. This might trigger numerical instabilities.");
                        auto const& schedulerIndependentStates = objectiveHelper[objIndex].getSchedulerIndependentStateValues();
                        // Create state variables and store variables of ecs which contain a state with a scheduler independent value
                        std::vector<storm::expressions::Expression> stateVars;
                        stateVars.reserve(numStates);
                        for (uint64_t state = 0; state < numStates; ++state) {
                            auto valIt = schedulerIndependentStates.find(state);
                            if (valIt == schedulerIndependentStates.end()) {
                                if (objectiveHelper[objIndex].minimizing()) {
                                    stateVars.push_back(lpModel->addBoundedContinuousVariable("x" + std::to_string(objIndex) + "_" + std::to_string(state), -objectiveHelper[objIndex].getUpperValueBoundAtState(env, state), -objectiveHelper[objIndex].getLowerValueBoundAtState(env, state)).getExpression());
                                } else {
                                    stateVars.push_back(lpModel->addBoundedContinuousVariable("x" + std::to_string(objIndex) + "_" + std::to_string(state), objectiveHelper[objIndex].getLowerValueBoundAtState(env, state), objectiveHelper[objIndex].getUpperValueBoundAtState(env, state)).getExpression());
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
                                    storm::expressions::Expression transitionValue = lpModel->getConstant(transition.getValue()) * stateVars[transition.getColumn()];
                                    if (choiceValue.isInitialized()) {
                                        choiceValue = choiceValue + transitionValue;
                                    } else {
                                        choiceValue = transitionValue;
                                    }
                                }
                                choiceValue = choiceValue.simplify().reduceNesting();
                                if (numChoices == 1) {
                                    stateValue = choiceValue;
                                } else {
                                    uint64_t globalChoiceIndex = groups[state] + choice;
                                    if (isMaxDiffEncoding()) {
                                        storm::expressions::Expression maxDiff = upperValueBoundAtState * (one - choiceVariables[globalChoiceIndex]);
                                        if (objectiveHelper[objIndex].minimizing()) {
                                            lpModel->addConstraint("", stateVars[state] >= choiceValue - maxDiff);
                                        } else {
                                            lpModel->addConstraint("", stateVars[state] <= choiceValue + maxDiff);
                                        }
                                    }
                                    
                                    storm::expressions::Expression choiceValVar;
                                    if (objectiveHelper[objIndex].minimizing()) {
                                        choiceValVar = lpModel->addBoundedContinuousVariable("x" + std::to_string(objIndex) + "_" + std::to_string(state) + "_" + std::to_string(choice), -objectiveHelper[objIndex].getUpperValueBoundAtState(env, state), storm::utility::zero<ValueType>()).getExpression();
                                    } else {
                                        choiceValVar = lpModel->addBoundedContinuousVariable("x" + std::to_string(objIndex) + "_" + std::to_string(state) + "_" + std::to_string(choice), storm::utility::zero<ValueType>(), objectiveHelper[objIndex].getUpperValueBoundAtState(env, state)).getExpression();
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
                                lpModel->addConstraint("", stateVars[state] <= stateValue + (lpModel->getConstant(storm::utility::convertNumber<ValueType>(numChoices - 1)) * upperValueBoundAtState));
                            } else {
                                lpModel->addConstraint("", stateVars[state] <= stateValue);
                            }
                            if (numChoices > 1 && hasEndComponents) {
                                auto& ecVar = ecVars[objIndex][state];
                                if (ecVar.isInitialized()) {
                                    // if this state is part of an ec, make sure to assign a value of zero.
                                    if (objectiveHelper[objIndex].minimizing()) {
                                        // TODO: this is optional
                                        lpModel->addConstraint("", stateVars[state] >= (ecVar - one) * lpModel->getConstant(objectiveHelper[objIndex].getUpperValueBoundAtState(env, state)));
                                    } else {
                                        lpModel->addConstraint("", stateVars[state] <= (one - ecVar) * lpModel->getConstant(objectiveHelper[objIndex].getUpperValueBoundAtState(env, state)));
                                    }
                                }
                            }
                        }
                    }
                }
                lpModel->update();
                STORM_LOG_INFO("Done initializing LP model.");
            }
            
            template <typename ModelType, typename GeometryValueType>
            void DeterministicSchedsLpChecker<ModelType, GeometryValueType>::checkRecursive(Environment const& env, storm::storage::geometry::PolytopeTree<GeometryValueType>& polytopeTree, Point const& eps, std::vector<Point>& foundPoints, std::vector<Polytope>& infeasableAreas, uint64_t const& depth) {
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
                    STORM_LOG_TRACE("\tSolving MILP...");
                    swCheckAreas.start();
                    lpModel->optimize();
                    swCheckAreas.stop();
                    ++numLpQueries;
                    STORM_LOG_TRACE("\tDone solving MILP...");

                    // STORM_PRINT_AND_LOG("Writing model to file '" << polytopeTree.toId() << ".lp'" << std::endl;);
                    // lpModel->writeModelToFile(polytopeTree.toId() + ".lp");
                    
                    if (lpModel->isInfeasible()) {
                        infeasableAreas.push_back(polytopeTree.getPolytope());
                        polytopeTree.clear();
                    } else {
                        STORM_LOG_ASSERT(!lpModel->isUnbounded(), "LP result is unbounded.");
                        swValidate.start();
                        Point newPoint = validateCurrentModel(env);
                        swValidate.stop();
                        std::vector<Point> newPoints = {newPoint};
                        if (gurobiLpModel && useNonOptimalSolutions()) {
                            // gurobi might have other good solutions.
                            for (uint64_t solutionIndex = 0; solutionIndex < gurobiLpModel->getSolutionCount(); ++ solutionIndex) {
                                Point p;
                                bool considerP = false;
                                for (uint64_t objIndex = 0; objIndex < currentObjectiveVariables.size(); ++objIndex) {
                                    p.push_back(storm::utility::convertNumber<GeometryValueType>(gurobiLpModel->getContinuousValue(currentObjectiveVariables[objIndex], solutionIndex)));
                                    if (p.back() > newPoint[objIndex] + eps[objIndex] / storm::utility::convertNumber<GeometryValueType>(2)) {
                                        // The other solution dominates the newPoint in this dimension and is also not too close to the newPoint.
                                        considerP = true;
                                    }
                                }
                                if (considerP) {
                                    newPoints.push_back(std::move(p));
                                }
                            }
                        }
                        GeometryValueType offset = storm::utility::convertNumber<GeometryValueType>(lpModel->getObjectiveValue());
                        if (gurobiLpModel) {
                            // Gurobi gives us the gap between the found solution and the known bound.
                            offset += storm::utility::convertNumber<GeometryValueType>(gurobiLpModel->getMILPGap(false));
                        }
                        // we might want to shift the halfspace to guarantee that our point is included.
                        for (auto const& p : newPoints) {
                            offset = std::max(offset, storm::utility::vector::dotProduct(currentWeightVector, p));
                        }
                        auto halfspace = storm::storage::geometry::Halfspace<GeometryValueType>(currentWeightVector, offset).invert();
                        infeasableAreas.push_back(polytopeTree.getPolytope()->intersection(halfspace));
                        if (infeasableAreas.back()->isEmpty()) {
                            infeasableAreas.pop_back();
                        }
                        polytopeTree.setMinus(storm::storage::geometry::Polytope<GeometryValueType>::create({halfspace}));
                        for (auto const& p : newPoints) {
                            foundPoints.push_back(p);
                            polytopeTree.substractDownwardClosure(p, eps);
                        }
                        if (!polytopeTree.isEmpty()) {
                            checkRecursive(env, polytopeTree, eps, foundPoints, infeasableAreas, depth);
                        }
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
            
            template <typename ModelType, typename GeometryValueType>
            typename DeterministicSchedsLpChecker<ModelType, GeometryValueType>::Point DeterministicSchedsLpChecker<ModelType, GeometryValueType>::validateCurrentModel(Environment const& env) const {
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
                            if (choiceVarReduction() && localChoice + 1 == numChoices) {
                                localChoiceEnabled = !choiceFound;
                            } else {
                                localChoiceEnabled = (lpModel->getIntegerValue(choiceVariables[globalChoiceOffset + localChoice].getBaseExpression().asVariableExpression().getVariable()) == 1);
                            }
                            if (localChoiceEnabled) {
                                STORM_LOG_THROW(!choiceFound, storm::exceptions::UnexpectedException, "Multiple choices selected at state " << state);
                                scheduler.setChoice(localChoice, state);
                                choiceFound = true;
                            }
                        }
                        STORM_LOG_THROW(choiceFound, storm::exceptions::UnexpectedException, "No choice selected at state " << state);
                    }
                }
                auto inducedModel = model.applyScheduler(scheduler)->template as<ModelType>();
                Point inducedPoint;
                for (uint64_t objIndex = 0; objIndex < objectiveHelper.size(); ++objIndex) {
                    ValueType inducedValue = objectiveHelper[objIndex].evaluateOnModel(env, *inducedModel);
                    if (objectiveHelper[objIndex].minimizing()) {
                        inducedValue = -inducedValue;
                    }
                    inducedPoint.push_back(inducedValue);
                    ValueType lpValue = lpModel->getContinuousValue(currentObjectiveVariables[objIndex]);
                    double diff = storm::utility::convertNumber<double>(storm::utility::abs<ValueType>(inducedValue - lpValue));
                    STORM_LOG_WARN_COND(diff <= 1e-4 * std::abs(storm::utility::convertNumber<double>(inducedValue)), "Imprecise value for objective " << objIndex << ": LP says " << lpValue << " but scheduler induces " << inducedValue << " (difference is " << diff << ")");
                }
                
                return inducedPoint;
            }
            
            template class DeterministicSchedsLpChecker<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
            template class DeterministicSchedsLpChecker<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber>;
            template class DeterministicSchedsLpChecker<storm::models::sparse::MarkovAutomaton<double>, storm::RationalNumber>;
            template class DeterministicSchedsLpChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>, storm::RationalNumber>;
        }
    }
}