#include "JaniLocalEliminator.h"
#include "JaniLocationExpander.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/jani/AutomatonComposition.h"
#include "storm/storage/expressions/Expression.h"

namespace storm {
    namespace jani {

        JaniLocalEliminator::JaniLocalEliminator(const Model &original, std::vector<storm::jani::Property>& properties) : original(original) {
            if (properties.size() != 1){
                STORM_LOG_ERROR("Local elimination only works with exactly one property");
            } else {
                property = properties[0];
            }
        }

        void JaniLocalEliminator::eliminate() {
            if (original.getAutomata().size() != 1){
                STORM_LOG_ERROR("State Space Reduction is only supported for Jani models with a single automaton.");
                return;
            }
            newModel = original; // TODO: Make copy instead?
            unfold("s");
            eliminate("multiplex", "l_s_2");
            eliminate("multiplex", "l_s_3");
            eliminate("multiplex", "l_s_1");
            cleanUpAutomaton("multiplex");
            // eliminate()
        }

        Model const &JaniLocalEliminator::getResult() {
            return newModel;
        }

        void JaniLocalEliminator::unfold(const std::string &variableName) {
            JaniLocationExpander expander = JaniLocationExpander(newModel);
            expander.transform("multiplex", "s");
            newModel = expander.getResult();
        }

        void JaniLocalEliminator::eliminate(const std::string &automatonName, const std::string &locationName) {
            STORM_LOG_THROW(!hasLoops(automatonName, locationName), storm::exceptions::InvalidArgumentException, "Locations with loops cannot be eliminated");

            Automaton& automaton = newModel.getAutomaton(automatonName);
            uint64_t locIndex = automaton.getLocationIndex(locationName);


            // std::vector<std::tuple<Edge, EdgeDestination>> incomingEdges;
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
                            eliminateDestination(automaton, edge, i, outgoingEdges);
                            changed = true;
                            break; // Avoids weird behaviour, but doesn't work if multiplicity is higher than 1
                            // incomingEdges.emplace_back(std::make_tuple(edge, dest));
                        }
                    }

                    if (changed)
                        break;
                }
            }


            for (Edge edge : automaton.getEdges()) {
                if (!edge.getGuard().containsVariables() && !edge.getGuard().evaluateAsBool())
                    continue;
                for (const EdgeDestination dest : edge.getDestinations()) {
                    if (dest.getLocationIndex() == locIndex){
                        STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "Could not eliminate location");
                    }
                }
            }


            // TODO: Handle multiplicity correctly
        }

        void JaniLocalEliminator::eliminateDestination(Automaton &automaton, Edge &edge, const uint64_t destIndex, detail::Edges &outgoing) {
            uint64_t sourceIndex = edge.getSourceLocationIndex();
            uint64_t actionIndex = edge.getActionIndex();
            EdgeDestination dest = edge.getDestination(destIndex);

            std::vector<Edge> newEdges; // Don't add the new edges immediately -- we cannot safely iterate over the outgoing edges while adding new edges to the structure

            for (Edge outEdge : outgoing) {
                if (!outEdge.getGuard().containsVariables() && !outEdge.getGuard().evaluateAsBool())
                    continue;

                STORM_LOG_THROW(actionIndex == outEdge.getActionIndex(), storm::exceptions::NotImplementedException, "Elimination of edges with different action indices is not implemented");

                expressions::Expression newGuard = getNewGuard(edge, dest, outEdge);
                std::shared_ptr<storm::jani::TemplateEdge> templateEdge = std::make_shared<storm::jani::TemplateEdge>(newGuard);
                std::vector<std::pair<uint64_t, storm::expressions::Expression>> destinationLocationsAndProbabilities;
                for (const EdgeDestination& outDest : outEdge.getDestinations()) {

                    expressions::Expression probability = getProbability(dest, outDest);

                    OrderedAssignments oa = executeInSequence(dest, outDest);
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

                STORM_LOG_THROW(!edge.hasRate() && !outEdge.hasRate(), storm::exceptions::NotImplementedException, "Edge Rates are not imlpemented");
                newEdges.emplace_back(Edge(sourceIndex, actionIndex, boost::none, templateEdge, destinationLocationsAndProbabilities));
            }
            for (const Edge& newEdge : newEdges){
                automaton.addEdge(newEdge);
            }

            edge.setGuard(edge.getGuard().getManager().boolean(false)); // Instead of deleting the edge
        }

        void JaniLocalEliminator::eliminate_all() {
            // TODO
        }

        expressions::Expression JaniLocalEliminator::getNewGuard(const Edge& edge, const EdgeDestination& dest, const Edge& outgoing) {
            expressions::Expression wp = outgoing.getGuard().substitute(dest.getAsVariableToExpressionMap()).simplify();
            return edge.getGuard() && wp;
        }

        expressions::Expression JaniLocalEliminator::getProbability(const EdgeDestination &first, const EdgeDestination &then) {
            return first.getProbability() * then.getProbability().substitute(first.getAsVariableToExpressionMap());
        }

        OrderedAssignments JaniLocalEliminator::executeInSequence(const EdgeDestination& first, const EdgeDestination& then) {
            OrderedAssignments oa(first.getOrderedAssignments().clone());
            uint64_t levelOffset = first.getOrderedAssignments().getHighestLevel() + 1;
            for (const auto& assignment : then.getOrderedAssignments()) {
                auto newAssignment = Assignment(assignment);
                newAssignment.setLevel(assignment.getLevel() + levelOffset);
                oa.add(newAssignment);
            }

            // TODO: Figure out how to use simplifyLevels:
            // oa.simplifyLevels( ... )

            return oa;
        }


        bool JaniLocalEliminator::hasLoops(const std::string &automatonName, std::string const& locationName) {
            Automaton &automaton = newModel.getAutomaton(automatonName);
            uint64_t locationIndex = automaton.getLocationIndex(locationName);
            for (Edge edge : automaton.getEdgesFromLocation(locationIndex)) {
                for (EdgeDestination dest : edge.getDestinations()){
                    if (dest.getLocationIndex() == locationIndex)
                        return true;
                }
            }
            return false;
        }

        void JaniLocalEliminator::cleanUpAutomaton(std::string const &automatonName){
            Automaton& oldAutomaton = newModel.getAutomaton(automatonName);
            Automaton newAutomaton(oldAutomaton.getName(), oldAutomaton.getLocationExpressionVariable());
            for (auto const& localVariable : oldAutomaton.getVariables())
                newAutomaton.addVariable(localVariable);
            newAutomaton.setInitialStatesRestriction(oldAutomaton.getInitialStatesRestriction());

            for (const Location &loc : oldAutomaton.getLocations())
                newAutomaton.addLocation(loc);
            for (uint64_t initialLocationIndex : oldAutomaton.getInitialLocationIndices())
                newAutomaton.addInitialLocation(initialLocationIndex);

            int eliminated = 0;
            for (const Edge &edge : oldAutomaton.getEdges()){
                if (edge.getGuard().containsVariables() || edge.getGuard().evaluateAsBool()){
                    newAutomaton.addEdge(edge);
                }
                else{
                    eliminated++;
                }
            }

            newModel.replaceAutomaton(0, newAutomaton);
        }
    }
}
