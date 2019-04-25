#pragma once

#include <vector>

#include "storm/storage/geometry/Polytope.h"
#include "storm/modelchecker/multiobjective/deterministicScheds/MultiObjectiveSchedulerEvaluator.h"
#include "storm/storage/expressions/Expressions.h"
#include "storm/utility/solver.h"
#include "storm/solver/LpSolver.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/csl/SparseCtmcCslModelChecker.h"
#include "storm/storage/BitVector.h"
#include "storm/utility/graph.h"
#include "storm/utility/vector.h"
#include "storm/utility/Stopwatch.h"

namespace storm {
    
    class Environment;
    
    namespace modelchecker {
        namespace multiobjective {
            
            /*!
             * Represents a set of points in euclidean space.
             * The set is defined as the union of the polytopes at the leafs of the tree.
             * The polytope at inner nodes is always the convex union of its children.
             * The sets described by the children of a node are disjoint.
             * A child is always non-empty, i.e., isEmpty() should only hold for the root node.
             * @tparam GeometryValueType
             */
            template <typename GeometryValueType>
            class PolytopeTree {
                typedef typename std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> Polytope;
                typedef typename std::vector<GeometryValueType> Point;

            public:
                PolytopeTree(Polytope const& polytope) : polytope(polytope) {}
                
                /*!
                 * Substracts the given rhs from this polytope.
                 */
                void setMinus(Polytope const& rhs) {
                    // This operation only has an effect if the intersection of this and rhs is non-empty.
                    if (!isEmpty() && !polytope->intersection(rhs)->isEmpty()) {
                        if (children.empty()) {
                            // This is a leaf node.
                            // Apply splitting.
                            auto newChildren = polytope->setMinus(rhs);
                            if (newChildren.empty()) {
                                // Delete this node.
                                polytope = nullptr;
                            } else if (newChildren.size() == 1) {
                                // Replace this node with its only child
                                polytope = newChildren.front()->clean();
                            } else {
                                // Add the new children to this node. There is no need to traverse them.
                                for (auto& c : newChildren) {
                                    children.push_back(c->clean());
                                }
                            }
                        } else {
                            // This is an inner node. Traverse the children and set this to the convex union of its children.
                            std::vector<PolytopeTree<GeometryValueType>> newChildren;
                            Polytope newPolytope = nullptr;
                            for (auto& c : children) {
                                c.setMinus(rhs);
                                if (c.polytope != nullptr) {
                                    newChildren.push_back(c);
                                    if (newPolytope) {
                                        newPolytope->convexUnion(c.polytope);
                                    } else {
                                        newPolytope = c.polytope;
                                    }
                                }
                            }
                            polytope = newPolytope; // nullptr, if no children left
                            children = std::move(newChildren);
                        }
                    }
                }
                
                void substractDownwardClosure(Point const& point, GeometryValueType const& eps) {
                    std::vector<GeometryValueType>(pointPlusEps);
                    for (auto const& coordinate : point) {
                        pointPlusEps.push_back(coordinate + eps);
                    }
                    auto downwardOfPoint = storm::storage::geometry::Polytope<GeometryValueType>::createDownwardClosure({pointPlusEps});
                    setMinus(downwardOfPoint);
                }

                bool isEmpty() const {
                    return polytope == nullptr;
                }
                
                void clear() {
                    children.clear();
                    polytope = nullptr;
                }
                
                Polytope getPolytope() const {
                    return polytope;
                }
                
                std::vector<PolytopeTree>& getChildren() {
                    return children;
                }
                
                std::string toString() {
                    std::stringstream s;
                    s << "PolytopeTree node with " << getChildren().size() << " children: " << getPolytope()->toString(true) << std::endl << "Vertices: ";
                    auto vertices = getPolytope()->getVertices();
                    for (auto const& v : vertices) {
                        s << "[";
                        for (auto const& vi : v) {
                            s << storm::utility::convertNumber<double>(vi) << ",";
                        }
                        s << "]\t";
                    }
                    s << std::endl;
                    return s.str();
                }
                
