#include "JaniGSPNBuilder.h"

#include <memory>

#include "storm/logic/Formulas.h"

#include "storm/exceptions/InvalidModelException.h"
namespace storm {
    namespace builder {

        storm::jani::Model* JaniGSPNBuilder::build(std::string const& automatonName, bool buildStandardProperties) {
            storm::jani::ModelType modelType = storm::jani::ModelType::MA;
            if (gspn.getNumberOfTimedTransitions() == 0) {
                modelType = storm::jani::ModelType::MDP;
            } else if (gspn.getNumberOfImmediateTransitions() == 0) {
                modelType = storm::jani::ModelType::CTMC;
            }
            storm::jani::Model* model = new storm::jani::Model(gspn.getName(), modelType, janiVersion, expressionManager);
            storm::jani::Automaton mainAutomaton(automatonName, expressionManager->declareIntegerVariable("loc"));
            addVariables(model);
            uint64_t locId = addLocation(mainAutomaton);
            addEdges(mainAutomaton, locId);
            model->addAutomaton(mainAutomaton);
            model->setStandardSystemComposition();
            if (buildStandardProperties) {
                buildProperties(model);
            }
            model->getModelFeatures().add(storm::jani::ModelFeature::DerivedOperators);
            model->finalize();
            return model;
        }
        
        std::vector<storm::jani::Property> const& JaniGSPNBuilder::getStandardProperties() const {
            return standardProperties;
        }

        void JaniGSPNBuilder::addVariables(storm::jani::Model* model) {
            for (auto const& place : gspn.getPlaces()) {
                storm::jani::Variable* janiVar = nullptr;
                if (!place.hasRestrictedCapacity()) {
                    // Effectively no capacity limit known
                    janiVar = new storm::jani::UnboundedIntegerVariable(place.getName(), expressionManager->getVariable(place.getName()), expressionManager->integer(place.getNumberOfInitialTokens()));
                } else {
                    assert(place.hasRestrictedCapacity());
                    janiVar = new storm::jani::BoundedIntegerVariable(place.getName(), expressionManager->getVariable(place.getName()), expressionManager->integer(place.getNumberOfInitialTokens()), expressionManager->integer(0), expressionManager->integer(place.getCapacity()));
                }
                assert(janiVar != nullptr);
                assert(vars.count(place.getID()) == 0);
                vars[place.getID()] = &model->addVariable(*janiVar);
                delete janiVar;
            }
        }

        uint64_t JaniGSPNBuilder::addLocation(storm::jani::Automaton& automaton) {
            uint64_t janiLoc = automaton.addLocation(storm::jani::Location("loc"));
            automaton.addInitialLocation("loc");
            return janiLoc;
        }

