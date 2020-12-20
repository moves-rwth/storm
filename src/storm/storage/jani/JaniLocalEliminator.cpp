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
            EliminationScheduler scheduler = EliminationScheduler();

            if (original.getAutomata().size() != 1) {
                STORM_LOG_ERROR("State Space Reduction is only supported for Jani models with a single automaton.");
                return;
            }

            newModel = original; // TODO: Make copy instead?

            Session session = Session(newModel);
            while (!session.getFinished()) {
                std::unique_ptr<Action> action = scheduler.getNextAction();
                action->doAction(session);
            }
            newModel = session.getModel();
        }

        Model const &JaniLocalEliminator::getResult() {
            return newModel;
        }

        bool JaniLocalEliminator::Session::hasLoops(const std::string &automatonName, std::string const& locationName) {
            Automaton &automaton = model.getAutomaton(automatonName);
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


        JaniLocalEliminator::UnfoldAction::UnfoldAction(const std::string &variableName) {
            this->variableName = variableName;
        }

        std::string JaniLocalEliminator::UnfoldAction::getDescription() {
            return "UnfoldAction (Variable " + this->variableName  + ")";
        }

        void JaniLocalEliminator::UnfoldAction::doAction(JaniLocalEliminator::Session &session) {
            JaniLocationExpander expander = JaniLocationExpander(session.getModel());
            expander.transform("multiplex", "s");
            session.setModel(expander.getResult());
        }


        JaniLocalEliminator::EliminateAction::EliminateAction(const std::string &locationName) {
            this->locationName = locationName;
        }

        std::string JaniLocalEliminator::EliminateAction::getDescription() {
            return "UnfoldAction (Variable " + this->locationName  + ")";
        }

        void JaniLocalEliminator::EliminateAction::doAction(JaniLocalEliminator::Session &session) {
            std::string automatonName = "multiplex";
            STORM_LOG_THROW(!session.hasLoops(automatonName, locationName), storm::exceptions::InvalidArgumentException, "Locations with loops cannot be eliminated");

            Automaton& automaton = session.getModel().getAutomaton(automatonName);
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


            for (Edge edge : automaton.getEdges()) {
                if (!edge.getGuard().containsVariables() && !edge.getGuard().evaluateAsBool())
                    continue;
                for (const EdgeDestination dest : edge.getDestinations()) {
                    if (dest.getLocationIndex() == locIndex){
                        STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "Could not eliminate location");
                    }
                }
            }
        }

        void JaniLocalEliminator::EliminateAction::eliminateDestination(JaniLocalEliminator::Session &session, Automaton &automaton, Edge &edge, const uint64_t destIndex, detail::Edges &outgoing) {
            uint64_t sourceIndex = edge.getSourceLocationIndex();
            uint64_t actionIndex = edge.getActionIndex();
            EdgeDestination dest = edge.getDestination(destIndex);

            std::vector<Edge> newEdges; // Don't add the new edges immediately -- we cannot safely iterate over the outgoing edges while adding new edges to the structure

            for (Edge outEdge : outgoing) {
                if (!outEdge.getGuard().containsVariables() && !outEdge.getGuard().evaluateAsBool())
                    continue;

                STORM_LOG_THROW(actionIndex == outEdge.getActionIndex(), storm::exceptions::NotImplementedException, "Elimination of edges with different action indices is not implemented");

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

                STORM_LOG_THROW(!edge.hasRate() && !outEdge.hasRate(), storm::exceptions::NotImplementedException, "Edge Rates are not imlpemented");
                newEdges.emplace_back(Edge(sourceIndex, actionIndex, boost::none, templateEdge, destinationLocationsAndProbabilities));
            }
            for (const Edge& newEdge : newEdges){
                automaton.addEdge(newEdge);
            }

            edge.setGuard(edge.getGuard().getManager().boolean(false)); // Instead of deleting the edge
        }


        JaniLocalEliminator::FinishAction::FinishAction() {
        }

        std::string JaniLocalEliminator::FinishAction::getDescription() {
            return "FinishAction";
        }

        void JaniLocalEliminator::FinishAction::doAction(JaniLocalEliminator::Session &session) {
            session.setFinished(true);
        }


        JaniLocalEliminator::EliminationScheduler::EliminationScheduler() {
            actionQueue.push(std::make_unique<JaniLocalEliminator::UnfoldAction>("s"));
            actionQueue.push(std::make_unique<JaniLocalEliminator::EliminateAction>("l_s_2"));
            actionQueue.push(std::make_unique<JaniLocalEliminator::EliminateAction>("l_s_3"));
            actionQueue.push(std::make_unique<JaniLocalEliminator::EliminateAction>("l_s_1"));
            actionQueue.push(std::make_unique<JaniLocalEliminator::FinishAction>());
        }

        std::unique_ptr<JaniLocalEliminator::Action> JaniLocalEliminator::EliminationScheduler::getNextAction() {
            if (actionQueue.empty()){
                return std::make_unique<JaniLocalEliminator::FinishAction>();
            }
            std::unique_ptr<JaniLocalEliminator::Action> val = std::move(actionQueue.front());
            actionQueue.pop();
            return val;
        }

        JaniLocalEliminator::Session::Session(Model model) : model(model), finished(false){

        }

        Model &JaniLocalEliminator::Session::getModel() {
            return model;
        }

        void JaniLocalEliminator::Session::setModel(const Model &model) {
            this->model = model;
        }

        bool JaniLocalEliminator::Session::getFinished() {
            return finished;
        }

        void JaniLocalEliminator::Session::setFinished(bool finished) {
            this->finished = finished;

        }

        expressions::Expression JaniLocalEliminator::Session::getNewGuard(const Edge& edge, const EdgeDestination& dest, const Edge& outgoing) {
            expressions::Expression wp = outgoing.getGuard().substitute(dest.getAsVariableToExpressionMap()).simplify();
            return edge.getGuard() && wp;
        }

        expressions::Expression JaniLocalEliminator::Session::getProbability(const EdgeDestination &first, const EdgeDestination &then) {
            return (first.getProbability() * then.getProbability().substitute(first.getAsVariableToExpressionMap())).simplify();
        }

        OrderedAssignments JaniLocalEliminator::Session::executeInSequence(const EdgeDestination& first, const EdgeDestination& then) {
            STORM_LOG_THROW(!first.usesAssignmentLevels() && !then.usesAssignmentLevels(), storm::exceptions::NotImplementedException, "Assignment levels are currently not supported");

            /*
             * x = 2
             * y = 1
             *
             * x = x + y
             *
             * =>
             *
             * thenVariables = [x]
             * y = 1
             * x = 2 + 1
             */

            OrderedAssignments newOa;

            // Collect variables that occur in the second set of assignments. This will be used to decide which first assignments to keep and which to discard.
            std::set<expressions::Variable> thenVariables;
            for (const auto &assignment : then.getOrderedAssignments()) {
                thenVariables.emplace(assignment.getExpressionVariable());
            }

            for (const auto &assignment : first.getOrderedAssignments()) {
                if (thenVariables.find(assignment.getExpressionVariable()) != thenVariables.end())
                    continue; // Skip variables for which a second assignment exists
                newOa.add(assignment);
            }

            std::map<expressions::Variable, expressions::Expression> substitutionMap = first.getAsVariableToExpressionMap();
            for (const auto &assignment : then.getOrderedAssignments()){
                newOa.add(Assignment(assignment.getVariable(), assignment.getAssignedExpression().substitute(substitutionMap).simplify()));
            }
            return newOa;
        }
    }
}
