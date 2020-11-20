#include "storm/storage/jani/JaniLocationExpander.h"

#include "storm/storage/jani/expressions/JaniExpressionSubstitutionVisitor.h"

#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/IllegalArgumentException.h"


namespace storm {
    namespace jani {
        JaniLocationExpander::JaniLocationExpander(Model const& origModel) : original(origModel) {

        }

        void JaniLocationExpander::transform(std::string const& automatonName, std::string const& variableName) {
            STORM_LOG_THROW(original.hasAutomaton(automatonName), storm::exceptions::IllegalArgumentException, "Model has no automaton with name " << automatonName << ". ");
            STORM_LOG_THROW(original.getAutomaton(automatonName).hasVariable(variableName) || original.hasGlobalVariable(variableName), storm::exceptions::IllegalArgumentException, "Automaton " << automatonName << " has no variable with name " << variableName << ". ");
            newModel = original;
            newModel.replaceAutomaton(newModel.getAutomatonIndex(automatonName), transformAutomaton(original.getAutomaton(automatonName), variableName, false));
        }


        Model const& JaniLocationExpander::getResult() const {
            return newModel;
        }

        Automaton JaniLocationExpander::transformAutomaton(Automaton const& automaton, std::string const& variableName, bool useTransientVariables) {

            Automaton newAutomaton(automaton.getName(), automaton.getLocationExpressionVariable());
            int64_t initialVariableValue;
            int64_t variableLowerBound;
            int64_t variableUpperBound;
            storm::expressions::Variable eliminatedExpressionVariable;
            const storm::jani::Variable* variable;

            for (auto const& localVariable : automaton.getVariables()) // The expanded variable is also added, but will be set to transient later
                newAutomaton.addVariable(localVariable);

            bool isGlobalVariable = !automaton.hasVariable(variableName);
            VariableSet& containingSet = isGlobalVariable ? newModel.getGlobalVariables() : newAutomaton.getVariables();

            auto uncastVar = &containingSet.getVariable(variableName);
            auto var = containingSet.getVariable(variableName).asBoundedIntegerVariable();
            STORM_LOG_THROW(var.hasInitExpression(), storm::exceptions::IllegalArgumentException, "Variable to be eliminated has to have an initexpression.");
            STORM_LOG_THROW(var.isBoundedIntegerVariable(), storm::exceptions::IllegalArgumentException, "Variable to be eliminated has to be an bounded integer variable.");
            STORM_LOG_THROW(!var.isTransient(), storm::exceptions::IllegalArgumentException, "Cannot eliminate transient variable");

            variableUpperBound = var.getUpperBound().evaluateAsInt();
            variableLowerBound  = var.getLowerBound().evaluateAsInt();
            initialVariableValue = var.getInitExpression().evaluateAsInt();
            eliminatedExpressionVariable = var.getExpressionVariable();

            variable = &var;

            // containingSet.eraseVariable(var.getExpressionVariable());
            var.setTransient(useTransientVariables);
            // containingSet.addVariable(var);

            STORM_LOG_THROW(!automaton.getInitialStatesRestriction().containsVariable({eliminatedExpressionVariable}), storm::exceptions::NotSupportedException, "Elimination of variable that occurs in the initial state restriction is not allowed");
            newAutomaton.setInitialStatesRestriction(automaton.getInitialStatesRestriction());


            std::map<storm::expressions::Variable, storm::expressions::Expression> substitutionMap;
            std::map<std::string, std::vector<std::string>> locationNames;
            std::map<uint64_t, std::map<int64_t, uint64_t>> locationVariableValueMap;
            for (auto const& loc : automaton.getLocations()) {
                locationNames[loc.getName()] = std::vector<std::string>();
                uint64_t origIndex = automaton.getLocationIndex(loc.getName());

                for (int64_t i = variableLowerBound; i <= variableUpperBound; i++) {
                    std::string newLocationName = loc.getName() + "_" + variableName + "_" + std::to_string(i);
                    substitutionMap[eliminatedExpressionVariable] = original.getExpressionManager().integer(i);
                    OrderedAssignments newAssignments = loc.getAssignments().clone();
                    newAssignments.substitute(substitutionMap);
                    Location loc(newLocationName, newAssignments);

                    if (useTransientVariables)
                        loc.addTransientAssignment(Assignment(var, original.getExpressionManager().integer(i), 0)); // TODO: What is the level?

                    uint64_t newLocationIndex = newAutomaton.addLocation(loc);

                    if (i == initialVariableValue)
                        newAutomaton.addInitialLocation(newLocationName);

                    locationVariableValueMap[origIndex][i] = newLocationIndex;
                    locationNames[loc.getName()].push_back(newLocationName);
                }
            }



            for (auto const& edge : automaton.getEdges()) {
                for (auto const& newValueAndLocation : locationVariableValueMap[edge.getSourceLocationIndex()]) {
                    int64_t currentValue = newValueAndLocation.first;
                    substitutionMap[eliminatedExpressionVariable] = original.getExpressionManager().integer(currentValue);

                    uint64_t newSourceIndex = newValueAndLocation.second;
                    storm::expressions::Expression newGuard = substituteJaniExpression(edge.getGuard(), substitutionMap).simplify();
                    if (!newGuard.containsVariables() && !newGuard.evaluateAsBool()) {
                        continue;
                    }
                    std::shared_ptr<storm::jani::TemplateEdge> templateEdge = std::make_shared<storm::jani::TemplateEdge>(newGuard);

                    STORM_LOG_THROW(edge.getAssignments().empty(), storm::exceptions::NotImplementedException, "Support for edge-assignments is not implemented");

                    std::vector<std::pair<uint64_t, storm::expressions::Expression>> destinationLocationsAndProbabilities;
                    for (auto const& destination : edge.getDestinations()) {
                        OrderedAssignments oa(destination.getOrderedAssignments().clone());
                        oa.substitute(substitutionMap);

                        int64_t value = currentValue;
                        for (auto const& assignment : oa) {
                            if (assignment.getVariable() == *variable) {
                                value = assignment.getAssignedExpression().evaluateAsInt();
                                oa.remove(assignment);
                                break;
                            }
                        }

                        if (!useTransientVariables)
                            oa.add(Assignment(*uncastVar, original.getExpressionManager().integer(value)));

                        TemplateEdgeDestination ted(oa);
                        templateEdge->addDestination(ted);
                        destinationLocationsAndProbabilities.emplace_back(locationVariableValueMap[destination.getLocationIndex()][value], substituteJaniExpression(destination.getProbability(), substitutionMap));
                    }
                    newAutomaton.addEdge(storm::jani::Edge(newSourceIndex, edge.getActionIndex(), edge.hasRate() ? boost::optional<storm::expressions::Expression>(substituteJaniExpression(edge.getRate(), substitutionMap)) : boost::none, templateEdge, destinationLocationsAndProbabilities));

                }
            }
            return newAutomaton;
        }
    }
}