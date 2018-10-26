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
            STORM_LOG_THROW(original.getAutomaton(automatonName).hasVariable(variableName), storm::exceptions::IllegalArgumentException, "Automaton " << automatonName << " has no variable with name " << variableName << ". ");
            newModel = original;
            newModel.replaceAutomaton(newModel.getAutomatonIndex(automatonName), transformAutomaton(original.getAutomaton(automatonName), variableName));
        }


        Model const& JaniLocationExpander::getResult() const {
            return newModel;
        }

        Automaton JaniLocationExpander::transformAutomaton(Automaton const& automaton, std::string const& variableName) {

            Automaton newAutomaton(automaton.getName(), automaton.getLocationExpressionVariable());
            int64_t initialVariableValue;
            int64_t variableLowerBound;
            int64_t variableUpperBound;
            storm::expressions::Variable eliminatedExpressionVariable;
            const storm::jani::Variable* variable;

            for (auto const& var : automaton.getVariables()) {
                if (var.getName() == variableName) {
                    // This variable will be eliminated in the new automaton.
                    STORM_LOG_THROW(var.hasInitExpression(), storm::exceptions::IllegalArgumentException, "Variable to be eliminated has to have an initexpression.");
                    STORM_LOG_THROW(var.isBoundedIntegerVariable(), storm::exceptions::IllegalArgumentException, "Variable to be eliminated has to be an bounded integer variable.");
                    STORM_LOG_THROW(!var.isTransient(), storm::exceptions::IllegalArgumentException, "Cannot eliminate transient variable");

                    variableUpperBound = var.asBoundedIntegerVariable().getUpperBound().evaluateAsInt();
                    variableLowerBound  = var.asBoundedIntegerVariable().getLowerBound().evaluateAsInt();
                    initialVariableValue = var.getInitExpression().evaluateAsInt();
                    variable = &var;
                    eliminatedExpressionVariable = var.getExpressionVariable();

                } else {
                    // Other variables are just copied.
                    newAutomaton.addVariable(var);
                }
            }

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
                    uint64_t newLocationIndex = newAutomaton.addLocation(Location(newLocationName, newAssignments));

                    locationVariableValueMap[origIndex][i] = newLocationIndex;
                    locationNames[loc.getName()].push_back(newLocationName);
                }
            }



            for (auto const& edge : automaton.getEdges()) {
                for (auto const& newValueAndLocation : locationVariableValueMap[edge.getSourceLocationIndex()]) {
                    substitutionMap[eliminatedExpressionVariable] = original.getExpressionManager().integer(newValueAndLocation.first);

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
                        int64_t value;
                        for (auto const& assignment : oa) {
                            if (assignment.getVariable() == *variable) {
                                value = assignment.getAssignedExpression().evaluateAsInt();
                                oa.remove(assignment);
                                break;
                            }
                        }
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