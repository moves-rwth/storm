#include "UnfoldDependencyGraph.h"
#include "storm/exceptions/InvalidOperationException.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/strong_components.hpp>
#include "storm/storage/expressions/ExpressionManager.h"

using namespace boost;

namespace storm {
    namespace jani {
        namespace elimination_actions {
            UnfoldDependencyGraph::VariableGroup::VariableGroup() : domainSize(1), unfolded(false),
                                                                    allVariablesUnfoldable(true) {

            }

            void UnfoldDependencyGraph::VariableGroup::addVariable(UnfoldDependencyGraph::VariableInfo variable) {
                variables.push_back(variable);
                this->domainSize *= variable.domainSize;
                if (!variable.isConstBoundedInteger) {
                    allVariablesUnfoldable = false;
                }
            }

            UnfoldDependencyGraph::UnfoldDependencyGraph(Model &model) {
                buildGroups(model);
            }

            void UnfoldDependencyGraph::markUnfolded(uint32_t groupIndex) {
                variableGroups[groupIndex].unfolded = true;
            }

            uint32_t UnfoldDependencyGraph::findGroupIndex(std::string expressionVariableName) {
                for (uint32_t i = 0; i < variableGroups.size(); i++)
                    for (auto variable : variableGroups[i].variables)
                        if (variable.expressionVariableName == expressionVariableName)
                            return i;

                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException,
                                "The UnfoldDependencyGraph does not contain the variable " + expressionVariableName);
            }

            std::vector<uint32_t> UnfoldDependencyGraph::getOrderedDependencies(uint32_t groupIndex, bool includeSelf) {
                // This method creates a topological sort of all the dependencies of groupIndex.

                std::vector<uint32_t> res;
                std::set<uint32_t> closedSet; // Contains the same vertices as res, but provides easier and faster lookup

                while (closedSet.count(groupIndex) == 0) {
                    uint32_t current = groupIndex;
                    bool isLeaf = false;
                    while (!isLeaf) {
                        isLeaf = true;
                        // Find dependency that is not yet listed in res:
                        for (auto dep : variableGroups[current].dependencies) {
                            if (variableGroups[dep].unfolded)
                                continue;
                            if (closedSet.count(dep) == 0) {
                                isLeaf = false;
                                current = dep;
                                break;
                            }
                        }
                    }
                    closedSet.insert(current);
                    if (includeSelf || current != groupIndex)
                        res.push_back(current);
                }

                return res;
            }

            uint32_t UnfoldDependencyGraph::getTotalBlowup(std::vector<uint32_t> groups) {
                uint32_t res = 1;
                for (uint32_t group : groups){
                    res *= variableGroups[group].domainSize;
                }
                return res;
            }

            std::set<uint32_t> UnfoldDependencyGraph::getGroupsWithNoDependencies() {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "getGroupsWithNoDependencies() is not yet implemented");
                return std::set<uint32_t>();
            }

