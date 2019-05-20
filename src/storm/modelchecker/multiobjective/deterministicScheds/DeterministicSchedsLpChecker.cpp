#include "storm/modelchecker/multiobjective/deterministicScheds/DeterministicSchedsLpChecker.h"

#include "storm/modelchecker/prctl/helper/BaierUpperRewardBoundsComputer.h"

#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/MultiObjectiveSettings.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/utility/graph.h"
#include "storm/utility/solver.h"

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
            
            bool isMinNegativeEncoding() {
                return encodingSettings().get(63);
            }
            
            bool isMaxDiffEncoding() {
                bool result = encodingSettings().get(62);
                STORM_LOG_ERROR_COND(!result || !isMinNegativeEncoding(), "maxDiffEncoding only works without minnegative encoding.");
                return result;
            }
            
            bool choiceVarReduction() {
                return encodingSettings().get(61);
            }
            
            bool inOutEncoding() {
                return encodingSettings().get(60);
            }
            
            bool assertBottomStateSum() {
                bool result = encodingSettings().get(59);
                STORM_LOG_ERROR_COND(!result || inOutEncoding(), "Asserting bottom state sum is only relevant for in-out encoding.");
                return result;
            }
            
            bool useNonOptimalSolutions() {
                bool result = encodingSettings().get(58);
                STORM_LOG_ERROR_COND(!result || inOutEncoding(), "Asserting bottom state sum is only relevant for in-out encoding.");
                return result;
            }
            
            template <typename ModelType, typename GeometryValueType>
            DeterministicSchedsLpChecker<ModelType, GeometryValueType>::DeterministicSchedsLpChecker(Environment const& env, ModelType const& model, std::vector<DeterministicSchedsObjectiveHelper<ModelType>> const& objectiveHelper) : model(model) , objectiveHelper(objectiveHelper) {
                swInit.start();
                initializeLpModel(env);
                swInit.stop();
            }
            
            template <typename ModelType, typename GeometryValueType>
            DeterministicSchedsLpChecker<ModelType, GeometryValueType>::~DeterministicSchedsLpChecker() {
                std::cout << "Deterministic Scheds LP CHECKER STATISTICS: " << std::endl;
                std::cout << "\t" << swInit << " seconds for initialization" << std::endl;
                std::cout << "\t" << swCheck << " seconds for checking, including" << std::endl;
                std::cout << "\t\t" << swLpBuild << " seconds for LP building" << std::endl;
                std::cout << "\t\t" << swLpSolve << " seconds for LP solving, including" << std::endl;
                std::cout << "\t\t\t" << swCheckVertices << " seconds for finding the vertices of the convex hull." << std::endl;
                std::cout << "\t" << swAux << " seconds for aux stuff" << std::endl;
            }
            
            template <typename ModelType, typename GeometryValueType>
            void DeterministicSchedsLpChecker<ModelType, GeometryValueType>::setCurrentWeightVector(std::vector<GeometryValueType> const& weightVector) {
                STORM_LOG_ASSERT(weightVector.size() == objectiveHelper.size(), "Setting a weight vector with invalid number of entries.");
                swLpBuild.start();
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
                    if (objectiveHelper[objIndex].minimizing() && !isMinNegativeEncoding()) {
                        lpModel->addConstraint("", currentObjectiveVariables.back().getExpression() == -initialStateResults[objIndex]);
                    } else {
                        lpModel->addConstraint("", currentObjectiveVariables.back().getExpression() == initialStateResults[objIndex]);
                    }
                }
                lpModel->update();
                swLpBuild.stop();
            }
            
            template <typename ModelType, typename GeometryValueType>
            boost::optional<std::vector<GeometryValueType>> DeterministicSchedsLpChecker<ModelType, GeometryValueType>::check(storm::Environment const& env, Polytope area) {
                STORM_LOG_ASSERT(!currentWeightVector.empty(), "Checking invoked before specifying a weight vector.");
                STORM_LOG_TRACE("Checking a vertex...");
                swCheck.start();
                swLpBuild.start();
                lpModel->push();
                auto areaConstraints = area->getConstraints(lpModel->getManager(), currentObjectiveVariables);
                for (auto const& c : areaConstraints) {
                    lpModel->addConstraint("", c);
                }
                lpModel->update();
                swLpBuild.stop();
                swLpSolve.start(); swCheckVertices.start();
                lpModel->optimize();
                swCheckVertices.stop(); swLpSolve.stop();
                STORM_LOG_TRACE("\t Done checking a vertex...");
                boost::optional<Point> result;
                if (!lpModel->isInfeasible()) {
                    STORM_LOG_ASSERT(!lpModel->isUnbounded(), "LP result is unbounded.");
                    result = Point();
                    for (auto const& objVar : currentObjectiveVariables) {
                        result->push_back(storm::utility::convertNumber<GeometryValueType>(lpModel->getContinuousValue(objVar)));
                    }
                }
                lpModel->pop();
                swCheck.stop();
                return result;
            }

            template <typename ModelType, typename GeometryValueType>
            std::pair<std::vector<std::vector<GeometryValueType>>, std::vector<std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>>>> DeterministicSchedsLpChecker<ModelType, GeometryValueType>::check(storm::Environment const& env, storm::storage::geometry::PolytopeTree<GeometryValueType>& polytopeTree, GeometryValueType const& eps) {
                std::cout << "Checking " << polytopeTree.toString() << std::endl << "\t";
                swCheck.start();
                STORM_LOG_ASSERT(!currentWeightVector.empty(), "Checking invoked before specifying a weight vector.");
                if (polytopeTree.isEmpty()) {
                    return {{}, {}};
                }
                
                if (gurobiLpModel) {
                    // For gurobi, it is possible to specify a gap between the obtained lower/upper objective bounds.
                    GeometryValueType milpGap = storm::utility::zero<GeometryValueType>();
                    for (auto const& wi : currentWeightVector) {
                        milpGap += wi;
                    }
                    milpGap *= eps;
                    gurobiLpModel->setMaximalMILPGap(storm::utility::convertNumber<ValueType>(milpGap), false);
                    gurobiLpModel->update();
                }
                
                std::vector<Point> foundPoints;
                std::vector<Polytope> infeasableAreas;
                checkRecursive(polytopeTree, eps, foundPoints, infeasableAreas, 0);
                
                swCheck.stop();
                std::cout << " done!" << std::endl;
                return {foundPoints, infeasableAreas};
            }
            
            
            template <typename ValueType>
            std::map<storm::storage::BitVector, storm::storage::BitVector> getSubEndComponents(storm::storage::SparseMatrix<ValueType> const& mecTransitions) {
                auto backwardTransitions = mecTransitions.transpose(true);
                std::map<storm::storage::BitVector, storm::storage::BitVector> unprocessed, processed;
                storm::storage::BitVector allStates(mecTransitions.getRowGroupCount());
                storm::storage::BitVector allChoices(mecTransitions.getRowCount());
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
            
            template <typename ModelType, typename GeometryValueType>
            std::vector<std::vector<storm::expressions::Variable>> DeterministicSchedsLpChecker<ModelType, GeometryValueType>::createEcVariables(std::vector<storm::expressions::Expression> const& choiceVars) {
                auto one = lpModel->getConstant(storm::utility::one<ValueType>());
                std::vector<std::vector<storm::expressions::Variable>> result(model.getNumberOfStates());
                storm::storage::MaximalEndComponentDecomposition<ValueType> mecs(model);
                uint64_t ecCounter = 0;
                
                for (auto const& mec : mecs) {
                    // Create a submatrix for the current mec as well as a mapping to map back to the original states.
                    std::vector<uint64_t> toGlobalStateIndexMapping;
                    std::vector<uint64_t> toGlobalChoiceIndexMapping;
                    storm::storage::BitVector mecStatesAsBitVector(model.getNumberOfStates(), false);
                    storm::storage::BitVector mecChoicesAsBitVector(model.getNumberOfChoices(), false);
                    for (auto const& stateChoices : mec) {
                        mecStatesAsBitVector.set(stateChoices.first, true);
                        toGlobalStateIndexMapping.push_back(stateChoices.first);
                        for (auto const& choice : stateChoices.second) {
                            mecChoicesAsBitVector.set(choice, true);
                            toGlobalChoiceIndexMapping.push_back(choice);
                        }
                    }
                    storm::storage::SparseMatrix<ValueType> mecTransitions = model.getTransitionMatrix().getSubmatrix(false, mecChoicesAsBitVector, mecStatesAsBitVector);
                    
                    // Create a variable for each subEC and add it for the corresponding states.
                    // Also assert that not every state takes an ec choice.
                    auto const& subEcs = getSubEndComponents(mecTransitions);
                    for (auto const& subEc : subEcs) {
                        // get the choices of the current EC with some non-zero value (i.e. reward).
                        storm::storage::BitVector subEcChoicesWithValueZero = subEc.second;
                        for (auto const& localSubEcChoiceIndex : subEc.second) {
                            uint64_t subEcChoice = toGlobalChoiceIndexMapping[localSubEcChoiceIndex];
                            for (auto const& objHelper : objectiveHelper) {
                                if (objHelper.getChoiceValueOffsets().count(subEcChoice) > 0) {
                                    STORM_LOG_ASSERT(!storm::utility::isZero(objHelper.getChoiceValueOffsets().at(subEcChoice)), "Expected non-zero choice-value offset.");
                                    subEcChoicesWithValueZero.set(localSubEcChoiceIndex, false);
                                    break;
                                }
                            }
                        }
                        
                        // Check whether each state has at least one zero-valued choice
                        bool zeroValueSubEc = true;
                        for (auto const& state : subEc.first) {
                            if (subEcChoicesWithValueZero.getNextSetIndex(mecTransitions.getRowGroupIndices()[state]) >= mecTransitions.getRowGroupIndices()[state + 1]) {
                                zeroValueSubEc = false;
                                break;
                            }
                        }
                        
                        if (zeroValueSubEc) {
                            // Create a variable that is one iff upon entering this subEC no more choice value is collected.
                            auto ecVar = lpModel->addBoundedIntegerVariable("ec" + std::to_string(ecCounter++), storm::utility::zero<ValueType>(), storm::utility::one<ValueType>());
                            // assign this variable to every state in the ec
                            for (auto const& localSubEcStateIndex : subEc.first) {
                                uint64_t subEcState = toGlobalStateIndexMapping[localSubEcStateIndex];
                                result[subEcState].push_back(ecVar);
                            }
                            // Create the sum over all choice vars that induce zero choice value
                            std::vector<storm::expressions::Expression> ecChoiceVars;
                            uint64_t numSubEcStatesWithMultipleChoices = subEc.first.getNumberOfSetBits();
                            for (auto const& localSubEcChoiceIndex : subEcChoicesWithValueZero) {
                                uint64_t subEcChoice = toGlobalChoiceIndexMapping[localSubEcChoiceIndex];
                                if (choiceVars[subEcChoice].isInitialized()) {
                                    ecChoiceVars.push_back(choiceVars[subEcChoice]);
                                } else {
                                    // If there is no choiceVariable, it means that this corresponds to a state with just one choice.
                                    assert(numSubEcStatesWithMultipleChoices > 0);
                                    --numSubEcStatesWithMultipleChoices;
                                }
                            }
                            // Assert that the ecVar is one iff the sum over the zero-value-choice variables equals the number of states in this ec
                            storm::expressions::Expression ecVarLowerBound = one - lpModel->getConstant(storm::utility::convertNumber<ValueType>(numSubEcStatesWithMultipleChoices)).simplify();
                            if (!ecChoiceVars.empty()) {
                                ecVarLowerBound = ecVarLowerBound + storm::expressions::sum(ecChoiceVars);
                            }
                            lpModel->addConstraint("", ecVar >= ecVarLowerBound);
                        }
                    }
                }
                
                STORM_LOG_TRACE("Found " << ecCounter << " end components.");
                return result;
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
                std::vector<storm::expressions::Expression> choiceVars;
                choiceVars.reserve(model.getNumberOfChoices());
                for (uint64_t state = 0; state < numStates; ++state) {
                    uint64_t numChoices = model.getNumberOfChoices(state);
                    if (numChoices == 1) {
                        choiceVars.emplace_back();
                    } else {
                        std::vector<storm::expressions::Expression> localChoices;
                        if (choiceVarReduction()) {
                            --numChoices;
                        }
                        for (uint64_t choice = 0; choice < numChoices; ++choice) {
                            localChoices.push_back(lpModel->addBoundedIntegerVariable("c" + std::to_string(state) + "_" + std::to_string(choice), 0, 1).getExpression());
                            choiceVars.push_back(localChoices.back());
                        }
                            storm::expressions::Expression localChoicesSum = storm::expressions::sum(localChoices);
                        if (choiceVarReduction()) {
                            lpModel->addConstraint("", localChoicesSum <= one);
                            choiceVars.push_back(one - localChoicesSum);
                        } else {
                            lpModel->addConstraint("", localChoicesSum == one);
                        }
                    }
                }
                
                // Create ec Variables and assert for each sub-ec that not all choice variables stay there
                auto ecVars = createEcVariables(choiceVars);
                bool hasEndComponents = false;
                for (auto const& stateEcVars : ecVars) {
                    if (!stateEcVars.empty()) {
                        hasEndComponents = true;
                        break;
                    }
                }
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
                    STORM_LOG_ERROR_COND(storm::utility::graph::performProb1A(model.getTransitionMatrix(), model.getNondeterministicChoiceIndices(), model.getBackwardTransitions(), nonBottomStates, bottomStates).full(), "End components not yet treated correctly.");
                    
                    // Compute upper bounds for each state
                    std::vector<ValueType> visitingTimesUpperBounds = DeterministicSchedsObjectiveHelper<ModelType>::computeUpperBoundOnExpectedVisitingTimes(model.getTransitionMatrix(), bottomStates, nonBottomStates, hasEndComponents);
                    
                    // create choiceValue variables and assert deterministic ones.
                    std::vector<storm::expressions::Expression> choiceValVars(model.getNumberOfChoices());
                    for (auto const& state : nonBottomStates) {
                        for (uint64_t globalChoice = groups[state]; globalChoice < groups[state + 1]; ++globalChoice) {
                            choiceValVars[globalChoice] = lpModel->addBoundedContinuousVariable("y" + std::to_string(globalChoice), storm::utility::zero<ValueType>(), visitingTimesUpperBounds[state]).getExpression();
                            if (model.getNumberOfChoices(state) > 1) {;
                                lpModel->addConstraint("", choiceValVars[globalChoice] <= lpModel->getConstant(visitingTimesUpperBounds[state]) * choiceVars[globalChoice]);
                            }
                        }
                    }
                    // create EC 'slack' variables for states that lie in an ec
                    std::vector<storm::expressions::Expression> ecValVars(model.getNumberOfStates());
                    if (hasEndComponents) {
                        for (auto const& state : nonBottomStates) {
                            if (!ecVars[state].empty()) {
                                ecValVars[state] = lpModel->addBoundedContinuousVariable("z" + std::to_string(state), storm::utility::zero<ValueType>(), visitingTimesUpperBounds[state]).getExpression();
                                std::vector<storm::expressions::Expression> ecValueSum;
                                for (auto const& ecVar : ecVars[state]) {
                                    ecValueSum.push_back(lpModel->getConstant(visitingTimesUpperBounds[state]) * ecVar.getExpression());
                                }
                                lpModel->addConstraint("", ecValVars[state] <= storm::expressions::sum(ecValueSum));
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
                        lpModel->addConstraint("", objValueVariable == storm::expressions::sum(objValue));
                        initialStateResults.push_back(objValueVariable);
                    }
                    
                } else {
                    // 'classic' backward encoding.
                    for (uint64_t objIndex = 0; objIndex < objectiveHelper.size(); ++objIndex) {
                        auto const& schedulerIndependentStates = objectiveHelper[objIndex].getSchedulerIndependentStateValues();
                        // Create state variables and store variables of ecs which contain a state with a scheduler independent value
                        std::vector<storm::expressions::Expression> stateVars;
                        std::set<storm::expressions::Variable> ecVarsWithValue;
                        stateVars.reserve(numStates);
                        for (uint64_t state = 0; state < numStates; ++state) {
                            auto valIt = schedulerIndependentStates.find(state);
                            if (valIt == schedulerIndependentStates.end()) {
                                if (objectiveHelper[objIndex].minimizing() && isMinNegativeEncoding()) {
                                    stateVars.push_back(lpModel->addBoundedContinuousVariable("x" + std::to_string(objIndex) + "_" + std::to_string(state), -objectiveHelper[objIndex].getUpperValueBoundAtState(env, state), -objectiveHelper[objIndex].getLowerValueBoundAtState(env, state)).getExpression());
                                } else {
                                    stateVars.push_back(lpModel->addBoundedContinuousVariable("x" + std::to_string(objIndex) + "_" + std::to_string(state), objectiveHelper[objIndex].getLowerValueBoundAtState(env, state), objectiveHelper[objIndex].getUpperValueBoundAtState(env, state)).getExpression());
                                }
                            } else {
                                ValueType value = valIt->second;
                                if (objectiveHelper[objIndex].minimizing() && isMinNegativeEncoding()) {
                                    value = -value;
                                }
                                stateVars.push_back(lpModel->getConstant(value));
                                if (hasEndComponents) {
                                    for (auto const& ecVar : ecVars[state]) {
                                        ecVarsWithValue.insert(ecVar);
                                    }
                                }
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
                                    if (objectiveHelper[objIndex].minimizing() && isMinNegativeEncoding()) {
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
                                        storm::expressions::Expression maxDiff = upperValueBoundAtState * (one - choiceVars[globalChoiceIndex]);
                                        if (objectiveHelper[objIndex].minimizing()) {
                                            lpModel->addConstraint("", stateVars[state] >= choiceValue - maxDiff);
                                        } else {
                                            lpModel->addConstraint("", stateVars[state] <= choiceValue + maxDiff);
                                        }
                                    }
                                    
                                    storm::expressions::Expression choiceValVar;
                                    if (objectiveHelper[objIndex].minimizing() && isMinNegativeEncoding()) {
                                        choiceValVar = lpModel->addBoundedContinuousVariable("x" + std::to_string(objIndex) + "_" + std::to_string(state) + "_" + std::to_string(choice), -objectiveHelper[objIndex].getUpperValueBoundAtState(env, state), storm::utility::zero<ValueType>()).getExpression();
                                    } else {
                                        choiceValVar = lpModel->addBoundedContinuousVariable("x" + std::to_string(objIndex) + "_" + std::to_string(state) + "_" + std::to_string(choice), storm::utility::zero<ValueType>(), objectiveHelper[objIndex].getUpperValueBoundAtState(env, state)).getExpression();
                                    }
                                    if (objectiveHelper[objIndex].minimizing()) {
                                        if (isMinNegativeEncoding()) {
                                            lpModel->addConstraint("", choiceValVar <= choiceValue);
                                            lpModel->addConstraint("", choiceValVar <= -upperValueBoundAtState * (one - choiceVars[globalChoiceIndex]));
                                        } else {
                                            lpModel->addConstraint("", choiceValVar + (upperValueBoundAtState * (one - choiceVars[globalChoiceIndex])) >= choiceValue);
                                            // Optional: lpModel->addConstraint("", choiceValVar <= choiceValue);
                                        }
                                    } else {
                                        lpModel->addConstraint("", choiceValVar <= choiceValue);
                                        lpModel->addConstraint("", choiceValVar <= upperValueBoundAtState * choiceVars[globalChoiceIndex]);
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
                                if (isMinNegativeEncoding()) {
                                    lpModel->addConstraint("", stateVars[state] <= stateValue + (lpModel->getConstant(storm::utility::convertNumber<ValueType>(numChoices - 1)) * upperValueBoundAtState));
                                } else {
                                    lpModel->addConstraint("", stateVars[state] >= stateValue);
                                }
                            } else {
                                lpModel->addConstraint("", stateVars[state] <= stateValue);
                            }
                            if (numChoices > 1) {
                                for (auto const& ecVar : ecVars[state]) {
                                    if (ecVarsWithValue.count(ecVar) == 0) {
                                        // if this ec is taken, make sure to assign a value of zero
                                        if (objectiveHelper[objIndex].minimizing()) {
                                            // TODO: these are optional
                                            if (isMinNegativeEncoding()) {
                                                lpModel->addConstraint("", stateVars[state] >= (ecVar.getExpression() - one) * lpModel->getConstant(objectiveHelper[objIndex].getUpperValueBoundAtState(env, state)));
                                            } else {
                                                lpModel->addConstraint("", stateVars[state] <= (one - ecVar.getExpression()) * lpModel->getConstant(objectiveHelper[objIndex].getUpperValueBoundAtState(env, state)));
                                            }
                                        } else {
                                            lpModel->addConstraint("", stateVars[state] <= (one - ecVar.getExpression()) * lpModel->getConstant(objectiveHelper[objIndex].getUpperValueBoundAtState(env, state)));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                swAux.start();
                lpModel->update();
                swAux.stop();
                STORM_LOG_INFO("Done initializing LP model.");
            }
            
            template <typename ModelType, typename GeometryValueType>
            void DeterministicSchedsLpChecker<ModelType, GeometryValueType>::checkRecursive(storm::storage::geometry::PolytopeTree <GeometryValueType>& polytopeTree, GeometryValueType const& eps, std::vector<Point>& foundPoints, std::vector<Polytope>& infeasableAreas, uint64_t const& depth) {
                std::cout << ".";
                std::cout.flush();
                STORM_LOG_ASSERT(!polytopeTree.isEmpty(), "Tree node is empty");
                STORM_LOG_ASSERT(!polytopeTree.getPolytope()->isEmpty(), "Tree node is empty.");
                STORM_LOG_TRACE("Checking at depth " << depth << ": " << polytopeTree.toString());
                
                swLpBuild.start();
                lpModel->push();
                // Assert the constraints of the current polytope
                auto nodeConstraints = polytopeTree.getPolytope()->getConstraints(lpModel->getManager(), currentObjectiveVariables);
                for (auto const& constr : nodeConstraints) {
                    lpModel->addConstraint("", constr);
                }
                lpModel->update();
                swLpBuild.stop();
                
                if (polytopeTree.getChildren().empty()) {
                    // At leaf nodes we need to perform the actual check.
                    swLpSolve.start();
                    STORM_LOG_TRACE("\tSolving MILP...");
                    lpModel->optimize();
                    STORM_LOG_TRACE("\tDone solving MILP...");
                    swLpSolve.stop();
                    
                    if (lpModel->isInfeasible()) {
                        infeasableAreas.push_back(polytopeTree.getPolytope());
                        lpModel->writeModelToFile("out.lp");
                        polytopeTree.clear();
                    } else {
                        STORM_LOG_ASSERT(!lpModel->isUnbounded(), "LP result is unbounded.");
                        Point newPoint;
                        for (auto const& objVar : currentObjectiveVariables) {
                            newPoint.push_back(storm::utility::convertNumber<GeometryValueType>(lpModel->getContinuousValue(objVar)));
                        }
                        std::vector<Point> newPoints = {newPoint};
                        if (gurobiLpModel && useNonOptimalSolutions()) {
                            // gurobi might have other good solutions.
                            for (uint64_t solutionIndex = 0; solutionIndex < gurobiLpModel->getSolutionCount(); ++ solutionIndex) {
                                Point p;
                                bool considerP = false;
                                for (uint64_t objIndex = 0; objIndex < currentObjectiveVariables.size(); ++objIndex) {
                                    p.push_back(storm::utility::convertNumber<GeometryValueType>(gurobiLpModel->getContinuousValue(currentObjectiveVariables[objIndex], solutionIndex)));
                                    if (p.back() > newPoint[objIndex] + eps / storm::utility::convertNumber<GeometryValueType>(2)) {
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
                        auto halfspace = storm::storage::geometry::Halfspace<GeometryValueType>(currentWeightVector, offset).invert();
                        infeasableAreas.push_back(polytopeTree.getPolytope()->intersection(halfspace));
                        if (infeasableAreas.back()->isEmpty()) {
                            infeasableAreas.pop_back();
                        }
                        swAux.start();
                        polytopeTree.setMinus(storm::storage::geometry::Polytope<GeometryValueType>::create({halfspace}));
                        for (auto const& p : newPoints) {
                            foundPoints.push_back(p);
                            polytopeTree.substractDownwardClosure(p, eps);
                        }
                        swAux.stop();
                        if (!polytopeTree.isEmpty()) {
                            checkRecursive(polytopeTree, eps, foundPoints, infeasableAreas, depth);
                        }
                    }
                } else {
                    // Traverse all the children.
                    for (uint64_t childId = 0; childId < polytopeTree.getChildren().size(); ++childId) {
                        uint64_t newPointIndex = foundPoints.size();
                        checkRecursive(polytopeTree.getChildren()[childId], eps, foundPoints, infeasableAreas, depth + 1);
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

                swLpBuild.start();
                lpModel->pop();
                lpModel->update();
                swLpBuild.stop();
            }
            
            template class DeterministicSchedsLpChecker<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
            template class DeterministicSchedsLpChecker<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber>;
            template class DeterministicSchedsLpChecker<storm::models::sparse::MarkovAutomaton<double>, storm::RationalNumber>;
            template class DeterministicSchedsLpChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>, storm::RationalNumber>;
        }
    }
}