            private:
                
                
                
                Polytope polytope;
                std::vector<PolytopeTree<GeometryValueType>> children;
            };
            
            template <typename ModelType, typename GeometryValueType>
            class DetSchedsSimplexChecker {
            public:
                
                typedef typename ModelType::ValueType ValueType;
                typedef typename std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> Polytope;
                typedef typename std::vector<GeometryValueType> Point;
                
                DetSchedsSimplexChecker(std::shared_ptr<MultiObjectiveSchedulerEvaluator<ModelType>> const& schedulerEvaluator) : schedulerEvaluator(schedulerEvaluator) {
                    init();
                }
                
                ~DetSchedsSimplexChecker() {
                    std::cout << "SIMPLEX CHECKER: " << swInit << " seconds for initialization" << std::endl;
                    std::cout << "SIMPLEX CHECKER: " << swCheck << " seconds for checking, including" << std::endl;
                    std::cout << "\t " << swLpBuild << " seconds for LP building" << std::endl;
                    std::cout << "\t " << swLpSolve << " seconds for LP solving" << std::endl;
                    std::cout << "SIMPLEX CHECKER: " << swAux << " seconds for aux stuff" << std::endl;
                }
                
                std::pair<std::vector<Point>, std::vector<Polytope>> check(storm::Environment const& env, std::vector<GeometryValueType> const& weightVector, PolytopeTree<GeometryValueType>& polytopeTree, GeometryValueType const& eps) {
                    std::cout << "Checking a Simplex with weight vector " << storm::utility::vector::toString(weightVector) << std::endl << " and root " << polytopeTree.toString() << std::endl << "\t";
                    if (polytopeTree.isEmpty()) {
                        return {{}, {}};
                    }
                    swCheck.start();
                    
                    swLpBuild.start();
                    lpModel->push();
                    currentObjectiveVariables.clear();
                    
                    // set up objective function for the given weight vector
                    for (uint64_t objIndex = 0; objIndex < initialStateResults.size(); ++objIndex) {
                        currentObjectiveVariables.push_back(lpModel->addUnboundedContinuousVariable("w_" + std::to_string(objIndex), storm::utility::convertNumber<ValueType>(weightVector[objIndex])));
                        lpModel->addConstraint("", currentObjectiveVariables.back().getExpression() == initialStateResults[objIndex]);
                    }
                    lpModel->update();
                    swLpBuild.stop();

                    auto result = checkRecursive(weightVector, polytopeTree, eps);
                    
                    swLpBuild.start();
                    lpModel->pop();
                    lpModel->update();
                    swLpBuild.stop();
                    swCheck.stop();
                    std::cout << " done!" << std::endl;
                    return result;
                }
                
            private:
                