        void JaniGSPNBuilder::addEdges(storm::jani::Automaton& automaton, uint64_t locId) {

            uint64_t lastPriority = -1;
            storm::expressions::Expression lastPriorityGuard = expressionManager->boolean(false);
            storm::expressions::Expression priorityGuard = expressionManager->boolean(true);

            for (auto const& partition : gspn.getPartitions()) {
                storm::expressions::Expression guard = expressionManager->boolean(false);

                assert(lastPriority >= partition.priority);
                if (lastPriority > partition.priority) {
                    priorityGuard = priorityGuard && !lastPriorityGuard;
                    lastPriority = partition.priority;
                } else {
                    assert(lastPriority == partition.priority);
                }

                // Compute enabled weight expression.
                storm::expressions::Expression totalWeight = expressionManager->rational(0.0);
                for (auto const& transId : partition.transitions) {
                    auto const& trans = gspn.getImmediateTransitions()[transId];
                    if (trans.noWeightAttached()) {
                        continue;
                    }
                    storm::expressions::Expression destguard = expressionManager->boolean(true);
                    for (auto const& inPlaceEntry : trans.getInputPlaces()) {
                        destguard = destguard && (vars[inPlaceEntry.first]->getExpressionVariable() >= inPlaceEntry.second);
                    }
                    for (auto const& inhibPlaceEntry : trans.getInhibitionPlaces()) {
                        destguard = destguard && (vars[inhibPlaceEntry.first]->getExpressionVariable() < inhibPlaceEntry.second);
                    }
                    totalWeight = totalWeight + storm::expressions::ite(destguard, expressionManager->rational(trans.getWeight()), expressionManager->rational(0.0));

                }
                totalWeight = totalWeight.simplify();


                std::vector<storm::jani::OrderedAssignments> oas;
                std::vector<storm::expressions::Expression> probabilities;
                std::vector<uint64_t> destinationLocations;
                for (auto const& transId : partition.transitions) {
                    auto const& trans = gspn.getImmediateTransitions()[transId];
                    if (trans.noWeightAttached()) {
                        std::cout << "ERROR -- no weights attached at transition" << std::endl;
                        continue;
                    }
                    storm::expressions::Expression destguard = expressionManager->boolean(true);
                    std::vector<storm::jani::Assignment> assignments;
                    for (auto const& inPlaceEntry : trans.getInputPlaces()) {
                        destguard = destguard && (vars[inPlaceEntry.first]->getExpressionVariable() >= inPlaceEntry.second);
                        if (trans.getOutputPlaces().count(inPlaceEntry.first) == 0) {
                            assignments.emplace_back(storm::jani::LValue(*vars[inPlaceEntry.first]), (vars[inPlaceEntry.first])->getExpressionVariable() - inPlaceEntry.second);
                        }
                    }
                    for (auto const& inhibPlaceEntry : trans.getInhibitionPlaces()) {
                        destguard = destguard && (vars[inhibPlaceEntry.first]->getExpressionVariable() < inhibPlaceEntry.second);
                    }
                    for (auto const& outputPlaceEntry : trans.getOutputPlaces()) {
                        if (trans.getInputPlaces().count(outputPlaceEntry.first) == 0) {
                            assignments.emplace_back(storm::jani::LValue(*vars[outputPlaceEntry.first]), (vars[outputPlaceEntry.first])->getExpressionVariable() + outputPlaceEntry.second );
                        } else {
                            assignments.emplace_back(storm::jani::LValue(*vars[outputPlaceEntry.first]), (vars[outputPlaceEntry.first])->getExpressionVariable() + outputPlaceEntry.second -  trans.getInputPlaces().at(outputPlaceEntry.first));
                        }
                    }
                    destguard = destguard.simplify();
                    guard = guard || destguard;

                    oas.emplace_back(assignments);
                    destinationLocations.emplace_back(locId);
                    probabilities.emplace_back(storm::expressions::ite(destguard, (expressionManager->rational(trans.getWeight()) / totalWeight), expressionManager->rational(0.0)));
                }

                std::shared_ptr<storm::jani::TemplateEdge> templateEdge = std::make_shared<storm::jani::TemplateEdge>((priorityGuard && guard).simplify());
                automaton.registerTemplateEdge(templateEdge);

                for (auto const& oa : oas) {
                    templateEdge->addDestination(storm::jani::TemplateEdgeDestination(oa));
                }
                storm::jani::Edge e(locId, storm::jani::Model::SILENT_ACTION_INDEX, boost::none, templateEdge, destinationLocations, probabilities);
                automaton.addEdge(e);
                lastPriorityGuard = lastPriorityGuard || guard;

            }
            for (auto const& trans : gspn.getTimedTransitions()) {
                storm::expressions::Expression guard = expressionManager->boolean(true);
                
                std::vector<storm::jani::Assignment> assignments;
                for (auto const& inPlaceEntry : trans.getInputPlaces()) {
                    guard = guard && (vars[inPlaceEntry.first]->getExpressionVariable() >= inPlaceEntry.second);
                    if (trans.getOutputPlaces().count(inPlaceEntry.first) == 0) {
                        assignments.emplace_back(storm::jani::LValue(*vars[inPlaceEntry.first]), (vars[inPlaceEntry.first])->getExpressionVariable() - inPlaceEntry.second);
                    }
                }
                for (auto const& inhibPlaceEntry : trans.getInhibitionPlaces()) {
                    guard = guard && (vars[inhibPlaceEntry.first]->getExpressionVariable() < inhibPlaceEntry.second);
                }
                for (auto const& outputPlaceEntry : trans.getOutputPlaces()) {
                    if (trans.getInputPlaces().count(outputPlaceEntry.first) == 0) {
                        assignments.emplace_back(storm::jani::LValue(*vars[outputPlaceEntry.first]), (vars[outputPlaceEntry.first])->getExpressionVariable() + outputPlaceEntry.second );
                    } else {
                        assignments.emplace_back(storm::jani::LValue(*vars[outputPlaceEntry.first]), (vars[outputPlaceEntry.first])->getExpressionVariable() + outputPlaceEntry.second -  trans.getInputPlaces().at(outputPlaceEntry.first));
                    }
                }

                std::shared_ptr<storm::jani::TemplateEdge> templateEdge = std::make_shared<storm::jani::TemplateEdge>(guard);
                automaton.registerTemplateEdge(templateEdge);
                
                storm::expressions::Expression rate = expressionManager->rational(trans.getRate());
                if (trans.hasInfiniteServerSemantics() || (trans.hasKServerSemantics() && !trans.hasSingleServerSemantics())) {
                    STORM_LOG_THROW(trans.hasKServerSemantics() || !trans.getInputPlaces().empty(), storm::exceptions::InvalidModelException, "Unclear semantics: Found a transition with infinite-server semantics and without input place.");
                    storm::expressions::Expression enablingDegree;
                    bool firstArgumentOfMinExpression = true;
                    if (trans.hasKServerSemantics()) {
                        enablingDegree = expressionManager->integer(trans.getNumberOfServers());
                        firstArgumentOfMinExpression = false;
                    }
                    for (auto const& inPlaceEntry : trans.getInputPlaces()) {
                        storm::expressions::Expression enablingDegreeInPlace = vars[inPlaceEntry.first]->getExpressionVariable() / expressionManager->integer(inPlaceEntry.second); // Integer division!
                        if (firstArgumentOfMinExpression == true) {
                            enablingDegree = enablingDegreeInPlace;
                        } else {
                            enablingDegree = storm::expressions::minimum(enablingDegree, enablingDegreeInPlace);
                        }
                    }
                    rate = rate * enablingDegree;
                }

                templateEdge->addDestination(assignments);
                storm::jani::Edge e(locId, storm::jani::Model::SILENT_ACTION_INDEX, rate, templateEdge, {locId}, {expressionManager->integer(1)});
                automaton.addEdge(e);

            }
        }
        
