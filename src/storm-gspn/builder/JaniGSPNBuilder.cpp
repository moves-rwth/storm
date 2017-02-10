#include "JaniGSPNBuilder.h"

namespace storm {
    namespace builder {

        storm::jani::Model* JaniGSPNBuilder::build(std::string const& automatonName) {
            storm::jani::Model* model = new storm::jani::Model(gspn.getName(), storm::jani::ModelType::MA, janiVersion, expressionManager);
            storm::jani::Automaton mainAutomaton(automatonName, expressionManager->declareIntegerVariable("loc"));
            addVariables(model);
            uint64_t locId = addLocation(mainAutomaton);
            addEdges(mainAutomaton, locId);
            model->addAutomaton(mainAutomaton);
            model->setStandardSystemComposition();
            return model;
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
                            assignments.emplace_back( *vars[inPlaceEntry.first], (vars[inPlaceEntry.first])->getExpressionVariable() - inPlaceEntry.second);
                        }
                    }
                    for (auto const& inhibPlaceEntry : trans.getInhibitionPlaces()) {
                        destguard = destguard && (vars[inhibPlaceEntry.first]->getExpressionVariable() < inhibPlaceEntry.second);
                    }
                    for (auto const& outputPlaceEntry : trans.getOutputPlaces()) {
                        if (trans.getInputPlaces().count(outputPlaceEntry.first) == 0) {
                            assignments.emplace_back( *vars[outputPlaceEntry.first], (vars[outputPlaceEntry.first])->getExpressionVariable() + outputPlaceEntry.second );
                        } else {
                            assignments.emplace_back( *vars[outputPlaceEntry.first], (vars[outputPlaceEntry.first])->getExpressionVariable() + outputPlaceEntry.second -  trans.getInputPlaces().at(outputPlaceEntry.first));
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
                        assignments.emplace_back( *vars[inPlaceEntry.first], (vars[inPlaceEntry.first])->getExpressionVariable() - inPlaceEntry.second);
                    }
                }
                for (auto const& inhibPlaceEntry : trans.getInhibitionPlaces()) {
                    guard = guard && (vars[inhibPlaceEntry.first]->getExpressionVariable() < inhibPlaceEntry.second);
                }
                for (auto const& outputPlaceEntry : trans.getOutputPlaces()) {
                    if (trans.getInputPlaces().count(outputPlaceEntry.first) == 0) {
                        assignments.emplace_back( *vars[outputPlaceEntry.first], (vars[outputPlaceEntry.first])->getExpressionVariable() + outputPlaceEntry.second );
                    } else {
                        assignments.emplace_back( *vars[outputPlaceEntry.first], (vars[outputPlaceEntry.first])->getExpressionVariable() + outputPlaceEntry.second -  trans.getInputPlaces().at(outputPlaceEntry.first));
                    }
                }

                std::shared_ptr<storm::jani::TemplateEdge> templateEdge = std::make_shared<storm::jani::TemplateEdge>(guard);
                automaton.registerTemplateEdge(templateEdge);

                templateEdge->addDestination(assignments);
                storm::jani::Edge e(locId, storm::jani::Model::SILENT_ACTION_INDEX, expressionManager->rational(trans.getRate()), templateEdge, {locId}, {expressionManager->integer(1)});
                automaton.addEdge(e);

            }
        }
    }
}