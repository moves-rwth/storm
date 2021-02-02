#include "JaniLocalEliminator.h"
#include "JaniLocationExpander.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/jani/AutomatonComposition.h"
#include "storm/storage/expressions/Expression.h"
#include <boost/format.hpp>
#include <storm/solver/Z3SmtSolver.h>

namespace storm {
    namespace jani {

        JaniLocalEliminator::JaniLocalEliminator(const Model &original, std::vector<storm::jani::Property>& properties) : original(original) {
            if (properties.size() != 1){
                STORM_LOG_WARN("Local elimination only works with exactly one property");
            }
            property = properties[0];
            scheduler = EliminationScheduler();
        }

        void JaniLocalEliminator::eliminate() {

            if (original.getAutomata().size() != 1) {
                STORM_LOG_ERROR("State Space Reduction is only supported for Jani models with a single automaton.");
                return;
            }

            newModel = original; // TODO: Make copy instead?

            Session session = Session(newModel, property);
            while (!session.getFinished()) {
                std::unique_ptr<Action> action = scheduler.getNextAction();
                action->doAction(session);
            }
            newModel = session.getModel();
        }

        Model const &JaniLocalEliminator::getResult() {
            return newModel;
        }

        bool JaniLocalEliminator::Session::isEliminable(const std::string &automatonName, std::string const& locationName) {
            return !isPossiblyInitial(automatonName, locationName) && !hasLoops(automatonName, locationName) && isPartOfProp(automatonName, locationName);
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

        bool JaniLocalEliminator::Session::isPossiblyInitial(const std::string &automatonName, std::string const& locationName) {
            Automaton &automaton = model.getAutomaton(automatonName);
            auto location = automaton.getLocation(automaton.getLocationIndex(locationName));
            for (auto asg : location.getAssignments()){
                if (!asg.isTransient())
                    continue;
                if (asg.getAssignedExpression().containsVariables() || asg.getVariable().getInitExpression().containsVariables())
                    continue;
                int initValue = asg.getVariable().getInitExpression().evaluateAsInt();
                int currentValue = asg.getAssignedExpression().evaluateAsInt();
                if (initValue != currentValue)
                    return false;
            }

            return true;
        }

        bool JaniLocalEliminator::Session::isPartOfProp(const std::string &automatonName, std::string const& locationName) {
            Automaton &automaton = model.getAutomaton(automatonName);
            auto location = automaton.getLocation(automaton.getLocationIndex(locationName));
            std::map<expressions::Variable, expressions::Expression> substitutionMap;
            for (auto asg : location.getAssignments()){
                if (!asg.isTransient())
                    continue;
                substitutionMap.insert(std::pair<expressions::Variable, expressions::Expression>(asg.getExpressionVariable(), asg.getAssignedExpression()));
            }

            storm::solver::Z3SmtSolver solver(model.getExpressionManager());
            auto propertyFormula = property.getRawFormula()->substitute(substitutionMap);
            for (auto atomicFormula : propertyFormula->getAtomicExpressionFormulas()){
                solver.add(atomicFormula->getExpression());
            }
            auto result = solver.check();
            return result != storm::solver::SmtSolver::CheckResult::Unsat;
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


        JaniLocalEliminator::UnfoldAction::UnfoldAction(const std::string &automatonName, const std::string &variableName) {
            this->automatonName = automatonName;
            this->variableName = variableName;
        }

        std::string JaniLocalEliminator::UnfoldAction::getDescription() {
            return (boost::format("UnfoldAction (Automaton %s, Variable %s)") % automatonName % variableName).str();
        }

        void JaniLocalEliminator::UnfoldAction::doAction(JaniLocalEliminator::Session &session) {
            JaniLocationExpander expander = JaniLocationExpander(session.getModel());
            expander.transform(automatonName, variableName);
            session.setModel(expander.getResult());
        }


        JaniLocalEliminator::EliminateAction::EliminateAction(const std::string &automatonName, const std::string &locationName) {
            this->automatonName = automatonName;
            this->locationName = locationName;
        }

        std::string JaniLocalEliminator::EliminateAction::getDescription() {
            return (boost::format("EliminateAction (Automaton %s, Location %s)") % automatonName % locationName).str();
        }

        void JaniLocalEliminator::EliminateAction::doAction(JaniLocalEliminator::Session &session) {
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

                STORM_LOG_THROW(!edge.hasRate() && !outEdge.hasRate(), storm::exceptions::NotImplementedException, "Edge Rates are not imlpemented");
                newEdges.emplace_back(Edge(sourceIndex, actionIndex, boost::none, templateEdge, destinationLocationsAndProbabilities));
            }
            for (const Edge& newEdge : newEdges){
                automaton.addEdge(newEdge);
            }

            edge.setGuard(edge.getGuard().getManager().boolean(false)); // Instead of deleting the edge
        }


        JaniLocalEliminator::EliminateAutomaticallyAction::EliminateAutomaticallyAction(const std::string &automatonName, JaniLocalEliminator::EliminateAutomaticallyAction::EliminationOrder order, uint32_t transitionCountThreshold)
            : automatonName(automatonName),
            eliminationOrder(order),
            transitionCountThreshold(transitionCountThreshold){
        }

        std::string JaniLocalEliminator::EliminateAutomaticallyAction::getDescription() {
            return "EliminateAutomaticallyAction";
        }

        void JaniLocalEliminator::EliminateAutomaticallyAction::doAction(JaniLocalEliminator::Session &session) {
            Automaton &automaton = session.getModel().getAutomaton(automatonName);
            switch (eliminationOrder){
                case EliminationOrder::Arbitrary: {
                    for (auto loc : automaton.getLocations()) {
                        if (session.isEliminable(automatonName, loc.getName())) {
                            EliminateAction action = EliminateAction(automatonName, loc.getName());
                            action.doAction(session);
                        }
                    }
                    break;
                }
                case EliminationOrder::NewTransitionCount: {
                    std::map<std::string, bool> isInitialOrPropMap;
                    std::map<std::string, bool> alreadyEliminated;
                    for (auto loc : automaton.getLocations()) {
                        bool isInitialOrProp = (session.isPossiblyInitial(automatonName, loc.getName())
                                                || session.isPartOfProp(automatonName, loc.getName()));
                        isInitialOrPropMap.insert(std::pair<std::string, bool>(loc.getName(), isInitialOrProp));
                        alreadyEliminated.insert(std::pair<std::string, bool>(loc.getName(), false));
                    }

                    bool done = false;
                    while (!done) {
                        int minNewEdges = transitionCountThreshold;
                        int bestLocIndex = -1;
                        for (auto loc : automaton.getLocations()) {
                            if (isInitialOrPropMap[loc.getName()] || alreadyEliminated[loc.getName()])
                                continue;

                            int locIndex = automaton.getLocationIndex(loc.getName());
                            int outgoing = automaton.getEdgesFromLocation(locIndex).size();
                            int incoming = 0;
                            for (auto edge : automaton.getEdges()) {
                                int addedTransitions = 1;
                                for (auto dest : edge.getDestinations())
                                    if (dest.getLocationIndex() == locIndex)
                                        addedTransitions *= outgoing;
                                incoming += addedTransitions;
                            }
                            int newEdges = incoming * outgoing;
                            if (newEdges <= minNewEdges){
                                minNewEdges = newEdges;
                                bestLocIndex = locIndex;
                            }
                        }

                        if (bestLocIndex == -1){
                            done = true;
                        } else {
                            std::string locName = automaton.getLocation(bestLocIndex).getName();
                            EliminateAction action = EliminateAction(automatonName, locName);
                            action.doAction(session);
                            automaton = session.getModel().getAutomaton(automatonName);
                            isInitialOrPropMap[locName] = true;
                        }
                    }

                    break;
                }
                default: {
                    STORM_LOG_THROW(true, storm::exceptions::NotImplementedException, "This elimination order is not yet implemented");
                    break;
                }
            }
        }

        std::string JaniLocalEliminator::EliminateAutomaticallyAction::find_next_location(JaniLocalEliminator::Session &session) {
            return "";
        }

        JaniLocalEliminator::FinishAction::FinishAction() {
        }

        std::string JaniLocalEliminator::FinishAction::getDescription() {
            return "FinishAction";
        }

        void JaniLocalEliminator::FinishAction::doAction(JaniLocalEliminator::Session &session) {
            session.setFinished(true);
        }

        JaniLocalEliminator::RebuildWithoutUnreachableAction::RebuildWithoutUnreachableAction() {

        }

        std::string JaniLocalEliminator::RebuildWithoutUnreachableAction::getDescription() {
            return "RebuildWithoutUnreachableAction";
        }

        void JaniLocalEliminator::RebuildWithoutUnreachableAction::doAction(JaniLocalEliminator::Session &session) {
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


        JaniLocalEliminator::EliminationScheduler::EliminationScheduler() {
        }

        std::unique_ptr<JaniLocalEliminator::Action> JaniLocalEliminator::EliminationScheduler::getNextAction() {
            if (actionQueue.empty()){
                return std::make_unique<JaniLocalEliminator::FinishAction>();
            }
            std::unique_ptr<JaniLocalEliminator::Action> val = std::move(actionQueue.front());
            actionQueue.pop();
            return val;
        }

        void JaniLocalEliminator::EliminationScheduler::addAction(std::unique_ptr<JaniLocalEliminator::Action> action) {
            actionQueue.push(std::move(action));
        }

        JaniLocalEliminator::Session::Session(Model model, Property property) : model(model), property(property), finished(false){

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
            return (edge.getGuard() && wp).simplify();
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
