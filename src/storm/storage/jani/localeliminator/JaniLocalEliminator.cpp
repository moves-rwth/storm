#include "JaniLocalEliminator.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/jani/AutomatonComposition.h"
#include "storm/storage/expressions/Expression.h"
#include <storm/solver/Z3SmtSolver.h>
#include "FinishAction.h"

namespace storm {
    namespace jani {
        JaniLocalEliminator::JaniLocalEliminator(const Model &original, std::vector<storm::jani::Property>& properties) : original(original) {
            if (properties.size() != 1){
                STORM_LOG_WARN("Only the first property will be used for local elimination.");
            }
            property = properties[0];

            scheduler = EliminationScheduler();
        }

        void JaniLocalEliminator::eliminate() {
            newModel = original; // TODO: Make copy instead?

            Session session = Session(newModel, property);
            while (!session.getFinished()) {
                std::unique_ptr<Action> action = scheduler.getNextAction();
                action->doAction(session);
            }
            log = session.getLog();
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
                for (const EdgeDestination& dest : edge.getDestinations()){
                    if (dest.getLocationIndex() == locationIndex)
                        return true;
                }
            }
            return false;
        }

        bool JaniLocalEliminator::Session::isPossiblyInitial(const std::string &automatonName, std::string const& locationName) {
            Automaton &automaton = model.getAutomaton(automatonName);
            auto location = automaton.getLocation(automaton.getLocationIndex(locationName));
            for (const auto& asg : location.getAssignments()){
                if (!asg.isTransient())
                    continue;
                if (asg.getAssignedExpression().containsVariables() || asg.getVariable().getInitExpression().containsVariables())
                    continue;
                if (asg.getVariable().isBoundedIntegerVariable()){
                    int initValue = asg.getVariable().getInitExpression().evaluateAsInt();
                    int currentValue = asg.getAssignedExpression().evaluateAsInt();
                    if (initValue != currentValue)
                        return false;
                }
                else if (asg.getVariable().isBooleanVariable()){
                    bool initValue = asg.getVariable().getInitExpression().evaluateAsBool();
                    bool currentValue = asg.getAssignedExpression().evaluateAsBool();
                    if (initValue != currentValue)
                        return false;
                }
            }

            return true;
        }

        bool JaniLocalEliminator::Session::isPartOfProp(const std::string &automatonName, std::string const& locationName) {
            uint64_t locationIndex = model.getAutomaton(automatonName).getLocationIndex(locationName);
            return isPartOfProp(automatonName, locationIndex);
        }

        bool JaniLocalEliminator::Session::isPartOfProp(const std::string &automatonName, uint64_t locationIndex) {
            AutomatonInfo &autInfo = automataInfo[automatonName];
            return autInfo.potentiallyPartOfProp.count(locationIndex) == 1;
        }


        bool JaniLocalEliminator::Session::computeIsPartOfProp(const std::string &automatonName,
                                                               const std::string &locationName) {
            Automaton &automaton = model.getAutomaton(automatonName);
            uint64_t locationIndex = automaton.getLocationIndex(locationName);
            return computeIsPartOfProp(automatonName, locationIndex);
        }

        bool JaniLocalEliminator::Session::computeIsPartOfProp(const std::string &automatonName,
                                                               uint64_t locationIndex) {
            Automaton &automaton = model.getAutomaton(automatonName);
            auto location = automaton.getLocation(locationIndex);
            std::map<expressions::Variable, expressions::Expression> substitutionMap;
            for (auto &asg : location.getAssignments()){
                if (!asg.isTransient())
                    continue;
                substitutionMap.insert(std::pair<expressions::Variable, expressions::Expression>(asg.getExpressionVariable(), asg.getAssignedExpression()));
            }
            return computeIsPartOfProp(substitutionMap);
        }

        bool JaniLocalEliminator::Session::computeIsPartOfProp(const std::map<expressions::Variable, expressions::Expression>& substitutionMap) {
            storm::solver::Z3SmtSolver solver(model.getExpressionManager());
            auto propertyFormula = property.getRawFormula()->substitute(substitutionMap);
            auto atomicFormulas = propertyFormula->getAtomicExpressionFormulas();
            STORM_LOG_THROW(atomicFormulas.size() == 1, storm::exceptions::NotImplementedException, "Formulas with more than one (or zero) atomic formulas are not implemented");
            auto simplified = atomicFormulas[0]->getExpression().simplify();
            if (simplified.isLiteral()) {
                return simplified.evaluateAsBool();
            }
            solver.add(simplified);

            auto result = solver.check();
            return result != storm::solver::SmtSolver::CheckResult::Unsat;
        }

