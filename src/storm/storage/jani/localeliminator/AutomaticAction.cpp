#include "AutomaticAction.h"
#include "UnfoldAction.h"
#include "EliminateAutomaticallyAction.h"

namespace storm {
    namespace jani {
        namespace elimination_actions {
            AutomaticAction::AutomaticAction() {

            }

            std::string AutomaticAction::getDescription() {
                return "AutomaticAction";
            }

            void AutomaticAction::doAction(JaniLocalEliminator::Session &session) {
                if (session.getModel().getAutomata().size() > 1){
                    session.setModel(session.getModel().flattenComposition());
                }
                std::string autName = session.getModel().getAutomata()[0].getName();

                auto variables = session.getProperty().getUsedVariablesAndConstants();
                std::vector<std::string> variablesToUnfold;
                for (const expressions::Variable &variable : variables){
                    bool isGlobalVar = session.getModel().getGlobalVariables().hasVariable(variable.getName());
                    bool isLocalVar = session.getModel().getAutomaton(autName).getVariables().hasVariable(variable.getName());
                    if (isGlobalVar || isLocalVar){
                        variablesToUnfold.push_back(variable.getName());
                    }
                }

                for (std::string &varName : variablesToUnfold){
                    UnfoldAction unfoldAction(autName, varName);
                    unfoldAction.doAction(session);
                }
                EliminateAutomaticallyAction eliminateAction(autName, EliminateAutomaticallyAction::EliminationOrder::NewTransitionCount);
                eliminateAction.doAction(session);
            }
        }
    }
}