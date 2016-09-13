#include "JSONExporter.h"

#include <iostream>
#include <fstream>
#include <vector>

#include "src/utility/macros.h"
#include "src/exceptions/FileIoException.h"



namespace storm {
    namespace jani {
        void JsonExporter::toFile(storm::jani::Model const& janiModel, std::string const& filepath) {
            std::ofstream ofs;
            ofs.open (filepath, std::ofstream::out );
            if(ofs.is_open()) {
                toStream(janiModel, ofs);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Cannot open " << filepath);
            }
        }
        
        void JsonExporter::toStream(storm::jani::Model const& janiModel, std::ostream& os) {
            JsonExporter exporter;
            exporter.convertModel(janiModel);
            os << exporter.finalize().dump(4) << std::endl;
        }
        
        modernjson::json buildActionArray(std::vector<storm::jani::Action> const& actions) {
            std::vector<modernjson::json> actionReprs;
            for(auto const& act : actions) {
                modernjson::json actEntry;
                actEntry["name"] = act.getName();
                actionReprs.push_back(actEntry);
            }
            
            return modernjson::json(actionReprs);
            
        }
        
        
        modernjson::json buildExpression(storm::expressions::Expression const& exp) {
            return modernjson::json();
        }
        
        
        modernjson::json buildConstantsArray(std::vector<storm::jani::Constant> const& constants) {
            std::vector<modernjson::json> constantDeclarations;
            for(auto const& constant : constants) {
                modernjson::json constantEntry;
                constantEntry["name"] = constant.getName();
                modernjson::json typeDesc;
                if(constant.isBooleanConstant()) {
                    typeDesc = "bool";
                } else if(constant.isRealConstant()) {
                    typeDesc = "real";
                } else {
                    assert(constant.isIntegerConstant());
                    typeDesc = "int";
                }
                constantEntry["type"] = typeDesc;
                if(constant.isDefined()) {
                    constantEntry["value"] = buildExpression(constant.getExpression());
                }
                constantDeclarations.push_back(constantEntry);
            }
            return modernjson::json(constantDeclarations);
        }
        
        modernjson::json buildVariablesArray(storm::jani::VariableSet const& varSet) {
            std::vector<modernjson::json> variableDeclarations;
            for(auto const& variable : varSet) {
                modernjson::json varEntry;
                varEntry["name"] = variable.getName();
                varEntry["transient"] = variable.isTransient();
                modernjson::json typeDesc;
                if(variable.isBooleanVariable()) {
                    typeDesc = "bool";
                } else if(variable.isRealVariable()) {
                    typeDesc = "real";
                } else if(variable.isUnboundedIntegerVariable()) {
                    typeDesc = "int";
                } else {
                    assert(variable.isBoundedIntegerVariable());
                    typeDesc["kind"] = "bounded";
                    typeDesc["base"] = "int";
                    typeDesc["lower-bound"] = buildExpression(variable.asBoundedIntegerVariable().getLowerBound());
                    typeDesc["upper-bound"] = buildExpression(variable.asBoundedIntegerVariable().getUpperBound());
                }
                
                varEntry["type"] = typeDesc;
                if(variable.hasInitExpression()) {
                    varEntry["initial-value"] = buildExpression(variable.getInitExpression());
                }
                variableDeclarations.push_back(varEntry);
            }
            return modernjson::json(variableDeclarations);
            
        }
        
        modernjson::json buildAssignmentArray(storm::jani::OrderedAssignments const& orderedAssignments) {
            std::vector<modernjson::json> assignmentDeclarations;
            bool addIndex = orderedAssignments.hasMultipleLevels();
            for(auto const& assignment : orderedAssignments) {
                modernjson::json assignmentEntry;
                assignmentEntry["ref"] = assignment.getVariable().getName();
                assignmentEntry["value"] = buildExpression(assignment.getAssignedExpression());
                if(addIndex) {
                    assignmentEntry["index"] = assignment.getLevel();
                }
                assignmentDeclarations.push_back(assignmentEntry);
            }
            return modernjson::json(assignmentDeclarations);
        }
        