        void JaniLocalEliminator::Session::setPartOfProp(const std::string &automatonName, const std::string &locationName, bool isPartOfProp) {
            uint64_t locationIndex = model.getAutomaton(automatonName).getLocationIndex(locationName);
            return setPartOfProp(automatonName, locationIndex, isPartOfProp);
        }

        void JaniLocalEliminator::Session::setPartOfProp(const std::string &automatonName, uint64_t locationIndex, bool isPartOfProp) {
            AutomatonInfo &autInfo = automataInfo[automatonName];
            if (autInfo.potentiallyPartOfProp.count(locationIndex) == 1) {
                if (!isPartOfProp){
                    autInfo.potentiallyPartOfProp.erase(locationIndex);
                }
            } else {
                if (isPartOfProp){
                    autInfo.potentiallyPartOfProp.insert(locationIndex);
                }
            }
        }

        void JaniLocalEliminator::Session::clearIsPartOfProp(const std::string &automatonName) {
            AutomatonInfo &autInfo = automataInfo[automatonName];
            autInfo.potentiallyPartOfProp.clear();
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

        std::vector<std::string> JaniLocalEliminator::getLog() {
            return log;
        }

        JaniLocalEliminator::EliminationScheduler::EliminationScheduler() {
        }

        std::unique_ptr<JaniLocalEliminator::Action> JaniLocalEliminator::EliminationScheduler::getNextAction() {
            if (actionQueue.empty()){
                return std::make_unique<elimination_actions::FinishAction>();
            }
            std::unique_ptr<JaniLocalEliminator::Action> val = std::move(actionQueue.front());
            actionQueue.pop();
            return val;
        }

        void JaniLocalEliminator::EliminationScheduler::addAction(std::unique_ptr<JaniLocalEliminator::Action> action) {
            actionQueue.push(std::move(action));
        }

        JaniLocalEliminator::Session::Session(Model model, Property property) : model(model), property(property), finished(false){
            buildAutomataInfo();

            if (property.getRawFormula()->isRewardOperatorFormula()) {
                isRewardFormula = true;
                rewardModels = property.getRawFormula()->getReferencedRewardModels();
            } else if (property.getRawFormula()->isProbabilityOperatorFormula()){
                isRewardFormula = false;
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This type of property is currently not supported");
            }

            for (auto &var : property.getUsedVariablesAndConstants()){
                expressionVarsInProperty.insert(var.getIndex());
            }

        }

        Model &JaniLocalEliminator::Session::getModel() {
            return model;
        }

        void JaniLocalEliminator::Session::setModel(const Model &model) {
            this->model = model;
        }

        Property &JaniLocalEliminator::Session::getProperty() {
            return property;
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

        OrderedAssignments JaniLocalEliminator::Session::executeInSequence(const EdgeDestination& first, const EdgeDestination& then, std::set<std::string> &rewardVariables) {
            STORM_LOG_THROW(!first.usesAssignmentLevels() && !then.usesAssignmentLevels(),
                            storm::exceptions::NotImplementedException,
                            "Assignment levels are currently not supported");

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
                if (thenVariables.find(assignment.getExpressionVariable()) != thenVariables.end()) {
                    continue;
                }
                newOa.add(assignment);
            }

            std::map<expressions::Variable, expressions::Expression> substitutionMap = first.getAsVariableToExpressionMap();
            for (const auto &assignment : then.getOrderedAssignments()) {

                bool isReward = rewardVariables.count(assignment.getExpressionVariable().getName());
                auto firstAssignment = substitutionMap.find(assignment.getExpressionVariable());
                if (isReward && firstAssignment != substitutionMap.end()) {

                    auto newAssignment =
                            (*firstAssignment).second + assignment.getAssignedExpression().substitute(substitutionMap);

                    newOa.add(Assignment(assignment.getVariable(), newAssignment));
                } else {
                    newOa.add(Assignment(assignment.getVariable(),
                                         assignment.getAssignedExpression().substitute(substitutionMap).simplify()));
                }
            }
            return newOa;
        }

        bool JaniLocalEliminator::Session::isVariablePartOfProperty(const std::string &expressionVariableName){
            auto expressionVariable = model.getExpressionManager().getVariable(expressionVariableName);
            uint_fast64_t expressionVariableIndex = expressionVariable.getIndex();
            return expressionVarsInProperty.count(expressionVariableIndex) != 0;
        }

        void JaniLocalEliminator::Session::addToLog(const std::string& item) {
            log.push_back(item);
        }

        std::vector<std::string> JaniLocalEliminator::Session::getLog() {
            return log;
        }

        void JaniLocalEliminator::Session::flatten_automata() {
            model = model.flattenComposition();
            automataInfo.clear();
            buildAutomataInfo();
        }



        void JaniLocalEliminator::Session::addMissingGuards(const std::string& automatonName) {
            auto &automaton = model.getAutomaton(automatonName);

            std::string sinkName = "sink_location";
            while (automaton.hasLocation(sinkName)){
                sinkName += "_";
            }
            Location sink(sinkName, OrderedAssignments());
            automaton.addLocation(sink);
            uint64_t sinkIndex = automaton.getNumberOfLocations() - 1;

            automataInfo[automatonName].hasSink = true;
            automataInfo[automatonName].sinkIndex = sinkIndex;

            for (uint64_t i = 0; i < automaton.getNumberOfLocations(); i++){
                if (i == sinkIndex)
                    continue;
                auto outgoingEdges = automaton.getEdgesFromLocation(i);
                expressions::Expression allGuards;
                allGuards = model.getExpressionManager().boolean(false);
                for (const auto &edge : outgoingEdges){
                    allGuards = edge.getGuard() || allGuards;
                }
                expressions::Expression newGuard = !allGuards;

                // Before we add the edge, check whether it is satisfiable:
                auto variables = newGuard.getVariables();
                storm::solver::Z3SmtSolver solver(model.getExpressionManager());
                solver.add(newGuard);
                for (const auto &var : model.getGlobalVariables()){
                    if (var.isBoundedIntegerVariable() && variables.count(var.getExpressionVariable()) > 0){
                        auto &biVariable = var.asBoundedIntegerVariable();
                        solver.add(var.getExpressionVariable().getExpression() >= biVariable.getLowerBound());
                        solver.add(var.getExpressionVariable().getExpression() <= biVariable.getUpperBound());
                    }
                }
                for (const auto &var : automaton.getVariables()){
                    if (var.isBoundedIntegerVariable() && variables.count(var.getExpressionVariable()) > 0){
                        auto &biVariable = var.asBoundedIntegerVariable();
                        solver.add(var.getExpressionVariable().getExpression() >= biVariable.getLowerBound());
                        solver.add(var.getExpressionVariable().getExpression() <= biVariable.getUpperBound());
                    }
                }
                auto result = solver.check();

                if (result != storm::solver::SmtSolver::CheckResult::Unsat) {
                    addToLog("\tAdding missing guard from location " + automaton.getLocation(i).getName());
                    if (result == storm::solver::SmtSolver::CheckResult::Sat){
                        auto satisfyingAssignment = solver.getModel();
                        addToLog("\t\tThe guard was satisfiable with assignment ");
                        for (auto &var : variables){
                            if (var.hasIntegerType()){
                                addToLog("\t\t\t" + var.getName() + ": " + std::to_string(satisfyingAssignment->getIntegerValue(var)));
                            }
                            else if (var.hasBooleanType()){
                                addToLog("\t\t\t" + var.getName() + ": " + std::to_string(satisfyingAssignment->getBooleanValue(var)));
                            }
                            else if (var.hasRationalType()){
                                addToLog("\t\t\t" + var.getName() + ": " + std::to_string(satisfyingAssignment->getRationalValue(var)));
                            }
                        }
                    } else {
                        addToLog("\t\tThe solver could not determine whether the guard was satisfiable");
                    }
                    std::vector<std::pair<uint64_t, storm::expressions::Expression>> destinationLocationsAndProbabilities;
                    std::shared_ptr<storm::jani::TemplateEdge> templateEdge = std::make_shared<storm::jani::TemplateEdge>(newGuard);
                    TemplateEdgeDestination ted((OrderedAssignments()));
                    templateEdge->addDestination(ted);
                    destinationLocationsAndProbabilities.emplace_back(sinkIndex, model.getExpressionManager().rational(1.0));

                    automaton.addEdge(storm::jani::Edge(i, 0, boost::none, templateEdge, destinationLocationsAndProbabilities));
                } else{
                    addToLog("\tLocation " + automaton.getLocation(i).getName() +  " has no missing guard");
                }
            }
        }

        void JaniLocalEliminator::Session::buildAutomataInfo() {for (auto &aut : model.getAutomata()){
                automataInfo[aut.getName()] =  AutomatonInfo();
                for (auto &loc : aut.getLocations()){
                    bool isPartOfProp = computeIsPartOfProp(aut.getName(), loc.getName());
                    setPartOfProp(aut.getName(), loc.getName(), isPartOfProp);
                }
            }
        }

        JaniLocalEliminator::AutomatonInfo &JaniLocalEliminator::Session::getAutomatonInfo(const std::string& name) {
            return automataInfo[name];
        }

        JaniLocalEliminator::AutomatonInfo::AutomatonInfo()  : hasSink(false), sinkIndex(0){

        }
    }
}
