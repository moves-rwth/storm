#include "storm/modelchecker/multiobjective/deterministicScheds/DeterministicSchedsLpChecker.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/solver.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
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
                    if (objectiveHelper[objIndex].minimizing()) {
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
                checkRecursive(polytopeTree, eps, foundPoints, infeasableAreas);
                
                swCheck.stop();
                std::cout << " done!" << std::endl;
                return {foundPoints, infeasableAreas};
            }
            
            template <typename ModelType, typename GeometryValueType>
            void DeterministicSchedsLpChecker<ModelType, GeometryValueType>::initializeLpModel(Environment const& env) {
                uint64_t numStates = model.getNumberOfStates();
                lpModel = storm::utility::solver::getLpSolver<ValueType>("model");
                gurobiLpModel = dynamic_cast<storm::solver::GurobiLpSolver<ValueType>*>(lpModel.get());
                
                lpModel->setOptimizationDirection(storm::solver::OptimizationDirection::Maximize);
                initialStateResults.clear();
                
                auto one = lpModel->getConstant(storm::utility::one<ValueType>());
                
                // Create choice variables and assert that at least one choice is taken at each state.
                std::vector<storm::expressions::Expression> choiceVars;
                choiceVars.reserve(model.getNumberOfChoices());
                for (uint64_t state = 0; state < numStates; ++state) {
                    uint64_t numChoices = model.getNumberOfChoices(state);
                    if (numChoices == 1) {
                        choiceVars.emplace_back();
                    } else {
                        std::vector<storm::expressions::Expression> localChoices;
                        for (uint64_t choice = 0; choice < numChoices; ++choice) {
                            localChoices.push_back(lpModel->addBoundedIntegerVariable("c" + std::to_string(state) + "_" + std::to_string(choice), 0, 1).getExpression());
                        }
                        lpModel->addConstraint("", storm::expressions::sum(localChoices).reduceNesting() == one);
                        choiceVars.insert(choiceVars.end(), localChoices.begin(), localChoices.end());
                    }
                }
                
                for (uint64_t objIndex = 0; objIndex < objectiveHelper.size(); ++objIndex) {
                    auto const& schedulerIndependentStates = objectiveHelper[objIndex].getSchedulerIndependentStateValues();
                    // Create state variables
                    std::vector<storm::expressions::Expression> stateVars;
                    stateVars.reserve(numStates);
                    for (uint64_t state = 0; state < numStates; ++state) {
                        auto valIt = schedulerIndependentStates.find(state);
                        if (valIt == schedulerIndependentStates.end()) {
                            stateVars.push_back(lpModel->addBoundedContinuousVariable("x" + std::to_string(objIndex) + "_" + std::to_string(state), objectiveHelper[objIndex].getLowerValueBoundAtState(env, state), objectiveHelper[objIndex].getUpperValueBoundAtState(env, state)).getExpression());
                        } else {
                            stateVars.push_back(lpModel->getConstant(valIt->second));
                        }
                        if (state == *model.getInitialStates().begin()) {
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
                        uint64_t choiceOffset = model.getTransitionMatrix().getRowGroupIndices()[state];
                        for (uint64_t choice = 0; choice < numChoices; ++choice) {
                            storm::expressions::Expression choiceValue;
                            auto valIt = choiceValueOffsets.find(choiceOffset + choice);
                            if (valIt != choiceValueOffsets.end()) {
                                choiceValue = lpModel->getConstant(valIt->second);
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
                                auto choiceValVar = lpModel->addBoundedContinuousVariable("x" + std::to_string(objIndex) + "_" + std::to_string(state) + "_" + std::to_string(choice), storm::utility::zero<ValueType>(), objectiveHelper[objIndex].getUpperValueBoundAtState(env, state)).getExpression();
                                uint64_t globalChoiceIndex = model.getTransitionMatrix().getRowGroupIndices()[state] + choice;
                                storm::expressions::Expression upperValueBoundAtChoice = lpModel->getConstant(objectiveHelper[objIndex].getUpperValueBoundAtState(env, state));
                                if (objectiveHelper[objIndex].minimizing()) {
                                    lpModel->addConstraint("", choiceValVar + (upperValueBoundAtChoice * (one - choiceVars[globalChoiceIndex])) >= choiceValue);
                                    // Optional: lpModel->addConstraint("", choiceValVar <= choiceValue);
                                } else {
                                    lpModel->addConstraint("", choiceValVar <= choiceValue);
                                    lpModel->addConstraint("", choiceValVar <= upperValueBoundAtChoice * choiceVars[globalChoiceIndex]);
                                }
                                if (choice == 0) {
                                    stateValue = choiceValVar;
                                } else {
                                    stateValue = stateValue + choiceValVar;
                                }
                                // Alternative encoding: (Then the sum of the choiceVars also has to be >= one (instead of == one)
                                //storm::expressions::Expression maxDiff = lpModel->getConstant(objectiveHelper[objIndex].getUpperValueBoundAtState(env, state) - objectiveHelper[objIndex].getLowerValueBoundAtState(env, state)) * (one - choiceVars[globalChoiceIndex]);
                                //lpModel->addConstraint("", stateVars[state] - choiceValVar <= maxDiff);
                                //lpModel->addConstraint("", choiceValVar - stateVars[state] <= maxDiff);
                            }
                        }
                        if (objectiveHelper[objIndex].minimizing()) {
                            lpModel->addConstraint("", stateVars[state] >= stateValue);
                        } else {
                            lpModel->addConstraint("", stateVars[state] <= stateValue);
                        }
                    }
                }
                swAux.start();
                lpModel->update();
                swAux.stop();
            }
            
            template <typename ModelType, typename GeometryValueType>
            void DeterministicSchedsLpChecker<ModelType, GeometryValueType>::checkRecursive(storm::storage::geometry::PolytopeTree <GeometryValueType>& polytopeTree, GeometryValueType const& eps, std::vector<Point>& foundPoints, std::vector<Polytope>& infeasableAreas) {
                std::cout << ".";
                std::cout.flush();
                STORM_LOG_ASSERT(!polytopeTree.isEmpty(), "Tree node is empty");
                STORM_LOG_ASSERT(!polytopeTree.getPolytope()->isEmpty(), "Tree node is empty.");
                STORM_LOG_TRACE("Checking " << polytopeTree.toString());
                
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
                        polytopeTree.clear();
                    } else {
                        STORM_LOG_ASSERT(!lpModel->isUnbounded(), "LP result is unbounded.");
                        Point newPoint;
                        for (auto const& objVar : currentObjectiveVariables) {
                            newPoint.push_back(storm::utility::convertNumber<GeometryValueType>(lpModel->getContinuousValue(objVar)));
                        }
                        std::vector<Point> newPoints = {newPoint};
                        if (gurobiLpModel) {
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
                            std::cout << "found " << (newPoints.size() - 1) << " useful points" << std::endl;
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
                            checkRecursive(polytopeTree, eps, foundPoints, infeasableAreas);
                        }
                    }
                } else {
                    // Traverse all the children.
                    for (uint64_t childId = 0; childId < polytopeTree.getChildren().size(); ++childId) {
                        uint64_t newPointIndex = foundPoints.size();
                        checkRecursive(polytopeTree.getChildren()[childId], eps, foundPoints, infeasableAreas);
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
                swLpBuild.start();
                lpModel->pop();
                swLpBuild.stop();
            }
            
            template class DeterministicSchedsLpChecker<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
            template class DeterministicSchedsLpChecker<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber>;
            template class DeterministicSchedsLpChecker<storm::models::sparse::MarkovAutomaton<double>, storm::RationalNumber>;
            template class DeterministicSchedsLpChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>, storm::RationalNumber>;
        }
    }
}