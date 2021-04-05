#include "EliminateAction.h"
#include <boost/format.hpp>
#include "storm/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace jani {
        namespace elimination_actions {
            EliminateAction::EliminateAction(const std::string &automatonName, const std::string &locationName) {
                this->automatonName = automatonName;
                this->locationName = locationName;
            }

            std::string EliminateAction::getDescription() {
                return (boost::format("EliminateAction (Automaton %s, Location %s)") % automatonName % locationName).str();
            }

            void EliminateAction::doAction(JaniLocalEliminator::Session &session) {
                STORM_LOG_THROW(!session.hasLoops(automatonName, locationName), storm::exceptions::InvalidArgumentException, "Locations with loops cannot be eliminated");

                Automaton& automaton = session.getModel().getAutomaton(automatonName);
                uint64_t locIndex = automaton.getLocationIndex(locationName);

                bool changed = true;
                while (changed) {
                    changed = false;
                    for (Edge edge : automaton.getEdges()) {

                        if (!edge.getGuard().containsVariables() && !edge.getGuard().evaluateAsBool())
                            continue;

                        uint64_t destCount = edge.getNumberOfDestinations();
                        for (uint64_t i = 0; i < destCount; i++) {
                            const EdgeDestination& dest = edge.getDestination(i);
                            if (dest.getLocationIndex() == locIndex) {
                                detail::Edges outgoingEdges = automaton.getEdgesFromLocation(locationName);
                                eliminateDestination(session, automaton, edge, i, outgoingEdges);
                                changed = true;
                                break; // Avoids weird behaviour, but doesn't work if multiplicity is higher than 1
                                // incomingEdges.emplace_back(std::make_tuple(edge, dest));
                            }
                        }

                        if (changed)
                            break;
                    }
                }


                for (Edge& edge : automaton.getEdges()) {
                    if (!edge.getGuard().containsVariables() && !edge.getGuard().evaluateAsBool())
                        continue;
                    for (const EdgeDestination& dest : edge.getDestinations()) {
                        if (dest.getLocationIndex() == locIndex){
                            STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "Could not eliminate location");
                        }
                    }
                }
            }

            void EliminateAction::eliminateDestination(JaniLocalEliminator::Session &session, Automaton &automaton, Edge &edge, const uint64_t destIndex, detail::Edges &outgoing) {
                uint64_t sourceIndex = edge.getSourceLocationIndex();
                uint64_t actionIndex = edge.getActionIndex();
                EdgeDestination dest = edge.getDestination(destIndex);

                std::vector<Edge> newEdges; // Don't add the new edges immediately -- we cannot safely iterate over the outgoing edges while adding new edges to the structure

                for (Edge& outEdge : outgoing) {
                    if (!outEdge.getGuard().containsVariables() && !outEdge.getGuard().evaluateAsBool())
                        continue;

                    // STORM_LOG_THROW(actionIndex == outEdge.getActionIndex(), storm::exceptions::NotImplementedException, "Elimination of edges with different action indices is not implemented");

                    expressions::Expression newGuard = session.getNewGuard(edge, dest, outEdge);
                    std::shared_ptr<storm::jani::TemplateEdge> templateEdge = std::make_shared<storm::jani::TemplateEdge>(newGuard);
                    std::vector<std::pair<uint64_t, storm::expressions::Expression>> destinationLocationsAndProbabilities;
                    for (const EdgeDestination& outDest : outEdge.getDestinations()) {

                        expressions::Expression probability = session.getProbability(dest, outDest);

                        OrderedAssignments oa = session.executeInSequence(dest, outDest);
                        TemplateEdgeDestination templateEdgeDestination(oa);
                        templateEdge->addDestination(templateEdgeDestination);

                        destinationLocationsAndProbabilities.emplace_back(outDest.getLocationIndex(), probability);

                    }

                    // Add remaining edges back to the edge:
                    uint64_t destCount = edge.getNumberOfDestinations();
                    for (uint64_t i = 0; i < destCount; i++) {
                        if (i == destIndex)
                            continue;
                        const EdgeDestination& unchangedDest = edge.getDestination(i);
                        OrderedAssignments oa(unchangedDest.getOrderedAssignments().clone());
                        TemplateEdgeDestination templateEdgeDestination(oa);
                        templateEdge->addDestination(templateEdgeDestination);
                        destinationLocationsAndProbabilities.emplace_back(unchangedDest.getLocationIndex(), unchangedDest.getProbability());
                    }

                    STORM_LOG_THROW(!edge.hasRate() && !outEdge.hasRate(), storm::exceptions::NotImplementedException, "Edge Rates are not implemented");
                    newEdges.emplace_back(Edge(sourceIndex, actionIndex, boost::none, templateEdge, destinationLocationsAndProbabilities));
                }
                for (const Edge& newEdge : newEdges){
                    automaton.addEdge(newEdge);
                }

                edge.setGuard(edge.getGuard().getManager().boolean(false)); // Instead of deleting the edge
            }
        }
    }
}