        modernjson::json buildLocationsArray(std::vector<storm::jani::Location> const& locations) {
            std::vector<modernjson::json> locationDeclarations;
            for(auto const& location : locations) {
                modernjson::json locEntry;
                locEntry["name"] = location.getName();
                // TODO support invariants?
                locEntry["transient-values"] = buildAssignmentArray(location.getAssignments());
                locationDeclarations.push_back(locEntry);
            }
            return modernjson::json(locationDeclarations);
        }
        
        modernjson::json buildInitialLocations(storm::jani::Automaton const& automaton) {
            std::vector<std::string> names;
            for(auto const& initLocIndex : automaton.getInitialLocationIndices()) {
                names.push_back(automaton.getLocation(initLocIndex).getName());
            }
            return modernjson::json(names);
        }
        
        modernjson::json buildDestinations(std::vector<EdgeDestination> const& destinations, std::map<uint64_t, std::string> const& locationNames) {
            std::vector<modernjson::json> destDeclarations;
            for(auto const& destination : destinations) {
                modernjson::json destEntry;
                destEntry["location"] = locationNames.at(destination.getLocationIndex());
                destEntry["probability"] = buildExpression(destination.getProbability());
                // TODO
                //destEntry["assignments"] = buildAssignmentArray(destination.getAssignments());
                destDeclarations.push_back(destEntry);
            }
            return modernjson::json(destDeclarations);
        }
        
        modernjson::json buildEdges(std::vector<Edge> const& edges , std::map<uint64_t, std::string> const& actionNames, std::map<uint64_t, std::string> const& locationNames) {
            std::vector<modernjson::json> edgeDeclarations;
            for(auto const& edge : edges) {
                modernjson::json edgeEntry;
                edgeEntry["location"] = locationNames.at(edge.getSourceLocationIndex());
                // TODO silent action
                //if(edge.nonSilentAction()) {
                edgeEntry["action"] = actionNames.at(edge.getActionIndex());
                //}
                if(edge.hasRate()) {
                    edgeEntry["rate"]["exp"] = buildExpression(edge.getRate());
                }
                edgeEntry["guard"]["exp"] = buildExpression(edge.getGuard());
                edgeEntry["destinations"] = buildDestinations(edge.getDestinations(), locationNames);
                
                edgeDeclarations.push_back(edgeEntry);
            }
            return modernjson::json(edgeDeclarations);
        }
        
        modernjson::json buildAutomataArray(std::vector<storm::jani::Automaton> const& automata, std::map<uint64_t, std::string> const& actionNames) {
            std::vector<modernjson::json> automataDeclarations;
            for(auto const& automaton : automata) {
                modernjson::json autoEntry;
                autoEntry["name"] = automaton.getName();
                autoEntry["variables"] = buildVariablesArray(automaton.getVariables());
                autoEntry["restrict-initial"]["exp"] = buildExpression(automaton.getInitialStatesRestriction());
                autoEntry["locations"] = buildLocationsArray(automaton.getLocations());
                autoEntry["initial-locations"] = buildInitialLocations(automaton);
                autoEntry["edges"] = buildEdges(automaton.getEdges(), actionNames, automaton.buildIdToLocationNameMap());
                automataDeclarations.push_back(autoEntry);
            }
            return modernjson::json(automataDeclarations);
        }
        
        
        void JsonExporter::convertModel(storm::jani::Model const& janiModel) {
            jsonStruct["jani-version"] = janiModel.getJaniVersion();
            jsonStruct["name"] = janiModel.getName();
            jsonStruct["type"] = to_string(janiModel.getModelType());
            jsonStruct["actions"] = buildActionArray(janiModel.getActions());
            jsonStruct["constants"] = buildConstantsArray(janiModel.getConstants());
            jsonStruct["variables"] = buildVariablesArray(janiModel.getGlobalVariables());
            jsonStruct["restrict-initial"]["exp"] = buildExpression(janiModel.getInitialStatesRestriction());
            jsonStruct["automata"] = buildAutomataArray(janiModel.getAutomata(), janiModel.buildActionToNameMap());
            //jsonStruct["system"] = buildComposition();
            
        }
        
        
        
    }
}