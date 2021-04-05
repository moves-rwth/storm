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
            newModel.replaceAutomaton(newModel.getAutomatonIndex(automatonName), transformAutomaton(original.getAutomaton(automatonName), variableName, true));
        }


        Model const& JaniLocationExpander::getResult() const {
            return newModel;
        }

        Automaton JaniLocationExpander::transformAutomaton(Automaton const& automaton, std::string const& variableName, bool useTransientVariables) {

            Automaton newAutomaton(automaton.getName(), automaton.getLocationExpressionVariable());
            for (auto const &localVariable : automaton.getVariables())
                newAutomaton.addVariable(
                        localVariable); // The expanded variable is also added, but will be set to transient later

            bool isGlobalVariable = !automaton.hasVariable(variableName);
            VariableSet &containingSet = isGlobalVariable ? newModel.getGlobalVariables() : newAutomaton.getVariables();

            auto &var = containingSet.getVariable(variableName);
            bool isBoundedInteger = var.isBoundedIntegerVariable();
            bool isBool = var.isBooleanVariable();

            STORM_LOG_THROW(isBoundedInteger || isBool, storm::exceptions::IllegalArgumentException,
                            "Variable to be eliminated has to be an bounded integer or boolean variable.");
            STORM_LOG_THROW(var.hasInitExpression(), storm::exceptions::IllegalArgumentException,
                            "Variable to be eliminated has to have an initexpression.");
            STORM_LOG_THROW(!var.isTransient(), storm::exceptions::IllegalArgumentException,
                            "Cannot eliminate transient variable");

            storm::expressions::Variable eliminatedExpressionVariable = var.getExpressionVariable();

            // As we are using transitive variables, we're no longer really eliminating variables and the initialStatesRestriction should hopefully still work
            // STORM_LOG_THROW(!automaton.getInitialStatesRestriction().containsVariable({eliminatedExpressionVariable}), storm::exceptions::NotSupportedException, "Elimination of variable that occurs in the initial state restriction is not allowed");
            newAutomaton.setInitialStatesRestriction(automaton.getInitialStatesRestriction());

            std::vector<storm::expressions::Expression> variableDomain;
            uint32_t initialValueIndex; // The index in variableDomain of the initial value

            if (isBoundedInteger) {
                auto biVariable = containingSet.getVariable(variableName).asBoundedIntegerVariable();

                int64_t variableUpperBound = biVariable.getUpperBound().evaluateAsInt();
                int64_t variableLowerBound = biVariable.getLowerBound().evaluateAsInt();
                int64_t initialVariableValue = biVariable.getInitExpression().evaluateAsInt();
                for (uint64_t i = variableLowerBound; i <= variableUpperBound; i++) {
                    variableDomain.push_back(original.getExpressionManager().integer(i));
                }
                initialValueIndex = initialVariableValue - variableLowerBound;

                containingSet.eraseVariable(biVariable.getExpressionVariable());
                biVariable.setTransient(useTransientVariables);
                containingSet.addVariable(biVariable);

            } else if (isBool) {
                auto boolVariable = containingSet.getVariable(variableName).asBooleanVariable();
                variableDomain.push_back(original.getExpressionManager().boolean(false));
                variableDomain.push_back(original.getExpressionManager().boolean(true));
                bool initialValue = boolVariable.getInitExpression().evaluateAsBool();
                if (initialValue) {
                    initialValueIndex = 1;
                } else {
                    initialValueIndex = 0;
                }

                containingSet.eraseVariable(boolVariable.getExpressionVariable());
                boolVariable.setTransient(useTransientVariables);
                containingSet.addVariable(boolVariable);
            }

            const Variable &newVariablePointer = containingSet.getVariable(variableName);

            // This map will only ever contain a single entry: the variable to be eliminated. It is used during substitutions
            std::map<storm::expressions::Variable, storm::expressions::Expression> substitutionMap;

            // Maps each old location index to a map that maps every variable value to the index of the (new) location that corresponds to the old location and variable value
            std::map<uint64_t, std::map<int64_t, uint64_t>> locationVariableValueMap; //TODO: Switch inner map to vector?

            for (auto const &loc : automaton.getLocations()) {
                uint64_t origIndex = automaton.getLocationIndex(loc.getName());

                for (int64_t i = 0; i < variableDomain.size(); i++) {
                    std::string newLocationName =
                            loc.getName() + "_" + variableName + "_" + variableDomain[i].toString();
                    substitutionMap[eliminatedExpressionVariable] = variableDomain[i];

                    OrderedAssignments newAssignments = loc.getAssignments().clone();
                    newAssignments.substitute(substitutionMap);

                    Location newLoc(newLocationName, newAssignments);

                    if (useTransientVariables)
                        newLoc.addTransientAssignment(Assignment(newVariablePointer, variableDomain[i], 0));


                    uint64_t newLocationIndex = newAutomaton.addLocation(newLoc);
                    locationVariableValueMap[origIndex][i] = newLocationIndex;
                }
            }

            for (uint64_t const &initialLoc : automaton.getInitialLocationIndices()) {
                newAutomaton.addInitialLocation(locationVariableValueMap[initialLoc][initialValueIndex]);
            }


            for (auto const &edge : automaton.getEdges()) {
                for (auto const &newValueAndLocation : locationVariableValueMap[edge.getSourceLocationIndex()]) {
                    int64_t currentValueIndex = newValueAndLocation.first;

                    substitutionMap[eliminatedExpressionVariable] = variableDomain[currentValueIndex];

                    uint64_t newSourceIndex = newValueAndLocation.second;
                    storm::expressions::Expression newGuard = substituteJaniExpression(edge.getGuard(),
                                                                                       substitutionMap).simplify();
                    if (!newGuard.containsVariables() && !newGuard.evaluateAsBool()) {
                        // continue;
                    }
                    std::shared_ptr<storm::jani::TemplateEdge> templateEdge = std::make_shared<storm::jani::TemplateEdge>(
                            newGuard);

                    STORM_LOG_THROW(edge.getAssignments().empty(), storm::exceptions::NotImplementedException,
                                    "Support for edge-assignments is not implemented");

                    std::vector<std::pair<uint64_t, storm::expressions::Expression>> destinationLocationsAndProbabilities;
                    for (auto const &destination : edge.getDestinations()) {
                        OrderedAssignments oa(destination.getOrderedAssignments().clone());
                        oa.substitute(substitutionMap);

                        int64_t newValueIndex = currentValueIndex;
                        for (auto const &assignment : oa) {
                            if (assignment.getVariable().getExpressionVariable().getIndex() ==
                                var.getExpressionVariable().getIndex()) {
                                if (isBoundedInteger) {
                                    // TODO: This index computation seems unnecessarily cumbersome
                                    newValueIndex = assignment.getAssignedExpression().evaluateAsInt() -
                                                    variableDomain[0].evaluateAsInt();
                                } else if (isBool) {
                                    if (assignment.getAssignedExpression().evaluateAsBool()) {
                                        newValueIndex = 1;
                                    } else {
                                        newValueIndex = 0;
                                    }
                                }
                                oa.remove(assignment);
                                break;
                            }
                        }

                        if (!useTransientVariables)
                            STORM_LOG_THROW(true, storm::exceptions::NotImplementedException,
                                            "Unfolding without transient variables is not implemented");
                        //oa.add(Assignment(*uncastVar, original.getExpressionManager().integer(value)));

                        TemplateEdgeDestination ted(oa);
                        templateEdge->addDestination(ted);
                        destinationLocationsAndProbabilities.emplace_back(
                                locationVariableValueMap[destination.getLocationIndex()][newValueIndex],
                                substituteJaniExpression(destination.getProbability(), substitutionMap));
                    }

                    newAutomaton.addEdge(storm::jani::Edge(newSourceIndex, edge.getActionIndex(), edge.hasRate()
                                                                                                  ? boost::optional<storm::expressions::Expression>(
                                    substituteJaniExpression(edge.getRate(), substitutionMap)) : boost::none,
                                                           templateEdge, destinationLocationsAndProbabilities));
                }
            }
            return newAutomaton;
        }
    }
}