            void UnfoldDependencyGraph::buildGroups(Model &model) {
                std::vector<std::pair<std::string, VariableSet *>> variableSets;
                // Use "" for the global set, as automata must not have an empty name according to the Jani spec
                variableSets.push_back(std::pair<std::string, VariableSet *>("", &model.getGlobalVariables()));
                for (auto &automaton : model.getAutomata())
                    variableSets.push_back(
                            std::pair<std::string, VariableSet *>(automaton.getName(), &automaton.getVariables()));

                std::vector<VariableInfo> variables;

                for (auto variableSet : variableSets) {
                    for (auto &var : *variableSet.second) {
                        bool isConstBounded = false;
                        if (var.isBoundedIntegerVariable()) {
                            auto biVar = var.asBoundedIntegerVariable();
                            if (biVar.hasLowerBound()
                                && biVar.hasUpperBound()
                                && !biVar.getLowerBound().containsVariables()
                                && !biVar.getUpperBound().containsVariables()) {
                                int lowerBound = biVar.getLowerBound().evaluateAsInt();
                                int upperBound = biVar.getUpperBound().evaluateAsInt();

                                variables.push_back(VariableInfo(var.getExpressionVariable().getName(), var.getName(),
                                                                 variableSet.first == "", variableSet.first,
                                                                 true, upperBound - lowerBound));

                                isConstBounded = true;
                            }
                        }
                        if (!isConstBounded) {
                            // Still add the variable (so that dependencies are computed correctly). The
                            // "isConstBoundedInteger" parameter ensures that this variable is not unfolded later
                            variables.push_back(VariableInfo(var.getExpressionVariable().getName(), var.getName(),
                                                             variableSet.first == "", variableSet.first,
                                                             false, 0));
                        }
                    }
                }

                adjacency_list<vecS, vecS, directedS> graph(variables.size());

                for (auto &automaton : model.getAutomata()) {
                    for (auto &janiEdge : automaton.getEdges()) {
                        for (auto &dest : janiEdge.getDestinations()) {
                            for (auto &asg : dest.getOrderedAssignments().getAllAssignments()) {
                                std::string leftName = asg.getExpressionVariable().getName();
                                int lIndex = std::distance(variables.begin(),
                                                           std::find_if(variables.begin(), variables.end(),
                                                                        [leftName](VariableInfo &v) {
                                                                            return v.expressionVariableName == leftName;
                                                                        }));

                                for (auto &rightVar : asg.getAssignedExpression().getVariables()) {
                                    std::string rightName = rightVar.getName();
                                    int rIndex = std::distance(variables.begin(),
                                                               std::find_if(variables.begin(), variables.end(),
                                                                            [rightName](VariableInfo &v) {
                                                                                return v.expressionVariableName ==
                                                                                       rightName;
                                                                            }));
                                    if (rIndex == lIndex)
                                        continue;
                                    if (rIndex !=
                                        variables.size()) {// TODO If the condition is false, we're probably dealing with a constant. This should be handled properly.
                                        if (!edge(lIndex, rIndex, graph).second) {
                                            add_edge(lIndex, rIndex, graph);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                std::vector<int> sccs(variables.size());

                int num = strong_components
                        (graph, make_iterator_property_map(sccs.begin(), get(vertex_index, graph), sccs[0]));


                std::cout << "Total number of components: " << num << std::endl;
                for (int i = 0; i < num; i++) {
                    variableGroups.push_back(VariableGroup());
                }

                std::vector<int>::iterator i;
                for (i = sccs.begin(); i != sccs.end(); ++i) {
                    int index = i - sccs.begin();
                    int component = *i;
                    variableGroups[component].addVariable(variables[index]);

                    std::cout << "Variable " << variables[index].expressionVariableName << " (Index: " << index
                              << ") is in component " << component << std::endl;
                }

                for (int i = 0; i < graph.m_vertices.size(); i++) {
                    for (const auto& outEdge : graph.m_vertices[i].m_out_edges) {
                        unsigned long target = findGroupIndex(variables[outEdge.m_target].expressionVariableName);
                        if (variableGroups[sccs[i]].dependencies.count(target) == 0) {
                            variableGroups[sccs[i]].dependencies.insert(target);
                        }
                    }
                }
                printGroups();
            }

            UnfoldDependencyGraph::VariableInfo::VariableInfo(std::string expressionVariableName,
                                                              std::string janiVariableName, bool isGlobal,
                                                              std::string automatonName, bool isConstBoundedInteger,
                                                              int domainSize) :
                    expressionVariableName(expressionVariableName),
                    janiVariableName(janiVariableName),
                    isGlobal(isGlobal),
                    automatonName(automatonName),
                    isConstBoundedInteger(isConstBoundedInteger),
                    domainSize(domainSize) {

            }

            void UnfoldDependencyGraph::printGroups() {
                int groupCounter = 0;
                for (auto group : variableGroups) {
                    std::cout << std::endl << "Variable Group " << groupCounter << std::endl;
                    std::cout << "\tDomain size: " << group.domainSize << std::endl;
                    if (group.allVariablesUnfoldable)
                        std::cout << "\tCan be unfolded" << std::endl;
                    else
                        std::cout << "\tCan not be unfolded" << std::endl;
                    std::cout << "\tVariables:" << std::endl;
                    for (auto var : group.variables) {
                        std::cout << "\t\t" << var.expressionVariableName;
                        if (var.isConstBoundedInteger)
                            std::cout << " (const-bounded integer with domain size " << var.domainSize << ")";
                        else
                            std::cout << " (not a const-bounded integer)";
                        std::cout << std::endl;
                    }
                    if (group.dependencies.size() == 0)
                        std::cout << "\tNo Dependencies" << std::endl;
                    else
                        std::cout << "\tDependencies:" << std::endl;
                    for (auto dep : group.dependencies) {
                        std::cout << "\t\t" << dep << std::endl;
                    }

                    groupCounter++;
                }
            }

            bool UnfoldDependencyGraph::areDependenciesUnfoldable(uint32_t groupIndex) {
                auto dependencies = getOrderedDependencies(groupIndex, false);
                return std::all_of(
                        dependencies.begin(), dependencies.end(),
                        [this](uint32_t dep) { return this->variableGroups[dep].allVariablesUnfoldable; });

            }
        }
    }
}