                std::pair<std::vector<Point>, std::vector<Polytope>> checkRecursive(std::vector<GeometryValueType> const& weightVector, PolytopeTree<GeometryValueType>& polytopeTree, GeometryValueType const& eps) {
                    std::cout << ".";
                    std::cout.flush();
                    STORM_LOG_ASSERT(!polytopeTree.isEmpty(), "Tree node is empty");
                    STORM_LOG_ASSERT(!polytopeTree.getPolytope()->isEmpty(), "Tree node is empty.");
                    STORM_LOG_TRACE("Checking " << polytopeTree.toString());
                    
                    auto vertices = polytopeTree.getPolytope()->getVertices();
                    
                    std::vector<Point> foundPoints;
                    std::vector<Polytope> infeasableAreas;
                    
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
                        lpModel->optimize();
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
                            auto halfspace = storm::storage::geometry::Halfspace<GeometryValueType>(weightVector, storm::utility::vector::dotProduct(weightVector, newPoint)).invert();
                            infeasableAreas.push_back(polytopeTree.getPolytope()->intersection(halfspace));
                            if (infeasableAreas.back()->isEmpty()) {
                                infeasableAreas.pop_back();
                            }
                            swAux.start();
                            polytopeTree.setMinus(storm::storage::geometry::Polytope<GeometryValueType>::create({halfspace}));
                            foundPoints.push_back(newPoint);
                            polytopeTree.substractDownwardClosure(newPoint, eps);
                            swAux.stop();
                            if (!polytopeTree.isEmpty()) {
                                auto childRes = checkRecursive(weightVector, polytopeTree, eps);
                                foundPoints.insert(foundPoints.end(), childRes.first.begin(), childRes.first.end());
                                infeasableAreas.insert(infeasableAreas.end(), childRes.second.begin(), childRes.second.end());
                            }
                        }
                    } else {
                        // Traverse all the children.
                        for (uint64_t childId = 0; childId < polytopeTree.getChildren().size(); ++childId) {
                            auto childRes = checkRecursive(weightVector, polytopeTree.getChildren()[childId], eps);
                            STORM_LOG_ASSERT(polytopeTree.getChildren()[childId].isEmpty(), "expected empty children.");
                            // Make the results known to the right siblings
                            for (auto const& newPoint : childRes.first) {
                                for (uint64_t siblingId = childId + 1; siblingId < polytopeTree.getChildren().size(); ++siblingId) {
                                    polytopeTree.getChildren()[siblingId].substractDownwardClosure(newPoint, eps);
                                }
                            }
                            foundPoints.insert(foundPoints.end(), childRes.first.begin(), childRes.first.end());
                            infeasableAreas.insert(infeasableAreas.end(), childRes.second.begin(), childRes.second.end());
                        }
                        // All children are empty now, so this becomes empty.
                        polytopeTree.clear();
                    }
                    swLpBuild.start();
                    lpModel->pop();
                    swLpBuild.stop();
                    return {foundPoints, infeasableAreas};
                }

                /* Todo
                ValueType getChoiceValueSummand(Objective<ValueType> const& objective, uint64_t choiceIndex) {
                    auto const& model = schedulerEvaluator->getModel();
                    storm::modelchecker::SparsePropositionalModelChecker<ModelType> mc(model);
                    auto const& formula = *objective.formula;
                    if (formula.isProbabilityOperatorFormula() && formula.getSubformula().isUntilFormula()) {
                        return storm::utility::zero<ValueType>();
                    } else if (formula.getSubformula().isEventuallyFormula() && (formula.isRewardOperatorFormula() || formula.isTimeOperatorFormula())) {
                        
                        storm::storage::BitVector rew0States = mc.check(formula.getSubformula().asEventuallyFormula().getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                        if (formula.isRewardOperatorFormula()) {
                            auto const& rewModel = formula.asRewardOperatorFormula().hasRewardModelName() ? model.getRewardModel(formula.asRewardOperatorFormula().getRewardModelName()) : model.getUniqueRewardModel();
                            storm::storage::BitVector statesWithoutReward = rewModel.getStatesWithZeroReward(model.getTransitionMatrix());
                            rew0States = storm::utility::graph::performProb1A(model.getTransitionMatrix(), model.getNondeterministicChoiceIndices(), model.getBackwardTransitions(), statesWithoutReward, rew0States);
                        }
                        storm::utility::vector::setVectorValues(results[objIndex], rew0States, storm::utility::zero<ValueType>());
                        schedulerIndependentStates.push_back(std::move(rew0States));
                    } else if (formula.isRewardOperatorFormula() && formula.getSubformula().isTotalRewardFormula()) {
                        auto const& rewModel = formula.asRewardOperatorFormula().hasRewardModelName() ? model.getRewardModel(formula.asRewardOperatorFormula().getRewardModelName()) : model.getUniqueRewardModel();
                        storm::storage::BitVector statesWithoutReward = rewModel.getStatesWithZeroReward(model.getTransitionMatrix());
                        storm::storage::BitVector rew0States = storm::utility::graph::performProbGreater0E(model.getBackwardTransitions(), statesWithoutReward, ~statesWithoutReward);
                        rew0States.complement();
                        storm::utility::vector::setVectorValues(results[objIndex], rew0States, storm::utility::zero<ValueType>());
                        schedulerIndependentStates.push_back(std::move(rew0States));
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The given formula " << formula << " is not supported.");
                    }
                }*/
                
