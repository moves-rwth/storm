#include "UnfoldAction.h"
#include <boost/format.hpp>
#include "storm/storage/jani/JaniLocationExpander.h"

namespace storm {
    namespace jani {
        namespace elimination_actions {
            UnfoldAction::UnfoldAction(const std::string &automatonName, const std::string &variableName) {
                this->automatonName = automatonName;
                this->variableName = variableName;
            }

            std::string UnfoldAction::getDescription() {
                return (boost::format("UnfoldAction (Automaton %s, Variable %s)") % automatonName % variableName).str();
            }

            void UnfoldAction::doAction(JaniLocalEliminator::Session &session) {
                JaniLocationExpander expander = JaniLocationExpander(session.getModel());
                expander.transform(automatonName, variableName);
                session.setModel(expander.getResult());
            }
        }
    }
}