        storm::jani::Variable const& JaniGSPNBuilder::addDeadlockTransientVariable(storm::jani::Model* model, std::string name, bool ignoreCapacities, bool ignoreInhibitorArcs, bool ignoreEmptyPlaces) {
            
            storm::expressions::Expression transientValue = expressionManager->boolean(true);
            
            // build the conjunction over all transitions
            std::vector<storm::gspn::Transition const*> transitions;
            transitions.reserve(gspn.getNumberOfImmediateTransitions() + gspn.getNumberOfTimedTransitions());
            for (auto const& t : gspn.getImmediateTransitions()) {
                transitions.push_back(&t);
            }
            for (auto const& t : gspn.getTimedTransitions()) {
                transitions.push_back(&t);
            }
            bool firstTransition = true;
            for (auto const& transition : transitions) {
                
                // build the disjunction over all in/out places and inhibitor arcs
                storm::expressions::Expression transitionDisabled = expressionManager->boolean(false);
                bool firstPlace = true;
                if (!ignoreEmptyPlaces) {
                    for (auto const& placeIdMult : transition->getInputPlaces()) {
                        storm::expressions::Expression placeBlocksTransition = (vars.at(placeIdMult.first)->getExpressionVariable() < expressionManager->integer(placeIdMult.second));
                        if (firstPlace) {
                            transitionDisabled = placeBlocksTransition;
                            firstPlace = false;
                        } else {
                            transitionDisabled = transitionDisabled || placeBlocksTransition;
                        }
                    }
                }
                if (!ignoreInhibitorArcs) {
                    for (auto const& placeIdMult : transition->getInhibitionPlaces()) {
                        storm::expressions::Expression placeBlocksTransition = (vars.at(placeIdMult.first)->getExpressionVariable() >= expressionManager->integer(placeIdMult.second));
                        if (firstPlace) {
                            transitionDisabled = placeBlocksTransition;
                            firstPlace = false;
                        } else {
                            transitionDisabled = transitionDisabled || placeBlocksTransition;
                        }
                    }
                }
                if (!ignoreCapacities) {
                    for (auto const& placeIdMult : transition->getOutputPlaces()) {
                        auto const& place = gspn.getPlace(placeIdMult.first);
                        if (place->hasRestrictedCapacity()) {
                            storm::expressions::Expression placeBlocksTransition = (vars.at(placeIdMult.first)->getExpressionVariable() + expressionManager->integer(placeIdMult.second) > expressionManager->integer(place->getCapacity()));
                            if (firstPlace) {
                                transitionDisabled = placeBlocksTransition;
                                firstPlace = false;
                            } else {
                                transitionDisabled = transitionDisabled || placeBlocksTransition;
                            }
                        }
                    }
                }
                
                if (firstTransition) {
                    transientValue = transitionDisabled;
                    firstTransition = false;
                } else {
                    transientValue = transientValue && transitionDisabled;
                }
            }
            
            auto exprVar = expressionManager->declareBooleanVariable(name);
            auto const& janiVar = model->addVariable(*storm::jani::makeBooleanVariable(name, exprVar, expressionManager->boolean(false), true));
            storm::jani::Assignment assignment(storm::jani::LValue(janiVar), transientValue);
            model->getAutomata().front().getLocations().front().addTransientAssignment(assignment);
            return janiVar;
        }
        
        
        std::string getUniqueVarName(storm::expressions::ExpressionManager const& manager, std::string name) {
            std::string res = name;
            while (manager.hasVariable(res)) {
                res.append("_");
            }
            return res;
        }
        
