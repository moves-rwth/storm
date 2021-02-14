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

//                    storm::expressions::Expression newGuard = oldEdge.getGuard();
//                    if (!newGuard.containsVariables() && !newGuard.evaluateAsBool())
//                        continue;

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
                    session.getModel().replaceAutomaton(session.getModel().getAutomatonIndex(oldAutomaton.getName()), newAutomaton);
                }
            }
        }
    }
}