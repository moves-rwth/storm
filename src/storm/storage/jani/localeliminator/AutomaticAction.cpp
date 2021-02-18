#include "AutomaticAction.h"
#include "UnfoldAction.h"
#include "EliminateAutomaticallyAction.h"
#include <boost/graph/strong_components.hpp>
#include "storm/storage/expressions/ExpressionManager.h"
#include "UnfoldDependencyGraph.h"

using namespace boost;

namespace storm {
    namespace jani {
        namespace elimination_actions {
            AutomaticAction::AutomaticAction() {

            }

            std::string AutomaticAction::getDescription() {
                return "AutomaticAction";
            }

            void AutomaticAction::doAction(JaniLocalEliminator::Session &session) {

                UnfoldDependencyGraph graph(session.getModel());

                buildVariableGraph(session);
                if (session.getModel().getAutomata().size() > 1) {
                    session.setModel(session.getModel().flattenComposition());
                }
                std::string autName = session.getModel().getAutomata()[0].getName();

                auto variables = session.getProperty().getUsedVariablesAndConstants();
                std::vector<std::string> variablesToUnfold;
                for (const expressions::Variable &variable : variables) {
                    bool isGlobalVar = session.getModel().getGlobalVariables().hasVariable(variable.getName());
                    bool isLocalVar = session.getModel().getAutomaton(autName).getVariables().hasVariable(
                            variable.getName());
                    if (isGlobalVar || isLocalVar) {
                        variablesToUnfold.push_back(variable.getName());
                    }
                }

                for (std::string &varName : variablesToUnfold) {
                    UnfoldAction unfoldAction(autName, varName);
                    unfoldAction.doAction(session);
                }
                EliminateAutomaticallyAction eliminateAction(autName,
                                                             EliminateAutomaticallyAction::EliminationOrder::NewTransitionCount);
                eliminateAction.doAction(session);
            }

            adjacency_list<vecS, vecS, directedS>
            AutomaticAction::buildVariableGraph(JaniLocalEliminator::Session &session) {
                std::vector<std::string> varNames = collectExpressionVariableNames(session);

                adjacency_list<vecS, vecS, directedS> graph(varNames.size());

                for (auto &automaton : session.getModel().getAutomata()){
                    for (auto &edge : automaton.getEdges()){
                        for (auto &dest : edge.getDestinations()){
                            for (auto &asg : dest.getOrderedAssignments().getAllAssignments()){
                                std::string leftName = asg.getExpressionVariable().getName();
                                int lIndex = std::distance(varNames.begin(), std::find(varNames.begin(), varNames.end(), leftName));

                                for (auto &rightVar : asg.getAssignedExpression().getVariables()){
                                    std::string rightName = rightVar.getName();
                                    int rIndex = std::distance(varNames.begin(), std::find(varNames.begin(), varNames.end(), rightName));
                                    add_edge(lIndex, rIndex, graph);
                                }
                            }
                        }
                    }
                }

                std::vector<int> c(varNames.size());

                int num = strong_components
                        (graph, make_iterator_property_map(c.begin(), get(vertex_index, graph), c[0]));

                auto l = get(vertex_index, graph);

                std::cout << "Total number of components: " << num << std::endl;
                std::vector<int>::iterator i;
                for (i = c.begin(); i != c.end(); ++i)
                    std::cout << "Variable " << varNames[i - c.begin()] << " (Index: " << i - c.begin()
                              << ") is in component " << *i << std::endl;

                return graph;
            }

            std::vector<std::string> AutomaticAction::collectExpressionVariableNames(JaniLocalEliminator::Session &session) {
                std::vector<std::string> names;

                for (auto &variable : session.getModel().getExpressionManager().getVariables()){
                    names.push_back(variable.getName());
                }

                return names;
            }
        }
    }
}