        void JaniGSPNBuilder::buildProperties(storm::jani::Model* model) {
            standardProperties.clear();
            
            auto const& deadlockVar = addDeadlockTransientVariable(model, getUniqueVarName(*expressionManager, "deadl"));
            auto deadlock = std::make_shared<storm::logic::AtomicExpressionFormula>(deadlockVar.getExpressionVariable().getExpression());
            auto trueFormula = std::make_shared<storm::logic::BooleanLiteralFormula>(true);
            std::set<storm::expressions::Variable> emptyVariableSet;
            
            auto maxReachDeadlock = std::make_shared<storm::logic::ProbabilityOperatorFormula>(
                    std::make_shared<storm::logic::EventuallyFormula>(deadlock, storm::logic::FormulaContext::Probability),
                    storm::logic::OperatorInformation(storm::solver::OptimizationDirection::Maximize));
            standardProperties.emplace_back("MaxPrReachDeadlock", maxReachDeadlock, emptyVariableSet, "The maximal probability to eventually reach a deadlock.");
            
            auto exprTB = expressionManager->declareRationalVariable(getUniqueVarName(*expressionManager, "TIME_BOUND"));
            auto janiTB = storm::jani::Constant(exprTB.getName(), exprTB);
            model->addConstant(janiTB);
            storm::logic::TimeBound tb(false, janiTB.getExpressionVariable().getExpression());
            storm::logic::TimeBoundReference tbr(storm::logic::TimeBoundType::Time);
            auto maxReachDeadlockTimeBounded = std::make_shared<storm::logic::ProbabilityOperatorFormula>(
                    std::make_shared<storm::logic::BoundedUntilFormula>(trueFormula, deadlock, boost::none, tb, tbr),
                    storm::logic::OperatorInformation(storm::solver::OptimizationDirection::Maximize));
            standardProperties.emplace_back("MaxPrReachDeadlockTB", maxReachDeadlockTimeBounded, emptyVariableSet, "The maximal probability to reach a deadlock within 'TIME_BOUND' steps.");
            
            auto expTimeDeadlock = std::make_shared<storm::logic::TimeOperatorFormula>(
                    std::make_shared<storm::logic::EventuallyFormula>(deadlock, storm::logic::FormulaContext::Time),
                    storm::logic::OperatorInformation(storm::solver::OptimizationDirection::Maximize));
            standardProperties.emplace_back("MinExpTimeDeadlock", expTimeDeadlock, emptyVariableSet, "The minimal expected time to reach a deadlock.");
            
        }

        
    }
}