                void init() {
                    swInit.start();
                    auto const& model = schedulerEvaluator->getModel();
                    auto const& objectives = schedulerEvaluator->getObjectives();
                    uint64_t numStates = model.getNumberOfStates();
                    lpModel = storm::utility::solver::getLpSolver<ValueType>("model");
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
                            lpModel->addConstraint("", storm::expressions::sum(localChoices).reduceNesting() >= one);
                            choiceVars.insert(choiceVars.end(), localChoices.begin(), localChoices.end());
                        }
                    }
                    
                    for (uint64_t objIndex = 0; objIndex < objectives.size(); ++objIndex) {
                        Objective<ValueType> const& objective = objectives[objIndex];
                        storm::storage::BitVector const& schedulerIndependentStates = schedulerEvaluator->getSchedulerIndependentStates(objIndex);
                        // Create state variables
                        std::vector<storm::expressions::Expression> stateVars;
                        stateVars.reserve(numStates);
                        for (uint64_t state = 0; state < numStates; ++state) {
                            if (schedulerIndependentStates.get(state)) {
                                stateVars.push_back(lpModel->getConstant(schedulerEvaluator->getSchedulerIndependentStateResult(objIndex, state)));
                            } else {
                                stateVars.push_back(lpModel->addContinuousVariable("x" + std::to_string(objIndex) + "_" + std::to_string(state), objective.lowerResultBound, objective.upperResultBound).getExpression());
                            }
                            if (state == *model.getInitialStates().begin()) {
                                initialStateResults.push_back(stateVars.back());
                            }
                        }
                        
                        // Create and assert choice values
                        for (uint64_t state = 0; state < numStates; ++state) {
                            if (schedulerIndependentStates.get(state)) {
                                continue;
                            }
                            storm::expressions::Expression stateValue;
                            uint64_t numChoices = model.getNumberOfChoices(state);
                            for (uint64_t choice = 0; choice < numChoices; ++choice) {
                                storm::expressions::Expression choiceValue;
                                if (objective.formula)
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
                                    lpModel->addConstraint("", stateVars[state] == choiceValue);
                                } else {
                                    uint64_t globalChoiceIndex = model.getTransitionMatrix().getRowGroupIndices()[state] + choice;
                                    storm::expressions::Expression maxDiff = lpModel->getConstant(objective.upperResultBound.get()) * (one - choiceVars[globalChoiceIndex]);
                                    lpModel->addConstraint("", stateVars[state] - choiceValue <= maxDiff);
                                    lpModel->addConstraint("", choiceValue - stateVars[state] <= maxDiff);
                                }
                            }
                        }
                    }
                    lpModel->update();
                    swInit.stop();
                }
                
                std::shared_ptr<MultiObjectiveSchedulerEvaluator<ModelType>> schedulerEvaluator;

                std::unique_ptr<storm::solver::LpSolver<ValueType>> lpModel;
                std::vector<storm::expressions::Expression> initialStateResults;
                std::vector<storm::expressions::Variable> currentObjectiveVariables;

                
                storm::utility::Stopwatch swInit;
                storm::utility::Stopwatch swCheck;
                storm::utility::Stopwatch swLpSolve;
                storm::utility::Stopwatch swLpBuild;
                storm::utility::Stopwatch swAux;
            };
            
        }
    }
}