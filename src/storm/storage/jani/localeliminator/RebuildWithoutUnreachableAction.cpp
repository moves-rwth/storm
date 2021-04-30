#include "RebuildWithoutUnreachableAction.h"

namespace storm {
    namespace jani {
        namespace elimination_actions {
            RebuildWithoutUnreachableAction::RebuildWithoutUnreachableAction() {

            }

            std::string RebuildWithoutUnreachableAction::getDescription() {
                return "RebuildWithoutUnreachableAction";
            }

            void RebuildWithoutUnreachableAction::doAction(JaniLocalEliminator::Session &session) {
                session.addToLog("Rebuilding model without unreachable locations");
                for (auto oldAutomaton : session.getModel().getAutomata()) {
                    Automaton newAutomaton(oldAutomaton.getName(), oldAutomaton.getLocationExpressionVariable());
                    for (auto const& localVariable : oldAutomaton.getVariables())
                        newAutomaton.addVariable(localVariable);
                    newAutomaton.setInitialStatesRestriction(oldAutomaton.getInitialStatesRestriction());

                    std::unordered_set<Edge*> satisfiableEdges;

                    for (auto &oldEdge : oldAutomaton.getEdges()) {
                        if (!oldEdge.getGuard().containsVariables() && !oldEdge.getGuard().evaluateAsBool())
                            continue;
                        satisfiableEdges.emplace(&oldEdge);
                    }
                    session.addToLog("\t" +  std::to_string(satisfiableEdges.size()) + " of " + std::to_string(oldAutomaton.getEdges().size()) + " edges are satisfiable.");

                    std::unordered_set<uint64_t> reachableLocs;
                    std::unordered_set<uint64_t> reachableLocsOpen;

                    for (auto initialLocIndex : oldAutomaton.getInitialLocationIndices()){
                        reachableLocs.emplace(initialLocIndex);
                        reachableLocsOpen.emplace(initialLocIndex);
                    }

                    while (!reachableLocsOpen.empty()){
                        uint64_t current = *reachableLocsOpen.begin();
                        reachableLocsOpen.erase(current);

                        for (auto &edge : oldAutomaton.getEdgesFromLocation(current)) {
                            if (satisfiableEdges.count(&edge) == 1) {
                                for (auto const &dest : edge.getDestinations()){
                                    uint64_t target = dest.getLocationIndex();
                                    if (reachableLocs.count(target) == 0){
                                        reachableLocs.emplace(target);
                                        reachableLocsOpen.emplace(target);
                                    }
                                }
                            }
                        }
                    }
                    session.addToLog("\t" +  std::to_string(reachableLocs.size()) + " of " + std::to_string(oldAutomaton.getLocations().size()) + " locations are reachable.");

                    // Because the session keeps track of which variables might satisfy the property, we need to update
                    // those values (because we're changing the indices of locations). As a first step, we store which
                    // (old) locations potentially satisfy the property.
                    std::set<uint64_t> oldIsPartOfProp;
                    for (auto const& oldLoc : oldAutomaton.getLocations()) {
                        uint64_t oldLocationIndex = oldAutomaton.getLocationIndex(oldLoc.getName());
                        if (session.isPartOfProp(oldAutomaton.getName(), oldLocationIndex)){
                            oldIsPartOfProp.insert(oldLocationIndex);
                        }
                    }

                    std::map<uint64_t, uint64_t> oldToNewLocationIndices;

                    for (auto const& oldLoc : oldAutomaton.getLocations()) {
                        uint64_t oldLocationIndex = oldAutomaton.getLocationIndex(oldLoc.getName());
                        if (reachableLocs.count(oldLocationIndex) == 0)
                            continue;

                        Location newLoc(oldLoc.getName(), oldLoc.getAssignments());
                        newAutomaton.addLocation(newLoc);

                        uint64_t newLocationIndex = newAutomaton.getLocationIndex(newLoc.getName());
                        oldToNewLocationIndices.insert(std::pair<uint64_t, uint64_t>(oldLocationIndex, newLocationIndex));

                    }

                    for (auto initialLocIndex : oldAutomaton.getInitialLocationIndices()){
                        newAutomaton.addInitialLocation(initialLocIndex);
                    }

                    for (auto& oldEdge : oldAutomaton.getEdges()) {
                        uint64_t oldSource = oldEdge.getSourceLocationIndex();
                        if (reachableLocs.count(oldSource) == 0)
                            continue;

                        if (satisfiableEdges.count(&oldEdge) == 0)
                            continue;

                        std::shared_ptr<storm::jani::TemplateEdge> templateEdge = std::make_shared<storm::jani::TemplateEdge>(oldEdge.getGuard());

                        STORM_LOG_THROW(oldEdge.getAssignments().empty(), storm::exceptions::NotImplementedException, "Support for oldEdge-assignments is not implemented");

                        std::vector<std::pair<uint64_t, storm::expressions::Expression>> destinationLocationsAndProbabilities;
                        for (auto const& destination : oldEdge.getDestinations()) {
                            uint64_t newTarget = oldToNewLocationIndices[destination.getLocationIndex()];

                            OrderedAssignments oa(destination.getOrderedAssignments().clone());
                            TemplateEdgeDestination ted(oa);
                            templateEdge->addDestination(ted);
                            destinationLocationsAndProbabilities.emplace_back(newTarget, destination.getProbability());
                        }

                        uint64_t newSource = oldToNewLocationIndices[oldEdge.getSourceLocationIndex()];
                        newAutomaton.addEdge(storm::jani::Edge(newSource, oldEdge.getActionIndex(), oldEdge.hasRate() ? boost::optional<storm::expressions::Expression>(oldEdge.getRate()) : boost::none, templateEdge, destinationLocationsAndProbabilities));
                    }

                    // We now update which locations might satisfy the property (based on which old locations did and
                    // the old-to-new map.
                    session.clearIsPartOfProp(oldAutomaton.getName());
                    for (uint64_t oldLocationIndex : oldIsPartOfProp) {
                        session.setPartOfProp(oldAutomaton.getName(), oldToNewLocationIndices[oldLocationIndex], true);
                    }

                    auto &automatonInfo = session.getAutomatonInfo(oldAutomaton.getName());
                    if (automatonInfo.hasSink) {
                        automatonInfo.sinkIndex = oldToNewLocationIndices[automatonInfo.sinkIndex];
                    }

                    session.addToLog("\tNew automaton has " +  std::to_string(newAutomaton.getEdges().size()) + " edges.");
                    session.getModel().replaceAutomaton(session.getModel().getAutomatonIndex(oldAutomaton.getName()), newAutomaton);
                }
            }
        }
    }
}